// Temporary isolated decision boundary, not the full authored encounter.
// Include after WinstonReservePlayer. Species/stats/items are native; compact
// menus remove unrelated setup/coverage decisions explicitly.
#line 48000 "work/winston_recoil_ai_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Winston recoil: isolated low HP decision range")
{
    u32 injury;
    PARAMETRIZE { injury = 5; }
    PARAMETRIZE { injury = 6; }
    PARAMETRIZE { injury = 7; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_FOLLOW_ME, MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_PROTECT},
            .item = ITEM_NONE, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        WinstonReservePlayer(SPECIES_PACHIRISU, &pachirisu);
        WinstonReservePlayer(SPECIES_TIMBURR, &timburr);
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
        struct Pokemon *foes = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
        u16 hp = 0;
        SetMonData(&party[0], MON_DATA_HP, &hp);
        SetMonData(&party[1], MON_DATA_HP, &hp);
        hp = injury;
        SetMonData(&party[3], MON_DATA_HP, &hp);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        static const u16 menus[4][MAX_MON_MOVES] = {
            {MOVE_FOLLOW_ME, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
            {MOVE_BULK_UP, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
            {MOVE_PROTECT, MOVE_NONE, MOVE_NONE, MOVE_NONE},
            {MOVE_DOUBLE_EDGE, MOVE_PROTECT, MOVE_NONE, MOVE_NONE},
        };
        for (u32 actor = 0; actor < 4; actor++)
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                struct Pokemon *mon = actor < 2 ? &foes[actor] : &party[actor - 2];
                u16 move = menus[actor][slot];
                u8 pp = move == MOVE_NONE ? 0 : GetMovePP(move);
                SetMonData(mon, MON_DATA_MOVE1 + slot, &move);
                SetMonData(mon, MON_DATA_PP1 + slot, &pp);
            }
        Test_MgbaPrintf("WINSTON_RECOIL_AI_INPUT injury=%d WoolHP=%d/%d Atk=%d Def=%d Speed=%d item=%d PachiHP=%d Def=%d",
            injury, GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_MAX_HP),
            GetMonData(&party[1], MON_DATA_ATK), GetMonData(&party[1], MON_DATA_DEF),
            GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_HELD_ITEM),
            GetMonData(&foes[0], MON_DATA_HP), GetMonData(&foes[0], MON_DATA_DEF));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_BULK_UP);
            EXPECT_MOVE(opponentRight, injury < 7 ? MOVE_PROTECT : MOVE_DOUBLE_EDGE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, injury < 7 ? injury + 4 : 6);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        Test_MgbaPrintf("WINSTON_RECOIL_AI_BOUNDARY injury=%d ownHP=%d/%d partyWoolHP=%d foes=%d/%d LAST=%S/%S ownFaints=%d item=%d",
            injury, opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, opponentRight->item);
        // Leftovers heals only if native recoil leaves Wooloo alive. The
        // decision-time trace, not a next-turn cache, identifies the choice.
    }
}
