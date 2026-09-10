#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Wyatt: physical setup and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (p ? (b == 4 ? 2 : 3) : (b == 2 ? 5 : b == 0 || b == 4 ? 4 : 3)); t++)
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
        ReviewOpponent(TRAINER_WYATT, visit ? 3 : 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
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
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);
        if (phase && board == 0 && turns == 1) {
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_DEF], DEFAULT_STAT_STAGE);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
            EXPECT_EQ(gBattleMons[3].item, ITEM_NONE);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 148);
        }
        Test_MgbaPrintf("WYATT_WEATHER=%d duration=%d leftSpeed=%d rightSpeed=%d", gBattleWeather, gBattleStruct->weatherDuration,
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Wyatt: Fairy spread pressure")
{
    u32 phase = 0, turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 2 : 4); t++) PARAMETRIZE { phase = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sylveon = {
            .moves = {MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_CALM_MIND, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_PIXILATE, .nature = NATURE_MODEST, .evs = {252,0,4,252,0,0},
        };
        ReviewPlayer(SPECIES_SYLVEON, &sylveon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_WYATT, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN { ReviewResult(70, turns, phase); }
}
AI_DOUBLE_BATTLE_TEST("EC live review Wyatt timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_WYATT, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Wyatt: redirection and sustained physical pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet gurdurr = sReviewGurdurr;
        gurdurr.moves[1] = MOVE_MACH_PUNCH;
        ReviewPlayer(SPECIES_GURDURR, &gurdurr, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_WYATT, 3, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        ReviewResult(80, turns, 1);
        if (turns == 1) {
            EXPECT_EQ(gBattleMons[1].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)), 127);
        }
    }
}
