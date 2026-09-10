// Temporary Thread safety diagnostics; include after rick_ai_probe.inc.
#line 13000 "work/rick_thread_controls.inc"

static void RickThreadControlPlayers(u32 control)
{
    static const u8 pachiPoints[NUM_STATS] = {252, 0, 160, 0, 0, 96};
    static const u8 dogPoints[NUM_STATS] = {88, 0, 0, 252, 0, 168};
    static const u8 birdPoints[NUM_STATS] = {104, 0, 0, 252, 0, 152};
    static const u16 pachiMoves[MAX_MON_MOVES] = {MOVE_THUNDERBOLT, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT};
    static const u16 dogMoves[MAX_MON_MOVES] = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SUBSTITUTE, MOVE_PROTECT};
    static const u16 snakeMoves[MAX_MON_MOVES] = {MOVE_LEAF_STORM, MOVE_GIGA_DRAIN, MOVE_GLARE, MOVE_PROTECT};
    static const u16 birdMoves[MAX_MON_MOVES] = {MOVE_PSYCHIC, MOVE_REFLECT, MOVE_ROOST, MOVE_PROTECT};
    RickProbePlayer(SPECIES_PACHIRISU, NATURE_CALM, ABILITY_VOLT_ABSORB,
        ITEM_SITRUS_BERRY, pachiPoints, pachiMoves);
    if (control < 2)
    {
        RickProbePlayer(SPECIES_ELECTRIKE, NATURE_MODEST, ABILITY_LIGHTNING_ROD,
            control == 1 ? ITEM_WHITE_HERB : ITEM_EVIOLITE, dogPoints, dogMoves);
        if (control == 1)
        {
            u16 hp = 20;
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1];
            EXPECT_GT(GetMonData(mon, MON_DATA_MAX_HP), hp);
            SetMonData(mon, MON_DATA_HP, &hp);
        }
    }
    else if (control == 2)
        RickProbePlayer(SPECIES_SNIVY, NATURE_MODEST, ABILITY_CONTRARY,
            ITEM_EVIOLITE, dogPoints, snakeMoves);
    else
        RickProbePlayer(SPECIES_NATU, NATURE_MODEST, ABILITY_MAGIC_BOUNCE,
            ITEM_EVIOLITE, birdPoints, birdMoves);
    // Pachirisu/Natu are current Route102, Electrike Route103; Snivy is a
    // regional starter. Beldum is deliberately not claimed as an early catch.
}

DOUBLE_BATTLE_TEST("EC live review Rick Thread controls: native Substitute Herb Contrary and Bounce")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    PARAMETRIZE { control = 3; }
    GIVEN {
        RickThreadControlPlayers(control);
        AuthoredOpponent(TRAINER_RICK, 0, FALSE);
    } WHEN {
        if (control == 0)
        {
            // Establish a real native Substitute, with both incidental
            // opponents' targets protected; no volatile or derived HP edits.
            TURN {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_SUBSTITUTE);
                MOVE(opponentLeft, MOVE_TOXIC_THREAD, target: playerLeft);
                MOVE(opponentRight, MOVE_PROTECT);
            }
        }
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, control < 2 ? MOVE_FLAMETHROWER : control == 2 ? MOVE_GIGA_DRAIN : MOVE_PSYCHIC,
                target: opponentRight);
            MOVE(opponentLeft, MOVE_TOXIC_THREAD, target: playerRight, hit: TRUE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("Rick Thread native control=%d target=%d HP=%d speedStage=%d status=%d item=%d spinSpeedStage=%d spinStatus=%d substitute=%d",
            control, playerRight->species, playerRight->hp, playerRight->statStages[STAT_SPEED],
            playerRight->status1, playerRight->item, opponentLeft->statStages[STAT_SPEED],
            opponentLeft->status1, playerRight->volatiles.substitute);
        if (control == 0)
        {
            EXPECT(playerRight->volatiles.substitute);
            EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
            EXPECT_EQ(playerRight->status1, 0);
        }
        else if (control == 1)
        {
            EXPECT_EQ(playerRight->item, ITEM_NONE);
            EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
            EXPECT(playerRight->status1 & STATUS1_POISON);
        }
        else if (control == 2)
        {
            EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
            EXPECT(playerRight->status1 & STATUS1_POISON);
        }
        else
        {
            EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
            EXPECT_EQ(playerRight->status1, 0);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
            // Reflected Thread cannot poison Poison-type Spinarak, but its
            // Speed effect still applies. Partial failure is not full failure.
            EXPECT_EQ(opponentLeft->status1, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rick Thread controls: actual AI Herb Contrary and Bounce")
{
    u32 control;
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    PARAMETRIZE { control = 3; }
    GIVEN {
        RickThreadControlPlayers(control);
        AuthoredOpponent(TRAINER_RICK, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft);
            MOVE(playerRight, control == 1 ? MOVE_FLAMETHROWER : control == 2 ? MOVE_GIGA_DRAIN : MOVE_PSYCHIC,
                target: opponentRight);
            // Do not prescribe Thread or prohibit it globally: the other
            // target can be valid, and poisoning may have independent value.
        }
    } THEN {
        Test_MgbaPrintf("Rick Thread AI control=%d LAST=%S/%S targetSpecies=%d HP=%d speedStage=%d status=%d item=%d spinSpeedStage=%d",
            control, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            playerRight->species, playerRight->hp, playerRight->statStages[STAT_SPEED],
            playerRight->status1, playerRight->item, opponentLeft->statStages[STAT_SPEED]);
        // Native decision-time traces establish chosen targets. A diagnostic
        // PASS does not assert that the AI picked an optimal action.
    }
}
