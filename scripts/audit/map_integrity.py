#!/usr/bin/env python3
"""Derive map/tile compatibility from production metadata; never infer playability.

Every registered map and layout is inventoried. Emerald's compiled region/layout
filters match mapjson; dormant data is reported separately. Packed placement cells
retain the engine's collision/elevation bits. Dynamic states not resolved by this
checker are explicitly unknown and do not receive a visual-fidelity certificate.
"""

from __future__ import annotations
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
from functools import lru_cache
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


class InvalidData(ValueError):
    pass


def require(condition, message):
    if not condition:
        raise InvalidData(message)


def expression(text, constants):
    """Strict wrapper around the shared conservative source-expression reader."""
    from map_dynamic_inventory import numeric

    if type(text) is int:
        return text
    text = re.sub(r"\bsizeof\s*\(\s*u32\s*\)", "4", text.strip())
    text = re.sub(r"(?<=\d)[uUlL]+\b", "", text)
    value = numeric(text, constants)
    require(value is not None, f"unresolved integer expression: {text}")
    return value


def read_constants(root):
    from map_dynamic_inventory import read_constants as source_constants

    return source_constants(root)


def words(raw, size, label):
    require(len(raw) % size == 0, f"{label}: misaligned binary size {len(raw)}")
    return list(
        struct.unpack("<" + ("H" if size == 2 else "I") * (len(raw) // size), raw)
    )


@lru_cache(maxsize=256)
def converted_graphics(raw, options, converter, converter_sha256):
    """Use the build converter, including -Wnum_tiles fallback semantics.

    Cache keys bind immutable source bytes, options and converter identity.
    The source bytes are staged so mutation tests exercise the same consumer.
    """
    with tempfile.TemporaryDirectory(prefix="ec-map-graphics-") as temporary:
        image = Path(temporary) / "tiles.png"
        output = Path(temporary) / "tiles.4bpp"
        image.write_bytes(raw)
        completed = subprocess.run(
            [converter, str(image), str(output), *shlex.split(options)],
            capture_output=True,
            text=True,
        )
        require(
            completed.returncode == 0,
            f"canonical graphics conversion failed: {completed.stderr}",
        )
        payload = output.read_bytes()
        require(len(payload) % 32 == 0, "converter produced a partial tile")
        return (
            len(payload) // 32,
            hashlib.sha256(payload).hexdigest(),
            completed.stderr.strip(),
        )


def metadata(root, constants):
    headers = (root / "src/data/tilesets/headers.h").read_text()
    graphics = (
        (root / "src/data/tilesets/graphics.h").read_text()
        + "\n"
        + (root / "src/graphics.c").read_text()
    )
    metatiles = (root / "src/data/tilesets/metatiles.h").read_text()
    tilesets = {
        name: dict(re.findall("\\.(\\w+)\\s*=\\s*(\\w+)", body))
        for name, body in re.findall(
            "const struct Tileset (\\w+)\\s*=\\s*\\{(.*?)\\};", headers, re.S
        )
    }
    gfx = {}
    converter = root / "tools/gbagfx/gbagfx"
    require(
        converter.is_file(), "build tools/gbagfx/gbagfx before auditing compiled art"
    )
    converter_sha256 = hashlib.sha256(converter.read_bytes()).hexdigest()
    for name, bound, path, extension, options in re.findall(
        'const u32 (gTilesetTiles_\\w+)\\[([^\\]]*)\\]\\s*=\\s*INCGFX_U32\\("([^"]+)",\\s*"([^"]+)"(?:,\\s*"([^"]+)")?\\);',
        graphics,
    ):
        raw = (root / path).read_bytes()
        count, converted_sha256, notes = converted_graphics(
            raw, options, str(converter), converter_sha256
        )
        array_tiles = expression(bound, constants) * 4 // 32 if bound else count
        require(array_tiles >= count, f"{name}: array truncates converted artwork")
        gfx[name] = {
            "source": path,
            "source_sha256": hashlib.sha256(raw).hexdigest(),
            "converter_sha256": converter_sha256,
            "converted_sha256": converted_sha256,
            "converter_notes": notes,
            "payload_tiles": count,
            "stored_tiles": array_tiles,
            "compressed": extension != ".4bpp",
        }
    palettes = {
        name: re.findall('INCGFX_U16\\("([^"]+)"', body)
        for name, body in re.findall(
            "const u16(?: ALIGNED\\(4\\))? (gTilesetPalettes_\\w+)\\[\\]\\[16\\]\\s*=\\s*\\{(.*?)\\};",
            graphics,
            re.S,
        )
    }
    binaries = {
        name: path
        for name, path in re.findall(
            'const u(?:16|32) (gMetatile\\w+)\\[\\]\\s*=\\s*INCBIN_U(?:16|32)\\("([^"]+)"',
            metatiles,
        )
    }
    for name, data in tilesets.items():
        for field in [
            "tiles",
            "palettes",
            "metatiles",
            "metatileAttributes",
            "isSecondary",
            "isCompressed",
            "callback",
        ]:
            require(field in data, f"{name}: missing {field}")
        require(data["tiles"] in gfx, f"{name}: graphics declaration unresolved")
        require(data["palettes"] in palettes, f"{name}: palette declaration unresolved")
        data["graphics"] = gfx[data["tiles"]]
        require(
            bool(expression(data["isCompressed"], constants))
            == data["graphics"]["compressed"],
            f"{name}: compression contract mismatch",
        )
        data["palette_sources"] = palettes[data["palettes"]]
        require(len(data["palette_sources"]) >= 13, f"{name}: missing palette slots")
        for path in data["palette_sources"]:
            require((root / path).is_file(), f"{name}: missing palette {path}")
            palette = (root / path).read_text().splitlines()
            require(
                palette[:3] == ["JASC-PAL", "0100", "16"] and len(palette) == 19,
                f"{name}: malformed palette {path}",
            )
            require(
                all(
                    (
                        len(row.split()) == 3
                        and all(
                            (v.isdigit() and 0 <= int(v) <= 255 for v in row.split())
                        )
                        for row in palette[3:]
                    )
                ),
                f"{name}: invalid RGB palette {path}",
            )
        for field in ["metatiles", "metatileAttributes"]:
            require(data[field] in binaries, f"{name}: unresolved {field} binary")
        data["metatile_source"] = binaries[data["metatiles"]]
        data["attribute_source"] = binaries[data["metatileAttributes"]]
        data["components"] = words(
            (root / data["metatile_source"]).read_bytes(), 2, data["metatile_source"]
        )
        require(len(data["components"]) % 8 == 0, f"{name}: incomplete metatile")
    return tilesets


def behavior_names(root, constants):
    source = re.sub(
        "//[^\\n]*|/\\*.*?\\*/",
        "",
        (root / "include/constants/metatile_behaviors.h").read_text(),
        flags=re.S,
    )
    body = source.split("enum", 1)[1].split("{", 1)[1].split("}", 1)[0]
    names = {}
    value = 0
    for entry in body.split(","):
        entry = entry.strip()
        if not entry:
            continue
        parts = entry.split("=", 1)
        name = parts[0].strip()
        if len(parts) == 2:
            value = expression(parts[1], constants)
        if name == "NUM_METATILE_BEHAVIORS":
            continue
        require(re.fullmatch("MB_\\w+", name), f"unparsed behavior enum: {entry}")
        names[value] = name
        constants[name] = value
        value += 1
    return names


def script_changes(root, constants, maps, layouts):
    from map_dynamic_inventory import derive_script_contexts

    diagnostics = {}
    contexts = derive_script_contexts(root, maps, diagnostics=diagnostics)
    result = []
    state_changes = []
    macros = (root / "asm/macros/event.inc").read_text()
    warp_commands = {
        name for name in re.findall(r"\.macro\s+(\w*warp\w*)\s+map:req", macros)
    }
    paths = {root / "data/event_scripts.s"}
    paths.update(
        path
        for path in (root / "data").rglob("*.inc")
        if "/scripts/" in str(path) or path.name == "scripts.inc"
    )
    for path in sorted(paths):
        label = None
        for line_number, line in enumerate(path.read_text().splitlines(), 1):
            text = line.split("@", 1)[0].strip()
            match = re.fullmatch("(\\w+)::?", text)
            if match:
                label = match[1]
            parts = text.split(None, 1)
            command = parts[0] if parts else ""
            tail = parts[1] if len(parts) > 1 else ""
            owners = contexts.get(label, [])
            if command in warp_commands or command == "setmaplayoutindex":
                state_changes.append(
                    {
                        "source": path.relative_to(root).as_posix(),
                        "line": line_number,
                        "label": label,
                        "command": command,
                        "target": tail.split(",", 1)[0].strip(),
                        "arguments": tail.strip(),
                        "possible_maps": owners,
                    }
                )
            if command not in ("setmetatile", "setmetatileinrange"):
                continue
            operands = [value.strip() for value in tail.split(",")]
            row = {
                "source": path.relative_to(root).as_posix(),
                "line": line_number,
                "label": label,
                "command": command,
                "operands": operands,
                "possible_maps": owners,
                "status": "unresolved",
                "reason": "no map-root context established",
            }
            try:
                if command == "setmetatile":
                    require(len(operands) == 4, "setmetatile arity")
                    x, y, metatile, collision = [
                        expression(value, constants) for value in operands
                    ]
                    xmax, ymax = x, y
                else:
                    require(5 <= len(operands) <= 7, "setmetatileinrange arity")
                    padded = operands + ["FALSE", "0xFF"][len(operands) - 5 :]
                    x, y, xmax, ymax, metatile, collision, elevation = [
                        expression(value, constants) for value in padded
                    ]
                    require(
                        all(0 <= value <= 255 for value in (x, y, xmax, ymax)),
                        "range coordinate byte overflow",
                    )
                    x, xmax = sorted((x, xmax))
                    y, ymax = sorted((y, ymax))
                    row["elevation"] = elevation
                row.update(
                    x=x,
                    y=y,
                    xmax=xmax,
                    ymax=ymax,
                    metatile=metatile,
                    collision=collision,
                )
            except (InvalidData, SyntaxError):
                row["reason"] = "runtime-dependent operand or unsupported expression"
                result.append(row)
                continue
            valid_owners = []
            for owner in owners:
                layout = layouts[maps[owner]["layout"]]
                if (
                    0 <= x <= xmax < layout["width"]
                    and 0 <= y <= ymax < layout["height"]
                ):
                    valid_owners.append(owner)
            row["in_bounds_maps"] = valid_owners
            if owners:
                row["status"] = (
                    "resolved"
                    if len(owners) == 1 and valid_owners == owners
                    else "possible_contexts"
                )
                row["reason"] = (
                    "single map-root literal write"
                    if row["status"] == "resolved"
                    else "control-flow feasibility or current layout requires runtime evidence"
                )
            result.append(row)
    return (result, diagnostics, state_changes)


def inventory(root=ROOT, *, include_dynamic=True):
    constants = read_constants(root)
    names = behavior_names(root, constants)
    sets = metadata(root, constants)
    groups = json.loads((root / "data/maps/map_groups.json").read_text())
    registered = [n for g in groups["group_order"] for n in groups[g]]
    require(len(set(registered)) == len(registered), "duplicate registered map")
    maps = {
        name: json.loads((root / "data/maps" / name / "map.json").read_text())
        for name in registered
    }
    rows = json.loads((root / "data/layouts/layouts.json").read_text())["layouts"]
    layouts = {l["id"]: l for l in rows}
    require(len(layouts) == len(rows), "duplicate layout ID")
    for name, data in maps.items():
        require(data["layout"] in layouts, f"{name}: missing layout")
    report = {
        "schema_version": 1,
        "game": "emerald",
        "includes_dynamic_inventory": include_dynamic,
        "scope": "structural tile compatibility; unresolved dynamic states are not certified",
        "engine_masks": {
            k: expression(constants[k], constants)
            for k in [
                "MAPGRID_METATILE_ID_MASK",
                "MAPGRID_COLLISION_MASK",
                "MAPGRID_ELEVATION_MASK",
                "MAPGRID_COLLISION_SHIFT",
                "MAPGRID_ELEVATION_SHIFT",
            ]
        },
        "maps": {},
        "tilesets": {},
        "layouts": {},
        "errors": [],
        "unresolved": [],
        "data_notes": [],
    }
    for name, data in maps.items():
        active = data.get("region", "REGION_HOENN") == "REGION_HOENN"
        version = layouts[data["layout"]].get("layout_version", "emerald")
        require(
            not active or version == "emerald",
            f"{name}: compiled map points to a dormant layout",
        )
        report["maps"][name] = {
            "id": data["id"],
            "layout": data["layout"],
            "compiled": active,
            "region": data.get("region", "REGION_HOENN"),
            "object_events": data.get("object_events", []),
            "warp_events": data.get("warp_events", []),
            "coord_events": data.get("coord_events", []),
            "bg_events": data.get("bg_events", []),
            "connections": data.get("connections") or [],
        }
    by_id = {data["id"]: name for name, data in maps.items()}
    require(len(by_id) == len(maps), "duplicate map ID")
    for name, data in maps.items():
        if not report["maps"][name]["compiled"]:
            continue
        destinations = [w["dest_map"] for w in data.get("warp_events", [])]
        destinations += [c["map"] for c in data.get("connections") or []]
        for destination in destinations:
            if destination in ("MAP_DYNAMIC", "MAP_NONE"):
                continue
            require(destination in by_id, f"{name}: missing destination {destination}")
            require(
                report["maps"][by_id[destination]]["compiled"],
                f"{name}: active map targets dormant {destination}",
            )
    dynamic, script_contexts, state_changes = (
        script_changes(root, constants, maps, layouts)
        if include_dynamic
        else ([], {}, [])
    )
    additions = {}
    for row in dynamic:
        if row["status"] == "resolved":
            for owner in row["possible_maps"]:
                additions.setdefault(maps[owner]["layout"], set()).add(row["metatile"])
    for layout in rows:
        lid = layout["id"]
        version = layout.get("layout_version", "emerald")
        require(
            version in ("emerald", "frlg"),
            f"{lid}: unsupported layout version {version}",
        )
        frlg = version == "frlg"
        partition = expression(
            constants["NUM_TILES_IN_PRIMARY_FRLG" if frlg else "NUM_TILES_IN_PRIMARY"],
            constants,
        )
        meta_partition = expression(
            constants[
                "NUM_METATILES_IN_PRIMARY_FRLG" if frlg else "NUM_METATILES_IN_PRIMARY"
            ],
            constants,
        )
        w, h = (layout["width"], layout["height"])
        require(
            type(w) is int and type(h) is int and (w > 0) and (h > 0),
            f"{lid}: invalid dimensions",
        )
        raw = (root / layout["blockdata_filepath"]).read_bytes()
        require(len(raw) >= w * h * 2, f"{lid}: map binary too short")
        trailing = len(raw) - w * h * 2
        if trailing:
            report["data_notes"].append(
                {
                    "layout": lid,
                    "reason": "trailing map bytes ignored by engine",
                    "bytes": trailing,
                }
            )
        raw = raw[: w * h * 2]
        border_raw = (root / layout["border_filepath"]).read_bytes()
        bw, bh = (
            (layout.get("border_width", 2), layout.get("border_height", 2))
            if frlg
            else (2, 2)
        )
        require(len(border_raw) == bw * bh * 2, f"{lid}: border binary size mismatch")
        cells = words(raw, 2, lid)
        border = words(border_raw, 2, lid)
        used = set((v & 1023 for v in cells + border)) | additions.get(lid, set())
        primary = sets.get(layout["primary_tileset"])
        secondary = sets.get(layout["secondary_tileset"])
        if primary is None or secondary is None:
            consumers = [
                n
                for n, d in report["maps"].items()
                if d["layout"] == lid and d["compiled"]
            ]
            require(
                not consumers, f"{lid}: missing tileset for compiled map {consumers}"
            )
            report["unresolved"].append(
                {
                    "layout": lid,
                    "reason": "null or missing tileset in unused/dormant layout",
                }
            )
            report["layouts"][lid] = {
                "compiled": not frlg,
                "width": w,
                "height": h,
                "status": "unresolved",
                "packed_cells": cells,
                "border_cells": border,
            }
            continue
        for data, expected_secondary, capacity in [
            (primary, False, partition),
            (secondary, True, 1024 - partition),
        ]:
            require(
                bool(expression(data["isSecondary"], constants)) == expected_secondary,
                f"{lid}: wrong tileset primary/secondary partition",
            )
            graphics = data["graphics"]
            stored = graphics["stored_tiles"]
            require(
                graphics["compressed"] or stored >= capacity,
                f"{lid}: raw tileset shorter than copied partition",
            )
        attrs = []
        for data in [primary, secondary]:
            values = words(
                (root / data["attribute_source"]).read_bytes(),
                4 if frlg else 2,
                data["attribute_source"],
            )
            require(
                len(values) == len(data["components"]) // 8,
                f"{lid}: attribute count/format mismatch",
            )
            attrs.append(values)
        metatile_rows = {}
        for mid in sorted(used):
            which = 0 if mid < meta_partition else 1
            data = [primary, secondary][which]
            index = mid if which == 0 else mid - meta_partition
            if not (0 <= mid < 1024 and 0 <= index < len(attrs[which])):
                report["errors"].append(
                    {
                        "layout": lid,
                        "metatile": mid,
                        "reason": "metatile outside owning tileset",
                    }
                )
                continue
            attribute = attrs[which][index]
            behavior = attribute & (511 if frlg else 255)
            layer = attribute >> (29 if frlg else 12) & (3 if frlg else 15)
            if behavior not in names or layer > 2:
                report["errors"].append(
                    {
                        "layout": lid,
                        "metatile": mid,
                        "reason": "unknown behavior or layer",
                        "behavior": behavior,
                        "layer": layer,
                    }
                )
            refs = []
            for word in data["components"][index * 8 : index * 8 + 8]:
                tile = word & 1023
                palette = word >> 12
                owner = primary if tile < partition else secondary
                offset = tile if tile < partition else tile - partition
                state = (
                    "payload"
                    if offset < owner["graphics"]["payload_tiles"]
                    else "blank_padding"
                )
                if palette >= 13:
                    state = "unresolved_dynamic_override"
                    report["errors"].append(
                        {
                            "layout": lid,
                            "metatile": mid,
                            "tile": tile,
                            "palette": palette,
                            "reason": "palette outside normal map loader; dynamic ownership unproved",
                        }
                    )
                refs.append(
                    {
                        "tile": tile,
                        "palette": palette,
                        "flip_x": bool(word & 1024),
                        "flip_y": bool(word & 2048),
                        "initial_source": state,
                    }
                )
            metatile_rows[str(mid)] = {
                "behavior": names.get(behavior, behavior),
                "layer": layer,
                "components": refs,
            }
        report["layouts"][lid] = {
            "compiled": not frlg,
            "width": w,
            "height": h,
            "primary": layout["primary_tileset"],
            "secondary": layout["secondary_tileset"],
            "partition": partition,
            "packed_cells": cells,
            "border_cells": border,
            "metatiles": metatile_rows,
            "collision_counts": dict(Counter((v >> 10 & 3 for v in cells))),
            "elevation_counts": dict(Counter((v >> 12 for v in cells))),
        }
    report["script_metatile_writes"] = dynamic
    report["script_context_diagnostics"] = script_contexts
    report["script_map_state_changes"] = state_changes
    sys.path.insert(0, str(root / "scripts"))
    from verify_emerald_champions_visual_contracts import (
        REVIEWED_DYNAMIC_SCRIPTED_WARPS,
    )

    for state in state_changes:
        target = state["target"]
        has_compiled_owner = any(
            report["maps"][owner]["compiled"] for owner in state["possible_maps"]
        )
        target_data = (
            layouts.get(target)
            if state["command"] == "setmaplayoutindex"
            else maps.get(by_id.get(target))
        )
        if target_data is None:
            state["status"] = "unresolved_target"
            key = (state["source"], state["command"], state["arguments"])
            if key in REVIEWED_DYNAMIC_SCRIPTED_WARPS:
                state["status"] = "reviewed_dynamic_target"
                state["reason"] = REVIEWED_DYNAMIC_SCRIPTED_WARPS[key]
                continue
            require(
                not has_compiled_owner
                or target in ("MAP_NONE", "MAP_DYNAMIC")
                or not target.startswith(("MAP_", "LAYOUT_")),
                f"unresolved active script target: {state}",
            )
            continue
        compiled_target = (
            target_data.get("layout_version", "emerald") == "emerald"
            if state["command"] == "setmaplayoutindex"
            else target_data.get("region", "REGION_HOENN") == "REGION_HOENN"
        )
        state["status"] = (
            "resolved_target_possible_contexts"
            if state["possible_maps"]
            else "target_known_context_unknown"
        )
        state["compiled_target"] = compiled_target
        require(
            not has_compiled_owner or compiled_target,
            f"active script targets dormant data: {state}",
        )
    for error in report["errors"]:
        error["compiled"] = report["layouts"][error["layout"]]["compiled"]
    for name, data in sets.items():
        report["tilesets"][name] = {k: v for k, v in data.items() if k != "components"}
    if include_dynamic:
        from map_dynamic_inventory import build_inventory

        report["dynamic"] = build_inventory(root)
        for row in report["dynamic"]["animation_writes"]:
            require(
                row["status"] != "invalid",
                f"invalid animation write: {row['source']}:{row['line']}",
            )
        ranges = report["dynamic"]["animation_ranges_by_tileset"]
        for lid, layout in report["layouts"].items():
            if "primary" not in layout:
                continue
            for owner, low, high in [
                (layout["primary"], 0, layout["partition"]),
                (layout["secondary"], layout["partition"], 1024),
            ]:
                for start, end in ranges.get(owner, []):
                    require(
                        low <= start < end <= high,
                        f"{lid}: {owner} animation outside partition",
                    )
            for metatile in layout["metatiles"].values():
                for component in metatile["components"]:
                    component["animation_owners"] = [
                        owner
                        for owner in (layout["primary"], layout["secondary"])
                        if any(
                            (
                                start <= component["tile"] < end
                                for start, end in ranges.get(owner, [])
                            )
                        )
                    ]
    report["counts"] = {
        "registered_maps": len(maps),
        "compiled_hoenn_maps": sum((r["compiled"] for r in report["maps"].values())),
        "dormant_maps": sum((not r["compiled"] for r in report["maps"].values())),
        "registered_layouts": len(rows),
        "compiled_layouts": sum((r["compiled"] for r in report["layouts"].values())),
        "tilesets": len(sets),
        "placement_cells": sum(
            (len(r["packed_cells"]) for r in report["layouts"].values())
        ),
        "compiled_placement_cells": sum(
            (
                len(r["packed_cells"])
                for r in report["layouts"].values()
                if r["compiled"]
            )
        ),
        "used_metatiles_per_layout": sum(
            (len(r.get("metatiles", {})) for r in report["layouts"].values())
        ),
        "script_writes": len(dynamic),
        "script_write_commands": dict(Counter(row["command"] for row in dynamic)),
        "script_map_state_changes": len(state_changes),
        "unresolved_script_writes": sum((r["status"] != "resolved" for r in dynamic)),
        "unresolved_script_writes_possible_hoenn": sum(
            (
                r["status"] != "resolved"
                and any((report["maps"][m]["compiled"] for m in r["possible_maps"]))
                for r in dynamic
            )
        ),
        "script_writes_without_map_context": sum(
            (not r["possible_maps"] for r in dynamic)
        ),
        "errors": len(report["errors"]),
        "compiled_errors": sum((e["compiled"] for e in report["errors"])),
        "dormant_errors": sum((not e["compiled"] for e in report["errors"])),
        "unresolved_static_ownership": len(report["unresolved"]),
    }
    definitions = {}
    for layout in report["layouts"].values():
        if "metatiles" not in layout:
            continue
        keys = []
        for mid, data in layout["metatiles"].items():
            key = f"{layout['primary']}/{layout['secondary']}:{mid}"
            require(
                key not in definitions or definitions[key] == data,
                f"inconsistent metatile definition {key}",
            )
            definitions[key] = data
            keys.append(key)
        layout["metatiles"] = keys
    report["metatiles"] = definitions
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument(
        "--all-data",
        action="store_true",
        help="also fail for invalid dormant data that mapjson excludes from this ROM",
    )
    args = parser.parse_args(argv)
    try:
        report = inventory(args.root)
    except (OSError, ValueError, KeyError, TypeError) as error:
        report = {"status": "failed", "error": f"{type(error).__name__}: {error}"}
    else:
        report["status"] = (
            "failed"
            if (
                report["errors"]
                if args.all_data
                else report["counts"]["compiled_errors"]
            )
            else "compiled_structure_passed_with_unresolved_states"
        )
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, separators=(",", ":")) + "\n")
    print(
        json.dumps(
            {k: report[k] for k in ["status", "counts", "error"] if k in report},
            indent=2,
        )
    )
    return int(report["status"] == "failed")


if __name__ == "__main__":
    raise SystemExit(main())
