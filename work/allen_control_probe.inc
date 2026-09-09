// Temporary historical native counterfactual; include after allen_ai_probe.inc.
// Preserves the pre-Wide-Guard Reflect slot only for this explicit comparison.
#line 16000 "work/allen_control_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Allen control: actual Fake Out Protect versus Web Spore Reflect", s16 firstPunch; s16 secondPunch)
{
    u32 line;
    PARAMETRIZE { line = 0; }
    PARAMETRIZE { line = 1; }
    PARAMETRIZE { line = 2; }
    GIVEN {
        AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *enemies = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        enum Move historicalMove = MOVE_REFLECT;
        u32 historicalPp = GetMovePP(historicalMove);
        EXPECT_EQ(GetMonData(&enemies[1], MON_DATA_SPECIES), SPECIES_PARAS);
        SetMonData(&enemies[1], MON_DATA_MOVE2, &historicalMove);
        SetMonData(&enemies[1], MON_DATA_PP2, &historicalPp);
        for (u32 slot = 0; slot < 2; slot++)
        {
            u32 speed = GetMonData(&players[slot], MON_DATA_SPEED);
            EXPECT_GT(speed, GetMonData(&enemies[1], MON_DATA_SPEED));
            EXPECT_LT(speed * 2 / 3, GetMonData(&enemies[1], MON_DATA_SPEED));
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, line == 0 ? MOVE_FAKE_OUT : MOVE_ELECTROWEB,
                target: playerRight, hit: TRUE, criticalHit: FALSE);
            if (line == 0)
                MOVE(opponentRight, MOVE_PROTECT);
            else if (line == 1)
                MOVE(opponentRight, MOVE_SPORE, target: playerRight, hit: TRUE);
            else
                MOVE(opponentRight, MOVE_REFLECT);
        }
        // The same native player builds and attacks are used in every line.
        // No custom damage is injected; native rolls are shared by the harness.
    } SCENE {
        if (line != 0)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTROWEB, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, line == 1 ? MOVE_SPORE : MOVE_REFLECT, opponentRight);
            // The actual Timburr builds tie Speed. Either can execute first;
            // capture chronological damage without prescribing the tie order.
            HP_BAR(opponentRight, captureDamage: &results[i].firstPunch);
            if (line == 2)
            {
                HP_BAR(opponentRight, captureDamage: &results[i].secondPunch);
            }
        }
    } THEN {
        Test_MgbaPrintf("Allen control line=%d LAST=%S/%S playerLAST=%S/%S nativePunch=%d/%d enemyHP=%d/%d foeHP=%d/%d sleep=%d/%d speedStage=%d/%d Reflect=%d ParasStatus=%d",
            line, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            results[i].firstPunch, results[i].secondPunch, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT) != 0, opponentRight->status1);
        if (line == 0)
        {
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        }
        else
        {
            EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
            if (line == 1)
            {
                EXPECT(playerRight->status1 & STATUS1_SLEEP);
                EXPECT_NE(gLastMoves[B_BATTLER_2], MOVE_ICE_PUNCH);
            }
            else
                EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
        }
        // This proves only the fixed-line native consequences, not that a
        // one-turn HP comparison determines the best multi-turn strategy.
    } FINALLY {
        EXPECT_LT(results[2].firstPunch, results[1].firstPunch);
        EXPECT_GT(results[2].secondPunch, 0);
        EXPECT_EQ(results[1].secondPunch, 0);
    }
}
