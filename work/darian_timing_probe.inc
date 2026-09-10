// Temporary board timing, not a campaign latency ceiling.
#line 47000 "work/darian_timing_probe.inc"
#include "main.h"
AI_DOUBLE_BATTLE_TEST("EC live review Darian timing: six native player members")
{
    GIVEN {
        DarianReservePlayer(SPECIES_PACHIRISU, &sDarianReservePachirisu);
        DarianReservePlayer(SPECIES_LOTAD, &sDarianReserveLotad);
        DarianReservePlayer(SPECIES_CHESPIN, &sDarianReserveChespin);
        DarianReservePlayer(SPECIES_SNIVY, &sDarianReserveSnivy);
        DarianReservePlayer(SPECIES_PACHIRISU, &sDarianReservePachirisu);
        DarianReservePlayer(SPECIES_LOTAD, &sDarianReserveLotad);
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
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
        Test_MgbaPrintf("DARIAN_TIMING selected=%d/%d warmFrames=%d cacheRebuildFrames=%d", warm, cold, warmFrames, coldFrames);
    }
}
