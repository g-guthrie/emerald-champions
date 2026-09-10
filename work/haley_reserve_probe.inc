// Temporary actual-party E0013 reserves; no invented Wish or lead support.
#line 52000 "work/haley_reserve_probe.inc"

static void HaleyReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Haley reserves: physical and special pressure")
{
    bool32 special;
    u32 turns;
    PARAMETRIZE { special = FALSE; turns = 1; }
    PARAMETRIZE { special = FALSE; turns = 2; }
    PARAMETRIZE { special = TRUE; turns = 1; }
    PARAMETRIZE { special = TRUE; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet pachi = {
            .moves = {MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_NUZZLE, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .evs = {252, 0, 0, 252, 4, 0},
        };
        HaleyReservePlayer(special ? SPECIES_PACHIRISU : SPECIES_EEVEE, special ? &pachi : &eevee);
        HaleyReservePlayer(special ? SPECIES_NATU : SPECIES_TIMBURR, special ? &natu : &timburr);
        AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MARILL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
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
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("HALEY_RESERVE_INPUT special=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d item=%d",
                    special, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonAbility(mon), GetMonData(mon, MON_DATA_HELD_ITEM));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                MOVE(playerLeft, special ? MOVE_THUNDERBOLT : MOVE_DOUBLE_EDGE, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, special ? MOVE_PSYCHIC : MOVE_ICE_PUNCH, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
    } THEN {
        Test_MgbaPrintf("HALEY_RESERVE_AFTER special=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownFaints=%d foeFaints=%d foeAtk=%d/%d foeStatus=%d/%d",
            special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            playerLeft->status1, playerRight->status1);
    }
}
