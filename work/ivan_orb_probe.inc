// Temporary actual Ivan reserves. Include after the Ivan lead player helpers.
#line 57000 "work/ivan_orb_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Ivan Orb: Knock Off removes damage boost and recoil", s16 knockDamage, s16 waterfallDamage, s16 orbDamage)
{
    bool32 removeOrb;
    PARAMETRIZE { removeOrb = FALSE; }
    PARAMETRIZE { removeOrb = TRUE; }
    GIVEN {
        IvanLeadPlayer(SPECIES_MIENFOO, &sIvanMienfoo);
        IvanLeadPlayer(SPECIES_PACHIRISU, &sIvanPachirisu);
        AuthoredOpponent(TRAINER_IVAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *player = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_TENTACOOL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_CLAMPERL);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CHEWTLE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CHINCHOU);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HELD_ITEM), ITEM_LIFE_ORB);
        EXPECT_GT(GetMonData(&player[0], MON_DATA_SPEED), GetMonData(&party[2], MON_DATA_SPEED));
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
        Test_MgbaPrintf("IVAN_ORB_INPUT MienAtk=%d Speed=%d ChewHP=%d Atk=%d Def=%d Speed=%d item=%d",
            GetMonData(&player[0], MON_DATA_ATK), GetMonData(&player[0], MON_DATA_SPEED),
            GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_ATK),
            GetMonData(&party[0], MON_DATA_DEF), GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&party[0], MON_DATA_HELD_ITEM));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: removeOrb ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_WATERFALL, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponentRight);
        if (removeOrb)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, playerLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].knockDamage);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATERFALL, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].waterfallDamage);
        if (!removeOrb)
            HP_BAR(opponentLeft, captureDamage: &results[i].orbDamage);
    } THEN {
        EXPECT_GT(opponentLeft->hp, 0);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_EQ(opponentLeft->item, removeOrb ? ITEM_NONE : ITEM_LIFE_ORB);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_WATERFALL);
        EXPECT_EQ(results[i].orbDamage, removeOrb ? 0 : max(1, opponentLeft->maxHP / 10));
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP - results[i].knockDamage - results[i].orbDamage);
        Test_MgbaPrintf("IVAN_ORB_NATIVE removed=%d Knock=%d Waterfall=%d Orb=%d ChewHP=%d/%d item=%d MienHP=%d/%d",
            removeOrb, results[i].knockDamage, results[i].waterfallDamage, results[i].orbDamage,
            opponentLeft->hp, opponentLeft->maxHP, opponentLeft->item, playerLeft->hp, playerLeft->maxHP);
    } FINALLY {
        EXPECT_GT(results[0].waterfallDamage, results[1].waterfallDamage);
    }
}
