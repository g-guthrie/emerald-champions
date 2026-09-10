#include "ec_review_common.inc"
#line 60000 "work/ben_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Ben: Battery offense and reserve counterplay")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[3][5] = {{6, 3, 4, 3, 3}, {3, 2, 5, 3, 4}, {4, 3, 2, 2, 2}};
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_BEN, 2, phase);
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
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (!phase && !board && turns == 1) {
            ABILITY_POPUP(opponentRight, ABILITY_VOLT_ABSORB);
            HP_BAR(opponentRight);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (phase == 2 && board == 3 && turns == 2) {
            EXPECT_EQ(opponentLeft->hp, 1);
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Ben: natural Ground Grass reserves and Dry Skin")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 0 ? 3 : 2); t++)
        PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        const struct EmeraldChampionsBattleSet grovyle = {
            .moves = {MOVE_LEAF_BLADE, MOVE_QUICK_ATTACK, MOVE_DRAIN_PUNCH, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_OVERGROW, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        const struct EmeraldChampionsBattleSet starmie = {
            .moves = {MOVE_SCALD, MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_RECOVER},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_NATURAL_CURE, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        const struct EmeraldChampionsBattleSet simisear = {
            .moves = {MOVE_FLAMETHROWER, MOVE_GRASS_KNOT, MOVE_FOCUS_BLAST, MOVE_OVERHEAT},
            .item = ITEM_CHOICE_SPECS, .ability = ABILITY_GLUTTONY, .nature = NATURE_MODEST, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board == 1 ? SPECIES_STARMIE : board == 2 ? SPECIES_SIMISEAR : SPECIES_DUGTRIO,
            board == 1 ? &starmie : board == 2 ? &simisear : &dugtrio, 30);
        ReviewPlayer(board == 1 ? SPECIES_XATU : SPECIES_GROVYLE, board == 1 ? &sReviewXatu : &grovyle, 30);
        ReviewOpponent(TRAINER_BEN, 2, board == 1 ? 2 : 0);
        if (board == 1) {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            u16 hp = 40; SetMonData(&party[0], MON_DATA_HP, &hp);
        }
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, board == 1 ? MOVE_SCALD : board == 2 ? MOVE_FLAMETHROWER : MOVE_EARTHQUAKE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, board == 1 ? MOVE_PSYCHIC : MOVE_LEAF_BLADE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board == 1 && turns == 2) {
            ABILITY_POPUP(opponentLeft, ABILITY_DRY_SKIN);
            HP_BAR(opponentLeft, damage: -20);
        }
    } THEN { ReviewResult(10 + board, turns, board == 1 ? 2 : 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Ben timing: full bench and Battery damage")
{
    u32 phase = 0;
    for (u32 p = 0; p < 3; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_BEN, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        ReviewResult(99, 1, phase); ReviewTiming();
        if (!phase) {
            struct DamageContext ctx = {
                .battlerAtk = B_BATTLER_3, .battlerDef = B_BATTLER_0,
                .move = MOVE_THUNDERBOLT, .chosenMove = MOVE_THUNDERBOLT, .baseMove = MOVE_THUNDERBOLT,
                .moveType = TYPE_ELECTRIC, .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
            };
            for (u32 i = 0; i < gBattlersCount; i++) {
                ctx.abilities[i] = GetBattlerAbility(i);
                ctx.holdEffects[i] = GetBattlerHoldEffect(i);
            }
            ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
            s32 linked = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            ctx.abilities[B_BATTLER_1] = ABILITY_NONE;
            s32 unlinked = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            Test_MgbaPrintf("BEN_BATTERY_DAMAGE linked=%d unlinked=%d", linked, unlinked);
            EXPECT(linked > unlinked);
        }
    }
}
