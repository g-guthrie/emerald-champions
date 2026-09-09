// Temporary actual Joey policy comparison, included after JoeyDrumBoard.
// Plans are a static const trainer table, not a mutable per-battle field.
// Run this identical fixture in root's SETUP and SETUP|PRESSURE builds; log
// the actual plan rather than substitute another trainer ID or fake a flag.
#line 58000 "work/joey_pressure_probe.inc"
#include "emerald_champions_battle_plan.h"

AI_DOUBLE_BATTLE_TEST("EC live review Joey pressure: actual compiled plan on leads and reserves")
{
    bool32 reserves;
    u32 turns;
    PARAMETRIZE { reserves = FALSE; turns = 1; }
    PARAMETRIZE { reserves = FALSE; turns = 2; }
    PARAMETRIZE { reserves = TRUE; turns = 1; }
    PARAMETRIZE { reserves = TRUE; turns = 2; }
    GIVEN {
        if (reserves)
            JoeyDrumBoard(TRUE);
        else
        {
            JoeyPlayer(SPECIES_PACHIRISU, &sJoeyPachirisu);
            JoeyPlayer(SPECIES_TIMBURR, &sJoeyTimburr);
            AuthoredOpponent(TRAINER_JOEY, 0, FALSE);
        }
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[reserves ? 2 : 0], MON_DATA_SPECIES), SPECIES_PAWMI);
        EXPECT_EQ(GetMonData(&party[reserves ? 3 : 1], MON_DATA_SPECIES), SPECIES_LECHONK);
        EXPECT_EQ(GetMonData(&party[reserves ? 0 : 2], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
        EXPECT_EQ(GetMonData(&party[reserves ? 1 : 3], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        if (reserves)
        {
            EXPECT_EQ(GetMonData(&party[2], MON_DATA_HP), 0);
            EXPECT_EQ(GetMonData(&party[3], MON_DATA_HP), 0);
        }
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOEY_PRESSURE_INPUT reserves=%d owner=%d slot=%d species=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    reserves, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                    GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                    GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (reserves)
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        u32 plan = EmeraldChampions_GetBattlePlan(B_BATTLER_1);
        EXPECT(plan == EC_BATTLE_PLAN_SETUP || plan == (EC_BATTLE_PLAN_SETUP | EC_BATTLE_PLAN_PRESSURE));
        EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), plan);
        Test_MgbaPrintf("JOEY_PRESSURE_AFTER plan=%d reserves=%d turns=%d nativeTurns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S Atk=%d/%d status=%d/%d items=%d/%d faints=%d/%d",
            plan, reserves, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->status1, opponentRight->status1, opponentLeft->item, opponentRight->item,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOEY_PRESSURE_PARTY plan=%d reserves=%d turns=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                plan, reserves, turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
        }
        // No pressure flag is injected, no move is required, and double Protect
        // is not automatically a failure. Compare actual native consequences.
    }
}
