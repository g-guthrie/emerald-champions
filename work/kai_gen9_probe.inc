#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Kai: pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{4,3,4,2,3}, {2,3,3,2,3}};
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
        ReviewOpponent(TRAINER_KAI, 3, phase);
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
        Test_MgbaPrintf("KAI_MECHANICS trap=%d/%d Def=%d/%d SpD=%d/%d types=%d/%d protean=%d", gBattleMons[0].volatiles.octolock, gBattleMons[2].volatiles.octolock, gBattleMons[0].statStages[STAT_DEF], gBattleMons[2].statStages[STAT_DEF], gBattleMons[0].statStages[STAT_SPDEF], gBattleMons[2].statStages[STAT_SPDEF], gBattleMons[1].types[0], gBattleMons[1].types[1], gBattleMons[1].volatiles.usedProteanLibero);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kai timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_KAI, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
AI_DOUBLE_BATTLE_TEST("EC live review Kai: bulky pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][2] = {{5,5}, {3,2}};
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        const struct EmeraldChampionsBattleSet dusclops = {
            .moves = {MOVE_NIGHT_SHADE, MOVE_WILL_O_WISP, MOVE_PAIN_SPLIT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_PRESSURE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        ReviewPlayer(board ? SPECIES_DUSCLOPS : SPECIES_UMBREON, board ? &dusclops : &umbreon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_KAI, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, board ? MOVE_NIGHT_SHADE : MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, board ? MOVE_SUPER_FANG : MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(80 + board, turns, phase);
        Test_MgbaPrintf("KAI_TRAP left=%d right=%d Def=%d/%d SpD=%d/%d", gBattleMons[0].volatiles.octolock, gBattleMons[2].volatiles.octolock, gBattleMons[0].statStages[STAT_DEF], gBattleMons[2].statStages[STAT_DEF], gBattleMons[0].statStages[STAT_SPDEF], gBattleMons[2].statStages[STAT_SPDEF]);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kai: revealed guards and Octolock")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_KAI, 3, FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
            else {
                MOVE(playerLeft, MOVE_HIGH_JUMP_KICK, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, t == 1 ? MOVE_PSYCHIC : MOVE_EARTH_POWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                if (t == 1) SEND_OUT(playerRight, 2);
                if (t == 2) SEND_OUT(playerLeft, 3);
            }
        }
    } THEN {
        ReviewResult(90, turns, FALSE);
        Test_MgbaPrintf("KAI_TRAP left=%d right=%d Def=%d/%d SpD=%d/%d", gBattleMons[0].volatiles.octolock, gBattleMons[2].volatiles.octolock, gBattleMons[0].statStages[STAT_DEF], gBattleMons[2].statStages[STAT_DEF], gBattleMons[0].statStages[STAT_SPDEF], gBattleMons[2].statStages[STAT_SPDEF]);
        if (turns == 2) ReviewTiming();
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kai: native defensive abilities")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_THUNDER_WAVE, MOVE_FOUL_PLAY, MOVE_TAUNT, MOVE_RECOVER},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_PRANKSTER, .nature = NATURE_CAREFUL, .evs = {252,0,4,0,252,0},
        };
        const struct EmeraldChampionsBattleSet roserade = {
            .moves = {MOVE_SLUDGE_BOMB, MOVE_GIGA_DRAIN, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM},
            .item = ITEM_YACHE_BERRY, .ability = ABILITY_NATURAL_CURE, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        ReviewPlayer(phase ? SPECIES_ROSERADE : SPECIES_SABLEYE, phase ? &roserade : &sableye, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_KAI, 3, phase);
    } WHEN {
        for (u32 t = 0; t < (phase ? 2 : 1); t++) TURN {
            MOVE(playerLeft, phase ? MOVE_SLUDGE_BOMB : MOVE_THUNDER_WAVE, target: phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: phase ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE { ABILITY_POPUP(phase ? opponentRight : opponentLeft, phase ? ABILITY_BULLETPROOF : ABILITY_LIMBER);
    } THEN {
        ReviewResult(91, phase ? 2 : 1, phase);
        if (!phase) EXPECT_EQ(opponentLeft->status1, 0);
    }
}
