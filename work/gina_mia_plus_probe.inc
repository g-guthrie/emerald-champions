// Temporary actual-team Plus/Minus timing; include after Gina/Mia lead helpers.
#line 55000 "work/gina_mia_plus_probe.inc"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"
#include "battle_ai_util.h"
#include "battle_controllers.h"

DOUBLE_BATTLE_TEST("EC live review Gina Plus: partner survives or faints before Thunderbolt", s16 thunderbolt)
{
    u32 control;
    PARAMETRIZE { control = 0; } // Native full-health partner survives priority hit.
    PARAMETRIZE { control = 1; } // Native injured partner faints before Thunderbolt.
    PARAMETRIZE { control = 2; } // Initialized AI cache comparison, no second turn.
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_QUICK_ATTACK, MOVE_DOUBLE_EDGE, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        GinaMiaLeadPlayer(SPECIES_EEVEE, &eevee);
        GinaMiaLeadPlayer(SPECIES_TIMBURR, &sGinaMiaTimburr);
        AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
        if (control)
        {
            u16 injury = 1;
            SetMonData(&party[1], MON_DATA_HP, &injury);
            // Actual Sitrus is retained; any native healing before turn two
            // must still leave a genuinely lethal priority-hit comparison.
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        if (control != 2)
        {
            TURN {
                MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_PROTECT);
                MOVE(opponentLeft, MOVE_THUNDERBOLT, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(opponentRight, MOVE_CHARM, target: playerRight);
                if (control == 1)
                    SEND_OUT(opponentRight, 2);
            }
        }
    } SCENE {
        if (control != 2)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentLeft);
            HP_BAR(playerLeft, captureDamage: &results[i].thunderbolt);
        }
    } THEN {
        if (control != 2)
        {
            u32 partnerHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP);
            if (control == 1)
                EXPECT_EQ(partnerHp, 0);
            else
                EXPECT_GT(partnerHp, 0);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_THUNDERBOLT);
            Test_MgbaPrintf("GINA_PLUS_NATIVE control=%d damage=%d partnerPartyHP=%d PlusHP=%d targetHP=%d/%d replacement=%d",
                control, results[i].thunderbolt, partnerHp, opponentLeft->hp,
                playerLeft->hp, playerLeft->maxHP, opponentRight->species);
        }
        else
        {
            u32 flags = gBattleTypeFlags;
            u32 controllers[MAX_BATTLERS_COUNT];
            for (u32 b = 0; b < MAX_BATTLERS_COUNT; b++)
                controllers[b] = gBattlerBattleController[b];
            struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
            EXPECT(state != NULL);
            gBattleTypeFlags = flags & ~(BATTLE_TYPE_RECORDED | BATTLE_TYPE_RECORDED_LINK | BATTLE_TYPE_RECORDED_IS_MASTER | BATTLE_TYPE_LINK);
            gBattlerBattleController[B_BATTLER_1] = BATTLE_CONTROLLER_OPPONENT;
            gBattlerBattleController[B_BATTLER_3] = BATTLE_CONTROLLER_OPPONENT;
            BattleAI_SetupFlags();
            Ai_InitPartyStruct();
            SetAiLogicDataForTurn(gAiLogicData);
            EXPECT(gAiThinkingStruct->aiFlags[B_BATTLER_1] & AI_FLAG_OMNISCIENT);
            u32 index = GetMoveIndex(B_BATTLER_1, MOVE_THUNDERBOLT);
            struct SimulatedDamage alive = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][index];
            EXPECT_GT(alive.median, 0);
            // Explicit state-only native-cache counterfactual. Do not confuse
            // it with an actual additional KO or a full paired evaluation.
            gBattleMons[B_BATTLER_3].hp = 0;
            gBattleStruct->battlerState[B_BATTLER_3].notOnField = TRUE;
            SetAiLogicDataForTurn(gAiLogicData);
            struct SimulatedDamage absent = gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][index];
            Test_MgbaPrintf("GINA_PLUS_CACHE alive=%d/%d/%d absent=%d/%d/%d absentAbility=%d",
                alive.minimum, alive.median, alive.maximum, absent.minimum, absent.median, absent.maximum,
                gAiLogicData->abilities[B_BATTLER_3]);
            EXPECT_GT(alive.minimum, absent.maximum);
            AI_RestoreCandidateState(state);
            AI_FreeCandidateState(state);
            gBattleTypeFlags = flags;
            for (u32 b = 0; b < MAX_BATTLERS_COUNT; b++)
                gBattlerBattleController[b] = controllers[b];
        }
    } FINALLY {
        EXPECT_GT(results[0].thunderbolt, results[1].thunderbolt);
    }
}
