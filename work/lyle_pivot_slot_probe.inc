// Temporary candidate role comparison; include after lyle_ai_probe.inc.
#line 57000 "work/lyle_pivot_slot_probe.inc"
#include "emerald_champions_battle_sets.h"

AI_DOUBLE_BATTLE_TEST("EC live review Lyle pivot slot: retain Sucker Punch and compare Protect with U-turn")
{
    u32 board, turns;
    bool32 pivot;
    PARAMETRIZE { board = 0; turns = 2; pivot = FALSE; }
    PARAMETRIZE { board = 0; turns = 2; pivot = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; pivot = FALSE; }
    PARAMETRIZE { board = 1; turns = 2; pivot = TRUE; }
    PARAMETRIZE { board = 2; turns = 2; pivot = FALSE; }
    PARAMETRIZE { board = 2; turns = 2; pivot = TRUE; }
    PARAMETRIZE { board = 0; turns = 3; pivot = FALSE; }
    PARAMETRIZE { board = 0; turns = 3; pivot = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet mienfoo = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
            .statPoints = {18, 16, 0, 0, 0, 32},
        };
        LyleLeadPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_CHARMANDER : SPECIES_MIENFOO,
                       board == 0 ? &eevee : board == 1 ? &charmander : &mienfoo);
        LyleLeadPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU,
                       board == 0 ? &timburr : &pachirisu);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *nymble = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0];
        EXPECT_EQ(GetMonData(nymble, MON_DATA_SPECIES), SPECIES_NYMBLE);
        EXPECT_EQ(GetMonData(nymble, MON_DATA_MOVE1), MOVE_FIRST_IMPRESSION);
        EXPECT_EQ(GetMonData(nymble, MON_DATA_MOVE2), MOVE_LEECH_LIFE);
        EXPECT_EQ(GetMonData(nymble, MON_DATA_MOVE3), MOVE_SUCKER_PUNCH);
        u16 move = pivot ? MOVE_U_TURN : MOVE_PROTECT;
        u8 pp = GetMovePP(move);
        SetMonData(nymble, MON_DATA_MOVE4, &move);
        SetMonData(nymble, MON_DATA_PP4, &pp);
        // All stats, ability, item, other moves and all three native teammates
        // are untouched. This is an offensive Sash-pivot role experiment,
        // not a claim that deleting Protect fixes the shared planning horizon.
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, turn == 0 ? MOVE_FAKE_OUT : MOVE_DRAIN_PUNCH,
                     target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                if (turn == 0)
                    MOVE(playerRight, MOVE_PROTECT);
                else
                    MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("LYLE_PIVOT_SLOT board=%d pivot=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S party=%d/%d",
            board, pivot, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        Test_MgbaPrintf("LYLE_PIVOT_SLOT_PARTY board=%d pivot=%d turns=%d Nymble=%d Sewaddle=%d Anorith=%d Dottler=%d",
            board, pivot, turns, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
    }
}
