// Temporary initialized-cache and forced-native controls; after LyleLeadPlayer.
#line 64000 "work/lyle_screen_cache_probe.inc"
#include "battle_ai_switch.h"

static const struct EmeraldChampionsBattleSet sLyleScreenGuard = {
    .moves = {MOVE_ICE_BEAM, MOVE_WATER_PULSE, MOVE_REST, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .statPoints = {32, 0, 18, 16, 0, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Lyle screens: initialized native cache exclusions and restoration")
{
    u32 kind;
    PARAMETRIZE { kind = 0; } // Native pre-damage Brick Break.
    PARAMETRIZE { kind = 1; } // Fixed-percent Super Fang.
    PARAMETRIZE { kind = 2; } // Guaranteed critical Storm Throw.
    PARAMETRIZE { kind = 3; } // Native Infiltrator bypass.
    PARAMETRIZE { kind = 4; } // Same Zubat move, ordinary screen reduction.
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_PACHIRISU, SPECIES_THROH, SPECIES_ZUBAT, SPECIES_ZUBAT};
        const enum Move moves[] = {MOVE_BRICK_BREAK, MOVE_SUPER_FANG, MOVE_STORM_THROW, MOVE_AERIAL_ACE, MOVE_AERIAL_ACE};
        const enum Ability abilities[] = {ABILITY_INNER_FOCUS, ABILITY_VOLT_ABSORB, ABILITY_MOLD_BREAKER, ABILITY_INFILTRATOR, ABILITY_INNER_FOCUS};
        struct EmeraldChampionsBattleSet attacker = {
            .moves = {moves[kind], MOVE_PROTECT, MOVE_REST, MOVE_SUBSTITUTE},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = abilities[kind],
            .statPoints = {32, 16, 18, 0, 0, 0},
        };
        // These are species-native abilities and moves. Storm Throw is in
        // Throh's level-up pool, not naturally learned by ordinary level 14.
        // Zubat/Throh are mechanic controls, not pre-Lyle catch guarantees.
        LyleLeadPlayer(species[kind], &attacker);
        LyleLeadPlayer(SPECIES_LOTAD, &sLyleScreenGuard);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        struct SwitchCandidateSnapshot *original = AI_SaveCandidateState();
        EXPECT(original != NULL);
        AI_LoadSwitchCandidate(B_BATTLER_3, 3, FALSE);
        SetAiLogicDataForTurn(gAiLogicData);
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_DOTTLER);
        enum Move move = gBattleMons[B_BATTLER_0].moves[0];
        EXPECT_EQ(AI_CanScreenReduceDamage(B_BATTLER_0, B_BATTLER_3, move), kind == 4);
        struct SimulatedDamage damage[2];
        for (u32 screened = 0; screened < 2; screened++)
        {
            // Explicit screen-state query, not a claim the harmless turn
            // established all three screens. The native damage consumer is
            // already initialized, and every query must restore these flags.
            gSideStatuses[B_SIDE_OPPONENT] &= ~SIDE_STATUS_SCREEN_ANY;
            if (screened)
                gSideStatuses[B_SIDE_OPPONENT] |= SIDE_STATUS_SCREEN_ANY;
            u32 before[2] = {gSideStatuses[B_SIDE_PLAYER], gSideStatuses[B_SIDE_OPPONENT]};
            rng_value_t rng = gRngValue, rng2 = gRng2Value;
            CalcBattlerAiMovesData(gAiLogicData, B_BATTLER_0, B_BATTLER_3, AI_GetWeather(), gFieldTimers.terrain);
            damage[screened] = gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_3][0];
            EXPECT_EQ(gSideStatuses[B_SIDE_PLAYER], before[0]);
            EXPECT_EQ(gSideStatuses[B_SIDE_OPPONENT], before[1]);
            EXPECT_EQ(memcmp(&gRngValue, &rng, sizeof(rng)), 0);
            EXPECT_EQ(memcmp(&gRng2Value, &rng2, sizeof(rng2)), 0);
            EXPECT_GT(damage[screened].minimum, 0);
            Test_MgbaPrintf("LYLE_SCREEN_CACHE kind=%d screened=%d move=%S ability=%d range=%d/%d/%d flags=%d/%d",
                kind, screened, GetMoveName(move), gAiLogicData->abilities[B_BATTLER_0],
                damage[screened].minimum, damage[screened].median, damage[screened].maximum,
                gSideStatuses[B_SIDE_PLAYER], gSideStatuses[B_SIDE_OPPONENT]);
        }
        if (kind == 4)
            EXPECT_LT(damage[1].median, damage[0].median);
        else
        {
            EXPECT_EQ(damage[1].minimum, damage[0].minimum);
            EXPECT_EQ(damage[1].median, damage[0].median);
            EXPECT_EQ(damage[1].maximum, damage[0].maximum);
        }
        AI_RestoreCandidateState(original);
        AI_FreeCandidateState(original);
    }
}

DOUBLE_BATTLE_TEST("EC live review Lyle screens: native Brick Break own hit removes prior Reflect", s16 damage)
{
    bool32 reflect;
    PARAMETRIZE { reflect = FALSE; }
    PARAMETRIZE { reflect = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet mienfoo = {
            .moves = {MOVE_BRICK_BREAK, MOVE_PROTECT, MOVE_REST, MOVE_SUBSTITUTE},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_INNER_FOCUS,
            .statPoints = {32, 16, 18, 0, 0, 0},
        };
        LyleLeadPlayer(SPECIES_MIENFOO, &mienfoo);
        LyleLeadPlayer(SPECIES_LOTAD, &sLyleScreenGuard);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_ANORITH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (reflect)
                MOVE(opponentRight, MOVE_REFLECT);
            else
                MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, reflect ? MOVE_REFLECT : MOVE_HELPING_HAND, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BRICK_BREAK, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_GT(results[i].damage, 0);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SCREEN_ANY, 0);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_BRICK_BREAK);
        Test_MgbaPrintf("LYLE_SCREEN_BREAK_NATIVE reflect=%d damage=%d DottlerHP=%d/%d screenFlags=%d",
            reflect, results[i].damage, opponentRight->hp, opponentRight->maxHP,
            gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SCREEN_ANY);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}
