#include "ec_review_common.inc"
#line 60000 "work/vivian_gen9_probe.inc"

static void ReviewVivian(u32 phase)
{
    ReviewOpponent(TRAINER_VIVIAN, 2, phase < 3 ? phase : 0);
    if (phase == 3) {
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        party[0] = party[3];
        party[1] = party[5];
        for (u32 i = 2; i < 6; i++) {
            u16 zero = 0; SetMonData(&party[i], MON_DATA_HP, &zero);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vivian: pressure and reserve partnerships")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[4][3] = {{5, 5, 3}, {4, 3, 4}, {4, 4, 4}, {3, 4, 4}};
    for (u32 p = 0; p < 4; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewVivian(phase);
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
        if (phase == 3 && board == 0) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DISCHARGE, opponentRight);
            if (turns >= 2) {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_DISCHARGE, opponentRight);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentLeft);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (phase == 3 && board == 0 && turns <= 2) {
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + turns);
            EXPECT_EQ(opponentRight->pp[3], 15 - turns);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vivian: Ground and Taunt counterplay")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[4][2] = {{3, 5}, {4, 3}, {4, 4}, {3, 3}};
    for (u32 p = 0; p < 4; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board ? SPECIES_MIENFOO : SPECIES_DUGTRIO, board ? &sReviewMienfoo : &dugtrio, 30);
        ReviewPlayer(board ? SPECIES_PACHIRISU : SPECIES_XATU, board ? &sReviewPachirisu : &sReviewXatu, 30);
        ReviewVivian(phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight, hit: TRUE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(10 + board, turns, phase);
        if (!phase && !board && turns == 1) {
            EXPECT_EQ(opponentRight->hp, 6);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 1);
        }
        if (phase == 2 && !board && turns == 1) {
            EXPECT_EQ(opponentLeft->hp, 9);
            EXPECT_EQ(opponentRight->hp, 88);
            EXPECT_EQ(opponentRight->item, ITEM_AIR_BALLOON);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vivian timing: six-player bench and native Plus Minus")
{
    u32 phase = 0;
    for (u32 p = 0; p < 4; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewVivian(phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        ReviewResult(99, 1, phase); ReviewTiming();
        if (!phase) {
            struct DamageContext ctx = {
                .battlerAtk = B_BATTLER_1, .battlerDef = B_BATTLER_0,
                .move = MOVE_THUNDERBOLT, .chosenMove = MOVE_THUNDERBOLT, .baseMove = MOVE_THUNDERBOLT,
                .moveType = TYPE_ELECTRIC, .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
            };
            for (u32 i = 0; i < gBattlersCount; i++) {
                ctx.abilities[i] = GetBattlerAbility(i);
                ctx.holdEffects[i] = GetBattlerHoldEffect(i);
            }
            ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
            s32 linked = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            ctx.abilities[B_BATTLER_3] = ABILITY_NONE;
            s32 unlinked = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            Test_MgbaPrintf("VIVIAN_PLUS_DAMAGE linked=%d unlinked=%d", linked, unlinked);
            EXPECT(linked > unlinked);
        }
    }
}
