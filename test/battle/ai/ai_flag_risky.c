#include "global.h"
#include "test/battle.h"

AI_DOUBLE_BATTLE_TEST("EC expert pair: personality flags preserve an accurate guaranteed KO")
{
    u64 preference;
    PARAMETRIZE { preference = 0; }
    PARAMETRIZE { preference = AI_FLAG_RISKY; }
    PARAMETRIZE { preference = AI_FLAG_CONSERVATIVE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PREDICTION | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE | preference);
        PLAYER(SPECIES_PSYDUCK) { Level(50); HP(20); MaxHP(20); Speed(40); Attack(1); SpDefense(100); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_PSYDUCK) { Level(50); HP(20); MaxHP(20); Speed(30); Attack(1); SpDefense(100); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_CASTFORM) { Level(50); HP(300); MaxHP(300); Speed(100); SpAttack(100); Moves(MOVE_THUNDER, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(300); MaxHP(300); Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_THUNDERBOLT);
            EXPECT_MOVE(opponentRight, MOVE_CELEBRATE);
        }
    }
}
