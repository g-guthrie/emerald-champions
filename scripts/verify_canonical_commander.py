#!/usr/bin/env python3
"""Prove Emerald Champions' canonical Commander integration is intact."""

from __future__ import annotations

import hashlib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(f"FAIL: {message}")


def require_tokens(source: str, tokens: tuple[str, ...], label: str) -> None:
    for token in tokens:
        require(token in source, f"{label} missing {token}")


def main() -> None:
    battle_h = read("include/battle.h")
    battle_constants = read("include/constants/battle.h")
    util = read("src/battle_util.c")
    commands = read("src/battle_script_commands.c")
    main_source = read("src/battle_main.c")
    ai = read("src/battle_ai_main.c") + read("src/battle_ai_util.c")
    scripts = read("data/battle_scripts_1.s")
    anim_scripts = read("data/battle_anim_scripts.s")
    anim_source = read("src/battle_anim_new.c")
    graphics_source = read("src/graphics.c")

    require_tokens(
        battle_h + battle_constants,
        (
            "STATUS4_COMMANDER",
            "u8 commandingDondozo;",
            "u8 commanderReleasePending;",
            "u16 commanderActive[MAX_BATTLERS_COUNT];",
        ),
        "explicit Commander state",
    )
    require_tokens(
        util,
        (
            "TryActivateCommander",
            "gBattleMons[dondozo].species != SPECIES_DONDOZO",
            "GET_BASE_SPECIES_ID(GetBattlerOriginalSpecies(commander)) != SPECIES_TATSUGIRI",
            "gBattleStruct->commandingDondozo |= gBitTable[commander]",
            "gBattleStruct->commanderActive[dondozo] = gBattleMons[commander].species",
            "gStatuses4[commander] |= STATUS4_COMMANDER",
            "gActionsByTurnOrder[turnOrderId] = B_ACTION_NOTHING_FAINTED",
            "BtlController_EmitSpriteInvisibility(0, TRUE)",
            "BattleScript_CommanderActivates",
            "ReleaseFaintedCommanders",
            "case ABILITY_COMMANDER:",
        ),
        "Commander activation and release",
    )
    require_tokens(
        scripts,
        (
            "BattleScript_CommanderActivates::",
            "playanimation BS_SCRIPTING, B_ANIM_COMMANDER, NULL",
            "setstatchanger STAT_ATK, 2, FALSE",
            "setstatchanger STAT_DEF, 2, FALSE",
            "setstatchanger STAT_SPATK, 2, FALSE",
            "setstatchanger STAT_SPDEF, 2, FALSE",
            "setstatchanger STAT_SPEED, 2, FALSE",
            "copybyte gBattlerAttacker, sSAVED_BATTLER",
        ),
        "Commander battle script",
    )
    require_tokens(
        commands + main_source,
        (
            "IsCommanderTatsugiri(gBattlerTarget)",
            "gBattleMoves[move == NO_ACC_CALC_CHECK_LOCK_ON ? gCurrentMove : move].effect != EFFECT_TRANSFORM",
            "IsCommanderTatsugiri(gActiveBattler)",
            "IsCommandedDondozo(battlerId)",
            "CanBattlerSwitch(battler)",
            "ReleaseFaintedCommanders();",
            "QueueCommanderRelease(gActiveBattler);",
            "ReleaseCommander(gActiveBattler);",
        ),
        "Commander targeting, action, switch, and cleanup rules",
    )
    require_tokens(
        commands,
        (
            "gBattleStruct->commanderActive[gBattlerAttacker]",
            "case SPECIES_TATSUGIRI_DROOPY:",
            "case SPECIES_TATSUGIRI_STRETCHY:",
            "case SPECIES_TATSUGIRI:",
            "|| IsCommanderTatsugiri(i)",
        ),
        "Order Up and Perish Song rules",
    )
    require_tokens(
        ai,
        (
            "IsCommanderTatsugiri(battlerDef)",
            "gBattleStruct->commanderActive[battlerAtk] == SPECIES_TATSUGIRI_STRETCHY",
        ),
        "Commander AI",
    )

    old_damage_hook = (
        "case ABILITY_COMMANDER:\n"
        "            if (gBattleMons[battlerAtk].species == SPECIES_DONDOZO)\n"
        "                MulModifier(&modifier, UQ_4_12(1.5));"
    )
    require(old_damage_hook not in util, "obsolete visible-partner 1.5x damage hook remains")

    expected_assets = {
        "graphics/battle_anims/sprites/tatsugiri_curly.png": "07b8cc3713ec38803b19f4695d546f2c9d0726d3fe795a295b5c4c005ee60c26",
        "graphics/battle_anims/sprites/tatsugiri_droopy.png": "28f9ee19d4d73d4e2e61be2bf16cc38a6024acf7f52bcfbeaff2360cc04e155a",
        "graphics/battle_anims/sprites/tatsugiri_stretchy.png": "3e7261d0bee695d01b0c800cc20b4207e62e671c2d34f0c20576227a7041ed2f",
    }
    for path, expected_hash in expected_assets.items():
        asset = ROOT / path
        require(asset.is_file(), f"missing Expansion asset {path}")
        require(hashlib.sha256(asset.read_bytes()).hexdigest() == expected_hash, f"Expansion asset drifted: {path}")

    require_tokens(
        graphics_source + anim_source + anim_scripts,
        (
            "gBattleAnimSpriteGfx_TatsugiriCurly",
            "gCommanderTatsugiriCurlySpriteTemplate",
            "gOrderUpTatsugiriStretchySpriteTemplate",
            "AnimTask_GetCommanderType",
            ".anims = gDummySpriteAnimTable",
            ".affineAnims = gDummySpriteAffineAnimTable",
            "General_Commander:",
            "Move_ORDER_UP:",
            "Move_OrderUpAllForms:",
            "Move_OrderUpCurly:",
            "Move_OrderUpDroopy:",
            "Move_OrderUpStretchy:",
            ".4byte Move_ORDER_UP         @ Order Up",
        ),
        "Commander and Order Up presentation",
    )

    print("PASS: canonical Commander mechanics, AI, cleanup, and Expansion presentation are source-complete")


if __name__ == "__main__":
    main()
