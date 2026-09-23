#include "global.h"
#include "battle.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Campaign doubles: a lone Pokemon fights both opponents and can lose normally")
{
    GIVEN {
        PLAYER(SPECIES_MAGIKARP) { HP(1); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(2); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(3); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentRight);
        HP_BAR(playerLeft, hp: 0);
    } THEN {
        EXPECT_EQ(gBattlersCount, 4);
        EXPECT_EQ(gBattleOutcome, B_OUTCOME_LOST);
        EXPECT(gAbsentBattlerFlags & (1u << B_POSITION_PLAYER_RIGHT));
    }
}

DOUBLE_BATTLE_TEST("Campaign doubles: a lone Pokemon can win by defeating both opponents")
{
    GIVEN {
        PLAYER(SPECIES_GROUDON) { Speed(3); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(2); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE);
            MOVE(opponentLeft, MOVE_SPLASH);
            MOVE(opponentRight, MOVE_SPLASH);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, playerLeft);
        HP_BAR(opponentLeft, hp: 0);
        HP_BAR(opponentRight, hp: 0);
    } THEN {
        EXPECT_EQ(gBattlersCount, 4);
        EXPECT_EQ(gBattleOutcome, B_OUTCOME_WON);
        EXPECT(gAbsentBattlerFlags & (1u << B_POSITION_PLAYER_RIGHT));
    }
}
