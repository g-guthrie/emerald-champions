#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Larry: physical setup and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (b == 0 ? (p ? 5 : 4) : b == 2 ? 3 : 2); t++)
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
        ReviewOpponent(TRAINER_LARRY, visit ? 3 : 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: board == 4 && !phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Larry timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_LARRY, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
AI_DOUBLE_BATTLE_TEST("EC live review Larry: Water Absorb and Sap Sipper control")
{
    u32 phase = 0, turns = 0;
    PARAMETRIZE { phase = 0; turns = 1; }
    PARAMETRIZE { phase = 0; turns = 2; }
    PARAMETRIZE { phase = 1; turns = 1; }
    GIVEN {
        const struct EmeraldChampionsBattleSet starmie = {
            .moves = {MOVE_SCALD, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_NATURAL_CURE, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewPlayer(phase ? SPECIES_LOMBRE : SPECIES_STARMIE, phase ? &sReviewLombre : &starmie, 40);
        ReviewOpponent(TRAINER_LARRY, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, phase ? MOVE_BITE : MOVE_DOUBLE_EDGE, target: phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, phase ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (phase) ABILITY_POPUP(opponentLeft, ABILITY_SAP_SIPPER);
        else if (turns == 2) ABILITY_POPUP(opponentLeft, ABILITY_WATER_ABSORB);
    } THEN {
        ReviewResult(80, turns, phase);
        if (!phase) {
            EXPECT_EQ(gBattleMons[2].pp[0], GetMovePP(MOVE_SCALD) - turns);
        } else {
            EXPECT_EQ(gBattleMons[1].hp, gBattleMons[1].maxHP);
            EXPECT(gBattleMons[1].statStages[STAT_ATK] >= DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].item, ITEM_NONE);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Larry: reserve setup opportunity and payoff")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sReviewMienfoo;
        mienfoo.moves[3] = MOVE_PROTECT;
        ReviewPlayer(SPECIES_MIENFOO, &mienfoo, 40);
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 40);
        ReviewOpponent(TRAINER_LARRY, 3, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!t) { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
            else {
                MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { ReviewResult(81, turns, 1); }
}
