#!/usr/bin/env python3
"""Verify the authored Emerald Champions campaign encounter distribution.

The distribution is now curated directly in wild_encounters.json.  This gate
must never reconstruct it from an old Inclement snapshot or globally allocate
species into whichever slot happens to be free.
"""

from __future__ import annotations

import argparse
import json
import re
from collections import defaultdict
from pathlib import Path

from verify_emerald_champions_campaign_roster import SpeciesGraph


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "src/data/wild_encounters.json"
SLOT_COUNTS = {
    "land_mons": 12,
    "water_mons": 5,
    "rock_smash_mons": 5,
    "fishing_mons": 10,
    "hidden_mons": 3,
}
SLOT_RATES = {
    "land_mons": (20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1),
    "water_mons": (60, 30, 5, 4, 1),
    "rock_smash_mons": (60, 30, 5, 4, 1),
    "fishing_mons": (70, 30, 60, 20, 20, 40, 40, 15, 4, 1),
}

# Ultra-space leakage is an explicit feature of restored sanctuaries.  Five
# percent is rare without making a targeted catch into a grind.
RESTORED_ULTRA_BEASTS = {
    "MAP_ALTERING_CAVE_B1F": "SPECIES_GUZZLORD",
    "MAP_ASHEN_WOODS": "SPECIES_BUZZWOLE",
    "MAP_DEWFORD_MEADOW": "SPECIES_PHEROMOSA",
    "MAP_EMBER_PATH": "SPECIES_BLACEPHALON",
    "MAP_PETALBURG_WOODS_3": "SPECIES_KARTANA",
    "MAP_SANDSTREWN_RUINS_B1F": "SPECIES_STAKATAKA",
    "MAP_SEASPRAY_CAVE": "SPECIES_NIHILEGO",
}

# First acquisition pass: ordinary wild starters were replaced with species
# that preserve the area's theme.  Evolved starter repetitions in Seafloor
# Cavern are deliberately room-specific rather than one global substitution.
STARTER_REPLACEMENTS = {
    ("MAP_ROUTE101", "land_mons", 4): "SPECIES_PIDGEY",
    ("MAP_ROUTE103", "land_mons", 4): "SPECIES_GROWLITHE",
    ("MAP_ROUTE104", "fishing_mons", 1): "SPECIES_TENTACOOL",
    ("MAP_ROUTE117", "land_mons", 8): "SPECIES_EXEGGCUTE",
    ("MAP_ROUTE117", "land_mons", 9): "SPECIES_PONYTA",
    ("MAP_FIERY_PATH", "land_mons", 9): "SPECIES_HOUNDOUR",
    ("MAP_MAGMA_HIDEOUT_4F", "land_mons", 9): "SPECIES_HEATMOR",
    ("MAP_MAGMA_HIDEOUT_4F", "land_mons", 11): "SPECIES_MAGMAR",
    ("MAP_ROUTE126", "water_mons", 2): "SPECIES_MILOTIC",
    ("MAP_ROUTE126", "water_mons", 3): "SPECIES_GOREBYSS",
    ("MAP_ROUTE126", "water_mons", 4): "SPECIES_HUNTAIL",
    ("MAP_ROUTE128", "fishing_mons", 8): "SPECIES_DRAGALGE",
    ("MAP_ROUTE128", "fishing_mons", 9): "SPECIES_DHELMISE",
    ("MAP_SEAFLOOR_CAVERN_ROOM1", "land_mons", 8): "SPECIES_BARRASKEWDA",
    ("MAP_SEAFLOOR_CAVERN_ROOM1", "land_mons", 10): "SPECIES_GRAPPLOCT",
    ("MAP_SEAFLOOR_CAVERN_ROOM2", "land_mons", 8): "SPECIES_BASCULEGION",
    ("MAP_SEAFLOOR_CAVERN_ROOM2", "land_mons", 10): "SPECIES_DRAGALGE",
    ("MAP_SEAFLOOR_CAVERN_ROOM3", "land_mons", 8): "SPECIES_DHELMISE",
    ("MAP_SEAFLOOR_CAVERN_ROOM3", "land_mons", 10): "SPECIES_TOXAPEX",
    ("MAP_SEAFLOOR_CAVERN_ROOM4", "land_mons", 8): "SPECIES_SHARPEDO",
    ("MAP_SEAFLOOR_CAVERN_ROOM4", "land_mons", 10): "SPECIES_MALAMAR",
    ("MAP_SEAFLOOR_CAVERN_ROOM5", "land_mons", 8): "SPECIES_KINGDRA",
    ("MAP_SEAFLOOR_CAVERN_ROOM5", "land_mons", 10): "SPECIES_GOLISOPOD",
    ("MAP_SEAFLOOR_CAVERN_ROOM6", "land_mons", 8): "SPECIES_BARRASKEWDA",
    ("MAP_SEAFLOOR_CAVERN_ROOM6", "land_mons", 10): "SPECIES_DHELMISE",
    ("MAP_SEAFLOOR_CAVERN_ROOM7", "land_mons", 8): "SPECIES_BASCULEGION",
    ("MAP_SEAFLOOR_CAVERN_ROOM7", "land_mons", 10): "SPECIES_GRAPPLOCT",
    ("MAP_SEAFLOOR_CAVERN_ROOM8", "land_mons", 8): "SPECIES_DRAGALGE",
    ("MAP_SEAFLOOR_CAVERN_ROOM8", "land_mons", 10): "SPECIES_KINGDRA",
}

QUEST_DEPENDENCY_REPLACEMENTS = {
    # Hoopa's visible Sign requires Unown.  Tanoby Ruins is unreachable FRLG
    # data and Mirage Tower can collapse, so permanently reachable Sandstrewn
    # Ruins provides the native Hoenn acquisition at 4%.
    ("MAP_SANDSTREWN_RUINS", "land_mons", 8): "SPECIES_UNOWN",
}

OPENING_MAPS = {
    "MAP_ROUTE101",
    "MAP_ROUTE102",
    "MAP_ROUTE103",
    "MAP_ROUTE104",
    "MAP_PETALBURG_WOODS",
    "MAP_ROUTE116",
    "MAP_RUSTURF_TUNNEL",
}
PRE_BRAWLY_MAPS = OPENING_MAPS | {
    "MAP_GRANITE_CAVE_1F",
    "MAP_GRANITE_CAVE_B1F",
    "MAP_GRANITE_CAVE_B2F",
}

# Each policy row names candidate species and the authored behavior their
# default preset must expose.  One source at four percent or better is enough.
ROLE_POLICY = {
    "opening Intimidate": (OPENING_MAPS, {"SPECIES_SHINX", "SPECIES_POOCHYENA"}, "ABILITY_INTIMIDATE"),
    "opening Fake Out": (OPENING_MAPS, {"SPECIES_BUNEARY", "SPECIES_MEOWTH"}, "MOVE_FAKE_OUT"),
    "opening redirection": (OPENING_MAPS, {"SPECIES_FOONGUS"}, "MOVE_RAGE_POWDER"),
    "opening Trick Room": (OPENING_MAPS, {"SPECIES_RALTS"}, "MOVE_TRICK_ROOM"),
    "opening Tailwind": (OPENING_MAPS, {"SPECIES_SCYTHER"}, "MOVE_TAILWIND"),
    "opening sleep": (OPENING_MAPS, {"SPECIES_SHROOMISH", "SPECIES_FOONGUS"}, "MOVE_SPORE"),
    "pre-Brawly Wide Guard": (PRE_BRAWLY_MAPS, {"SPECIES_ONIX"}, "MOVE_WIDE_GUARD"),
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def hoenn_map_ids() -> set[str]:
    groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
    result = set()
    for group, maps in groups.items():
        if group == "group_order" or "_Frlg" in group:
            continue
        for map_name in maps:
            path = ROOT / "data/maps" / map_name / "map.json"
            if path.exists():
                result.add(json.loads(path.read_text())["id"])
    return result


def encounter_rows() -> list[dict]:
    payload = json.loads(TARGET.read_text())
    group = next(row for row in payload["wild_encounter_groups"] if row["label"] == "gWildMonHeaders")
    allowed = hoenn_map_ids()
    return [row for row in group["encounters"] if row.get("map") in allowed]


def encounter_map() -> dict[str, dict]:
    return {row["map"]: row for row in encounter_rows()}


def species_generations(graph: SpeciesGraph) -> dict[str, int]:
    result = {}
    for generation in range(1, 10):
        source = (ROOT / f"src/data/pokemon/species_info/gen_{generation}_families.h").read_text()
        for species in re.findall(r"\[?(SPECIES_[A-Z0-9_]+)\]?\s*=", source):
            if species in graph.species:
                result.setdefault(graph.find(species), generation)
    return result


def starter_components(graph: SpeciesGraph) -> set[str]:
    source = (ROOT / "src/starter_choose.c").read_text()
    array = re.search(r"static const enum Species sStarterMons.*?\n\};", source, re.S)
    require(array is not None, "regional starter table is missing")
    return {
        graph.find(species)
        for species in re.findall(r"SPECIES_[A-Z0-9_]+", array.group())
        if species in graph.species
    }


def species_rate(entry: dict, species: str) -> int:
    chance = 0
    for method_name, rates in SLOT_RATES.items():
        for index, mon in enumerate(entry.get(method_name, {}).get("mons", [])):
            if mon["species"] == species:
                chance += rates[index]
    return chance


def preset_blocks() -> dict[str, str]:
    source = (ROOT / "src/data/pokemon/emerald_champions_battle_sets.h").read_text()
    starts = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", source))
    return {
        match.group(1): source[match.start(): starts[index + 1].start() if index + 1 < len(starts) else len(source)]
        for index, match in enumerate(starts)
    }


def verify_role_policy(by_map: dict[str, dict]) -> None:
    presets = preset_blocks()
    for label, (maps, candidates, required_token) in ROLE_POLICY.items():
        qualifying = []
        for species in candidates:
            chance = sum(species_rate(by_map[map_name], species) for map_name in maps if map_name in by_map)
            if chance >= 4 and required_token in presets.get(species, ""):
                qualifying.append((species, chance))
        require(qualifying, f"{label} has no >=4% source with {required_token}")


def verify_opening_act_bias(by_map: dict[str, dict], graph: SpeciesGraph) -> tuple[float, float]:
    generations = species_generations(graph)
    common_slots = {
        "land_mons": range(6),
        "water_mons": range(2),
        "rock_smash_mons": range(2),
        "fishing_mons": (0, 1, 2, 5, 6),
    }
    values = []
    for map_name in OPENING_MAPS:
        entry = by_map.get(map_name, {})
        for method_name, indexes in common_slots.items():
            mons = entry.get(method_name, {}).get("mons", [])
            for index in indexes:
                if index >= len(mons) or mons[index]["species"] not in graph.species:
                    continue
                generation = generations.get(graph.find(mons[index]["species"]))
                if generation is not None:
                    values.append(generation)
    require(values, "opening act has no measurable common encounter slots")
    old_share = sum(generation <= 3 for generation in values) / len(values)
    modern_share = sum(generation >= 7 for generation in values) / len(values)
    require(old_share >= 0.60, f"opening common slots lost the Gen 1-3 bias: {old_share:.1%}")
    require(modern_share <= 0.10, f"opening common slots overuse Gen 7-9 families: {modern_share:.1%}")
    return old_share, modern_share


def check() -> None:
    rows = encounter_rows()
    by_map = {row["map"]: row for row in rows}
    graph = SpeciesGraph()

    require(len(rows) == 145, f"Hoenn campaign encounter-header count drifted: {len(rows)}")
    require(len(by_map) == 137, f"Hoenn campaign wild-map count drifted: {len(by_map)}")
    for entry in rows:
        map_name = entry["map"]
        for method_name, method in entry.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            require(method_name in SLOT_COUNTS, f"{map_name}: unknown encounter method {method_name}")
            require(len(method.get("mons", [])) == SLOT_COUNTS[method_name],
                    f"{map_name} {method_name}: invalid slot count")
            require(0 < method.get("encounter_rate", 0) <= 100,
                    f"{map_name} {method_name}: invalid encounter rate {method.get('encounter_rate')}")

    for (map_name, method_name, slot), species in STARTER_REPLACEMENTS.items():
        require(by_map[map_name][method_name]["mons"][slot]["species"] == species,
                f"{map_name} {method_name}[{slot}] lost {species}")
    for (map_name, method_name, slot), species in QUEST_DEPENDENCY_REPLACEMENTS.items():
        require(by_map[map_name][method_name]["mons"][slot]["species"] == species,
                f"{map_name} {method_name}[{slot}] lost quest dependency {species}")

    starter_roots = starter_components(graph)
    leaked_starters = []
    for map_name, entry in by_map.items():
        for method_name, method in entry.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            for slot, mon in enumerate(method.get("mons", [])):
                if mon["species"] in graph.species and graph.find(mon["species"]) in starter_roots:
                    leaked_starters.append(f"{map_name}:{method_name}[{slot}]={mon['species']}")
    require(not leaked_starters, "starter families remain ordinary Hoenn wilds: " + ", ".join(leaked_starters))

    for map_name, species in RESTORED_ULTRA_BEASTS.items():
        slots = by_map[map_name]["land_mons"]["mons"]
        require(slots[6]["species"] == species,
                f"{map_name} lost its no-grind 5% Ultra Beast {species}")

    verify_role_policy(by_map)
    old_share, modern_share = verify_opening_act_bias(by_map, graph)
    species = {
        mon["species"]
        for entry in rows
        for method_name, method in entry.items()
        if method_name.endswith("_mons") and isinstance(method, dict)
        for mon in method.get("mons", [])
    }
    print(f"PASS: {len(rows)} headers on {len(by_map)} Hoenn wild maps expose {len(species)} unique species/forms")
    print("PASS: no ordinary Hoenn wild table contains a regional starter family")
    print(f"PASS: opening common slots are {old_share:.1%} Gen 1-3 and {modern_share:.1%} Gen 7-9")
    print(f"PASS: {len(ROLE_POLICY)} early doubles-role availability contracts hold")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    require(not args.write, "wild encounters are directly curated; the stale snapshot writer is retired")
    check()


if __name__ == "__main__":
    main()
