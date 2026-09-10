// Temporary actual Cyndy observations; include after CyndyPlayer.
// Two native player members avoid inventing an unobserved replacement timeline.
// Full menus are visible to the AI; these human commands are not AI forecasts.
#line 55000 "work/cyndy_counter_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Cyndy counters: Fairy Charm and Reflect burn")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet ralts = {
            .moves = {MOVE_DAZZLING_GLEAM, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .evs = {252, 0, 120, 136, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet togepi = {
            .moves = {MOVE_DAZZLING_GLEAM, MOVE_CHARM, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_SERENE_GRACE,
            .evs = {252, 0, 120, 136, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_AIR_SLASH, MOVE_PSYCHIC, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
            .evs = {252, 0, 4, 0, 0, 252},
        };
        static const struct EmeraldChampionsBattleSet growlithe = {
            .moves = {MOVE_FLAMETHROWER, MOVE_WILL_O_WISP, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_INTIMIDATE,
            .evs = {252, 0, 252, 4, 0, 0},
        };
        // Ralts, Togepi and Natu: Route102 grass; Growlithe: Route103 grass.
        // The helper generates level14, IV31, native stats and legal preparation.
        CyndyPlayer(board == 0 ? SPECIES_RALTS : SPECIES_NATU, board == 0 ? &ralts : &natu);
        CyndyPlayer(board == 0 ? SPECIES_TOGEPI : SPECIES_GROWLITHE, board == 0 ? &togepi : &growlithe);
        AuthoredOpponent(TRAINER_CYNDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_RIOLU);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MEDITITE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CRABRAWLER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CLOBBOPUS);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("CYNDY_COUNTER_INPUT board=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE);
                if (turn == 0)
                    MOVE(playerRight, MOVE_CHARM, target: opponentRight);
                else
                    MOVE(playerRight, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE);
            }
            else if (turn == 0)
            {
                MOVE(playerLeft, MOVE_REFLECT);
                MOVE(playerRight, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
            }
            else
            {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("CYNDY_COUNTER_AFTER board=%d turns=%d nativeTurns=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S Atk=%d/%d Def=%d/%d Speed=%d/%d status=%d/%d reflect=%d faints=%d/%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            opponentLeft->status1, opponentRight->status1,
            !!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gParties[owner][slot];
                Test_MgbaPrintf("CYNDY_COUNTER_PARTY board=%d turns=%d owner=%d slot=%d species=%d HP=%d/%d status=%d item=%d",
                    board, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_STATUS), GetMonData(mon, MON_DATA_HELD_ITEM));
            }
        // These are executed observations, not assertions that Charm, burn or
        // Reflect necessarily landed, nor that either counterplay board wins.
    }
}
