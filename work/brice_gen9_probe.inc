#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Brice: Levitate and Fire pressure")
{
    u32 phase = 0, turns = 1;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 3 : 2); t++) PARAMETRIZE { phase = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet weezing = {
            .moves = {MOVE_FLAMETHROWER, MOVE_SLUDGE_BOMB, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_LEVITATE, .nature = NATURE_BOLD,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        const struct EmeraldChampionsBattleSet magcargo = {
            .moves = {MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_ANCIENT_POWER, MOVE_PROTECT},
            .item = ITEM_WHITE_HERB, .ability = ABILITY_FLAME_BODY, .nature = NATURE_MODEST,
            .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(SPECIES_WEEZING, &weezing, 40);
        ReviewPlayer(SPECIES_MAGCARGO, &magcargo, 40);
        ReviewOpponent(TRAINER_BRICE, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_HEAT_WAVE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (!phase && turns == 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HIGH_HORSEPOWER, opponentLeft);
            HP_BAR(playerRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAD_SMASH, opponentRight);
            HP_BAR(playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HIGH_HORSEPOWER, opponentLeft);
            HP_BAR(playerLeft);
        }
    } THEN {
        ReviewResult(70, turns, phase);
        if (!phase) EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Brice: physical setup and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    const u32 horizons[2][5] = {{4,2,5,4,5},{3,3,5,2,5}};
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
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
        ReviewOpponent(TRAINER_BRICE, visit ? 3 : 2, phase);
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
AI_DOUBLE_BATTLE_TEST("EC live review Brice timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_BRICE, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
