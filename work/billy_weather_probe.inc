// Native weather counterfactuals, separate from unrestricted AI decisions.
#line 34000 "work/billy_weather_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Billy weather: rain damage respects defender Umbrella", s16 damage)
{
    bool32 rain, umbrella;
    PARAMETRIZE { rain = FALSE; umbrella = FALSE; }
    PARAMETRIZE { rain = TRUE; umbrella = FALSE; }
    PARAMETRIZE { rain = FALSE; umbrella = TRUE; }
    PARAMETRIZE { rain = TRUE; umbrella = TRUE; }
    GIVEN {
        struct EmeraldChampionsBattleSet target = sBillyProbePachirisu;
        target.item = umbrella ? ITEM_UTILITY_UMBRELLA : ITEM_NONE;
        BillyProbePlayer(SPECIES_PACHIRISU, &target);
        BillyProbePlayer(SPECIES_EEVEE, &sBillyProbeEevee);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            if (rain)
                MOVE(opponentLeft, MOVE_RAIN_DANCE);
            else
                MOVE(opponentLeft, MOVE_ICE_BEAM, target: playerRight);
            MOVE(opponentRight, MOVE_MUDDY_WATER, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUDDY_WATER, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } THEN {
        Test_MgbaPrintf("BILLY_WEATHER rain=%d umbrella=%d damage=%d weather=%d HP=%d/%d",
            rain, umbrella, results[i].damage, gBattleWeather, playerLeft->hp, opponentRight->hp);
        EXPECT_GT(results[i].damage, 0);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

DOUBLE_BATTLE_TEST("EC live review Billy weather: native Hurricane accuracy after faster weather replacement")
{
    u32 scenario;
    PARAMETRIZE { scenario = 0; } // Rain remains.
    PARAMETRIZE { scenario = 1; } // Natu changes rain to sun before Hurricane.
    PARAMETRIZE { scenario = 2; } // Explicit attacker Umbrella mechanics variant.
    GIVEN {
        struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_SUNNY_DAY, MOVE_REFLECT, MOVE_ROOST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_MAGIC_BOUNCE,
            .statPoints = {32, 0, 13, 0, 0, 21},
        };
        BillyProbePlayer(SPECIES_NATU, &natu);
        BillyProbePlayer(SPECIES_PACHIRISU, &sBillyProbePachirisu);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&players[0], MON_DATA_SPEED));
        EXPECT_GT(GetMonData(&players[0], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        if (scenario == 2)
        {
            u16 item = ITEM_UTILITY_UMBRELLA;
            SetMonData(&party[1], MON_DATA_HELD_ITEM, &item);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, scenario == 1 ? MOVE_SUNNY_DAY : MOVE_REFLECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_RAIN_DANCE);
            MOVE(opponentRight, MOVE_HURRICANE, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, scenario == 1 ? MOVE_SUNNY_DAY : MOVE_REFLECT, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HURRICANE, opponentRight);
    } THEN {
        struct BattleCalcValues cv = { .battlerAtk = B_BATTLER_3, .battlerDef = B_BATTLER_0,
            .move = MOVE_HURRICANE, .moveEffect = GetMoveEffect(MOVE_HURRICANE) };
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            cv.holdEffects[battler] = GetBattlerHoldEffect(battler);
            cv.abilities[battler] = GetBattlerAbility(battler);
        }
        u32 accuracy = CanMoveSkipAccuracyCalc(&cv, gBattleWeather, AI_CHECK) ? 100 : GetTotalAccuracy(&cv, gBattleWeather);
        EXPECT_EQ(accuracy, scenario == 0 ? 100 : scenario == 1 ? 50 : 70);
        Test_MgbaPrintf("BILLY_HURRICANE c=%d weather=%d nativeAccuracy=%d HP=%d/%d",
            scenario, gBattleWeather, accuracy, playerLeft->hp, opponentRight->hp);
        // Forced hit establishes turn order, not a measured hit frequency.
    }
}
