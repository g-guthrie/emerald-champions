#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/bryan_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Bryan: fossil setup and Contrary reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{3, 3, 3, 2, 3}, {3, 2, 3, 2, 3}};
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
        ReviewOpponent(TRAINER_BRYAN, 3, phase);
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
        if (turns == 1) Test_MgbaPrintf("BRYAN_PLAYER_SPEED species=%d/%d speed=%d/%d", playerLeft->species, playerRight->species,
            playerLeft->speed, playerRight->speed);
        Test_MgbaPrintf("BRYAN_SPEED actual=%d/%d", GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Bryan: Shell Smash opportunity against durable low-pressure foes")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet wartortle = {
            .moves = {MOVE_WATER_PULSE, MOVE_ICE_BEAM, MOVE_RAPID_SPIN, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_TORRENT, .nature = NATURE_BOLD, .evs = {252, 0, 252, 0, 4, 0},
        };
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewPlayer(SPECIES_WARTORTLE, &wartortle, 40);
        ReviewOpponent(TRAINER_BRYAN, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_WATER_PULSE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70, turns, 0);
        Test_MgbaPrintf("BRYAN_SMASH speed=%d", GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)));
        if (turns >= 2) {
            EXPECT_EQ(opponentRight->pp[0], 14);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], 8);
            EXPECT_EQ(opponentRight->statStages[STAT_DEF], 6);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], 6);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), 142);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Bryan timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_BRYAN, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
