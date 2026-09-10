#include "ec_review_common.inc"
#line 63000 "work/victoria_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Victoria: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? b == 0 ? 6 : 4 : b == 1 ? 4 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_VICTORIA, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (phase && board == 0 && turns == 6) EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Victoria: bulky Ice and focused Fire pressure")
{
    u32 turns = 0, board = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; board = b; }
    GIVEN {
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_FOUL_PLAY, MOVE_YAWN, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .nature = NATURE_CAREFUL, .ability = ABILITY_SYNCHRONIZE, .evs = {252, 0, 0, 0, 252, 4},
        };
        const struct EmeraldChampionsBattleSet dewgong = {
            .moves = {MOVE_ICE_BEAM, MOVE_CHILLING_WATER, MOVE_ICY_WIND, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_MODEST, .ability = ABILITY_THICK_FAT, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_SCORCHING_SANDS, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_FLAME_BODY, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board ? SPECIES_MAGMAR : SPECIES_UMBREON, board ? &magmar : &umbreon, 30);
        ReviewPlayer(board ? SPECIES_PACHIRISU : SPECIES_DEWGONG, board ? &sReviewPachirisu : &dewgong, 30);
        ReviewOpponent(TRAINER_VICTORIA, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, board ? MOVE_FLAMETHROWER : MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, board ? MOVE_THUNDERBOLT : MOVE_ICE_BEAM, target: board ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, 0);
        if (!board && turns == 2) {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_DANCER);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_PP2), 19);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_REVELATION_DANCE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Victoria timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_VICTORIA, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
