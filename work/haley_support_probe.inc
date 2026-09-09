// Temporary actual Haley full-menu support observations, not an instruction
// to choose Helping Hand or Thunder Wave. Include after HaleyLeadPlayer.
#line 58000 "work/haley_support_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Haley support: special bulk and public speed pressure")
{
    u32 board;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    PARAMETRIZE { board = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet specialEevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 2, 0, 0, 32, 0},
        };
        static const struct EmeraldChampionsBattleSet specialPachi = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 2, 0, 32, 0},
        };
        static const struct EmeraldChampionsBattleSet happiny = {
            .moves = {MOVE_SEISMIC_TOSS, MOVE_ICY_WIND, MOVE_LIGHT_SCREEN, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_NATURAL_CURE,
            .statPoints = {32, 0, 18, 0, 16, 0},
        };
        static const struct EmeraldChampionsBattleSet physicalEevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet fastPachi = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_TIMID, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {2, 0, 0, 32, 0, 32},
        };
        // Eevee/Pachi are early native choices; Happiny is the opening meadow
        // baby. All sets use legal prepared moves and exactly 66 Stat Points.
        // Special bulk offers a reason to support a physical partner, while
        // the third board supplies public speed pressure, not a forced TW win.
        HaleyLeadPlayer(board == 1 ? SPECIES_HAPPINY : SPECIES_EEVEE,
            board == 1 ? &happiny : &specialEevee);
        HaleyLeadPlayer(board == 1 ? SPECIES_EEVEE : SPECIES_PACHIRISU,
            board == 1 ? &physicalEevee : board == 2 ? &fastPachi : &specialPachi);
        AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MARILL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("HALEY_SUPPORT_INPUT board=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        TURN {
            if (board == 1)
            {
                MOVE(playerLeft, MOVE_SEISMIC_TOSS, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_BITE, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // Avoid interpreting Electric immunity from native Trace as
                // a support benefit; this publicly available attack hits Eevee.
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("HALEY_SUPPORT_AFTER board=%d nativeTurns=%d species=%d/%d slots=%d/%d ownHP=%d/%d foeHP=%d/%d LAST=%S/%S foeLAST=%S/%S abilities=%d/%d speedStages=%d/%d foeStatus=%d/%d faints=%d/%d",
            board, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->ability, opponentRight->ability,
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            playerLeft->status1, playerRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // No desired move/KO assertion: attacks, protection and voluntary
        // switching are legitimate alternatives to a support move here.
    }
}
