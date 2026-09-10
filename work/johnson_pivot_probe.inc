// Temporary native Shinx Fire Fang / Volt Switch candidate comparison.
// Include after JohnsonPlayer/constants. No production team or AI edits.
#line 57000 "work/johnson_pivot_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Johnson pivot: native Volt Switch utility versus Fire Fang")
{
    u32 board, turns;
    bool32 candidate;
    PARAMETRIZE { board = 0; turns = 1; candidate = FALSE; }
    PARAMETRIZE { board = 0; turns = 1; candidate = TRUE; }
    PARAMETRIZE { board = 0; turns = 2; candidate = FALSE; }
    PARAMETRIZE { board = 0; turns = 2; candidate = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; candidate = FALSE; }
    PARAMETRIZE { board = 1; turns = 2; candidate = TRUE; }
    PARAMETRIZE { board = 2; turns = 2; candidate = FALSE; }
    PARAMETRIZE { board = 2; turns = 2; candidate = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet johnsonPivotDreepy = {
            .moves = {MOVE_DRAGON_TAIL, MOVE_INFESTATION, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_CLEAR_BODY,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        if (board == 0)
        {
            JohnsonPlayer(SPECIES_EEVEE, &sJohnsonEevee);
            JohnsonPlayer(SPECIES_TIMBURR, &sJohnsonTimburr);
        }
        else if (board == 1)
        {
            JohnsonPlayer(SPECIES_PACHIRISU, &sJohnsonPachirisu);
            JohnsonPlayer(SPECIES_LOTAD, &sJohnsonLotad);
        }
        else
        {
            JohnsonPlayer(SPECIES_MIENFOO, &sJohnsonMienfoo);
            JohnsonPlayer(SPECIES_DREEPY, &johnsonPivotDreepy);
        }
        AuthoredOpponent(TRAINER_JOHNSON, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SHINX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_STARLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BUNNELBY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TAILLOW);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_INTIMIDATE);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE3), MOVE_FIRE_FANG);
        if (candidate)
        {
            u16 move = MOVE_VOLT_SWITCH;
            u8 pp = GetMovePP(move);
            SetMonData(&party[0], MON_DATA_MOVE3, &move);
            SetMonData(&party[0], MON_DATA_PP3, &pp);
            EXPECT_EQ(GetMonData(&party[0], MON_DATA_PP3), GetMovePP(MOVE_VOLT_SWITCH));
        }
        // Exact native moves, PP, stat generation and complete party otherwise
        // preserved. Fire Fang coverage is genuinely lost in the candidate.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOHNSON_PIVOT_INPUT b=%d c=%d owner=%d slot=%d species=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d",
                    board, candidate, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                    GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                    GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
                Test_MgbaPrintf("JOHNSON_PIVOT_MENU b=%d c=%d owner=%d slot=%d moves=%d/%d/%d/%d pp3=%d friendship=%d",
                    board, candidate, owner, slot, GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4), GetMonData(mon, MON_DATA_PP3),
                    GetMonData(mon, MON_DATA_FRIENDSHIP));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                // Unlike the denial fixture, do not phaze Shinx ourselves:
                // an observed pivot must come from native trainer behavior.
                MOVE(playerRight, MOVE_BITE, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JOHNSON_PIVOT_END b=%d c=%d t=%d native=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d faints=%d/%d",
            board, candidate, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOHNSON_PIVOT_MOVES b=%d c=%d t=%d own=%S/%S player=%S/%S Atk=%d/%d",
            board, candidate, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK]);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOHNSON_PIVOT_PARTY b=%d c=%d t=%d slot=%d species=%d HP=%d/%d item=%d status=%d move3=%d pp3=%d",
                board, candidate, turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS),
                GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_PP3));
        }
        // No required Volt Switch, replacement or Intimidate cycle. Read the
        // native decision/action trace: a changed active species alone is not
        // proof Volt Switch executed (ordinary switching is also available),
        // and last-move records can reset when the incoming Pokemon replaces it.
    }
}
