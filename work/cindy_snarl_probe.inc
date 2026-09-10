// Temporary forced native mechanics, after cindy_reserve_probe.inc.
// These commands isolate Snarl; they are not unrestricted AI-choice evidence.
#line 53000 "work/cindy_snarl_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Cindy Snarl: native special pressure Soundproof and Herb retaliation order", s16 snarlDamage, s16 specialDamage)
{
    u32 board;
    PARAMETRIZE { board = 0; } // Cute Charm, no Snarl: downstream damage control.
    PARAMETRIZE { board = 1; } // Cute Charm, Snarl: ordinary Special Attack drop.
    PARAMETRIZE { board = 2; } // Cute Charm, Snarl + immediate White Herb cure.
    PARAMETRIZE { board = 3; } // Competitive, Snarl + White Herb: boost precedes Herb.
    PARAMETRIZE { board = 4; } // Native Soundproof Whismur: Snarl is ineffective.
    GIVEN {
        struct EmeraldChampionsBattleSet target = {
            .moves = {MOVE_ICE_BEAM, MOVE_WATER_PULSE, MOVE_REST, MOVE_PROTECT},
            .item = board == 2 || board == 3 ? ITEM_WHITE_HERB : ITEM_NONE,
            .nature = NATURE_MODEST,
            .ability = board == 4 ? ABILITY_SOUNDPROOF : board == 3 ? ABILITY_COMPETITIVE : ABILITY_CUTE_CHARM,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        // Every listed move is in both species' native teachable array.
        // Jigglypuff/Whismur are mechanic controls, not claims of catches at
        // Cindy's exact location. Species abilities are never fabricated.
        CindyReservePlayer(board == 4 ? SPECIES_WHISMUR : SPECIES_JIGGLYPUFF, &target);
        CindyReservePlayer(SPECIES_LOTAD, &sCindyReserveLotad);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_FURFROU);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_MOVE4), MOVE_SNARL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SLAKOTH);
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
        struct Pokemon *player = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0];
        EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED), GetMonData(player, MON_DATA_SPEED));
        Test_MgbaPrintf("CINDY_SNARL_INPUT board=%d species=%d level=%d HP=%d SpA=%d Speed=%d ability=%d item=%d FurfrouSpeed=%d SlakothHP=%d SpD=%d",
            board, GetMonData(player, MON_DATA_SPECIES), GetMonData(player, MON_DATA_LEVEL),
            GetMonData(player, MON_DATA_HP), GetMonData(player, MON_DATA_SPATK), GetMonData(player, MON_DATA_SPEED),
            GetMonAbility(player), GetMonData(player, MON_DATA_HELD_ITEM), GetMonData(&party[0], MON_DATA_SPEED),
            GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_SPDEF));
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_PROTECT);
            if (board == 0)
                MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerRight);
            else
                MOVE(opponentLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_BODY_SLAM, target: playerRight);
            MOVE(playerLeft, MOVE_ICE_BEAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board > 0 && board < 4)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, opponentLeft);
            HP_BAR(playerLeft, captureDamage: &results[i].snarlDamage);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].specialDamage);
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_FURFROU);
        EXPECT_EQ(opponentRight->species, SPECIES_SLAKOTH);
        EXPECT_GT(results[i].specialDamage, 0);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ICE_BEAM);
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK],
            board == 1 ? DEFAULT_STAT_STAGE - 1 : board == 3 ? DEFAULT_STAT_STAGE + 1 : DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerLeft->item, board == 3 ? ITEM_WHITE_HERB : ITEM_NONE);
        if (board == 0 || board == 4)
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        else
            EXPECT_GT(results[i].snarlDamage, 0);
        if (board > 0)
            EXPECT_LT(opponentLeft->pp[3], GetMovePP(MOVE_SNARL));
        Test_MgbaPrintf("CINDY_SNARL_AFTER board=%d snarlDamage=%d specialDamage=%d targetHP=%d/%d SpAStage=%d item=%d SlakothHP=%d FurfrouLAST=%S targetLAST=%S",
            board, results[i].snarlDamage, results[i].specialDamage, playerLeft->hp, playerLeft->maxHP,
            playerLeft->statStages[STAT_SPATK], playerLeft->item, opponentRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_0]));
    } FINALLY {
        EXPECT_LT(results[1].specialDamage, results[0].specialDamage);
        EXPECT_EQ(results[2].specialDamage, results[0].specialDamage);
        EXPECT_GT(results[3].specialDamage, results[0].specialDamage);
        // Soundproof uses a different native species: its absolute damage is
        // logged, not incorrectly compared as an equal-stat Jigglypuff clone.
    }
}
