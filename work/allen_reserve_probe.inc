// Temporary actual-party reserve diagnostic, included after Allen's native
// player helper. No permanent trainer suite and no prescribed AI action.
#line 15000 "work/allen_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Allen reserves: protected attack then Truant continuation")
{
    bool32 reservesAvailable;
    PARAMETRIZE { reservesAvailable = TRUE; }
    PARAMETRIZE { reservesAvailable = FALSE; }
    GIVEN {
        AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
        AllenProbePlayer(SPECIES_LOTAD, &sAllenProbeLotad);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        if (!reservesAvailable)
        {
            u16 zero = 0;
            SetMonData(&party[0], MON_DATA_HP, &zero);
            SetMonData(&party[1], MON_DATA_HP, &zero);
        }
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_SLAKOTH);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_TRUANT);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_BUNNELBY);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_HUGE_POWER);
        // Native battle initialization sends the first living slots; changing
        // the DSL's currentMonIndexes alone does not select active reserves.
        // Permute the exact generated Pokemon for this later-board snapshot.
        // All four have explicit native moves/speeds and legal (not forced)
        // abilities; no deferred per-slot override needs moving with them.
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("ALLEN_RESERVE_INPUT spare=%d slot=%d species=%d level=%d hp=%d atk=%d def=%d spa=%d spd=%d speed=%d item=%d ability=%d",
                reservesAvailable, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
                GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
                GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight);
        }
    } THEN {
        Test_MgbaPrintf("ALLEN_RESERVE_AFTER spare=%d slots=%d/%d species=%d/%d hp=%d/%d foeHP=%d/%d last=%S/%S loaf=%d/%d",
            reservesAvailable, gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->volatiles.truantCounter, opponentRight->volatiles.truantCounter);
        // Starting on reserves represents a reachable later board, not proof
        // that the AI independently selected this exact previous switch path.
    }
}
