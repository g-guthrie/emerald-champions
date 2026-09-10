// Temporary actual-party room-state queries and latency, not natural expiry proof.
#line 47000 "work/winston_room_probe.inc"
#include "main.h"

AI_DOUBLE_BATTLE_TEST("EC live review Winston room: current native team absent active and expiring field")
{
    u32 timer;
    PARAMETRIZE { timer = 0; }
    PARAMETRIZE { timer = 2; }
    PARAMETRIZE { timer = 1; }
    GIVEN {
        WinstonLeadPlayer(SPECIES_EEVEE, &sWinstonEevee);
        WinstonLeadPlayer(SPECIES_TIMBURR, &sWinstonTimburr);
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_PORYGON);
        EXPECT_EQ(opponentRight->species, SPECIES_HONEDGE);
        u32 savedFields = gFieldStatuses;
        u8 savedTimer = gFieldTimers.trickRoomTimer;
        // Explicit field-only diagnostic after a genuine native first turn.
        // It does not claim these three states occurred on one battle path.
        if (timer)
            gFieldStatuses |= STATUS_FIELD_TRICK_ROOM;
        else
            gFieldStatuses &= ~STATUS_FIELD_TRICK_ROOM;
        gFieldTimers.trickRoomTimer = timer;
        SetAiLogicDataForTurn(gAiLogicData);
        EXPECT(AI_ComputeDoublesDecisions(B_BATTLER_1));
        u32 slot = gAiBattleData->chosenMoveIndex[B_BATTLER_1];
        enum Move move = opponentLeft->moves[slot];
        bool32 clear = ShouldClearFieldStatus(B_BATTLER_1, STATUS_FIELD_TRICK_ROOM);
        Test_MgbaPrintf("WINSTON_ROOM_QUERY timer=%d set=%d clear=%d moves=%S/%S switches=%d HP=%d/%d stages=%d/%d",
            timer, ShouldSetFieldStatus(B_BATTLER_1, STATUS_FIELD_TRICK_ROOM), clear, GetMoveName(move),
            GetMoveName(opponentRight->moves[gAiBattleData->chosenMoveIndex[B_BATTLER_3]]),
            gAiLogicData->shouldSwitch, opponentLeft->hp, opponentRight->hp,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_ATK]);
        if (timer == 1 || (timer && !clear))
            EXPECT_NE(move, MOVE_TRICK_ROOM);
        gFieldStatuses = savedFields;
        gFieldTimers.trickRoomTimer = savedTimer;
    }
}
