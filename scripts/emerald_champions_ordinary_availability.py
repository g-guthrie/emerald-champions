#!/usr/bin/env python3
"""Place every currently missing ordinary species family in a deliberate habitat."""

from __future__ import annotations

import argparse
import json
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
WILD_PATH = ROOT / "src/data/wild_encounters.json"


@dataclass(frozen=True)
class Placement:
    base_label: str
    method: str
    slot: int
    old_species: str
    species: str
    min_level: int | None = None
    max_level: int | None = None
    rationale: str = ""


PLACEMENTS = (
    Placement("gPetalburgWoods", "land_mons", 7, "SPECIES_PICHU", "SPECIES_SCYTHER", 8, 8, "rare early woodland hunter; Metal Coat preserves later Scizor progression"),
    Placement("gRoute111", "land_mons", 8, "SPECIES_MARACTUS", "SPECIES_SANDILE", rationale="desert predator"),
    Placement("gRoute110", "land_mons", 7, "SPECIES_STUNKY", "SPECIES_TRUBBISH", rationale="industrial roadside scavenger"),
    Placement("gRoute110", "land_mons", 11, "SPECIES_TOXEL", "SPECIES_STUNKY", rationale="preserve the original Route 110 poison encounter family"),
    Placement("gRoute104", "fishing_mons", 1, "SPECIES_FINIZEN", "SPECIES_SOBBLE", rationale="opening Old Rod water starter"),
    Placement("gRoute104", "fishing_mons", 4, "SPECIES_MAGIKARP", "SPECIES_FINIZEN", rationale="retain Finizen as the later Good Rod reward on its original coast"),
    Placement("gRoute101", "honey_mons", 4, "SPECIES_BEAUTIFLY", "SPECIES_SKWOVET", rationale="small forest forager"),
    Placement("gPetalburgWoods_2", "land_mons", 8, "SPECIES_BOUNSWEET", "SPECIES_BLIPBUG", rationale="deep-woods early bug"),
    Placement("gRoute116", "honey_mons", 5, "SPECIES_PURRLOIN", "SPECIES_NICKIT", rationale="hedgerow thief"),
    Placement("gRoute117", "land_mons", 10, "SPECIES_MARILL", "SPECIES_GOSSIFLEUR", rationale="flower-route native"),
    Placement("gRoute117", "land_mons", 11, "SPECIES_MINCCINO", "SPECIES_WOOLOO", rationale="day-care pasture native"),
    Placement("gRoute104", "fishing_mons", 0, "SPECIES_MAGIKARP", "SPECIES_CHEWTLE", rationale="opening Old Rod shoreline catch"),
    Placement("gGraniteCave_1F", "land_mons", 8, "SPECIES_TIMBURR", "SPECIES_ROLYCOLY", rationale="Granite Cave mineral line"),
    Placement("gPetalburgWoods_2", "land_mons", 9, "SPECIES_MORELULL", "SPECIES_APPLIN", rationale="deep-woods fruit mimic"),
    Placement("gRoute111", "land_mons", 9, "SPECIES_HIPPOPOTAS", "SPECIES_SILICOBRA", rationale="desert burrower"),
    Placement("gRoute118", "fishing_mons", 6, "SPECIES_BASCULIN", "SPECIES_ARROKUDA", rationale="fast river predator"),
    Placement("gFieryPath", "land_mons", 10, "SPECIES_HEATMOR", "SPECIES_SIZZLIPEDE", rationale="volcanic centipede"),
    Placement("gRoute109", "fishing_mons", 4, "SPECIES_BRUXISH", "SPECIES_CLOBBOPUS", rationale="Slateport shallows fighter"),
    Placement("gRoute110", "honey_mons", 3, "SPECIES_EKANS", "SPECIES_ZIGZAGOON_GALARIAN", rationale="urban Galarian scavenger"),
    Placement("gRoute116", "land_mons", 10, "SPECIES_HOUNDOUR", "SPECIES_MEOWTH_GALARIAN", rationale="mining-route steel scavenger"),
    Placement("gMtPyre_Exterior", "land_mons", 10, "SPECIES_GROWLITHE", "SPECIES_CORSOLA_GALARIAN", rationale="Mt. Pyre ghost habitat"),
    Placement("gRoute117", "land_mons", 6, "SPECIES_MARILL", "SPECIES_FARFETCHD_GALARIAN", rationale="rural training-route fighter"),
    Placement("gShoalCave_LowTideIceRoom", "land_mons", 10, "SPECIES_CRYOGONAL", "SPECIES_MR_MIME_GALARIAN", rationale="ice-cave performer"),
    Placement("gRoute111", "land_mons", 10, "SPECIES_MARACTUS", "SPECIES_YAMASK_GALARIAN", rationale="desert ruin spirit"),
    Placement("gRoute113", "land_mons", 10, "SPECIES_SKARMORY", "SPECIES_FALINKS", rationale="ash-route marching formation"),
    Placement("gRoute109", "rock_smash_mons", 3, "SPECIES_SANDYGAST", "SPECIES_PINCURCHIN", rationale="rock-pool inhabitant"),
    Placement("gShoalCave_LowTideIceRoom", "land_mons", 11, "SPECIES_JYNX", "SPECIES_SNOM", rationale="ice-cave larva"),
    Placement("gRoute111", "land_mons", 11, "SPECIES_HIPPOPOTAS", "SPECIES_STONJOURNER", rationale="desert monolith"),
    Placement("gShoalCave_LowTideIceRoom", "land_mons", 8, "SPECIES_CRYOGONAL", "SPECIES_EISCUE", rationale="rare ice-room penguin"),
    Placement("gRoute110", "land_mons", 10, "SPECIES_HELIOLISK", "SPECIES_MORPEKO", rationale="electric-route commuter"),
    Placement("gSafariZone_North", "land_mons", 10, "SPECIES_KANGASKHAN", "SPECIES_CUFANT", rationale="Safari Zone herd species"),
    Placement("gDesertUnderpass", "land_mons", 8, "SPECIES_DITTO", "SPECIES_DRACOZOLT", rationale="late fossil refuge"),
    Placement("gDesertUnderpass", "land_mons", 9, "SPECIES_DITTO", "SPECIES_ARCTOZOLT", rationale="late fossil refuge"),
    Placement("gDesertUnderpass", "land_mons", 10, "SPECIES_DITTO", "SPECIES_DRACOVISH", rationale="late fossil refuge"),
    Placement("gDesertUnderpass", "land_mons", 11, "SPECIES_DITTO", "SPECIES_ARCTOVISH", rationale="late fossil refuge"),
)


def alter_cave_unown_placements(payload: dict) -> list[Placement]:
    encounter = next(
        encounter
        for group in payload["wild_encounter_groups"]
        for encounter in group["encounters"]
        if encounter.get("base_label") == "gAlteringCave2"
    )
    return [
        Placement(
            "gAlteringCave2",
            "land_mons",
            slot,
            mon["species"],
            "SPECIES_UNOWN",
            rationale="Altering Cave's dedicated rotating alphabet habitat",
        )
        for slot, mon in enumerate(encounter["land_mons"]["mons"])
    ]


def apply(payload: dict) -> None:
    encounters = {
        encounter["base_label"]: encounter
        for group in payload["wild_encounter_groups"]
        for encounter in group["encounters"]
    }
    for placement in (*PLACEMENTS, *alter_cave_unown_placements(payload)):
        mon = encounters[placement.base_label][placement.method]["mons"][placement.slot]
        if mon["species"] not in {placement.old_species, placement.species}:
            raise ValueError(
                f"{placement.base_label}/{placement.method}/{placement.slot}: "
                f"expected {placement.old_species}, found {mon['species']}"
            )
        mon["species"] = placement.species
        if placement.min_level is not None:
            mon["min_level"] = placement.min_level
        if placement.max_level is not None:
            mon["max_level"] = placement.max_level


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    payload = json.loads(WILD_PATH.read_text())
    apply(payload)
    expected = json.dumps(payload, indent=2) + "\n"
    if args.write:
        WILD_PATH.write_text(expected)
    elif WILD_PATH.read_text() != expected:
        raise SystemExit("ordinary-species habitat placements are stale; run with --write")
    print(f"PASS: {len(PLACEMENTS) + 12} explicit slots close 34 ordinary acquisition families")


if __name__ == "__main__":
    main()
