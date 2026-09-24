#!/usr/bin/env python3
"""Release gate: route signs list exactly the legends of their own wild table.

A route sign shows the map's ordinary roster (Common_EventScript_ShowRouteRoster)
and then one requirement line per gated legend (setvar VAR_0x8004,
LEGENDARY_SIGN_X / call Common_EventScript_ShowRouteLegend). The lines are
hand-written, so a table change can leave a sign naming a legend that moved
away (Route 120 once advertised Mesprit). This gate fails on any difference and
warns about outdoor routes that hold a gated legend but have no sign wiring.
"""
from __future__ import annotations

import json
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GATES = ROOT / "src/data/pokemon/legendary_signs.h"
WILD = ROOT / "src/data/wild_encounters.json"
MAPS = ROOT / "data/maps"
METHODS = ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons")


def gate_species() -> dict[str, str]:
    rows = re.findall(r"^\s*(?:GATE|VISITOR)\((\w+),", GATES.read_text(), re.M)
    return {f"SPECIES_{name}": f"LEGENDARY_SIGN_{name}" for name in rows}


def table_species() -> dict[str, set[str]]:
    tables: dict[str, set[str]] = {}
    for group in json.loads(WILD.read_text())["wild_encounter_groups"]:
        for entry in group["encounters"]:
            name = entry.get("map")
            if not name:
                continue
            for method in METHODS:
                if method in entry:
                    tables.setdefault(name, set()).update(m["species"] for m in entry[method]["mons"])
    return tables


def map_dir(map_constant: str) -> Path | None:
    key = map_constant[4:].replace("_", "").lower()
    for entry in MAPS.iterdir():
        if entry.name.replace("_", "").lower() == key and (entry / "scripts.inc").exists():
            return entry
    return None


def main() -> int:
    species_to_sign = gate_species()
    failures, warnings = [], []
    for map_constant, species in sorted(table_species().items()):
        directory = map_dir(map_constant)
        if directory is None:
            continue
        expected = {species_to_sign[s] for s in species if s in species_to_sign}
        text = (directory / "scripts.inc").read_text()
        shown = set(re.findall(r"setvar VAR_0x8004, (LEGENDARY_SIGN_\w+)\s*\n\s*call Common_EventScript_ShowRouteLegend", text))
        wired = "Common_EventScript_ShowRouteRoster" in text or "Common_EventScript_ShowRouteSpecies" in text
        if wired and shown != expected:
            failures.append(f"{directory.name}: sign lists {sorted(shown)} but the table holds {sorted(expected)}")
        elif not wired and expected and directory.name.startswith("Route"):
            warnings.append(f"{directory.name}: holds {sorted(expected)} but its signs show no roster")
    for line in warnings:
        print("WARN:", line)
    if failures:
        print(f"FAIL: {len(failures)} route signs disagree with their wild tables")
        for line in failures:
            print("  " + line)
        return 1
    print("PASS: every wired route sign lists exactly its table's legends")
    return 0


if __name__ == "__main__":
    sys.exit(main())
