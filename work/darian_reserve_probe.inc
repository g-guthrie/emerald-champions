// Temporary current-party Darian reserve baseline; include after AuthoredOpponent.
#line 42000 "work/darian_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void DarianReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sDarianReservePachirisu = {
    .moves = {MOVE_ROUND, MOVE_LIGHT_SCREEN, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 120, 0, 136, 0},
};
static const struct EmeraldChampionsBattleSet sDarianReserveLotad = {
    .moves = {MOVE_ROUND, MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 120, 0, 136, 0},
};
static const struct EmeraldChampionsBattleSet sDarianReserveChespin = {
    .moves = {MOVE_SEED_BOMB, MOVE_DRAIN_PUNCH, MOVE_SYNTHESIS, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_BULLETPROOF,
    .evs = {252, 252, 4, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sDarianReserveSnivy = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_LEAF_STORM, MOVE_GLARE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_CONTRARY,
    .evs = {252, 0, 4, 252, 0, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Darian reserves: native immediate power setup window and Grass pressure")
{
    u32 board, turns;
    bool32 window;
    PARAMETRIZE { board = 0; turns = 1; window = TRUE; }
    PARAMETRIZE { board = 0; turns = 2; window = TRUE; }
    PARAMETRIZE { board = 0; turns = 1; window = FALSE; }
    PARAMETRIZE { board = 1; turns = 1; window = FALSE; }
    PARAMETRIZE { board = 1; turns = 2; window = TRUE; }
    GIVEN {
        if (board == 0)
        {
            DarianReservePlayer(SPECIES_PACHIRISU, &sDarianReservePachirisu);
            DarianReservePlayer(SPECIES_LOTAD, &sDarianReserveLotad);
        }
        else
        {
            DarianReservePlayer(SPECIES_CHESPIN, &sDarianReserveChespin);
            DarianReservePlayer(SPECIES_SNIVY, &sDarianReserveSnivy);
        }
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CHINCHOU);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_BARBOACH);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CORPHISH);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_ADAPTABILITY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HELD_ITEM), ITEM_CHOICE_BAND);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CLAMPERL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_DEEP_SEA_TOOTH);
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
                Test_MgbaPrintf("DARIAN_RESERVE_INPUT board=%d turns=%d window=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    board, turns, window, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK),
                    GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
        // Native early Pachirisu/Lotad or selectable regional starters. Player
        // preparation moves and stat budgets checked against current source.
        // Original fainted leads remain; reserve stats/moves/items are exact.
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                if (window && turn == 0)
                {
                    MOVE(playerLeft, MOVE_PROTECT);
                    MOVE(playerRight, MOVE_PROTECT);
                }
                else if (board == 0)
                {
                    MOVE(playerLeft, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                }
                else
                {
                    MOVE(playerLeft, MOVE_SEED_BOMB, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                }
                // The AI sees legal opposing sets, not these pending commands.
                // Protect gives an actual observed window, not a must-Smash
                // expectation. Immediate attacks and defensive play stay legal.
            }
        }
    } THEN {
        Test_MgbaPrintf("DARIAN_RESERVE_AFTER board=%d turns=%d window=%d nativeTurns=%d species=%d/%d HP=%d/%d playerHP=%d/%d LAST=%S/%S choice=%S clamStages=%d/%d/%d/%d/%d items=%d/%d status=%d/%d",
            board, turns, window, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gBattleStruct->choicedMove[B_BATTLER_1]),
            opponentRight->statStages[STAT_ATK], opponentRight->statStages[STAT_DEF],
            opponentRight->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPEED],
            opponentLeft->item, opponentRight->item, opponentLeft->status1, opponentRight->status1);
        Test_MgbaPrintf("DARIAN_RESERVE_PARTY corphishHP=%d clamperlHP=%d activeParty=%d/%d",
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        // Decision-time trace supplies actual choices/targets; THEN may already
        // have next-turn caches. Execution PASS is not an optimality verdict.
    }
}
