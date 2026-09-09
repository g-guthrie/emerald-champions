// Temporary native Encore negatives; include after Tiana reserve helpers.
#line 30000 "work/tiana_encore_controls.inc"

DOUBLE_BATTLE_TEST("EC live review Tiana Encore safety: Mental Herb and absent move history", s16 punch)
{
    bool32 herb;
    PARAMETRIZE { herb = FALSE; }
    PARAMETRIZE { herb = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet timburr = sTianaReserveTimburr;
        if (herb)
            timburr.item = ITEM_MENTAL_HERB;
        TianaReservePlayer(SPECIES_TIMBURR, &timburr);
        TianaReservePlayer(SPECIES_PACHIRISU, &sTianaReservePachirisu);
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_BUNEARY);
        // Same native later-board transition as the existing reserve probe:
        // defeated original leads retained, real reserve records moved forward.
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
        EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED),
            GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
    } WHEN {
        if (herb)
        {
            TURN {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_PROTECT);
                MOVE(opponentLeft, MOVE_WISH);
                MOVE(opponentRight, MOVE_FAKE_OUT, target: playerRight);
            }
        }
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_ENCORE, target: playerLeft);
        }
    } SCENE {
        if (herb)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, playerLeft);
        // A failed/cured Encore must not erase the requested physical attack.
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_PUNCH, playerLeft);
        // Native Encore assigns a random opponent target before Mental Herb
        // clears its lock. Preserve that native target freedom after curing.
        ONE_OF {
            HP_BAR(opponentLeft, captureDamage: &results[i].punch);
            HP_BAR(opponentRight, captureDamage: &results[i].punch);
        }
    } THEN {
        EXPECT_GT(results[i].punch, 0);
        EXPECT_EQ((u32)playerLeft->volatiles.encoreTimer, 0);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ICE_PUNCH);
        EXPECT_EQ(playerLeft->item, herb ? ITEM_NONE : ITEM_EVIOLITE);
        Test_MgbaPrintf("TIANA_ENCORE_NEGATIVE herb=%d punch=%d item=%d timer=%d last=%S ownHP=%d/%d targetHP=%d",
            herb, results[i].punch, playerLeft->item, playerLeft->volatiles.encoreTimer,
            GetMoveName(gLastMoves[B_BATTLER_0]), opponentLeft->hp, opponentRight->hp, playerLeft->hp);
        // These forced native safety contracts do not prescribe an AI move.
        // Herb consumption is observed; any delayed Wish healing is separate
        // from the actual captured Ice Punch damage.
    }
}
