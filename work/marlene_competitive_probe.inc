// Actual reserve snapshot under opposing special drops, not a forced AI combo.
#line 59000 "work/marlene_competitive_probe.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Marlene Competitive: special drops invite reserve retaliation")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    PARAMETRIZE { turns = 4; }
    GIVEN {
        MarleneYawnBoard(FALSE);
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
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
            Test_MgbaPrintf("MARLENE_COMP_CACHE turns=%d move=%S left=%d/%d right=%d/%d effective=%d/%d", turns,
                GetMoveName(opponentRight->moves[index]),
                gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_0][index].minimum,
                gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_0][index].maximum,
                gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_2][index].minimum,
                gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_2][index].maximum,
                gAiLogicData->effectiveness[B_BATTLER_3][B_BATTLER_0][index],
                gAiLogicData->effectiveness[B_BATTLER_3][B_BATTLER_2][index]);
        Test_MgbaPrintf("MARLENE_COMPETITIVE turns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S SpA=%d/%d faints=%d/%d",
            turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
