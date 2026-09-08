#!/usr/bin/env python3
"""Map/script structure and selected source contracts for Emerald Champions.

This gate inventories every Hoenn map event and every assembled story-script
reference. It checks dangling maps, warps, labels, call return types, and selected
source patterns around progression. It does not solve state-dependent campaign
reachability or prove that a subsystem is reachable or works at runtime.
"""

from __future__ import annotations

import json
import re
from pathlib import Path

from verify_emerald_champions_visual_contracts import registered_map_names, map_event_geometry_errors


ROOT = Path(__file__).resolve().parents[1]
MAPS_ROOT = ROOT / "data/maps"
NULL_SCRIPT_REFS = {None, "0", "0x0", "NULL"}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def load_json(path: Path) -> dict:
    return json.loads(path.read_text())


def hoenn_map_names() -> list[str]:
    return [name for name in registered_map_names() if "_Frlg" not in name]


def assembled_sources(map_names: list[str]) -> list[Path]:
    result = [ROOT / "data/event_scripts.s"]
    result.extend(
        path
        for path in (ROOT / "data/scripts").glob("*.inc")
        if "frlg" not in path.name.lower()
    )
    result.extend(
        path
        for map_name in map_names
        if (path := MAPS_ROOT / map_name / "scripts.inc").is_file()
    )
    return list(dict.fromkeys(result))


def all_assembly_sources() -> list[Path]:
    paths = list((ROOT / "data").rglob("*.inc"))
    paths.extend((ROOT / "data").rglob("*.s"))
    paths.extend((ROOT / "asm").rglob("*.s"))
    return list(dict.fromkeys(paths))


def label_index(paths: list[Path]) -> dict[str, tuple[Path, int]]:
    labels: dict[str, tuple[Path, int]] = {}
    for path in paths:
        for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            match = re.match(r"^\s*([A-Za-z_][A-Za-z0-9_]*)::?", line)
            if match is not None:
                labels.setdefault(match.group(1), (path, line_number))
    return labels


def verify_map_data(map_names: list[str], labels: dict[str, tuple[Path, int]]) -> tuple[int, int]:
    errors, _, _, _ = map_event_geometry_errors()
    require(not errors, "invalid map geometry:\n" + "\n".join(errors))
    layouts = {
        row["id"]: row
        for row in load_json(ROOT / "data/layouts/layouts.json")["layouts"]
    }
    event_count = 0
    warp_count = 0
    for name in map_names:
        payload = load_json(MAPS_ROOT / name / "map.json")
        layout = layouts[payload["layout"]]
        for asset_key in ("border_filepath", "blockdata_filepath"):
            asset = ROOT / layout[asset_key]
            require(asset.is_file() and asset.stat().st_size > 0, f"{name}: missing layout asset {asset}")
        for section in ("object_events", "coord_events", "bg_events"):
            for index, event in enumerate(payload.get(section) or []):
                event_count += 1
                script = event.get("script")
                if script not in NULL_SCRIPT_REFS:
                    require(script in labels, f"{name}:{section}[{index}]: missing script label {script}")
        warp_count += len(payload.get("warp_events") or [])

    includes = set(re.findall(r'(?m)^\s*\.include\s+"([^"\n]+)"', (ROOT / "data/event_scripts.s").read_text()))
    for map_name in map_names:
        script = MAPS_ROOT / map_name / "scripts.inc"
        if script.is_file():
            require(
                f"data/maps/{map_name}/scripts.inc" in includes,
                f"{map_name}: scripts exist but are not assembled",
            )
    return event_count, warp_count


def clean_script_line(line: str) -> str:
    line = line.split("@", 1)[0]
    return re.sub(r'"(?:\\.|[^"\\])*"', "", line).strip()


def verify_script_references(paths: list[Path], labels: dict[str, tuple[Path, int]]) -> int:
    checked = 0
    for path in paths:
        for line_number, raw_line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            line = clean_script_line(raw_line)
            if not line or line.startswith(".") or re.match(r"^[A-Za-z_]\w*::?", line):
                continue
            parts = line.split(None, 1)
            if len(parts) != 2:
                continue
            command, payload = parts
            arguments = [part.strip() for part in payload.split(",")]
            targets: list[str] = []
            if command in {"goto", "call", "msgbox", "braillemessage"}:
                targets = arguments[:1]
            elif command.startswith("goto_if_") or command.startswith("call_if_"):
                targets = arguments[-1:]
            elif command in {"applymovement", "applymovementat"}:
                targets = arguments[-1:]
            elif command in {"map_script", "map_script_2", "case", "switchcase"}:
                targets = arguments[-1:]
            elif command == "loadword" and arguments and arguments[0] in {"0", "1"}:
                targets = arguments[-1:]
            elif command.startswith("trainerbattle_") or command == "multi_2_vs_2":
                targets = [
                    argument for argument in arguments
                    if "_Text_" in argument or "_EventScript_" in argument
                ]

            for target in targets:
                target = target.split()[0] if target else target
                if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", target or ""):
                    continue
                if target.startswith((
                    "VAR_", "FLAG_", "ITEM_", "SPECIES_", "TRAINER_", "LOCALID_",
                    "OBJ_EVENT_", "MAP_", "DIR_", "MSGBOX_",
                )) or target in {"TRUE", "FALSE", "NULL", "NO_MUSIC"}:
                    continue
                if target.startswith("gStringVar"):
                    continue
                checked += 1
                require(target in labels, f"{path.relative_to(ROOT)}:{line_number}: missing script reference {target}")
    return checked


def verify_specialvar_return_contracts(paths: list[Path]) -> int:
    """Reject `specialvar` calls to C functions that cannot return a value."""
    void_specials: set[str] = set()
    for path in (ROOT / "src").rglob("*.c"):
        void_specials.update(
            re.findall(r"(?m)^void\s+([A-Za-z_][A-Za-z0-9_]*)\s*\(", path.read_text(errors="ignore"))
        )

    checked = 0
    for path in paths:
        for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            match = re.search(r"\bspecialvar\s+[^,]+,\s*([A-Za-z_][A-Za-z0-9_]*)", line)
            if match is None:
                continue
            checked += 1
            require(
                match.group(1) not in void_specials,
                f"{path.relative_to(ROOT)}:{line_number}: specialvar reads void special {match.group(1)}",
            )
    return checked


def main() -> None:
    map_names = hoenn_map_names()
    all_sources = all_assembly_sources()
    labels = label_index(all_sources)
    event_count, warp_count = verify_map_data(map_names, labels)
    campaign_sources = assembled_sources(map_names)
    script_refs = verify_script_references(campaign_sources, labels)
    specialvar_refs = verify_specialvar_return_contracts(campaign_sources)
    script_lines = sum(
        len(path.read_text(errors="ignore").splitlines())
        for path in assembled_sources(map_names)
    )
    print(f"PASS: {len(map_names)} Hoenn maps have valid layouts, events, warps, and assembled scripts")
    print(f"PASS: {event_count} physical NPC/trigger/sign events and {warp_count} warps resolve")
    print(f"PASS: {script_refs} control-flow/dialogue/movement references resolve across {script_lines} script lines")
    print(f"PASS: {specialvar_refs} value-returning special calls never read a void C function")
    print("Scope: static references and source patterns only; state-dependent campaign reachability is not verified")


if __name__ == "__main__":
    main()
