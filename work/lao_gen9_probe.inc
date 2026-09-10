#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Lao: pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{5,3,3,2,3}, {2,2,3,2,3}};
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
        ReviewOpponent(TRAINER_LAO_1, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
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
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("LAO_TYPES left=%d/%d right=%d/%d", gBattleMons[1].types[0], gBattleMons[1].types[1], gBattleMons[3].types[0], gBattleMons[3].types[1]);
        if (!phase && board == 3 && turns == 1) {
            EXPECT_EQ(opponentRight->pp[1], 9);
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        }
        if (!phase && board == 2 && turns >= 2) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            if (turns == 3) EXPECT_EQ(opponentRight->pp[0], 13);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Lao timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_LAO_1, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        ReviewResult(99, 1, phase); ReviewTiming();
        if (!phase) EXPECT_EQ(GetIllusionMonSpecies(B_BATTLER_1), SPECIES_PANGORO);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lao: Fairy counter and Roseli")
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
        ReviewOpponent(TRAINER_LAO_1, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(70, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lao: single Fairy hit reserve contribution")
{
    u32 turns = 0, oldSpread = 0;
    for (u32 old = 0; old < 2; old++) for (u32 t = 1; t <= (old ? 1 : 3); t++) PARAMETRIZE { turns = t; oldSpread = old; }
    GIVEN {
        const struct EmeraldChampionsBattleSet clefairy = {
            .moves = {MOVE_MOONBLAST, MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FRIEND_GUARD, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        ReviewPlayer(SPECIES_CLEFAIRY, &clefairy, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_LAO_1, 3, TRUE);
        if (oldSpread) {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            const u8 atk = 252, def = 4, spdef = 0;
            EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_PANGORO);
            SetMonData(&party[1], MON_DATA_ATK_EV, &atk);
            SetMonData(&party[1], MON_DATA_DEF_EV, &def);
            SetMonData(&party[1], MON_DATA_SPDEF_EV, &spdef);
            CalculateMonStats(&party[1]);
        }
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(80 + oldSpread, turns, TRUE);
        if (turns == 1) {
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT(opponentRight->hp > 0);
            EXPECT_EQ(opponentRight->pp[1], 19);
        }
    }
}
