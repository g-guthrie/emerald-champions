#include "ec_review_common.inc"
#line 60000 "work/bianca_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Bianca: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? b == 2 ? 4 : 6 : b == 0 ? 4 : b == 1 ? 3 : 5); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_BIANCA, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (phase && board == 0 && turns == 3) {
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_POLLEN_PUFF);
            EXPECT(opponentLeft->hp > 80);
        }
        if (phase && board == 1 && turns == 1) {
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_QUIVER_DANCE);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 121);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Bianca: Grass pivot and Taunt")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 4; b++) for (u32 t = 1; t <= (b == 1 ? 5 : b == 2 ? 4 : b == 3 ? 4 : 2); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet grovyle = {
            .moves = {MOVE_LEAF_BLADE, MOVE_ACROBATICS, MOVE_QUICK_ATTACK, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_OVERGROW, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board % 2 ? SPECIES_MIENFOO : SPECIES_GROVYLE, board % 2 ? &sReviewMienfoo : &grovyle, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_BIANCA, 2, board >= 2);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board % 2 && t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight, hit: TRUE);
            else MOVE(playerLeft, board % 2 ? MOVE_DRAIN_PUNCH : MOVE_LEAF_BLADE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, board >= 2);
        Test_MgbaPrintf("BIANCA_PLAYER_STATUS left=%d right=%d", playerLeft->status1, playerRight->status1);
        if (board == 2 && turns == 2) {
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_SAP_SIPPER);
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentLeft->pp[1], 19);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Bianca: Ice coverage pressure")
{
    u32 turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { phase = p; turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 30);
        ReviewPlayer(SPECIES_GURDURR, &sReviewGurdurr, 30);
        ReviewOpponent(TRAINER_BIANCA, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(20, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Bianca timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_BIANCA, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
