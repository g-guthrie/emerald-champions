// Temporary actual Johnson observations; include after JohnsonPlayer/constants.
// Route116 Dreepy is a real land encounter. No trainer decisions are forced.
#line 53000 "work/johnson_denial_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Johnson denial: Intimidate special pressure and Ghost disruption")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    PARAMETRIZE { board = 2; turns = 3; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet johnsonDenialDreepy = {
            .moves = {MOVE_DRAGON_TAIL, MOVE_INFESTATION, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_CLEAR_BODY,
            .evs = {252, 0, 252, 0, 4, 0},
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
            // Native Clear Body also blocks Intimidate; Ghost typing alone
            // would not. All four Dreepy moves are native preparation legal.
            JohnsonPlayer(SPECIES_MIENFOO, &sJohnsonMienfoo);
            JohnsonPlayer(SPECIES_DREEPY, &johnsonDenialDreepy);
        }
        AuthoredOpponent(TRAINER_JOHNSON, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SHINX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_STARLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BUNNELBY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TAILLOW);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_INTIMIDATE);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_RECKLESS);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_HUGE_POWER);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_GUTS);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOHNSON_DENIAL_INPUT b=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
                Test_MgbaPrintf("JOHNSON_DENIAL_MENU b=%d owner=%d slot=%d moves=%d/%d/%d/%d friendship=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4), GetMonData(mon, MON_DATA_FRIENDSHIP));
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
            else if (turn == 0)
            {
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                // Genuine player phazing attempt may expose a native reserve;
                // no chosen incoming trainer species or AI switch is imposed.
                MOVE(playerRight, MOVE_DRAGON_TAIL, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_BITE, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JOHNSON_DENIAL_END b=%d t=%d native=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d faints=%d/%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOHNSON_DENIAL_MOVES b=%d t=%d own=%S/%S player=%S/%S",
            board, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]));
        Test_MgbaPrintf("JOHNSON_DENIAL_STATE b=%d t=%d playerAtk=%d/%d playerSpA=%d/%d ownStatus=%d/%d foeStatus=%d/%d items=%d/%d",
            board, turns, playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1,
            opponentLeft->item, opponentRight->item);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOHNSON_DENIAL_PARTY b=%d t=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                board, turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
        }
        // No prescribed denial success, phazing result, switch, attack or win.
    }
}
