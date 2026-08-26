#!/usr/bin/env python3
"""Focused static gates for Verdant's battle edge-mechanics backports."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def section(text: str, start: str, end: str) -> str:
    begin = text.index(start)
    finish = text.index(end, begin + len(start))
    return text[begin:finish]


commands = read("src/battle_script_commands.c")
util = read("src/battle_util.c")
main = read("src/battle_main.c")
moves = read("src/data/battle_moves.h")
scripts = read("data/battle_scripts_1.s")
battle_h = read("include/battle.h")
battle_util_h = read("include/battle_util.h")
battle_scripts_h = read("include/battle_scripts.h")

checks = {
    "Dragon Darts move data no longer advertises a TODO implementation": (
        ".effect = EFFECT_DOUBLE_HIT, //TODO" not in moves
        and "[MOVE_DRAGON_DARTS]" in moves
    ),
    "Dragon Darts has pure legality and immunity helpers": all(
        token in commands
        for token in (
            "IsDragonDartsFollowMeLocked",
            "CanDragonDartsTargetPartner",
            "IsDragonDartsTargetSemiInvulnerable",
            "DoesDragonDartsTargetAbsorb",
            "IsDragonDartsTargetImmune",
            "CanDragonDartsRedirectToPartner",
        )
    ),
    "Dragon Darts redirects an immune selected target before blocking scripts": (
        section(commands, "static void Cmd_attackcanceler(void)", "static bool32 JumpIfMoveFailed")
        .index("TryRedirectImmuneDragonDartsTarget();")
        < section(commands, "static void Cmd_attackcanceler(void)", "static bool32 JumpIfMoveFailed")
        .index("AbilityBattleEffects(ABILITYEFFECT_MOVES_BLOCK")
    ),
    "Dragon Darts checks accuracy independently and retries one legal foe": all(
        token in section(commands, "static void Cmd_accuracycheck(void)\n{", "static void Cmd_attackstring(void)\n{")
        for token in (
            "move != MOVE_DRAGON_DARTS",
            "retriedDragonDarts",
            "CanDragonDartsRedirectToPartner(gBattlerTarget)",
            "gBattlerTarget = BATTLE_PARTNER(gBattlerTarget)",
        )
    ),
    "Dragon Darts reports per-target effectiveness": (
        commands.count("!gMultiHitCounter || gCurrentMove == MOVE_DRAGON_DARTS") == 2
    ),
    "Dragon Darts splits its second hit and suppresses generic hit-count text": all(
        token in section(commands, "case MOVEEND_MULTIHIT_MOVE:", "case MOVEEND_SYMBIOSIS:")
        for token in (
            "gBattleStruct->moveTarget[gBattlerAttacker] == gBattlerTarget",
            "CanDragonDartsRedirectToPartner(gBattlerTarget)",
            "gCurrentMove != MOVE_DRAGON_DARTS",
        )
    ),
    "Future Sight resolves one explicit target without a partner loop": (
        "gBattlerTarget = gActiveBattler;" in section(util, "bool8 HandleWishPerishSongOnTurnEnd", "bool8 HandleFaintedMonActions")
        and "gBattlerAttacker = gWishFutureKnock.futureSightAttacker[gActiveBattler];" in util
        and "jumpifnexttargetvalid" not in section(scripts, "BattleScript_MonTookFutureAttack::", "BattleScript_NoMovesLeft::")
        and section(scripts, "BattleScript_MonTookFutureAttack::", "BattleScript_NoMovesLeft::").count("accuracycheck") == 2
    ),
    "Spread moves never enqueue a fainted second target": (
        "if (IsBattlerAlive(battlerId))" in section(commands, "case MOVEEND_NEXT_TARGET:", "case MOVEEND_EJECT_BUTTON:")
        and "if (!IsBattlerAlive(gBattlerTarget))" in section(util, "void HandleAction_UseMove(void)", "void HandleAction_Switch(void)")
    ),
    "Emergency Exit exposes reusable activation and threshold predicates": all(
        token in util and token in battle_util_h
        for token in (
            "CanBattlerActivateEmergencyExit",
            "DidBattlerCrossEmergencyExitThreshold",
        )
    ),
    "Direct HP-cost moves do not masquerade as Emergency Exit damage": (
        "gBattleMoves[gCurrentMove].power != 0" in section(util, "case ABILITY_EMERGENCY_EXIT:", "case ABILITY_WEAK_ARMOR:")
        and "gBattleMoves[gCurrentMove].effect != EFFECT_PAIN_SPLIT" in util
    ),
    "All native recoil families latch Emergency Exit before move-end healing": all(
        token in section(commands, "static bool32 IsEmergencyExitRecoilMove", "static void Cmd_moveend(void)")
        for token in (
            "EFFECT_RECOIL_IF_MISS",
            "EFFECT_RECOIL_25",
            "EFFECT_RECOIL_33",
            "EFFECT_RECOIL_50",
            "EFFECT_RECOIL_33_STATUS",
            "EFFECT_MIND_BLOWN",
            "EFFECT_RECOIL_HP_25",
        )
    ) and "RESOURCE_FLAG_EMERGENCY_EXIT_LATCHED" in battle_h,
    "Move-end Emergency Exit revalidates pending events and honors recoil latch": all(
        token in section(commands, "case MOVEEND_EMERGENCY_EXIT:", "case MOVEEND_MULTIHIT_MOVE:")
        for token in (
            "CanBattlerActivateEmergencyExit(i)",
            "latched || DidBattlerCrossEmergencyExitThreshold(i)",
            "RESOURCE_FLAG_EMERGENCY_EXIT_LATCHED",
        )
    ),
    "Shell Bell latches any attacker threshold crossing before healing": all(
        token in section(util, "case HOLD_EFFECT_SHELL_BELL:", "case HOLD_EFFECT_LIFE_ORB:")
        for token in (
            "CanBattlerActivateEmergencyExit(gBattlerAttacker)",
            "DidBattlerCrossEmergencyExitThreshold(gBattlerAttacker)",
            "RESOURCE_FLAG_EMERGENCY_EXIT_LATCHED",
        )
    ),
    "Residual damage snapshots and checks Emergency Exit between events": (
        all(
            token in section(main, "void BattleTurnPassed(void)\n{", "void HandleTurnActionSelectionState")
            for token in (
                "gBattleStruct->turnCountersTracker == 0",
                "gBattleStruct->turnEffectsTracker == 0",
                "gBattleStruct->hpBefore[i] = gBattleMons[i].hp;",
            )
        )
        and "gBattleStruct->turnEffectsTracker > ENDTURN_ITEMS1" in util
        and "BattleScript_EmergencyExitEnd2" in util
        and "BattleScript_EmergencyExitWildEnd2" in util
    ),
    "Hazards reset threshold state and can interrupt the remaining hazard queue": (
        "gBattleStruct->hpBefore[gActiveBattler] = gBattleMons[gActiveBattler].hp;" in section(commands, "static void Cmd_switchindataupdate(void)\n{", "static void Cmd_switchinanim(void)\n{")
        and all(
            token in section(commands, "static void Cmd_switchineffects(void)\n{", "static void Cmd_trainerslidein(void)\n{")
            for token in (
                "SIDE_STATUS_SPIKES_DAMAGED | SIDE_STATUS_STEALTH_ROCK_DAMAGED",
                "CanBattlerActivateEmergencyExit(gActiveBattler)",
                "DidBattlerCrossEmergencyExitThreshold(gActiveBattler)",
                "BattleScriptPush(gBattlescriptCurrInstr + 2);",
            )
        )
    ),
    "End-turn Emergency Exit scripts terminate through the callback stack": all(
        token in scripts and token in battle_scripts_h
        for token in (
            "BattleScript_EmergencyExitEnd2",
            "BattleScript_EmergencyExitWildEnd2",
        )
    ),
    "Attack-string guard is canonicalized to one condition": (
        section(commands, "static void Cmd_attackstring(void)\n{", "static void Cmd_ppreduce(void)\n{")
        .count("HITMARKER_NO_ATTACKSTRING | HITMARKER_ATTACKSTRING_PRINTED") == 1
    ),
}

failed = [name for name, passed in checks.items() if not passed]
for name, passed in checks.items():
    print(f"{'PASS' if passed else 'FAIL'}: {name}")

if failed:
    raise SystemExit(f"{len(failed)} battle edge-mechanics checks failed")

print(f"PASS: {len(checks)} battle edge-mechanics checks")
