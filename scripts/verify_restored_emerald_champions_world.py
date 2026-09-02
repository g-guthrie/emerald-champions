#!/usr/bin/env python3
"""Release gates for the restored Emerald Champions side-area network."""

from __future__ import annotations

import json
import re
import struct
from collections import Counter, defaultdict, deque
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
GROUP = "gMapGroup_EmeraldChampionsExpansion"
EXPECTED_MAPS = {
    "AlteringCave_1F",
    "AlteringCave_B1F",
    "AshenWoods",
    "CaveOfOrigin_DianciesRoom",
    "DewfordManor_1F",
    "DewfordMeadow",
    "EmberPath",
    "MeteorFalls_JirachisRoom",
    "MirageTower_B1F",
    "PetalburgWoods_2",
    "PetalburgWoods_3",
    "Route111_RuinsExterior",
    "SandstrewnRuins",
    "SandstrewnRuins_2F",
    "SandstrewnRuins_3F",
    "SandstrewnRuins_B1F",
    "ScorchedSlab_B1F",
    "ScorchedSlab_B2F",
    "ScorchedSlab_HeatransRoom",
    "Seaspray_Cave",
    "Seaspray_Cave_B1F",
    "VerdanturfMeadow",
}
NO_WILD_TABLE = {"MeteorFalls_JirachisRoom"}
OBSOLETE_TOKENS = {
    "HG_SEQ_GS_D_IWAYAMA",
    "HG_SEQ_GS_D_UNKNOWN_ISEKI",
    "MUS_ROUTE111",
}
FIXED_INCLEMENT_SPECIES_GFX = {
    "OBJ_EVENT_GFX_INCLEMENT_ARTICUNO": "ARTICUNO",
    "OBJ_EVENT_GFX_INCLEMENT_ZAPDOS": "ZAPDOS",
    "OBJ_EVENT_GFX_INCLEMENT_MOLTRES": "MOLTRES",
    "OBJ_EVENT_GFX_INCLEMENT_MEWTWO": "MEWTWO",
    "OBJ_EVENT_GFX_INCLEMENT_JIRACHI": "JIRACHI",
    "OBJ_EVENT_GFX_INCLEMENT_HEATRAN": "HEATRAN",
    "OBJ_EVENT_GFX_INCLEMENT_DIANCIE": "DIANCIE",
    "OBJ_EVENT_GFX_INCLEMENT_CARBINK": "CARBINK",
    "OBJ_EVENT_GFX_REGIGIGAS_STATUE": "REGIGIGAS",
}

# Source: Inclement Emerald v1.13, commit
# cf41a95b68a39ca74fefeb934c460f6f47eb0b3b.  These are the hidden
# evolution/form rewards that remain meaningful under Emerald Champions'
# non-scarce battle-item economy.
INCLEMENT_PROGRESSION_BG_EVENTS = {
    "DewfordMeadow": (
        (11, 18, 3, "ITEM_YELLOW_NECTAR", "FLAG_EC_HIDDEN_ITEM_DEWFORD_MEADOW_YELLOW_NECTAR"),
        (21, 5, 3, "ITEM_RED_NECTAR", "FLAG_EC_HIDDEN_ITEM_DEWFORD_MEADOW_RED_NECTAR"),
    ),
    "EmberPath": (
        (9, 38, 3, "ITEM_MAGMARIZER", "FLAG_EC_HIDDEN_ITEM_EMBER_PATH_MAGMARIZER"),
    ),
    "Route106": (
        (53, 12, 3, "ITEM_PRISM_SCALE", "FLAG_EC_HIDDEN_ITEM_ROUTE_106_PRISM_SCALE"),
    ),
    "SandstrewnRuins": (
        (8, 31, 3, "ITEM_PROTECTOR", "FLAG_EC_HIDDEN_ITEM_SANDSTREWN_PROTECTOR"),
    ),
    "Seaspray_Cave": (
        (36, 22, 4, "ITEM_DAWN_STONE", "FLAG_EC_HIDDEN_ITEM_SEASPRAY_DAWN_STONE"),
    ),
    "Seaspray_Cave_B1F": (
        (25, 20, 3, "ITEM_ICE_STONE", "FLAG_EC_HIDDEN_ITEM_SEASPRAY_B1F_ICE_STONE"),
    ),
    "VerdanturfMeadow": (
        (4, 15, 3, "ITEM_PINK_NECTAR", "FLAG_EC_HIDDEN_ITEM_VERDANTURF_PINK_NECTAR"),
        (10, 15, 3, "ITEM_PURPLE_NECTAR", "FLAG_EC_HIDDEN_ITEM_VERDANTURF_PURPLE_NECTAR"),
    ),
}

INCLEMENT_MANOR_SIGN = {
    "type": "sign",
    "x": 8,
    "y": 8,
    "elevation": 0,
    "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
    "script": "DewfordMeadow_EventScript_ManorSign",
}

INCLEMENT_SPIRITOMB_SIGN = {
    "type": "sign",
    "x": 7,
    "y": 7,
    "elevation": 0,
    "player_facing_dir": "BG_EVENT_PLAYER_FACING_ANY",
    "script": "AbandonedShip_Room_B1F_EventScript_Spiritomb",
}


def load(path: Path):
    return json.loads(path.read_text())


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def main() -> None:
    maps_root = ROOT / "data" / "maps"
    groups = load(maps_root / "map_groups.json")
    restored = set(groups.get(GROUP, []))
    require(restored == EXPECTED_MAPS, f"restored map set drifted: {sorted(EXPECTED_MAPS ^ restored)}")

    layouts_payload = load(ROOT / "data" / "layouts" / "layouts.json")
    layouts = {row["id"]: row for row in layouts_payload["layouts"]}
    event_scripts = (ROOT / "data" / "event_scripts.s").read_text()
    flag_header = (ROOT / "include" / "constants" / "flags.h").read_text()
    flag_values = {
        name: int(value, 0)
        for name, value in re.findall(r"^#define\s+(FLAG_EC_[A-Z0-9_]+)\s+(0x[0-9A-Fa-f]+|\d+)", flag_header, re.M)
    }

    all_maps: dict[str, dict] = {}
    id_to_name: dict[str, str] = {}
    for map_path in maps_root.glob("*/map.json"):
        payload = load(map_path)
        name = map_path.parent.name
        all_maps[name] = payload
        map_id = payload["id"]
        require(map_id not in id_to_name, f"duplicate map id {map_id}")
        id_to_name[map_id] = name

    restored_hidden_flags = []
    for map_name, expected_rows in INCLEMENT_PROGRESSION_BG_EVENTS.items():
        payload = all_maps[map_name]
        layout = layouts[payload["layout"]]
        actual_rows = payload.get("bg_events", []) or []
        positions = Counter(
            (event.get("x"), event.get("y"), event.get("elevation"))
            for event in actual_rows
        )
        require(
            all(count == 1 for count in positions.values()),
            f"{map_name}: overlapping background events",
        )
        for x, y, elevation, item_id, flag in expected_rows:
            require(
                0 <= x < layout["width"] and 0 <= y < layout["height"],
                f"{map_name}: restored Inclement reward is outside the layout",
            )
            matches = [
                event for event in actual_rows
                if event.get("type") == "hidden_item"
                and event.get("x") == x
                and event.get("y") == y
                and event.get("elevation") == elevation
                and event.get("item") == item_id
                and event.get("flag") == flag
            ]
            require(
                len(matches) == 1,
                f"{map_name}: missing exact Inclement progression reward "
                f"{item_id} at {x},{y},{elevation}",
            )
            require(flag in flag_values, f"{map_name}: undefined hidden-item flag {flag}")
            restored_hidden_flags.append(flag)

    require(
        len(restored_hidden_flags) == len(set(restored_hidden_flags)) == 9,
        "Inclement progression reward flags are not unique",
    )
    require(
        len({flag_values[flag] for flag in restored_hidden_flags}) == 9,
        "Inclement progression reward flag values collide",
    )

    meadow_bg_events = all_maps["DewfordMeadow"].get("bg_events", []) or []
    require(
        meadow_bg_events.count(INCLEMENT_MANOR_SIGN) == 1,
        "Dewford Meadow lost Inclement's manor sign",
    )
    meadow_script = (maps_root / "DewfordMeadow" / "scripts.inc").read_text()
    require(
        "DewfordMeadow_EventScript_ManorSign::" in meadow_script
        and "DewfordMeadow_Text_ManorSign:" in meadow_script,
        "Dewford Meadow manor sign script or copy is missing",
    )

    ship_bg_events = all_maps["AbandonedShip_Room_B1F"].get("bg_events", []) or []
    require(
        ship_bg_events.count(INCLEMENT_SPIRITOMB_SIGN) == 1,
        "Abandoned Ship lost Inclement's Spiritomb interaction",
    )
    spiritomb_script = (maps_root / "AbandonedShip_Room_B1F" / "scripts.inc").read_text()
    for token in (
        "checkspecies SPECIES_LICKITUNG",
        "checkspecies SPECIES_SLUGMA",
        "checkitem ITEM_ODD_KEYSTONE, 1",
        "setvar VAR_0x8004, SPECIES_SPIRITOMB",
        "setvar VAR_0x8005, 0",
        "special CreateEmeraldChampionsStaticLegendaryEncounter",
        "special BattleSetup_StartLegendaryBattle",
        "specialvar VAR_RESULT, GetBattleOutcome",
        "goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, AbandonedShip_Room_B1F_EventScript_End",
        "removeitem ITEM_ODD_KEYSTONE, 1",
    ):
        require(token in spiritomb_script, f"Abandoned Ship Spiritomb flow lost: {token}")
    require(
        spiritomb_script.index("special BattleSetup_StartLegendaryBattle")
        < spiritomb_script.index("specialvar VAR_RESULT, GetBattleOutcome")
        < spiritomb_script.index(
            "goto_if_ne VAR_RESULT, B_OUTCOME_CAUGHT, AbandonedShip_Room_B1F_EventScript_End"
        )
        < spiritomb_script.index("removeitem ITEM_ODD_KEYSTONE, 1"),
        "Odd Keystone is consumed before Spiritomb is safely caught",
    )
    scripted_spiritomb_count = sum(
        (maps_root / name / "scripts.inc").read_text().count("setvar VAR_0x8004, SPECIES_SPIRITOMB")
        for name in all_maps
        if (maps_root / name / "scripts.inc").is_file()
    )
    require(
        scripted_spiritomb_count == 1,
        "the restored Spiritomb interaction duplicates another scripted encounter",
    )
    legendary_source = (ROOT / "src/legendary_signs.c").read_text()
    require(
        "if (level > MAX_LEVEL)" in legendary_source
        and "CreateScriptedWildMon(species, GetSignLevel(levelOffset), ITEM_NONE);"
            in legendary_source,
        "the Spiritomb encounter no longer clamps the postgame level sentinel",
    )

    graph: dict[str, set[str]] = defaultdict(set)
    inbound = Counter()
    for name, payload in all_maps.items():
        for warp in payload.get("warp_events", []):
            destination = warp.get("dest_map")
            if destination in {"MAP_DYNAMIC", "MAP_UNDEFINED", None}:
                continue
            require(destination in id_to_name, f"{name}: unknown warp destination {destination}")
            other = id_to_name[destination]
            graph[name].add(other)
            inbound[other] += 1
        for connection in payload.get("connections") or []:
            destination = connection.get("map")
            require(destination in id_to_name, f"{name}: unknown connection destination {destination}")
            other = id_to_name[destination]
            graph[name].add(other)
            inbound[other] += 1

    # Sootopolis is entered through Dive/emerge state rather than an ordinary
    # directed map connection, so it is an explicit second campaign root.
    campaign_roots = {"LittlerootTown", "SootopolisCity"}
    reachable = set(campaign_roots)
    queue = deque(campaign_roots)
    while queue:
        current = queue.popleft()
        for other in graph[current]:
            if other not in reachable:
                reachable.add(other)
                queue.append(other)

    item_flags: list[str] = []
    visible_species = set()
    total_objects = 0
    for name in sorted(restored):
        payload = all_maps[name]
        require(payload["region"] == "REGION_HOENN", f"{name}: not in Hoenn region")
        require(name in reachable, f"{name}: disconnected from the campaign graph")
        require(inbound[name] > 0, f"{name}: no inbound warp or connection")
        require(payload["layout"] in layouts, f"{name}: missing layout {payload['layout']}")
        layout = layouts[payload["layout"]]
        for key in ("border_filepath", "blockdata_filepath"):
            path = ROOT / layout[key]
            require(path.is_file() and path.stat().st_size > 0, f"{name}: missing layout asset {path}")
        include = f'\t.include "data/maps/{name}/scripts.inc"'
        require(include in event_scripts, f"{name}: scripts are not globally included")
        script_text = (maps_root / name / "scripts.inc").read_text()
        encoded = json.dumps(payload)
        for token in OBSOLETE_TOKENS:
            require(token not in encoded and token not in script_text, f"{name}: obsolete token {token}")

        local_ids = []
        for section in ("object_events", "warp_events", "coord_events", "bg_events"):
            for event in payload.get(section, []) or []:
                require(0 <= event["x"] < layout["width"] and 0 <= event["y"] < layout["height"],
                        f"{name}: {section} event outside {layout['width']}x{layout['height']} at {event['x']},{event['y']}")
        warp_coordinates = [
            (event["x"], event["y"], event["elevation"])
            for event in payload.get("warp_events", []) or []
        ]
        require(len(warp_coordinates) == len(set(warp_coordinates)), f"{name}: overlapping warp events")
        for warp in payload.get("warp_events", []) or []:
            destination = warp["dest_map"]
            dest_id = warp["dest_warp_id"]
            if destination == "MAP_DYNAMIC" or str(dest_id) in {"WARP_ID_DYNAMIC", "WARP_ID_SECRET_BASE"}:
                continue
            other = id_to_name[destination]
            destination_warps = all_maps[other].get("warp_events", []) or []
            dest_id = int(dest_id)
            require(0 <= dest_id < len(destination_warps),
                    f"{name}: warp points beyond {other}'s warp table")
            require(destination_warps[dest_id]["dest_map"] == payload["id"],
                    f"{name}: warp to {other} does not have a reciprocal destination")
        for event in payload.get("object_events", []):
            total_objects += 1
            if event.get("local_id"):
                local_ids.append(event["local_id"])
            flag = event.get("flag", "0")
            if flag.startswith("FLAG_EC_ITEM_"):
                item_flags.append(flag)
                require(flag in flag_values, f"{name}: undefined item flag {flag}")
            graphics = event.get("graphics_id", "")
            match = re.fullmatch(r"OBJ_EVENT_GFX_SPECIES\(([A-Z0-9_]+)\)", graphics)
            if match:
                visible_species.add(match.group(1))
            elif graphics in FIXED_INCLEMENT_SPECIES_GFX:
                visible_species.add(FIXED_INCLEMENT_SPECIES_GFX[graphics])
        require(len(local_ids) == len(set(local_ids)), f"{name}: duplicate local object ids")

    require(len(item_flags) == len(set(item_flags)), "a restored pickup flag is reused")
    numeric_item_flags = [flag_values[name] for name in item_flags]
    require(len(numeric_item_flags) == len(set(numeric_item_flags)), "restored pickup flag values collide")

    wild = load(ROOT / "src" / "data" / "wild_encounters.json")
    wild_maps = {
        entry["map"]
        for group in wild["wild_encounter_groups"]
        if group.get("label") == "gWildMonHeaders"
        for entry in group["encounters"]
    }
    expected_wild = {all_maps[name]["id"] for name in restored - NO_WILD_TABLE}
    require(expected_wild <= wild_maps, f"restored maps missing wild tables: {sorted(expected_wild - wild_maps)}")

    definitions = (ROOT / "src" / "data" / "pokemon" / "legendary_signs.h").read_text()
    restored_map_tokens = {all_maps[name]["id"].removeprefix("MAP_") for name in restored}
    visible_sign_species = {
        species
        for species, map_name in re.findall(r"VISIBLE_SIGN\([^,]+,\s*([A-Z0-9_]+),\s*([A-Z0-9_]+)", definitions)
        if map_name in restored_map_tokens
    }
    require(visible_sign_species <= visible_species,
            f"visible Sign species lack overworld objects: {sorted(visible_sign_species - visible_species)}")
    require({"DIANCIE", "JIRACHI", "MOLTRES", "HEATRAN"} <= visible_species,
            "a restored static sanctuary lacks its legendary object")

    fossil_tunnel = (maps_root / "Route114_FossilManiacsTunnel" / "scripts.inc").read_text()
    require(
        "goto_if_ge VAR_MIRAGE_TOWER_STATE, 2, Route114_FossilManiacsTunnel_EventScript_EndLoad"
        in fossil_tunnel,
        "Sandstrewn Ruins becomes campaign-inaccessible after Mirage Tower collapses",
    )

    cave_1f = all_maps["CaveOfOrigin_1F"]
    cave_map1 = all_maps["CaveOfOrigin_UnusedRubySapphireMap1"]
    cave_map3 = all_maps["CaveOfOrigin_UnusedRubySapphireMap3"]
    require(
        any(
            event.get("graphics_id") == "OBJ_EVENT_GFX_INCLEMENT_CARBINK"
            and event.get("flag") == "FLAG_BADGE08_GET"
            for event in cave_1f["object_events"]
        ),
        "the badge-eight Carbink gate to Diancie's chamber is missing",
    )
    require(
        cave_1f["warp_events"][2]["dest_map"] == "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1"
        and int(cave_map1["warp_events"][0]["dest_warp_id"]) == 2
        and cave_map3["warp_events"][1]["dest_map"] == "MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM",
        "the directed Cave of Origin path to Diancie's chamber is broken",
    )
    cave_layout = layouts[cave_1f["layout"]]
    cave_blocks = struct.unpack(
        f"<{cave_layout['width'] * cave_layout['height']}H",
        (ROOT / cave_layout["blockdata_filepath"]).read_bytes(),
    )
    require(
        cave_blocks[8 * cave_layout["width"] + 5] == 0x333D,
        "Cave of Origin lost the native ladder tile beneath Carbink",
    )

    print(f"PASS: {len(restored)} restored maps are connected to the Hoenn campaign")
    print("PASS: 9 Inclement evolution/form rewards and the Dewford Manor sign are restored")
    print("PASS: Abandoned Ship has exactly one source-backed Spiritomb interaction")
    print(f"PASS: {total_objects} objects include {len(item_flags)} unique progression pickups")
    print(f"PASS: {len(expected_wild)} restored maps have themed wild encounter tables")
    print(f"PASS: {len(visible_sign_species)} Sign objects and 4 static sanctuaries are visible in-world")


if __name__ == "__main__":
    main()
