// Temporary forced native counterfactual. Include after rick_ai_probe.inc;
// calls its actual level-14 IV31 preparation helper and AuthoredOpponent.
#line 12000 "work/rick_thread_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Rick Thread: absorber board attack versus speed control")
{
    u32 line;
    bool32 injured;
    PARAMETRIZE { line = 0; injured = FALSE; }
    PARAMETRIZE { line = 1; injured = FALSE; }
    PARAMETRIZE { line = 2; injured = FALSE; }
    PARAMETRIZE { line = 0; injured = TRUE; }
    PARAMETRIZE { line = 1; injured = TRUE; }
    PARAMETRIZE { line = 2; injured = TRUE; }
    GIVEN {
        // Identical species, native preparation, and original four moves to
        // control2 in rick_ai_probe.inc. Only the optional injury differs.
        static const u8 squirrelPoints[NUM_STATS] = {32, 0, 21, 0, 0, 13};
        static const u8 dogPoints[NUM_STATS] = {12, 0, 0, 32, 0, 22};
        static const u16 squirrelMoves[MAX_MON_MOVES] = {MOVE_THUNDERBOLT, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT};
        static const u16 dogMoves[MAX_MON_MOVES] = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT};
        RickProbePlayer(SPECIES_PACHIRISU, NATURE_CALM, ABILITY_VOLT_ABSORB,
            ITEM_SITRUS_BERRY, squirrelPoints, squirrelMoves);
        RickProbePlayer(SPECIES_ELECTRIKE, NATURE_MODEST, ABILITY_LIGHTNING_ROD,
            ITEM_EVIOLITE, dogPoints, dogMoves);
        if (injured)
        {
            u16 hp = 20;
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1];
            EXPECT(GetMonData(mon, MON_DATA_MAX_HP) > hp);
            SetMonData(mon, MON_DATA_HP, &hp);
        }
        AuthoredOpponent(TRAINER_RICK, 0, FALSE);
        struct Pokemon *enemy = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        u32 spinSpeed = GetMonData(&enemy[0], MON_DATA_SPEED);
        u32 rockSpeed = GetMonData(&enemy[1], MON_DATA_SPEED);
        u32 dogSpeed = GetMonData(&players[1], MON_DATA_SPEED);
        EXPECT(spinSpeed > dogSpeed);
        EXPECT(dogSpeed > rockSpeed);
        EXPECT(dogSpeed / 2 < rockSpeed);
        Test_MgbaPrintf("Rick Thread input line=%d injured=%d speeds=%d/%d versus%d/%d dogHP=%d/%d",
            line, injured, spinSpeed, rockSpeed,
            GetMonData(&players[0], MON_DATA_SPEED), dogSpeed,
            GetMonData(&players[1], MON_DATA_HP), GetMonData(&players[1], MON_DATA_MAX_HP));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, line == 0 ? MOVE_POISON_JAB : MOVE_TOXIC_THREAD,
                target: playerRight, hit: TRUE, criticalHit: FALSE);
            if (line == 2)
                MOVE(opponentRight, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentRight, MOVE_X_SCISSOR, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 0);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 1);
        EXPECT_EQ(opponentLeft->species, SPECIES_SPINARAK);
        EXPECT_EQ(opponentRight->species, SPECIES_DWEBBLE);
        Test_MgbaPrintf("Rick Thread result line=%d injured=%d LAST=%S/%S dogLAST=%S HP=%d/%d versus%d/%d",
            line, injured, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_2]), opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp);
        Test_MgbaPrintf("Rick Thread dogStatus=%d dogSpeedStage=%d dogSpAStage=%d rockStatus=%d dogBaseSpeed=%d rockBaseSpeed=%d",
            playerRight->status1, playerRight->statStages[STAT_SPEED], playerRight->statStages[STAT_SPATK],
            opponentRight->status1, playerRight->speed, opponentRight->speed);
        // A slower Electrike can receive its partner's redirected Thunderbolt
        // boost before attacking. Speed control can therefore increase incoming
        // damage unless the earlier attack actually removes or stops Electrike.
        // Accuracy and critical hits are controlled; secondary effects and
        // native damage remain their actual sampled outcomes. A Rock Slide
        // flinch in one result is not guaranteed flinch or an AI-optimality proof.
    }
}
