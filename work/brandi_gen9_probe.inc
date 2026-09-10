#include "ec_review_common.inc"
#line 60000 "work/brandi_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Brandi: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? (b == 2 ? 5 : 4) : b == 0 ? 6 : b == 1 ? 4 : 5); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_BRANDI, 2, phase);
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
        Test_MgbaPrintf("BRANDI_ROOM timer=%d PP=%d", gFieldTimers.trickRoomTimer, opponentLeft->pp[2]);
        if (!phase && board < 2 && turns <= 4) {
            EXPECT_EQ(opponentLeft->species, SPECIES_BRONZOR);
            EXPECT_EQ(opponentLeft->pp[2], 4);
            // A finishing KO exits before the final end-turn timer decrement.
            EXPECT(gFieldTimers.trickRoomTimer > 0);
        }
        if (!phase && board == 1 && turns == 3) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->pp[2], 19);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_DAZZLING_GLEAM);
        }
        if (!phase && board == 2 && turns == 3) {
            EXPECT_EQ(gFieldTimers.trickRoomTimer, 4);
            EXPECT_EQ(opponentLeft->pp[2], 4);
            EXPECT_EQ(opponentRight->species, SPECIES_MUSHARNA);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Brandi timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_BRANDI, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Brandi disruption: Taunt slower enemies and Dark reserves")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 1 ? 6 : b == 2 ? 3 : 5); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) {
            ReviewPlayer(SPECIES_MIENFOO, &sReviewMienfoo, 30);
            ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        } else if (board == 1) {
            ReviewPlayer(SPECIES_MUNCHLAX, &gEmeraldChampionsDefaultBattleSets[SPECIES_MUNCHLAX], 30);
            ReviewPlayer(SPECIES_FERROSEED, &gEmeraldChampionsDefaultBattleSets[SPECIES_FERROSEED], 30);
        } else {
            ReviewPlayer(SPECIES_SABLEYE, &gEmeraldChampionsDefaultBattleSets[SPECIES_SABLEYE], 30);
            ReviewPlayer(SPECIES_MURKROW, &gEmeraldChampionsDefaultBattleSets[SPECIES_MURKROW], 30);
        }
        ReviewOpponent(TRAINER_BRANDI, 2, board == 2);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                if (t < 2) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        ReviewResult(10 + board, turns, board == 2);
        Test_MgbaPrintf("BRANDI_DISRUPT timer=%d taunt=%d playerSpeed=%d/%d", gFieldTimers.trickRoomTimer,
            opponentLeft->volatiles.tauntTimer, playerLeft->speed, playerRight->speed);
        if (board == 1) {
            EXPECT_EQ(gFieldTimers.trickRoomTimer, 0);
            EXPECT_EQ(opponentLeft->pp[2], 5);
        }
        if (board == 0 && turns == 2) {
            EXPECT(opponentLeft->volatiles.tauntTimer);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_GYRO_BALL);
        }
        if (board == 2 && turns == 3) {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_DAZZLING_GLEAM);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_HELPING_HAND);
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(playerRight->hp, 0);
        }
    }
}
