// Temporary deterministic Trace parity. Include after Haley lead helpers.
// Actual Haley records are preserved; Ralts and Snubbull exchange party slots.
#line 65000 "work/haley_trace_probe.inc"
#include "battle_ai_switch.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_controllers.h"
#include "random.h"

DOUBLE_BATTLE_TEST("EC live review Haley Trace: deterministic candidate versus native entry")
{
    bool32 intimidate, native;
    PARAMETRIZE { intimidate = FALSE; native = FALSE; }
    PARAMETRIZE { intimidate = FALSE; native = TRUE; }
    PARAMETRIZE { intimidate = TRUE; native = FALSE; }
    PARAMETRIZE { intimidate = TRUE; native = TRUE; }
    GIVEN {
        for (u32 slot = 0; slot < 2; slot++)
            HaleyLeadPlayer(intimidate ? SPECIES_GROWLITHE : SPECIES_PACHIRISU,
                intimidate ? &sHaleyGrowlithe : &sHaleyPachirisu);
        AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_TRACE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        struct Pokemon saved = party[1];
        party[1] = party[3];
        party[3] = saved;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        if (native)
        {
            TURN {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_PROTECT);
                MOVE(opponentLeft, MOVE_PROTECT);
                SWITCH(opponentRight, 3);
            }
        }
    } THEN {
        struct SwitchCandidateSnapshot *state = NULL;
        struct BattlePokemon before = gBattleMons[B_BATTLER_3];
        rng_value_t rng = gRngValue, rng2 = gRng2Value;
        rng_value_t queryRng = rng, queryRng2 = rng2;
        u32 flags = gBattleTypeFlags;
        u32 controllers[MAX_BATTLERS_COUNT];
        for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
            controllers[battler] = gBattlerBattleController[battler];
        if (!native)
        {
            state = AI_SaveCandidateState();
            EXPECT(state != NULL);
            // Forced recorded battles do not initialize ordinary trainer AI.
            // Establish that context before testing candidate ability/cache.
            gBattleTypeFlags = flags & ~(BATTLE_TYPE_RECORDED | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_RECORDED_IS_MASTER | BATTLE_TYPE_LINK);
            gBattlerBattleController[B_BATTLER_1] = BATTLE_CONTROLLER_OPPONENT;
            gBattlerBattleController[B_BATTLER_3] = BATTLE_CONTROLLER_OPPONENT;
            BattleAI_SetupFlags();
            Ai_InitPartyStruct();
            SetAiLogicDataForTurn(gAiLogicData);
            EXPECT(gAiThinkingStruct->aiFlags[B_BATTLER_3] & AI_FLAG_OMNISCIENT);
            EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_0], intimidate ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_2], intimidate ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            queryRng = gRngValue;
            queryRng2 = gRng2Value;
            AI_LoadSwitchCandidate(B_BATTLER_3, 3, FALSE);
        }
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_RALTS);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 3);
        Test_MgbaPrintf("HALEY_TRACE intimidate=%d native=%d ability=%d cached=%d overwritten=%d activated=%d HP=%d/%d Speed=%d foeAtk=%d/%d rngChanged=%d",
            intimidate, native, GetBattlerAbility(B_BATTLER_3), gAiLogicData->abilities[B_BATTLER_3],
            gBattleMons[B_BATTLER_3].volatiles.overwrittenAbility,
            gBattleMons[B_BATTLER_3].volatiles.traceActivated,
            gBattleMons[B_BATTLER_3].hp, gBattleMons[B_BATTLER_3].maxHP,
            gBattleMons[B_BATTLER_3].speed,
            gBattleMons[B_BATTLER_0].statStages[STAT_ATK],
            gBattleMons[B_BATTLER_2].statStages[STAT_ATK],
            memcmp(&gRngValue, &queryRng, sizeof(queryRng)) != 0 || memcmp(&gRng2Value, &queryRng2, sizeof(queryRng2)) != 0);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), intimidate ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
        EXPECT_EQ((u32)gBattleMons[B_BATTLER_3].volatiles.overwrittenAbility, intimidate ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
        EXPECT((u32)gBattleMons[B_BATTLER_3].volatiles.traceActivated);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].statStages[STAT_ATK], intimidate ? 4 : 5);
        EXPECT_EQ(gBattleMons[B_BATTLER_2].statStages[STAT_ATK], intimidate ? 4 : 5);
        if (!native)
        {
            EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_3], intimidate ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            // Loading mutates the trial board and may consume internal RNG.
            // The caller's snapshot/restore owns preservation of live RNG.
            AI_RestoreCandidateState(state);
            AI_FreeCandidateState(state);
            gBattleTypeFlags = flags;
            for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
                gBattlerBattleController[battler] = controllers[battler];
            EXPECT_EQ(memcmp(&before, &gBattleMons[B_BATTLER_3], sizeof(before)), 0);
            EXPECT_EQ(memcmp(&gRngValue, &rng, sizeof(rng)), 0);
            EXPECT_EQ(memcmp(&gRng2Value, &rng2, sizeof(rng2)), 0);
        }
    }
}
