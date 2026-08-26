#!/usr/bin/env python3
"""Focused source gates for Verdant's Magician and Symbiosis backport."""

from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


COMMANDS = read("src/battle_script_commands.c")
SCRIPTS = read("data/battle_scripts_1.s")
MACROS = read("asm/macros/battle_script.inc")
CONSTANTS = read("include/constants/battle_script_commands.h")
STRUCTS = read("include/battle.h")
MESSAGES = read("src/battle_message.c")
ABILITIES = read("src/data/text/abilities.h")
BATTLE_UTIL = read("src/battle_util.c")

checks: list[tuple[str, bool]] = []


def check(name: str, condition: bool) -> None:
    checks.append((name, bool(condition)))


def block(source: str, start: str, end: str) -> str:
    begin = source.index(start)
    finish = source.index(end, begin)
    return source[begin:finish]


magician = block(COMMANDS, "static bool32 TryMagician(void)", "#define INCREMENT_RESET_RETURN")
symbiosis = block(COMMANDS, "static bool32 CanTriggerSymbiosis", "static bool32 TryMagician")
remove_item = block(COMMANDS, "static void Cmd_removeitem(void)\n{", "static void Cmd_atknameinbuff1")
move_end = block(COMMANDS, "static void Cmd_moveend(void)\n{", "static void Cmd_sethealblock")
bestow = block(COMMANDS, "case VARIOUS_BESTOW:", "case VARIOUS_ARGUMENT_TO_MOVE_EFFECT:")
knock_off = block(COMMANDS, "static bool32 TryKnockOffBattleScript", "static void Cmd_moveend")
incinerate = block(COMMANDS, "case MOVE_EFFECT_INCINERATE:", "case MOVE_EFFECT_BUG_BITE:")

check("Magician has a dedicated move-end state", "case MOVEEND_MAGICIAN:" in move_end)
check("Magician requires its Ability and an empty item slot",
      "IsBattlerAlive(gBattlerAttacker)" in magician
      and "GetBattlerAbility(gBattlerAttacker) != ABILITY_MAGICIAN" in magician
      and "gBattleMons[gBattlerAttacker].item != ITEM_NONE" in magician
      and "gBattleStruct->changedItems[gBattlerAttacker] != ITEM_NONE" in magician)
check("Magician excludes Fling, Natural Gift, Future Sight, and Gems",
      all(token in magician for token in (
          "EFFECT_FLING", "EFFECT_NATURAL_GIFT", "EFFECT_FUTURE_SIGHT", "gemBoost")))
check("Magician requires current direct non-Substitute damage",
      "damagedMons" in magician
      and "physicalBattlerId == gBattlerAttacker" in magician
      and "specialBattlerId == gBattlerAttacker" in magician
      and "DoesSubstituteBlockMove" in magician)
check("Magician rejects unstealable and knocked-off items",
      "CanStealItem" in magician and "knockedOffMons" in magician)
check("Magician prioritizes foes and uses Trick Room speed order",
      "targets = (foeTargets != 0) ? foeTargets : allyTargets" in magician
      and "STATUS_FIELD_TRICK_ROOM" in magician
      and "SortBattlersBySpeed" in magician)
check("Magician respects live Sticky Hold",
      "ABILITY_STICKY_HOLD" in magician and "IsBattlerAlive(target)" in magician)
check("Magician steals once, records its source, and runs native presentation",
      "StealTargetItem(gBattlerAttacker, target)" in magician
      and "gEffectBattler = target" in magician
      and "BattleScript_MagicianActivates" in magician
      and "BattleScript_MagicianActivates::" in SCRIPTS
      and "call BattleScript_AbilityPopUp" in SCRIPTS)
check("Magician resolves after recoil but before eject, Red Card, Life Orb, and Pickpocket",
      int(re.search(r"#define MOVEEND_RECOIL\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_MAGICIAN\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_EJECT_BUTTON\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_RED_CARD\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_LIFEORB_SHELLBELL\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_PICKPOCKET\s+(\d+)", CONSTANTS).group(1)))

check("Symbiosis requires two live allies, its Ability, and an empty real slot",
      all(token in symbiosis for token in (
          "IsBattlerAlive(battler)", "IsBattlerAlive(partner)",
          "ABILITY_SYMBIOSIS", "gBattleMons[battler].item != ITEM_NONE",
          "gBattleStruct->changedItems[battler] != ITEM_NONE")))
check("Symbiosis validates both sides can lose or receive the item",
      symbiosis.count("CanBattlerGetOrLoseItem") >= 2)
check("Symbiosis synchronizes both battlers to controllers",
      symbiosis.count("BtlController_EmitSetMonData") == 2
      and symbiosis.count("MarkBattlerForControllerExec") == 2)
check("Symbiosis updates choice locks and Unburden on both battlers",
      symbiosis.count("choicedMove") >= 2
      and "CheckSetUnburden(battlerDonor)" in symbiosis
      and "~RESOURCE_FLAG_UNBURDEN" in symbiosis)
check("Symbiosis updates item knowledge without creating Recycle history",
      "ClearBattlerItemEffectHistory(battlerDonor)" in symbiosis
      and "RecordItemEffectBattle(battlerRecipient" in symbiosis
      and "usedHeldItems" not in symbiosis)
check("Normal removeitem queues immediate Symbiosis after real consumption",
      "TrySymbiosis(gActiveBattler, nextInstr)" in remove_item
      and "itemId != ITEM_NONE" in remove_item
      and "!gBattleScripting.overrideBerryRequirements" in remove_item)
check("Eject items are excluded and Gem/resist Berry transfers are delayed",
      "HOLD_EFFECT_EJECT_BUTTON" in remove_item
      and "HOLD_EFFECT_EJECT_PACK" in remove_item
      and "gemBoost" in remove_item
      and "berryReduced" in remove_item
      and "symbiosisPending = TRUE" in remove_item)
check("Delayed Symbiosis drains every pending battler before clearing",
      "case MOVEEND_SYMBIOSIS:" in move_end
      and "if (!gSpecialStatuses[i].symbiosisPending)" in move_end
      and "gSpecialStatuses[i].symbiosisPending = FALSE" in move_end)
check("Symbiosis runs after the final multi-hit and before recoil",
      re.search(r"#define MOVEEND_MULTIHIT_MOVE\s+(\d+)", CONSTANTS)
      and re.search(r"#define MOVEEND_SYMBIOSIS\s+(\d+)", CONSTANTS)
      and re.search(r"#define MOVEEND_RECOIL\s+(\d+)", CONSTANTS)
      and int(re.search(r"#define MOVEEND_MULTIHIT_MOVE\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_SYMBIOSIS\s+(\d+)", CONSTANTS).group(1))
      < int(re.search(r"#define MOVEEND_RECOIL\s+(\d+)", CONSTANTS).group(1)))
check("Fling success, miss, and consumed-failure paths all try Symbiosis",
      block(SCRIPTS, "BattleScript_EffectFling::", "BattleScript_FlingBlockedByShieldDust::")
      .count("trysymbiosis BS_ATTACKER") == 3)
check("Bug Bite/Pluck tries Symbiosis for the Berry's original holder",
      "trysymbiosis BS_TARGET" in block(
          SCRIPTS, "BattleScript_MoveEffectBugBite::", "BattleScript_EffectCoreEnforcer:"))
check("Bestow uses centralized transfer bookkeeping then tries Symbiosis",
      "PassHeldItem(gBattlerAttacker, gBattlerTarget)" in bestow
      and "trysymbiosis BS_ATTACKER" in block(
          SCRIPTS, "BattleScript_EffectBestow:", "BattleScript_EffectAfterYou:"))
check("Symbiosis has an explicit battle-script command and native popup/message",
      "VARIOUS_TRY_SYMBIOSIS" in CONSTANTS
      and ".macro trysymbiosis" in MACROS
      and "case VARIOUS_TRY_SYMBIOSIS:" in COMMANDS
      and "BattleScript_SymbiosisActivates::" in SCRIPTS
      and "STRINGID_SYMBIOSISITEMPASS" in SCRIPTS)
check("Symbiosis pending state is explicit and cleared at move end",
      "symbiosisPending:1" in STRUCTS
      and "for (i = 0; i < gBattlersCount; i++)" in move_end
      and "symbiosisPending = FALSE" in move_end)
check("Ability descriptions match implemented behavior",
      "Steals a hit target's item." in ABILITIES
      and "Passes its item to an ally." in ABILITIES)
check("Symbiosis message uses donor and recipient battle placeholders",
      "gBattleScripting.battler = gBattlerAbility = partner" in symbiosis
      and "gEffectBattler = battler" in symbiosis
      and "{B_SCR_ACTIVE_NAME_WITH_PREFIX}'s {B_SCR_ACTIVE_ABILITY}\\npassed {B_LAST_ITEM} to {B_EFF_NAME_WITH_PREFIX}!" in MESSAGES
      and "copybyte gBattlerAbility, sBATTLER" in block(
          SCRIPTS, "BattleScript_SymbiosisActivates::", "BattleScript_DrizzleActivates::"))
check("Knock Off and Incinerate retain canonical no-Symbiosis behavior",
      "TrySymbiosis" not in knock_off and "TrySymbiosis" not in incinerate)
check("Postbattle restoration recognizes ally-owned transferred items",
      "currentItem == gBattleStruct->itemStolen[j].originalItem" in BATTLE_UTIL
      and "cameFromPlayerParty = TRUE" in BATTLE_UTIL
      and "if (currentItem != ITEM_NONE && !cameFromPlayerParty)" in BATTLE_UTIL
      and "SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &originalItem)" in BATTLE_UTIL)

failed = [name for name, ok in checks if not ok]
for name, ok in checks:
    print(f"[{'PASS' if ok else 'FAIL'}] {name}")

print(f"\n{len(checks) - len(failed)}/{len(checks)} checks passed")
if failed:
    sys.exit(1)
