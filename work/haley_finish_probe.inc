// Temporary E0013 observations. Include after Haley lead and Wish helpers.
// Player commands repeat the observed two-turn boards; no trainer action is
// prescribed. Reserve Natu was natively defeated on turn two in this board.
#line 56000 "work/haley_finish_probe.inc"
#include "main.h"
#include "battle_ai_switch.h"

AI_DOUBLE_BATTLE_TEST("EC live review Haley followthrough: Wish lead and special reserves three turns")
{
    u32 board;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    PARAMETRIZE { board = 2; }
    GIVEN {
        if (board == 0)
            HaleyWishBoard();
        else
        {
            if (board == 1)
            {
                HaleyLeadPlayer(SPECIES_PACHIRISU, &sHaleyPachirisu);
                HaleyLeadPlayer(SPECIES_TIMBURR, &sHaleyTimburr);
            }
            else
            {
                // Match the exact reserve probe menus, not a substitute
                // offensive Pachirisu or an invented native speed value.
                static const struct EmeraldChampionsBattleSet pachi = {
                    .moves = {MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_NUZZLE, MOVE_PROTECT},
                    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
                    .statPoints = {32, 0, 32, 0, 2, 0},
                };
                static const struct EmeraldChampionsBattleSet natu = {
                    .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
                    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
                    .statPoints = {32, 0, 0, 32, 2, 0},
                };
                HaleyLeadPlayer(SPECIES_PACHIRISU, &pachi);
                HaleyLeadPlayer(SPECIES_NATU, &natu);
            }
            AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
        }
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MARILL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        if (board == 2)
        {
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
        }
    } WHEN {
        for (u32 turn = 0; turn < 3; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (board == 0)
                MOVE(playerRight, MOVE_PROTECT);
            else if (board == 1)
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (turn < 2)
                MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                SKIP_TURN(playerRight);
        }
    } THEN {
        Test_MgbaPrintf("HALEY_FINISH board=%d turns=%d species=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S wish=%d/%d abilities=%d/%d faints=%d/%d",
            board, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            gBattleStruct->wish[B_BATTLER_1].counter, gBattleStruct->wish[B_BATTLER_3].counter,
            opponentLeft->ability, opponentRight->ability,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
                Test_MgbaPrintf("HALEY_FINISH_PARTY board=%d owner=%d slot=%d species=%d HP=%d/%d status=%d",
                    board, owner, slot, GetMonData(&gParties[owner][slot], MON_DATA_SPECIES),
                    GetMonData(&gParties[owner][slot], MON_DATA_HP), GetMonData(&gParties[owner][slot], MON_DATA_MAX_HP),
                    GetMonData(&gParties[owner][slot], MON_DATA_STATUS));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Haley timing: six native player members warm and rebuilt cache")
{
    GIVEN {
        for (u32 slot = 0; slot < 3; slot++)
        {
            HaleyLeadPlayer(SPECIES_PACHIRISU, &sHaleyPachirisu);
            HaleyLeadPlayer(SPECIES_TIMBURR, &sHaleyTimburr);
        }
        AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MARILL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        // No outer snapshot: it would consume memory beyond the actual
        // evaluator's production allocation. These helpers restore internally.
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 warm = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 cold = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 coldFrames = gMain.vblankCounter1 - start;
        Test_MgbaPrintf("HALEY_TIMING selected=%d/%d warmFrames=%d cacheRebuildFrames=%d active=%d/%d HP=%d/%d wish=%d/%d",
            warm, cold, warmFrames, coldFrames, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, gBattleStruct->wish[B_BATTLER_1].counter,
            gBattleStruct->wish[B_BATTLER_3].counter);
    }
}
