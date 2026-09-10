// Temporary remaining-position diagnostic, not a complete rival battle.
// Include after AuthoredOpponent and rival_ai_probe.inc (RivalReviewPlayer).
#line 8000 "work/rival_finish_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review E0001 Finish: actual Taillow takes the last available KO")
{
    bool32 poisoned;
    PARAMETRIZE { poisoned = FALSE; }
    PARAMETRIZE { poisoned = TRUE; }
    GIVEN {
        // Native level-14 IV31 preparation sets. Only current HP changes:
        // Natu is the last survivor; Mienfoo has fainted earlier in the fight.
        RivalReviewPlayer(SPECIES_NATU);
        RivalReviewPlayer(SPECIES_MIENFOO);
        u16 one = 1, zero = 0;
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_HP, &one);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_HP, &zero);

        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_TORCHIC, 0, FALSE);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP, &zero);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
        // Reachable just after Toxic Orb activation; derived stats, ability,
        // held item, four moves, and full original party remain untouched.
        u32 status = poisoned ? STATUS1_TOXIC_POISON : 0;
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][3], MON_DATA_STATUS, &status);
        EXPECT_EQ(GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][3], MON_DATA_HELD_ITEM), ITEM_TOXIC_ORB);
        EXPECT_EQ(GetMonAbility(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][3]), ABILITY_GUTS);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight);
            SKIP_TURN(playerRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        enum Move executed = gLastMoves[B_BATTLER_3];
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 3);
        EXPECT_EQ(opponentLeft->species, SPECIES_SHROOMISH);
        EXPECT_EQ(opponentRight->species, SPECIES_TAILLOW);
        EXPECT(gAbsentBattlerFlags & (1u << B_BATTLER_2));
        EXPECT(executed == MOVE_FACADE || executed == MOVE_BRAVE_BIRD || executed == MOVE_QUICK_ATTACK);
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT(opponentRight->hp > 0);
        Test_MgbaPrintf("E1 Finish poisoned=%d executed=%S playerHP=%d TaillowHP=%d status=%d speeds=%d/%d",
            poisoned, GetMoveName(executed), playerLeft->hp, opponentRight->hp,
            opponentRight->status1, playerLeft->speed, opponentRight->speed);
        // Facade, Brave Bird and Quick Attack are all acceptable immediate
        // finishes here. Do not prescribe Protect for orb activation or insist
        // on a particular attacking move when several native KOs are safe.
    }
}
