// Temporary native drain safety comparison; include after tiana_drain_probe.inc.
// Tentacool is a later-availability mechanical control (e.g. current Slateport
// Surf table), NOT a claim of Route102 opening catch access.
#line 29000 "work/tiana_drain_controls.inc"

DOUBLE_BATTLE_TEST("EC live review Tiana drain controls: native Liquid Ooze and Big Root", s16 targetDamage, s16 drainChange, u16 startingHp, u16 endingHp)
{
    u32 control;
    PARAMETRIZE { control = 0; } // Actual Sitrus Swirlix; Clear Body permits drain.
    PARAMETRIZE { control = 1; } // Actual Sitrus Swirlix; Liquid Ooze reverses drain.
    PARAMETRIZE { control = 2; } // Labeled mechanics-only Big Root item variant.
    GIVEN {
        struct EmeraldChampionsBattleSet tentacool = {
            .moves = {MOVE_ACID_ARMOR, MOVE_SCALD, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_CLEAR_BODY,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        if (control != 0)
            tentacool.ability = ABILITY_LIQUID_OOZE;
        TianaDrainPlayer(SPECIES_TENTACOOL, &tentacool);
        TianaDrainPlayer(SPECIES_PACHIRISU, &pachirisu);
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *swirlix = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1];
        EXPECT_EQ(GetMonData(swirlix, MON_DATA_SPECIES), SPECIES_SWIRLIX);
        EXPECT_EQ(GetMonData(swirlix, MON_DATA_MOVE1), MOVE_DRAINING_KISS);
        EXPECT_EQ(GetMonData(swirlix, MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
        // Same reachable injury as the prior drain probe; derive from native
        // max HP rather than inserting new defensive stats or berry state.
        u16 hp = (GetMonData(swirlix, MON_DATA_MAX_HP) + 1) / 2 + 1;
        SetMonData(swirlix, MON_DATA_HP, &hp);
        results[i].startingHp = hp;
        if (control == 2)
        {
            // Sole loadout override, only for the explicitly labeled mechanic
            // comparison. This is not authored Tiana or a proposed team edit.
            u16 item = ITEM_BIG_ROOT;
            SetMonData(swirlix, MON_DATA_HELD_ITEM, &item);
        }
        Test_MgbaPrintf("TIANA_DRAIN_CONTROL_INPUT c=%d SwirlixHP=%d/%d SpA=%d Speed=%d item=%d TentacoolAbility=%d",
            control, hp, GetMonData(swirlix, MON_DATA_MAX_HP), GetMonData(swirlix, MON_DATA_SPATK),
            GetMonData(swirlix, MON_DATA_SPEED), GetMonData(swirlix, MON_DATA_HELD_ITEM),
            GetMonAbility(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0]));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ACID_ARMOR);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_DRAINING_KISS, target: playerLeft,
                hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAINING_KISS, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].targetDamage);
        // Capture the immediate drain/Ooze HP change, before any later Sitrus
        // recovery. Final HP alone can misleadingly make Ooze look beneficial
        // because the harmful effect crosses Sitrus's activation threshold.
        HP_BAR(opponentRight, captureDamage: &results[i].drainChange);
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_SWIRLIX);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_DRAINING_KISS);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ACID_ARMOR);
        EXPECT_GT(results[i].targetDamage, 0);
        s32 nativeDrain = results[i].targetDamage * 75 / 100;
        if (control == 2)
            nativeDrain = nativeDrain * 1300 / 1000;
        nativeDrain = max(1, nativeDrain);
        if (control == 0)
        {
            EXPECT_LT(results[i].drainChange, 0);
            EXPECT_EQ(-results[i].drainChange,
                min(nativeDrain, opponentRight->maxHP - results[i].startingHp));
            EXPECT_EQ(opponentRight->item, ITEM_SITRUS_BERRY);
            EXPECT_EQ((u32)opponentRight->volatiles.unburdenActive, 0);
        }
        else
        {
            EXPECT_EQ(results[i].drainChange, nativeDrain);
            if (control == 1)
            {
                // This small native hit should cross the initial just-above-
                // half HP threshold; verify real consumption, not just heal.
                EXPECT_EQ(opponentRight->item, ITEM_NONE);
                EXPECT_EQ((u32)opponentRight->volatiles.unburdenActive, 1);
            }
            else
            {
                EXPECT_EQ(opponentRight->item, ITEM_BIG_ROOT);
                EXPECT_EQ((u32)opponentRight->volatiles.unburdenActive, 0);
            }
        }
        results[i].endingHp = opponentRight->hp;
        Test_MgbaPrintf("TIANA_DRAIN_CONTROL_AFTER c=%d targetDamage=%d drainChange=%d SwirlixHP=%d->%d item=%d Unburden=%d targetHP=%d/%d",
            control, results[i].targetDamage, results[i].drainChange,
            results[i].startingHp, results[i].endingHp, opponentRight->item,
            opponentRight->volatiles.unburdenActive, playerLeft->hp, playerLeft->maxHP);
        // Forced native mechanics, not AI choice or an opening availability
        // claim. No Magic Guard is assigned to species that cannot have it.
    }
}
