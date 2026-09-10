// Temporary native recovery timing comparison; include after Cindy helpers.
#line 50000 "work/cindy_recover_probe.inc"

static void CindyRecoverBoard(void)
{
    static const struct EmeraldChampionsBattleSet munchlax = {
        .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
        .evs = {252, 252, 4, 0, 0, 0},
    };
    static const struct EmeraldChampionsBattleSet pachirisu = {
        .moves = {MOVE_ROUND, MOVE_THUNDERBOLT, MOVE_PROTECT, MOVE_SUPER_FANG},
        .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
        .evs = {252, 0, 252, 0, 4, 0},
    };
    CindyReservePlayer(SPECIES_MUNCHLAX, &munchlax);
    CindyReservePlayer(SPECIES_PACHIRISU, &pachirisu);
    AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SNUBBULL);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MILCERY);
    u16 hp = 20;
    EXPECT_GT(GetMonData(&party[1], MON_DATA_MAX_HP), hp);
    SetMonData(&party[1], MON_DATA_HP, &hp);
    EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED),
        GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("CINDY_RECOVER_INPUT owner=%d slot=%d species=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP),
                GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
        }
    // Current Route116 Munchlax is a pre-badge native mechanic matchup, not a
    // claim it is caught on Route101. All original Cindy reserves stay alive.
    // Injury is explicit; every derived stat and Intimidate activation is native.
}

AI_DOUBLE_BATTLE_TEST("EC live review Cindy recovery: actual injured Milcery decision before slower Body Slam")
{
    bool32 reservesAlive;
    PARAMETRIZE { reservesAlive = TRUE; }
    PARAMETRIZE { reservesAlive = FALSE; }
    GIVEN {
        CindyRecoverBoard();
        if (!reservesAlive)
        {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            u16 hp = 0;
            EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_FURFROU);
            EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SLAKOTH);
            SetMonData(&party[2], MON_DATA_HP, &hp);
            SetMonData(&party[3], MON_DATA_HP, &hp);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("CINDY_RECOVER_AI reservesAlive=%d LAST=%S/%S liveSpecies=%d/%d HP=%d/%d party=%d/%d MilceryPartyHP=%d MunchAtkStage=%d",
            reservesAlive, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), playerLeft->statStages[STAT_ATK]);
        // A voluntary switch or partner-assisted line is a valid AI choice.
    }
}

DOUBLE_BATTLE_TEST("Historical flat-stat Cindy: forced recovery damage comparison", s16 heal, s16 slam, u16 milceryHp)
{
    bool32 recover;
    PARAMETRIZE { recover = FALSE; }
    PARAMETRIZE { recover = TRUE; }
    GIVEN {
        CindyRecoverBoard();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (recover)
                MOVE(opponentRight, MOVE_RECOVER);
            else
            {
                MOVE(opponentRight, MOVE_DRAINING_KISS, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // Explicit native hypothesis: insufficient draining lets the
                // slower Slam KO Milcery. SCENE/THEN below must confirm it.
                SEND_OUT(opponentRight, 2);
            }
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, recover ? MOVE_RECOVER : MOVE_DRAINING_KISS, opponentRight);
        HP_BAR(opponentRight, captureDamage: &results[i].heal);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BODY_SLAM, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].slam);
    } THEN {
        results[i].milceryHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP);
        EXPECT_LT(results[i].heal, 0);
        EXPECT_GT(results[i].slam, 0);
        if (!recover)
            EXPECT_EQ(results[i].milceryHp, 0);
        Test_MgbaPrintf("CINDY_RECOVER_FIXED recover=%d heal=%d slam=%d originalMilceryHP=%d liveSpecies=%d liveHP=%d MunchHP=%d MunchAtkStage=%d",
            recover, results[i].heal, results[i].slam, results[i].milceryHp,
            opponentRight->species, opponentRight->hp, playerLeft->hp, playerLeft->statStages[STAT_ATK]);
        // The fixed line isolates healing before a real hit; it does not force
        // the unrestricted AI to Recover or claim a whole-battle win.
    }
}
