#include "ec_review_common.inc"
#line 60000 "work/dalton_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Dalton: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p && b != 1 ? 3 : !p && b == 1 ? 5 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_DALTON_1, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, phase ? MOVE_ICE_PUNCH : MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
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
        if (!phase && board == 0 && turns >= 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, opponentRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, playerLeft);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("DALTON_SPEED actual=%d/%d tailwind=%d", GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)), GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND));
        if (!phase && board == 0 && turns == 1) {
            EXPECT_EQ(opponentLeft->hp, 1);
            EXPECT_EQ(opponentLeft->pp[1], 14);
            EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), 80);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dalton: sound Fairy and Taunt pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 2 || (p && b == 1) ? 3 : !p && b == 1 ? 5 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet houndoom = {
            .moves = {MOVE_SNARL, MOVE_DARK_PULSE, MOVE_FLAMETHROWER, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_FLASH_FIRE, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        const struct EmeraldChampionsBattleSet gardevoir = {
            .moves = {MOVE_DAZZLING_GLEAM, MOVE_PSYCHIC, MOVE_MOONBLAST, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_TELEPATHY, .nature = NATURE_MODEST, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_HOUNDOOM : board == 1 ? SPECIES_GARDEVOIR : SPECIES_MIENFOO,
            board == 0 ? &houndoom : board == 1 ? &gardevoir : &sReviewMienfoo, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_DALTON_1, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
            else if (board == 1) MOVE(playerLeft, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE);
            else if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (!phase && board == 0) ABILITY_POPUP(opponentRight, ABILITY_SOUNDPROOF);
        if (phase && board == 1 && turns == 3) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BUZZ, opponentRight);
        }
    } THEN {
        ReviewResult(10 + board, turns, phase);
        if (!phase && board == 0 && turns == 1) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentRight->pp[0], 9);
        }
        if (phase && board == 2 && turns == 1) {
            EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentLeft->pp[1], 9);
        }
        if (phase && board == 1 && turns == 3) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT_EQ(opponentLeft->pp[2], 19);
            EXPECT_EQ(opponentRight->hp, 33);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dalton timing: healthy six-player bench")
{
    u32 phase = 0;
    PARAMETRIZE { phase = 0; }
    PARAMETRIZE { phase = 1; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_DALTON_1, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
