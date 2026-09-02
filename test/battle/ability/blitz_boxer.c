#include "global.h"
#include "test/battle.h"


SINGLE_BATTLE_TEST("Blitz Boxer gives punching moves one priority stage")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_DRAIN_PUNCH; }
    PARAMETRIZE { move = MOVE_CLOSE_COMBAT; }

    GIVEN {
        ASSUME(IsPunchingMove(MOVE_DRAIN_PUNCH));
        ASSUME(!IsPunchingMove(MOVE_CLOSE_COMBAT));
        PLAYER(SPECIES_HITMONCHAN) { Ability(ABILITY_BLITZ_BOXER); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        if (move == MOVE_DRAIN_PUNCH) {
            MESSAGE("Hitmonchan used Drain Punch!");
            MESSAGE("The opposing Wobbuffet used Celebrate!");
        } else {
            MESSAGE("The opposing Wobbuffet used Celebrate!");
            MESSAGE("Hitmonchan used Close Combat!");
        }
    }
}
