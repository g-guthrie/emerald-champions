#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Tori and Tia: dance and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{3,5,3,4,2}, {4,3,2,2,2}};
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
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
        ReviewOpponent(TRAINER_TORI_AND_TIA, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, phase ? MOVE_BITE : MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && board == 1 && turns == 2) {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_DANCER);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[1].pp[0], 19);
        }
        Test_MgbaPrintf("DANCE_SPEED left=%d right=%d tailwind=%d", 
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), gSideTimers[1].tailwindTimer);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Tori and Tia: bulky opposition and single Rock pressure")
{
    u32 board = 0, phase = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 p = (b ? 1 : 0); p < 2; p++) for (u32 t = 1; t <= (b || p ? 4 : 3); t++)
        PARAMETRIZE { board = b; phase = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_YAWN, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .nature = NATURE_CAREFUL, .ability = ABILITY_SYNCHRONIZE, .evs = {252,0,0,0,252,4},
        };
        const struct EmeraldChampionsBattleSet dewgong = {
            .moves = {MOVE_ICE_BEAM, MOVE_CHILLING_WATER, MOVE_ICY_WIND, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_MODEST, .ability = ABILITY_THICK_FAT, .evs = {252,0,4,252,0,0},
        };
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252},
        };
        ReviewPlayer(board ? SPECIES_DUGTRIO : SPECIES_UMBREON, board ? &dugtrio : &umbreon, 40);
        ReviewPlayer(board ? SPECIES_XATU : SPECIES_DEWGONG, board ? &sReviewXatu : &dewgong, 40);
        ReviewOpponent(TRAINER_TORI_AND_TIA, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) {
                MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                if (t == 0) MOVE(playerRight, MOVE_TAILWIND);
                else MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_BEAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(90 + board, turns, phase);
        if (!board && !phase && turns == 2) {
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_REVELATION_DANCE);
        }
        if (!board && phase && turns == 1) {
            EXPECT_EQ(gBattleMons[1].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[1].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[1].statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].pp[0], 19);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 148);
        }
        if (board && turns == 1) {
            EXPECT_EQ(gBattleMons[3].item, ITEM_NONE);
            EXPECT(gBattleMons[3].hp > 0);
            EXPECT_EQ(gBattleMons[3].pp[1], 9);
        }
        if (!board && ((phase && turns == 1) || (!phase && turns == 2))) ReviewTiming();
        Test_MgbaPrintf("DANCE_SPEED left=%d right=%d tailwind=%d",
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), gSideTimers[1].tailwindTimer);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Tori and Tia: Snarl disruption")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        ReviewPlayer(SPECIES_UMBREON, &umbreon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_TORI_AND_TIA, 3, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE, secondaryEffect: TRUE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(80, turns, 1);
        Test_MgbaPrintf("DANCE_SPEED left=%d right=%d tailwind=%d",
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), gSideTimers[1].tailwindTimer);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Tori and Tia timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_TORI_AND_TIA, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
