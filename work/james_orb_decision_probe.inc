// Temporary synthetic decision isolation, not an authored James matchup.
// Include after JamesOrbBoard. All species/stats/abilities/items are native;
// restricted move menus deliberately remove unrelated Taunt/coverage choices.
#line 27000 "work/james_orb_decision_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review James Life Orb: isolated survival threshold decision")
{
    u32 injury;
    PARAMETRIZE { injury = 4; }
    PARAMETRIZE { injury = 5; }
    GIVEN {
        JamesOrbBoard(injury, FALSE);
        struct Pokemon *foes = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        // Bring the genuine slower, powerful Water Bubble Dewpider alongside
        // Kricketune. The genuine Venipede and Surskit are fainted reserves.
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        struct Pokemon saved = party[0];
        party[0] = party[3];
        party[3] = saved;
        u16 hp = GetMonData(&party[0], MON_DATA_MAX_HP);
        SetMonData(&party[0], MON_DATA_HP, &hp);
        hp = 0;
        SetMonData(&party[3], MON_DATA_HP, &hp);
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
        // Pachirisu Follow Me/Protect and Timburr Bulk Up/Protect: no incoming
        // damage, contact penalty, or berry theft can explain an own-side KO.
        EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&party[0], MON_DATA_SPEED));
        EXPECT_EQ(GetMonData(&foes[0], MON_DATA_HELD_ITEM), ITEM_NONE);
        Test_MgbaPrintf("JAMES_ORB_ISOLATED_INPUT injury=%d DewHP=%d Atk=%d Speed=%d KrickHP=%d/%d Atk=%d Speed=%d foeHP=%d/%d",
            injury, GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_ATK),
            GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_HP),
            GetMonData(&party[1], MON_DATA_MAX_HP), GetMonData(&party[1], MON_DATA_ATK),
            GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&foes[0], MON_DATA_HP), GetMonData(&foes[1], MON_DATA_HP));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_BULK_UP);
            EXPECT_MOVE(opponentLeft, MOVE_LIQUIDATION);
            EXPECT_MOVE(opponentRight, injury == 4 ? MOVE_PROTECT : MOVE_BUG_BITE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, injury == 4 ? 4 : 1);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        Test_MgbaPrintf("JAMES_ORB_ISOLATED_AI injury=%d ownHP=%d/%d partyKrickHP=%d foes=%d/%d LAST=%S/%S faintCounts=%d/%d",
            injury, opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]),
            GetMoveName(gLastMoves[B_BATTLER_3]), gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Decision-time trace is authoritative for the selected action;
        // LAST and HP describe native execution, not the next cached choice.
    }
}
