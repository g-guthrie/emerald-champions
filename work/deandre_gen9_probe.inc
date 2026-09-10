#include "ec_review_common.inc"
#line 60000 "work/deandre_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Deandre: pressure and reserve decisions")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (!p ? (b == 1 ? 4 : 6) : b == 1 ? 2 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_DEANDRE, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { ReviewResult(board, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Deandre: Water Fairy and Taunt")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (!p ? (b == 2 ? 4 : 5) : b == 2 ? 2 : b == 1 ? 4 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet gardevoir = {
            .moves = {MOVE_DAZZLING_GLEAM, MOVE_PSYCHIC, MOVE_MOONBLAST, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_TELEPATHY, .nature = NATURE_MODEST, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_LOMBRE : board == 1 ? SPECIES_GARDEVOIR : SPECIES_MIENFOO,
            board == 0 ? &sReviewLombre : board == 1 ? &gardevoir : &sReviewMienfoo, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_DEANDRE, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (board == 1) MOVE(playerLeft, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight, hit: TRUE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (phase && board == 0 && turns >= 2) {
            ABILITY_POPUP(opponentRight, ABILITY_STORM_DRAIN);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTH_POWER, opponentRight);
        }
    } THEN {
        ReviewResult(10 + board, turns, phase);
        if (phase && board == 0 && turns == 2) {
            EXPECT_EQ(opponentLeft->hp, 88);
            EXPECT_EQ(opponentRight->hp, 126);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->pp[0], 9);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 1);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Deandre: Grass counterplay and powder immunity")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++)
        PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet vivillon = {
            .moves = {MOVE_SLEEP_POWDER, MOVE_BUG_BUZZ, MOVE_HURRICANE, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_COMPOUND_EYES, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board ? SPECIES_VIVILLON : SPECIES_LOMBRE, board ? &vivillon : &sReviewLombre, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_DEANDRE, 2, board ? 0 : 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board) MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_SLEEP_POWDER, target: opponentRight, hit: TRUE);
            else MOVE(playerLeft, MOVE_BUG_BUZZ, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (!board) MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
            else MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board) ABILITY_POPUP(opponentRight, ABILITY_OVERCOAT);
        else if (turns >= 2) ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponentRight);
    } THEN {
        ReviewResult(20 + board, turns, board ? 0 : 1);
        if (board && turns == 1) EXPECT_EQ(opponentRight->status1, 0);
        if (!board && turns == 2) {
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_GT(opponentRight->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Deandre timing: healthy six-player bench")
{
    u32 phase = 0;
    PARAMETRIZE { phase = 0; }
    PARAMETRIZE { phase = 1; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_DEANDRE, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
