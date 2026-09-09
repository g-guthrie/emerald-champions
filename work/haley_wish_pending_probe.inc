// Temporary native Wish state followed by explicit initialized AI queries.
// Requires HaleyWishBoard from haley_wish_probe.inc. No authored-menu edits.
#line 57000 "work/haley_wish_pending_probe.inc"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"
#include "battle_ai_util.h"
#include "battle_controllers.h"
#include "random.h"

DOUBLE_BATTLE_TEST("EC live review Haley pending Wish: native first turn then isolated timer queries")
{
    u16 counter;
    PARAMETRIZE { counter = 1; }
    PARAMETRIZE { counter = 0; }
    PARAMETRIZE { counter = 2; }
    GIVEN { HaleyWishBoard(); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_WISH);
            MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_EEVEE);
        EXPECT_EQ(opponentRight->species, SPECIES_RALTS);
        EXPECT_EQ(gBattleStruct->wish[B_BATTLER_1].counter, 1);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_WISH);
        // Each timer gets a fresh native battle. An outer 8 KiB candidate
        // snapshot would change the evaluator's available heap and is omitted.
        u32 flags = gBattleTypeFlags;
        u16 savedCounter = gBattleStruct->wish[B_BATTLER_1].counter;
        rng_value_t rng = gRngValue, rng2 = gRng2Value;
        u32 controllers[MAX_BATTLERS_COUNT];
        for (u32 b = 0; b < MAX_BATTLERS_COUNT; b++)
            controllers[b] = gBattlerBattleController[b];
        {
            // Forced DOUBLE runs recorded controllers and has no initialized
            // smart trainer flags. Enter the ordinary native AI context only
            // for this query; never treat uninitialized zero caches as proof.
            gBattleTypeFlags = flags & ~(BATTLE_TYPE_RECORDED | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_RECORDED_IS_MASTER | BATTLE_TYPE_LINK);
            gBattlerBattleController[B_BATTLER_1] = BATTLE_CONTROLLER_OPPONENT;
            gBattlerBattleController[B_BATTLER_3] = BATTLE_CONTROLLER_OPPONENT;
            BattleAI_SetupFlags();
            Ai_InitPartyStruct();
            gBattleStruct->wish[B_BATTLER_1].counter = counter;
            SetAiLogicDataForTurn(gAiLogicData);
            EXPECT(gAiThinkingStruct->aiFlags[B_BATTLER_1] & AI_FLAG_SMART_MON_CHOICES);
            EXPECT_GT(gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_1][0].median, 0);
            s32 value = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            gAiLogicData->battlerMovesScored = 0;
            bool32 selected = AI_ComputeDoublesDecisions(B_BATTLER_1);
            EXPECT(selected);
            u32 left = gAiBattleData->chosenMoveIndex[B_BATTLER_1];
            u32 right = gAiBattleData->chosenMoveIndex[B_BATTLER_3];
            Test_MgbaPrintf("HALEY_WISH_PENDING_QUERY counter=%d value=%d selected=%d moves=%S/%S slots=%d/%d switches=%d/%d HP=%d/%d sourceParty=%d tboltMedian=%d",
                counter, value, selected,
                GetMoveName(left < MAX_MON_MOVES ? opponentLeft->moves[left] : MOVE_NONE),
                GetMoveName(right < MAX_MON_MOVES ? opponentRight->moves[right] : MOVE_NONE), left, right,
                gBattleStruct->monToSwitchIntoId[B_BATTLER_1], gBattleStruct->monToSwitchIntoId[B_BATTLER_3],
                opponentLeft->hp, opponentRight->hp, gBattleStruct->wish[B_BATTLER_1].partyId,
                gAiLogicData->simulatedDmg[B_BATTLER_0][B_BATTLER_1][0].median);
            // Counter zero/two are deliberate state-only counterfactuals.
            // No equality or preferred-move assertion predetermines the result.
        }
        // Pair routines restore their trial boards internally. Discarded AI
        // query caches need no snapshot at test teardown; restore explicit
        // native-state interventions and RNG, without a heap-budget change.
        gBattleStruct->wish[B_BATTLER_1].counter = savedCounter;
        gRngValue = rng;
        gRng2Value = rng2;
        gBattleTypeFlags = flags;
        for (u32 b = 0; b < MAX_BATTLERS_COUNT; b++)
            gBattlerBattleController[b] = controllers[b];
        EXPECT_EQ(gBattleStruct->wish[B_BATTLER_1].counter, 1);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_WISH);
    }
}

DOUBLE_BATTLE_TEST("EC live review Haley pending Wish: native next-turn Protect versus attack", s16 firstDamage, s16 secondDamage, s16 recoil, s16 healing)
{
    bool32 protect;
    PARAMETRIZE { protect = TRUE; }
    PARAMETRIZE { protect = FALSE; }
    GIVEN { HaleyWishBoard(); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_WISH);
            MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            if (protect)
                MOVE(opponentLeft, MOVE_PROTECT);
            else
                MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_PSYCHIC, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].firstDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, opponentLeft);
        if (protect)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponentLeft);
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, playerLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].secondDamage);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, opponentLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].recoil);
        }
        HP_BAR(opponentLeft, captureDamage: &results[i].healing);
    } THEN {
        EXPECT_EQ(gBattleStruct->wish[B_BATTLER_1].counter, 0);
        EXPECT_LT(results[i].healing, 0);
        Test_MgbaPrintf("HALEY_WISH_PENDING_NATIVE protect=%d firstDamage=%d secondDamage=%d recoil=%d healing=%d EeveeHP=%d/%d RaltsHP=%d foeHP=%d/%d LAST=%S/%S faints=%d/%d",
            protect, results[i].firstDamage, results[i].secondDamage, results[i].recoil, results[i].healing,
            opponentLeft->hp, opponentLeft->maxHP, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // These forced continuations demonstrate native timing/payoff only.
    }
}
