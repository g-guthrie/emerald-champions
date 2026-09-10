// Temporary native Stat Point comparison; include after joey_drum_probe.inc.
#line 59000 "work/joey_bulk_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Joey bulk: balanced Zigzagoon under split pressure and opportunity")
{
    bool32 bulk, opportunity;
    u32 turns;
    PARAMETRIZE { bulk = FALSE; opportunity = FALSE; turns = 1; }
    PARAMETRIZE { bulk = TRUE; opportunity = FALSE; turns = 1; }
    PARAMETRIZE { bulk = FALSE; opportunity = FALSE; turns = 2; }
    PARAMETRIZE { bulk = TRUE; opportunity = FALSE; turns = 2; }
    PARAMETRIZE { bulk = FALSE; opportunity = TRUE; turns = 1; }
    PARAMETRIZE { bulk = TRUE; opportunity = TRUE; turns = 1; }
    PARAMETRIZE { bulk = FALSE; opportunity = TRUE; turns = 2; }
    PARAMETRIZE { bulk = TRUE; opportunity = TRUE; turns = 2; }
    GIVEN {
        JoeyDrumBoard(TRUE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HP), 0);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HP), 0);
        EXPECT_EQ(GetNature(&party[1]), NATURE_ADAMANT);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_GLUTTONY);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_FIGY_BERRY);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_ATK_EV), 32);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_DEF_EV), 0);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPATK_EV), 0);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPDEF_EV), 0);
        // Recompute from the actual generated record, not injected battle
        // stats or a fresh species template. Explicitly normalize both arms
        // so this comparison remains original-versus-candidate if adopted.
        u8 hpPoints = bulk ? 18 : 2;
        u8 speedPoints = bulk ? 16 : 32;
        SetMonData(&party[1], MON_DATA_HP_EV, &hpPoints);
        SetMonData(&party[1], MON_DATA_SPEED_EV, &speedPoints);
        CalculateMonStats(&party[1]);
        u16 fullHp = GetMonData(&party[1], MON_DATA_MAX_HP);
        SetMonData(&party[1], MON_DATA_HP, &fullHp);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HP_EV)
            + GetMonData(&party[1], MON_DATA_ATK_EV)
            + GetMonData(&party[1], MON_DATA_SPEED_EV), 66);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOEY_BULK_INPUT bulk=%d opportunity=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d points=%d/%d/%d/%d/%d/%d moves=%d/%d/%d/%d",
                    bulk, opportunity, turns, owner, slot,
                    GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP),
                    GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_HP_EV), GetMonData(mon, MON_DATA_ATK_EV),
                    GetMonData(mon, MON_DATA_DEF_EV), GetMonData(mon, MON_DATA_SPATK_EV),
                    GetMonData(mon, MON_DATA_SPDEF_EV), GetMonData(mon, MON_DATA_SPEED_EV),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (opportunity)
            {
                // Same public full menus as pressure. These actual player
                // choices offer a possible window, never AI command reads.
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_BULK_UP);
            }
            else
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE);
            }
        }
        // No trainer action, Drum success, target or number of hits forced.
    } THEN {
        Test_MgbaPrintf("JOEY_BULK_AFTER bulk=%d opportunity=%d turns=%d nativeTurns=%d active=%d/%d HP=%d/%d maxHP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S Atk=%d/%d Speed=%d/%d AtkStages=%d/%d plus6=%d/%d items=%d/%d confusion=%d/%d foeAtkStages=%d/%d faints=%d/%d",
            bulk, opportunity, turns, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, opponentLeft->maxHP, opponentRight->maxHP,
            playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->attack, opponentRight->attack, opponentLeft->speed, opponentRight->speed,
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_ATK] == MAX_STAT_STAGE,
            opponentRight->statStages[STAT_ATK] == MAX_STAT_STAGE,
            opponentLeft->item, opponentRight->item,
            opponentLeft->volatiles.confusionTimer, opponentRight->volatiles.confusionTimer,
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("JOEY_BULK_PARTY bulk=%d opportunity=%d turns=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                bulk, opportunity, turns, slot,
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
