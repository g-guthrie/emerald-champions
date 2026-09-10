// Actual reserve records, with an explicit hypothetical carried sleep state.
// This does not claim Lechonk naturally produced the board in an earlier turn.
#line 62000 "work/joey_sleep_window_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Joey sleep window: actual reserve setup against sleeping or awake pressure")
{
    bool32 asleep;
    u32 turns;
    PARAMETRIZE { asleep = FALSE; turns = 1; }
    PARAMETRIZE { asleep = TRUE; turns = 1; }
    PARAMETRIZE { asleep = TRUE; turns = 2; }
    GIVEN {
        JoeyDrumBoard(TRUE);
        if (asleep)
        {
            // Native sleep countdown: 3 becomes2 at the first ordinary move
            // attempt, then1 at the second. Neither chosen attack executes.
            // This is a known current status, not the human's pending command.
            u32 status = STATUS1_SLEEP_TURN(3);
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
            SetMonData(&party[0], MON_DATA_STATUS, &status);
            SetMonData(&party[1], MON_DATA_STATUS, &status);
        }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOEY_SLEEP_WINDOW asleep=%d turns=%d moves=%S/%S ownHP=%d/%d foes=%d/%d stage=%d item=%d sleep=%d/%d faints=%d/%d",
            asleep, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentRight->statStages[STAT_ATK], opponentRight->item,
            playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        if (asleep)
        {
            if (playerLeft->hp)
                EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 3 - turns);
            if (playerRight->hp)
                EXPECT_EQ(playerRight->status1 & STATUS1_SLEEP, 3 - turns);
        }
        // No desired setup move is forced. If the selected attack already
        // secures a KO, that may be preferable to spending the sleep window.
    }
}
