// Temporary actual Lyle reserve diagnostics, not forced strategy assertions.
#line 60000 "work/lyle_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void LyleReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lyle reserves: physical versus special pressure continuation")
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
        static const struct EmeraldChampionsBattleSet munchlax = {
            .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_ROUND, MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_ROUND, MOVE_ICE_BEAM, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_QUIET, .ability = ABILITY_RAIN_DISH,
            .evs = {252, 0, 252, 4, 0, 0},
        };
        LyleReservePlayer(special ? SPECIES_PACHIRISU : SPECIES_EEVEE, special ? &pachirisu : &eevee);
        LyleReservePlayer(special ? SPECIES_LOTAD : SPECIES_MUNCHLAX, special ? &lotad : &munchlax);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_NYMBLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SEWADDLE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_ANORITH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
        u16 hp = 0;
        SetMonData(&party[0], MON_DATA_HP, &hp);
        SetMonData(&party[1], MON_DATA_HP, &hp);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        if (!special)
            EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED),
                GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_SPEED));
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < 2; slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("LYLE_RESERVE_INPUT special=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d item=%d moves=%S/%S/%S/%S",
                    special, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonAbility(mon), GetMonData(mon, MON_DATA_HELD_ITEM),
                    GetMoveName(GetMonData(mon, MON_DATA_MOVE1)), GetMoveName(GetMonData(mon, MON_DATA_MOVE2)),
                    GetMoveName(GetMonData(mon, MON_DATA_MOVE3)), GetMoveName(GetMonData(mon, MON_DATA_MOVE4)));
            }
        // No invented stat values. Route116 Munchlax is a pre-badge matchup,
        // not a claim that every player owns one before meeting Woods Lyle.
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                MOVE(playerLeft, special ? MOVE_ROUND : MOVE_DOUBLE_EDGE, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, special ? MOVE_GIGA_DRAIN : MOVE_BODY_SLAM, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
    } THEN {
        Test_MgbaPrintf("LYLE_RESERVE_AFTER special=%d turns=%d nativeTurns=%d ownSpecies=%d/%d ownHP=%d/%d playerHP=%d/%d LAST=%S/%S foeLAST=%S/%S reflect=%d reflectTimer=%d foeSpA=%d/%d ownStatus=%d/%d",
            special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT) != 0,
            gSideTimers[B_SIDE_OPPONENT].reflectTimer,
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            opponentLeft->status1, opponentRight->status1);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            Test_MgbaPrintf("LYLE_RESERVE_PP slot=%d Anorith=%S:%d Dottler=%S:%d", slot,
                GetMoveName(opponentLeft->moves[slot]), opponentLeft->pp[slot],
                GetMoveName(opponentRight->moves[slot]), opponentRight->pp[slot]);
        // Actual decision trace and native execution are evidence; a retained
        // last move does not prove a fainted/flinched actor acted this turn.
        // No must-Reflect, Helping Hand or attack-name assertion is imposed.
    }
}
