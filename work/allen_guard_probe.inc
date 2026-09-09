// Temporary candidate-only native comparison. Include after allen_ai_probe.inc.
// No canonical/generated party or shared helper is changed.
#line 17000 "work/allen_guard_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Allen Guard: spread protection and single-target counter", u16 pikachuHp, u16 parasHp)
{
    bool32 wide;
    bool32 psychic;
    PARAMETRIZE { wide = FALSE; psychic = FALSE; }
    PARAMETRIZE { wide = TRUE; psychic = FALSE; }
    PARAMETRIZE { wide = TRUE; psychic = TRUE; }
    GIVEN {
        AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
        AllenProbePlayer(SPECIES_NATU, &sAllenProbeHeatWaveNatu);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *paras = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1];
        EXPECT_EQ(GetMonData(paras, MON_DATA_SPECIES), SPECIES_PARAS);
        EXPECT_EQ(GetMonData(paras, MON_DATA_MOVE2), MOVE_WIDE_GUARD);
        if (!wide)
        {
            enum Move move = MOVE_REFLECT;
            u32 pp = GetMovePP(move);
            // Restore the historical opening's unused Reflect slot only for
            // its Protect control. Current Allen already has Wide Guard.
            // AuthoredOpponent already registered all four explicit moves.
            // CurrentMon/MoveGetIdAndSlot read this recorded Pokemon itself;
            // there is no separate move list requiring another annotation.
            SetMonData(paras, MON_DATA_MOVE2, &move);
            SetMonData(paras, MON_DATA_PP2, &pp);
            EXPECT_EQ(GetMonData(paras, MON_DATA_MOVE2), MOVE_REFLECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            if (psychic)
                MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerRight, MOVE_HEAT_WAVE, hit: TRUE, criticalHit: FALSE);
            // Suppress Volt Tackle's paralysis only in this comparison. Its
            // native damage/recoil remain; Natu must actually execute the
            // attack whose interaction with Guard we are investigating.
            MOVE(opponentLeft, MOVE_VOLT_TACKLE, target: playerRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, wide ? MOVE_WIDE_GUARD : MOVE_PROTECT);
            if (!wide)
                SEND_OUT(opponentLeft, 2);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VOLT_TACKLE, opponentLeft);
        if (!wide)
        {
            // Confirm the reason a replacement is required: actual Heat Wave
            // damage faints Pikachu, rather than accepting a queued send-out
            // as proof of a knockout or mistaking the wait for a game freeze.
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAT_WAVE, playerRight);
            HP_BAR(opponentLeft, hp: 0);
        }
        else if (psychic)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, playerRight);
    } THEN {
        if (wide)
        {
            EXPECT_EQ(opponentLeft->species, SPECIES_PIKACHU);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_VOLT_TACKLE);
        }
        else
        {
            EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
            EXPECT_EQ(opponentLeft->species, SPECIES_SLAKOTH);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), 0);
        }
        EXPECT_EQ(opponentRight->species, SPECIES_PARAS);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], wide ? MOVE_WIDE_GUARD : MOVE_PROTECT);
        EXPECT_EQ(gLastMoves[B_BATTLER_2], psychic ? MOVE_PSYCHIC : MOVE_HEAT_WAVE);
        results[i].pikachuHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP);
        results[i].parasHp = opponentRight->hp;
        if (!psychic)
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        else
            EXPECT(opponentRight->hp < opponentRight->maxHP);
        Test_MgbaPrintf("Allen Guard wide=%d psychic=%d activeLAST=%S/%S NatuLAST=%S originalPikaHP=%d ParasHP=%d playerHP=%d/%d status=%d/%d activeLeft=%d",
            wide, psychic, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_2]), results[i].pikachuHp, opponentRight->hp,
            playerLeft->hp, playerRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_STATUS), opponentRight->status1,
            opponentLeft->species);
        // Only Paras's Reflect slot differs in the candidate. Guard does not
        // stop single-target Super Fang or Psychic, nor Volt Tackle's recoil.
        // These forced outcomes are not claims about unrestricted AI choices.
    } FINALLY {
        // Compare executed attacks and actual surviving HP, not merely the
        // commands queued above. All three disable the same Volt secondary.
        EXPECT_GT(results[1].pikachuHp, results[0].pikachuHp);
        EXPECT_LT(results[2].parasHp, results[1].parasHp);
    }
}
