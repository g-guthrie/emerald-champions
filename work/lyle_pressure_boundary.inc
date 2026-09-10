// Temporary initialized owner-boundary query, not a fabricated Rick matchup.
#line 54000 "work/lyle_pressure_boundary.inc"
#include "battle_ai_switch.h"
#include "battle_controllers.h"

AI_DOUBLE_BATTLE_TEST("EC live review Lyle pressure boundary: own side and active reserve consistency")
{
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        LyleLeadPlayer(SPECIES_EEVEE, &eevee);
        LyleLeadPlayer(SPECIES_TIMBURR, &timburr);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_NYMBLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SEWADDLE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_ANORITH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
        for (u32 slot = 0; slot < 4; slot++)
        {
            // Explicit injury only; every stat, set and party record is native.
            u16 hp = GetMonData(&party[slot], MON_DATA_MAX_HP) / 2;
            SetMonData(&party[slot], MON_DATA_HP, &hp);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            // Opponent decisions remain unrestricted; this initializes the
            // ordinary native AI caches before any direct evaluator query.
        }
    } THEN {
        struct SwitchCandidateSnapshot *original = AI_SaveCandidateState();
        EXPECT(original != NULL);
        u16 savedTrainer = TRAINER_BATTLE_PARAM.opponentA;
        rng_value_t savedRng = gRngValue, savedRng2 = gRng2Value;
        s32 scores[2][2], idleScores[2][2];
        s32 expectedIdleDifference = 0;
        for (u32 slot = 0; slot < 4; slot++)
        {
            u32 hp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP);
            u32 maxHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP);
            bool32 active = FALSE;
            for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
                if (GetBattlerTrainer(actor) == B_TRAINER_OPPONENT_A
                 && gBattlerPartyIndexes[actor] == slot
                 && !(gAbsentBattlerFlags & (1u << actor))
                 && !gBattleStruct->battlerState[actor].notOnField)
                {
                    active = TRUE;
                    hp = gBattleMons[actor].hp;
                    maxHp = gBattleMons[actor].maxHP;
                    break;
                }
            // Exact additive oracle only for the all-idle diagnostic below:
            // the two specified values have the same full-HP total of 180.
            s32 difference = hp ? 75 + hp * 25 / max(1, maxHp) - hp * 100 / max(1, maxHp) : 0;
            expectedIdleDifference += difference;
            Test_MgbaPrintf("LYLE_PRESSURE_BOUNDARY_RECORD slot=%d active=%d hp=%d/%d valueDifference=%d",
                slot, active, hp, maxHp, difference);
        }
        for (u32 profile = 0; profile < 2; profile++)
        {
            AI_RestoreCandidateState(original);
            TRAINER_BATTLE_PARAM.opponentA = profile == 0 ? TRAINER_LYLE : TRAINER_RICK;
            EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), profile == 0 ? EC_BATTLE_PLAN_PRESSURE : 0);
            for (u32 side = 0; side < 2; side++)
            {
                enum BattlerId actor = side == 0 ? B_BATTLER_0 : B_BATTLER_1;
                scores[profile][side] = AI_EvaluateDoublesPosition(actor, 0);
                // This second, explicit idle boundary removes tactical choice
                // changes and proves both active and reserve contributions.
                idleScores[profile][side] = AI_EvaluateDoublesPosition(actor, (1u << gBattlersCount) - 1);
                EXPECT_EQ(memcmp(&gRngValue, &savedRng, sizeof(savedRng)), 0);
                EXPECT_EQ(memcmp(&gRng2Value, &savedRng2, sizeof(savedRng2)), 0);
            }
        }
        Test_MgbaPrintf("LYLE_PRESSURE_BOUNDARY player=%d/%d opponent=%d/%d idlePlayer=%d/%d idleOpponent=%d/%d expectedIdleDifference=%d",
            scores[0][0], scores[1][0], scores[0][1], scores[1][1],
            idleScores[0][0], idleScores[1][0], idleScores[0][1], idleScores[1][1], expectedIdleDifference);
        EXPECT_EQ(scores[0][0], scores[1][0]);
        EXPECT_GT(scores[0][1], scores[1][1]);
        EXPECT_EQ(idleScores[0][0], idleScores[1][0]);
        EXPECT_GT(expectedIdleDifference, 0);
        EXPECT_EQ(idleScores[0][1] - idleScores[1][1], expectedIdleDifference);
        // The ID substitution changes only the authored-policy context of
        // this query. It does not claim Rick owns Lyle's native party.
        AI_RestoreCandidateState(original);
        TRAINER_BATTLE_PARAM.opponentA = savedTrainer;
        EXPECT_EQ(memcmp(&gRngValue, &savedRng, sizeof(savedRng)), 0);
        EXPECT_EQ(memcmp(&gRng2Value, &savedRng2, sizeof(savedRng2)), 0);
        AI_FreeCandidateState(original);
    }
}
