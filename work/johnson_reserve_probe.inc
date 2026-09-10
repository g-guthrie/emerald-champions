// Actual E0023 Johnson reserves; include after JohnsonPlayer, not Clark.
#line 52000 "work/johnson_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Johnson reserves: native friendship physical special and Fake Out pressure")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    PARAMETRIZE { board = 2; turns = 3; }
    PARAMETRIZE { board = 2; turns = 4; }
    GIVEN {
        JohnsonPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_PACHIRISU : SPECIES_MIENFOO,
            board == 0 ? &sJohnsonEevee : board == 1 ? &sJohnsonPachirisu : &sJohnsonMienfoo);
        JohnsonPlayer(board == 0 ? SPECIES_TIMBURR : board == 1 ? SPECIES_LOTAD : SPECIES_PACHIRISU,
            board == 0 ? &sJohnsonTimburr : board == 1 ? &sJohnsonLotad : &sJohnsonPachirisu);
        AuthoredOpponent(TRAINER_JOHNSON, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SHINX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_STARLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BUNNELBY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TAILLOW);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_HUGE_POWER);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_GUTS);
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
        for (u32 slot = 0; slot < 2; slot++)
            Test_MgbaPrintf("JOHNSON_RESERVE_INPUT board=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d friendship=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_ATK),
                GetMonData(&party[slot], MON_DATA_DEF), GetMonData(&party[slot], MON_DATA_SPATK),
                GetMonData(&party[slot], MON_DATA_SPDEF), GetMonData(&party[slot], MON_DATA_SPEED),
                GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]),
                GetMonData(&party[slot], MON_DATA_FRIENDSHIP));
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JOHNSON_RESERVE_STATS board=%d turns=%d nativeSpeeds=%d/%d/%d/%d species=%d/%d/%d/%d ownAtk=%d/%d moves=%S/%S/%S/%S",
            board, turns, playerLeft->speed, opponentLeft->speed, playerRight->speed, opponentRight->speed,
            playerLeft->species, opponentLeft->species, playerRight->species, opponentRight->species,
            opponentLeft->attack, opponentRight->attack,
            GetMoveName(opponentLeft->moves[0]), GetMoveName(opponentLeft->moves[1]),
            GetMoveName(opponentLeft->moves[2]), GetMoveName(opponentLeft->moves[3]));
        Test_MgbaPrintf("JOHNSON_RESERVE_AFTER board=%d turns=%d moves=%S/%S HP=%d/%d foes=%d/%d status=%d/%d item=%d/%d friendship=%d/%d faint=%d/%d",
            board, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentLeft->status1, opponentRight->status1, opponentLeft->item, opponentRight->item,
            opponentLeft->friendship, opponentRight->friendship,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOHNSON_NATIVE_CACHE board=%d turns=%d slot2=%S vsLeft=%d/%d/%d vsRight=%d/%d/%d",
            board, turns, GetMoveName(opponentLeft->moves[2]),
            gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][2].minimum,
            gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][2].median,
            gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_0][2].maximum,
            gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_2][2].minimum,
            gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_2][2].median,
            gAiLogicData->simulatedDmg[B_BATTLER_1][B_BATTLER_2][2].maximum);
        // Cache diagnostics may represent next-turn initialization or a stale
        // fainted actor; they are not the damage of the already executed turn.
        // No choices are imposed, and Toxic/Flame Orb remains the compiled item.
    }
}
