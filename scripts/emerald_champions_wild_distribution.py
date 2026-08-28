#!/usr/bin/env python3
"""Merge the preserved curated Hoenn distribution into the modern engine."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE_COMMIT = "33202c162ebc34a1dbe2000acd26b0720baa109d"
TARGET = ROOT / "src" / "data" / "wild_encounters.json"
TARGET_COUNTS = {
    "land_mons": 12,
    "water_mons": 5,
    "rock_smash_mons": 5,
    "fishing_mons": 10,
}
NORMALIZATION = {
    "_ALOLAN": "_ALOLA",
    "_GALARIAN": "_GALAR",
    "_HISUIAN": "_HISUI",
    "_EAST_SEA": "_EAST",
    "_BLUE_FLOWER": "_BLUE",
    "_ORANGE_FLOWER": "_ORANGE",
    "_WHITE_FLOWER": "_WHITE",
    "_YELLOW_FLOWER": "_YELLOW",
}
BESPOKE_EXCLUSIONS = {
    "SPECIES_ROTOM",
    "SPECIES_GROUDON",
    "SPECIES_KYOGRE",
    "SPECIES_RAYQUAZA",
    "SPECIES_DEOXYS",
    "SPECIES_MEW",
    "SPECIES_LATIAS",
    "SPECIES_LATIOS",
    "SPECIES_REGIROCK",
    "SPECIES_REGICE",
    "SPECIES_REGISTEEL",
}


def source_json() -> dict:
    result = subprocess.run(
        ["git", "show", f"{SOURCE_COMMIT}:src/data/wild_encounters.json"],
        cwd=ROOT,
        check=True,
        stdout=subprocess.PIPE,
    )
    return json.loads(result.stdout)


def species_constants() -> set[str]:
    return set(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", (ROOT / "include" / "constants" / "species.h").read_text()))


def normalize_species(species: str, constants: set[str]) -> str:
    if species in constants:
        return species
    normalized = species
    for old, new in NORMALIZATION.items():
        normalized = normalized.replace(old, new)
    if normalized not in constants:
        raise ValueError(f"missing modern species constant for {species} -> {normalized}")
    return normalized


def normalize_method(method: dict, target_count: int, constants: set[str]) -> dict:
    result = {"encounter_rate": method["encounter_rate"], "mons": []}
    for mon in method["mons"]:
        species = normalize_species(mon["species"], constants)
        if species in BESPOKE_EXCLUSIONS:
            continue
        result["mons"].append({**mon, "species": species})
    if not result["mons"]:
        raise ValueError("encounter method became empty after bespoke exclusions")
    result["mons"] = result["mons"][:target_count]
    while len(result["mons"]) < target_count:
        result["mons"].append(dict(result["mons"][-1]))
    return result


def normalize_hidden(method: dict, constants: set[str]) -> dict:
    source = method["mons"]
    picks = [source[0], source[len(source) // 2], source[-1]]
    return {
        "encounter_rate": method["encounter_rate"],
        "mons": [
            {**mon, "species": normalize_species(mon["species"], constants)}
            for mon in picks
            if normalize_species(mon["species"], constants) not in BESPOKE_EXCLUSIONS
        ],
    }


def encounter_map(payload: dict) -> dict[str, dict]:
    return {
        entry["map"]: entry
        for group in payload["wild_encounter_groups"]
        for entry in group["encounters"]
        if "map" in entry
    }


def write() -> None:
    modern = json.loads(TARGET.read_text())
    preserved = source_json()
    modern_by_map = encounter_map(modern)
    preserved_by_map = encounter_map(preserved)
    constants = species_constants()

    for map_name, old_entry in preserved_by_map.items():
        if map_name not in modern_by_map:
            continue
        target = modern_by_map[map_name]
        for field, count in TARGET_COUNTS.items():
            if field in old_entry:
                target[field] = normalize_method(old_entry[field], count, constants)
            else:
                target.pop(field, None)
        target.pop("hidden_mons", None)
        if "honey_mons" in old_entry:
            hidden = normalize_hidden(old_entry["honey_mons"], constants)
            if hidden["mons"]:
                while len(hidden["mons"]) < 3:
                    hidden["mons"].append(dict(hidden["mons"][-1]))
                target["hidden_mons"] = hidden

    TARGET.write_text(json.dumps(modern, indent=2) + "\n")


def check() -> None:
    payload = json.loads(TARGET.read_text())
    by_map = encounter_map(payload)
    species = {
        mon["species"]
        for entry in by_map.values()
        for field in (*TARGET_COUNTS, "hidden_mons")
        for mon in entry.get(field, {}).get("mons", [])
    }
    for map_name in ("MAP_ROUTE101", "MAP_ROUTE102", "MAP_ROUTE103", "MAP_PETALBURG_WOODS", "MAP_GRANITE_CAVE_1F"):
        if map_name not in by_map:
            raise ValueError(f"missing early-game table {map_name}")
    required_early = {"SPECIES_SPRIGATITO", "SPECIES_DREEPY", "SPECIES_SCYTHER", "SPECIES_AXEW", "SPECIES_FUECOCO"}
    early_species = {
        mon["species"]
        for map_name in ("MAP_ROUTE101", "MAP_ROUTE102", "MAP_ROUTE103", "MAP_ROUTE104", "MAP_PETALBURG_WOODS", "MAP_GRANITE_CAVE_1F")
        for field in (*TARGET_COUNTS, "hidden_mons")
        for mon in by_map[map_name].get(field, {}).get("mons", [])
    }
    missing_early = required_early - early_species
    if missing_early:
        raise ValueError(f"curated early identity drifted: {sorted(missing_early)}")
    duplicate_bespoke = BESPOKE_EXCLUSIONS & species
    if duplicate_bespoke:
        raise ValueError(f"bespoke species leaked into ordinary wild tables: {sorted(duplicate_bespoke)}")
    print(f"PASS: curated modern Hoenn tables expose {len(species)} unique wild species")
    print(f"PASS: early routes expose {len(early_species)} unique species including rare competitive anchors")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    if args.write:
        write()
    check()


if __name__ == "__main__":
    main()
