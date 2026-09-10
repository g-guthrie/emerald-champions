// Temporary native Jose observation and separate synthetic shared boundaries.
// Include after JosePlayer. No authored team changes or forced trainer moves.
#line 56000 "work/jose_quiver_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Jose Quiver: actual full party under special pressure")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        JosePlayer(SPECIES_PACHIRISU, &sJosePachirisu);
        JosePlayer(SPECIES_LOTAD, &sJoseLotad);
        AuthoredOpponent(TRAINER_JOSE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_DUSTOX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_BEAUTIFLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_VENONAT);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOSE_QUIVER_INPUT owner=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOSE_QUIVER_END t=%d active=%d/%d HP=%d/%d foes=%d/%d SpA=%d/%d SpD=%d/%d Speed=%d/%d wind=%d faints=%d/%d",
            turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOSE_QUIVER_MOVES t=%d own=%S/%S player=%S/%S",
            turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]));
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOSE_QUIVER_PARTY t=%d slot=%d species=%d HP=%d/%d status=%d item=%d",
                turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_STATUS), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
    }
}
