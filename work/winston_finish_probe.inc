// Temporary actual E0012 full-menu followthrough; after Winston lead helpers.
// The accepted two-turn baselines leave both player slots alive. No fourth
// turn or synthetic substitute move is queued after a possible third-turn KO.
#line 45000 "work/winston_finish_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Winston followthrough: three leads and physical reserves")
{
    u32 board;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    PARAMETRIZE { board = 2; }
    PARAMETRIZE { board = 3; }
    GIVEN {
        if (board == 3)
        {
            // Match the existing physical reserve probe, not the different
            // defensive Eevee and Knock Off menu used by the lead probe.
            static const struct EmeraldChampionsBattleSet eevee = {
                .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
                .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
                .statPoints = {32, 32, 2, 0, 0, 0},
            };
            static const struct EmeraldChampionsBattleSet timburr = {
                .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
                .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
                .statPoints = {32, 0, 32, 0, 2, 0},
            };
            WinstonLeadPlayer(SPECIES_EEVEE, &eevee);
            WinstonLeadPlayer(SPECIES_TIMBURR, &timburr);
        }
        else
        {
            WinstonLeadPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_LOTAD : SPECIES_MIENFOO,
                board == 0 ? &sWinstonEevee : board == 1 ? &sWinstonLotad : &sWinstonMienfoo);
            WinstonLeadPlayer(board == 0 ? SPECIES_TIMBURR : board == 1 ? SPECIES_CHARMANDER : SPECIES_PACHIRISU,
                board == 0 ? &sWinstonTimburr : board == 1 ? &sWinstonCharmander : &sWinstonPachirisu);
        }
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
        if (board == 3)
        {
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
        }
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("WINSTON_FINISH_INPUT board=%d slot=%d species=%d level=%d HP=%d item=%d moves=%d/%d/%d/%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_HELD_ITEM),
                GetMonData(&party[slot], MON_DATA_MOVE1), GetMonData(&party[slot], MON_DATA_MOVE2),
                GetMonData(&party[slot], MON_DATA_MOVE3), GetMonData(&party[slot], MON_DATA_MOVE4));
    } WHEN {
        for (u32 turn = 0; turn < 3; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_BITE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else if (board == 2)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else if (turn == 1)
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                else
                    MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_BITE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("WINSTON_FINISH_AFTER board=%d nativeTurns=%d room=%d roomTimer=%d active=%d/%d party=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownAtk=%d/%d ownDef=%d/%d ownSpA=%d/%d ownSpeed=%d/%d ownFaints=%d foeFaints=%d",
            board, gBattleResults.battleTurnCounter, (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) != 0,
            gFieldTimers.trickRoomTimer, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("WINSTON_FINISH_PARTY board=%d slot=%d species=%d HP=%d/%d status=%d",
                board, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
