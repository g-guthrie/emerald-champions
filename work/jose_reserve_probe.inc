// Temporary actual E0021 reserve observations; include after JosePlayer.
#line 52000 "work/jose_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Jose reserves: ordinary pressure Grass powder immunity and priority denial")
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
        JosePlayer(board == 2 ? SPECIES_MIENFOO : SPECIES_PACHIRISU,
            board == 2 ? &sJoseMienfoo : &sJosePachirisu);
        JosePlayer(board == 0 ? SPECIES_TIMBURR : board == 1 ? SPECIES_LOTAD : SPECIES_PACHIRISU,
            board == 0 ? &sJoseTimburr : board == 1 ? &sJoseLotad : &sJosePachirisu);
        AuthoredOpponent(TRAINER_JOSE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_DUSTOX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_BEAUTIFLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_VENONAT);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_COMPOUND_EYES);
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
            Test_MgbaPrintf("JOSE_RESERVE_INPUT board=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_ATK),
                GetMonData(&party[slot], MON_DATA_DEF), GetMonData(&party[slot], MON_DATA_SPATK),
                GetMonData(&party[slot], MON_DATA_SPDEF), GetMonData(&party[slot], MON_DATA_SPEED),
                GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 2)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                if (board == 0)
                    MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JOSE_RESERVE_AFTER board=%d turns=%d moves=%S/%S HP=%d/%d foes=%d/%d sleep=%d/%d taunt=%d/%d SpA=%d/%d faints=%d/%d",
            board, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            (u32)opponentLeft->volatiles.tauntTimer, (u32)opponentRight->volatiles.tauntTimer,
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        if (board == 1)
            EXPECT_EQ(playerRight->status1 & STATUS1_SLEEP, 0);
        // Unrestricted native trainer choices. A Grass attacker ignores Rage
        // Powder and Sleep Powder; spread moves are never pulled by redirection.
        // Executed last moves may persist across a flinch or sleep denial, so
        // use the decision trace, native HP and statuses together.
    }
}
