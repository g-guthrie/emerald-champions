// Candidate shared regression; independent of campaign trainers and helpers.
#line 59000 "work/haley_trace_shared_probe.inc"
#include "battle_ai_switch.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "random.h"

static u32 TraceCandidateStateHash(void)
{
    const void *blocks[] = {gBattleMons, gParties, gBattleStruct, gAiLogicData, &gRngValue, &gRng2Value};
    const u32 sizes[] = {sizeof(gBattleMons), sizeof(gParties), sizeof(*gBattleStruct), sizeof(*gAiLogicData), sizeof(gRngValue), sizeof(gRng2Value)};
    u32 hash = 2166136261u;
    for (u32 block = 0; block < ARRAY_COUNT(blocks); block++)
        for (u32 index = 0; index < sizes[block]; index++)
            hash = (hash ^ ((const u8 *)blocks[block])[index]) * 16777619u;
    return hash;
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: candidate Trace copies deterministic abilities and restores state")
{
    u32 control;
    PARAMETRIZE { control = 0; } // Identical Volt Absorb.
    PARAMETRIZE { control = 1; } // Identical Intimidate, including its entry effect.
    PARAMETRIZE { control = 2; } // Ability Shield prevents copying.
    PARAMETRIZE { control = 3; } // Different legal outcomes remain unresolved.
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(control == 1 ? SPECIES_GROWLITHE : SPECIES_PACHIRISU) {
            Level(14); Ability(control == 1 ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            Moves(MOVE_PROTECT);
        }
        PLAYER(control == 1 || control == 3 ? SPECIES_GROWLITHE : SPECIES_PACHIRISU) {
            Level(14); Ability(control == 1 || control == 3 ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_EEVEE) {
            Level(12); Ability(ABILITY_ADAPTABILITY); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SNUBBULL) {
            Level(12); Ability(ABILITY_INTIMIDATE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_RALTS) {
            Level(12); Ability(ABILITY_TRACE);
            Item(control == 2 ? ITEM_ABILITY_SHIELD : ITEM_FOCUS_SASH);
            Moves(MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT(gAiThinkingStruct->aiFlags[B_BATTLER_3] & AI_FLAG_OMNISCIENT);
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_SNUBBULL);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMons[B_BATTLER_2].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        u32 before = TraceCandidateStateHash();
        struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
        EXPECT(state != NULL);
        AI_LoadSwitchCandidate(B_BATTLER_3, 2, FALSE);
        enum Ability expected = control == 0 ? ABILITY_VOLT_ABSORB : control == 1 ? ABILITY_INTIMIDATE : ABILITY_TRACE;
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_RALTS);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), expected);
        EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_3], expected);
        EXPECT_EQ((u32)gBattleMons[B_BATTLER_3].volatiles.overwrittenAbility, control < 2 ? expected : ABILITY_NONE);
        EXPECT_EQ((u32)gBattleMons[B_BATTLER_3].volatiles.traceActivated, control != 3);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].statStages[STAT_ATK], DEFAULT_STAT_STAGE - (control == 1 ? 2 : 1));
        EXPECT_EQ(gBattleMons[B_BATTLER_2].statStages[STAT_ATK], DEFAULT_STAT_STAGE - (control == 1 ? 2 : 1));
        // Internal trial RNG use is permitted. No complete pair evaluation is
        // nested inside this snapshot; only the loader and its entry effects.
        AI_RestoreCandidateState(state);
        AI_FreeCandidateState(state);
        EXPECT_EQ(TraceCandidateStateHash(), before);
    }
}
