// Temporary current-party Cindy reserve diagnostics; after AuthoredOpponent.
#line 49000 "work/cindy_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void CindyReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sCindyReserveTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sCindyReserveLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ROUND, MOVE_ICE_BEAM, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 136, 120, 0, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Cindy reserves: physical special and Protect into Truant continuation")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 2; turns = 2; }
    PARAMETRIZE { board = 2; turns = 3; }
    GIVEN {
        for (u32 slot = 0; slot < 2; slot++)
            if (board == 1)
                CindyReservePlayer(SPECIES_LOTAD, &sCindyReserveLotad);
            else
                CindyReservePlayer(SPECIES_TIMBURR, &sCindyReserveTimburr);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MILCERY);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_FURFROU);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_FUR_COAT);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SLAKOTH);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_TRUANT);
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
        if (board == 1)
            EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED),
                GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("CINDY_RESERVE_INPUT board=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    board, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK),
                    GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
        // Actual early species, legal prepared moves, native level14/IV31.
        // No original reserve stat/item/move edits and no fabricated speed.
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                if (board == 2 && turn == 0)
                {
                    MOVE(playerLeft, MOVE_PROTECT);
                    MOVE(playerRight, MOVE_PROTECT);
                }
                else
                {
                    MOVE(playerLeft, board == 1 ? MOVE_GIGA_DRAIN : MOVE_ICE_PUNCH,
                        target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    MOVE(playerRight, board == 1 ? MOVE_GIGA_DRAIN : MOVE_ICE_PUNCH,
                        target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                }
            }
        }
    } THEN {
        Test_MgbaPrintf("CINDY_RESERVE_AFTER board=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foeHP=%d/%d LAST=%S/%S foeLAST=%S/%S choice=%S truant=%d foeSpA=%d/%d ownStatus=%d/%d foeStatus=%d/%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            GetMoveName(gBattleStruct->choicedMove[B_BATTLER_3]), opponentRight->volatiles.truantCounter,
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            Test_MgbaPrintf("CINDY_SLAKOTH_PP board=%d turns=%d slot=%d move=%S PP=%d",
                board, turns, slot, GetMoveName(opponentRight->moves[slot]), opponentRight->pp[slot]);
        Test_MgbaPrintf("CINDY_RESERVE_PARTY FurfrouHP=%d SlakothHP=%d activeParty=%d/%d",
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        // No desired opponent move is enforced. Decision-time trace distinguishes
        // selected actions from execution; unchanged PP on the second turn plus
        // native Truant state/outcome is not replaced by a guessed T2 command.
        // With both leads genuinely fainted, U-turn cannot restore Intimidate;
        // this probe does not certify a live-bench pivot or full-battle victory.
    }
}
