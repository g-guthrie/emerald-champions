#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/branden_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Branden: Steely Spirit pressure and Storm Drain reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{3, 3, 3, 2, 2}, {3, 2, 3, 2, 2}};
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 40);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 40);
        ReviewOpponent(TRAINER_BRANDEN, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Branden: Fire pressure and native reserve transition")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_MAGMAR, &magmar, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_BRANDEN, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(71, turns, 0); }
}
AI_DOUBLE_BATTLE_TEST("EC live review Branden: Storm Drain redirects Water from Sirfetchd")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_MIENFOO, &sReviewMienfoo, 40);
        ReviewPlayer(SPECIES_LOMBRE, &sReviewLombre, 40);
        ReviewOpponent(TRAINER_BRANDEN, 3, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, t == 0 ? MOVE_FAKE_OUT : MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE { ABILITY_POPUP(opponentRight, ABILITY_STORM_DRAIN);
    } THEN {
        ReviewResult(70, turns, 1);
        EXPECT(opponentRight->statStages[STAT_SPATK] >= DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        if (turns == 1) EXPECT_EQ(opponentLeft->hp, 114);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Branden timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_BRANDEN, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        ReviewResult(99, 1, phase); ReviewTiming();
        if (!phase) {
            struct DamageContext ctx = {
                .battlerAtk = B_BATTLER_3, .battlerDef = B_BATTLER_0,
                .move = MOVE_ANCHOR_SHOT, .chosenMove = MOVE_ANCHOR_SHOT, .baseMove = MOVE_ANCHOR_SHOT,
                .moveType = TYPE_STEEL, .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
            };
            for (u32 i = 0; i < gBattlersCount; i++) {
                ctx.abilities[i] = GetBattlerAbility(i);
                ctx.holdEffects[i] = GetBattlerHoldEffect(i);
            }
            ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
            s32 linked = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            ctx.abilities[1] = ABILITY_NONE;
            s32 worker = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            ctx.abilities[3] = ABILITY_NONE;
            s32 ordinary = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            Test_MgbaPrintf("BRANDEN_STEEL linked=%d workerOnly=%d ordinary=%d", linked, worker, ordinary);
            EXPECT(linked > worker); EXPECT(worker > ordinary);
        }
    }
}
