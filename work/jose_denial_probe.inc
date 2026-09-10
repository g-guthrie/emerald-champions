// Temporary native Jose denial/transition observations. Include after JosePlayer.
// Reserve board is a constructed later phase; it does not claim the opening
// naturally reaches that state. Full native trainer/player menus remain live.
#line 53000 "work/jose_denial_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Jose denial: lead disruption and Grass powder counterplay")
{
    bool32 reserves;
    u32 turns;
    PARAMETRIZE { reserves = FALSE; turns = 1; }
    PARAMETRIZE { reserves = FALSE; turns = 2; }
    PARAMETRIZE { reserves = FALSE; turns = 3; }
    PARAMETRIZE { reserves = TRUE; turns = 1; }
    PARAMETRIZE { reserves = TRUE; turns = 2; }
    PARAMETRIZE { reserves = TRUE; turns = 3; }
    GIVEN {
        JosePlayer(SPECIES_MIENFOO, &sJoseMienfoo);
        JosePlayer(reserves ? SPECIES_LOTAD : SPECIES_PACHIRISU,
            reserves ? &sJoseLotad : &sJosePachirisu);
        AuthoredOpponent(TRAINER_JOSE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_DUSTOX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_BEAUTIFLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_VENONAT);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_SHIELD_DUST);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_SWARM);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_TECHNICIAN);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_COMPOUND_EYES);
        if (reserves)
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
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOSE_DENIAL_INPUT r=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d",
                    reserves, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
                Test_MgbaPrintf("JOSE_DENIAL_MENU r=%d owner=%d slot=%d moves=%d/%d/%d/%d",
                    reserves, owner, slot, GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0)
                // Beautifly/Venonat lack Shield Dust. Do not pretend Fake Out
                // flinches Dustox through its actual Shield Dust ability.
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else if (turn == 1)
                MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            else
                MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            if (reserves)
                // Grass Lotad ignores Rage Powder and Sleep Powder, but not
                // Bug attacks; this is a real tradeoff, not a guaranteed wall.
                MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (turn == 0)
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOSE_DENIAL_END r=%d t=%d native=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d faints=%d/%d",
            reserves, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOSE_DENIAL_MOVES r=%d t=%d own=%S/%S player=%S/%S",
            reserves, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]));
        Test_MgbaPrintf("JOSE_DENIAL_STATE r=%d t=%d SpA=%d/%d SpD=%d/%d Speed=%d/%d taunt=%d/%d foeStatus=%d/%d wind=%d",
            reserves, turns, opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            playerLeft->status1, playerRight->status1, !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND));
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOSE_DENIAL_PARTY r=%d t=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                reserves, turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
        }
        // No chosen trainer move, denial success, setup, switch or win asserted.
        // New early player faints may need an explicit native timeline update.
    }
}
