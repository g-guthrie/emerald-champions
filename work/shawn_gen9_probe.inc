#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/shawn_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Shawn: terrain offense and Mega reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[3][5] = {{3, 2, 4, 5, 3}, {2, 3, 3, 2, 2}, {2, 2, 3, 2, 2}};
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_SHAWN, 2, phase);
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
        if (phase == 2 && board == 0) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentRight);
        }
        if (!phase && board == 1) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, opponentRight);
            if (turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponentRight);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("SHAWN_FIELD terrain=%d timer=%d charge=%d/%d speed=%d/%d nativeSpeed=%d/%d",
            gFieldTimers.terrain, gFieldTimers.terrainTimer, gBattleMons[1].volatiles.chargeTimer, gBattleMons[3].volatiles.chargeTimer,
            GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), gBattleMons[1].speed, gBattleMons[3].speed);
        if (opponentRight->species == SPECIES_AMPHAROS_MEGA)
            Test_MgbaPrintf("SHAWN_MEGA HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d",
                opponentRight->maxHP, opponentRight->attack, opponentRight->defense, opponentRight->spAttack,
                opponentRight->spDefense, opponentRight->speed, GetBattlerAbility(3));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Shawn: terrain against a faster incoming foe")
{
    u32 turns = 1;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 30);
        ReviewPlayer(SPECIES_GURDURR, &sReviewGurdurr, 30);
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 30);
        ReviewOpponent(TRAINER_SHAWN, 2, 0);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            SEND_OUT(playerRight, 2);
        }
        if (turns >= 2) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponentLeft);
        if (turns >= 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentRight);
            MESSAGE("Xatu fainted!");
        }
    } THEN {
        ReviewResult(10, turns, 0);
        Test_MgbaPrintf("SHAWN_SPEED terrain=%d raichu=%d xatu=%d", gFieldTimers.terrain,
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), playerRight->speed);
        EXPECT_EQ(opponentRight->speed, 102);
        EXPECT_EQ(GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), 204);
        EXPECT_EQ(opponentLeft->pp[2], 9);
        if (turns >= 2) EXPECT_EQ(playerRight->pp[0], 10);
        enum BattleTerrain savedTerrain = gFieldTimers.terrain;
        gFieldTimers.terrain = B_TERRAIN_GRASSY;
        EXPECT_EQ(GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), 102);
        gFieldTimers.terrain = savedTerrain;
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Shawn timing: six-player bench with Mega reserve")
{
    u32 phase = 0;
    for (u32 p = 0; p < 3; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_SHAWN, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
