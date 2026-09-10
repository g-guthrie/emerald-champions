// Temporary native Starly investment tradeoff; after JohnsonPlayer helpers.
#line 55000 "work/johnson_bulk_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Johnson Starly bulk: physical special denial and lost Speed")
{
    u32 bulk; // 0 original, 1 maximum tested bulk, 2 Speed53 compromise.
    u32 board;
    PARAMETRIZE { bulk = FALSE; board = 0; }
    PARAMETRIZE { bulk = TRUE; board = 0; }
    PARAMETRIZE { bulk = FALSE; board = 1; }
    PARAMETRIZE { bulk = TRUE; board = 1; }
    PARAMETRIZE { bulk = FALSE; board = 2; }
    PARAMETRIZE { bulk = TRUE; board = 2; }
    PARAMETRIZE { bulk = FALSE; board = 3; }
    PARAMETRIZE { bulk = TRUE; board = 3; }
    PARAMETRIZE { bulk = 2; board = 0; }
    PARAMETRIZE { bulk = 2; board = 1; }
    PARAMETRIZE { bulk = 2; board = 2; }
    PARAMETRIZE { bulk = 2; board = 3; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_THUNDERBOLT, MOVE_FLAMETHROWER, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
            .statPoints = {32, 0, 10, 0, 0, 24},
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
        else if (board == 2)
        {
            JohnsonPlayer(SPECIES_MIENFOO, &sJohnsonMienfoo);
            JohnsonPlayer(SPECIES_PACHIRISU, &sJohnsonPachirisu);
        }
        else
        {
            // Current Route103 species and mapped legal preparation moves.
            // Native51Speed lies between original60 and candidate42 Starly.
            JohnsonPlayer(SPECIES_ELECTRIKE, &electrike);
            JohnsonPlayer(SPECIES_LOTAD, &sJohnsonLotad);
        }
        AuthoredOpponent(TRAINER_JOHNSON, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        const enum Species expected[] = {SPECIES_SHINX, SPECIES_STARLY, SPECIES_BUNNELBY, SPECIES_TAILLOW};
        for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
            EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        struct EmeraldChampionsBattleSet starly = {0};
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
            starly.moves[index] = GetMonData(&party[1], MON_DATA_MOVE1 + index);
        starly.item = GetMonData(&party[1], MON_DATA_HELD_ITEM);
        starly.nature = GetNature(&party[1]);
        starly.ability = GetMonAbility(&party[1]);
        // Preset order is HP/Atk/Def/SpA/SpD/Speed, not enum Stat's
        // HP/Atk/Def/Speed/SpA/SpD battle order.
        starly.statPoints[0] = bulk == 2 ? 8 : bulk ? 18 : 2;
        starly.statPoints[1] = 32;
        starly.statPoints[5] = bulk == 2 ? 26 : bulk ? 16 : 32;
        EXPECT_EQ(starly.nature, NATURE_JOLLY);
        EXPECT_EQ(starly.item, ITEM_SHARP_BEAK);
        EXPECT_EQ(starly.ability, ABILITY_RECKLESS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&party[1], &starly), EC_BATTLE_SET_SUCCESS);
        CalculateMonStats(&party[1]);
        u16 fullHp = GetMonData(&party[1], MON_DATA_MAX_HP);
        SetMonData(&party[1], MON_DATA_HP, &fullHp);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOHNSON_BULK_INPUT bulk=%d board=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d HPpoints=%d AtkPoints=%d SpeedPoints=%d moves=%d/%d/%d/%d",
                    bulk, board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_HP_EV), GetMonData(mon, MON_DATA_ATK_EV), GetMonData(mon, MON_DATA_SPEED_EV),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 2)
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: turn == 0 ? opponentLeft : opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
        }
        // Trainer move/target/switch choices and recoil remain native/unforced.
    } THEN {
        Test_MgbaPrintf("JOHNSON_BULK_AFTER bulk=%d board=%d native=%d active=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S foeMoves=%S/%S speed=%d/%d faints=%d/%d",
            bulk, board, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->speed, opponentRight->speed,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOHNSON_BULK_PARTY bulk=%d board=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                bulk, board, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
        }
    }
}
