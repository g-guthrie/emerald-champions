// Temporary exact native Helmet counterfactual; include after Billy helpers.
#line 37000 "work/billy_contact_controls.inc"

static void BillyContactBoard(void)
{
    struct EmeraldChampionsBattleSet pachirisu = sBillyReservePachirisu;
    pachirisu.item = ITEM_ROCKY_HELMET;
    BillyReservePlayer(SPECIES_PACHIRISU, &pachirisu);
    BillyReservePlayer(SPECIES_TIMBURR, &sBillyReserveTimburr);
    AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_DITTO);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
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
    // Exact reserve records and native player stats. Ditto copies diagonal
    // Timburr, whose real slot 1 is Drain Punch. Specify that copied slot in
    // WHEN, without inserting fake attacks into Ditto's authored party record.
}

DOUBLE_BATTLE_TEST("EC live review Billy contact: native third-turn Bomb Fang or Protect after identical opening", s16 hitDamage[3], s16 helmetDamage[3], u16 playerHp, u16 tandemHp)
{
    u32 choice;
    PARAMETRIZE { choice = 0; }
    PARAMETRIZE { choice = 1; }
    PARAMETRIZE { choice = 2; }
    GIVEN {
        BillyContactBoard();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, moveSlot: 1, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, moveSlot: 1, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_SUPER_FANG, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, moveSlot: 1, target: playerRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (choice == 0)
                MOVE(opponentRight, MOVE_POPULATION_BOMB, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (choice == 1)
                MOVE(opponentRight, MOVE_SUPER_FANG, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, opponentRight);
        if (choice == 0)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_POPULATION_BOMB, opponentRight);
            for (u32 hit = 0; hit < 3; hit++)
            {
                HP_BAR(playerLeft, captureDamage: &results[i].hitDamage[hit]);
                HP_BAR(opponentRight, captureDamage: &results[i].helmetDamage[hit]);
            }
        }
        else
            ANIMATION(ANIM_TYPE_MOVE, choice == 1 ? MOVE_SUPER_FANG : MOVE_PROTECT, opponentRight);
    } THEN {
        results[i].playerHp = playerLeft->hp;
        results[i].tandemHp = GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP);
        Test_MgbaPrintf("BILLY_CONTACT choice=%d ownHP=%d/%d PachiHP=%d TimHP=%d TandPartyHP=%d LAST=%S/%S status=%d/%d",
            choice, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp, results[i].tandemHp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]), opponentLeft->status1, opponentRight->status1);
        if (choice == 0)
        {
            u32 hits = 0, retaliation = 0;
            for (u32 hit = 0; hit < 3; hit++)
            {
                EXPECT_GT(results[i].hitDamage[hit], 0);
                EXPECT_GT(results[i].helmetDamage[hit], 0);
                hits += results[i].hitDamage[hit];
                retaliation += results[i].helmetDamage[hit];
                Test_MgbaPrintf("BILLY_CONTACT_HIT hit=%d damage=%d helmet=%d", hit + 1,
                    results[i].hitDamage[hit], results[i].helmetDamage[hit]);
            }
            EXPECT_EQ(results[i].tandemHp, 0);
            Test_MgbaPrintf("BILLY_CONTACT_SUM threeHits=%d retaliation=%d", hits, retaliation);
        }
        // This fixed hit-success branch isolates contact-chain termination.
        // The current DSL offers one hit boolean per action, not a per-hit
        // sequence; it cannot honestly certify a third-hit accuracy miss.
        // Compare native outcomes before claiming which line is preferable.
    }
}
