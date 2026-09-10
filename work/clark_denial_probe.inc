// Temporary actual Clark observations. Include after ClarkPlayer/constants.
// Native full menus retain both single-target and spread alternatives; player
// commands below are not knowledge available to the AI's decision forecast.
#line 53000 "work/clark_denial_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Clark denial: single target disruption and Water spread pressure")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet clarkDenialSpreadLotad = {
            .moves = {MOVE_MUDDY_WATER, MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
            .evs = {252, 0, 0, 136, 120, 0},
        };
        static const struct EmeraldChampionsBattleSet clarkDenialSpreadShellos = {
            .moves = {MOVE_MUDDY_WATER, MOVE_SCALD, MOVE_EARTH_POWER, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_STORM_DRAIN,
            .evs = {252, 0, 0, 252, 4, 0},
        };
        // Route102 Lotad / Route103 Shellos land and every specified move are
        // native early preparation options. Muddy Water does not hit allies;
        // Shellos Storm Drain does not redirect the other player's spread move.
        if (board < 2)
        {
            ClarkPlayer(SPECIES_MIENFOO, &sClarkMienfoo);
            ClarkPlayer(SPECIES_LOTAD, board == 0 ? &sClarkLotad : &clarkDenialSpreadLotad);
        }
        else
        {
            ClarkPlayer(SPECIES_LOTAD, &clarkDenialSpreadLotad);
            ClarkPlayer(SPECIES_SHELLOS, &clarkDenialSpreadShellos);
        }
        AuthoredOpponent(TRAINER_CLARK, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_ROGGENROLA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_GEODUDE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MUDBRAY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ARCHEN);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_STURDY);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_ROCK_HEAD);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_STAMINA);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_DEFEATIST);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("CLARK_DENIAL_INPUT b=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
                Test_MgbaPrintf("CLARK_DENIAL_MENU b=%d owner=%d slot=%d moves=%d/%d/%d/%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 1)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                MOVE(playerRight, MOVE_MUDDY_WATER, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_MUDDY_WATER, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_MUDDY_WATER, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("CLARK_DENIAL_END b=%d t=%d native=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d faints=%d/%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("CLARK_DENIAL_MOVES b=%d t=%d own=%S/%S player=%S/%S",
            board, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]));
        Test_MgbaPrintf("CLARK_DENIAL_STATE b=%d t=%d Def=%d/%d Speed=%d/%d taunt=%d/%d foeStatus=%d/%d",
            board, turns, opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            playerLeft->status1, playerRight->status1);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("CLARK_DENIAL_PARTY b=%d t=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                board, turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
        }
        // No Wide Guard, setup, flinch, switch or KO is prescribed. Real early
        // faints may require a subsequent explicit command-timeline correction.
    }
}
