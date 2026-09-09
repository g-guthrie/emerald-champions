// Temporary native Reflect comparison; after Lyle reserve helper.
#line 63000 "work/lyle_reflect_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Lyle Reflect: native slower Body Slam and critical bypass", s16 damage)
{
    bool32 reflect, critical;
    PARAMETRIZE { reflect = FALSE; critical = FALSE; }
    PARAMETRIZE { reflect = TRUE; critical = FALSE; }
    PARAMETRIZE { reflect = FALSE; critical = TRUE; }
    PARAMETRIZE { reflect = TRUE; critical = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet munchlax = {
            .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        LyleReservePlayer(SPECIES_EEVEE, &eevee);
        LyleReservePlayer(SPECIES_MUNCHLAX, &munchlax);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_NYMBLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SEWADDLE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_ANORITH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
        u16 hp = 0;
        SetMonData(&party[0], MON_DATA_HP, &hp);
        SetMonData(&party[1], MON_DATA_HP, &hp);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED),
            GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_BODY_SLAM, target: opponentRight,
                hit: TRUE, criticalHit: critical, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (reflect)
                MOVE(opponentRight, MOVE_REFLECT);
            else
                MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, reflect ? MOVE_REFLECT : MOVE_HELPING_HAND, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BODY_SLAM, playerRight);
        HP_BAR(opponentRight, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_GT(results[i].damage, 0);
        EXPECT_EQ((gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT) != 0, reflect);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        Test_MgbaPrintf("LYLE_REFLECT_NATIVE reflect=%d crit=%d damage=%d DottlerHP=%d/%d Speed=%d MunchSpeed=%d MunchAtk=%d DottlerDef=%d timer=%d",
            reflect, critical, results[i].damage, opponentRight->hp, opponentRight->maxHP,
            opponentRight->speed, playerRight->speed, playerRight->attack, opponentRight->defense,
            gSideTimers[B_SIDE_OPPONENT].reflectTimer);
        // Helping Hand on a protecting partner is a harmless non-screen
        // control, not a recommended actual AI line or a new authored set.
    } FINALLY {
        EXPECT_LT(results[1].damage, results[0].damage);
        EXPECT_EQ(results[3].damage, results[2].damage);
    }
}
