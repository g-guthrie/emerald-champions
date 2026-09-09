// Temporary isolated Life Orb boundaries. Include after JamesOrbBoard.
#line 25000 "work/james_orb_ai_probe.inc"

DOUBLE_BATTLE_TEST("EC live review James Life Orb: stolen Sitrus precedes recoil", s16 healing; s16 recoil)
{
    bool32 berry;
    PARAMETRIZE { berry = FALSE; }
    PARAMETRIZE { berry = TRUE; }
    GIVEN {
        JamesOrbBoard(4, FALSE);
        u16 item = berry ? ITEM_SITRUS_BERRY : ITEM_NONE;
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_BUG_BITE, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, opponentRight);
        if (berry)
            HP_BAR(opponentRight, captureDamage: &results[i].healing);
        HP_BAR(opponentRight, captureDamage: &results[i].recoil);
    } THEN {
        u32 nativeHeal = max(1, opponentRight->maxHP * GetItemHoldEffectParam(ITEM_SITRUS_BERRY) / 100);
        u32 nativeRecoil = max(1, opponentRight->maxHP / 10);
        EXPECT_EQ(nativeRecoil, 4);
        EXPECT_EQ(results[i].recoil, nativeRecoil);
        EXPECT_EQ(opponentRight->hp, berry ? nativeHeal : 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), opponentRight->hp);
        EXPECT_EQ(opponentRight->item, ITEM_LIFE_ORB);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        if (berry)
            EXPECT_EQ(results[i].healing, -(s32)nativeHeal);
        Test_MgbaPrintf("JAMES_ORB_STOLEN berry=%d heal=%d recoil=%d nativeHP=%d/%d targetHP=%d item=%d",
            berry, berry ? -results[i].healing : 0, results[i].recoil,
            opponentRight->hp, opponentRight->maxHP, playerLeft->hp, opponentRight->item);
        // Actual James stats, ability and Orb remain unchanged. Only explicit
        // injury and the otherwise identical native player's held item vary.
    }
}
