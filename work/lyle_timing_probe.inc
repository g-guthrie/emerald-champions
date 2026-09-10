// Temporary actual-team latency observation, not a campaign ceiling.
#line 65000 "work/lyle_timing_probe.inc"
#include "main.h"
#include "battle_ai_switch.h"
AI_DOUBLE_BATTLE_TEST("EC live review Lyle timing: six native player members")
{
    GIVEN {
        for (u32 slot = 0; slot < 3; slot++)
        {
            LylePivotPlayer(SPECIES_EEVEE, &sLylePivotEevee);
            LylePivotPlayer(SPECIES_TIMBURR, &sLylePivotTimburr);
        }
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
        EXPECT(state != NULL);
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 warm = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        AI_RestoreCandidateState(state);
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 cold = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 coldFrames = gMain.vblankCounter1 - start;
        AI_RestoreCandidateState(state);
        AI_FreeCandidateState(state);
        Test_MgbaPrintf("LYLE_TIMING selected=%d/%d warmFrames=%d cacheRebuildFrames=%d", warm, cold, warmFrames, coldFrames);
    }
}
