#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Coby: pressure and reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[2][5] = {{3,4,3,2,2}, {2,4,2,2,2}};
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
        ReviewOpponent(TRAINER_COBY, 3, phase);
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
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("COBY_SPEED left=%d right=%d tailwind=%d", GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), gSideTimers[1].tailwindTimer);
        if (!phase && board == 3) {
            EXPECT_EQ(opponentLeft->pp[2], 14);
            EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 152);
            EXPECT_EQ(GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), 172);
            if (turns == 2) EXPECT_EQ(opponentRight->pp[2], 9);
        }
        if (phase && board == 3) {
            EXPECT_EQ(opponentRight->pp[2], 14);
            EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 206);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Coby: Competitive and critical reserve pressure")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 5; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LUM_BERRY, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_CALM, .evs = {252,0,4,0,252,0},
        };
        const struct EmeraldChampionsBattleSet dusclops = {
            .moves = {MOVE_NIGHT_SHADE, MOVE_WILL_O_WISP, MOVE_HAZE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_PRESSURE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        ReviewPlayer(board ? SPECIES_DUSCLOPS : SPECIES_UMBREON, board ? &dusclops : &umbreon, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_COBY, 3, TRUE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) MOVE(playerLeft, MOVE_NIGHT_SHADE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE, secondaryEffect: TRUE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board && turns >= 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SLASH, opponentLeft);
            MESSAGE("A critical hit!");
        }
    } THEN {
        ReviewResult(80 + board, turns, TRUE);
        Test_MgbaPrintf("COBY_FOES PP=%d/%d/%d/%d SpA=%d/%d", gBattleMons[0].pp[0], gBattleMons[0].pp[1], gBattleMons[0].pp[2], gBattleMons[0].pp[3],
            gBattleMons[0].statStages[STAT_SPATK], gBattleMons[2].statStages[STAT_SPATK]);
        Test_MgbaPrintf("COBY_PLAYER_MOVE %S / %S", GetMoveName(gLastMoves[0] < MOVES_COUNT ? gLastMoves[0] : MOVE_NONE), GetMoveName(gLastMoves[2] < MOVES_COUNT ? gLastMoves[2] : MOVE_NONE));
        if (!board && turns <= 2) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[0].pp[1], 14);
            if (turns == 2) EXPECT_EQ(opponentRight->pp[0], 13);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Coby timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_COBY, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
