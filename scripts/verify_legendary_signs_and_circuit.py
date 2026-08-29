#!/usr/bin/env python3
"""Static release gates for Legendary Signs and the Showdown Circuit."""

from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def main() -> None:
    manifest = json.loads(read("docs/showdown_champions_random_doubles.json"))
    generated = read("src/data/pokemon/showdown_champions_circuit.h")
    circuit = read("src/champions_circuit.c")
    definitions = read("src/data/pokemon/legendary_signs.h")

    require(manifest["source_commit"] == "bb179fbf8449e3c31632bd56f671ffb4404fa6e7", "Showdown source commit drifted")
    require(manifest["variant_count"] == 311, "Showdown variant count drifted")
    require(manifest["template_count"] == 444, "Showdown template count drifted")
    require(generated.count(".partySpecies =") == 311, "generated Showdown variant table is incomplete")
    require(generated.count(".role =") == 444, "generated Showdown template table is incomplete")
    require("Pokemon Showdown" in read("docs/THIRD_PARTY_NOTICES.md"), "Showdown MIT notice is missing")
    require("gShowdownCircuitVariants" in circuit, "Circuit is not using Showdown's species pool")
    require("gShowdownCircuitTemplates" in circuit, "Circuit is not using Showdown's role templates")
    require("ChooseBaseDex" in circuit and "CandidateAllowed" in circuit, "live Showdown team composition is missing")
    require("towerNumWins" not in circuit and "towerSinglesStreak" not in circuit, "Circuit contaminates Battle Tower records")
    require("VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS" in circuit, "Circuit lacks dedicated current-run state")
    require("CIRCUIT_MASTERY_WINS 40" in circuit, "Circuit mastery milestone drifted")
    require("Random() %" not in circuit, "Circuit still uses biased modulo sampling")
    require(
        "RandomUniform(RNG_NONE, 0, FRONTIER_TRAINERS_COUNT - 1)"
        in read("src/battle_setup.c"),
        "Circuit trainer presentation is not sampled uniformly",
    )

    sign_ids = re.findall(r"(?:WILD|VISIBLE|OTHER)_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+)", definitions)
    require(len(sign_ids) == 81 and len(set(sign_ids)) == 81, "Legendary Sign definitions are incomplete or duplicated")
    require("MIRAGE_TOWER" not in definitions, "a Sign still depends on collapsible Mirage Tower")
    require("SAFARI_ZONE" not in definitions, "a Sign still requires Safari capture rules")
    require("min(MAX_LEVEL, GetCurrentLevelCap())" in read("src/legendary_signs.c"), "Arceus reward level is not clamped")
    require("MarkLegendarySignCaughtBySpecies" in read("src/battle_script_commands.c"), "wild catches do not close Signs")
    require("MarkLegendarySignCaughtBySpecies" in read("src/script_pokemon_util.c"), "gift catches do not close Signs")
    require("MarkLegendarySignCaughtBySpecies" in read("src/egg_hatch.c"), "Phione hatching does not close its Sign")
    require("SPECIES_MANAPHY" in read("src/daycare.c"), "Manaphy and Ditto breeding gate is missing")
    require("FLAG_HIDE_LEGENDARY_SIGN_DARKRAI" in read("data/scripts/new_game.inc"), "visible Sign reset flags are missing")
    require("OBJ_EVENT_GFX_SPECIES(DARKRAI)" in read("data/maps/MtPyre_Summit/map.json"), "Darkrai overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(CRESSELIA)" in read("data/maps/MeteorFalls_B1F_2R/map.json"), "Cresselia overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(DIALGA)" in read("data/maps/MeteorFalls_B1F_1R/map.json"), "Dialga overworld object is missing")
    require("OBJ_EVENT_GFX_SPECIES(REGIGIGAS)" in read("data/maps/SealedChamber_InnerRoom/map.json"), "giant Regigigas object is missing")
    require("SIZE_64x64" in read("src/data/pokemon/species_info/gen_4_families.h"), "Regigigas is not using its giant overworld size")
    require("VAR_LEGENDARY_SIGNS_UNLOCKED_4" in read("src/legendary_signs.c"), "appended Sign state is not persisted")
    require(
        "GetEggSpecies(partySpecies) == requestedRoot"
        in read("src/legendary_signs.c"),
        "legendary requirements do not accept pre-evolutions from the named family",
    )
    for ultra_beast in ("BLACEPHALON", "BUZZWOLE", "GUZZLORD", "KARTANA", "NIHILEGO", "PHEROMOSA", "STAKATAKA"):
        require(
            f"ORDINARY_WILD_SIGN(LEGENDARY_SIGN_{ultra_beast}" in definitions,
            f"{ultra_beast} is not assigned to a restored-area wild table",
        )
    require("SPECIES_GENESECT" in read("data/maps/MauvilleCity_GameCorner/scripts.inc"), "Genesect Game Corner reward is missing")
    require("SPECIES_POIPOLE" in read("data/maps/MauvilleCity_GameCorner/scripts.inc"), "Poipole Game Corner reward is missing")

    generator_hash_before = hashlib.sha256((ROOT / "src/data/pokemon/showdown_champions_circuit.h").read_bytes()).hexdigest()
    print(f"Legendary Signs: {len(sign_ids)} complete acquisition definitions")
    print(f"Showdown Circuit: {manifest['variant_count']} variants, {manifest['template_count']} templates")
    print(f"Generated table SHA256: {generator_hash_before}")


if __name__ == "__main__":
    main()
