// Temporary measured native AI timing, no arbitrary pass/fail frame ceiling.
#line 40000 "work/billy_timing_probe.inc"
#include "main.h"

AI_DOUBLE_BATTLE_TEST("EC live review Billy timing: full player party and actual four opponents")
{
    GIVEN {
        BillyProbePlayer(SPECIES_PACHIRISU, &sBillyProbePachirisu);
        BillyProbePlayer(SPECIES_EEVEE, &sBillyProbeEevee);
        BillyProbePlayer(SPECIES_TIMBURR, &sBillyProbeTimburr);
        BillyProbePlayer(SPECIES_MIENFOO, &sBillyProbeMienfoo);
        BillyReservePlayer(SPECIES_PACHIRISU, &sBillyReservePachirisu);
        BillyReservePlayer(SPECIES_TIMBURR, &sBillyReserveTimburr);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        // Repeated species are legal separate catches, not invented abilities
        // or a recommendation that the player use this diagnostic team.
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
        EXPECT(state != NULL);
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 selected = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        AI_RestoreCandidateState(state);
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 coldSelected = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 coldFrames = gMain.vblankCounter1 - start;
        AI_RestoreCandidateState(state);
        AI_FreeCandidateState(state);
        Test_MgbaPrintf("BILLY_TIMING selected=%d/%d warmFrames=%d withCacheRebuildFrames=%d",
            selected, coldSelected, warmFrames, coldFrames);
        // Neither one board nor the warmed probe is a worst-case guarantee.
    }
}
