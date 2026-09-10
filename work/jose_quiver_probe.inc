// Temporary native Jose observation and separate synthetic shared boundaries.
// Include after JosePlayer. No authored team changes or forced trainer moves.
#line 56000 "work/jose_quiver_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Jose Quiver: actual full party under special pressure")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    GIVEN {
        JosePlayer(SPECIES_PACHIRISU, &sJosePachirisu);
        JosePlayer(SPECIES_LOTAD, &sJoseLotad);
        AuthoredOpponent(TRAINER_JOSE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_DUSTOX);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_BEAUTIFLY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_VENONAT);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOSE_QUIVER_INPUT owner=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOSE_QUIVER_END t=%d active=%d/%d HP=%d/%d foes=%d/%d SpA=%d/%d SpD=%d/%d Speed=%d/%d wind=%d faints=%d/%d",
            turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOSE_QUIVER_MOVES t=%d own=%S/%S player=%S/%S",
            turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]));
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOSE_QUIVER_PARTY t=%d slot=%d species=%d HP=%d/%d status=%d item=%d",
                turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_STATUS), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
    }
}

// Adapted from the retained Cotton Guard timing diagnostic, but deliberately
// SYNTHETIC: no authored ID/SETUP opinion, no claim these are prepared Wooper
// stats. Native legal Wooper ability/move isolate the action-order boundary.
// Beautifly has a restricted generic QD/Bug Buzz menu and no Focus Sash;
// this is not its authored loadout or a campaign balance test.
AI_DOUBLE_BATTLE_TEST("EC live review Jose Quiver shared: timely special defense versus faster incoming attack")
{
    u32 board;
    PARAMETRIZE { board = 0; } // Slow special attacker: possible immediate payoff.
    PARAMETRIZE { board = 1; } // Faster special attacker: boost arrives too late.
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_WOOPER) {
            Level(14); HP(98); MaxHP(98); Attack(49); Defense(22);
            SpAttack(39); SpDefense(37); Speed(board == 1 ? 65 : 9);
            Nature(NATURE_QUIET); Ability(ABILITY_WATER_ABSORB);
            Item(ITEM_EVIOLITE); Moves(MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RATTLED); Item(ITEM_FOCUS_SASH); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BEAUTIFLY) {
            Level(12); HP(40); MaxHP(50); Attack(18); Defense(23);
            SpAttack(67); SpDefense(23); Speed(58); Nature(NATURE_TIMID);
            Ability(ABILITY_SWARM); Item(ITEM_NONE);
            Moves(MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ);
        }
        Test_MgbaPrintf("JOSE_QUIVER_SHARED_INPUT b=%d synthetic=1 BeautyHP=40/50 Def=23 SpA=67 SpD=23 Speed=58 WooperHP=98 Atk=49 Def=22 SpA=39 SpD=37 Speed=%d",
            board, board == 1 ? 65 : 9);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            EXPECT_MOVE(opponentRight, board == 0 ? MOVE_QUIVER_DANCE : MOVE_BUG_BUZZ);
        }
    } THEN {
        Test_MgbaPrintf("JOSE_QUIVER_SHARED_END b=%d synthetic=1 BeautyHP=%d/%d WooperHP=%d/%d SpA=%d SpD=%d Speed=%d faints=%d/%d",
            board, opponentRight->hp, opponentRight->maxHP, playerRight->hp, playerRight->maxHP,
            opponentRight->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPDEF],
            opponentRight->statStages[STAT_SPEED], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        EXPECT_EQ(opponentRight->hp, board == 0 ? 14 : 2);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + (board == 0));
        Test_MgbaPrintf("JOSE_QUIVER_SHARED_MOVES b=%d own=%S/%S incoming=%S",
            board, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_2]));
        // Observe first: selecting QD is not required. Retain a regression
        // only after native positive and disabled-cache negative evidence.
    }
}
