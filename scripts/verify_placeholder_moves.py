#!/usr/bin/env python3
"""Focused static regression checks for Verdant's formerly-placeholder moves."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


failures: list[str] = []
checks = 0


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if not condition:
        failures.append(label)


moves = {
    "MOVE_SKY_DROP": "EFFECT_SKY_DROP",
    "MOVE_BEAK_BLAST": "EFFECT_BEAK_BLAST",
    "MOVE_EXPANDING_FORCE": "EFFECT_EXPANDING_FORCE",
    "MOVE_SCALE_SHOT": "EFFECT_SCALE_SHOT",
    "MOVE_METEOR_BEAM": "EFFECT_METEOR_BEAM",
    "MOVE_RISING_VOLTAGE": "EFFECT_RISING_VOLTAGE",
    "MOVE_CORROSIVE_GAS": "EFFECT_CORROSIVE_GAS",
}

move_data = read("src/data/battle_moves.h")
for move, effect in moves.items():
    block = re.search(rf"\[{move}\]\s*=\s*\{{(?P<body>.*?)\n\s*\}},", move_data, re.S)
    require(block is not None, f"missing move-data block for {move}")
    if block:
        require(f".effect = {effect}" in block.group("body"), f"{move} does not use {effect}")
        require("EFFECT_PLACEHOLDER" not in block.group("body"), f"{move} is still a placeholder")

placeholder_moves = re.findall(
    r"\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{(?:(?!\n\s*\},).)*?\.effect\s*=\s*EFFECT_PLACEHOLDER",
    move_data,
    re.S,
)
require(not placeholder_moves, f"legal moves still use EFFECT_PLACEHOLDER: {placeholder_moves}")

effects = read("include/constants/battle_move_effects.h")
for effect in moves.values():
    require(re.search(rf"#define\s+{effect}\s+\d+", effects) is not None, f"missing {effect} constant")

scripts = read("data/battle_scripts_1.s")
for symbol in (
    "BattleScript_EffectSkyDrop",
    "BattleScript_BeakBlastSetUp",
    "BattleScript_BeakBlastBurn",
    "BattleScript_EffectScaleShot",
    "BattleScript_EffectMeteorBeam",
    "BattleScript_EffectCorrosiveGas",
):
    require(symbol in scripts, f"missing native script {symbol}")

commands = read("src/battle_script_commands.c")
utility = read("src/battle_util.c")
main = read("src/battle_main.c")
require("skyDropTarget" in commands and "skyDropUser" in commands, "Sky Drop lacks explicit paired state")
require("skyDropPartyId" in commands, "Sky Drop lacks replacement-slot validation")
require("beakBlastCharge" in commands, "Beak Blast contact state is absent")
require("IsBattlerTerrainAffected(battlerAtk, STATUS_FIELD_PSYCHIC_TERRAIN)" in utility,
        "Expanding Force does not require a grounded user")
require("IsBattlerTerrainAffected(battlerDef, STATUS_FIELD_ELECTRIC_TERRAIN)" in utility,
        "Rising Voltage does not require a grounded target")
require("STATUS3_SKY_DROPPED" in utility and "STATUS3_SKY_DROPPED" in main,
        "Sky Drop cleanup/action suppression is incomplete")
require("EFFECT_CORROSIVE_GAS" in commands and "usedHeldItems" in commands,
        "Corrosive Gas permanent item destruction is not wired")

animations = read("data/battle_anim_scripts.s")
for symbol in (
    "Move_SKY_DROP",
    "General_BeakBlastSetUp",
    "Move_BEAK_BLAST",
    "Move_EXPANDING_FORCE",
    "Move_SCALE_SHOT",
    "Move_METEOR_BEAM",
    "Move_RISING_VOLTAGE",
    "Move_CORROSIVE_GAS",
):
    require(symbol in animations, f"missing native animation {symbol}")

require("STRINGID_NOTDONEYET" not in "\n".join(
    line for line in scripts.splitlines() if "BattleScript_EffectPlaceholder" not in line
), "reachable move scripts still print STRINGID_NOTDONEYET")

if failures:
    print(f"placeholder-move verifier: {len(failures)} failure(s) / {checks} checks")
    for failure in failures:
        print(f"FAIL: {failure}")
    sys.exit(1)

print(f"placeholder-move verifier: {checks}/{checks} checks passed")
