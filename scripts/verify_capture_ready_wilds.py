#!/usr/bin/env python3
"""Verify capture-ready ordinary wild Pokémon and prompt-free leveling."""

from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

import verdant_battle_set_presets as battle_sets


ROOT = Path(__file__).resolve().parents[1]
METHODS = ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "honey_mons")


def read(path: str) -> str:
    return (ROOT / path).read_text()


def check(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


def function_body(source: str, name: str) -> str:
    match = re.search(rf"\b{name}\s*\([^)]*\)\s*\{{(.*?)\n\}}", source, re.S)
    if match is None:
        raise AssertionError(f"missing function {name}")
    return match.group(1)


subprocess.run(
    [sys.executable, str(ROOT / "scripts/generate_verdant_legendary_species.py"), "--check"],
    cwd=ROOT,
    check=True,
)

encounter_data = json.loads(read("src/data/wild_encounters.json"))
encounter_group = next(group for group in encounter_data["wild_encounter_groups"] if group.get("for_maps"))
ordinary_species = {
    mon["species"]
    for encounter in encounter_group["encounters"]
    if "map" in encounter
    for method in METHODS
    for mon in encounter.get(method, {}).get("mons", [])
}
legendary_species = set(re.findall(r"case (SPECIES_[A-Z0-9_]+):", read("src/data/pokemon/verdant_legendary_species.h")))
eligible_species = ordinary_species - legendary_species
preset_manifest = json.loads(read("docs/verdant_battle_set_presets.json"))
presets = {row["species"]: row for row in preset_manifest["presets"]}

missing = sorted(eligible_species - presets.keys())
invalid = sorted(
    species
    for species in eligible_species & presets.keys()
    if presets[species].get("review_status") != "authored"
    or len(presets[species].get("moves", [])) != 4
    or not presets[species].get("nature")
    or not presets[species].get("ability")
    or not presets[species].get("runtime_item")
    or presets[species].get("runtime_item") in battle_sets.PROTECTED_SET_ITEMS
)
check(
    "every eligible ordinary-wild species has one authored four-slot preset",
    not missing and not invalid,
)

capture = function_body(read("src/battle_script_commands.c"), "Cmd_givecaughtmon")
runtime = read("src/verdant_battle_sets.c")
wild = read("src/wild_encounter.c")
pokemon = read("src/pokemon.c")
party = read("src/party_menu.c")

check(
    "ordinary wild Pokémon receive their complete set before the battle starts",
    "ApplyVerdantRandomWildBattleSet(&gEnemyParty[0])" in wild
    and wild.index("CreateMonWithNature(&gEnemyParty[0]") < wild.index("ApplyVerdantRandomWildBattleSet(&gEnemyParty[0])"),
)
check(
    "caught Pokémon retain the exact item from the loadout they fought with",
    "SetMonData(&gEnemyParty[partyIndex], MON_DATA_HELD_ITEM, &originalItem);" in capture
    and capture.index("MON_DATA_HELD_ITEM, &originalItem") < capture.index("GiveMonToPlayer"),
)
check(
    "scripted, Frontier, and legendary or mythical encounters remain excluded",
    "!InBattlePike() && !InBattlePyramid() && !IsVerdantLegendarySpecies(species)" in wild
    and "ApplyVerdantRandomWildBattleSet" not in read("src/script_pokemon_util.c"),
)
check(
    "wild presets change only moves, PP, nature, ability, held item, and derived stats",
    all(token in runtime for token in (
        "MON_DATA_PP_BONUSES",
        "SetMonMoveSlot",
        "MON_DATA_NATURE",
        "MON_DATA_ABILITY_NUM",
        "MON_DATA_HELD_ITEM",
        "CalculateMonStats(mon)",
    ))
    and all(token not in runtime for token in (
        "MON_DATA_PERSONALITY",
        "MON_DATA_IV",
        "MON_DATA_EV",
        "MON_DATA_POKEBALL",
        "MON_DATA_FRIENDSHIP",
        "MON_DATA_NICKNAME",
    )),
)
check(
    "one, two, and three tutor sets become uniform one-way wild rolls without safety filtering",
    "Random() % count" in runtime
    and all(token not in runtime for token in ("wildSafe", "EXPLOSION", "MEMENTO", "TELEPORT")),
)

for function in ("MonTryLearningNewMove", "MonTryLearningNewMoveInRange", "MonTryLearningNewEvolutionMove"):
    body = function_body(pokemon, function)
    check(
        f"{function} preserves learnset data without automatic move acquisition",
        "return MOVE_NONE;" in body and "GiveMove" not in body,
    )

check(
    "the move tutor still consumes level, egg, TM, HM, and tutor legality data",
    all(token in pokemon for token in (
        "AddAllLegalMovesForSpecies",
        "gLevelUpLearnsets",
        "GetEggMovesSpecies",
        "NUM_TECHNICAL_MACHINES + NUM_HIDDEN_MACHINES",
        "TUTOR_MOVE_COUNT",
    )),
)
check(
    "Rare Candy remains a ten-level cap-bounded jump with an intermediate evolution stop",
    all(token in party for token in (
        "targetLevel = min(level + 10, GetLevelCap())",
        "GetRareCandyTargetLevel(mon, level)",
        "IsLevelThresholdEvolution",
        "evolution->param < targetLevel",
        "One Rare Candy stops after one evolution",
    )),
)
check(
    "the Leveler still targets the cap and may chain evolutions without move prompts",
    "if (isLeveler)\n            targetLevel = GetLevelCap();" in party
    and "gCB2_AfterEvolution = CB2_ContinueLevelerEvolution" in party
    and "RemoveLevelUpStatsWindow();\n        PartyMenuTryEvolution(taskId);" in party,
)

print(
    "Capture-ready wild verification: "
    f"{len(ordinary_species)} ordinary-table species, "
    f"{len(ordinary_species & legendary_species)} legendary/mythical exclusions, "
    f"{len(eligible_species)} preset-ready species"
)
