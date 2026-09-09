// Temporary native setup-Speed score comparison; include after Darian helpers.
#line 44000 "work/darian_speed_probe.inc"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"

DOUBLE_BATTLE_TEST("EC live review Darian setup Speed: genuine crossing versus still slower")
{
    bool32 fast, reserves;
    PARAMETRIZE { fast = FALSE; reserves = FALSE; }
    PARAMETRIZE { fast = TRUE; reserves = FALSE; }
    PARAMETRIZE { fast = FALSE; reserves = TRUE; }
    PARAMETRIZE { fast = TRUE; reserves = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_TACKLE, MOVE_HELPING_HAND, MOVE_WISH, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_RUN_AWAY,
            .statPoints = {32, 0, 2, 0, 32, 0},
        };
        if (fast)
        {
            // Preset display order is HP/Atk/Def/SpA/SpD/Speed, not STAT_*.
            eevee.statPoints[4] = 0;
            eevee.statPoints[5] = 32;
        }
        DarianReservePlayer(SPECIES_EEVEE, &eevee);
        DarianReservePlayer(SPECIES_EEVEE, &eevee);
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
        if (reserves)
        {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
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
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            if (reserves)
                MOVE(opponentLeft, MOVE_AQUA_JET, target: playerLeft);
            else
                MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
        EXPECT(state != NULL);
        SetAiLogicDataForTurn(gAiLogicData);
        enum BattlerId actor = B_BATTLER_3, target = B_BATTLER_0;
        s32 stages = reserves ? 2 : 1;
        u32 originalStage = opponentRight->statStages[STAT_SPEED];
        u32 oldSpeed = gAiLogicData->speedStats[actor];
        u32 targetSpeed = gAiLogicData->speedStats[target];
        opponentRight->statStages[STAT_SPEED] = min(MAX_STAT_STAGE, originalStage + stages);
        u32 boosted = GetBattlerTotalSpeedStat(actor, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor]);
        opponentRight->statStages[STAT_SPEED] = originalStage;
        s32 speedScore = IncreaseStatUpScore(actor, target, STAT_SPEED, stages);
        s32 fullScore = GetSelfStatChangeScore(actor, target, reserves ? MOVE_SHELL_SMASH : MOVE_DRAGON_DANCE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], originalStage);
        EXPECT_EQ(gAiLogicData->speedStats[actor], oldSpeed);
        EXPECT_LT(oldSpeed, targetSpeed);
        if (fast)
        {
            EXPECT_LT(boosted, targetSpeed);
            EXPECT_EQ(speedScore, NO_INCREASE);
        }
        else
        {
            EXPECT_GT(boosted, targetSpeed);
            EXPECT_GT(speedScore, NO_INCREASE);
        }
        Test_MgbaPrintf("DARIAN_SPEED reserves=%d fast=%d actor=%d HP=%d nativeSpeed=%d boosted=%d target=%d speedScore=%d wholeSetupScore=%d",
            reserves, fast, opponentRight->species, opponentRight->hp, oldSpeed, boosted, targetSpeed, speedScore, fullScore);
        AI_RestoreCandidateState(state);
        AI_FreeCandidateState(state);
        // Scores on actual generated stats, not a mandated AI setup choice.
        // Whole-move score may retain legitimate Attack/SpA utility even when
        // its Speed component cannot overtake the real opposing Eevee.
    }
}
