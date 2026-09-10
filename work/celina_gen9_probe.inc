#include "ec_review_common.inc"
#line 60000 "work/celina_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Celina: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? 4 : b == 1 ? 3 : 2); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_CELINA, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (!phase && board == 2) {
            ABILITY_POPUP(opponentRight, ABILITY_QUEENLY_MAJESTY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GIGA_DRAIN, opponentLeft);
        }
        if (phase && board == 1 && turns >= 3) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAT_WAVE, opponentRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLORAL_HEALING, opponentLeft);
            HP_BAR(opponentRight, hp: 87);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, playerLeft);
        }
    } THEN { ReviewResult(board, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Celina: Fire and Taunt pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (p == b ? 4 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board ? SPECIES_MIENFOO : SPECIES_MAGMAR, board ? &sReviewMienfoo : &magmar, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_CELINA, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board) MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, phase);
        if (!board && turns == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_NINETALES);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT(opponentRight->volatiles.flashFireBoosted);
            if (phase) EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            else EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Celina timing: healthy six-player bench")
{
    u32 phase = 0;
    PARAMETRIZE { phase = 0; }
    PARAMETRIZE { phase = 1; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_CELINA, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
