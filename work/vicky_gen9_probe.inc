#include "ec_review_common.inc"
#line 65000 "work/vicky_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Vicky: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? b == 1 ? 2 : 3 : b == 0 ? 2 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_VICKY, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { ReviewResult(board, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vicky: Taunt spread and confusion denial")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (b == 4 ? 2 : b == 3 ? 3 : b ? 4 : 3); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sylveon = {
            .moves = {MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_PIXILATE, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet mudsdale = {
            .moves = {MOVE_BODY_PRESS, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_OWN_TEMPO, .nature = NATURE_IMPISH, .evs = {252, 0, 252, 0, 4, 0},
        };
        const struct EmeraldChampionsBattleSet gardevoir = {
            .moves = {MOVE_MOONBLAST, MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_SYNCHRONIZE, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(!board ? SPECIES_MIENFOO : board == 1 ? SPECIES_SYLVEON : board == 2 ? SPECIES_MUDSDALE : board == 3 ? SPECIES_GARDEVOIR : SPECIES_XATU,
            !board ? &sReviewMienfoo : board == 1 ? &sylveon : board == 2 ? &mudsdale : board == 3 ? &gardevoir : &sReviewXatu, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_VICKY, 2, board >= 3);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board && t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft, hit: TRUE);
            else if (board == 1) MOVE(playerLeft, MOVE_HYPER_VOICE, hit: TRUE, criticalHit: FALSE);
            else if (board == 3) MOVE(playerLeft, MOVE_MOONBLAST, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            else if (board == 4) MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else MOVE(playerLeft, board ? MOVE_BODY_PRESS : MOVE_DRAIN_PUNCH,
                target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: board == 4 ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, board >= 3);
        if (board == 2 && turns == 3) {
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_DYNAMIC_PUNCH);
            EXPECT(playerLeft->volatiles.confusionTimer == 0);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_0), ABILITY_OWN_TEMPO);
        }
        if (board == 3 && turns == 1) {
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_HELPING_HAND);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_THUNDER_PUNCH);
        }
        if (board == 4 && turns == 1) {
            EXPECT_EQ(opponentLeft->species, SPECIES_PAWMOT);
            EXPECT_EQ(opponentLeft->hp, 1);
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vicky timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_VICKY, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
