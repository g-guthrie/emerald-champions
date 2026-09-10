#line 57000 "work/joey_drum_probe.inc"

static void JoeyDrumOpponent(bool32 berry)
{
    AuthoredOpponent(TRAINER_JOEY, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PAWMI);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_LECHONK);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CLEFAIRY);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_FIGY_BERRY);
    u16 zero = 0;
    SetMonData(&party[0], MON_DATA_HP, &zero);
    SetMonData(&party[1], MON_DATA_HP, &zero);
    if (!berry)
    {
        u16 none = ITEM_NONE;
        SetMonData(&party[3], MON_DATA_HELD_ITEM, &none);
    }
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
}

static void JoeyDrumBoard(bool32 berry)
{
    JoeyPlayer(SPECIES_PACHIRISU, &sJoeyPachirisu);
    JoeyPlayer(SPECIES_TIMBURR, &sJoeyTimburr);
    JoeyDrumOpponent(berry);
}

DOUBLE_BATTLE_TEST("EC live review Joey Drum: native Figy restores HP after actual Gluttony setup", s16 cost, s16 heal)
{
    bool32 berry;
    PARAMETRIZE { berry = FALSE; }
    PARAMETRIZE { berry = TRUE; }
    GIVEN { JoeyDrumBoard(berry); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_BELLY_DRUM);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, opponentRight);
        HP_BAR(opponentRight, captureDamage: &results[i].cost);
        if (berry)
            HP_BAR(opponentRight, captureDamage: &results[i].heal);
    } THEN {
        EXPECT_EQ(results[i].cost, opponentRight->maxHP / 2);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], MAX_STAT_STAGE);
        EXPECT_EQ((u32)opponentRight->volatiles.confusionTimer, 0);
        if (berry)
        {
            EXPECT_EQ(results[i].heal, -(s32)(opponentRight->maxHP / GetItemHoldEffectParam(ITEM_FIGY_BERRY)));
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
        }
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP - results[i].cost - results[i].heal);
        Test_MgbaPrintf("JOEY_DRUM_NATIVE berry=%d maxHP=%d cost=%d heal=%d HP=%d speed=%d ability=%d",
            berry, opponentRight->maxHP, results[i].cost, results[i].heal, opponentRight->hp,
            opponentRight->speed, opponentRight->ability);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Joey Drum: actual full reserve menus under setup opportunity and pressure")
{
    bool32 pressure;
    PARAMETRIZE { pressure = FALSE; }
    PARAMETRIZE { pressure = TRUE; }
    GIVEN { JoeyDrumBoard(TRUE); }
    WHEN {
        TURN {
            if (pressure)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_BULK_UP);
            }
        }
    } THEN {
        Test_MgbaPrintf("JOEY_DRUM_AI pressure=%d moves=%S/%S HP=%d/%d foes=%d/%d item=%d stage=%d faints=%d/%d",
            pressure, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentRight->item, opponentRight->statStages[STAT_ATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
