// Temporary native Cotton Guard timing; include after WinstonReservePlayer.
#line 44000 "work/winston_cotton_probe.inc"

static void WinstonCottonBoard(u16 injury)
{
    static const struct EmeraldChampionsBattleSet munchlax = {
        .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
        .statPoints = {32, 32, 2, 0, 0, 0},
    };
    static const struct EmeraldChampionsBattleSet pachirisu = {
        .moves = {MOVE_ROUND, MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_PROTECT},
        .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
        .statPoints = {32, 0, 32, 0, 2, 0},
    };
    WinstonReservePlayer(SPECIES_MUNCHLAX, &munchlax);
    WinstonReservePlayer(SPECIES_PACHIRISU, &pachirisu);
    AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
    SetMonData(&party[3], MON_DATA_HP, &injury);
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED),
        GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("WINSTON_COTTON_INPUT owner=%d slot=%d species=%d HP=%d/%d Atk=%d Def=%d Speed=%d ability=%d item=%d",
                owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                GetMonData(mon, MON_DATA_SPEED), GetMonAbility(mon), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
    // Full genuine team and move menus retained, Room absent. Only reserve
    // order and explicit injury change; both original leads remain available.
}

DOUBLE_BATTLE_TEST("EC live review Winston Cotton Guard: native mitigation before slower Body Slam", s16 recoil; s16 slam)
{
    bool32 cotton;
    PARAMETRIZE { cotton = FALSE; }
    PARAMETRIZE { cotton = TRUE; }
    GIVEN {
        WinstonCottonBoard(50);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (cotton)
                MOVE(opponentRight, MOVE_COTTON_GUARD);
            else
                MOVE(opponentRight, MOVE_DOUBLE_EDGE, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, cotton ? MOVE_COTTON_GUARD : MOVE_DOUBLE_EDGE, opponentRight);
        if (!cotton)
            HP_BAR(opponentRight, captureDamage: &results[i].recoil);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BODY_SLAM, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].slam);
    } THEN {
        u32 woolooHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP);
        EXPECT_GT(results[i].slam, 0);
        EXPECT_GT(woolooHp, 0);
        EXPECT_LT((cotton ? 0 : results[i].recoil) + results[i].slam, 50);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (cotton ? 3 : 0));
        EXPECT_EQ(woolooHp, 50 - (cotton ? 0 : results[i].recoil) - results[i].slam
            + max(1, opponentRight->maxHP / 16));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_STATUS) & STATUS1_PARALYSIS, 0);
        Test_MgbaPrintf("WINSTON_COTTON_NATIVE cotton=%d recoil=%d uncappedSlam=%d originalWoolooHP=%d currentSpecies=%d currentHP=%d MunchHP=%d Room=%d",
            cotton, cotton ? 0 : results[i].recoil, results[i].slam, woolooHp,
            opponentRight->species, opponentRight->hp, playerLeft->hp,
            (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) != 0);
        // Both lines remain alive, so the incoming Slam is not HP-capped.
        // This isolates same-turn mitigation, not a full-battle win.
    } FINALLY {
        EXPECT_LT(results[1].slam, results[0].slam);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Winston Cotton Guard: unrestricted injured reserve decision")
{
    GIVEN {
        WinstonCottonBoard(20);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("WINSTON_COTTON_AI species=%d/%d ownHP=%d/%d originalWoolooHP=%d foes=%d/%d LAST=%S/%S currentParty=%d/%d ownFaints=%d",
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3], gBattleResults.opponentFaintCounter);
        // Switching, protection and partner assistance remain valid choices;
        // no Cotton Guard command or preferred survival result is prescribed.
    }
}
