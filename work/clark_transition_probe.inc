// Temporary accepted E0023 party followup; no trainer loadout intervention.
// Include after ClarkPlayer. Candidate flag only preserves comparable log labels.
#line 58000 "work/clark_transition_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Clark transitions: accepted Protect through third-turn followup")
{
    u32 board, turns;
    bool32 candidate;
    PARAMETRIZE { board = 0; turns = 2; candidate = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; candidate = TRUE; }
    PARAMETRIZE { board = 1; turns = 3; candidate = TRUE; }
    PARAMETRIZE { board = 2; turns = 2; candidate = TRUE; }
    PARAMETRIZE { board = 2; turns = 3; candidate = TRUE; }
    PARAMETRIZE { board = 3; turns = 2; candidate = TRUE; }
    PARAMETRIZE { board = 3; turns = 3; candidate = TRUE; }
    GIVEN {
        ClarkPlayer(board == 3 ? SPECIES_MIENFOO : board == 1 ? SPECIES_PACHIRISU : SPECIES_EEVEE,
            board == 3 ? &sClarkMienfoo : board == 1 ? &sClarkPachirisu : &sClarkEevee);
        ClarkPlayer(board == 3 ? SPECIES_PACHIRISU : board == 1 ? SPECIES_LOTAD : SPECIES_TIMBURR,
            board == 3 ? &sClarkPachirisu : board == 1 ? &sClarkLotad : &sClarkTimburr);
        AuthoredOpponent(TRAINER_CLARK, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_ROGGENROLA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_GEODUDE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MUDBRAY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ARCHEN);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_STAMINA);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_DEFEATIST);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_NONE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE3), MOVE_PROTECT);
        if (board == 3)
            EXPECT_GT(GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED),
                GetMonData(&party[3], MON_DATA_SPEED));
        Test_MgbaPrintf("CLARK_PROTECT_CASE board=%d turns=%d candidate=%d move3=%d pp3=%d",
            board, turns, candidate, GetMonData(&party[3], MON_DATA_MOVE3), GetMonData(&party[3], MON_DATA_PP3));
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        if (board == 2)
        {
            // Explicit injury, not a claim that this entry sequence occurred.
            // Native species, max HP, item, moves and all derived stats remain.
            u16 halfHp = GetMonData(&party[1], MON_DATA_MAX_HP) / 2;
            SetMonData(&party[1], MON_DATA_HP, &halfHp);
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        for (u32 slot = 0; slot < 2; slot++)
            Test_MgbaPrintf("CLARK_PROTECT_INPUT board=%d slot=%d species=%d level=%d HP=%d max=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_MAX_HP),
                GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
                GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
                GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM),
                GetMonAbility(&party[slot]));
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 3)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("CLARK_PROTECT_AFTER board=%d turns=%d moves=%S/%S HP=%d/%d foes=%d/%d Def=%d/%d speeds=%d/%d foeSpeStages=%d/%d item=%d/%d faints=%d/%d",
            board, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            gAiLogicData->speedStats[B_BATTLER_1], gAiLogicData->speedStats[B_BATTLER_3],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            opponentLeft->item, opponentRight->item,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
        Test_MgbaPrintf("CLARK_PROTECT_RESULT board=%d candidate=%d active=%d/%d slots=%d/%d move3=%d pp3=%d",
            board, candidate, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_MOVE3),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_PP3));
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("CLARK_PROTECT_PARTY board=%d candidate=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                board, candidate, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
        }
        // No mandated trainer decision. The injury comparison tests the known
        // starting Defeatist context, not its within-turn onset.
        // Last-move records can persist after a denied action; use native HP,
        // stages and the decision-time trace to interpret actual execution.
    }
}
