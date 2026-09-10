#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/gabby1_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Gabby1: Tailwind offense and physical reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{4, 5, 3, 3, 3}, {2, 3, 2, 1, 2}};
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
        ReviewOpponent(TRAINER_GABBY_AND_TY_1, 3, phase);
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
    } SCENE {
        if (!phase && board == 1) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, opponentRight);
            if (turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, opponentLeft);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && board == 1 && turns <= 2) {
            EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 168);
            EXPECT_EQ(opponentRight->pp[2], 14);
        }
        Test_MgbaPrintf("GABBY_WIND turns=%d foeConfusion=%d/%d foeStatus=%d/%d", gSideTimers[1].tailwindTimer,
            playerLeft->volatiles.confusionTimer, playerRight->volatiles.confusionTimer, playerLeft->status1, playerRight->status1);
        Test_MgbaPrintf("GABBY_AND_TY_1_FIELD terrain=%d timer=%d charge=%d/%d speed=%d/%d nativeSpeed=%d/%d",
            gFieldTimers.terrain, gFieldTimers.terrainTimer, gBattleMons[1].volatiles.chargeTimer, gBattleMons[3].volatiles.chargeTimer,
            GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), gBattleMons[1].speed, gBattleMons[3].speed);

    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gabby1: Scrappy boundary and contact reserve")
{
    u32 contact = 0;
    PARAMETRIZE { contact = 0; }
    PARAMETRIZE { contact = 1; }
    GIVEN {
        const struct EmeraldChampionsBattleSet gengar = {
            .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_ENERGY_BALL, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_CURSED_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(contact ? SPECIES_MIENFOO : SPECIES_GENGAR, contact ? &sReviewMienfoo : &gengar, 40);
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewOpponent(TRAINER_GABBY_AND_TY_1, 3, contact);
    } WHEN {
        TURN {
            if (!contact) {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_PROTECT);
            } else {
                MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        ReviewResult(30 + contact, 1, contact);
        if (!contact) {
            struct DamageContext ctx = {
                .battlerAtk = B_BATTLER_1, .battlerDef = B_BATTLER_0,
                .move = MOVE_HYPER_VOICE, .chosenMove = MOVE_HYPER_VOICE, .baseMove = MOVE_HYPER_VOICE,
                .moveType = TYPE_NORMAL, .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
            };
            for (u32 i = 0; i < gBattlersCount; i++) {
                ctx.abilities[i] = GetBattlerAbility(i);
                ctx.holdEffects[i] = GetBattlerHoldEffect(i);
            }
            u32 scrappy = CalcTypeEffectivenessMultiplier(&ctx);
            ctx.abilities[1] = ABILITY_SOUNDPROOF;
            u32 ordinary = CalcTypeEffectivenessMultiplier(&ctx);
            Test_MgbaPrintf("GABBY_SCRAPPY normalGhostWithScrappy=%d without=%d", scrappy, ordinary);
            EXPECT(scrappy > 0);
            EXPECT_EQ(ordinary, 0);
        } else Test_MgbaPrintf("GABBY_CONTACT foeStatus=%d/%d", playerLeft->status1, playerRight->status1);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Gabby1 timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_GABBY_AND_TY_1, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
