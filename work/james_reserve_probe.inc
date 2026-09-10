// Temporary current-party E0011 reserve observations, not a trainer lock.
#line 21000 "work/james_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void JamesReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review James reserves: native physical special and Speed Boost continuation")
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
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .evs = {252, 0, 0, 252, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .evs = {252, 0, 0, 252, 4, 0},
        };
        JamesReservePlayer(special ? SPECIES_NATU : SPECIES_EEVEE, special ? &natu : &eevee);
        JamesReservePlayer(special ? SPECIES_CHARMANDER : SPECIES_TIMBURR, special ? &charmander : &timburr);
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
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
        // Actual reserves with native moves/items/abilities/stats. Both original
        // leads are fainted, not erased; no synthetic weather is carried over.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JAMES_RESERVE_INPUT special=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d item=%d",
                    special, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK),
                    GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonAbility(mon), GetMonData(mon, MON_DATA_HELD_ITEM));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                MOVE(playerLeft, special ? MOVE_PSYCHIC : MOVE_DOUBLE_EDGE,
                    target: special ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, special ? MOVE_FLAMETHROWER : MOVE_ICE_PUNCH,
                    target: special ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // No opponent command, hazard, KO or survival is forced.
            }
        }
    } THEN {
        Test_MgbaPrintf("JAMES_RESERVE_AFTER special=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S speedStages=%d/%d foeSpeedStages=%d/%d ownFaints=%d foeFaints=%d status=%d/%d foeStatus=%d/%d weather=%d",
            special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1, gBattleWeather);
        Test_MgbaPrintf("JAMES_RESERVE_HAZARDS special=%d turns=%d sideFlags=%d/%d spikes=%d/%d toxicSpikes=%d/%d",
            special, turns, gSideStatuses[B_SIDE_PLAYER], gSideStatuses[B_SIDE_OPPONENT],
            gSideTimers[B_SIDE_PLAYER].spikesAmount, gSideTimers[B_SIDE_OPPONENT].spikesAmount,
            gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount, gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount);
        Test_MgbaPrintf("JAMES_RESERVE_DROPS special=%d turns=%d foeSpA=%d/%d",
            special, turns, playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK]);
        Test_MgbaPrintf("JAMES_RESERVE_HAZARDS_RECHECK special=%d turns=%d sideFlags=%d/%d spikes=%d/%d toxicSpikes=%d/%d",
            special, turns, gSideStatuses[B_SIDE_PLAYER], gSideStatuses[B_SIDE_OPPONENT],
            gSideTimers[B_SIDE_PLAYER].spikesAmount, gSideTimers[B_SIDE_OPPONENT].spikesAmount,
            gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount, gSideTimers[B_SIDE_OPPONENT].toxicSpikesAmount);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("JAMES_RESERVE_PARTY special=%d turns=%d slot=%d species=%d HP=%d status=%d",
                special, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
