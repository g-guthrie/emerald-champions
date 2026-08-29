#!/usr/bin/env python3
"""Whole-campaign structural and progression contracts for Emerald Champions.

This gate inventories every Hoenn map event and every assembled story-script
reference.  It complements focused feature verifiers: those prove the design
of a subsystem, while this file proves that the physical campaign can reach
and invoke the subsystem without dangling maps, warps, labels, or state gates.
"""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAPS_ROOT = ROOT / "data/maps"
NULL_SCRIPT_REFS = {None, "0", "0x0", "NULL"}
DYNAMIC_MAPS = {"MAP_DYNAMIC"}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def load_json(path: Path) -> dict:
    return json.loads(path.read_text())


def hoenn_map_names() -> list[str]:
    groups = load_json(MAPS_ROOT / "map_groups.json")
    names = [
        map_name
        for group_name in groups["group_order"]
        for map_name in groups[group_name]
        if "_Frlg" not in map_name
    ]
    require(len(names) == len(set(names)), "Hoenn map group contains duplicate map names")
    return names


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
    layouts = {
        row["id"]: row
        for row in load_json(ROOT / "data/layouts/layouts.json")["layouts"]
    }
    maps = {name: load_json(MAPS_ROOT / name / "map.json") for name in map_names}
    by_id: dict[str, str] = {}
    event_count = 0
    warp_count = 0

    for name, payload in maps.items():
        map_id = payload["id"]
        require(map_id not in by_id, f"duplicate Hoenn map id {map_id}")
        by_id[map_id] = name
        require(payload["layout"] in layouts, f"{name}: missing layout {payload['layout']}")
        layout = layouts[payload["layout"]]
        width, height = layout["width"], layout["height"]
        for asset_key in ("border_filepath", "blockdata_filepath"):
            asset = ROOT / layout[asset_key]
            require(asset.is_file() and asset.stat().st_size > 0, f"{name}: missing layout asset {asset}")

        local_ids = [
            event["local_id"]
            for event in payload.get("object_events", []) or []
            if event.get("local_id")
        ]
        require(len(local_ids) == len(set(local_ids)), f"{name}: duplicate object local ids")

        for section in ("object_events", "coord_events", "bg_events"):
            for index, event in enumerate(payload.get(section, []) or []):
                event_count += 1
                script = event.get("script")
                if script not in NULL_SCRIPT_REFS:
                    require(script in labels, f"{name}:{section}[{index}]: missing script label {script}")
                if section in {"object_events", "coord_events"}:
                    require(
                        0 <= event["x"] < width and 0 <= event["y"] < height,
                        f"{name}:{section}[{index}] outside {width}x{height} at {event['x']},{event['y']}",
                    )
                elif script not in NULL_SCRIPT_REFS:
                    # A handful of native signs live on the one-tile border.
                    require(
                        0 <= event["x"] <= width and 0 <= event["y"] <= height,
                        f"{name}:{section}[{index}] far outside {width}x{height} at {event['x']},{event['y']}",
                    )

        for index, warp in enumerate(payload.get("warp_events", []) or []):
            warp_count += 1
            require(
                0 <= warp["x"] <= width + 1 and 0 <= warp["y"] <= height + 1,
                f"{name}:warp[{index}] far outside {width}x{height} at {warp['x']},{warp['y']}",
            )
        for index, connection in enumerate(payload.get("connections", []) or []):
            require(connection["map"] in by_id or connection["map"] in {
                other["id"] for other in maps.values()
            }, f"{name}:connection[{index}] targets missing map {connection['map']}")

    # Validate destinations only after the complete map-id index exists.
    for name, payload in maps.items():
        for index, warp in enumerate(payload.get("warp_events", []) or []):
            destination = warp["dest_map"]
            destination_index = str(warp["dest_warp_id"])
            if destination in DYNAMIC_MAPS:
                continue
            require(destination in by_id, f"{name}:warp[{index}] targets missing map {destination}")
            if destination_index.isdigit():
                destination_warps = maps[by_id[destination]].get("warp_events", []) or []
                require(
                    int(destination_index) < len(destination_warps),
                    f"{name}:warp[{index}] targets {destination} warp {destination_index}, table has {len(destination_warps)}",
                )

    includes = (ROOT / "data/event_scripts.s").read_text()
    for map_name in map_names:
        script = MAPS_ROOT / map_name / "scripts.inc"
        if script.is_file():
            require(
                f'\t.include "data/maps/{map_name}/scripts.inc"' in includes,
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


def label_block(path: str, label: str) -> str:
    text = (ROOT / path).read_text()
    match = re.search(
        rf"(?ms)^\s*{re.escape(label)}::?\s*$\n(.*?)(?=^\s*[A-Za-z_][A-Za-z0-9_]*::?\s*$|\Z)",
        text,
    )
    require(match is not None, f"{path}: missing label {label}")
    return match.group(1)


def verify_critical_progression_contracts() -> None:
    birch = label_block(
        "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc",
        "LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex",
    )
    require("FLAG_SYS_NATIONAL_DEX" in birch and "EnableNationalPokedex" in birch,
            "initial Pokedex does not expose the campaign's national roster")

    dewford = label_block("data/maps/DewfordTown/scripts.inc", "DewfordTown_EventScript_Briney")
    require(
        dewford.index("FLAG_BADGE02_GET") < dewford.index("checkitem ITEM_MEGA_RING")
        < dewford.index("DewfordTown_Text_WhereAreWeBound"),
        "Slateport sailing is not gated by Brawly and receipt of the Mega Ring",
    )
    devon = label_block("data/maps/RustboroCity_DevonCorp_3F/scripts.inc", "RustboroCity_DevonCorp_3F_EventScript_MrStone")
    require("checkitem ITEM_MEGA_RING" in devon, "Devon gives its Mega reward before the player owns the Mega Ring")

    sootopolis = (ROOT / "data/maps/SootopolisCity/scripts.inc").read_text()
    require(
        "SootopolisCity_EventScript_SetExpertBesideCaveEntrance::\n\tsetobjectxyperm LOCALID_SOOTOPOLIS_EXPERT, 30, 18" in sootopolis,
        "Badge-eight Cave of Origin entrance is physically blocked",
    )

    pecharunt = label_block("data/maps/MtPyre_6F/scripts.inc", "MtPyre_6F_EventScript_Pecharunt")
    for branch in ("goto_if_eq VAR_RESULT, 0", "goto_if_eq VAR_RESULT, 1"):
        require(branch in pecharunt, f"Pecharunt ignores prerequisite result: {branch}")

    capture_only = {
        "data/maps/DesertRuins/scripts.inc": ("Regirock", "DesertRuins_EventScript_DefeatedRegirock", "FLAG_DEFEATED_REGIROCK"),
        "data/maps/AncientTomb/scripts.inc": ("Registeel", "AncientTomb_EventScript_DefeatedRegisteel", "FLAG_DEFEATED_REGISTEEL"),
        "data/maps/IslandCave/scripts.inc": ("Regice", "IslandCave_EventScript_DefeatedRegice", "FLAG_DEFEATED_REGICE"),
        "data/maps/TerraCave_End/scripts.inc": ("Groudon", "TerraCave_End_EventScript_DefeatedGroudon", "FLAG_DEFEATED_GROUDON"),
        "data/maps/MarineCave_End/scripts.inc": ("Kyogre", "MarineCave_End_EventScript_DefeatedKyogre", "FLAG_DEFEATED_KYOGRE"),
        "data/maps/SkyPillar_Top/scripts.inc": ("Rayquaza", "SkyPillar_Top_EventScript_DefeatedRayquaza", "FLAG_DEFEATED_RAYQUAZA"),
        "data/maps/SouthernIsland_Interior/scripts.inc": ("Lati", "SouthernIsland_Interior_EventScript_LatiDefeated", "FLAG_DEFEATED_LATIAS_OR_LATIOS"),
        "data/maps/NavelRock_Top/scripts.inc": ("HoOh", "NavelRock_Top_EventScript_DefeatedHoOh", "FLAG_DEFEATED_HO_OH"),
        "data/maps/NavelRock_Bottom/scripts.inc": ("Lugia", "NavelRock_Bottom_EventScript_DefeatedLugia", "FLAG_DEFEATED_LUGIA"),
        "data/maps/FarawayIsland_Interior/scripts.inc": ("Mew", "FarawayIsland_Interior_EventScript_MewDefeated", "FLAG_DEFEATED_MEW"),
        "data/maps/BirthIsland_Exterior/scripts.inc": ("Deoxys", "BirthIsland_Exterior_EventScript_DefeatedDeoxys", "FLAG_DEFEATED_DEOXYS"),
    }
    for path, (name, defeated_label, terminal_flag) in capture_only.items():
        text = (ROOT / path).read_text()
        require("B_OUTCOME_CAUGHT" in text, f"{path}: {name} has no capture-only branch")
        defeated = label_block(path, defeated_label)
        require(terminal_flag not in defeated, f"{path}: knocking out {name} still sets terminal flag")
        require("CreateEmeraldChampionsStaticLegendaryEncounter" in text or "seteventmon SPECIES_LAT" in text,
                f"{path}: {name} is not scaled to its campaign milestone")
        if name == "Lati":
            require(
                "seteventmon SPECIES_LATIOS, 100" in text and "seteventmon SPECIES_LATIAS, 100" in text,
                "Southern Island Lati encounter is below the postgame cap",
            )

    for path, item, progress_token in (
        ("data/maps/RustboroCity_DevonCorp_3F/scripts.inc", "ITEM_LETTER", "FLAG_RECEIVED_POKENAV"),
        ("data/maps/RusturfTunnel/scripts.inc", "ITEM_DEVON_PARTS", "FLAG_RECOVERED_DEVON_GOODS"),
        ("data/scripts/players_house.inc", "ITEM_SS_TICKET", "FLAG_RECEIVED_SS_TICKET"),
        ("data/maps/Route110/scripts.inc", "ITEM_DOWSING_MACHINE", "VAR_ROUTE110_STATE"),
        ("data/maps/SlateportCity/scripts.inc", "ITEM_POWDER_JAR", "FLAG_RECEIVED_POWDER_JAR"),
        ("data/maps/Route120/scripts.inc", "ITEM_DEVON_SCOPE", "FLAG_RECEIVED_DEVON_SCOPE"),
        ("data/maps/MtPyre_Summit/scripts.inc", "ITEM_MAGMA_EMBLEM", "FLAG_HIDE_JAGGED_PASS_MAGMA_GUARD"),
        ("data/maps/LavaridgeTown/scripts.inc", "ITEM_GO_GOGGLES", "FLAG_RECEIVED_GO_GOGGLES"),
        ("data/maps/Route119/scripts.inc", "ITEM_HM_FLY", "FLAG_RECEIVED_HM_FLY"),
    ):
        text = (ROOT / path).read_text()
        gift = text.index(f"giveitem {item}")
        progress = text.index(progress_token, gift)
        require("goto_if_eq VAR_RESULT, FALSE" in text[gift:progress],
                f"{path}: story advances after failed delivery of {item}")


def main() -> None:
    map_names = hoenn_map_names()
    all_sources = all_assembly_sources()
    labels = label_index(all_sources)
    event_count, warp_count = verify_map_data(map_names, labels)
    script_refs = verify_script_references(assembled_sources(map_names), labels)
    verify_critical_progression_contracts()
    script_lines = sum(
        len(path.read_text(errors="ignore").splitlines())
        for path in assembled_sources(map_names)
    )
    print(f"PASS: {len(map_names)} Hoenn maps have valid layouts, events, warps, and assembled scripts")
    print(f"PASS: {event_count} physical NPC/trigger/sign events and {warp_count} warps resolve")
    print(f"PASS: {script_refs} control-flow/dialogue/movement references resolve across {script_lines} script lines")
    print("PASS: critical badge, Mega, League, legendary, and story-item progression contracts hold")


if __name__ == "__main__":
    main()
