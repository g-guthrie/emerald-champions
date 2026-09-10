// Temporary actual-team latency observation, not a campaign ceiling.
#line 26000 "work/james_timing_probe.inc"
#include "main.h"
#include "battle_ai_switch.h"

AI_DOUBLE_BATTLE_TEST("EC live review James timing: six native player members without outer snapshot")
{
    GIVEN {
        for (u32 slot = 0; slot < 3; slot++)
        {
            JamesLeadPlayer(SPECIES_EEVEE, &sJamesEevee);
            JamesLeadPlayer(SPECIES_TIMBURR, &sJamesTimburr);
        }
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        struct PreviousDamageLayout { u16 minimum, median, maximum, random; u8 consumedItem; };
        EXPECT_EQ(sizeof(struct SimulatedDamage), sizeof(struct PreviousDamageLayout));
        // An outer snapshot would artificially consume the production memory
        // budget. The evaluator restores its board internally; invalidate only
        // the cached decisions, as in the corrected Woods timing observation.
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 warm = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 cold = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 coldFrames = gMain.vblankCounter1 - start;
        Test_MgbaPrintf("JAMES_TIMING selected=%d/%d warmFrames=%d cacheRebuildFrames=%d simulatedDamageBytes=%d active=%d/%d HP=%d/%d weather=%d",
            warm, cold, warmFrames, coldFrames, sizeof(struct SimulatedDamage),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp, gBattleWeather);
    }
}
