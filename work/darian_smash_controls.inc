// Temporary native same-turn defense cost, not a proposed White Herb team edit.
#line 46000 "work/darian_smash_controls.inc"
DOUBLE_BATTLE_TEST("EC live review Darian Smash: native defense loss before slower Body Slam", s16 slamDamage)
{
    u32 mode;
    PARAMETRIZE { mode = 0; }
    PARAMETRIZE { mode = 1; }
    PARAMETRIZE { mode = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet munchlax = {
            .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        DarianReservePlayer(SPECIES_PACHIRISU, &sDarianReservePachirisu);
        DarianReservePlayer(SPECIES_MUNCHLAX, &munchlax);
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 i = 0; i < 2; i++) {
            struct Pokemon temp = party[i];
            party[i] = party[i + 2];
            party[i + 2] = temp;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        if (mode == 2) {
            u16 herb = ITEM_WHITE_HERB;
            SetMonData(&party[1], MON_DATA_HELD_ITEM, &herb);
        }
        EXPECT_LT(GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_SPEED),
                  GetMonData(&party[1], MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_KNOCK_OFF, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (mode)
                MOVE(opponentRight, MOVE_SHELL_SMASH, target: opponentRight);
            else
                MOVE(opponentRight, MOVE_ICE_BEAM, target: playerRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BODY_SLAM, playerRight);
        HP_BAR(opponentRight, captureDamage: &results[i].slamDamage);
    } THEN {
        Test_MgbaPrintf("DARIAN_SMASH mode=%d slam=%d ownHP=%d/%d foes=%d/%d clamDef=%d clamSpDef=%d clamSpeed=%d item=%d",
            mode, results[i].slamDamage, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentRight->statStages[STAT_DEF], opponentRight->statStages[STAT_SPDEF],
            opponentRight->statStages[STAT_SPEED], opponentRight->item);
    }
}
