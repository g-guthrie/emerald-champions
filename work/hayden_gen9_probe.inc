#include "ec_review_common.inc"
#line 60000 "work/hayden_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Hayden: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 0 ? p ? 6 : 4 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_HAYDEN, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("HAYDEN_WEATHER weather=%d duration=%d", gBattleWeather, gBattleStruct->weatherDuration);
        if (!phase && board == 2 && turns == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_HOUNDOOM);
            EXPECT(opponentRight->hp > 0);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
        }
        if (!phase && board == 2 && turns == 2) {
            EXPECT(gBattleWeather & B_WEATHER_SUN);
            EXPECT_EQ(opponentRight->pp[0], 4);
        }
        if (phase && board == 1 && turns == 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_DEERLING);
            EXPECT(gBattleWeather & B_WEATHER_SUN);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 146);
            EXPECT(playerLeft->speed > opponentRight->speed);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_DOUBLE_EDGE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hayden: rain replacement and Taunt")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 2 : 4); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_RAIN_DANCE, MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0},
        };
        ReviewPlayer(board ? SPECIES_MIENFOO : SPECIES_SABLEYE, board ? &sReviewMienfoo : &sableye, 30);
        ReviewPlayer(SPECIES_LOMBRE, &sReviewLombre, 30);
        ReviewOpponent(TRAINER_HAYDEN, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board && t == 1) MOVE(playerLeft, MOVE_RAIN_DANCE);
            else if (!board) MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight, hit: TRUE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, 0);
        Test_MgbaPrintf("HAYDEN_WEATHER weather=%d duration=%d", gBattleWeather, gBattleStruct->weatherDuration);
        if (!board && turns == 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_TORKOAL);
            EXPECT(gBattleWeather & B_WEATHER_SUN);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_DROUGHT);
        }
        if (!board && turns == 3) {
            EXPECT_EQ(opponentLeft->species, SPECIES_DEERLING);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)), 146);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hayden timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_HAYDEN, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
