// Temporary actual E0015 setup-window observations, not forced setup.
// Include after IvanLeadPlayer. All offensive menus and reserves remain live.
#line 53000 "work/ivan_setup_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Ivan setup: speed crossing and specially bulky low pressure")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet fastPachi = {
            .moves = {MOVE_SUPER_FANG, MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB,
            .evs = {4, 0, 0, 0, 252, 252},
        };
        static const struct EmeraldChampionsBattleSet fastTimburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
            .evs = {4, 0, 0, 0, 252, 252},
        };
        static const struct EmeraldChampionsBattleSet bulkyLotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_RAIN_DISH,
            .evs = {252, 0, 4, 0, 252, 0},
        };
        IvanLeadPlayer(board == 0 ? SPECIES_PACHIRISU : SPECIES_EEVEE,
            board == 0 ? &fastPachi : &sIvanEevee);
        IvanLeadPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_LOTAD,
            board == 0 ? &fastTimburr : &bulkyLotad);
        AuthoredOpponent(TRAINER_IVAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_TENTACOOL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_CLAMPERL);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CHEWTLE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CHINCHOU);
        if (board == 0)
            for (u32 slot = 0; slot < 2; slot++)
                EXPECT_GT(GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][slot], MON_DATA_SPEED),
                    GetMonData(&party[1], MON_DATA_SPEED));
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("IVAN_SETUP_INPUT board=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_BITE, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("IVAN_SETUP_AFTER board=%d turns=%d nativeTurns=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S ownAtk=%d/%d ownDef=%d/%d ownSpA=%d/%d ownSpD=%d/%d ownSpeed=%d/%d foeSpD=%d/%d foeSpeed=%d/%d faints=%d/%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPDEF], playerRight->statStages[STAT_SPDEF],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("IVAN_SETUP_PARTY board=%d turns=%d slot=%d species=%d HP=%d/%d item=%d",
                board, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM));
        // The AI sees the full threatening menus, not these selected commands.
        // Nonselection of Shell Smash is not itself a failed observation.
    }
}
