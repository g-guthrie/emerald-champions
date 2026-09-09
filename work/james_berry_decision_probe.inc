// Temporary synthetic decision isolation, not an authored James matchup.
// Include after JamesOrbBoard; the only paired parameter is the target item.
#line 28000 "work/james_berry_decision_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review James Life Orb: isolated stolen berry decision")
{
    bool32 berry;
    PARAMETRIZE { berry = FALSE; }
    PARAMETRIZE { berry = TRUE; }
    GIVEN {
        JamesOrbBoard(4, FALSE);
        struct Pokemon *foes = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        struct Pokemon saved = party[0];
        party[0] = party[3];
        party[3] = saved;
        u16 hp = GetMonData(&party[0], MON_DATA_MAX_HP);
        SetMonData(&party[0], MON_DATA_HP, &hp);
        hp = 0;
        SetMonData(&party[3], MON_DATA_HP, &hp);
        u16 item = berry ? ITEM_SITRUS_BERRY : ITEM_NONE;
        SetMonData(&foes[0], MON_DATA_HELD_ITEM, &item);
        static const u16 menus[4][MAX_MON_MOVES] = {
            {MOVE_LIQUIDATION, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
            {MOVE_BUG_BITE, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
            {MOVE_FOLLOW_ME, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
            {MOVE_BULK_UP, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
        };
        for (u32 actor = 0; actor < 4; actor++)
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                struct Pokemon *mon = actor < 2 ? &party[actor] : &foes[actor - 2];
                u16 move = menus[actor][slot];
                u8 pp = move == MOVE_NONE ? 0 : GetMovePP(move);
                SetMonData(mon, MON_DATA_MOVE1 + slot, &move);
                SetMonData(mon, MON_DATA_PP1 + slot, &pp);
            }
        EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&party[0], MON_DATA_SPEED));
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_LIFE_ORB);
        Test_MgbaPrintf("JAMES_BERRY_ISOLATED_INPUT berry=%d DewHP=%d Speed=%d KrickHP=%d/%d Speed=%d targetHP=%d item=%d",
            berry, GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_SPEED),
            GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_MAX_HP),
            GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&foes[0], MON_DATA_HP), item);
        // Native stats, abilities and species remain unchanged. Compact menus
        // remove unrelated Taunt/coverage choices; this is an explicit boundary
        // query and not evidence that the whole authored encounter is solved.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_BULK_UP);
            // Initial observation: neither AI move nor survival is prescribed.
        }
    } THEN {
        Test_MgbaPrintf("JAMES_BERRY_ISOLATED_AI berry=%d ownHP=%d/%d partyKrickHP=%d foes=%d/%d targetItem=%d ownItem=%d LAST=%S/%S faintCounts=%d/%d",
            berry, opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp, playerLeft->item, opponentRight->item,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
