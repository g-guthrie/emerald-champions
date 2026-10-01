#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_BRICK_BREAK) == EFFECT_HIT);
}

// E0348: with only Light Screen up, Brick Break also announced an Aurora Veil.
DOUBLE_BATTLE_TEST("Brick Break announces only the screen it removes")
{
    enum Move screen;
    PARAMETRIZE { screen = MOVE_REFLECT; }
    PARAMETRIZE { screen = MOVE_LIGHT_SCREEN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); }
    } WHEN {
        TURN { MOVE(opponentLeft, screen); MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, screen, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BRICK_BREAK, playerLeft);
        if (screen == MOVE_REFLECT)
        {
            MESSAGE("The opposing side's Reflect wore off!");
            NONE_OF {
                MESSAGE("The opposing side's Light Screen wore off!");
                MESSAGE("The opposing side's Aurora Veil wore off!");
            }
        }
        else
        {
            NOT MESSAGE("The opposing side's Reflect wore off!");
            MESSAGE("The opposing side's Light Screen wore off!");
            NOT MESSAGE("The opposing side's Aurora Veil wore off!");
        }
    } THEN {
        EXPECT_EQ(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SCREEN_ANY, 0);
    }
}

SINGLE_BATTLE_TEST("A screen Brick Break removed does not wear off again later")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, MOVE_BRICK_BREAK); }
        TURN { MOVE(opponent, MOVE_LIGHT_SCREEN); MOVE(player, MOVE_BRICK_BREAK); }
        TURN { }
        TURN { }
        TURN { }
    } SCENE {
        MESSAGE("The opposing side's Reflect wore off!");
        MESSAGE("The opposing Wobbuffet used Light Screen!");
        NONE_OF {
            MESSAGE("The opposing side's Reflect wore off!");
            MESSAGE("The opposing side's Light Screen wore off!");
        }
        MESSAGE("Wobbuffet used Brick Break!");
        MESSAGE("The opposing side's Light Screen wore off!");
        NONE_OF {
            MESSAGE("The opposing side's Reflect wore off!");
            MESSAGE("The opposing side's Light Screen wore off!");
        }
    }
}
