// Temporary actual AI on the native injured-drain board; after reserve helper.
#line 28000 "work/tiana_drain_ai_probe.inc"
#include "battle_util.h"

AI_DOUBLE_BATTLE_TEST("EC live review Tiana drain AI: actual injured attacker continuation")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_FIST,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        TianaReservePlayer(SPECIES_TIMBURR, &timburr);
        TianaReservePlayer(SPECIES_PACHIRISU, &sTianaReservePachirisu);
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *swirlix = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1];
        EXPECT_EQ(GetMonData(swirlix, MON_DATA_SPECIES), SPECIES_SWIRLIX);
        u16 hp = GetBerryActivationThreshold(GetMonData(swirlix, MON_DATA_MAX_HP),
            2, GetMonAbility(swirlix), ITEM_SITRUS_BERRY) + 1;
        SetMonData(swirlix, MON_DATA_HP, &hp);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_PROTECT);
                // All actual trainer decisions and replacements unrestricted.
            }
        }
    } THEN {
        Test_MgbaPrintf("TIANA_DRAIN_AI turns=%d LAST=%S/%S party=%d/%d HP=%d/%d originalSwirlixHP=%d foeHP=%d/%d sleep=%d/%d item=%d unburden=%d",
            turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp, playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            opponentRight->item, opponentRight->volatiles.unburdenActive);
    }
}
