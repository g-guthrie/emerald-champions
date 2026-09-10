#include "ec_review_common.inc"
#line 62000 "work/victor_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Victor: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? b == 2 ? 3 : b == 1 ? 4 : 5 : b == 1 ? 2 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_VICTOR, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("VICTOR_WIND timer=%d foeAtk=%d/%d", gSideTimers[1].tailwindTimer,
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK]);
        if (phase && turns <= 2) {
            EXPECT(opponentLeft->status1 & STATUS1_TOXIC_POISON);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)), 142);
        }
        if (!phase && board == 0) EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_PP1), 15);
        if (!phase && board == 1 && turns == 1) {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_TAILWIND);
            EXPECT_EQ(gSideTimers[1].tailwindTimer, 3);
            EXPECT(opponentLeft->hp > 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Victor: Ghost burn and Taunt disruption")
{
    u32 turns = 0, board = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 2 : 5); t++) PARAMETRIZE { turns = t; board = b; }
    GIVEN {
        ReviewPlayer(board ? SPECIES_MIENFOO : SPECIES_SABLEYE,
            board ? &sReviewMienfoo : &gEmeraldChampionsDefaultBattleSets[SPECIES_SABLEYE], 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_VICTOR, 2, board ? 0 : 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) {
                if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft, hit: TRUE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
                else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, board ? 0 : 1);
        if (!board && turns == 1) {
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(opponentRight->status1, 0);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        }
        if (!board && turns == 2) {
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_BODY_SLAM);
            EXPECT(playerLeft->hp < playerLeft->maxHP);
            EXPECT_EQ(opponentRight->pp[3], 9);
        }
        if (!board && turns == 5) EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        // Staravia83 is faster than this legal level30 Mienfoo79: Taunt lands after Tailwind.
        if (board && turns == 1) {
            EXPECT_EQ(gSideTimers[1].tailwindTimer, 3);
            EXPECT(opponentLeft->volatiles.tauntTimer > 0);
        }
        if (board && turns == 2) EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_PP4), 19);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Victor: faster neutral attacking pair")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet tauros = {
            .moves = {MOVE_BODY_SLAM, MOVE_ZEN_HEADBUTT, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_ANGER_POINT, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 30);
        ReviewPlayer(SPECIES_TAUROS, &tauros, 30);
        ReviewOpponent(TRAINER_VICTOR, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(12, turns, 0);
        Test_MgbaPrintf("VICTOR_FAST timer=%d", gSideTimers[1].tailwindTimer);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_PP1), 14);
        if (turns == 1) EXPECT_EQ(gSideTimers[1].tailwindTimer, 3);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Victor timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_VICTOR, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
