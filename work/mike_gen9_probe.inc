#include "ec_review_common.inc"
#include "battle_main.h"
AI_DOUBLE_BATTLE_TEST("EC live review Mike: Surf protection and weather denial")
{
    u32 board = 0, turns = 1;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 3 : 4); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet lombre = {
            .moves = {MOVE_SURF, MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
            .ability = ABILITY_SWIFT_SWIM, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet oranguru = {
            .moves = {MOVE_PSYCHIC, MOVE_INSTRUCT, MOVE_TRICK_ROOM, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY,
            .ability = ABILITY_TELEPATHY, .nature = NATURE_CALM, .evs = {252, 0, 4, 0, 252, 0},
        };
        ReviewPlayer(SPECIES_LOMBRE, &lombre, 40); ReviewPlayer(SPECIES_ORANGURU, &oranguru, 40);
        ReviewOpponent(TRAINER_MIKE_2, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board && !t) MOVE(playerLeft, MOVE_RAIN_DANCE);
            else MOVE(playerLeft, MOVE_SURF, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70 + board, turns, 0);
        Test_MgbaPrintf("MIKE_WEATHER weather=%d timer=%d", gBattleWeather, gBattleStruct->weatherDuration);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Mike: opposing Intimidate and Defiant attacks")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet arbok = {
            .moves = {MOVE_GUNK_SHOT, MOVE_CRUNCH, MOVE_GLARE, MOVE_PROTECT}, .item = ITEM_BLACK_SLUDGE,
            .ability = ABILITY_INTIMIDATE, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(SPECIES_ARBOK, &arbok, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_MIKE_2, 3, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_GUNK_SHOT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
        }
    } SCENE { ABILITY_POPUP(opponentLeft, ABILITY_DEFIANT);
    } THEN {
        ReviewResult(72, turns, 1);
        if (turns == 1) {
            // Fainting resets Primeape's stages; verify its executed attack instead.
            EXPECT_EQ(opponentLeft->pp[0], 4);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Mike: sand pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    const u32 horizons[2][2][5] = {{{2,2,3,2,3},{2,1,3,2,2}},{{4,2,3,2,3},{2,2,3,2,3}}};
    for (u32 v = 0; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[v][p][b]; t++)
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
        ReviewOpponent(TRAINER_MIKE_2, visit ? 3 : 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: board == 4 && phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);

    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Mike timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 0; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_MIKE_2, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
