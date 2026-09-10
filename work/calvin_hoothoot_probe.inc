// Temporary actual-position diagnostic. Include after calvin_ai_probe.inc;
// uses its native CalvinProbePlayer helper and legal bulky Lotad set.
#line 9000 "work/calvin_hoothoot_probe.inc"

static const struct EmeraldChampionsBattleSet sCalvinHootPhysicalPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sCalvinHootFastPachirisu = {
    .moves = {MOVE_SUPER_FANG, MOVE_HELPING_HAND, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_TIMID, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 0, 4, 0, 252},
};
static const struct EmeraldChampionsBattleSet sCalvinHootFastNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_REFLECT, MOVE_ROOST, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {252, 0, 0, 4, 0, 252},
};

AI_DOUBLE_BATTLE_TEST("EC live review Calvin Hoothoot: physical bulk and future speed opportunity")
{
    bool32 fast;
    u32 turns;
    PARAMETRIZE { fast = FALSE; turns = 1; }
    PARAMETRIZE { fast = TRUE; turns = 1; }
    PARAMETRIZE { fast = TRUE; turns = 2; }
    GIVEN {
        if (fast)
        {
            // Fast support builds with native neutral Psychic and percentage
            // damage. Night Shade is NOT a valid threat to these Normal types.
            CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinHootFastPachirisu);
            CalvinProbePlayer(SPECIES_NATU, &sCalvinHootFastNatu);
        }
        else
        {
            CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinHootPhysicalPachirisu);
            CalvinProbePlayer(SPECIES_LOTAD, &sCalvinProbeLotad);
        }
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
        u16 zero = 0;
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP, &zero);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
        struct Pokemon *enemy = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        EXPECT_EQ(GetMonData(&enemy[2], MON_DATA_SPECIES), SPECIES_HOOTHOOT);
        EXPECT_EQ(GetMonAbility(&enemy[2]), ABILITY_TINTED_LENS);
        EXPECT_EQ(GetMonData(&enemy[3], MON_DATA_SPECIES), SPECIES_STARLY);
        for (u32 p = 0; p < 2; p++)
        {
            if (fast)
            {
                u32 starlySpeed = GetMonData(&enemy[3], MON_DATA_SPEED);
                u32 playerSpeed = GetMonData(&players[p], MON_DATA_SPEED);
                EXPECT(playerSpeed > starlySpeed);
                EXPECT(playerSpeed < 2 * starlySpeed);
            }
            else
                EXPECT(GetMonData(&players[p], MON_DATA_DEF) > GetMonData(&players[p], MON_DATA_SPDEF));
        }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                if (fast)
                {
                    MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                    MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft);
                }
                else
                {
                    MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft);
                    MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight);
                }
                // Observe actual AI decisions; Tailwind is not compulsory
                // merely because both players are faster. Protect or attacking
                // may be better, especially after first-turn damage. Continued
                // pressure can remove the setter before a slow Tailwind.
            }
        }
    } THEN {
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 3);
        Test_MgbaPrintf("Calvin Hoot fast=%d turns=%d executed=%S/%S HP=%d/%d playerHP=%d/%d Wind=%d speeds=%d/%d vs%d/%d",
            fast, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
            opponentLeft->speed, opponentRight->speed, playerLeft->speed, playerRight->speed);
        Test_MgbaPrintf("Calvin Hoot status=%d paralysis=%d StarlyStatus=%d completedTurns=%d",
            opponentLeft->status1, (opponentLeft->status1 & STATUS1_PARALYSIS) != 0,
            opponentRight->status1, gBattleResults.battleTurnCounter);
        Test_MgbaPrintf("Calvin Hoot native SpA=%d Atk=%d players Def=%d/%d SpD=%d/%d",
            opponentLeft->spAttack, opponentRight->attack,
            playerLeft->defense, playerRight->defense, playerLeft->spDefense, playerRight->spDefense);
        // Root's decision-time logger owns each turn's targets and choices.
        // With two turns, gLastMoves is only the most recently executed move;
        // it can remain from turn one if that battler never acts on turn two.
        // Paralysis status is evidence of paralysis, not by itself proof that
        // full paralysis caused a missing action (inspect native scene/events).
        // PASS certifies these positions ran, not that every observed choice
        // was optimal or that the entire Calvin encounter has been played.
    }
}
