// Temporary native comparison of the actual authored reserve's Normal move.
#line 19000 "work/allen_frustration_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Allen Frustration: native zero friendship and direct damage", s16 damage)
{
    bool32 historical;
    PARAMETRIZE { historical = TRUE; }
    PARAMETRIZE { historical = FALSE; }
    GIVEN {
        AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
        AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_FRIENDSHIP), 0);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE2), MOVE_FRUSTRATION);
        if (historical)
        {
            u16 move = MOVE_STRENGTH;
            u8 pp = GetMovePP(move);
            SetMonData(&party[3], MON_DATA_MOVE2, &move);
            SetMonData(&party[3], MON_DATA_PP2, &pp);
        }
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_BODY_SLAM, target: playerLeft);
            MOVE(opponentRight, historical ? MOVE_STRENGTH : MOVE_FRUSTRATION, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, historical ? MOVE_STRENGTH : MOVE_FRUSTRATION, opponentRight);
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_BUNNELBY);
        EXPECT_EQ(opponentRight->friendship, 0);
        Test_MgbaPrintf("Allen Normal historical=%d move=%S friendship=%d damage=%d playerHP=%d/%d enemyHP=%d/%d",
            historical, GetMoveName(gLastMoves[B_BATTLER_3]), opponentRight->friendship,
            results[i].damage, playerLeft->hp, playerRight->hp, opponentLeft->hp, opponentRight->hp);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}
