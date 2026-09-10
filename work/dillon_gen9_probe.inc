#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Dillon: pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{5,2,3,2,3}, {4,2,2,2,2}};
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
        ReviewOpponent(TRAINER_DILLON, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: (board == 4 && phase) ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && board == 0 && turns == 4) {
            EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->pp[1], 8);
        }
        if (phase && board == 3 && turns == 2) {
            EXPECT_EQ(opponentLeft->pp[3], 9);
            EXPECT_EQ(opponentLeft->hp, 18);
        }
        if (!phase && board == 4 && turns == 2) {
            EXPECT(gBattleMons[2].volatiles.saltCure);
            EXPECT_EQ(opponentRight->pp[0], 14);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dillon: bulky targets")
{
    u32 phase = 0, turns = 0, snarl = 0;
    for (u32 s = 0; s < 2; s++) for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (s ? 2 : 4); t++)
        PARAMETRIZE { phase = p; turns = t; snarl = s; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        const struct EmeraldChampionsBattleSet dewgong = {
            .moves = {MOVE_ICE_BEAM, MOVE_SURF, MOVE_ENCORE, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_THICK_FAT, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        ReviewPlayer(SPECIES_UMBREON, &umbreon, 40);
        ReviewPlayer(SPECIES_DEWGONG, &dewgong, 40);
        ReviewOpponent(TRAINER_DILLON, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, snarl ? MOVE_SNARL : MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SURF, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ReviewResult(80 + snarl, turns, phase);
        Test_MgbaPrintf("DILLON_STATE cure=%d/%d speed=%d/%d", gBattleMons[0].volatiles.saltCure, gBattleMons[2].volatiles.saltCure,
            GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dillon: Thermal Exchange fire counter")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet torkoal = {
            .moves = {MOVE_FLAMETHROWER, MOVE_HEAT_WAVE, MOVE_YAWN, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_DROUGHT, .nature = NATURE_MODEST, .evs = {252,0,4,252,0,0},
        };
        ReviewPlayer(SPECIES_TORKOAL, &torkoal, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_DILLON, 3, TRUE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: TRUE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ReviewResult(90, turns, TRUE);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        EXPECT(opponentRight->hp > 0);
        if (turns >= 2) EXPECT(opponentRight->pp[1] < 20);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dillon: physical bulk and setup opportunity")
{
    u32 phase = 0, turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { phase = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        ReviewPlayer(SPECIES_UMBREON, &umbreon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_DILLON, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ReviewResult(85, turns, phase);
        Test_MgbaPrintf("DILLON_SETUP speed=%d/%d", GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dillon timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_DILLON, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
