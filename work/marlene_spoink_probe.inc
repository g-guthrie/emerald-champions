// Temporary native Spoink investment comparison. Include after MarlenePlayer.
// Only candidate Spoink's defensive Stat Points differ; no trainer decisions
// are prescribed, and the other three actual trainer members are unchanged.
#line 55000 "work/marlene_spoink_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Marlene Spoink: special mixed and physical bulk under Dark focus")
{
    u32 spread;
    PARAMETRIZE { spread = 0; }
    PARAMETRIZE { spread = 1; }
    PARAMETRIZE { spread = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet poochyena = {
            .moves = {MOVE_CRUNCH, MOVE_TAUNT, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_QUICK_FEET,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        // Route101 Poochyena has native Quick Feet and all four moves.
        MarlenePlayer(SPECIES_POOCHYENA, &poochyena);
        MarlenePlayer(SPECIES_PACHIRISU, &sMarlenePachirisu);
        AuthoredOpponent(TRAINER_MARLENE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPOINK);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SLOWPOKE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BALTOY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_GOTHITA);
        EXPECT_EQ(GetNature(&party[0]), NATURE_SASSY);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_THICK_FAT);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
        EXPECT_EQ(GetMonData(&party[0], EC_STAT_POINT_DATA(0)), 32);
        EXPECT_EQ(GetMonData(&party[0], EC_STAT_POINT_DATA(2)), 2);
        EXPECT_EQ(GetMonData(&party[0], EC_STAT_POINT_DATA(4)), 32);
        if (spread != 0)
        {
            u32 level = GetMonData(&party[0], MON_DATA_LEVEL);
            u32 speed = GetMonData(&party[0], MON_DATA_SPEED);
            u32 maxHp = GetMonData(&party[0], MON_DATA_MAX_HP);
            struct EmeraldChampionsBattleSet candidate = {
                .item = GetMonData(&party[0], MON_DATA_HELD_ITEM),
                .nature = GetNature(&party[0]),
                .ability = GetMonAbility(&party[0]),
                .statPoints = {32, 0, 0, 0, 0, 0},
            };
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                candidate.moves[slot] = GetMonData(&party[0], MON_DATA_MOVE1 + slot);
            candidate.statPoints[2] = spread == 1 ? 16 : 32;
            candidate.statPoints[4] = spread == 1 ? 18 : 2;
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&party[0], &candidate), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&party[0]);
            EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), level);
            EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPEED), speed);
            EXPECT_EQ(GetMonData(&party[0], MON_DATA_MAX_HP), maxHp);
            EXPECT_EQ(GetNature(&party[0]), NATURE_SASSY);
            EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_THICK_FAT);
            EXPECT_EQ(GetMonData(&party[0], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE1 + slot), candidate.moves[slot]);
            // AuthoredOpponent already made moves and Speed explicit. Those
            // values did not change; the actual recorded Pokemon owns Def/SpD.
        }
        for (u32 stat = 0; stat < NUM_STATS; stat++)
            EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP_IV + stat), MAX_PER_STAT_IVS);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("MARLENE_SPOINK_INPUT spread=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d points=%d/%d/%d/%d/%d/%d",
                    spread, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, EC_STAT_POINT_DATA(0)), GetMonData(mon, EC_STAT_POINT_DATA(1)),
                    GetMonData(mon, EC_STAT_POINT_DATA(2)), GetMonData(mon, EC_STAT_POINT_DATA(3)),
                    GetMonData(mon, EC_STAT_POINT_DATA(4)), GetMonData(mon, EC_STAT_POINT_DATA(5)));
            }
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            // Target Spoink's battlefield slot, retaining any unrestricted
            // native switch or replacement rather than tracking its species.
            MOVE(playerLeft, MOVE_CRUNCH, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("MARLENE_SPOINK_AFTER spread=%d turns=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S room=%d faints=%d/%d",
            spread, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            !!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("MARLENE_SPOINK_PARTY spread=%d slot=%d species=%d HP=%d/%d status=%d item=%d",
                spread, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_STATUS), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
        // No required Room, survival, switch, attack or winning spread.
    }
}
