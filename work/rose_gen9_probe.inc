#include "ec_review_common.inc"
#line 60000 "work/rose_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Rose: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? 4 : b == 0 ? 6 : b == 1 ? 4 : 2); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_ROSE_1, 2, phase);
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
        if (phase && board == 0) {
            HP_BAR(opponentRight, hp: 49);
            HP_BAR(opponentRight, hp: 76);
            ABILITY_POPUP(opponentRight, ABILITY_HARVEST);
        }
        if (!phase && board == 1 && turns >= 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, opponentRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, playerLeft);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("ROSE_SPEED actual=%d/%d tailwind=%d", GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)), GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND));
        if (!phase && board < 2 && turns == 1) {
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 178);
            EXPECT_EQ(opponentLeft->pp[1], 14);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rose: Fire status and Taunt")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 2 ? 4 : p ? 5 : b == 0 ? 7 : 6); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board == 2 ? SPECIES_MIENFOO : SPECIES_MAGMAR, board == 2 ? &sReviewMienfoo : &magmar, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_ROSE_1, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (board == 1 && t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
            else if (board == 1) MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(10 + board, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rose ability: Flower Veil supports an attacking Grass partner")
{
    GIVEN {
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_MAGMAR, &magmar, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_ROSE_1, 2, 1);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponentRight);
        ABILITY_POPUP(opponentLeft, ABILITY_FLOWER_VEIL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POLTERGEIST, opponentRight);
    } THEN {
        ReviewResult(30, 2, 1);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->pp[1], 4);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rose timing: healthy six-player bench")
{
    u32 phase = 0;
    PARAMETRIZE { phase = 0; }
    PARAMETRIZE { phase = 1; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_ROSE_1, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
