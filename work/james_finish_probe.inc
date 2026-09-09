// Temporary actual-party three-turn followthrough, after James lead helpers.
// Explicit skips follow the already observed native KO timing; no extra move
// is inserted into a fainted player's four-slot set. All AI choices are free.
#line 31000 "work/james_finish_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review James followthrough: three lead boards and physical reserves")
{
    u32 board;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    PARAMETRIZE { board = 2; }
    PARAMETRIZE { board = 3; }
    GIVEN {
        JamesLeadPlayer(board == 1 ? SPECIES_NATU : board == 2 ? SPECIES_MIENFOO : SPECIES_EEVEE,
            board == 1 ? &sJamesNatu : board == 2 ? &sJamesMienfoo : &sJamesEevee);
        JamesLeadPlayer(board == 1 || board == 2 ? SPECIES_PACHIRISU : SPECIES_TIMBURR,
            board == 1 || board == 2 ? &sJamesPachirisu : &sJamesTimburr);
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
        if (board == 3)
        {
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
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("JAMES_FINISH_INPUT board=%d slot=%d species=%d level=%d HP=%d Atk=%d SpD=%d Speed=%d item=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_ATK),
                GetMonData(&party[slot], MON_DATA_SPDEF), GetMonData(&party[slot], MON_DATA_SPEED),
                GetMonData(&party[slot], MON_DATA_HELD_ITEM));
    } WHEN {
        for (u32 turn = 0; turn < 3; turn++) TURN {
            if (board == 0)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                else
                    SKIP_TURN(playerLeft);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else if (board == 1)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                else
                    SKIP_TURN(playerLeft);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else if (board == 2)
            {
                if (turn < 2)
                    MOVE(playerLeft, turn == 0 ? MOVE_FAKE_OUT : MOVE_DRAIN_PUNCH, target: opponentLeft,
                        hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                else
                    SKIP_TURN(playerLeft);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                // The observed second turn loses Kricketune, leaving Venipede
                // in the left slot. Third-turn player attacks target it.
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: turn < 2 ? opponentRight : opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JAMES_FINISH_AFTER board=%d nativeTurns=%d weather=%d rainTimer=%d active=%d/%d party=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownFaints=%d foeFaints=%d",
            board, gBattleResults.battleTurnCounter, gBattleWeather, gBattleStruct->weatherDuration,
            opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("JAMES_FINISH_PARTY board=%d slot=%d species=%d HP=%d/%d status=%d",
                board, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
