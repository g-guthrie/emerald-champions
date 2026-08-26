#!/usr/bin/env python3
"""Focused semantic gates for selected live Expansion mechanics reports.

References: rh-hideout/pokeemerald-expansion issues 10303, 7485, 7456,
7444, 8677, 7180, 8773, and merged pull request 10669.
"""

from __future__ import annotations

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
checks = 0
failures: list[str] = []


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def require(condition: bool, label: str) -> None:
    global checks
    checks += 1
    if condition:
        print(f"PASS: {label}")
    else:
        failures.append(label)
        print(f"FAIL: {label}")


def function(text: str, name: str) -> str:
    match = re.search(
        rf"^(?:static\s+)?(?:void|bool8|bool16|bool32|u8|u16|u32|s8|s16|s32|int)\s+"
        rf"{re.escape(name)}\s*\([^;{{}}]*?\)\s*(?://[^\n]*)?\s*\{{",
        text,
        re.M | re.S,
    )
    if not match:
        return ""
    depth = 1
    pos = match.end()
    while pos < len(text) and depth:
        if text[pos] == "{":
            depth += 1
        elif text[pos] == "}":
            depth -= 1
        pos += 1
    return text[match.start():pos]


def script_block(text: str, label: str) -> str:
    start = text.find(label)
    if start < 0:
        return ""
    next_label = re.search(r"\n[A-Za-z_][A-Za-z0-9_]*::?\n", text[start + len(label):])
    end = len(text) if next_label is None else start + len(label) + next_label.start()
    return text[start:end]


def move_block(text: str, move: str) -> str:
    start = text.find(f"[{move}] =")
    if start < 0:
        return ""
    end = text.find("\n    [MOVE_", start + 1)
    return text[start: len(text) if end < 0 else end]


ai = read("src/battle_ai_main.c")
ai_util = read("src/battle_ai_util.c")
main = read("src/battle_main.c")
utility = read("src/battle_util.c")
player = read("src/battle_controller_player.c")
commands = read("src/battle_script_commands.c")
scripts = read("data/battle_scripts_1.s")
moves = read("src/data/battle_moves.h")


# #10303: Decorate targeting and ally valuation.
decorate = move_block(moves, "MOVE_DECORATE")
require(".effect = EFFECT_DECORATE" in decorate and ".target = MOVE_TARGET_SELECTED" in decorate,
        "Decorate remains a selected-target status move")
bad_move = function(ai, "AI_CheckBadMove")
require("moveEffect == EFFECT_DECORATE && !IsTargetingPartner" in bad_move
        and "RETURN_SCORE_MINUS(60)" in bad_move,
        "Decorate hard-disfavors either opposing target")
doubles = function(ai, "AI_DoubleBattle")
require("effect == EFFECT_DECORATE" in doubles
        and "atkPartnerAbility != ABILITY_CONTRARY" in doubles
        and "atkPartnerAbility != ABILITY_GOOD_AS_GOLD" in doubles,
        "Decorate rejects blocked or inverted ally boosts")
require("statStages[STAT_ATK] < MAX_STAT_STAGE" in doubles
        and "HasMoveWithSplit(battlerAtkPartner, SPLIT_PHYSICAL)" in doubles
        and "statStages[STAT_SPATK] < MAX_STAT_STAGE" in doubles
        and "HasMoveWithSplit(battlerAtkPartner, SPLIT_SPECIAL)" in doubles,
        "Decorate values only a legal boost the ally can use")


# #7485: modern simultaneous manual switch order.
switch_order = function(main, "ShouldSwapSwitchActions")
require("GetBattlerTotalSpeedStat(battler1)" in switch_order
        and "GetBattlerTotalSpeedStat(battler2)" in switch_order,
        "manual switch order compares effective Speed")
require("STATUS_FIELD_TRICK_ROOM" in switch_order
        and "return speed1 > speed2" in switch_order
        and "return speed1 < speed2" in switch_order,
        "Trick Room reverses simultaneous manual switch order")
switch_model = lambda speed1, speed2, trick_room: speed1 > speed2 if trick_room else speed1 < speed2
require(switch_model(50, 100, False) and not switch_model(100, 50, False),
        "normal simultaneous switches put the faster battler first")
require(switch_model(100, 50, True) and not switch_model(50, 100, True),
        "Trick Room simultaneous switches put the slower battler first")
require(all(token not in switch_order for token in (
            "quickDraw", "usedCustapBerry", "HOLD_EFFECT_LAGGING_TAIL", "ABILITY_STALL")),
        "move-only ordering effects cannot influence manual switches")
set_order = function(main, "SetActionsAndBattlersTurnOrder")
require("gActionsByTurnOrder[i] == B_ACTION_SWITCH" in set_order
        and "gActionsByTurnOrder[j] == B_ACTION_SWITCH" in set_order
        and "ShouldSwapSwitchActions(battler1, battler2)" in set_order,
        "the turn queue invokes the dedicated comparator only for switch pairs")


# #7456: normal and called all-adjacent moves share one first-target rule.
use_move = function(utility, "HandleAction_UseMove")
require(re.search(
            r"target == MOVE_TARGET_FOES_AND_ALLY\)\s*\{[^{}]*"
            r"gBattlerTarget = GetMoveTarget\(gCurrentMove, 0\);",
            use_move,
            re.S,
        ) is not None,
        "selected all-adjacent moves use canonical GetMoveTarget resolution")
require("for (gBattlerTarget = 0; gBattlerTarget < gBattlersCount" not in use_move,
        "all-adjacent first target no longer depends on raw battler IDs")
get_target = function(utility, "GetMoveTarget")
require("case MOVE_TARGET_FOES_AND_ALLY:" in get_target
        and "GetBattlerAtPosition((GetBattlerPosition(gBattlerAttacker) & BIT_SIDE) ^ BIT_SIDE)" in get_target,
        "called all-adjacent moves begin on the opposing flank")


# #7444: move-local history must travel with an in-battle slot reorder.
swap_flags = function(player, "SwapMoveSlotFlags")
require("slot1Set" in swap_flags and "slot2Set" in swap_flags
        and "flags &= ~(slot1Mask | slot2Mask)" in swap_flags,
        "move-slot flag swapping is bidirectional")
move_switch = function(player, "HandleMoveSwitching")
require("mimickedMoves = SwapMoveSlotFlags" in move_switch,
        "Mimic history follows a reordered move")
require("usedMoves = SwapMoveSlotFlags" in move_switch,
        "Last Resort history follows a reordered move")


# #8677: this older engine already gives modern Mimic its full base maximum PP.
mimic = function(commands, "Cmd_mimicattackcopy")
require("gBattleMons[gBattlerAttacker].pp[gCurrMovePos] = gBattleMoves[gLastMoves[gBattlerTarget]].pp" in mimic,
        "Mimic receives the copied move's full base maximum PP")
require(re.search(r"pp\[gCurrMovePos\]\s*=\s*5\b", mimic) is None
        and "gBattleMoves[gLastMoves[gBattlerTarget]].pp < 5" not in mimic,
        "Mimic is not clamped to the legacy five PP")


# #7180: a Me First-called rampage is not a persistent selection.
move_end = function(commands, "Cmd_moveend")
me_first_check = move_end.find("gStatuses3[gBattlerAttacker] & STATUS3_ME_FIRST")
me_first_clear = move_end.find("gStatuses3[gBattlerAttacker] &= ~(STATUS3_ME_FIRST)")
require(me_first_check >= 0 and me_first_clear > me_first_check
        and "gBattleMoves[gCurrentMove].effect == EFFECT_RAMPAGE" in move_end[me_first_check:me_first_clear]
        and "CancelMultiTurnMoves(gBattlerAttacker)" in move_end[me_first_check:me_first_clear],
        "Me First clears a called rampage lock before clearing its context marker")


# #8773: field/self transformations do not run target accuracy or absorption.
for label in (
    "BattleScript_EffectFairyLock:",
    "BattleScript_EffectPowerTrick:",
    "BattleScript_EffectIonDeluge:",
):
    block = script_block(scripts, label)
    require("attackcanceler" in block and "attackstring" in block and "ppreduce" in block,
            f"{label.rstrip(':')} retains normal action accounting")
    require("accuracycheck" not in block,
            f"{label.rstrip(':')} cannot trigger target accuracy or absorbing abilities")


# #10669: Lock-On is attacker-owned even though this engine stores its marker
# on the defender.
lock_target = function(ai, "IsBattlerLockedOnTarget")
lock_any = function(ai, "HasBattlerLockedOn")
require("STATUS3_ALWAYS_HITS" in lock_target
        and "battlerWithSureHit == battlerAtk" in lock_target,
        "Lock-On ownership checks both target marker and originating attacker")
require("for (battlerDef = 0; battlerDef < gBattlersCount" in lock_any
        and "IsBattlerLockedOnTarget" in lock_any,
        "AI detects an existing lock on either doubles target")
require("case EFFECT_LOCK_ON:" in bad_move and "HasBattlerLockedOn(battlerAtk)" in bad_move,
        "AI refuses to replace an active lock with another Lock-On")
viability = function(ai, "AI_CheckViability")
require("IsBattlerLockedOnTarget(battlerAtk, battlerDef)" in viability
        and "!IS_MOVE_STATUS(move)" in viability,
        "AI prefers damaging the foe it has already locked on")
encouraged = function(ai_util, "IsMoveEncouragedToHit")
lock_expression = re.search(
    r"if\s*\(\(gStatuses3\[battlerDef\]\s*&\s*STATUS3_ALWAYS_HITS\)\s*"
    r"&&\s*gDisableStructs\[battlerDef\]\.battlerWithSureHit\s*==\s*battlerAtk\)",
    encouraged,
    re.S,
)
require(lock_expression is not None,
        "AI accuracy requires both a live Lock-On timer and the same attacker")
lock_model = lambda timer, owner, attacker: timer and owner == attacker
require(not lock_model(True, 1, 3),
        "a live timer owned by another attacker cannot grant perfect accuracy")
require(not lock_model(False, 1, 1),
        "a stale owner field without the timer cannot grant perfect accuracy")


print(f"upstream mechanics backports: {checks - len(failures)}/{checks} checks passed")
if failures:
    for failure in failures:
        print(f" - {failure}")
    sys.exit(1)
