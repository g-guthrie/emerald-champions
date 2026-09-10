#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Sophie: pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{2,2,2,4,2}, {4,8,4,3,4}};
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
        ReviewOpponent(TRAINER_SOPHIE, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: !phase && t >= 2 ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && board == 3 && turns == 3) {
            EXPECT_EQ(opponentLeft->species, SPECIES_PARASECT);
            EXPECT_EQ(opponentLeft->pp[3], 19);
            EXPECT_EQ(opponentLeft->hp, 1);
            ReviewTiming();
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sophie timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_SOPHIE, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sophie: reserve sleep and spread protection")
{
    u32 board = 0, turns = 0;
    const u8 horizons[] = {5,5,2,3};
    for (u32 b = 0; b < 4; b++) for (u32 t = 1; t <= horizons[b]; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        const struct EmeraldChampionsBattleSet torkoal = {
            .moves = {MOVE_HEAT_WAVE, MOVE_FLAMETHROWER, MOVE_BODY_PRESS, MOVE_PROTECT},
            .item = ITEM_CHARCOAL, .ability = ABILITY_WHITE_SMOKE, .nature = NATURE_MODEST, .evs = {252,0,4,252,0,0},
        };
        struct EmeraldChampionsBattleSet arcanine = torkoal;
        arcanine.ability = ABILITY_FLASH_FIRE;
        arcanine.moves[2] = MOVE_SNARL;
        const struct EmeraldChampionsBattleSet jumpluff = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_TAILWIND, MOVE_SLEEP_POWDER, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_CHLOROPHYLL, .nature = NATURE_TIMID, .evs = {252,0,4,0,0,252},
        };
        ReviewPlayer(board == 3 ? SPECIES_JUMPLUFF : board == 2 ? SPECIES_ARCANINE : board == 1 ? SPECIES_TORKOAL : SPECIES_UMBREON,
            board == 3 ? &jumpluff : board == 2 ? &arcanine : board == 1 ? &torkoal : &umbreon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_SOPHIE, 3, TRUE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 3) MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, board ? MOVE_HEAT_WAVE : MOVE_SNARL, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: board == 3 ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ReviewResult(80 + board, turns, TRUE);
        if (board == 1 && turns == 3) {
            EXPECT_EQ(opponentRight->pp[0], 14);
            EXPECT_EQ(opponentRight->pp[1], 9);
            EXPECT(opponentLeft->hp > 0);
            ReviewTiming();
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sophie: Sturdy Red Card against actual bench")
{
    GIVEN {
        const struct EmeraldChampionsBattleSet torkoal = {
            .moves = {MOVE_HEAT_WAVE, MOVE_FLAMETHROWER, MOVE_BODY_PRESS, MOVE_PROTECT},
            .item = ITEM_CHARCOAL, .ability = ABILITY_WHITE_SMOKE, .nature = NATURE_MODEST, .evs = {252,0,4,252,0,0},
        };
        ReviewPlayer(SPECIES_TORKOAL, &torkoal, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewOpponent(TRAINER_SOPHIE, 3, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        ReviewResult(90, 1, FALSE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), 1);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_NONE);
        EXPECT_EQ(gBattleMons[0].species, SPECIES_EEVEE);
        EXPECT_EQ(opponentLeft->pp[0], 19);
    }
}
