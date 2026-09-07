#include "global.h"
#include "test/battle.h"
#include "champions_circuit.h"
#include "event_data.h"
#include "pokemon.h"
#include "constants/vars.h"

// Prepare through production's transient opponent/stat path, then let the
// battle harness transport that party through the actual controllers.
static void PrepareCircuitBattleLevel(u8 level)
{
    struct Pokemon *recorded = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0];
    struct Pokemon *opponent = &gParties[B_TRAINER_OPPONENT_A][0];
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
    *opponent = *recorded;
    SetMonData(opponent, MON_DATA_LEVEL, &level);
    CalculateMonStats(opponent);
    *recorded = *opponent;
}

DOUBLE_BATTLE_TEST("Champions Circuit level 255 reaches the damage engine through battle controllers")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MACHAMP) { Moves(MOVE_SEISMIC_TOSS); }
        OPPONENT(SPECIES_WOBBUFFET);
        PrepareCircuitBattleLevel(255);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_SEISMIC_TOSS, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(playerLeft, damage: 255);
    } THEN {
        EXPECT_EQ(opponentLeft->level, 255);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 255);
        VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
    }
}

DOUBLE_BATTLE_TEST("Champions Circuit Mega Evolution retains the overlevel battle handicap")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GARCHOMP) { Item(ITEM_GARCHOMPITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        PrepareCircuitBattleLevel(150);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE, gimmick: GIMMICK_MEGA);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_GARCHOMP_MEGA);
        EXPECT_EQ(opponentLeft->level, 150);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 150);
        VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
    }
}
