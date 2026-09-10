// Temporary controls and an isolated generic decision boundary.
#line 58000 "work/hector_paralysis_probe.inc"

static void HectorParalysisReserves(void)
{
    AuthoredOpponent(TRAINER_HECTOR, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_SEVIPER);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DUNSPARCE);
    u16 zero = 0;
    SetMonData(&party[0], MON_DATA_HP, &zero);
    SetMonData(&party[1], MON_DATA_HP, &zero);
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
}

DOUBLE_BATTLE_TEST("EC live review Hector paralysis: Glare affects Ground but not Electric")
{
    bool32 electric;
    PARAMETRIZE { electric = FALSE; }
    PARAMETRIZE { electric = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet target = {
            .moves = {MOVE_WATER_GUN, MOVE_PROTECT}, .item = ITEM_NONE,
            .nature = NATURE_BOLD, .ability = ABILITY_WATER_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        if (electric)
        {
            target = sHectorPachirisu;
            target.item = ITEM_NONE;
        }
        HectorPlayer(electric ? SPECIES_PACHIRISU : SPECIES_WOOPER, &target);
        HectorPlayer(SPECIES_PACHIRISU, &sHectorPachirisu);
        HectorParalysisReserves();
    } WHEN {
        TURN {
            if (electric)
                MOVE(playerLeft, MOVE_FOLLOW_ME);
            else
                MOVE(playerLeft, MOVE_WATER_GUN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_GLARE, target: playerLeft, hit: TRUE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(playerLeft->status1 & STATUS1_PARALYSIS, electric ? 0 : STATUS1_PARALYSIS);
        Test_MgbaPrintf("HECTOR_GLARE_NATIVE electric=%d status=%d HP=%d", electric, playerLeft->status1, playerLeft->hp);
    }
}

DOUBLE_BATTLE_TEST("EC live review Hector paralysis: Serene Grace Body Slam respects Covert Cloak")
{
    bool32 cloak;
    PARAMETRIZE { cloak = FALSE; }
    PARAMETRIZE { cloak = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet target = {
            .moves = {MOVE_BODY_SLAM, MOVE_PROTECT}, .item = cloak ? ITEM_COVERT_CLOAK : ITEM_NONE,
            .nature = NATURE_BOLD, .ability = ABILITY_THICK_FAT,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        HectorPlayer(SPECIES_MUNCHLAX, &target);
        HectorPlayer(SPECIES_PACHIRISU, &sHectorPachirisu);
        HectorParalysisReserves();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_GLARE, target: playerRight);
            MOVE(opponentRight, MOVE_BODY_SLAM, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: TRUE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->status1 & STATUS1_PARALYSIS, cloak ? 0 : STATUS1_PARALYSIS);
        EXPECT_EQ(CalcSecondaryEffectChance(B_BATTLER_3, ABILITY_SERENE_GRACE, GetMoveAdditionalEffectById(MOVE_BODY_SLAM, 0)), 60);
        EXPECT_EQ(CalcSecondaryEffectChance(B_BATTLER_3, ABILITY_NONE, GetMoveAdditionalEffectById(MOVE_BODY_SLAM, 0)), 30);
        Test_MgbaPrintf("HECTOR_SLAM_NATIVE cloak=%d status=%d HP=%d", cloak, playerLeft->status1, playerLeft->hp);
    }
}
