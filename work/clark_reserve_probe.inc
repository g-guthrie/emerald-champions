// Temporary E0023 actual reserve observations; include after ClarkPlayer.
#line 52000 "work/clark_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Clark reserves: physical special and injured Defeatist observations")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    PARAMETRIZE { board = 2; turns = 3; }
    GIVEN {
        ClarkPlayer(board == 1 ? SPECIES_PACHIRISU : SPECIES_EEVEE,
            board == 1 ? &sClarkPachirisu : &sClarkEevee);
        ClarkPlayer(board == 1 ? SPECIES_LOTAD : SPECIES_TIMBURR,
            board == 1 ? &sClarkLotad : &sClarkTimburr);
        AuthoredOpponent(TRAINER_CLARK, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_ROGGENROLA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_GEODUDE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MUDBRAY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ARCHEN);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_STAMINA);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_DEFEATIST);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_NONE);
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
            Test_MgbaPrintf("CLARK_RESERVE_INPUT board=%d slot=%d species=%d level=%d HP=%d max=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_MAX_HP),
                GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
                GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
                GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM),
                GetMonAbility(&party[slot]));
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 1)
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
        Test_MgbaPrintf("CLARK_RESERVE_AFTER board=%d turns=%d moves=%S/%S HP=%d/%d foes=%d/%d Def=%d/%d speeds=%d/%d foeSpeStages=%d/%d item=%d/%d faints=%d/%d",
            board, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            gAiLogicData->speedStats[B_BATTLER_1], gAiLogicData->speedStats[B_BATTLER_3],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            opponentLeft->item, opponentRight->item,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
        // No mandated trainer decision. The injury comparison tests the known
        // starting Defeatist context; this is not an isolated mechanics control.
        // Last-move records can persist after a denied action; use native HP,
        // stages and the decision-time trace to interpret actual execution.
    }
}
