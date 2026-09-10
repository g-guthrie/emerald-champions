// Item-only Taillow comparison; include after JohnsonPlayer.
#line 56000 "work/johnson_orb_probe.inc"

static void JohnsonOrbReserves(bool32 flame)
{
    AuthoredOpponent(TRAINER_JOHNSON, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SHINX);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_STARLY);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BUNNELBY);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TAILLOW);
    EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_GUTS);
    u16 item = flame ? ITEM_FLAME_ORB : ITEM_TOXIC_ORB;
    SetMonData(&party[3], MON_DATA_HELD_ITEM, &item);
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
    Test_MgbaPrintf("JOHNSON_ORB_INPUT flame=%d TaillowHP=%d Atk=%d Speed=%d item=%d BunnelbySlot2=%S friendship=%d",
        flame, GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_ATK),
        GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_HELD_ITEM),
        GetMoveName(GetMonData(&party[0], MON_DATA_MOVE3)), GetMonData(&party[0], MON_DATA_FRIENDSHIP));
}

DOUBLE_BATTLE_TEST("EC live review Johnson Orb: native Guts Facade and three turn residual comparison",
    s16 facadeDamage; u16 finalHp)
{
    bool32 flame;
    PARAMETRIZE { flame = FALSE; }
    PARAMETRIZE { flame = TRUE; }
    GIVEN {
        JohnsonPlayer(SPECIES_PACHIRISU, &sJohnsonPachirisu);
        JohnsonPlayer(SPECIES_TIMBURR, &sJohnsonTimburr);
        JohnsonOrbReserves(flame);
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        // Explicit synthetic passive player menus isolate native item/status
        // mechanics. Their generated stats and the entire trainer menus remain.
        for (u32 actor = 0; actor < 2; actor++)
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                u16 move = slot == 0 ? MOVE_CELEBRATE : MOVE_NONE;
                u8 pp = slot == 0 ? GetMovePP(move) : 0;
                SetMonData(&players[actor], MON_DATA_MOVE1 + slot, &move);
                SetMonData(&players[actor], MON_DATA_PP1 + slot, &pp);
            }
    } WHEN {
        for (u32 turn = 0; turn < 3; turn++) TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (turn == 1)
                MOVE(opponentRight, MOVE_FACADE, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FACADE, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].facadeDamage);
    } THEN {
        results[i].finalHp = opponentRight->hp;
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(opponentRight->status1 & (STATUS1_BURN | STATUS1_TOXIC_POISON),
            flame ? STATUS1_BURN : STATUS1_TOXIC_POISON);
        Test_MgbaPrintf("JOHNSON_ORB_NATIVE flame=%d FacadeDamage=%d TaillowHP=%d/%d totalLoss=%d status=%d item=%d turns=%d",
            flame, results[i].facadeDamage, opponentRight->hp, opponentRight->maxHP,
            opponentRight->maxHP - opponentRight->hp, opponentRight->status1,
            opponentRight->item, gBattleResults.battleTurnCounter);
    } FINALLY {
        EXPECT_GT(results[0].facadeDamage, 0);
        EXPECT_EQ(results[0].facadeDamage, results[1].facadeDamage);
        EXPECT_GT(results[1].finalHp, results[0].finalHp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Johnson Orb: unrestricted special and Fake Out two turn followthrough")
{
    bool32 flame;
    u32 board;
    PARAMETRIZE { board = 0; flame = FALSE; }
    PARAMETRIZE { board = 0; flame = TRUE; }
    PARAMETRIZE { board = 1; flame = FALSE; }
    PARAMETRIZE { board = 1; flame = TRUE; }
    GIVEN {
        JohnsonPlayer(board == 0 ? SPECIES_PACHIRISU : SPECIES_MIENFOO,
            board == 0 ? &sJohnsonPachirisu : &sJohnsonMienfoo);
        JohnsonPlayer(board == 0 ? SPECIES_LOTAD : SPECIES_PACHIRISU,
            board == 0 ? &sJohnsonLotad : &sJohnsonPachirisu);
        JohnsonOrbReserves(flame);
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JOHNSON_ORB_AI board=%d flame=%d moves=%S/%S ownHP=%d/%d foes=%d/%d status=%d/%d item=%d/%d faint=%d/%d",
            board, flame, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentLeft->status1, opponentRight->status1, opponentLeft->item, opponentRight->item,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Same native party and opponents; only Taillow's item differs.
        // Unrestricted observations do not certify terminal residual scoring.
    }
}
