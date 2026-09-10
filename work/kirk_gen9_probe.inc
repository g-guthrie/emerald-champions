#include "ec_review_common.inc"
#line 60000 "work/kirk_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Kirk: pressure and all reserve pairs")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p == 1 && b == 0 ? 3 : p == 2 ? b == 1 ? 4 : 3 : b == 1 || p == 1 ? 4 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_KIRK, 2, phase);
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
        if (!phase && !board) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentRight);
            ABILITY_POPUP(opponentRight, ABILITY_CHEEK_POUCH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RISING_VOLTAGE, opponentLeft);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && !board && turns == 1) {
            EXPECT_EQ(opponentRight->hp, 84);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(opponentRight->pp[2], 19);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 1);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kirk: Ground counterplay and terrain sleep protection")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (!p ? !b ? 4 : 2 : p == 1 && !b ? 2 : p == 2 ? 3 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        const struct EmeraldChampionsBattleSet vivillon = {
            .moves = {MOVE_SLEEP_POWDER, MOVE_BUG_BUZZ, MOVE_HURRICANE, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_SHIELD_DUST, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board ? SPECIES_VIVILLON : SPECIES_DUGTRIO, board ? &vivillon : &dugtrio, 30);
        ReviewPlayer(board ? SPECIES_GURDURR : SPECIES_XATU, board ? &sReviewGurdurr : &sReviewXatu, 30);
        ReviewOpponent(TRAINER_KIRK, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board) {
                if (t % 2 == 0) MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_SLEEP_POWDER, target: opponentRight, hit: TRUE);
                else MOVE(playerLeft, MOVE_BUG_BUZZ, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (board && !phase) MESSAGE("The opposing Dedenne surrounds itself with electrified terrain!");
    } THEN {
        ReviewResult(10 + board, turns, phase);
        if (board && !phase && turns == 1) {
            EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
            EXPECT_EQ(opponentRight->status1 & STATUS1_SLEEP, 0);
            EXPECT_EQ(opponentRight->pp[0], 14);
            EXPECT_EQ(playerLeft->pp[0], 14);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kirk: Motor Drive before slower disruption")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet manectric = {
            .moves = {MOVE_THUNDERBOLT, MOVE_FLAMETHROWER, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_STATIC, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 30);
        ReviewPlayer(SPECIES_MANECTRIC, &manectric, 30);
        ReviewOpponent(TRAINER_KIRK, 2, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ABILITY_POPUP(opponentRight, ABILITY_MOTOR_DRIVE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentRight);
    } THEN {
        ReviewResult(20, turns, 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + turns);
        EXPECT_EQ(GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)), turns == 1 ? 154 : 206);
        if (turns == 1) EXPECT_EQ(opponentLeft->hp, 83);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kirk: native terrain damage boundaries")
{
    GIVEN {
        ReviewPlayer(SPECIES_GURDURR, &sReviewGurdurr, 30);
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 30);
        ReviewOpponent(TRAINER_KIRK, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        struct DamageContext ctx = {
            .battlerAtk = B_BATTLER_1, .battlerDef = B_BATTLER_0,
            .move = MOVE_RISING_VOLTAGE, .chosenMove = MOVE_RISING_VOLTAGE, .baseMove = MOVE_RISING_VOLTAGE,
            .moveType = TYPE_ELECTRIC, .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
        };
        for (u32 i = 0; i < gBattlersCount; i++) {
            ctx.abilities[i] = GetBattlerAbility(i);
            ctx.holdEffects[i] = GetBattlerHoldEffect(i);
        }
        EXPECT_EQ((u32)ctx.terrain, B_TERRAIN_ELECTRIC);
        ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
        s32 grounded = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
        ctx.abilities[B_BATTLER_0] = ABILITY_LEVITATE;
        s32 airborne = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
        ctx.abilities[B_BATTLER_0] = GetBattlerAbility(B_BATTLER_0);
        ctx.terrain = B_TERRAIN_NONE;
        s32 absent = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
        Test_MgbaPrintf("KIRK_VOLTAGE_DAMAGE terrainGrounded=%d terrainAirborne=%d noTerrain=%d", grounded, airborne, absent);
        EXPECT(grounded > airborne);
        EXPECT(grounded > absent);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kirk timing: healthy six-player bench")
{
    u32 phase = 0;
    PARAMETRIZE { phase = 0; }
    PARAMETRIZE { phase = 1; }
    PARAMETRIZE { phase = 2; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_KIRK, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
