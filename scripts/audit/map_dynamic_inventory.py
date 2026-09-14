#!/usr/bin/env python3
"""Source inventory of map mutations and animation destinations, not execution proof.

Ranges are half-open absolute VRAM tile indices. Table destinations are a possible
write set, not a claim that every element is reached. Unresolved expressions and
map association remain explicit; this deliberately does not emulate C control flow.
"""

import argparse
import ast
import json
import operator
from collections import Counter
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]


_C_NONCODE = re.compile(
    r""""(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*'|/\*.*?\*/|//[^\n]*""", re.S
)


def _blank(text):
    return "".join("\n" if char == "\n" else " " for char in text)


def strip_comments(text):
    """Preserve literals and source offsets while removing only actual comments."""
    return _C_NONCODE.sub(
        lambda match: _blank(match[0]) if match[0].startswith("/") else match[0], text
    )


def _code_only(text):
    return _C_NONCODE.sub(lambda match: _blank(match[0]), text)


def balanced(text, start, opening="(", closing=")", *, code=None):
    depth = 0
    code = _code_only(text) if code is None else code
    for index in range(start, len(code)):
        if code[index] == opening:
            depth += 1
        elif code[index] == closing:
            depth -= 1
            if depth == 0:
                return index
    raise ValueError(f"unbalanced {opening} at offset {start}")


def arguments(text):
    result, start, depth = [], 0, 0
    for index, char in enumerate(_code_only(text)):
        if char in "([{":
            depth += 1
        elif char in ")]}":
            depth -= 1
        elif char == "," and depth == 0:
            result.append(text[start:index].strip())
            start = index + 1
    if text[start:].strip():
        result.append(text[start:].strip())
    return result


def functions(text):
    """Read ordinary repository function definitions; prototypes are excluded."""
    result = {}
    pattern = r"(?m)^(?:static\s+)?(?:\w+[ \t*]+)+(?P<name>\w+)\s*\([^;{}]*\)\s*\{"
    code = _code_only(text)
    for match in re.finditer(pattern, code):
        start = text.index("{", match.start())
        end = balanced(text, start, "{", "}", code=code)
        result[match["name"]] = (start + 1, end)
    return result


def calls(text, targets):
    pattern = r"\b(" + "|".join(map(re.escape, targets)) + r")\s*\("
    code = _code_only(text)
    for match in re.finditer(pattern, code):
        start = text.index("(", match.start())
        end = balanced(text, start, code=code)
        yield match[1], match.start(), arguments(text[start + 1 : end])


def numeric(expression, constants):
    """Conservative integer expressions; names/macros outside known inputs stay unknown."""
    operations = {
        ast.Add: operator.add,
        ast.Sub: operator.sub,
        ast.Mult: operator.mul,
        ast.FloorDiv: operator.floordiv,
        ast.Div: operator.floordiv,
        ast.LShift: operator.lshift,
        ast.RShift: operator.rshift,
        ast.BitOr: operator.or_,
        ast.BitAnd: operator.and_,
        ast.Mod: operator.mod,
    }

    def evaluate(node):
        if isinstance(node, ast.Constant) and type(node.value) is int:
            return node.value
        if isinstance(node, ast.Name):
            return constants[node.id]
        if isinstance(node, ast.BinOp) and type(node.op) in operations:
            return operations[type(node.op)](evaluate(node.left), evaluate(node.right))
        if isinstance(node, ast.UnaryOp) and isinstance(
            node.op, (ast.USub, ast.UAdd, ast.Invert)
        ):
            return (
                ~evaluate(node.operand)
                if isinstance(node.op, ast.Invert)
                else (-1 if isinstance(node.op, ast.USub) else 1)
                * evaluate(node.operand)
            )
        if (
            isinstance(node, ast.Call)
            and isinstance(node.func, ast.Name)
            and node.func.id == "TILE_SIZE"
            and len(node.args) == 1
        ):
            return (
                evaluate(node.args[0])
                * constants["TILE_WIDTH"]
                * constants["TILE_HEIGHT"]
                // 8
            )
        raise ValueError("unresolved expression")

    try:
        return evaluate(ast.parse(expression.strip(), mode="eval").body)
    except (ValueError, SyntaxError, KeyError, ZeroDivisionError, TypeError):
        return None


def read_constants(root):
    constants = {"TRUE": 1, "FALSE": 0}
    expressions = []
    for relative in (
        "include/fieldmap.h",
        "include/global.fieldmap.h",
        "include/gba/defines.h",
        "include/constants/metatile_labels.h",
    ):
        text = strip_comments((root / relative).read_text())
        expressions.extend(re.findall(r"^#define\s+(\w+)[ \t]+([^\n]+)", text, re.M))
    for _ in range(8):
        changed = False
        for name, expression in expressions:
            value = numeric(expression, constants)
            if value is not None and name not in constants:
                constants[name] = value
                changed = True
        if not changed:
            break
    for required in (
        "NUM_TILES_TOTAL",
        "NUM_TILES_IN_PRIMARY",
        "NUM_TILES_IN_PRIMARY_FRLG",
        "TILE_SIZE_4BPP",
    ):
        if required not in constants or constants[required] <= 0:
            raise ValueError(f"missing or invalid constant: {required}")
    return constants


def record_at(text, offset, source, spans, operands):
    owner = next(
        (name for name, (start, end) in spans.items() if start <= offset < end), None
    )
    return {
        "source": source,
        "line": text.count("\n", 0, offset) + 1,
        "function": owner,
        "operands": operands,
    }


def animation_inventory(root, constants):
    relative = "src/tileset_anims.c"
    text = strip_comments((root / relative).read_text())
    spans = functions(text)
    if not spans:
        raise ValueError("no tileset animation functions found")
    graph = {}
    for name, (start, end) in spans.items():
        body = text[start:end]
        graph[name] = {callee for callee, _, _ in calls(body, spans)}
        graph[name].update(
            re.findall(r"s(?:Primary|Secondary)TilesetAnimCallback\s*=\s*(\w+)", body)
        )
    owners = {}
    headers = strip_comments((root / "src/data/tilesets/headers.h").read_text())
    for match in re.finditer(
        r"const struct Tileset\s+(\w+)\s*=\s*\{(.*?)\};", headers, re.S
    ):
        callback = re.search(r"\.callback\s*=\s*(\w+)", match[2])
        if callback is None:
            raise ValueError(f"tileset missing callback: {match[1]}")
        if callback[1] == "NULL":
            continue
        pending, reached = [callback[1]], set()
        while pending:
            node = pending.pop()
            if node in reached:
                continue
            reached.add(node)
            pending.extend(graph.get(node, ()))
        owners[match[1]] = {"callback": callback[1], "functions": reached}
    arrays = {
        m[1]: arguments(m[2])
        for m in re.finditer(r"\b(\w+)\[\]\s*=\s*\{([^{}]*)\}", text, re.S)
    }
    writes = []
    for _, offset, operands in calls(text, ["AppendTilesetAnimToBuffer"]):
        row = record_at(text, offset, relative, spans, operands)
        if row["function"] is None:  # Function definition, not a call.
            continue
        if len(operands) != 3:
            raise ValueError(f'invalid animation copy arguments at {row["line"]}')
        row["owners"] = sorted(
            name
            for name, owner in owners.items()
            if row["function"] in owner["functions"]
        )
        destination = operands[1]
        table = re.fullmatch(r"(\w+)\[([^]]+)\]", destination)
        destinations = arrays.get(table[1], []) if table else [destination]
        row["destination_kind"] = "table_possible_elements" if table else "direct"
        row["tile_ranges"] = []
        size = numeric(operands[2], constants)
        unresolved = not destinations or size is None
        invalid = size is not None and (
            size <= 0 or size % constants["TILE_SIZE_4BPP"] != 0
        )
        for expression in destinations:
            match = re.search(r"TILE_OFFSET_4BPP\(([^()]*)\)", expression)
            tile = numeric(match[1], constants) if match else None
            if tile is None or size is None:
                unresolved = True
                continue
            end = tile + size // constants["TILE_SIZE_4BPP"]
            row["tile_ranges"].append([tile, end])
            invalid |= tile < 0 or end > constants["NUM_TILES_TOTAL"]
        row["status"] = (
            "invalid"
            if invalid
            else "unresolved" if unresolved or not row["owners"] else "resolved"
        )
        writes.append(row)
    if not writes:
        raise ValueError("no animation writes found")
    ranges = {
        owner: sorted(
            {
                tuple(pair)
                for row in writes
                if row["status"] == "resolved" and owner in row["owners"]
                for pair in row["tile_ranges"]
            }
        )
        for owner in owners
    }
    return writes, {
        owner: [list(pair) for pair in pairs] for owner, pairs in ranges.items()
    }


def build_inventory(root=ROOT):
    root = Path(root)
    constants = read_constants(root)
    animations, owner_ranges = animation_inventory(root, constants)
    map_ids = {
        json.loads(path.read_text())["id"]
        for path in (root / "data/maps").glob("*/map.json")
    }
    mutations, loaders, transfers, vram_references = [], [], [], []
    # These are actual graphics/palette consumer APIs, not per-map exceptions.
    loader_apis = (
        "LoadBgTiles",
        "LoadPalette",
        "LoadCompressedPalette",
        "DecompressAndCopyTileDataToVram",
        "DecompressAndLoadBgGfxUsingHeap",
    )
    transfer_apis = (
        "CpuCopy16",
        "CpuCopy32",
        "CpuFastCopy",
        "CpuFastFill",
        "CpuFastFill16",
        "CpuFill16",
        "CpuFill32",
        "DmaCopy16",
        "DmaCopy32",
        "DmaFill16",
        "DmaFill32",
        "RequestDma3Copy",
        "RequestDma3Fill",
    )
    for path in sorted((root / "src").rglob("*.c")):
        text = strip_comments(path.read_text())
        setters = (
            "MapGridSetMetatileIdAt",
            "MapGridSetMetatileEntryAt",
            "MapGridSetMetatileImpassabilityAt",
        )
        targets = [*setters, *loader_apis, *transfer_apis]
        if not any(target in text for target in targets) and not re.search(
            r"\b(?:BG_|OBJ_)?VRAM\b", text
        ):
            continue
        spans = functions(text)
        code = _code_only(text)
        for match in re.finditer(r"(?m)^.*\b(?:BG_|OBJ_)?VRAM\b.*$", code):
            row = record_at(text, match.start(), str(path.relative_to(root)), spans, [])
            row["expression"] = text[match.start() : match.end()].strip()
            row["status"] = "unresolved"
            row["category"] = "vram_reference_direction_unknown"
            vram_references.append(row)
        for api, offset, operands in calls(text, targets):
            row = record_at(text, offset, str(path.relative_to(root)), spans, operands)
            if row["function"] is None:
                # A declaration starts with a return type on its logical line;
                # unknown call ownership must not silently erase mutation evidence.
                prefix = text[text.rfind("\n", 0, offset) + 1 : offset].strip()
                if re.fullmatch(
                    r"(?:static\s+)?(?:void|bool8|bool32|u8|u16|u32|s8|s16|s32|int)\s*",
                    prefix,
                ):
                    continue
            row["api"] = api
            row["numeric_operands"] = [
                numeric(expression, constants) for expression in operands
            ]
            row["status"] = (
                "resolved"
                if all(value is not None for value in row["numeric_operands"])
                else "unresolved"
            )
            row["map_association"] = "unresolved"
            body = (
                text[slice(*spans[row["function"]])] if row["function"] in spans else ""
            )
            row["map_constants_in_function"] = sorted(
                set(re.findall(r"\bMAP_[A-Z][A-Z0-9_]*\b", body)) & map_ids
            )
            if api in transfer_apis:
                destination_index = 2 if api.startswith("Dma") else 1
                destination = (
                    operands[destination_index]
                    if len(operands) > destination_index
                    else ""
                )
                row["destination_expression"] = destination
                row["category"] = (
                    "unresolved_owner"
                    if row["function"] is None
                    else (
                        "explicit_vram_destination"
                        if re.search(r"\b(?:BG_|OBJ_)?VRAM\b", destination)
                        else (
                            "fixed_destination"
                            if numeric(destination, constants) is not None
                            else (
                                "dynamic_table_destination"
                                if "[" in destination
                                else "generic_transfer"
                            )
                        )
                    )
                )
                # A call's operands do not establish its destination allocation,
                # lifetime, transfer size safety or whether its caller runs on a map.
                row["range_status"] = "unresolved"
                transfers.append(row)
            elif api in setters:
                if len(operands) != 3:
                    raise ValueError(f"invalid metatile write arguments: {row}")
                row["coordinate_space"] = "map grid (includes MAP_OFFSET)"
                row["category"] = (
                    "unresolved_owner"
                    if row["function"] is None
                    else (
                        "fixed_coordinate"
                        if all(
                            value is not None for value in row["numeric_operands"][:2]
                        )
                        else (
                            "dynamic_table_coordinate"
                            if any("[" in expression for expression in operands[:2])
                            else "dynamic_coordinate"
                        )
                    )
                )
                mutations.append(row)
            else:
                row["range_status"] = "unresolved"
                values = row["numeric_operands"]
                if api in ("LoadPalette", "LoadCompressedPalette") and len(values) == 3:
                    start, size = values[1:]
                    if start is not None and size is not None:
                        row["palette_color_range"] = [start, start + size // 2]
                        row["range_status"] = (
                            "resolved"
                            if start >= 0
                            and size > 0
                            and size % 2 == 0
                            and start + size // 2 <= 512
                            else "invalid"
                        )
                elif (
                    api
                    in (
                        "LoadBgTiles",
                        "DecompressAndCopyTileDataToVram",
                        "DecompressAndLoadBgGfxUsingHeap",
                    )
                    and len(values) >= 4
                ):
                    size, start = values[2:4]
                    if start is not None and size is not None:
                        # These APIs' tile size also depends on the runtime BG color mode.
                        row["tile_offset"] = start
                        row["byte_count"] = size
                        row["range_status"] = "requires_bg_color_mode"
                row["category"] = (
                    "unresolved_owner"
                    if row["function"] is None
                    else (
                        "fixed_range_loader"
                        if row["range_status"] in ("resolved", "requires_bg_color_mode")
                        else (
                            "dynamic_table_loader"
                            if any("[" in expression for expression in operands[1:])
                            else "generic_loader"
                        )
                    )
                )
                loaders.append(row)
    return {
        "animation_writes": animations,
        "animation_ranges_by_tileset": owner_ranges,
        "c_metatile_writes": mutations,
        "dynamic_loaders": loaders,
        "memory_transfers": transfers,
        "direct_vram_references": vram_references,
        "category_counts": {
            name: dict(Counter(row["category"] for row in rows))
            for name, rows in (
                ("c_metatile_writes", mutations),
                ("dynamic_loaders", loaders),
                ("memory_transfers", transfers),
            )
        },
        "unresolved_counts": {
            "animation_writes": sum(
                row["status"] == "unresolved" for row in animations
            ),
            "c_metatile_writes": sum(
                row["status"] == "unresolved" for row in mutations
            ),
            "dynamic_loaders": sum(row["status"] == "unresolved" for row in loaders),
            "c_map_associations": len(mutations),
            "memory_transfer_ranges": len(transfers),
            "direct_vram_reference_effects": len(vram_references),
        },
        "invalid_animation_count": sum(
            row["status"] == "invalid" for row in animations
        ),
        "limitations": [
            "Source possible-write inventory, not path feasibility or runtime coverage.",
            "Dynamic table indices and C map association are not symbolically executed.",
            "Memory transfers and VRAM references are inventoried without proving effects or map association.",
            "VRAM references include reads/declarations; aliases and object sprite allocation are not resolved.",
            "Metatile setters exclude direct engine map-buffer assignments.",
        ],
    }


def filter_build_conditionals(text, *, is_frlg=False, source="", unresolved=None):
    """Select known game-version branches, preserving line numbers and unknowns."""
    if unresolved is None:
        unresolved = []
    output, stack = [], []
    active = True

    def condition(expression, line):
        expression = expression.strip().replace(" ", "")
        if expression in ("IS_FRLG", "(IS_FRLG)"):
            return is_frlg
        if expression in ("!IS_FRLG", "(!IS_FRLG)"):
            return not is_frlg
        unresolved.append({"source": source, "line": line, "condition": expression})
        return None

    for line_number, raw in enumerate(text.splitlines(keepends=True), 1):
        match = re.match(r"\s*(?:#|\.)(if|ifdef|ifndef|elif|else|endif)\b(.*)", raw)
        if match is None:
            output.append(raw if active else _blank(raw))
            continue
        directive, expression = match.groups()
        if directive in ("if", "ifdef", "ifndef"):
            value = (
                condition(expression, line_number)
                if directive == "if"
                else condition(directive + " " + expression, line_number)
            )
            stack.append([active, value])
            active = active and value is not False
        elif directive in ("else", "elif"):
            if not stack:
                raise ValueError(f"unmatched conditional at {source}:{line_number}")
            parent_active, taken = stack[-1]
            value = True if directive == "else" else condition(expression, line_number)
            active = parent_active and taken is not True and value is not False
            stack[-1][1] = (
                True
                if taken is True or value is True
                else None if taken is None or value is None else False
            )
        else:
            if not stack:
                raise ValueError(f"unmatched endif at {source}:{line_number}")
            active = stack.pop()[0]
        output.append(_blank(raw))
    if stack:
        raise ValueError(f"unclosed build conditional in {source}")
    return "".join(output)


def derive_script_contexts(root, maps, *, diagnostics=None, is_frlg=False, flow=None):
    """Associate labels with possible map-entry reachability, independent of file.

    This is a control-flow overapproximation: flag/variable conditions and switch
    values are not executed. Callbacks invoked only from native C remain unknown.
    `diagnostics` receives missing/dynamic edges, missing roots and ambiguities.
    """
    root = Path(root)
    if diagnostics is None:
        diagnostics = {}
    diagnostics.update(
        unresolved_edges=[],
        missing_roots=[],
        ambiguous_labels=[],
        duplicate_labels=[],
        unresolved_build_conditions=[],
        is_frlg=is_frlg,
        association="possible_control_flow",
    )
    paths = sorted(
        set((root / "data/maps").glob("*/scripts.inc"))
        | set((root / "data/scripts").rglob("*.inc"))
        | {path for path in (root / "data/event_scripts.s",) if path.is_file()}
    )
    if not paths:
        raise ValueError("no map/event scripts found")
    blocks, successors = {}, {}
    for path in paths:
        text = filter_build_conditionals(
            strip_comments(path.read_text()),
            is_frlg=is_frlg,
            source=str(path.relative_to(root)),
            unresolved=diagnostics["unresolved_build_conditions"],
        )
        matches = list(
            re.finditer(r"(?m)^([A-Za-z_][A-Za-z_0-9]*):{1,2}[^\S\n]*", text)
        )
        for index, match in enumerate(matches):
            label = match[1]
            if label in blocks:
                diagnostics["duplicate_labels"].append(label)
            end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
            blocks.setdefault(label, []).append(
                (
                    str(path.relative_to(root)),
                    text.count("\n", 0, match.end()) + 1,
                    text[match.end() : end],
                )
            )
            if index + 1 < len(matches):
                successors[(label, str(path.relative_to(root)))] = matches[index + 1][1]
    graph = {label: set() for label in blocks}
    # Trainer macro operands named event_script are code; *_text operands are not.
    trainer_positions = {}
    macro_path = root / "asm/macros/event.inc"
    if macro_path.is_file():
        for match in re.finditer(
            r"\.macro\s+(trainerbattle\w*)[ \t]+([^\n]+)", macro_path.read_text()
        ):
            trainer_positions[match[1]] = [
                index
                for index, arg in enumerate(arguments(match[2]))
                if arg.split(":")[0].split("=")[0].strip().startswith("event_script")
            ]

    def edge(label, target, source, line, command):
        if target in ("NULL", "FALSE", "0"):
            return
        if target in graph:
            graph[label].add(target)
        else:
            diagnostics["unresolved_edges"].append(
                {
                    "label": label,
                    "target": target,
                    "source": source,
                    "line": line,
                    "command": command,
                }
            )

    terminal = {
        "end",
        "return",
        "endram",
        "returnram",
        "goto",
        "vgoto",
        "gotostd",
        "gotonative",
        "gotopostbattlescript",
        "gotobeatenscript",
        "step_end",
    }
    for label, definitions in blocks.items():
        for source, first_line, body in definitions:
            falls_through = True
            for delta, raw in enumerate(body.splitlines()):
                # @ is GAS commentary; do not consume quoted text as instructions.
                line = raw.split("@", 1)[0].strip()
                if not line or line.startswith(("#", ".if", ".else", ".endif")):
                    continue
                fields = line.split(None, 1)
                command = fields[0]
                operands = arguments(fields[1]) if len(fields) == 2 else []
                targets = []
                if command in ("goto", "call", "vgoto", "vcall"):
                    targets = operands[:1]
                elif command.startswith(
                    ("goto_if", "call_if", "vgoto_if", "vcall_if")
                ) or command in ("map_script", "map_script_2", "case"):
                    targets = operands[-1:]
                elif command in trainer_positions:
                    targets = [
                        operands[index]
                        for index in trainer_positions[command]
                        if index < len(operands)
                    ]
                elif command in (
                    "callstd",
                    "gotostd",
                    "callstd_if",
                    "gotostd_if",
                    "callnative",
                    "gotonative",
                    "gotopostbattlescript",
                    "gotobeatenscript",
                    "trywondercardscript",
                ):
                    diagnostics["unresolved_edges"].append(
                        {
                            "label": label,
                            "source": source,
                            "line": first_line + delta,
                            "command": command,
                            "operands": operands,
                        }
                    )
                for target in targets:
                    edge(label, target, source, first_line + delta, command)
                if command in terminal or command.startswith(
                    (".string", ".byte", ".2byte", ".4byte")
                ):
                    falls_through = False
                    break
            following = successors.get((label, source))
            if falls_through and following:
                graph[label].add(following)
    if flow is not None:
        flow.update(blocks=blocks, graph=graph)
    contexts = {label: set() for label in graph}
    for map_name, data in maps.items():
        script_owner = data.get("shared_scripts_map", map_name)
        roots = {f"{script_owner}_MapScripts"}
        for category in ("object_events", "coord_events", "bg_events"):
            roots.update(
                row["script"]
                for row in (data.get(category) or [])
                if row.get("script") not in (None, 0, "NULL", "0", "0x0", "0x00000000")
            )
        for label in sorted(roots):
            if label not in graph:
                diagnostics["missing_roots"].append(
                    {
                        "map": map_name,
                        "label": label,
                        "region": data.get("region"),
                        "scope": (
                            "hoenn"
                            if data.get("region") == "REGION_HOENN"
                            else "other_or_unknown"
                        ),
                    }
                )
        pending = list(roots & graph.keys())
        reached = set()
        while pending:
            label = pending.pop()
            if label in reached:
                continue
            reached.add(label)
            contexts[label].add(map_name)
            pending.extend(graph[label] - reached)
    result = {label: sorted(names) for label, names in contexts.items()}
    diagnostics["ambiguous_labels"] = [
        label for label, names in result.items() if len(names) > 1
    ]
    diagnostics["unassociated_labels"] = sum(not names for names in result.values())
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--out", type=Path)
    args = parser.parse_args()
    inventory = build_inventory(args.root)
    output = json.dumps(inventory, indent=2) + "\n"
    if args.out:
        args.out.write_text(output)
    else:
        print(output, end="")
    return int(inventory["invalid_animation_count"] != 0)


if __name__ == "__main__":
    raise SystemExit(main())
