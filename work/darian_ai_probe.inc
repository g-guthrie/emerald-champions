// Temporary actual-party investigation, not a permanent trainer lock.
#line 41000 "work/darian_ai_probe.inc"
static void DarianLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Darian leads: native pressure speed control and Grass counterplay")
{
    u32 scenario, turns;
    PARAMETRIZE { scenario = 0; turns = 1; }
    PARAMETRIZE { scenario = 0; turns = 2; }
    PARAMETRIZE { scenario = 1; turns = 1; }
    PARAMETRIZE { scenario = 1; turns = 2; }
    PARAMETRIZE { scenario = 2; turns = 1; }
    PARAMETRIZE { scenario = 2; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet mienfoo = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
            .statPoints = {18, 16, 0, 0, 0, 32},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet treecko = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_QUICK_GUARD, MOVE_ENERGY_BALL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_OVERGROW,
            .statPoints = {2, 0, 0, 32, 0, 32},
        };
        DarianLeadPlayer(scenario == 0 ? SPECIES_TIMBURR : scenario == 1 ? SPECIES_MIENFOO : SPECIES_TREECKO,
                         scenario == 0 ? &timburr : scenario == 1 ? &mienfoo : &treecko);
        DarianLeadPlayer(scenario == 1 ? SPECIES_PACHIRISU : SPECIES_EEVEE,
                         scenario == 1 ? &pachirisu : &eevee);
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("DARIAN_INPUT c=%d owner=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                    scenario, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                MOVE(playerLeft, scenario == 2 ? MOVE_GIGA_DRAIN : MOVE_DRAIN_PUNCH,
                     target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, scenario == 1 ? MOVE_SUPER_FANG : MOVE_DOUBLE_EDGE,
                     target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
    } THEN {
        Test_MgbaPrintf("DARIAN_AFTER c=%d turns=%d species=%d/%d HP=%d/%d foes=%d/%d foeStatus=%d/%d LAST=%S/%S AtkStage=%d/%d SpeedStage=%d/%d party=%d/%d",
            scenario, turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, playerLeft->status1, playerRight->status1,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
    }
}
