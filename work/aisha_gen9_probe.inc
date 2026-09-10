#include "ec_review_common.inc"
#line 59000 "work/aisha_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Aisha: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= 3; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_AISHA, 2, phase);
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
        if (phase && board == 0 && turns == 1) {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_INTIMIDATE);
            EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        }
        if (phase && board == 2 && turns == 3) {
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_FEINT);
            EXPECT_EQ(opponentRight->pp[2], 9);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Aisha reserves: Ghost burn and critical Grass coverage")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 3 : 5); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet quagsire = {
            .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_MODEST, .ability = ABILITY_UNAWARE, .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(board ? SPECIES_QUAGSIRE : SPECIES_SABLEYE,
            board ? &quagsire : &gEmeraldChampionsDefaultBattleSets[SPECIES_SABLEYE], 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_AISHA, 2, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) MOVE(playerLeft, MOVE_ICE_BEAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, 1);
        if (!board && turns == 2) {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_SCRAPPY);
            EXPECT(opponentLeft->status1 & STATUS1_BURN);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_CLOSE_COMBAT);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_HELPING_HAND);
            EXPECT_EQ(playerLeft->hp, 0);
        }
        if (board && turns == 1) {
            struct DamageContext ctx = {.battlerAtk = B_BATTLER_1, .battlerDef = B_BATTLER_0, .move = MOVE_LEAF_BLADE};
            ctx.holdEffects[B_BATTLER_1] = GetBattlerHoldEffect(B_BATTLER_1);
            ctx.abilities[B_BATTLER_1] = ABILITY_SCRAPPY;
            ctx.abilities[B_BATTLER_0] = ABILITY_UNAWARE;
            EXPECT_EQ(GetCriticalHitOdds(CalcCritChanceStage(&ctx)), 1);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_LEAF_BLADE);
            EXPECT_EQ(playerLeft->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Aisha disruption: flinch-proof Flying and spread Fairy")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_CROBAT, &gEmeraldChampionsDefaultBattleSets[SPECIES_CROBAT], 30);
        ReviewPlayer(SPECIES_SYLVEON, &gEmeraldChampionsDefaultBattleSets[SPECIES_SYLVEON], 30);
        ReviewOpponent(TRAINER_AISHA, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_BRAVE_BIRD, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_HYPER_VOICE, hit: TRUE, criticalHit: FALSE);
        }
    } THEN { ReviewResult(20, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Aisha timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_AISHA, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
