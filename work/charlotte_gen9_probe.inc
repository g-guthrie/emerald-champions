#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Charlotte: pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{3,3,4,3,3}, {2,3,2,2,3}};
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
        ReviewOpponent(TRAINER_CHARLOTTE, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("CHARLOTTE_ROOM timer=%d", gFieldTimers.trickRoomTimer);
        if (!phase) EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_FRIENDSHIP), 255);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Charlotte: bulky setup and sound pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (!p && !b ? 5 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_SNARL, MOVE_FOUL_PLAY, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        const struct EmeraldChampionsBattleSet sylveon = {
            .moves = {MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_CALM_MIND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_PIXILATE, .nature = NATURE_MODEST, .evs = {252,0,4,252,0,0},
        };
        ReviewPlayer(board ? SPECIES_SYLVEON : SPECIES_UMBREON, board ? &sylveon : &umbreon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_CHARLOTTE, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, board ? MOVE_HYPER_VOICE : MOVE_SNARL, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (phase) ABILITY_POPUP(opponentLeft, ABILITY_SOUNDPROOF);
    } THEN {
        ReviewResult(80 + board, turns, phase); Test_MgbaPrintf("CHARLOTTE_ROOM timer=%d", gFieldTimers.trickRoomTimer);
        if (!phase && board && turns <= 2) {
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->pp[0], 19);
            if (turns == 2) EXPECT_EQ(opponentRight->pp[1], 19);
        }
        if (phase && !board && turns == 1) EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Charlotte timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_CHARLOTTE, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        ReviewResult(99, 1, phase);
        if (!phase) EXPECT_EQ(IsAbilityPreventingEscape(B_BATTLER_0), B_BATTLER_1 + 1);
        ReviewTiming();
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Charlotte: overlapping Room setters")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_CHARLOTTE, 3, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon komala = party[1]; party[1] = party[3]; party[3] = komala;
        u16 zero = 0; SetMonData(&party[3], MON_DATA_HP, &zero);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(90, turns, FALSE);
        EXPECT_EQ(opponentLeft->pp[0] + opponentRight->pp[3], 9);
        EXPECT(gFieldTimers.trickRoomTimer > 0);
        if (turns == 1) ReviewTiming();
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Charlotte: Comatose resists burn disruption")
{
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_WILL_O_WISP, MOVE_FOUL_PLAY, MOVE_TAUNT, MOVE_RECOVER},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_PRANKSTER, .nature = NATURE_CAREFUL, .evs = {252,0,4,0,252,0},
        };
        ReviewPlayer(SPECIES_SABLEYE, &sableye, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_CHARLOTTE, 3, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(91, 1, FALSE);
        EXPECT_EQ(gBattleMons[0].pp[0], 14);
        EXPECT_EQ(opponentRight->species, SPECIES_KOMALA);
        EXPECT_EQ(opponentRight->status1, 0);
        EXPECT_EQ(IsAbilityPreventingEscape(B_BATTLER_0), 0);
    }
}
