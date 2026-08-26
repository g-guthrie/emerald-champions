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
CURATED_SPECIES_CONSTANTS = ROOT / "include/constants/verdant_gen9_species.h"
BASE_STATS = ROOT / "src/data/pokemon/base_stats.h"
MIN_ORDINARY_WILD_LEGEND_CATCH_RATE = 45

# These legacy legendary and mythical species remain in ordinary random wild
# tables.  Match the curated Gen 9 access policy: finding a team-building tool
# should not be followed by a catch-rate-3 ball grind.  Darkrai and Marshadow
# are deliberately absent from ordinary wild tables so their League uses stay
# protected; dedicated postgame access can retain their native catch rates.
ORDINARY_WILD_LEGENDS = (
    "SPECIES_KUBFU",
    "SPECIES_TYPE_NULL",
    "SPECIES_SUICUNE",
    "SPECIES_HEATRAN",
    "SPECIES_MANAPHY",
    "SPECIES_TERRAKION",
    "SPECIES_KELDEO",
    "SPECIES_TAPU_FINI",
)
WITHHELD_ORDINARY_WILD_SPECIES = {
    "SPECIES_DARKRAI",
    "SPECIES_MARSHADOW",
}

# Eight-percent showcase slots are common enough to support team building
# without hunting.  On the specifically consolidated routes, the matching
# four-percent tail slot repeats the showcase and raises its effective rate to
# twelve percent rather than hiding a separate desirable species at four.
LAND_UPGRADES = {
    "MAP_ROUTE101": {8: "SPECIES_DREEPY", 9: "SPECIES_LARVESTA"},
    "MAP_ROUTE102": {8: "SPECIES_HATENNA", 9: "SPECIES_INDEEDEE"},
    "MAP_ROUTE103": {8: "SPECIES_TOXEL", 9: "SPECIES_ROTOM"},
    "MAP_ROUTE104": {
        8: "SPECIES_MAREANIE",
        9: "SPECIES_WIMPOD",
        10: "SPECIES_MAREANIE",
        11: "SPECIES_WIMPOD",
    },
    "MAP_PETALBURG_WOODS": {8: "SPECIES_IMPIDIMP", 9: "SPECIES_FOONGUS"},
    "MAP_ROUTE116": {8: "SPECIES_ROOKIDEE", 9: "SPECIES_DREEPY"},
    "MAP_RUSTURF_TUNNEL": {8: "SPECIES_LARVESTA", 9: "SPECIES_BAGON"},
    "MAP_GRANITE_CAVE_STEVENS_ROOM": {8: "SPECIES_DREEPY", 9: "SPECIES_LARVESTA"},
    "MAP_ROUTE110": {
        # Timmy's source-closed Route 110 battle explicitly uses four local
        # catches, including Rotom and Stunky.  Preserve all four tail slots;
        # Porygon and Klefki remain available from Mauville and Route 113.
        8: "SPECIES_ROTOM",
        9: "SPECIES_TOXEL",
        10: "SPECIES_PACHIRISU",
        11: "SPECIES_STUNKY",
    },
    "MAP_ROUTE112": {8: "SPECIES_HAWLUCHA", 9: "SPECIES_KUBFU"},
    "MAP_FIERY_PATH": {8: "SPECIES_LARVESTA", 9: "SPECIES_CHARMANDER"},
    "MAP_JAGGED_PASS": {8: "SPECIES_BAGON", 9: "SPECIES_DEINO"},
    "MAP_ROUTE117": {8: "SPECIES_GROOKEY", 9: "SPECIES_SCORBUNNY"},
    "MAP_ROUTE118": {8: "SPECIES_TYPE_NULL", 9: "SPECIES_ZORUA"},
    "MAP_ROUTE119": {8: "SPECIES_DREEPY", 9: "SPECIES_LARVESTA"},
    "MAP_ROUTE120": {8: "SPECIES_MIMIKYU", 9: "SPECIES_HONEDGE"},
    "MAP_ROUTE121": {
        8: "SPECIES_ZOROARK",
        9: "SPECIES_SPIRITOMB",
        10: "SPECIES_ZOROARK",
        11: "SPECIES_SPIRITOMB",
    },
    # Steven's Cave is postgame-only.  Roaring Moon moves to the Waterfall
    # section of Meteor Falls so the curated family is usable before the
    # League; the postgame table consolidates its 10% and 4% Metagross slots.
    "MAP_METEOR_FALLS_B1F_2R": {4: "SPECIES_ROARING_MOON"},
    "MAP_METEOR_FALLS_STEVENS_CAVE": {4: "SPECIES_METAGROSS"},
    "MAP_VICTORY_ROAD_1F": {8: "SPECIES_METAGROSS", 9: "SPECIES_KOMMO_O", 10: "SPECIES_DRAGAPULT", 11: "SPECIES_VOLCARONA"},
    "MAP_VICTORY_ROAD_B1F": {8: "SPECIES_AEGISLASH", 9: "SPECIES_TERRAKION", 10: "SPECIES_METAGROSS", 11: "SPECIES_KOMMO_O"},
    "MAP_VICTORY_ROAD_B2F": {8: "SPECIES_HYDREIGON", 9: "SPECIES_VOLCARONA", 10: "SPECIES_DRAGAPULT", 11: "SPECIES_AEGISLASH"},
}

OCEAN_UPGRADES = {
    "MAP_ROUTE124": {
        "water_mons": {2: "SPECIES_MILOTIC", 3: "SPECIES_MILOTIC"},
        "fishing_mons": {7: "SPECIES_KINGDRA", 8: "SPECIES_MANAPHY", 9: "SPECIES_MANAPHY"},
    },
    "MAP_ROUTE125": {
        "water_mons": {2: "SPECIES_LAPRAS", 3: "SPECIES_LAPRAS"},
        "fishing_mons": {7: "SPECIES_KINGDRA", 8: "SPECIES_SUICUNE", 9: "SPECIES_SUICUNE"},
    },
    "MAP_ROUTE126": {
        "water_mons": {2: "SPECIES_PRIMARINA", 3: "SPECIES_PRIMARINA"},
        "fishing_mons": {7: "SPECIES_KINGDRA", 8: "SPECIES_TAPU_FINI", 9: "SPECIES_TAPU_FINI"},
    },
    "MAP_ROUTE127": {
        "water_mons": {2: "SPECIES_GOLISOPOD", 3: "SPECIES_GOLISOPOD"},
        "fishing_mons": {7: "SPECIES_DHELMISE", 8: "SPECIES_KELDEO", 9: "SPECIES_KELDEO"},
    },
    "MAP_ROUTE128": {
        "water_mons": {2: "SPECIES_KINGDRA", 3: "SPECIES_KINGDRA"},
        "fishing_mons": {7: "SPECIES_DRAGALGE", 8: "SPECIES_PRIMARINA", 9: "SPECIES_PRIMARINA"},
    },
}

MAGMA_RARE_SLOTS = {
    8: "SPECIES_VOLCARONA",
    9: "SPECIES_MAGMORTAR",
    10: "SPECIES_VOLCARONA",
    11: "SPECIES_MAGMORTAR",
}

MAGMA_4F_RARE_SLOTS = {
    8: "SPECIES_HEATRAN",
    9: "SPECIES_EMBOAR",
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
    10: "SPECIES_GRENINJA",
    11: "SPECIES_CROBAT",
}

SEAFLOOR_WATER = {
    0: "SPECIES_KINGDRA",
    1: "SPECIES_MILOTIC",
    2: "SPECIES_LAPRAS",
    3: "SPECIES_LAPRAS",
}

SEAFLOOR_FISHING = {
    8: "SPECIES_MANAPHY",
    9: "SPECIES_MANAPHY",
}

# Once the Stone Badge shop begins selling Honey, this pool supplies a legal
# level-up family for the first post-badge Mega showcase.  Keep the encounter
# in its unevolved form: Weedle reaches Beedrill naturally at level 10, before
# Steven grants the Bracelet at the cap-20 Granite Cave milestone.
EARLY_MEGA_HONEY = {
    0: "SPECIES_AUDINO",
    1: "SPECIES_WEEDLE",
    2: "SPECIES_CATERPIE",
    3: "SPECIES_WEEDLE",
    4: "SPECIES_CATERPIE",
    5: "SPECIES_CATERPIE",
}

EARLY_WOODS_HONEY = {
    0: "SPECIES_AUDINO",
    1: "SPECIES_PIKACHU",
    2: "SPECIES_PIDGEY",
    3: "SPECIES_PIKACHU",
    4: "SPECIES_PIDGEY",
    5: "SPECIES_PIDGEY",
}

# Cascoon and Silcoon are already evolved from Wurmple and cannot exist below
# level 7 in Verdant's own evolution table.
EARLY_WOODS_LAND_MINIMUMS = {10: 7, 11: 7}


def all_encounters(data: dict) -> list[dict]:
    return [
        encounter
        for group in data["wild_encounter_groups"]
        for encounter in group["encounters"]
        if "map" in encounter
    ]


def set_slots(encounter: dict, method: str, changes: dict[int, str]) -> int:
    table = encounter.get(method)
    if not table:
        return 0
    slots = table["mons"]
    changed = 0
    for index, species in changes.items():
        if index >= len(slots):
            raise ValueError(f"{encounter['map']} {method} has no slot {index}")
        if slots[index]["species"] != species:
            slots[index]["species"] = species
            changed += 1
    return changed


def update_ordinary_wild_legend_catch_rates(text: str) -> tuple[str, int]:
    changed = 0
    for species in ORDINARY_WILD_LEGENDS:
        pattern = re.compile(
            rf"(^\s*\[{re.escape(species)}\]\s*=\s*\{{"
            rf"(?:(?!^\s*\[SPECIES_).)*?^\s*\.catchRate\s*=\s*)\d+",
            re.M | re.S,
        )
        match = pattern.search(text)
        if not match:
            raise ValueError(f"missing base-stat catch rate for {species}")
        old = match.group(0)
        replacement = match.group(1) + str(MIN_ORDINARY_WILD_LEGEND_CATCH_RATE)
        if old != replacement:
            text = text[: match.start()] + replacement + text[match.end() :]
            changed += 1
    return text, changed


def catch_rate_for(text: str, species: str) -> int | None:
    match = re.search(
        rf"^\s*\[{re.escape(species)}\]\s*=\s*\{{"
        rf"(?:(?!^\s*\[SPECIES_).)*?^\s*\.catchRate\s*=\s*(\d+)",
        text,
        re.M | re.S,
    )
    return int(match.group(1)) if match else None


def expected_changes(data: dict) -> list[tuple[str, str, int, str]]:
    output = []
    for encounter in all_encounters(data):
        map_id = encounter["map"]
        if map_id == "MAP_PETALBURG_WOODS":
            for index, species in EARLY_WOODS_HONEY.items():
                output.append((map_id, "honey_mons", index, species))
        if map_id == "MAP_PETALBURG_WOODS_2":
            for index, species in EARLY_MEGA_HONEY.items():
                output.append((map_id, "honey_mons", index, species))
        for index, species in LAND_UPGRADES.get(map_id, {}).items():
            output.append((map_id, "land_mons", index, species))
        for method, changes in OCEAN_UPGRADES.get(map_id, {}).items():
            for index, species in changes.items():
                output.append((map_id, method, index, species))
        if map_id.startswith("MAP_MAGMA_HIDEOUT_"):
            if encounter.get("land_mons"):
                magma_slots = MAGMA_4F_RARE_SLOTS if map_id == "MAP_MAGMA_HIDEOUT_4F" else MAGMA_RARE_SLOTS
                for index, species in magma_slots.items():
                    output.append((map_id, "land_mons", index, species))
        if map_id.startswith("MAP_SEAFLOOR_CAVERN_"):
            if encounter.get("land_mons"):
                for index, species in SEAFLOOR_LAND.items():
                    output.append((map_id, "land_mons", index, species))
            if encounter.get("water_mons"):
                for index, species in SEAFLOOR_WATER.items():
                    output.append((map_id, "water_mons", index, species))
            if encounter.get("fishing_mons"):
                output.extend((map_id, "fishing_mons", index, species) for index, species in SEAFLOOR_FISHING.items())
    return output


def apply() -> None:
    data = json.loads(ENCOUNTERS.read_text())
    by_map = {encounter["map"]: encounter for encounter in all_encounters(data)}
    changed_slots = set_slots(by_map["MAP_PETALBURG_WOODS"], "honey_mons", EARLY_WOODS_HONEY)
    for index, minimum in EARLY_WOODS_LAND_MINIMUMS.items():
        mon = by_map["MAP_PETALBURG_WOODS"]["land_mons"]["mons"][index]
        if mon["min_level"] != minimum:
            mon["min_level"] = minimum
            changed_slots += 1
    changed_slots += set_slots(by_map["MAP_PETALBURG_WOODS_2"], "honey_mons", EARLY_MEGA_HONEY)
    for map_id, changes in LAND_UPGRADES.items():
        changed_slots += set_slots(by_map[map_id], "land_mons", changes)
    for map_id, methods in OCEAN_UPGRADES.items():
        for method, changes in methods.items():
            changed_slots += set_slots(by_map[map_id], method, changes)
    for encounter in by_map.values():
        map_id = encounter["map"]
        if map_id.startswith("MAP_MAGMA_HIDEOUT_"):
            magma_slots = MAGMA_4F_RARE_SLOTS if map_id == "MAP_MAGMA_HIDEOUT_4F" else MAGMA_RARE_SLOTS
            changed_slots += set_slots(encounter, "land_mons", magma_slots)
        if map_id.startswith("MAP_SEAFLOOR_CAVERN_"):
            changed_slots += set_slots(encounter, "land_mons", SEAFLOOR_LAND)
            changed_slots += set_slots(encounter, "water_mons", SEAFLOOR_WATER)
            changed_slots += set_slots(encounter, "fishing_mons", SEAFLOOR_FISHING)
    ENCOUNTERS.write_text(json.dumps(data, indent=2) + "\n")
    base_stats, catch_rate_changes = update_ordinary_wild_legend_catch_rates(BASE_STATS.read_text())
    BASE_STATS.write_text(base_stats)
    print(
        f"restored {changed_slots} encounter values and "
        f"{catch_rate_changes} ordinary-wild catch rates; "
        f"{len(expected_changes(data))} canonical slots guarded"
    )


def check() -> None:
    data = json.loads(ENCOUNTERS.read_text())
    by_map = {encounter["map"]: encounter for encounter in all_encounters(data)}
    defined = set(re.findall(
        r"^#define\s+(SPECIES_[A-Z0-9_]+)\b",
        SPECIES_CONSTANTS.read_text() + "\n" + CURATED_SPECIES_CONSTANTS.read_text(),
        re.M,
    ))
    base_stats = BASE_STATS.read_text()
    problems = []
    expected_land_rates = [13, 13, 10, 10, 10, 10, 5, 5, 8, 8, 4, 4]
    land_field = next(field for field in data["wild_encounter_groups"][0]["fields"] if field["type"] == "land_mons")
    if land_field["encounter_rates"] != expected_land_rates:
        problems.append(f"land encounter rates drifted: {land_field['encounter_rates']}")
    changes = expected_changes(data)
    for map_id, method, index, species in changes:
        actual = by_map[map_id][method]["mons"][index]["species"]
        if actual != species:
            problems.append(f"{map_id} {method}[{index}]: {actual} != {species}")
        if species not in defined:
            problems.append(f"unknown species constant: {species}")
    for index, minimum in EARLY_WOODS_LAND_MINIMUMS.items():
        mon = by_map["MAP_PETALBURG_WOODS"]["land_mons"]["mons"][index]
        if mon["min_level"] < minimum:
            problems.append(
                f"MAP_PETALBURG_WOODS land_mons[{index}] {mon['species']} "
                f"appears below legal level {minimum}"
            )
    ordinary_wild_species = {
        mon["species"]
        for encounter in all_encounters(data)
        for method in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons")
        for mon in encounter.get(method, {}).get("mons", [])
    }
    for species in ORDINARY_WILD_LEGENDS:
        rate = catch_rate_for(base_stats, species)
        if species not in ordinary_wild_species:
            problems.append(f"ordinary-wild catch policy species is no longer wild: {species}")
        if rate != MIN_ORDINARY_WILD_LEGEND_CATCH_RATE:
            problems.append(
                f"{species} ordinary-wild catch rate is {rate}, "
                f"expected {MIN_ORDINARY_WILD_LEGEND_CATCH_RATE}"
            )
    leaked = sorted(WITHHELD_ORDINARY_WILD_SPECIES & ordinary_wild_species)
    if leaked:
        problems.append("withheld League species leaked into ordinary wild tables: " + ", ".join(leaked))
    for encounter in by_map.values():
        for method, expected in (("land_mons", 12), ("water_mons", 4), ("rock_smash_mons", 4), ("fishing_mons", 10), ("honey_mons", 6)):
            if encounter.get(method) and len(encounter[method]["mons"]) != expected:
                problems.append(f"{encounter['map']} {method}: unsafe slot count")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(
        f"PASS: {len(changes)} upgraded encounter slots, "
        f"{len(ORDINARY_WILD_LEGENDS)} low-grind legend catch rates, "
        "and native table lengths validated"
    )


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
