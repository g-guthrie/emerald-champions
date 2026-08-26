#!/usr/bin/env python3
"""Static regression gate for Verdant's old-engine Gen 8 Fling backport."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


errors: list[str] = []


def require(condition: bool, message: str) -> None:
    if not condition:
        errors.append(message)


items = read("src/data/items.h")
item_blocks = {
    match.group(1): match.group(2)
    for match in re.finditer(
        r"^\s*\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\n\s*\{(.*?)^\s*\},",
        items,
        re.MULTILINE | re.DOTALL,
    )
}


def block_for_item_id(item: str) -> str:
    for block in item_blocks.values():
        if re.search(rf"\.itemId\s*=\s*{re.escape(item)}\b", block):
            return block
    return item_blocks.get(item, "")


def fling_power(item: str) -> int:
    block = block_for_item_id(item)
    match = re.search(r"\.flingPower\s*=\s*(\d+)", block)
    return int(match.group(1)) if match else 0


expected_powers = {
    "ITEM_BIG_NUGGET": 130,
    "ITEM_IRON_BALL": 130,
    "ITEM_HARD_STONE": 100,
    "ITEM_FLAME_PLATE": 90,
    "ITEM_VENUSAURITE": 80,
    "ITEM_TATSUGIRINITE": 80,
    "ITEM_GLIMMORANITE": 80,
    "ITEM_POISON_BARB": 70,
    "ITEM_BURN_DRIVE": 70,
    "ITEM_MACHO_BRACE": 60,
    "ITEM_DAMP_ROCK": 60,
    "ITEM_FIRE_MEMORY": 50,
    "ITEM_POTION": 30,
    "ITEM_BOOSTER_ENERGY": 30,
    "ITEM_ORAN_BERRY": 10,
    "ITEM_WHITE_HERB": 10,
    "ITEM_MENTAL_HERB": 10,
    "ITEM_FOCUS_SASH": 10,
}
for item, expected in expected_powers.items():
    require(fling_power(item) == expected, f"{item} must have Fling power {expected}")

zero_power_items = (
    "ITEM_POKE_BALL",
    "ITEM_NORMAL_GEM",
    "ITEM_ABILITY_CAPSULE",
    "ITEM_TM01_FOCUS_PUNCH",
    "ITEM_LEADERS_CREST",
    "ITEM_GIMMIGHOUL_COIN",
    "ITEM_METAL_ALLOY",
    "ITEM_WELLSPRING_MASK",
    "ITEM_HEARTHFLAME_MASK",
    "ITEM_CORNERSTONE_MASK",
)
for item in zero_power_items:
    require(fling_power(item) == 0, f"{item} must remain unflingable")

for item, block in item_blocks.items():
    if "HOLD_EFFECT_MEGA_STONE" in block:
        require(fling_power(item) == 80, f"Mega Stone {item} must have Fling power 80")
    if ".pocket = POCKET_TM_HM" in block:
        require(fling_power(item) == 0, f"Reusable TM/HM {item} must be unflingable")

power_distribution: dict[int, int] = {}
for block in item_blocks.values():
    match = re.search(r"\.flingPower\s*=\s*(\d+)", block)
    if match:
        power = int(match.group(1))
        power_distribution[power] = power_distribution.get(power, 0) + 1
canonical_power_tiers = {10, 30, 40, 50, 60, 70, 80, 90, 100, 130}
require(
    set(power_distribution) == canonical_power_tiers,
    f"Fling uses an invalid power or lost a canonical power tier: {power_distribution}",
)
require(
    all(count > 0 for count in power_distribution.values()),
    f"Fling contains an empty power tier: {power_distribution}",
)

battle_moves = read("src/data/battle_moves.h")
fling_move = re.search(
    r"\[MOVE_FLING\]\s*=\s*\{(.*?)^\s*\},",
    battle_moves,
    re.MULTILINE | re.DOTALL,
)
require(fling_move is not None, "MOVE_FLING data block is missing")
if fling_move:
    require("FLAG_SHEER_FORCE_BOOST" in fling_move.group(1), "Fling must retain Sheer Force behavior")

battle_util = read("src/battle_util.c")
require(
    "basePower = ItemId_GetFlingPower(gBattleMons[battlerAtk].item);" in battle_util,
    "Fling damage does not use its held item's power",
)
can_fling = re.search(r"bool32 CanFling\(u8 battlerId\)(.*?)^}", battle_util, re.MULTILINE | re.DOTALL)
require(can_fling is not None, "CanFling is missing")
if can_fling:
    body = can_fling.group(1)
    for token in (
        "ABILITY_KLUTZ",
        "STATUS_FIELD_MAGIC_ROOM",
        "embargoTimer",
        "ItemId_GetFlingPower(item) == 0",
        "CanBattlerGetOrLoseItem",
        "ITEM_BOOSTER_ENERGY",
    ):
        require(token in body, f"CanFling is missing {token}")
    require("ABILITY_UNNERVE" not in body, "Unnerve must not prevent using Fling")

scripts = read("data/battle_scripts_1.s")
require(
    ".4byte BattleScript_EffectFling                   @ EFFECT_FLING" in scripts,
    "EFFECT_FLING still routes through the generic hit script",
)
fling_script = re.search(
    r"BattleScript_EffectFling::(.*?)^BattleScript_EffectFlingConsumeBerry::",
    scripts,
    re.MULTILINE | re.DOTALL,
)
require(fling_script is not None, "dedicated Fling script is missing")
if fling_script:
    body = fling_script.group(1)
    ordered = [
        "attackcanceler",
        "jumpifcantfling",
        "accuracycheck BattleScript_FlingMissed",
        "setlastuseditem",
        "damagecalc",
        "adjustdamage",
        "removeflingitem",
        "tryflingholdeffect",
        "tryfaintmon",
    ]
    positions = [body.find(token) for token in ordered]
    require(all(pos >= 0 for pos in positions), "Fling script is missing a required phase")
    require(positions == sorted(positions), "Fling item reveal, damage, removal, and effects are misordered")

for label in ("BattleScript_FlingMentalHerb::", "BattleScript_FlingWhiteHerb::"):
    start = scripts.find(label)
    end = scripts.find("\n\n", start)
    require(start >= 0, f"{label} is missing")
    if start >= 0:
        require("removeitem BS_TARGET" not in scripts[start:end], f"{label} removes the target's real item")
mental_start = scripts.find("BattleScript_FlingMentalHerb::")
mental_end = scripts.find("\n\n", mental_start)
mental_script = scripts[mental_start:mental_end]
require(
    "copybyte gBattlerAttacker, gBattlerTarget" in mental_script
    and "copybyte gBattlerAttacker, sSAVED_BATTLER" in mental_script,
    "Fling Mental Herb messages must name the cured target and restore the attacker",
)

commands = read("src/battle_script_commands.c")
require(
    "gBattleMoves[gCurrentMove].effect == EFFECT_FLING && CanFling(gBattlerAttacker)" in commands,
    "attack canceler does not consume a valid Fling item when no target remains",
)
remove_helper = re.search(
    r"static void RemoveFlingItem\(.*?\n}(?=\n\nstatic void Cmd_various)",
    commands,
    re.DOTALL,
)
require(remove_helper is not None, "Fling-specific item removal helper is missing")
if remove_helper:
    body = remove_helper.group(0)
    for token in ("usedHeldItems", "CheckSetUnburden", "choicedMove", "ClearBattlerItemEffectHistory"):
        require(token in body, f"Fling removal is missing {token}")
    require("TryCheekPouch" not in body, "the Fling user must not activate Cheek Pouch")

for token in (
    "VARIOUS_JUMP_IF_CANT_FLING",
    "VARIOUS_REMOVE_FLING_ITEM",
    "VARIOUS_TRY_FLING_HOLD_EFFECT",
    "BattleScript_FlingBlockedByShieldDust",
    "BattleScript_EffectFlingConsumeBerry",
    "BattleScript_FlingMentalHerb",
    "BattleScript_FlingWhiteHerb",
):
    require(token in commands, f"Fling command implementation is missing {token}")

if errors:
    for error in errors:
        print(f"FAIL: {error}")
    sys.exit(1)

print(f"Fling backport OK: {items.count('.flingPower =')} powered items audited")
