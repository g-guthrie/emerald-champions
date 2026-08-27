#!/usr/bin/env python3
"""Static release gate for Legendary Signs and the Champions Circuit."""

from __future__ import annotations

import json
import re
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> None:
    definitions = read("src/data/pokemon/legendary_signs.h")
    header = read("include/legendary_signs.h")
    engine = read("src/legendary_signs.c")
    circuit = read("src/champions_circuit.c")
    specials = read("data/specials.inc")

    enum_block = re.search(
        r"enum LegendarySignId\s*\{(?P<body>.*?)LEGENDARY_SIGN_COUNT,",
        header,
        re.S,
    )
    require(enum_block is not None, "Legendary Sign enum is missing")
    ids = re.findall(r"\b(LEGENDARY_SIGN_[A-Z0-9_]+)\b", enum_block.group("body"))
    rows = re.findall(r"(?:WILD|OTHER)_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+),", definitions)
    require(len(ids) == 53, f"expected 53 Legendary Sign IDs, found {len(ids)}")
    require(Counter(rows) == Counter(ids), "Legendary Sign table is not a one-to-one enum allocation")

    source_counts = Counter()
    source_counts["conditional"] = len(re.findall(r"^WILD_SIGN\(", definitions, re.M))
    for source in ("VISIBLE", "BREEDING", "GAME_CORNER", "CIRCUIT", "MASTERY"):
        source_counts[source.lower()] = len(
            re.findall(rf"LEGENDARY_SOURCE_{source}\),", definitions)
        )
    expected = {
        "conditional": 28,
        "visible": 3,
        "breeding": 1,
        "game_corner": 2,
        "circuit": 17,
        "mastery": 2,
    }
    require(dict(source_counts) == expected, f"allocation counts drifted: {dict(source_counts)}")

    wild_rows = re.findall(
        r"WILD_SIGN\([^,]+,\s*([A-Z0-9_]+),\s*([A-Z0-9_]+),\s*"
        r"LEGENDARY_AREA_[A-Z]+,\s*(\d+),\s*(\d+),\s*(-?\d+),\s*"
        r"([A-Z0-9_]+),\s*([^\)]+)\)",
        definitions,
    )
    require(len(wild_rows) == 28, "failed to parse all conditional-wild rows")
    map_constants = read("include/constants/map_groups.h")
    for species, map_name, chance, badges, offset, requirement, flag in wild_rows:
        require(f"#define MAP_{map_name}" in map_constants, f"unknown map for {species}: {map_name}")
        require(1 <= int(chance) <= 100, f"invalid chance for {species}")
        require(0 <= int(badges) <= 8, f"invalid badge gate for {species}")
        require(-10 <= int(offset) <= 10, f"invalid level offset for {species}")
        require(requirement != "NONE", f"{species} has no party requirement")
        require(flag.strip() != "0", f"{species} has no persistent story gate")

    required_specials = (
        "TryUnlockSelectedLegendarySign",
        "TryDiscoverEligibleLegendarySign",
        "TryGiveArceusLegendarySignMasteryReward",
        "ChampionsCircuitCanEnter",
        "ChampionsCircuitBegin",
        "ChampionsCircuitGenerateOpponent",
        "BattleSetup_StartChampionsCircuitBattle",
        "ChampionsCircuitHandleBattleResult",
        "ChampionsCircuitTryGiveReward",
        "ChampionsCircuitEnd",
    )
    for special in required_specials:
        require(f"def_special {special}" in specials, f"missing script special {special}")

    require("MarkLegendarySignCaughtBySpecies(caughtSpecies)" in read("src/battle_script_commands.c"),
            "wild captures do not close Legendary Signs")
    require("TryGetLegendarySignWildOverride" in read("src/wild_encounter.c"),
            "conditional wild hook is missing")
    require("SPECIES_MANAPHY" in read("src/daycare.c") and "SPECIES_PHIONE" in read("src/daycare.c"),
            "Manaphy plus Ditto breeding path is missing")
    require("SPECIES_GENESECT" in read("data/maps/MauvilleCity_GameCorner/scripts.inc"),
            "Genesect Game Corner prize is missing")
    require("SPECIES_POIPOLE" in read("data/maps/MauvilleCity_GameCorner/scripts.inc"),
            "Poipole Game Corner prize is missing")

    for flag in (
        "FLAG_HIDE_LEGENDARY_SIGN_DARKRAI",
        "FLAG_HIDE_LEGENDARY_SIGN_CRESSELIA",
        "FLAG_HIDE_LEGENDARY_SIGN_DIALGA",
    ):
        require(flag in read("data/scripts/new_game.inc"), f"{flag} is not hidden on a new game")
        require(flag in engine, f"{flag} is not synchronized with persistent sign state")

    circuit_requirements = (
        "SpeciesToNationalPokedexNum",
        "team->items[i] == item",
        "team->typeCounts[type1] >= 2",
        "team->legendaryCount >= 2",
        "team->hasMega",
        "PresetHasSpeedControl",
        "SetMatchesCircuitTheme",
        "80 + wins / PARTY_SIZE",
        "wins % PARTY_SIZE",
        "GetVerdantBattleSetRawCount",
        "LEGENDARY_SIGN_ETERNATUS",
    )
    for token in circuit_requirements:
        require(token in circuit, f"Circuit invariant missing: {token}")

    corpus = json.loads(read("docs/verdant_multi_battle_sets.json"))
    require(corpus["set_count"] == 1309, f"expected 1309 legal sets, found {corpus['set_count']}")
    require(corpus["alternative_count"] == 166, "battle-set alternatives drifted")

    vars_h = read("include/constants/vars.h")
    for suffix in range(4):
        require(f"VAR_LEGENDARY_SIGNS_UNLOCKED_{suffix}" in vars_h, "unlocked bitset is incomplete")
        require(f"VAR_LEGENDARY_SIGNS_CAUGHT_{suffix}" in vars_h, "caught bitset is incomplete")
    require("VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS" in vars_h, "Circuit total-win counter is missing")

    print("Legendary Signs: 53/53 allocated")
    print("Conditional wilds: 28; visible: 3; breeding: 1; Game Corner: 2; Circuit: 17; mastery: 2")
    print("Champions Circuit corpus: 1,309 competitive sets")
    print("Legendary Sign and Champions Circuit release gate: PASS")


if __name__ == "__main__":
    main()
