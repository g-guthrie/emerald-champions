// Temporary native between-hit berry timing; include after Billy helpers.
#line 39000 "work/billy_population_berry_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Billy Population Bomb: native Sitrus between hits versus no item", s16 firstHit, s16 healing, s16 laterHit, u16 finalHp)
{
    bool32 berry;
    PARAMETRIZE { berry = FALSE; }
    PARAMETRIZE { berry = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_FOLLOW_ME, MOVE_ROUND, MOVE_SUPER_FANG, MOVE_PROTECT},
            .item = berry ? ITEM_SITRUS_BERRY : ITEM_NONE,
            .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {2, 0, 18, 0, 14, 32},
        };
        BillyReservePlayer(SPECIES_PACHIRISU, &pachirisu);
        BillyReservePlayer(SPECIES_TIMBURR, &sBillyReserveTimburr);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_DITTO);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
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
        struct Pokemon *target = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0];
        Test_MgbaPrintf("BILLY_BERRY_INPUT berry=%d PachiHP=%d Def=%d Speed=%d TandHP=%d Atk=%d item=%d",
            berry, GetMonData(target, MON_DATA_MAX_HP), GetMonData(target, MON_DATA_DEF),
            GetMonData(target, MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_HP),
            GetMonData(&party[1], MON_DATA_ATK), GetMonData(&party[1], MON_DATA_HELD_ITEM));
        // Native full HP and real points only; the item is the sole difference.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            // Diagonal Imposter copies Timburr's actual Bulk Up in slot 2.
            MOVE(opponentLeft, moveSlot: 2, target: opponentLeft);
            MOVE(opponentRight, MOVE_POPULATION_BOMB, target: playerLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POPULATION_BOMB, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].firstHit);
        if (berry)
        {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, playerLeft);
            HP_BAR(playerLeft, captureDamage: &results[i].healing);
            HP_BAR(playerLeft, captureDamage: &results[i].laterHit);
        }
    } THEN {
        results[i].finalHp = playerLeft->hp;
        EXPECT_GT(results[i].firstHit, 0);
        if (berry)
        {
            EXPECT_LT(results[i].healing, 0);
            EXPECT_GT(results[i].laterHit, 0);
            EXPECT_EQ(playerLeft->item, ITEM_NONE);
        }
        Test_MgbaPrintf("BILLY_BERRY_AFTER berry=%d firstHit=%d healing=%d laterHit=%d PachiHP=%d/%d item=%d TandHP=%d/%d",
            berry, results[i].firstHit, results[i].healing, results[i].laterHit,
            playerLeft->hp, playerLeft->maxHP, playerLeft->item, opponentRight->hp, opponentRight->maxHP);
        // Hit-success is intentionally fixed for this damage/berry branch.
        // Observe final survival before asserting a numerical KO comparison;
        // this does not claim Wide Lens guarantees ten hits in normal play.
    }
}
