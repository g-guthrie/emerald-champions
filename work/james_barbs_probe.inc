// Temporary native early-access Ferroseed boundary; include after JamesLeadPlayer.
#line 32000 "work/james_barbs_probe.inc"

DOUBLE_BATTLE_TEST("EC live review James contact: stolen Sitrus Iron Barbs and Life Orb order", s16 healing; s16 barbs; s16 orb)
{
    bool32 berry;
    PARAMETRIZE { berry = FALSE; }
    PARAMETRIZE { berry = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet ferroseed = {
            .moves = {MOVE_HARDEN, MOVE_PROTECT, MOVE_SEED_BOMB, MOVE_KNOCK_OFF},
            .item = berry ? ITEM_SITRUS_BERRY : ITEM_NONE,
            .nature = NATURE_IMPISH, .ability = ABILITY_IRON_BARBS,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        JamesLeadPlayer(SPECIES_FERROSEED, &ferroseed);
        JamesLeadPlayer(SPECIES_TIMBURR, &sJamesTimburr);
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_LIFE_ORB);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_TECHNICIAN);
        u16 hp = 0;
        SetMonData(&party[0], MON_DATA_HP, &hp);
        SetMonData(&party[1], MON_DATA_HP, &hp);
        hp = 5;
        SetMonData(&party[3], MON_DATA_HP, &hp);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        // Actual four-member party/native stats, real reserves placed first;
        // explicit injury is the only trainer stat alteration.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HARDEN);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_BUG_BITE, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, opponentRight);
        if (berry)
            HP_BAR(opponentRight, captureDamage: &results[i].healing);
        HP_BAR(opponentRight, captureDamage: &results[i].barbs);
        if (berry)
            HP_BAR(opponentRight, captureDamage: &results[i].orb);
        else
            NONE_OF { HP_BAR(opponentRight); }
    } THEN {
        u32 nativeBarbs = max(1, opponentRight->maxHP / 8);
        u32 nativeOrb = max(1, opponentRight->maxHP / 10);
        u32 nativeHeal = max(1, opponentRight->maxHP * GetItemHoldEffectParam(ITEM_SITRUS_BERRY) / 100);
        EXPECT_EQ(nativeBarbs, 5);
        EXPECT_EQ(results[i].barbs, nativeBarbs);
        EXPECT_EQ(opponentRight->hp, berry ? 5 + nativeHeal - nativeBarbs - nativeOrb : 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), opponentRight->hp);
        EXPECT_EQ(opponentRight->item, ITEM_LIFE_ORB);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, berry ? 0 : 1);
        if (berry)
        {
            EXPECT_EQ(results[i].healing, -(s32)nativeHeal);
            EXPECT_EQ(results[i].orb, nativeOrb);
        }
        Test_MgbaPrintf("JAMES_BARBS_NATIVE berry=%d heal=%d barbs=%d orb=%d KrickHP=%d/%d FerroHP=%d/%d FerroDef=%d ability=%d",
            berry, berry ? -results[i].healing : 0, results[i].barbs, berry ? results[i].orb : 0,
            opponentRight->hp, opponentRight->maxHP, playerLeft->hp, playerLeft->maxHP,
            playerLeft->defense, GetBattlerAbility(B_BATTLER_0));
    }
}
