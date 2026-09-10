#include "ec_review_common.inc"
#line 58000 "work/derek_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Derek: physical special and flinch pressure")
{
    u32 board = 0, turns = 0; bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? (b == 1 ? 5 : 3) : (b == 0 ? 5 : b == 1 ? 3 : 4)); t++)
        PARAMETRIZE { reserves = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_DEREK, 2, reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, reserves);
        if (reserves && board == 2 && turns == 3) {
            EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentLeft->pp[2], 14);
            EXPECT_EQ(opponentLeft->pp[1], 9);
        }
        if (reserves && board == 1 && turns >= 3) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->pp[0], 19);
            if (turns >= 4) EXPECT(opponentRight->pp[1] < 9);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Derek powder immunity and Fire counterplay")
{
    bool32 reserves = FALSE; u32 turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 5 : 4); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet amoonguss = {
            .moves = {MOVE_SPORE, MOVE_SLUDGE_BOMB, MOVE_GIGA_DRAIN, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_MODEST, .ability = ABILITY_REGENERATOR, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet litleo = {
            .moves = {MOVE_FLAMETHROWER, MOVE_HYPER_VOICE, MOVE_DARK_PULSE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_UNNERVE, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_AMOONGUSS, &amoonguss, 30); ReviewPlayer(SPECIES_LITLEO, &litleo, 30);
        ReviewOpponent(TRAINER_DEREK, 2, reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) {
                if (reserves) MOVE(playerLeft, MOVE_SPORE, target: opponentLeft);
                else MOVE(playerLeft, MOVE_SPORE, target: opponentRight);
            }
            else MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10, turns, reserves);
        if (turns == 1 && !reserves) {
            // Ground/Bug Sandy is not naturally powder-immune, and Assault
            // Vest leaves it no Protect: this exercises Overcoat itself.
            EXPECT_EQ(opponentRight->species, SPECIES_WORMADAM_SANDY);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_OVERCOAT);
            EXPECT(!(opponentRight->status1 & STATUS1_SLEEP));
            EXPECT_EQ(playerLeft->pp[0], 14);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Derek special-pressure setup and payoff")
{
    bool32 reserves = FALSE; u32 turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 5 : 3); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewPlayer(SPECIES_LOMBRE, &sReviewLombre, 30);
        ReviewOpponent(TRAINER_DEREK, 2, reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(20, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Derek timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_DEREK, 2, FALSE);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, FALSE); ReviewTiming(); }
}
