#line 57000 "work/joey_drum_probe.inc"

static void JoeyDrumBoard(bool32 berry)
{
    JoeyPlayer(SPECIES_PACHIRISU, &sJoeyPachirisu);
    JoeyPlayer(SPECIES_TIMBURR, &sJoeyTimburr);
    AuthoredOpponent(TRAINER_JOEY, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PAWMI);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_LECHONK);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
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

AI_DOUBLE_BATTLE_TEST("EC live review Joey berry: native recoil opens healing but Unnerve blocks consumption")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_CORVIKNIGHT) {
            Level(14); HP(500); MaxHP(500); Attack(10); Defense(40); SpAttack(30); SpDefense(30); Speed(40);
            Ability(control == 2 ? ABILITY_UNNERVE : ABILITY_PRESSURE); Item(ITEM_NONE); Moves(MOVE_ROOST);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(10); Defense(100); SpAttack(30); SpDefense(30); Speed(30);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Level(12); HP(200); MaxHP(200); Defense(100); SpDefense(100); Speed(10);
            Ability(ABILITY_TELEPATHY); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ZIGZAGOON) {
            Level(12); HP(19); MaxHP(36); Attack(50); Defense(30); SpAttack(20); SpDefense(30); Speed(60);
            Nature(NATURE_ADAMANT); Ability(ABILITY_GLUTTONY); Item(control ? ITEM_FIGY_BERRY : ITEM_NONE);
            Moves(MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROOST);
            MOVE(playerRight, MOVE_PROTECT);
            if (control == 1)
                EXPECT_MOVE(opponentRight, MOVE_DOUBLE_EDGE, target: playerLeft);
            else
                EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, control == 1 ? 28 : 19);
        EXPECT_EQ(opponentRight->item, control == 2 ? ITEM_FIGY_BERRY : ITEM_NONE);
        EXPECT_EQ((u32)opponentRight->volatiles.confusionTimer, 0);
        Test_MgbaPrintf("JOEY_BERRY_BOUNDARY control=%d move=%S HP=%d/%d foes=%d/%d stage=%d item=%d",
            control, GetMoveName(gLastMoves[B_BATTLER_3]), opponentRight->hp, opponentRight->maxHP,
            playerLeft->hp, playerRight->hp, opponentRight->statStages[STAT_ATK], opponentRight->item);
        // Explicit synthetic menu/stats: recoil can cross Gluttony's threshold
        // and make a supported berry attack heal net HP. Corviknight is a
        // generic mechanic boundary, not claimed early access. No forced AI.
        // Positive native choices and HP are observed. Root still requires a
        // distinct disabled-consumer failure before retaining this group.
    }
}
