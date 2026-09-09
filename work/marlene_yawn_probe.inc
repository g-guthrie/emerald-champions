// Actual native Marlene; include after MarlenePlayer.
#line 58000 "work/marlene_yawn_probe.inc"

static void MarleneYawnBoard(bool32 chesto)
{
    struct EmeraldChampionsBattleSet poochyena = {
        .moves = {MOVE_SNARL, MOVE_CRUNCH, MOVE_TAUNT, MOVE_PROTECT},
        .item = chesto ? ITEM_CHESTO_BERRY : ITEM_EVIOLITE,
        .nature = NATURE_CAREFUL, .ability = ABILITY_RATTLED,
        .statPoints = {32, 0, 2, 0, 32, 0},
    };
    static const struct EmeraldChampionsBattleSet shellos = {
        .moves = {MOVE_EARTH_POWER, MOVE_SCALD, MOVE_RECOVER, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_STORM_DRAIN,
        .statPoints = {32, 0, 2, 0, 32, 0},
    };
    // Both native early species and full legal preparation menus. Dark blocks
    // Psychic; the live Storm Drain partner redirects Hydro Pump. No fake stats.
    MarlenePlayer(SPECIES_POOCHYENA, &poochyena);
    MarlenePlayer(SPECIES_SHELLOS, &shellos);
    AuthoredOpponent(TRAINER_MARLENE, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPOINK);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SLOWPOKE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BALTOY);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_GOTHITA);
}

DOUBLE_BATTLE_TEST("EC live review Marlene Yawn: sleep follows two end turns and respects immediate cure")
{
    u32 turns;
    bool32 chesto;
    PARAMETRIZE { turns = 1; chesto = FALSE; }
    PARAMETRIZE { turns = 2; chesto = FALSE; }
    PARAMETRIZE { turns = 2; chesto = TRUE; }
    GIVEN { MarleneYawnBoard(chesto); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (turn == 0)
                MOVE(playerRight, MOVE_PROTECT);
            else
                MOVE(playerRight, MOVE_RECOVER);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (turn == 0)
                MOVE(opponentRight, MOVE_YAWN, target: playerLeft);
            else
                MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        for (u32 turn = 0; turn < turns; turn++)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, playerLeft);
    } THEN {
        if (turns == 1)
        {
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 0);
            EXPECT_EQ((u32)playerLeft->volatiles.yawn, 1);
        }
        else
        {
            EXPECT_EQ((u32)playerLeft->volatiles.yawn, 0);
            if (chesto)
            {
                EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 0);
                EXPECT_EQ(playerLeft->item, ITEM_NONE);
            }
            else
                EXPECT_NE(playerLeft->status1 & STATUS1_SLEEP, 0);
        }
        Test_MgbaPrintf("MARLENE_YAWN_NATIVE turns=%d chesto=%d status=%d yawn=%d item=%d HP=%d/%d ownHP=%d/%d",
            turns, chesto, playerLeft->status1, (u32)playerLeft->volatiles.yawn, playerLeft->item,
            playerLeft->hp, playerRight->hp, opponentLeft->hp, opponentRight->hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Marlene Yawn: actual choices against Dark and live Storm Drain")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    PARAMETRIZE { turns = 4; }
    GIVEN { MarleneYawnBoard(FALSE); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (turn == 0)
                MOVE(playerRight, MOVE_PROTECT);
            else
                MOVE(playerRight, MOVE_RECOVER);
        }
    } THEN {
        Test_MgbaPrintf("MARLENE_YAWN_AI turns=%d moves=%S/%S ownSpecies=%d/%d ownHP=%d/%d foes=%d/%d yawn=%d/%d status=%d/%d switches=%d",
            turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, (u32)playerLeft->volatiles.yawn, (u32)playerRight->volatiles.yawn,
            playerLeft->status1, playerRight->status1, gAiLogicData->shouldSwitch);
        Test_MgbaPrintf("MARLENE_YAWN_CLAUSE turns=%d enabled=%d active=%d partyIndex=%d trainer=%d PoochItem=%d ShellosItem=%d",
            turns, IsSleepClauseEnabled(), IsSleepClauseActiveForSide(B_SIDE_PLAYER),
            gBattleStruct->monCausingSleepClause[B_SIDE_PLAYER].partyIndex,
            gBattleStruct->monCausingSleepClause[B_SIDE_PLAYER].trainer,
            playerLeft->item, playerRight->item);
        // No forced opponent sequence. A native switch to coverage is a valid
        // alternative to Yawn; the diagnostic does not mandate the move name.
    }
}
