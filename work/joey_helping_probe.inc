// Temporary actual E0020 Encore / Helping Hand reserve comparison.
// Include after JoeyPlayer. Run against the corrected berry forecast build;
// a native pass is not an assertion that the candidate is tactically better.
#line 55000 "work/joey_helping_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Joey Helping Hand: candidate versus Encore under split pressure")
{
    bool32 helping;
    u32 turns;
    PARAMETRIZE { helping = FALSE; turns = 1; }
    PARAMETRIZE { helping = FALSE; turns = 2; }
    PARAMETRIZE { helping = TRUE; turns = 1; }
    PARAMETRIZE { helping = TRUE; turns = 2; }
    GIVEN {
        // Same native level14 full menus in each comparison.
        JoeyPlayer(SPECIES_PACHIRISU, &sJoeyPachirisu);
        JoeyPlayer(SPECIES_TIMBURR, &sJoeyTimburr);
        AuthoredOpponent(TRAINER_JOEY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PAWMI);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_LECHONK);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HELD_ITEM), ITEM_WIDE_LENS);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_FIGY_BERRY);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_OWN_TEMPO);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_MOVE3), MOVE_ENCORE);
        if (helping)
        {
            u16 move = MOVE_HELPING_HAND;
            u8 pp = GetMovePP(move);
            SetMonData(&party[2], MON_DATA_MOVE3, &move);
            SetMonData(&party[2], MON_DATA_PP3, &pp);
            // AuthoredOpponent made all moves explicit. Only slot three and
            // its native starting PP change; stats, ability and item do not.
            EXPECT_EQ(GetMonData(&party[2], MON_DATA_MOVE3), MOVE_HELPING_HAND);
            EXPECT_EQ(GetMonData(&party[2], MON_DATA_PP3), GetMovePP(MOVE_HELPING_HAND));
        }
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
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOEY_HELPING_INPUT helping=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    helping, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE);
        }
        // Full native Population Bomb/contact timing and Figy consumption;
        // no trainer Drum, Helping Hand, Encore, Protect or target is forced.
    } THEN {
        Test_MgbaPrintf("JOEY_HELPING_AFTER helping=%d turns=%d nativeTurns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownAtk=%d/%d ownSpeed=%d/%d ownStatus=%d/%d confusion=%d/%d items=%d/%d foeItems=%d/%d encore=%d/%d faints=%d/%d",
            helping, turns, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->speed, opponentRight->speed,
            opponentLeft->status1, opponentRight->status1,
            opponentLeft->volatiles.confusionTimer, opponentRight->volatiles.confusionTimer,
            opponentLeft->item, opponentRight->item, playerLeft->item, playerRight->item,
            playerLeft->volatiles.encoreTimer, playerRight->volatiles.encoreTimer,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("JOEY_HELPING_PARTY helping=%d turns=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                helping, turns, slot,
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
        Test_MgbaPrintf("JOEY_HELPING_PP helping=%d turns=%d move3=%d pp3=%d",
            helping, turns, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MOVE3),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_PP3));
        // Execution is evidence, not an assertion of optimal target or wins.
    }
}
