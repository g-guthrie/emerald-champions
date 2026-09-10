#include "ec_review_common.inc"
#line 61000 "work/melina_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Melina: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? (b == 2 ? 2 : 3) : b == 2 ? 3 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_MELINA, 2, phase);
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
        Test_MgbaPrintf("MELINA_SPEED raw=%d/%d stages=%d/%d foes=%d/%d", opponentLeft->speed, opponentRight->speed,
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED], playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED]);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Melina Costar: fast opposing replacement")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet tauros = {
            .moves = {MOVE_BODY_SLAM, MOVE_ZEN_HEADBUTT, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_ANGER_POINT, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewPlayer(SPECIES_TAUROS, &tauros, 30);
        ReviewOpponent(TRAINER_MELINA, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t < 2) MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else MOVE(playerLeft, MOVE_ZEN_HEADBUTT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (t == 1) SEND_OUT(playerLeft, 2);
        }
    } THEN {
        ReviewResult(10, turns, 0);
        Test_MgbaPrintf("MELINA_COSTAR stages=%d/%d raw=%d/%d foeSpeed=%d", opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED], opponentLeft->speed, opponentRight->speed, playerLeft->speed);
        if (turns == 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_FLAMIGO);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_COSTAR);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(playerLeft->speed, 108);
            EXPECT_EQ(opponentRight->speed, 81);
        }
        if (turns == 3) {
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_PP2), 4);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Melina Wide Guard: Rock spread pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet aerodactyl = {
            .moves = {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_CRUNCH, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_ROCK_HEAD, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(SPECIES_AERODACTYL, &aerodactyl, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_MELINA, 2, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(11, turns, 1);
        if (turns == 3) EXPECT(playerLeft->hp < playerLeft->maxHP);
        if (turns == 4) {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_WIDE_GUARD);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_ACROBATICS);
            EXPECT_EQ(opponentLeft->hp, 27);
            EXPECT_EQ(opponentRight->hp, 29);
        }
        if (turns == 5) EXPECT_EQ(playerLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Melina reserves: Ghost and burn pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_SABLEYE, &gEmeraldChampionsDefaultBattleSets[SPECIES_SABLEYE], 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_MELINA, 2, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(12, turns, 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Melina timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_MELINA, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
