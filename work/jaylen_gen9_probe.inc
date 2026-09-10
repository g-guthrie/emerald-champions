#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Jaylen: snow pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (p ? (b == 0 || b == 2 ? 3 : 2) : b == 0 || b == 1 || b == 3 ? 3 : 2); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 40);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 40);
        ReviewOpponent(TRAINER_JAYLEN, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("JAYLEN_WEATHER=%d duration=%d leftSpeed=%d rightSpeed=%d", gBattleWeather, gBattleStruct->weatherDuration,
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Jaylen timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_JAYLEN, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaylen: sustained physical and rain disruption")
{
    u32 rain = 0, turns = 0;
    for (u32 r = 0; r < 2; r++) for (u32 t = 1; t <= (r ? 4 : 5); t++) PARAMETRIZE { rain = r; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet politoed = {
            .moves = {MOVE_SCALD, MOVE_ICE_BEAM, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_DRIZZLE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        ReviewPlayer(rain ? SPECIES_POLITOED : SPECIES_GURDURR, rain ? &politoed : &sReviewGurdurr, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_JAYLEN, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (rain && t == 0) MOVE(playerLeft, MOVE_PROTECT);
            else MOVE(playerLeft, rain ? MOVE_SCALD : MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        ReviewResult(80 + rain, turns, 0);
        Test_MgbaPrintf("JAYLEN_WEATHER=%d duration=%d", gBattleWeather, gBattleStruct->weatherDuration);
        if (turns == 1) EXPECT(gBattleWeather & B_WEATHER_SNOW);
        if (!rain && turns == 2) EXPECT_EQ(gBattleMons[3].statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        if (turns == 2) ReviewTiming();
    }
}
