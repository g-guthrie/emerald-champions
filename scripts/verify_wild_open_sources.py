#!/usr/bin/env python3
"""Guard against regressing a species' only open-map wild source.

Compares the working tree's src/data/wild_encounters.json against the
HEAD-committed version. Sealed maps (MAP_SANDSTREWN_RUINS*, MAP_MIRAGE_TOWER_*)
never count as open sources. A species that had >=1 open source before must
still have >=1 open source now. Species with zero open sources both before
and after are reported informationally (they should be on the sealed-only
allowlist the caller reports separately)."""
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WILD_JSON = "src/data/wild_encounters.json"
FIELD_TYPES = ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons")


def is_sealed(map_name: str) -> bool:
    return map_name.startswith("MAP_SANDSTREWN_RUINS") or map_name.startswith("MAP_MIRAGE_TOWER_")


def open_species_sources(payload: dict) -> dict[str, set[str]]:
    group = next(g for g in payload["wild_encounter_groups"] if g["label"] == "gWildMonHeaders")
    sources: dict[str, set[str]] = {}
    for entry in group["encounters"]:
        map_name = entry["map"]
        if is_sealed(map_name):
            continue
        for field in FIELD_TYPES:
            if field not in entry:
                continue
            for mon in entry[field]["mons"]:
                species = mon.get("species")
                if not species:
                    continue
                sources.setdefault(species, set()).add(f"{map_name}/{field}")
    return sources


def load_current() -> dict:
    return json.loads((ROOT / WILD_JSON).read_text())


def load_head() -> dict:
    text = subprocess.run(
        ["git", "show", f"HEAD:{WILD_JSON}"], cwd=ROOT, capture_output=True, text=True, check=True
    ).stdout
    return json.loads(text)


def main() -> None:
    before = open_species_sources(load_head())
    after = open_species_sources(load_current())

    lost = sorted(species for species in before if before[species] and not after.get(species))
    still_sealed_only = sorted(
        species for species in after if not before.get(species) and not after[species]
    ) if False else sorted(
        species for species in set(before) | set(after)
        if not before.get(species) and not after.get(species)
    )

    if lost:
        print("FAIL: species lost their only open-map wild source:")
        for species in lost:
            print(f"  - {species}: was at {sorted(before[species])}")
        sys.exit(1)

    if still_sealed_only:
        print("INFO: species with zero open-map wild sources both before and after "
              "(expect these on the sealed-only allowlist):")
        for species in still_sealed_only:
            print(f"  - {species}")

    print(f"PASS: no species regressed to zero open-map wild sources "
          f"({len(after)} species currently have an open source)")


if __name__ == "__main__":
    main()
