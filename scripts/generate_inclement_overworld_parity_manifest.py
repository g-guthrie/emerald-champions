#!/usr/bin/env python3
"""Generate the fail-closed Inclement Emerald v1.13 overworld manifest.

The manifest records the visual and spatial layer only.  Story scripts,
dialogue, trainer data, object flags, and item contents are intentionally not
part of this contract.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = ROOT / "docs" / "inclement_overworld_parity_manifest.json"
VISUAL_OBJECT_FIELDS = (
    "graphics_id",
    "x",
    "y",
    "elevation",
    "movement_type",
    "movement_range_x",
    "movement_range_y",
)
VISUAL_HEADER_FIELDS = (
    "id",
    "name",
    "layout",
    "region_map_section",
    "requires_flash",
    "weather",
    "map_type",
    "allow_cycling",
    "allow_escaping",
    "allow_running",
    "show_map_name",
    "battle_scene",
)
LAYOUT_VISUAL_FIELDS = (
    "width",
    "height",
    "primary_tileset",
    "secondary_tileset",
)
OBJECT_SEMANTIC_FIELDS = VISUAL_OBJECT_FIELDS + (
    "trainer_type",
    "trainer_sight_or_berry_tree_id",
    "script",
    "flag",
    "local_id",
)
COORD_EVENT_FIELDS = (
    "type",
    "x",
    "y",
    "elevation",
    "var",
    "var_value",
    "script",
)
BG_EVENT_FIELDS = (
    "type",
    "x",
    "y",
    "elevation",
    "player_facing_dir",
    "script",
    "item",
    "flag",
)
ASSET_TARGETS = {
    "graphics/object_events/pics/misc/item_ball_inclement.png": "graphics/object_events/pics/misc/item_ball.png",
    "graphics/object_events/pics/misc/gold_item_ball.png": "graphics/object_events/pics/misc/gold_item_ball.png",
    "graphics/object_events/pics/misc/mega_stone.png": "graphics/object_events/pics/misc/mega_stone.png",
    "graphics/object_events/pics/pokemon/articuno_inclement.png": "graphics/object_events/pics/pokemon/articuno.png",
    "graphics/object_events/pics/pokemon/zapdos_inclement.png": "graphics/object_events/pics/pokemon/zapdos.png",
    "graphics/object_events/pics/pokemon/moltres_inclement.png": "graphics/object_events/pics/pokemon/moltres.png",
    "graphics/object_events/pics/pokemon/mewtwo_inclement.png": "graphics/object_events/pics/pokemon/mewtwo.png",
    "graphics/object_events/pics/pokemon/jirachi_inclement.png": "graphics/object_events/pics/pokemon/jirachi.png",
    "graphics/object_events/pics/pokemon/heatran_inclement.png": "graphics/object_events/pics/pokemon/heatran.png",
    "graphics/object_events/pics/pokemon/diancie_inclement.png": "graphics/object_events/pics/pokemon/diancie.png",
    "graphics/object_events/pics/pokemon/regigigas_statue.png": "graphics/object_events/pics/pokemon/regigigas.png",
    "graphics/object_events/pics/pokemon/carbink_inclement.png": "graphics/object_events/pics/pokemon/carbink.png",
    "graphics/object_events/pics/pokemon/regirock_inclement.png": "graphics/object_events/pics/pokemon/regirock.png",
    "graphics/object_events/pics/pokemon/regice_inclement.png": "graphics/object_events/pics/pokemon/regice.png",
    "graphics/object_events/pics/pokemon/registeel_inclement.png": "graphics/object_events/pics/pokemon/registeel.png",
    "graphics/object_events/pics/pokemon_old/chansey.png": "graphics/object_events/pics/pokemon/chansey.png",
}


def load(path: Path):
    return json.loads(path.read_text())


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def is_ignored_build_product(relative: Path) -> bool:
    """Skip files that git ignores in this repository (compiled .4bpp/.lz etc.).

    A local build leaves generated tileset binaries beside the tracked sources;
    they are not migration seams and must not change the fail-closed asset count.
    """
    probe = subprocess.run(
        ("git", "check-ignore", "-q", relative.as_posix()),
        cwd=ROOT,
        check=False,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )
    return probe.returncode == 0


def normalized_rows(rows: list[dict] | None, fields: tuple[str, ...]) -> list[list]:
    return sorted([[row.get(field) for field in fields] for row in (rows or [])], key=repr)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("inclement_root", type=Path)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    source = args.inclement_root.resolve()
    maps_root = source / "data" / "maps"
    layouts_payload = load(source / "data" / "layouts" / "layouts.json")
    layouts = {row["id"]: row for row in layouts_payload["layouts"]}
    commit = subprocess.check_output(
        ["git", "-C", str(source), "rev-parse", "HEAD"], text=True
    ).strip()

    maps = {}
    for map_path in sorted(maps_root.glob("*/map.json")):
        payload = load(map_path)
        current_map_path = ROOT / "data" / "maps" / map_path.parent.name / "map.json"
        if not current_map_path.is_file():
            raise SystemExit(f"current inherited map is missing: {map_path.parent.name}")
        current = load(current_map_path)
        layout = layouts[payload["layout"]]
        source_objects = normalized_rows(payload.get("object_events"), OBJECT_SEMANTIC_FIELDS)
        current_objects = normalized_rows(current.get("object_events"), OBJECT_SEMANTIC_FIELDS)
        source_coords = normalized_rows(payload.get("coord_events"), COORD_EVENT_FIELDS)
        current_coords = normalized_rows(current.get("coord_events"), COORD_EVENT_FIELDS)
        source_bg = normalized_rows(payload.get("bg_events"), BG_EVENT_FIELDS)
        current_bg = normalized_rows(current.get("bg_events"), BG_EVENT_FIELDS)
        maps[map_path.parent.name] = {
            "layout": payload["layout"],
            "visual_header": {
                field: payload.get(field) for field in VISUAL_HEADER_FIELDS
            },
            "layout_visual": {
                field: layout.get(field) for field in LAYOUT_VISUAL_FIELDS
            },
            "blockdata_sha256": sha256(source / layout["blockdata_filepath"]),
            "border_sha256": sha256(source / layout["border_filepath"]),
            "connections": normalized_rows(
                payload.get("connections"), ("direction", "offset", "map")
            ),
            "warps": normalized_rows(
                payload.get("warp_events"),
                ("x", "y", "elevation", "dest_map", "dest_warp_id"),
            ),
            "objects": normalized_rows(payload.get("object_events"), VISUAL_OBJECT_FIELDS),
            "semantic_contract": {
                "objects_classification": "exact" if source_objects == current_objects else "adapted",
                "source_objects": source_objects,
                "current_objects": current_objects,
                "coord_events_classification": "exact" if source_coords == current_coords else "adapted",
                "source_coord_events": source_coords,
                "current_coord_events": current_coords,
                "bg_events_classification": "exact" if source_bg == current_bg else "adapted",
                "source_bg_events": source_bg,
                "current_bg_events": current_bg,
            },
        }

    semantic_summary = {
        "objects_exact_maps": sum(
            row["semantic_contract"]["objects_classification"] == "exact"
            for row in maps.values()
        ),
        "objects_adapted_maps": sum(
            row["semantic_contract"]["objects_classification"] == "adapted"
            for row in maps.values()
        ),
        "coord_events_exact_maps": sum(
            row["semantic_contract"]["coord_events_classification"] == "exact"
            for row in maps.values()
        ),
        "coord_events_adapted_maps": sum(
            row["semantic_contract"]["coord_events_classification"] == "adapted"
            for row in maps.values()
        ),
        "bg_events_exact_maps": sum(
            row["semantic_contract"]["bg_events_classification"] == "exact"
            for row in maps.values()
        ),
        "bg_events_adapted_maps": sum(
            row["semantic_contract"]["bg_events_classification"] == "adapted"
            for row in maps.values()
        ),
    }

    output = {
        "schema": 3,
        "source": "Inclement Emerald v1.13",
        "source_commit": commit,
        "map_count": len(maps),
        "visual_object_fields": list(VISUAL_OBJECT_FIELDS),
        "visual_header_fields": list(VISUAL_HEADER_FIELDS),
        "layout_visual_fields": list(LAYOUT_VISUAL_FIELDS),
        "object_semantic_fields": list(OBJECT_SEMANTIC_FIELDS),
        "coord_event_fields": list(COORD_EVENT_FIELDS),
        "bg_event_fields": list(BG_EVENT_FIELDS),
        "semantic_summary": semantic_summary,
        "asset_sha256": {
            target: sha256(source / source_path)
            for target, source_path in ASSET_TARGETS.items()
        },
        # Only same-path assets consumed by both engines belong in this
        # contract.  Inclement also carries obsolete/generated tileset files
        # that the modern engine never references; those are not migration
        # seams.  The resulting list is still fail-closed once generated.
        "tileset_asset_sha256": {
            path.relative_to(source).as_posix(): sha256(path)
            for path in sorted((source / "data" / "tilesets").rglob("*"))
            if path.is_file()
            and (ROOT / path.relative_to(source)).is_file()
            and not is_ignored_build_product(path.relative_to(source))
        },
        "maps": maps,
    }
    output["tileset_asset_count"] = len(output["tileset_asset_sha256"])
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(output, indent=2) + "\n")
    print(
        f"wrote {args.output} ({len(maps)} inherited maps, "
        f"{output['tileset_asset_count']} shared tileset assets, "
        f"{semantic_summary['objects_adapted_maps']} object adaptations, "
        f"{semantic_summary['coord_events_adapted_maps']} coord adaptations, "
        f"{semantic_summary['bg_events_adapted_maps']} bg adaptations, {commit})"
    )


if __name__ == "__main__":
    main()
