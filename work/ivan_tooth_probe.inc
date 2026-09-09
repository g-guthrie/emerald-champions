// Temporary actual Ivan mechanics and decisions; include after Ivan lead helper.
#line 55000 "work/ivan_tooth_probe.inc"

static void IvanToothBoard(void)
{
    IvanLeadPlayer(SPECIES_MIENFOO, &sIvanMienfoo);
    IvanLeadPlayer(SPECIES_PACHIRISU, &sIvanPachirisu);
    AuthoredOpponent(TRAINER_IVAN, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    struct Pokemon *player = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_TENTACOOL);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_CLAMPERL);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CHEWTLE);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CHINCHOU);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_DEEP_SEA_TOOTH);
    EXPECT_GT(GetMonData(&player[0], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
    Test_MgbaPrintf("IVAN_TOOTH_INPUT MienHP=%d Atk=%d Speed=%d ClamHP=%d Def=%d SpA=%d Speed=%d Tooth=%d",
        GetMonData(&player[0], MON_DATA_HP), GetMonData(&player[0], MON_DATA_ATK), GetMonData(&player[0], MON_DATA_SPEED),
        GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_DEF), GetMonData(&party[1], MON_DATA_SPATK),
        GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_HELD_ITEM));
}

DOUBLE_BATTLE_TEST("EC live review Ivan Tooth: native Knock Off before the special attack", s16 knockDamage, s16 iceDamage)
{
    bool32 removeTooth;
    PARAMETRIZE { removeTooth = FALSE; }
    PARAMETRIZE { removeTooth = TRUE; }
    GIVEN { IvanToothBoard(); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: removeTooth ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_ICE_BEAM, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponentLeft);
        if (removeTooth)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, playerLeft);
            HP_BAR(opponentRight, captureDamage: &results[i].knockDamage);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].iceDamage);
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_EQ(opponentRight->item, removeTooth ? ITEM_NONE : ITEM_DEEP_SEA_TOOTH);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_ICE_BEAM);
        Test_MgbaPrintf("IVAN_TOOTH_NATIVE removed=%d Knock=%d Ice=%d ClamHP=%d/%d item=%d MienHP=%d/%d",
            removeTooth, results[i].knockDamage, results[i].iceDamage,
            opponentRight->hp, opponentRight->maxHP, opponentRight->item, playerLeft->hp, playerLeft->maxHP);
    } FINALLY {
        EXPECT_GT(results[0].iceDamage, results[1].iceDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Ivan Tooth: unrestricted response to fast Knock Off")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN { IvanToothBoard(); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("IVAN_TOOTH_AI turns=%d species=%d/%d party=%d/%d HP=%d/%d items=%d/%d foes=%d/%d LAST=%S/%S ownFaints=%d foeFaints=%d",
            turns, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, opponentLeft->item, opponentRight->item,
            playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
