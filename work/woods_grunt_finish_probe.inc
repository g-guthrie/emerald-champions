// Temporary integrated-team end-position observations; after Woods helpers.
// Only reachable current HP / reserve phase and native player sets change.
#line 16000 "work/woods_grunt_finish_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Woods grunt finish: injured leads bench availability and reserve continuation")
{
    u32 board;
    PARAMETRIZE { board = 0; } // Injured leads, healthy actual reserves.
    PARAMETRIZE { board = 1; } // Same injured leads, no remaining reserves.
    PARAMETRIZE { board = 2; } // Healthy actual reserves, special pressure.
    PARAMETRIZE { board = 3; } // Healthy actual reserves, physical pressure.
    GIVEN {
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        WoodsLeadPlayer(board == 2 ? SPECIES_NATU : SPECIES_EEVEE, board == 2 ? &natu : &sWoodsEevee);
        WoodsLeadPlayer(board == 2 ? SPECIES_CHARMANDER : SPECIES_TIMBURR, board == 2 ? &charmander : &sWoodsTimburr);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CARVANHA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WAILMER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GRIMER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_POOCHYENA);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
        u16 zero = 0;
        if (board < 2)
        {
            u16 carvanhaHp = 1, wailmerHp = 10;
            SetMonData(&party[0], MON_DATA_HP, &carvanhaHp);
            SetMonData(&party[1], MON_DATA_HP, &wailmerHp);
            if (board == 1)
            {
                SetMonData(&party[2], MON_DATA_HP, &zero);
                SetMonData(&party[3], MON_DATA_HP, &zero);
            }
        }
        else
        {
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
            Test_MgbaPrintf("WOODS_FINISH_INPUT board=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_MAX_HP),
                GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
                GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
                GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    } WHEN {
        // The DSL records turns before execution; it has no dynamic early-end
        // loop. A native finish before turn3 must be reported and that board's
        // record shortened, never treated as a game hang or forced extra turn.
        for (u32 turn = 0; turn < (board < 2 ? 1 : 3); turn++)
        {
            TURN {
                if (board == 2)
                {
                    MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                }
                else
                {
                    // Integrated physical continuation already established
                    // Eevee's real turn2 KO; do not command it on turn3.
                    if (board != 3 || turn < 2)
                        MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: board < 2 ? opponentRight : opponentLeft,
                            hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    else
                        SKIP_TURN(playerLeft); // Fainted; do not auto-insert Celebrate as a fifth move.
                    MOVE(playerRight, MOVE_DRAIN_PUNCH, target: board < 2 ? opponentLeft : opponentRight,
                        hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                }
                // All opponent actions/replacements remain native AI choices.
            }
        }
    } THEN {
        Test_MgbaPrintf("WOODS_FINISH_AFTER board=%d nativeTurns=%d outcome=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S foeSpA=%d/%d ownFaints=%d foeFaints=%d status=%d/%d foeStatus=%d/%d",
            board, gBattleResults.battleTurnCounter, gBattleOutcome, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("WOODS_FINISH_PARTY board=%d slot=%d species=%d HP=%d/%d status=%d",
                board, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
        // Initial KO scenario members are not new battle faint events; party
        // readback and current battle faint counters are reported separately.
    }
}
