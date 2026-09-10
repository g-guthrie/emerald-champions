// Temporary actual-AI spread-pressure diagnostic, after allen_ai_probe.inc.
#line 21000 "work/allen_guard_ai_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Allen Guard AI: two native Heat Wave attackers")
{
    u32 turns;
    bool32 wacan;
    PARAMETRIZE { turns = 1; wacan = FALSE; }
    PARAMETRIZE { turns = 2; wacan = FALSE; }
    PARAMETRIZE { turns = 1; wacan = TRUE; }
    PARAMETRIZE { turns = 2; wacan = TRUE; }
    GIVEN {
        // Two legitimate Route102 captures, each using the existing native
        // level14/IV31 physical-bulk Natu preparation. No stats are overridden.
        struct EmeraldChampionsBattleSet natu = sAllenProbeHeatWaveNatu;
        if (wacan)
            natu.item = ITEM_WACAN_BERRY;
        AllenProbePlayer(SPECIES_NATU, &natu);
        AllenProbePlayer(SPECIES_NATU, &natu);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *paras = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1];
        EXPECT_EQ(GetMonData(paras, MON_DATA_SPECIES), SPECIES_PARAS);
        EXPECT_EQ(GetMonData(paras, MON_DATA_MOVE2), MOVE_WIDE_GUARD);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                MOVE(playerLeft, MOVE_HEAT_WAVE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_HEAT_WAVE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // No prescribed AI move or target. Both native spread threats
                // offer shared protection value; a chosen single-target attack
                // or an earlier KO may still change the best continuation.
            }
        }
    } THEN {
        if (wacan && turns == 1)
            EXPECT(!(gLastMoves[B_BATTLER_1] == MOVE_PROTECT && gLastMoves[B_BATTLER_3] == MOVE_PROTECT));
        Test_MgbaPrintf("Allen Guard AI wacan=%d turns=%d completed=%d LAST=%S/%S activeParty=%d/%d species=%d/%d HP=%d/%d foeHP=%d/%d foeLAST=%S/%S guardUses=%d/%d",
            wacan, turns, gBattleResults.battleTurnCounter,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->volatiles.consecutiveMoveUses, opponentRight->volatiles.consecutiveMoveUses);
        // Root's decision-time trace supplies each turn's actual selections.
        // A PASS records the scenario, not a forced Wide Guard success or a
        // full matchup win. Replacement species/party slots are unrestricted.
    }
}
