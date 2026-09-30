#include "global.h"
#include "test/battle.h"

// E0148: Eli's Oranguru Instructed its partner, and on the next turn its own
// Psychic printed its name and did nothing at all.
DOUBLE_BATTLE_TEST("The Instruct user's own move the next turn resolves normally")
{
    enum Move first;
    PARAMETRIZE { first = MOVE_INSTRUCT; }
    PARAMETRIZE { first = MOVE_CELEBRATE; }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_INSTRUCT) == EFFECT_INSTRUCT);
        PLAYER(SPECIES_POLITOED) { Speed(40); }
        PLAYER(SPECIES_GYARADOS) { Speed(50); }
        OPPONENT(SPECIES_ORANGURU) { Speed(20); }
        OPPONENT(SPECIES_BRONZONG) { Speed(30); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentLeft, first, target: opponentRight);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_PSYCHIC, target: playerRight);
        }
    } SCENE {
        MESSAGE("The opposing Oranguru used Psychic!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponentLeft);
        HP_BAR(playerRight);
    }
}
