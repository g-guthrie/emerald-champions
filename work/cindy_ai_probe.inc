// Temporary actual-party E0008 observations, not a permanent trainer lock.
#line 48000 "work/cindy_ai_probe.inc"
static void CindyLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review Cindy leads: physical special pressure and native Aroma Veil")
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
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_THUNDERBOLT, MOVE_FLAMETHROWER, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
            .statPoints = {32, 0, 0, 32, 2, 0},
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
        CindyLeadPlayer(scenario == 0 ? SPECIES_TIMBURR : scenario == 1 ? SPECIES_CHARMANDER : SPECIES_MIENFOO,
                        scenario == 0 ? &timburr : scenario == 1 ? &charmander : &mienfoo);
        CindyLeadPlayer(scenario == 0 ? SPECIES_EEVEE : scenario == 1 ? SPECIES_ELECTRIKE : SPECIES_PACHIRISU,
                        scenario == 0 ? &eevee : scenario == 1 ? &electrike : &pachirisu);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("CINDY_INPUT c=%d owner=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                    scenario, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                if (scenario == 2 && turn == 0)
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                else
                    MOVE(playerLeft, scenario == 1 ? MOVE_FLAMETHROWER : MOVE_DRAIN_PUNCH,
                         target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, scenario == 0 ? MOVE_DOUBLE_EDGE : MOVE_THUNDERBOLT,
                     target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
    } THEN {
        Test_MgbaPrintf("CINDY_AFTER c=%d turns=%d species=%d/%d HP=%d/%d foeHP=%d/%d LAST=%S/%S taunt=%d/%d foeAtk=%d/%d party=%d/%d",
            scenario, turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
    }
}
