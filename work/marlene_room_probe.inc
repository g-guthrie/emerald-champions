// Actual generated team, explicit field-state decision queries. This is not a
// claim that all three states arose along one natural six-turn battle path.
#line 57000 "work/marlene_room_probe.inc"
#include "random.h"

AI_DOUBLE_BATTLE_TEST("EC live review Marlene Room: absent active and expiring native team decisions")
{
    u32 timer;
    PARAMETRIZE { timer = 0; }
    PARAMETRIZE { timer = 2; }
    PARAMETRIZE { timer = 1; }
    GIVEN {
        MarlenePlayer(SPECIES_PACHIRISU, &sMarlenePachirisu);
        MarlenePlayer(SPECIES_TIMBURR, &sMarleneTimburr);
        AuthoredOpponent(TRAINER_MARLENE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPOINK);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SLOWPOKE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BALTOY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_GOTHITA);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_SPOINK);
        EXPECT_EQ(opponentRight->species, SPECIES_SLOWPOKE);
        u32 savedFields = gFieldStatuses;
        u8 savedTimer = gFieldTimers.trickRoomTimer;
        rng_value_t savedRng = gRngValue, savedRng2 = gRng2Value;
        if (timer)
            gFieldStatuses |= STATUS_FIELD_TRICK_ROOM;
        else
            gFieldStatuses &= ~STATUS_FIELD_TRICK_ROOM;
        gFieldTimers.trickRoomTimer = timer;
        // AI_DOUBLE's native first turn supplies the ordinary initialized AI
        // context. No outer 8-KiB snapshot around the evaluator's own snapshots.
        SetAiLogicDataForTurn(gAiLogicData);
        gAiLogicData->battlerMovesScored = 0;
        bool32 clear = ShouldClearFieldStatus(B_BATTLER_1, STATUS_FIELD_TRICK_ROOM);
        bool32 set = ShouldSetFieldStatus(B_BATTLER_1, STATUS_FIELD_TRICK_ROOM);
        EXPECT(AI_ComputeDoublesDecisions(B_BATTLER_1));
        u32 leftSlot = gAiBattleData->chosenMoveIndex[B_BATTLER_1];
        u32 rightSlot = gAiBattleData->chosenMoveIndex[B_BATTLER_3];
        enum Move leftMove = leftSlot < MAX_MON_MOVES ? opponentLeft->moves[leftSlot] : MOVE_NONE;
        enum Move rightMove = rightSlot < MAX_MON_MOVES ? opponentRight->moves[rightSlot] : MOVE_NONE;
        bool32 leftSwitch = (gAiLogicData->shouldSwitch & (1u << B_BATTLER_1)) != 0;
        if (!leftSwitch && (timer == 1 || (timer && !clear)))
            EXPECT_NE(leftMove, MOVE_TRICK_ROOM);
        Test_MgbaPrintf("MARLENE_ROOM_QUERY timer=%d set=%d clear=%d moves=%S/%S switch=%d speed=%d/%d foespeed=%d/%d HP=%d/%d",
            timer, set, clear, GetMoveName(leftMove), GetMoveName(rightMove), gAiLogicData->shouldSwitch,
            gAiLogicData->speedStats[B_BATTLER_1], gAiLogicData->speedStats[B_BATTLER_3],
            gAiLogicData->speedStats[B_BATTLER_0], gAiLogicData->speedStats[B_BATTLER_2],
            opponentLeft->hp, opponentRight->hp);
        EXPECT_EQ(gFieldTimers.trickRoomTimer, timer);
        gFieldStatuses = savedFields;
        gFieldTimers.trickRoomTimer = savedTimer;
        SetAiLogicDataForTurn(gAiLogicData);
        gRngValue = savedRng;
        gRng2Value = savedRng2;
        // Query-selected moves are diagnostic output, not executed turn two.
    }
}
