#include "ec_review_common.inc"
#include "battle_main.h"
AI_DOUBLE_BATTLE_TEST("EC live review Ted: durable opponents and native Heavy Metal")
{
    u32 turns = 1, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 1 : 3); t++)
        PARAMETRIZE { phase = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet snorlax = {
            .moves = {MOVE_BODY_SLAM, MOVE_REST, MOVE_CURSE, MOVE_PROTECT}, .item = ITEM_LEFTOVERS,
            .ability = ABILITY_THICK_FAT, .nature = NATURE_CAREFUL, .evs = {252, 4, 0, 0, 252, 0},
        };
        const struct EmeraldChampionsBattleSet dusclops = {
            .moves = {MOVE_NIGHT_SHADE, MOVE_WILL_O_WISP, MOVE_PAIN_SPLIT, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
            .ability = ABILITY_PRESSURE, .nature = NATURE_CALM, .evs = {252, 0, 4, 0, 252, 0},
        };
        ReviewPlayer(SPECIES_SNORLAX, &snorlax, 40);
        ReviewPlayer(SPECIES_DUSCLOPS, &dusclops, 40);
        ReviewOpponent(TRAINER_TED, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, phase ? MOVE_PROTECT : MOVE_CURSE);
            MOVE(playerRight, phase ? MOVE_PROTECT : MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
        }
    } THEN {
        ReviewResult(72, turns, phase);
        Test_MgbaPrintf("TED_ITEMS players=%d/%d trainers=%d/%d", gBattleMons[0].item, gBattleMons[2].item, gBattleMons[1].item, gBattleMons[3].item);
        if (phase) {
            struct DamageContext ctx = {
                .battlerAtk = B_BATTLER_1, .battlerDef = B_BATTLER_0, .move = MOVE_HEAVY_SLAM,
                .chosenMove = MOVE_HEAVY_SLAM, .baseMove = MOVE_HEAVY_SLAM, .moveType = TYPE_STEEL,
                .weather = GetWeather(), .terrain = gFieldTimers.terrain, .aiCalc = TRUE,
            };
            for (u32 i = 0; i < gBattlersCount; i++) { ctx.abilities[i] = GetBattlerAbility(i); ctx.holdEffects[i] = GetBattlerHoldEffect(i); }
            ctx.typeEffectivenessModifier = CalcTypeEffectivenessMultiplier(&ctx);
            s32 heavy = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            ctx.abilities[1] = ABILITY_NONE;
            s32 ordinary = ApplyModifiersAfterDmgRoll(&ctx, CalculateMoveDamageVars(&ctx));
            Test_MgbaPrintf("TED_WEIGHT heavy=%d ordinary=%d", heavy, ordinary);
            EXPECT(heavy > ordinary);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Ted: Fire pressure")
{
    u32 phase = 0, turns = 1;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 5; t++) PARAMETRIZE { phase = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
            .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_MAGMAR, &magmar, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_TED, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(71, turns, phase); }
}
#line 60000 "work/ted_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Ted: Scarf pressure and heavy Steel reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{3, 3, 3, 2, 3}, {3, 4, 5, 3, 4}};
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 40);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 40);
        ReviewOpponent(TRAINER_TED, 3, phase);
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
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: board == 4 && phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Ted timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_TED, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
