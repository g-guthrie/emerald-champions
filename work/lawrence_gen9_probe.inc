#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Lawrence: physical setup and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (b == 1 || (!p && b == 3) ? 2 : b == 0 ? (p ? 5 : 4) : b == 4 ? 4 : 3); t++)
        PARAMETRIZE { visit = v; phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, visit ? 40 : 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, visit ? 40 : 30);
        ReviewOpponent(TRAINER_LAWRENCE, visit ? 3 : 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE, criticalHit: FALSE);
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
        if (phase && board == 3 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_WIDE_GUARD, opponentRight);
        if (phase && board == 2 && turns >= 2) ABILITY_POPUP(opponentLeft, ABILITY_STORM_DRAIN);
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);
        Test_MgbaPrintf("LAWRENCE_WEATHER=%d duration=%d leftSpeed=%d rightSpeed=%d", gBattleWeather, gBattleStruct->weatherDuration,
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Lawrence: opposing rain and weatherless fallback")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet lombre = {
            .moves = {MOVE_RAIN_DANCE, MOVE_SCALD, MOVE_GIGA_DRAIN, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_SWIFT_SWIM, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet oranguru = {
            .moves = {MOVE_PSYCHIC, MOVE_INSTRUCT, MOVE_TRICK_ROOM, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_TELEPATHY, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(SPECIES_LOMBRE, &lombre, 40);
        ReviewPlayer(SPECIES_ORANGURU, &oranguru, 40);
        ReviewOpponent(TRAINER_LAWRENCE, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!t) MOVE(playerLeft, MOVE_RAIN_DANCE);
            else MOVE(playerLeft, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (turns >= 2) ABILITY_POPUP(opponentRight, ABILITY_STORM_DRAIN);
    } THEN {
        ReviewResult(70, turns, 0);
        if (turns == 1) EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 90);
        if (turns == 2) EXPECT_EQ(gBattleMons[3].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        if (turns == 3) EXPECT_EQ(gBattleMons[3].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        Test_MgbaPrintf("LAWRENCE_RAIN_CASE weather=%d duration=%d leftSpeed=%d rightSpeed=%d", gBattleWeather, gBattleStruct->weatherDuration,
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Lawrence timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_LAWRENCE, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
