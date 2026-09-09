// Temporary forced counterfactuals. Include after tiana_reserve_probe.inc.
// Reproduce its actual generated reserve board and observed first-turn line.
#line 25000 "work/tiana_reserve_controls.inc"

static void TianaReserveControlBoard(void)
{
    TianaReservePlayer(SPECIES_TIMBURR, &sTianaReserveTimburr);
    TianaReservePlayer(SPECIES_PACHIRISU, &sTianaReservePachirisu);
    AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_EEVEE);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_BUNEARY);
    EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_KLUTZ);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_FLAME_ORB);
    // Match the baseline's defeated leads only. No active HP, ability,
    // generated stat, move, item, or prior-action history is overridden.
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

DOUBLE_BATTLE_TEST("EC live review Tiana control: Switcheroo now versus Drain Punch and later burn", s16 secondPunch, s16 thirdPunch)
{
    bool32 trade;
    PARAMETRIZE { trade = FALSE; }
    PARAMETRIZE { trade = TRUE; }
    GIVEN {
        TianaReserveControlBoard();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_FAKE_OUT, target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerRight, hit: TRUE, criticalHit: FALSE);
            if (trade)
                MOVE(opponentRight, MOVE_SWITCHEROO, target: playerLeft);
            else
                MOVE(opponentRight, MOVE_DRAIN_PUNCH, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
        TURN {
            // Identical legal measurement continuation: Wish allows Eevee to
            // take a further punch; Drain Punch and Super Fang still execute.
            // Wish is forced here, NOT a prediction of the AI's third turn.
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(opponentLeft, MOVE_WISH);
            MOVE(opponentRight, MOVE_DRAIN_PUNCH, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponentRight);
        if (trade)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWITCHEROO, opponentRight);
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_PUNCH, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].secondPunch);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_PUNCH, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].thirdPunch);
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_EEVEE);
        EXPECT_EQ(opponentRight->species, SPECIES_BUNEARY);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ICE_PUNCH);
        EXPECT_GT(results[i].secondPunch, 0);
        EXPECT_GT(results[i].thirdPunch, 0);
        if (trade)
        {
            EXPECT_EQ(playerLeft->item, ITEM_FLAME_ORB);
            EXPECT(playerLeft->status1 & STATUS1_BURN);
            EXPECT_EQ(opponentRight->item, ITEM_EVIOLITE);
        }
        else
        {
            EXPECT_EQ(playerLeft->item, ITEM_EVIOLITE);
            EXPECT_EQ(playerLeft->status1 & STATUS1_BURN, 0);
            EXPECT_EQ(opponentRight->item, ITEM_FLAME_ORB);
        }
        EXPECT_EQ(opponentRight->status1 & STATUS1_BURN, 0);
        Test_MgbaPrintf("TIANA_SWITCH_CONTROL trade=%d T2punch=%d T3punch=%d ownHP=%d/%d foeHP=%d/%d items=%d/%d statuses=%d/%d LAST=%S/%S",
            trade, results[i].secondPunch, results[i].thirdPunch,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentRight->item, playerLeft->item, opponentRight->status1, playerLeft->status1,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]));
        // Item loss and foregoing Drain Punch are real costs. Reduced later
        // physical damage alone does not establish the globally best line.
    } FINALLY {
        EXPECT_LT(results[1].thirdPunch, results[0].thirdPunch);
    }
}

DOUBLE_BATTLE_TEST("EC live review Tiana control: genuine Protect into fast Encore versus Drain Punch", s16 punch, u16 eeveeHp, u16 bunearyHp, u16 pachirisuHp)
{
    bool32 encore;
    PARAMETRIZE { encore = FALSE; }
    PARAMETRIZE { encore = TRUE; }
    GIVEN {
        TianaReserveControlBoard();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_FAKE_OUT, target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerRight, hit: TRUE, criticalHit: FALSE);
            if (encore)
                MOVE(opponentRight, MOVE_ENCORE, target: playerLeft);
            else
                MOVE(opponentRight, MOVE_DRAIN_PUNCH, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, playerLeft);
        if (encore)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ENCORE, opponentRight);
            NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_PUNCH, playerLeft); }
            // Consecutive Protect may fail. Denial of Ice Punch is the
            // measured effect; successful repeated protection is not assumed.
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, opponentRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_PUNCH, playerLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].punch);
        }
    } THEN {
        if (encore)
        {
            EXPECT_GT((u32)playerLeft->volatiles.encoreTimer, 0);
            EXPECT_EQ((u32)playerLeft->volatiles.encoredMove, MOVE_PROTECT);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_ENCORE);
        }
        else
        {
            EXPECT_GT(results[i].punch, 0);
            EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ICE_PUNCH);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_DRAIN_PUNCH);
        }
        results[i].eeveeHp = opponentLeft->hp;
        results[i].bunearyHp = opponentRight->hp;
        results[i].pachirisuHp = playerRight->hp;
        Test_MgbaPrintf("TIANA_ENCORE_CONTROL encore=%d punch=%d ownHP=%d/%d foeHP=%d/%d LAST=%S/%S foeLAST=%S encoreMove=%d timer=%d",
            encore, results[i].punch, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), playerLeft->volatiles.encoredMove, playerLeft->volatiles.encoreTimer);
    } FINALLY {
        // Report the saved punch and the lost damage/healing separately; do
        // not claim this small board proves an optimal full-battle strategy.
        EXPECT_GT(results[1].eeveeHp, results[0].eeveeHp);
    }
}
