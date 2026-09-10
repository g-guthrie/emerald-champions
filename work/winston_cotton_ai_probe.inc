// Temporary isolated decision diagnostic; after WinstonReservePlayer.
// Native generated species/stats/items, but SYNTHETIC restricted menus on both sides.
#line 46000 "work/winston_cotton_ai_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Winston Cotton Guard: isolated native-stat slow fast and special boundaries")
{
    u32 board;
    PARAMETRIZE { board = 0; } // Slower physical attacker: immediate mitigation possible.
    PARAMETRIZE { board = 1; } // Faster physical attacker: cannot mitigate the earlier hit.
    PARAMETRIZE { board = 2; } // Slower special attacker: Defense cannot reduce this hit.
    GIVEN {
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        struct EmeraldChampionsBattleSet munchlax = {
            .moves = {board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM, MOVE_REST, MOVE_PROTECT, MOVE_NONE},
            .item = ITEM_EVIOLITE, .nature = NATURE_QUIET, .ability = ABILITY_THICK_FAT,
            .statPoints = {board == 1 ? 16 : 32, 16, board == 1 ? 0 : 2, 16, 0, board == 1 ? 18 : 0},
        };
        // A native Route116 mechanic reference, not a claimed pre-Winston
        // catch. The fast variant trades HP/Defense for real Speed points;
        // Attack/SpA/nature stay equal. Do not call it a pure speed-only clone.
        // The visible menu, not just the pending human command, distinguishes
        // physical and special pressure: each Munchlax has one damaging move.
        // This intentionally synthetic menu intervention exposes what the AI
        // can know; it does not claim a naturally encountered player loadout.
        WinstonReservePlayer(SPECIES_LOTAD, &lotad);
        WinstonReservePlayer(SPECIES_MUNCHLAX, &munchlax);
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE1), MOVE_COTTON_GUARD);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE2), MOVE_DOUBLE_EDGE);
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
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        {
            u16 move = slot == 0 ? MOVE_PROTECT : MOVE_NONE;
            u8 pp = move == MOVE_NONE ? 0 : GetMovePP(move);
            SetMonData(&party[0], MON_DATA_MOVE1 + slot, &move);
            SetMonData(&party[0], MON_DATA_PP1 + slot, &pp);
            if (slot >= 2)
            {
                move = MOVE_NONE;
                pp = 0;
                SetMonData(&party[1], MON_DATA_MOVE1 + slot, &move);
                SetMonData(&party[1], MON_DATA_PP1 + slot, &pp);
            }
        }
        struct Pokemon *foe = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1];
        if (board == 1)
            EXPECT_GT(GetMonData(foe, MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        else
            EXPECT_LT(GetMonData(foe, MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        Test_MgbaPrintf("WINSTON_COTTON_AI_INPUT board=%d syntheticMenus=1 WoolHP=%d Atk=%d Def=%d SpD=%d Speed=%d MunchHP=%d Atk=%d Def=%d SpA=%d Speed=%d",
            board, GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_ATK),
            GetMonData(&party[1], MON_DATA_DEF), GetMonData(&party[1], MON_DATA_SPDEF), GetMonData(&party[1], MON_DATA_SPEED),
            GetMonData(foe, MON_DATA_HP), GetMonData(foe, MON_DATA_ATK), GetMonData(foe, MON_DATA_DEF),
            GetMonData(foe, MON_DATA_SPATK), GetMonData(foe, MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // Opponent commands are unrestricted within the disclosed menus.
        }
    } THEN {
        Test_MgbaPrintf("WINSTON_COTTON_POSITION board=%d value=%d", board, AI_EvaluateDoublesPosition(B_BATTLER_1, 0));
        Test_MgbaPrintf("WINSTON_COTTON_AI_AFTER board=%d syntheticMenus=1 WoolHP=%d/%d MunchHP=%d/%d LAST=%S/%S foeLAST=%S WoolDefStage=%d status=%d/%d faints=%d/%d room=%d",
            board, opponentRight->hp, opponentRight->maxHP, playerRight->hp, playerRight->maxHP,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentRight->statStages[STAT_DEF], opponentRight->status1, playerRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) != 0);
        // No desired move/HP outcome assertion: this is a bounded observation
        // for the shared defense forecast, not authored full-menu completion.
    }
}
