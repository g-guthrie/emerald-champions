#!/usr/bin/env python3
"""Make Verdant's progression encounter pools consistently exciting."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ENCOUNTERS = ROOT / "src/data/wild_encounters.json"
SPECIES_CONSTANTS = ROOT / "include/constants/species.h"

# Four-percent land slots: accessible enough to discover naturally, rare enough
# to remain exciting. Existing route identities and common encounters stay put.
LAND_UPGRADES = {
    "MAP_ROUTE101": {8: "SPECIES_DREEPY", 9: "SPECIES_LARVESTA"},
    "MAP_ROUTE102": {8: "SPECIES_HATENNA", 9: "SPECIES_INDEEDEE"},
    "MAP_ROUTE103": {8: "SPECIES_TOXEL", 9: "SPECIES_ROTOM"},
    "MAP_PETALBURG_WOODS": {8: "SPECIES_IMPIDIMP", 9: "SPECIES_FOONGUS"},
    "MAP_ROUTE116": {8: "SPECIES_ROOKIDEE", 9: "SPECIES_DREEPY"},
    "MAP_RUSTURF_TUNNEL": {8: "SPECIES_LARVESTA", 9: "SPECIES_BAGON"},
    "MAP_GRANITE_CAVE_STEVENS_ROOM": {8: "SPECIES_DREEPY", 9: "SPECIES_LARVESTA"},
    "MAP_ROUTE110": {8: "SPECIES_ROTOM", 9: "SPECIES_TOXEL"},
    "MAP_ROUTE112": {8: "SPECIES_HAWLUCHA", 9: "SPECIES_KUBFU"},
    "MAP_FIERY_PATH": {8: "SPECIES_LARVESTA", 9: "SPECIES_CHARMANDER"},
    "MAP_JAGGED_PASS": {8: "SPECIES_BAGON", 9: "SPECIES_DEINO"},
    "MAP_ROUTE117": {8: "SPECIES_GROOKEY", 9: "SPECIES_SCORBUNNY"},
    "MAP_ROUTE118": {8: "SPECIES_TYPE_NULL", 9: "SPECIES_ZORUA"},
    "MAP_ROUTE119": {8: "SPECIES_DREEPY", 9: "SPECIES_LARVESTA"},
    "MAP_ROUTE120": {8: "SPECIES_MIMIKYU", 9: "SPECIES_HONEDGE"},
    "MAP_ROUTE121": {8: "SPECIES_DARKRAI", 9: "SPECIES_MARSHADOW"},
    "MAP_VICTORY_ROAD_1F": {8: "SPECIES_METAGROSS", 9: "SPECIES_KOMMO_O", 10: "SPECIES_DRAGAPULT", 11: "SPECIES_VOLCARONA"},
    "MAP_VICTORY_ROAD_B1F": {8: "SPECIES_AEGISLASH", 9: "SPECIES_TERRAKION", 10: "SPECIES_METAGROSS", 11: "SPECIES_KOMMO_O"},
    "MAP_VICTORY_ROAD_B2F": {8: "SPECIES_HYDREIGON", 9: "SPECIES_VOLCARONA", 10: "SPECIES_DRAGAPULT", 11: "SPECIES_AEGISLASH"},
}

OCEAN_UPGRADES = {
    "MAP_ROUTE124": {
        "water_mons": {2: "SPECIES_MILOTIC", 3: "SPECIES_LAPRAS"},
        "fishing_mons": {7: "SPECIES_KINGDRA", 8: "SPECIES_PRIMARINA", 9: "SPECIES_MANAPHY"},
    },
    "MAP_ROUTE125": {
        "water_mons": {2: "SPECIES_DHELMISE", 3: "SPECIES_LAPRAS"},
        "fishing_mons": {7: "SPECIES_KINGDRA", 8: "SPECIES_SUICUNE", 9: "SPECIES_KELDEO"},
    },
    "MAP_ROUTE126": {
        "water_mons": {2: "SPECIES_PRIMARINA", 3: "SPECIES_MILOTIC"},
        "fishing_mons": {7: "SPECIES_KINGDRA", 8: "SPECIES_TAPU_FINI", 9: "SPECIES_MANAPHY"},
    },
    "MAP_ROUTE127": {
        "water_mons": {2: "SPECIES_GOLISOPOD", 3: "SPECIES_DRAGALGE"},
        "fishing_mons": {7: "SPECIES_DHELMISE", 8: "SPECIES_TAPU_FINI", 9: "SPECIES_KELDEO"},
    },
    "MAP_ROUTE128": {
        "water_mons": {2: "SPECIES_KINGDRA", 3: "SPECIES_MILOTIC"},
        "fishing_mons": {7: "SPECIES_PRIMARINA", 8: "SPECIES_SUICUNE", 9: "SPECIES_MANAPHY"},
    },
}

MAGMA_RARE_SLOTS = {
    8: "SPECIES_VOLCARONA",
    9: "SPECIES_MAGMORTAR",
    10: "SPECIES_HEATRAN",
    11: "SPECIES_EMBOAR",
}

SEAFLOOR_LAND = {
    0: "SPECIES_DRAGALGE",
    1: "SPECIES_GOLISOPOD",
    4: "SPECIES_DHELMISE",
    6: "SPECIES_MALAMAR",
    8: "SPECIES_GRENINJA",
    9: "SPECIES_CROBAT",
    10: "SPECIES_TAPU_FINI",
    11: "SPECIES_SUICUNE",
}


def all_encounters(data: dict) -> list[dict]:
    return [
        encounter
        for group in data["wild_encounter_groups"]
        for encounter in group["encounters"]
        if "map" in encounter
    ]


def set_slots(encounter: dict, method: str, changes: dict[int, str]) -> None:
    table = encounter.get(method)
    if not table:
        return
    slots = table["mons"]
    for index, species in changes.items():
        if index >= len(slots):
            raise ValueError(f"{encounter['map']} {method} has no slot {index}")
        slots[index]["species"] = species


def expected_changes(data: dict) -> list[tuple[str, str, int, str]]:
    output = []
    for encounter in all_encounters(data):
        map_id = encounter["map"]
        for index, species in LAND_UPGRADES.get(map_id, {}).items():
            output.append((map_id, "land_mons", index, species))
        for method, changes in OCEAN_UPGRADES.get(map_id, {}).items():
            for index, species in changes.items():
                output.append((map_id, method, index, species))
        if map_id.startswith("MAP_MAGMA_HIDEOUT_"):
            if encounter.get("land_mons"):
                for index, species in MAGMA_RARE_SLOTS.items():
                    output.append((map_id, "land_mons", index, species))
        if map_id.startswith("MAP_SEAFLOOR_CAVERN_"):
            if encounter.get("land_mons"):
                for index, species in SEAFLOOR_LAND.items():
                    output.append((map_id, "land_mons", index, species))
            if encounter.get("water_mons"):
                for index, species in enumerate(("SPECIES_KINGDRA", "SPECIES_MILOTIC", "SPECIES_LAPRAS", "SPECIES_TAPU_FINI")):
                    output.append((map_id, "water_mons", index, species))
            if encounter.get("fishing_mons"):
                output.extend((map_id, "fishing_mons", index, species) for index, species in {8: "SPECIES_MANAPHY", 9: "SPECIES_KELDEO"}.items())
    return output


def apply() -> None:
    data = json.loads(ENCOUNTERS.read_text())
    by_map = {encounter["map"]: encounter for encounter in all_encounters(data)}
    for map_id, changes in LAND_UPGRADES.items():
        set_slots(by_map[map_id], "land_mons", changes)
    for map_id, methods in OCEAN_UPGRADES.items():
        for method, changes in methods.items():
            set_slots(by_map[map_id], method, changes)
    for encounter in by_map.values():
        map_id = encounter["map"]
        if map_id.startswith("MAP_MAGMA_HIDEOUT_"):
            set_slots(encounter, "land_mons", MAGMA_RARE_SLOTS)
        if map_id.startswith("MAP_SEAFLOOR_CAVERN_"):
            set_slots(encounter, "land_mons", SEAFLOOR_LAND)
            set_slots(encounter, "water_mons", dict(enumerate(("SPECIES_KINGDRA", "SPECIES_MILOTIC", "SPECIES_LAPRAS", "SPECIES_TAPU_FINI"))))
            set_slots(encounter, "fishing_mons", {8: "SPECIES_MANAPHY", 9: "SPECIES_KELDEO"})
    ENCOUNTERS.write_text(json.dumps(data, indent=2) + "\n")
    print(f"updated {len(expected_changes(data))} encounter slots")


def check() -> None:
    data = json.loads(ENCOUNTERS.read_text())
    by_map = {encounter["map"]: encounter for encounter in all_encounters(data)}
    defined = set(re.findall(r"^#define\s+(SPECIES_[A-Z0-9_]+)\b", SPECIES_CONSTANTS.read_text(), re.M))
    problems = []
    changes = expected_changes(data)
    for map_id, method, index, species in changes:
        actual = by_map[map_id][method]["mons"][index]["species"]
        if actual != species:
            problems.append(f"{map_id} {method}[{index}]: {actual} != {species}")
        if species not in defined:
            problems.append(f"unknown species constant: {species}")
    for encounter in by_map.values():
        for method, expected in (("land_mons", 12), ("water_mons", 4), ("rock_smash_mons", 4), ("fishing_mons", 10), ("honey_mons", 6)):
            if encounter.get(method) and len(encounter[method]["mons"]) != expected:
                problems.append(f"{encounter['map']} {method}: unsafe slot count")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(f"PASS: {len(changes)} upgraded encounter slots validated with native table lengths")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.apply and not args.check:
        parser.error("choose --apply or --check")
    if args.apply:
        apply()
    if args.check:
        check()


if __name__ == "__main__":
    main()
