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
                else:
                    # A handful of native signs live on the one-tile border,
                    # but even inert/null background entries must belong to
                    # the map rather than surviving as out-of-layout debris.
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


def label_block(path: str, label: str) -> str:
    text = (ROOT / path).read_text()
    match = re.search(
        rf"(?ms)^\s*{re.escape(label)}::?\s*$\n(.*?)(?=^\s*[A-Za-z_][A-Za-z0-9_]*::?\s*$|\Z)",
        text,
    )
    require(match is not None, f"{path}: missing label {label}")
    return match.group(1)


def verify_critical_progression_contracts() -> None:
    dewford = load_json(MAPS_ROOT / "DewfordTown" / "map.json")
    gym_warp = next(
        warp for warp in dewford["warp_events"]
        if warp["dest_map"] == "MAP_DEWFORD_TOWN_GYM"
    )
    require(
        not any(
            event["x"] == gym_warp["x"]
            and event["y"] == gym_warp["y"] + 1
            and event.get("flag") in {None, "0"}
            for event in dewford["object_events"]
        ),
        "Dewford's permanent Gym guide blocks the only approach to the Gym warp",
    )

    birch = label_block(
        "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc",
        "LittlerootTown_ProfessorBirchsLab_EventScript_ReceivePokedex",
    )
    require("FLAG_SYS_NATIONAL_DEX" in birch and "EnableNationalPokedex" in birch,
            "initial Pokedex does not expose the campaign's national roster")

    league_path = "data/maps/EverGrandeCity_PokemonLeague_1F/scripts.inc"
    league = label_block(league_path, "EverGrandeCity_PokemonLeague_1F_EventScript_DoorGuard")
    for badge in range(1, 9):
        require(
            f"goto_if_unset FLAG_BADGE{badge:02d}_GET, EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges" in league,
            f"League entrance does not explicitly require badge {badge}",
        )
    require(
        league.index("FLAG_BADGE08_GET") < league.index("setflag FLAG_ENTERED_ELITE_FOUR"),
        "League opens before completing its explicit eight-badge checks",
    )

    route120 = label_block("data/maps/Route120/scripts.inc", "Route120_EventScript_Steven")
    require(
        route120.index("FLAG_BADGE06_GET") < route120.index("FLAG_RECEIVED_DEVON_SCOPE")
        < route120.index("Route120_EventScript_StevenBattleKecleon"),
        "Route 120 does not hold the eastward story path behind Winona",
    )
    fortree = label_block("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_EventScript_WinonaDefeated")
    require(
        fortree.index("FLAG_BADGE06_GET") < fortree.index("FLAG_HIDE_ROUTE_120_STEVEN"),
        "the Route 120 Winona gate is not retired with the Feather Badge",
    )
    mossdeep = label_block("data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_TateAndLiza")
    require(
        mossdeep.index("FLAG_BADGE06_GET") < mossdeep.index("trainerbattle_double"),
        "Tate and Liza can be challenged before Winona",
    )
    juan = label_block("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_EventScript_Juan")
    require(
        juan.index("FLAG_BADGE07_GET") < juan.index("trainerbattle_double"),
        "Juan can be defeated before Tate and Liza",
    )

    # Inclement's order: deliver Steven's Letter, sail to Slateport, find Brawly there,
    # and only then can his Gym be challenged. Sailing must NOT require his badge.
    dewford = label_block("data/maps/DewfordTown/scripts.inc", "DewfordTown_EventScript_Briney")
    require(
        dewford.index("FLAG_DELIVERED_STEVEN_LETTER") < dewford.index("DewfordTown_Text_WhereAreWeBound"),
        "Slateport sailing is not gated by Steven's Letter",
    )
    require(
        "FLAG_BADGE02_GET" not in dewford and "ITEM_MEGA_RING" not in dewford,
        "Slateport sailing still requires Brawly's badge or the Mega Ring, reversing Inclement's story",
    )
    # The Dewford guide and Slateport Brawly must share one hide flag, so finding him
    # clears the Gym entrance in the same moment.
    dew_map = (ROOT / "data/maps/DewfordTown/map.json").read_text()
    slate_map = (ROOT / "data/maps/SlateportCity/map.json").read_text()
    require(
        dew_map.count("FLAG_HIDE_SLATEPORT_CITY_BRAWLY") == 1
        and slate_map.count("FLAG_HIDE_SLATEPORT_CITY_BRAWLY") == 1,
        "the Dewford Gym guide and Slateport Brawly no longer share a hide flag",
    )
    slate_scripts = (ROOT / "data/maps/SlateportCity/scripts.inc").read_text()
    require(
        "SlateportCity_EventScript_Brawly::" in slate_scripts
        and "removeobject" in slate_scripts.split("SlateportCity_EventScript_Brawly::", 1)[1][:600],
        "finding Brawly in Slateport no longer sends him home",
    )
    # Inclement lights Granite Cave B1F/B2F so the Letter is deliverable before Flash.
    for cave in ("GraniteCave_B1F", "GraniteCave_B2F"):
        require(
            "setflashlevel" in (ROOT / f"data/maps/{cave}/scripts.inc").read_text(),
            f"{cave} lost its ambient light, making Steven's Letter undeliverable before Flash",
        )
    # Both Cycling Road entrances must start/reset the Mach Bike challenge.
    for gate in ("Route110_SeasideCyclingRoadNorthEntrance", "Route110_SeasideCyclingRoadSouthEntrance"):
        require(
            "VAR_CYCLING_CHALLENGE_STATE" in (ROOT / f"data/maps/{gate}/scripts.inc").read_text(),
            f"{gate} no longer initializes the Mach Bike challenge",
        )
    devon = label_block("data/maps/RustboroCity_DevonCorp_3F/scripts.inc", "RustboroCity_DevonCorp_3F_EventScript_MrStone")
    require("checkitem ITEM_MEGA_RING" in devon, "Devon gives its Mega reward before the player owns the Mega Ring")
    devon_path = ROOT / "data/maps/RustboroCity_DevonCorp_3F/scripts.inc"
    devon_text = devon_path.read_text()
    require(
        "FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON" in devon_text
        and "RustboroCity_DevonCorp_3F_EventScript_GivePidgeotite" in devon_text,
        "Devon's Pidgeotite reward still has misleading internal names",
    )
    require(
        "FLAG_RECEIVED_EXP_SHARE" not in devon_text
        and "GiveExpShare" not in devon_text
        and "ExplainExpShare" not in devon_text,
        "obsolete EXP Share terminology remains in Devon's Pidgeotite path",
    )
    require(
        len(re.findall(r"^RustboroCity_DevonCorp_3F_EventScript_Employee::$", devon_text, re.MULTILINE)) == 1,
        "Devon's employee event label is missing or duplicated",
    )

    vars_text = (ROOT / "include/constants/vars.h").read_text()
    require(
        re.search(r"#define\s+VAR_BIRCH_POSTGAME_RESEARCH_STATE\s+0x40D3\b", vars_text) is not None,
        "Birch's renamed postgame state no longer preserves old-save storage",
    )
    birch_path = ROOT / "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc"
    birch_text = birch_path.read_text()
    require("VAR_DEX_UPGRADE_JOHTO_STARTER_STATE" not in birch_text,
            "Birch's postgame state still claims to be a Johto starter reward")
    require(
        not re.search(r"givemon\s+SPECIES_(?:CYNDAQUIL|TOTODILE|CHIKORITA)\b", birch_text),
        "Birch still gives a redundant Johto starter after all regions are initial choices",
    )
    research_tools = (
        "ITEM_DNA_SPLICERS",
        "ITEM_ZYGARDE_CUBE",
        "ITEM_N_SOLARIZER",
        "ITEM_N_LUNARIZER",
        "ITEM_REINS_OF_UNITY",
    )
    campaign_acquisition_sources = assembled_sources(hoenn_map_names())
    campaign_acquisition_sources.extend(MAPS_ROOT.glob("*/map.json"))
    for item in research_tools:
        require(f"checkitem {item}, 1" in birch_text, f"Birch reward cannot resume safely at {item}")
        require(f"giveitem {item}" in birch_text, f"Birch reward does not deliver {item}")
        require(
            len(re.findall(rf"\bgiveitem\s+{item}\b", birch_text)) == 1,
            f"Birch's completion reward gives {item} more than once",
        )
        other_sources = [
            str(path.relative_to(ROOT))
            for path in campaign_acquisition_sources
            if path != birch_path and re.search(rf"\b{item}\b", path.read_text(errors="ignore"))
        ]
        require(
            not other_sources,
            f"{item} is not unique to Birch's completion reward: {', '.join(other_sources)}",
        )
    require(
        birch_text.index("giveitem ITEM_REINS_OF_UNITY")
        < birch_text.index("setvar VAR_BIRCH_POSTGAME_RESEARCH_STATE, 6"),
        "Birch marks the research kit complete before offering every tool",
    )
    migration = label_block(
        "data/maps/LittlerootTown_ProfessorBirchsLab/scripts.inc",
        "LittlerootTown_ProfessorBirchsLab_EventScript_MigrateLegacyResearchReward",
    )
    require(
        all(item in migration for item in research_tools)
        and "LittlerootTown_ProfessorBirchsLab_EventScript_ReopenResearchReward" in migration,
        "old state-six saves cannot reopen the replacement Birch reward",
    )

    for path, label, destination_token in (
        (
            "data/maps/SlateportCity_Harbor/scripts.inc",
            "SlateportCity_Harbor_EventScript_FerryAttendant",
            "SlateportCity_Harbor_EventScript_AskForTicket",
        ),
        (
            "data/maps/LilycoveCity_Harbor/scripts.inc",
            "LilycoveCity_Harbor_EventScript_FerryAttendant",
            "LilycoveCity_Harbor_EventScript_GetEonTicketState",
        ),
    ):
        ferry = label_block(path, label)
        require(
            ferry.index("FLAG_SYS_GAME_CLEAR")
            < ferry.index("FLAG_RECEIVED_SS_TICKET")
            < ferry.index(destination_token),
            f"{path}: postgame ferry can board without the registered S.S. Ticket entitlement",
        )
    ticket_fallback = label_block(
        "data/scripts/players_house.inc",
        "PlayersHouse_1F_EventScript_SSTicketNoRoom",
    )
    require(
        "goto PlayersHouse_1F_EventScript_ReceivedSSTicket" in ticket_fallback,
        "full KEY ITEMS and PC storage trap the forced postgame scene before ferry registration",
    )
    frontier_ferry = label_block(
        "data/maps/BattleFrontier_OutsideWest/scripts.inc",
        "BattleFrontier_OutsideWest_EventScript_FerryAttendant",
    )
    require(
        "FLAG_RECEIVED_SS_TICKET" in frontier_ferry
        and "checkitem ITEM_SS_TICKET" not in frontier_ferry
        and frontier_ferry.index("FLAG_RECEIVED_SS_TICKET")
        < frontier_ferry.index("BattleFrontier_OutsideWest_EventScript_ChooseFerryDestination"),
        "Battle Frontier return ferry ignores the registered S.S. Ticket entitlement",
    )
    for path in (
        "data/maps/SlateportCity_Harbor/scripts.inc",
        "data/maps/LilycoveCity_Harbor/scripts.inc",
        "data/maps/BattleFrontier_OutsideWest/scripts.inc",
    ):
        ferry_text = (ROOT / path).read_text()
        require(
            "confirmed your ferry" in ferry_text
            and "flashed the TICKET" not in ferry_text,
            f"{path}: ferry narration ignores the full-storage registration fallback",
        )
        require(
            "FlashTicketWhereTo" not in ferry_text
            and "FlashedTicketWhereTo" not in ferry_text,
            f"{path}: stale physical-ticket narration symbol remains live",
        )

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
        ("data/maps/RustboroCity_CuttersHouse/scripts.inc", "ITEM_HM_CUT", "FLAG_RECEIVED_HM_CUT"),
        ("data/maps/GraniteCave_1F/scripts.inc", "ITEM_HM_FLASH", "FLAG_RECEIVED_HM_FLASH"),
        ("data/maps/MauvilleCity_House1/scripts.inc", "ITEM_HM_ROCK_SMASH", "FLAG_RECEIVED_HM_ROCK_SMASH"),
        ("data/maps/RusturfTunnel/scripts.inc", "ITEM_HM_STRENGTH", "FLAG_RECEIVED_HM_STRENGTH"),
        ("data/maps/PetalburgCity_WallysHouse/scripts.inc", "ITEM_HM_SURF", "FLAG_RECEIVED_HM_SURF"),
        ("data/maps/MossdeepCity_StevensHouse/scripts.inc", "ITEM_HM_DIVE", "FLAG_RECEIVED_HM_DIVE"),
        ("data/maps/SootopolisCity/scripts.inc", "ITEM_HM_WATERFALL", "FLAG_RECEIVED_HM_WATERFALL"),
        ("data/maps/RustboroCity/scripts.inc", "ITEM_GREAT_BALL", "FLAG_RETURNED_DEVON_GOODS"),
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
    campaign_sources = assembled_sources(map_names)
    script_refs = verify_script_references(campaign_sources, labels)
    specialvar_refs = verify_specialvar_return_contracts(campaign_sources)
    verify_critical_progression_contracts()
    script_lines = sum(
        len(path.read_text(errors="ignore").splitlines())
        for path in assembled_sources(map_names)
    )
    print(f"PASS: {len(map_names)} Hoenn maps have valid layouts, events, warps, and assembled scripts")
    print(f"PASS: {event_count} physical NPC/trigger/sign events and {warp_count} warps resolve")
    print(f"PASS: {script_refs} control-flow/dialogue/movement references resolve across {script_lines} script lines")
    print(f"PASS: {specialvar_refs} value-returning special calls never read a void C function")
    print("PASS: critical badge, Mega, League, legendary, and story-item progression contracts hold")


if __name__ == "__main__":
    main()
