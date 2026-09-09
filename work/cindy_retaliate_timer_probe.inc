// Temporary native candidate context observer; after Cindy revenge AI helper.
#line 58000 "work/cindy_retaliate_timer_probe.inc"
#include "battle_ai_switch.h"
#include "battle_main.h"

AI_DOUBLE_BATTLE_TEST("EC live review Cindy Retaliate timer: replacement tick expiry and excluded contexts")
{
    u32 timer, phase;
    PARAMETRIZE { timer = 2; phase = 0; }
    PARAMETRIZE { timer = 1; phase = 0; }
    PARAMETRIZE { timer = 0; phase = 0; }
    PARAMETRIZE { timer = 2; phase = 1; }
    PARAMETRIZE { timer = 2; phase = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_SUPER_FANG, MOVE_ROUND, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        CindyRevengeAiPlayer(SPECIES_PACHIRISU, &pachirisu);
        CindyRevengeAiPlayer(SPECIES_EEVEE, &eevee);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SLAKOTH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE3), MOVE_RETALIATE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        struct SwitchCandidateSnapshot *original = AI_SaveCandidateState();
        EXPECT(original != NULL);
        // The unrestricted harmless turn may already pivot Milcery out.
        // Explicitly load its real record for this initialized boundary check.
        AI_LoadSwitchCandidate(B_BATTLER_3, 1, FALSE);
        SetAiLogicDataForTurn(gAiLogicData);
        // Deliberately initialized timing boundaries, not fabricated damage
        // or a claim these timers were produced by this harmless native turn.
        u16 zero = 0;
        gBattleMons[B_BATTLER_3].hp = 0;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &zero);
        for (enum BattlerId actor = B_BATTLER_1; actor <= B_BATTLER_3; actor += 2)
        {
            gBattleStruct->monToSwitchIntoId[actor] = PARTY_SIZE;
            gBattleStruct->AI_monToSwitchIntoId[actor] = PARTY_SIZE;
        }
        gCurrentTurnActionNumber = phase == 2 ? 0 : gBattlersCount;
        gBattleStruct->eventState.beforeFirstTurn = phase == 1 ? FIRST_TURN_FAINTED_BATTLERS : 0;
        gSideTimers[B_SIDE_OPPONENT].retaliateTimer = timer;
        gSideTimers[B_SIDE_PLAYER].retaliateTimer = timer;
        rng_value_t rng = gRngValue, rng2 = gRng2Value;
        Test_MgbaPrintf("CINDY_TIMER_BEGIN timer=%d phase=%d expectedTrialTimer=%d action=%d beforeFirst=%d",
            timer, phase, phase == 0 && timer ? timer - 1 : timer,
            gCurrentTurnActionNumber, gBattleStruct->eventState.beforeFirstTurn);
        // Existing temporary CINDY_FORECAST output observes the actual timer
        // inside each loaded Furfrou/Slakoth evaluation, before it restores.
        u32 selected = GetMostSuitableMonToSwitchInto(B_BATTLER_3,
            phase == 2 ? SWITCH_MID_BATTLE_FORCED : SWITCH_AFTER_KO);
        EXPECT(selected == 2 || selected == 3);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].retaliateTimer, timer);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].retaliateTimer, timer);
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_MILCERY);
        EXPECT_EQ(gBattleMons[B_BATTLER_3].hp, 0);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 1);
        EXPECT_EQ(memcmp(&gRngValue, &rng, sizeof(rng)), 0);
        EXPECT_EQ(memcmp(&gRng2Value, &rng2, sizeof(rng2)), 0);
        Test_MgbaPrintf("CINDY_TIMER_RESTORED timer=%d phase=%d selected=%d bothTimers=%d/%d",
            timer, phase, selected, gSideTimers[B_SIDE_PLAYER].retaliateTimer,
            gSideTimers[B_SIDE_OPPONENT].retaliateTimer);

        // Native damage reference, using the unchanged compiled Slakoth and
        // Eevee: timer1 boosts while timer0 does not. No hardcoded stats/power.
        AI_LoadSwitchCandidate(B_BATTLER_3, 3, FALSE);
        SetAiLogicDataForTurn(gAiLogicData);
        u32 damage[2];
        for (u32 window = 0; window < 2; window++)
        {
            gSideTimers[B_SIDE_OPPONENT].retaliateTimer = window;
            CalcBattlerAiMovesData(gAiLogicData, B_BATTLER_3, B_BATTLER_2, AI_GetWeather(), gFieldTimers.terrain);
            damage[window] = gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_2][2].median;
        }
        EXPECT_GT(damage[1], damage[0]);
        Test_MgbaPrintf("CINDY_TIMER_NATIVE_DAMAGE expired=%d revenge=%d switchCalc=%d",
            damage[0], damage[1], gAiLogicData->switchInCalc);
        AI_RestoreCandidateState(original);
        AI_FreeCandidateState(original);
    }
}
