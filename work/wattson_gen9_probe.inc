#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/wattson_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Wattson: terrain offense and Mega reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[3][5] = {{2, 2, 2, 2, 2}, {3, 3, 3, 2, 2}, {3, 2, 2, 2, 2}};
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
        ReviewOpponent(TRAINER_WATTSON_1, 2, phase);
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
        if (!phase && board == 0) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DISCHARGE, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WILD_CHARGE, opponentRight);
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && board == 0) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + turns);
            EXPECT_EQ(opponentLeft->pp[0], 15 - turns);
            EXPECT_EQ(opponentRight->pp[0], 15 - turns);
        }
        Test_MgbaPrintf("WATTSON_FIELD terrain=%d timer=%d charge=%d/%d speed=%d/%d nativeSpeed=%d/%d",
            gFieldTimers.terrain, gFieldTimers.terrainTimer, gBattleMons[1].volatiles.chargeTimer, gBattleMons[3].volatiles.chargeTimer,
            GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), gBattleMons[1].speed, gBattleMons[3].speed);
        if (opponentRight->species == SPECIES_MANECTRIC_MEGA)
            Test_MgbaPrintf("WATTSON_MEGA HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d",
                opponentRight->maxHP, opponentRight->attack, opponentRight->defense, opponentRight->spAttack,
                opponentRight->spDefense, opponentRight->speed, GetBattlerAbility(3));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Wattson: Wide Guard and base-form partner safety")
{
    u32 board = 0, turns = 1;
    for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 1 ? 2 : 3); t++)
        PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet guard = sReviewGurdurr;
        guard.moves[2] = MOVE_WIDE_GUARD;
        ReviewPlayer(board == 2 ? SPECIES_XATU : SPECIES_EEVEE, board == 2 ? &sReviewXatu : &sReviewEevee, 30);
        ReviewPlayer(board == 2 ? SPECIES_PACHIRISU : SPECIES_GURDURR,
            board == 2 ? &sReviewPachirisu : board == 0 ? &guard : &sReviewGurdurr, 30);
        ReviewOpponent(TRAINER_WATTSON_1, 2, 0);
        if (board) {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            struct Pokemon mon = party[1]; party[1] = party[5]; party[5] = mon;
            for (u32 i = 2; i < PARTY_SIZE; i++) { u16 hp = 0; SetMonData(&party[i], MON_DATA_HP, &hp); }
        }
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, board == 2 ? MOVE_PSYCHIC : MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (!board) MOVE(playerRight, MOVE_WIDE_GUARD);
            else MOVE(playerRight, board == 2 ? MOVE_THUNDERBOLT : MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(20 + board, turns, board ? 3 : 0);
        Test_MgbaPrintf("WATTSON_PARTNER ability=%d form=%d Speed=%d item=%d", GetBattlerAbility(3), opponentRight->species,
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), opponentRight->item);
        if (board == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_MANECTRIC);
            EXPECT_EQ(GetBattlerAbility(3), ABILITY_LIGHTNING_ROD);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + turns);
            EXPECT_EQ(opponentRight->pp[0], 15 - turns);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Wattson: two Ground foes deny Electric spread")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        const struct EmeraldChampionsBattleSet gastrodon = {
            .moves = {MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_SCALD, MOVE_RECOVER},
            .item = ITEM_RINDO_BERRY, .ability = ABILITY_STORM_DRAIN, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(SPECIES_DUGTRIO, &dugtrio, 30);
        ReviewPlayer(SPECIES_GASTRODON, &gastrodon, 30);
        ReviewOpponent(TRAINER_WATTSON_1, 2, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_EARTH_POWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(30, turns, 0); }
}
AI_DOUBLE_BATTLE_TEST("EC live review Wattson timing: six-player bench with Mega reserve")
{
    u32 phase = 0;
    for (u32 p = 0; p < 3; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_WATTSON_1, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
