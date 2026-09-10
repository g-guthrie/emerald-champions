#include "ec_review_common.inc"
#include "battle_main.h"
AI_DOUBLE_BATTLE_TEST("EC live review Georgia: damaged partner healing opportunity")
{
    u32 partner = 0, turns = 1;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 5; t++) PARAMETRIZE { partner = p; turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewPlayer(SPECIES_LOMBRE, &sReviewLombre, 40);
        ReviewOpponent(TRAINER_GEORGIA, 3, 0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon mon = party[0]; party[0] = party[2]; party[2] = mon;
        if (!partner) { mon = party[1]; party[1] = party[3]; party[3] = mon; }
        u16 hp = 97; SetMonData(&party[0], MON_DATA_HP, &hp);
        hp = partner ? 40 : 20; SetMonData(&party[1], MON_DATA_HP, &hp);
        u16 item = ITEM_NONE; SetMonData(&party[1], MON_DATA_HELD_ITEM, &item);
        for (u32 i = 2; i < 4; i++) { hp = 0; SetMonData(&party[i], MON_DATA_HP, &hp); }
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(72 + partner, turns, 2);
        if (turns == 1) {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_LIFE_DEW);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_PROTECT);
            EXPECT_EQ(opponentLeft->hp, 84);
            EXPECT_EQ(opponentRight->hp, partner ? 72 : 47);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Georgia: Hattrem Metang support and reflection")
{
    u32 board = 0, turns = 1;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 3 : 2); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_MIENFOO, &sReviewMienfoo, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_GEORGIA, 3, 0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon mon = party[0]; party[0] = party[2]; party[2] = mon;
        for (u32 i = 2; i < 4; i++) { u16 zero = 0; SetMonData(&party[i], MON_DATA_HP, &zero); }
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board && t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board) { ABILITY_POPUP(opponentLeft, ABILITY_MAGIC_BOUNCE); }
    } THEN {
        ReviewResult(70 + board, turns, 2);
        EXPECT_EQ(opponentLeft->speed, 55);
        EXPECT_EQ(opponentRight->speed, 54);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Georgia: powder support and Policy reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{3, 4, 3, 3, 2}, {4, 5, 5, 5, 5}};
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 40);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 40);
        ReviewOpponent(TRAINER_GEORGIA, 3, phase);
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
        ReviewResult(board, turns, phase);
        if (!phase && board == 3 && turns == 1) {
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT(opponentRight->statStages[STAT_ATK] >= DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_METEOR_MASH);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Georgia timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_GEORGIA, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
