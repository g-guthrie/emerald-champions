// Temporary actual E0018 loadout comparison. Include after HectorPlayer.
// Only Spinda's second move differs; trainer choices remain unrestricted.
#line 55000 "work/hector_helping_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Hector Helping Hand: original Encore versus partner support")
{
    u32 board;
    bool32 helping;
    PARAMETRIZE { board = 0; helping = FALSE; }
    PARAMETRIZE { board = 0; helping = TRUE; }
    PARAMETRIZE { board = 1; helping = FALSE; }
    PARAMETRIZE { board = 1; helping = TRUE; }
    GIVEN {
        HectorPlayer(SPECIES_PACHIRISU, &sHectorPachirisu);
        HectorPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_LOTAD,
            board == 0 ? &sHectorTimburr : &sHectorLotad);
        AuthoredOpponent(TRAINER_HECTOR, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPINDA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_ZANGOOSE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_SEVIPER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DUNSPARCE);
        // Both moves occur in Spinda's actual mapped preparation array. Keep
        // native PP bonuses and every unrelated trainer field unchanged.
        u16 move = helping ? MOVE_HELPING_HAND : MOVE_ENCORE;
        u8 pp = CalculatePPWithBonus(move, GetMonData(&party[0], MON_DATA_PP_BONUSES), 1);
        SetMonData(&party[0], MON_DATA_MOVE2, &move);
        SetMonData(&party[0], MON_DATA_PP2, &pp);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &party[slot];
            Test_MgbaPrintf("HECTOR_HELP_INPUT board=%d helping=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                board, helping, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
        }
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("HECTOR_HELP_AFTER board=%d helping=%d nativeTurns=%d active=%d/%d slots=%d/%d ownHP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownStatus=%d/%d foeStatus=%d/%d items=%d/%d faints=%d/%d",
            board, helping, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1,
            opponentLeft->item, opponentRight->item,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("HECTOR_HELP_PARTY board=%d helping=%d slot=%d species=%d HP=%d/%d status=%d",
                board, helping, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
        // Helping Hand is available, not mandated. Its nonselection is not
        // itself a defect; compare actual pressure and survival tradeoffs.
    }
}
