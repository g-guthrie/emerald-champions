#!/usr/bin/env python3
"""Copy exact authored Surf/rod species into the native encounter JSON.

The route sheet owns water species in native slot order. Existing native
method tables retain their levels and encounter rates. Land, Rock Smash and
selector alternatives are authored directly in wild_encounters.json.

There is no automatic filler, evolution promotion or rarity substitution:
regeneration must preserve deliberate duplicate residents and discoveries.
"""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WILD = ROOT / "src/data/wild_encounters.json"
ROUTE_SHEET = ROOT / "data/emerald_champions/wild_route_sheet.json"


def load_route_sheet() -> dict[str, dict]:
    return {k: v for k, v in json.loads(ROUTE_SHEET.read_text()).items() if k.startswith("MAP_")}


def materialize_water(data: dict) -> list[str]:
    """Apply the route sheet in memory and return the changed method names."""
    group = next(g for g in data["wild_encounter_groups"] if g["label"] == "gWildMonHeaders")
    fields = {f["type"]: f for f in group["fields"]}
    by_map = {}
    for entry in group["encounters"]:
        by_map.setdefault(entry["map"], entry)
    available = set(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", (ROOT / "include/constants/species.h").read_text()))
    available -= {"SPECIES_NONE", "SPECIES_EGG"}
    changed = []
    for map_id, row in load_route_sheet().items():
        entry = by_map[map_id]
        for field, method in (("water_mons", "surf"), ("fishing_mons", None)):
            if field not in entry:
                continue
            mons = entry[field]["mons"]
            methods = fields[field].get("groups", {method: list(range(len(mons)))})
            for name, indices in methods.items():
                species = ["SPECIES_" + s for s in row[name]]
                if len(species) != len(indices) or any(s not in available for s in species):
                    raise ValueError(f"{map_id}/{name}: invalid authored slots or species")
                if species != [mons[i]["species"] for i in indices]:
                    changed.append(f"{map_id}/{name}")
                for i, s in zip(indices, species):
                    mons[i]["species"] = s
    return changed


def main() -> None:
    data = json.loads(WILD.read_text())
    changed = materialize_water(data)
    if changed:
        WILD.write_text(json.dumps(data, indent=2) + "\n")
    print(f"Materialized {len(load_route_sheet())} water rosters; {len(changed)} methods changed")
    for name in changed:
        print("  " + name)


if __name__ == "__main__":
    main()
