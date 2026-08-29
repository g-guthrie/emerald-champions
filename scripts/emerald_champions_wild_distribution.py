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
    "_POKE_BALL": "_POKEBALL",
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

# These Inclement areas were intentionally restored to Emerald Champions after
# the modern-engine migration.  Their old encounter tables are still the best
# thematic starting point, but the modern engine requires different slot
# counts and uses hidden encounters in place of Inclement's honey field.
RESTORED_ENCOUNTER_MAPS = {
    "MAP_ALTERING_CAVE_1F",
    "MAP_ALTERING_CAVE_B1F",
    "MAP_ASHEN_WOODS",
    "MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM",
    "MAP_DEWFORD_MANOR_1F",
    "MAP_DEWFORD_MEADOW",
    "MAP_EMBER_PATH",
    "MAP_MIRAGE_TOWER_B1F",
    "MAP_PETALBURG_WOODS_2",
    "MAP_PETALBURG_WOODS_3",
    "MAP_ROUTE111_RUINS_EXTERIOR",
    "MAP_SANDSTREWN_RUINS",
    "MAP_SANDSTREWN_RUINS_2F",
    "MAP_SANDSTREWN_RUINS_3F",
    "MAP_SANDSTREWN_RUINS_B1F",
    "MAP_SCORCHED_SLAB_B1F",
    "MAP_SCORCHED_SLAB_B2F",
    "MAP_SCORCHED_SLAB_HEATRANS_ROOM",
    "MAP_SEASPRAY_CAVE",
    "MAP_SEASPRAY_CAVE_B1F",
    "MAP_VERDANTURF_MEADOW",
}

# Ultra-space leakage is a deliberate identity of the restored side areas.
# Slot 6 is a native 5-percent land slot: rare enough to feel startling, but
# not a 1-percent grind.  These species are removed from Circuit rewards.
RESTORED_ULTRA_BEASTS = {
    "MAP_ALTERING_CAVE_B1F": "SPECIES_GUZZLORD",
    "MAP_ASHEN_WOODS": "SPECIES_BUZZWOLE",
    "MAP_DEWFORD_MEADOW": "SPECIES_PHEROMOSA",
    "MAP_EMBER_PATH": "SPECIES_BLACEPHALON",
    "MAP_PETALBURG_WOODS_3": "SPECIES_KARTANA",
    "MAP_SANDSTREWN_RUINS_B1F": "SPECIES_STAKATAKA",
    "MAP_SEASPRAY_CAVE": "SPECIES_NIHILEGO",
}

# These are deliberate campaign-roster holes, not a global species quota.
# Every replacement uses a 5- or 10-percent slot (except no slot below 5) and
# replaces a species that remains available elsewhere.  The result guarantees
# the complete Champions roster and the original Kanto families before the
# League while preserving each area's identity.
CAMPAIGN_ROSTER_SLOTS = (
    # map, method, slot, expected source species, replacement species
    ("MAP_ROUTE116", "land_mons", 6, "SPECIES_HOUNDOUR", "SPECIES_EEVEE"),
    ("MAP_SANDSTREWN_RUINS", "land_mons", 6, "SPECIES_CLAYDOL", "SPECIES_AERODACTYL"),
    ("MAP_SEASPRAY_CAVE_B1F", "land_mons", 6, "SPECIES_VANILLITE", "SPECIES_AMAURA"),
    ("MAP_ROUTE119", "water_mons", 2, "SPECIES_PELIPPER", "SPECIES_BASCULIN_WHITE_STRIPED"),
    ("MAP_ROUTE112", "land_mons", 6, "SPECIES_RUFFLET", "SPECIES_CAPSAKID"),
    ("MAP_FIERY_PATH", "land_mons", 6, "SPECIES_HEATMOR", "SPECIES_CHARCADET"),
    ("MAP_VERDANTURF_MEADOW", "land_mons", 6, "SPECIES_STUFFUL", "SPECIES_COTTONEE"),
    ("MAP_SANDSTREWN_RUINS_2F", "land_mons", 6, "SPECIES_CLAYDOL", "SPECIES_CRANIDOS"),
    ("MAP_DESERT_UNDERPASS", "land_mons", 6, "SPECIES_DITTO", "SPECIES_FLITTLE"),
    ("MAP_VERDANTURF_MEADOW", "land_mons", 7, "SPECIES_STUFFUL", "SPECIES_FLOETTE_ETERNAL"),
    ("MAP_MIRAGE_TOWER_B1F", "land_mons", 6, "SPECIES_YAMASK", "SPECIES_GIMMIGHOUL_CHEST"),
    ("MAP_MT_PYRE_6F", "land_mons", 6, "SPECIES_MISDREAVUS", "SPECIES_GREAVARD"),
    ("MAP_ASHEN_WOODS", "land_mons", 5, "SPECIES_CAMERUPT", "SPECIES_GROWLITHE_HISUI"),
    ("MAP_SANDSTREWN_RUINS_2F", "land_mons", 7, "SPECIES_GABITE", "SPECIES_ORTHWORM"),
    ("MAP_MT_PYRE_6F", "land_mons", 7, "SPECIES_MURKROW", "SPECIES_POLTCHAGEIST"),
    ("MAP_ROUTE115", "water_mons", 2, "SPECIES_TENTACRUEL", "SPECIES_QWILFISH_HISUI"),
    ("MAP_PETALBURG_WOODS_2", "land_mons", 6, "SPECIES_BOUNSWEET", "SPECIES_SCATTERBUG"),
    ("MAP_SANDSTREWN_RUINS_3F", "land_mons", 6, "SPECIES_CLAYDOL", "SPECIES_SHIELDON"),
    ("MAP_DEWFORD_MANOR_1F", "land_mons", 6, "SPECIES_RATTATA", "SPECIES_SLOWPOKE_GALAR"),
    ("MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM", "land_mons", 7, "SPECIES_JYNX", "SPECIES_SNEASEL_HISUI"),
    ("MAP_SANDSTREWN_RUINS", "land_mons", 7, "SPECIES_GABITE", "SPECIES_SPIRITOMB"),
    ("MAP_SEASPRAY_CAVE", "land_mons", 7, "SPECIES_WOOBAT", "SPECIES_STUNFISK_GALAR"),
    ("MAP_ROUTE110", "land_mons", 5, "SPECIES_MAGNEMITE", "SPECIES_TADBULB"),
    ("MAP_ROUTE117", "land_mons", 7, "SPECIES_MINCCINO", "SPECIES_TANDEMAUS"),
    ("MAP_ROUTE112", "land_mons", 7, "SPECIES_VULLABY", "SPECIES_TAUROS_PALDEA_COMBAT"),
    ("MAP_EMBER_PATH", "land_mons", 7, "SPECIES_GRUMPIG", "SPECIES_TAUROS_PALDEA_BLAZE"),
    ("MAP_ROUTE118", "water_mons", 2, "SPECIES_PELIPPER", "SPECIES_TAUROS_PALDEA_AQUA"),
    ("MAP_SANDSTREWN_RUINS_3F", "land_mons", 7, "SPECIES_GABITE", "SPECIES_TINKATINK"),
    ("MAP_MIRAGE_TOWER_1F", "land_mons", 4, "SPECIES_GOLETT", "SPECIES_TYRUNT"),
    ("MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM", "land_mons", 3, "SPECIES_BEARTIC", "SPECIES_VULPIX_ALOLA"),
    ("MAP_MT_PYRE_EXTERIOR", "land_mons", 6, "SPECIES_GROWLITHE", "SPECIES_ZORUA_HISUI"),
    ("MAP_ROUTE111_RUINS_EXTERIOR", "land_mons", 3, "SPECIES_ROCKRUFF", "SPECIES_ROCKRUFF_OWN_TEMPO"),
    # Kanto's remaining non-legendary family roots.
    ("MAP_MIRAGE_TOWER_1F", "land_mons", 5, "SPECIES_SIGILYPH", "SPECIES_KABUTO"),
    ("MAP_SEASPRAY_CAVE", "rock_smash_mons", 2, "SPECIES_DWEBBLE", "SPECIES_OMANYTE"),
    ("MAP_NEW_MAUVILLE_INSIDE", "land_mons", 7, "SPECIES_TOGEDEMARU", "SPECIES_PORYGON"),
)


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

    main_group = next(group for group in modern["wild_encounter_groups"] if group["label"] == "gWildMonHeaders")

    for map_name, old_entry in preserved_by_map.items():
        if map_name not in modern_by_map and map_name not in RESTORED_ENCOUNTER_MAPS:
            continue
        if map_name not in modern_by_map:
            target = {"map": map_name, "base_label": map_name.removeprefix("MAP_").title().replace("_", "")}
            main_group["encounters"].append(target)
            modern_by_map[map_name] = target
        else:
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

    missing_restored = RESTORED_ENCOUNTER_MAPS - modern_by_map.keys()
    if missing_restored:
        raise ValueError(f"restored maps lack preserved encounter tables: {sorted(missing_restored)}")

    for map_name, species in RESTORED_ULTRA_BEASTS.items():
        target = modern_by_map[map_name]
        if "land_mons" not in target or len(target["land_mons"]["mons"]) != TARGET_COUNTS["land_mons"]:
            raise ValueError(f"{map_name} lacks a complete land table for {species}")
        target["land_mons"]["mons"][6]["species"] = species

    for map_name, method, slot, expected, species in CAMPAIGN_ROSTER_SLOTS:
        target = modern_by_map[map_name]
        slots = target[method]["mons"]
        if slots[slot]["species"] != expected:
            raise ValueError(
                f"{map_name} {method}[{slot}] drifted: "
                f"{slots[slot]['species']} != {expected}"
            )
        slots[slot]["species"] = species

    main_group["encounters"].sort(key=lambda entry: entry.get("map", ""))

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
    missing_restored = RESTORED_ENCOUNTER_MAPS - by_map.keys()
    if missing_restored:
        raise ValueError(f"missing restored-area encounter tables: {sorted(missing_restored)}")
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
    for map_name, ultra_beast in RESTORED_ULTRA_BEASTS.items():
        slots = by_map[map_name]["land_mons"]["mons"]
        if slots[6]["species"] != ultra_beast:
            raise ValueError(f"{map_name} lost its 5-percent Ultra Beast {ultra_beast}")
    for map_name, method, slot, _, campaign_species in CAMPAIGN_ROSTER_SLOTS:
        if by_map[map_name][method]["mons"][slot]["species"] != campaign_species:
            raise ValueError(f"{map_name} lost campaign roster species {campaign_species}")
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
