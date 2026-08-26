#!/usr/bin/env python3
"""Focused source invariants for upstream battle/overworld bug cross-checks."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, description: str) -> None:
    if not condition:
        raise AssertionError(description)
    print(f"PASS: {description}")


battle_main = read("src/battle_main.c")
battle_util = read("src/battle_util.c")
commands = read("src/battle_script_commands.c")
opponent = read("src/battle_controller_opponent.c")
scripts = read("data/battle_scripts_1.s")
moves = read("src/data/battle_moves.h")

# expansion #8034 / #10312: turn arrays and battler accesses are bounded.
require("gActionsByTurnOrder[i] = B_ACTION_FINISHED;" in battle_main,
        "turn action array starts with an inactive sentinel")
require("gBattlerByTurnOrder[i] = i;" in battle_main,
        "turn battler array is initialized for every battle")
require("gCurrentTurnActionNumber = gBattlersCount;" in battle_main
        and "gCurrentActionFuncId = B_ACTION_FINISHED;" in battle_main,
        "pre-turn action context cannot masquerade as a move")
require("if (gCurrentTurnActionNumber < gBattlersCount)\n    {\n        gBattleStruct->monToSwitchIntoId" in battle_util,
        "finished actions guard turn-order array access")
require("if ((gCurrentTurnActionNumber < gBattlersCount)" in battle_util,
        "ability suppression bounds-checks current turn first")
require("gActionsByTurnOrder[gCurrentTurnActionNumber] == B_ACTION_USE_MOVE" in battle_util,
        "ability suppression reads the current action slot")
require("gActionsByTurnOrder[gBattlerByTurnOrder[gBattlerAttacker]]" not in battle_util,
        "ability suppression no longer mixes battler and turn indices")

ability_start = battle_util.index("u32 GetBattlerAbility(u8 battlerId)")
ability_end = battle_util.index("u32 IsAbilityOnSide", ability_start)
ability_body = battle_util[ability_start:ability_end]
require(ability_body.index("battlerId >= gBattlersCount") < ability_body.index("gStatuses3[battlerId]"),
        "GetBattlerAbility rejects invalid battlers before reading arrays")

alive_start = battle_util.index("bool32 IsBattlerAlive(u8 battlerId)")
alive_end = battle_util.index("u8 GetBattleMonMoveSlot", alive_start)
alive_body = battle_util[alive_start:alive_end]
require(alive_body.index("battlerId >= gBattlersCount") < alive_body.index("gBattleMons[battlerId].hp"),
        "IsBattlerAlive bounds-checks before reading HP")

# expansion #8150 / PR #10022: only one controller may own both intro mons.
require("static bool32 TwoMonsAtSendOut" in opponent,
        "opponent intro has one ownership predicate")
require("!(gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)" in opponent,
        "two-opponent intros never claim the partner sprite")
require("if (TwoMonsAtSendOut(gActiveBattler))" in opponent,
        "partner intro setup and cleanup share the ownership predicate")

# expansion #8564: Trick updates both sides of the exchange.
require("UpdateUnburdenAfterItemChange(gBattlerAttacker, oldItemAtk, *newItemAtk);" in commands,
        "Trick updates attacker Unburden state")
require("UpdateUnburdenAfterItemChange(gBattlerTarget, *newItemAtk, oldItemAtk);" in commands,
        "Trick updates target Unburden state")

# expansion #5842 / pret #2093: use the canonical selected party slot.
selected_slot = "gBattleStruct->monToSwitchIntoId[gBattlerTarget]"
require(f"SwitchPartyOrderLinkMulti(gBattlerTarget, {selected_slot}, 0);" in commands,
        "forced link-multi switch uses stored selected slot")
require(f"SwitchPartyOrderInGameMulti(gBattlerTarget, {selected_slot});" in commands,
        "forced partner switch uses stored selected slot")

# expansion #7465: the Beat Up user participates even while statused.
user_exception = "i == gBattlerPartyIndexes[gBattlerAttacker]"
script_user_exception = "gBattleCommunication[0] == gBattlerPartyIndexes[gBattlerAttacker]"
require(user_exception in battle_util,
        "Beat Up hit count includes a statused user")
require(script_user_exception in commands,
        "Beat Up damage loop includes a statused user")

# BuffelSaft issue cross-checks that were already fixed in this checkout.
require("gBattleStruct->changedItems[gBattlerAttacker] = gBattleMons[gBattlerAttacker].item;" in commands
        and "gBattleMons[gActiveBattler].item = gBattleStruct->changedItems[gActiveBattler];" in commands,
        "Bug Bite preserves and restores the attacker's real held item (#360)")
require("BattleScript_BerryReduceDmg::" in scripts
        and "removeitem BS_TARGET" in scripts
        and "TryCheekPouch(gActiveBattler, itemId," in commands,
        "resist berries feed the common Cheek Pouch path (#358)")
require("STATUS3_ROOTED\n          && !(B_GHOSTS_ESCAPE >= GEN_6 && IS_BATTLER_OF_TYPE" in battle_util
        and "jumpifstatus3 BS_TARGET, STATUS3_ROOTED, BattleScript_PrintMonIsRooted" in scripts,
        "Ghosts may leave Ingrain voluntarily but remain immune to forced switching (#352)")

rain_start = moves.index("[MOVE_RAIN_DANCE]")
rain_end = moves.index("[MOVE_SUNNY_DAY]", rain_start)
require(".target = MOVE_TARGET_ALL_BATTLERS" in moves[rain_start:rain_end]
        and "gBattleMoves[gCurrentMove].target == MOVE_TARGET_SELECTED" in battle_util,
        "field-targeting Rain Dance cannot be redirected by Storm Drain (#350)")
require("gBattleMons[battler].hp <= gBattleMons[battler].maxHP / 2" in battle_util,
        "Berserk activates on exactly half HP (#349)")
require("case ABILITY_EFFECT_SPORE:" in battle_util
        and "CanSleep(gBattlerAttacker)" in battle_util
        and "CanBePoisoned(gBattlerAttacker, gBattlerTarget)" in battle_util
        and "CanBeParalyzed(gBattlerAttacker)" in battle_util,
        "Effect Spore validates the selected status before announcing (#347)")

print("All upstream engine invariants passed.")
