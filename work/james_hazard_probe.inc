// Temporary actual-menu hazard observation; after JamesReservePlayer.
#line 29000 "work/james_hazard_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review James hazards: actual reserves with zero versus two player bench members")
{
    bool32 bench;
    PARAMETRIZE { bench = FALSE; }
    PARAMETRIZE { bench = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_SCALD, MOVE_ICE_BEAM, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        JamesReservePlayer(SPECIES_PACHIRISU, &pachirisu);
        JamesReservePlayer(SPECIES_EEVEE, &eevee);
        if (bench)
        {
            JamesReservePlayer(SPECIES_TIMBURR, &timburr);
            JamesReservePlayer(SPECIES_LOTAD, &lotad);
        }
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_MOVE4), MOVE_TOXIC_SPIKES);
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
        // Neither actual trainer menu is restricted, and no hazard/ability/
        // weather state is injected. Early Route101/102 player species only.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER && !bench ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JAMES_HAZARD_INPUT bench=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Speed=%d item=%d ability=%d",
                    bench, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        TURN {
            // Plausible scout plus fixed-percent pressure. Identical commands
            // on both boards; the opponent does not read pending commands.
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("JAMES_HAZARD_AFTER bench=%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S toxicSpikes=%d/%d sideFlags=%d/%d remainingPlayerBench=%d ownFaints=%d foeFaints=%d status=%d/%d foeStatus=%d/%d",
            bench, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount, gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount,
            gSideStatuses[B_SIDE_PLAYER], gSideStatuses[B_SIDE_OPPONENT], CountUsablePartyMons(B_BATTLER_0),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1);
        Test_MgbaPrintf("JAMES_HAZARD_PP bench=%d Venipede=%d/%d/%d/%d Kricketune=%d/%d/%d/%d",
            bench, opponentLeft->pp[0], opponentLeft->pp[1], opponentLeft->pp[2], opponentLeft->pp[3],
            opponentRight->pp[0], opponentRight->pp[1], opponentRight->pp[2], opponentRight->pp[3]);
        // A selected/executed layer establishes only this board's choice. This
        // one-turn observation does not prove later switches or poison payoff.
    }
}
