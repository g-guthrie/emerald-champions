#include "ec_review_common.inc"
#line 64000 "work/vivi_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Vivi: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 1 ? p ? 2 : 3 : p && b == 0 ? 3 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_VIVI, 2, phase);
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
        if (!phase && turns == 2) {
            EXPECT_EQ(opponentLeft->species, SPECIES_FOMANTIS);
            EXPECT(opponentLeft->hp > 0);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vivi: Unaware and Grass disruption")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet miltank = {
            .moves = {MOVE_WORK_UP, MOVE_BODY_SLAM, MOVE_MILK_DRINK, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .ability = ABILITY_SCRAPPY, .nature = NATURE_CAREFUL, .evs = {252, 4, 0, 0, 252, 0},
        };
        const struct EmeraldChampionsBattleSet grovyle = {
            .moves = {MOVE_LEAF_BLADE, MOVE_ACROBATICS, MOVE_QUICK_ATTACK, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_OVERGROW, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 2 ? SPECIES_XATU : board ? SPECIES_GROVYLE : SPECIES_MILTANK,
            board == 2 ? &sReviewXatu : board ? &grovyle : &miltank, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_VIVI, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board && t == 0) MOVE(playerLeft, MOVE_WORK_UP);
            else MOVE(playerLeft, board == 2 ? MOVE_AIR_SLASH : board ? MOVE_LEAF_BLADE : MOVE_BODY_SLAM,
                target: board == 2 ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(10 + board, turns, 0);
        if (!board && turns == 1) {
            struct DamageContext ctx = {
                .battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_3,
                .move = MOVE_BODY_SLAM, .chosenMove = MOVE_BODY_SLAM, .baseMove = MOVE_BODY_SLAM,
                .moveType = TYPE_NORMAL, .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
            };
            for (u32 i = 0; i < gBattlersCount; i++) {
                ctx.abilities[i] = GetBattlerAbility(i);
                ctx.holdEffects[i] = GetBattlerHoldEffect(i);
            }
            EXPECT_EQ(opponentRight->species, SPECIES_QUAGSIRE);
            EXPECT_EQ(ctx.abilities[B_BATTLER_3], ABILITY_UNAWARE);
            ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
            u32 saved = playerLeft->statStages[STAT_ATK];
            playerLeft->statStages[STAT_ATK] = DEFAULT_STAT_STAGE;
            s32 base = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            playerLeft->statStages[STAT_ATK] = MAX_STAT_STAGE;
            s32 boosted = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            ctx.abilities[B_BATTLER_3] = ABILITY_WATER_ABSORB;
            s32 disabled = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            Test_MgbaPrintf("VIVI_UNAWARE_DAMAGE neutral=%d plusSix=%d disabled=%d", base, boosted, disabled);
            EXPECT_EQ(base, boosted);
            EXPECT(disabled > boosted);
            playerLeft->statStages[STAT_ATK] = saved;
        }
        if (board == 1 && turns == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_QUAGSIRE);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT(opponentRight->hp > 0);
        }
        if (board == 1 && turns == 3) EXPECT_EQ(opponentRight->species, SPECIES_VIBRAVA);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Vivi timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_VIVI, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
