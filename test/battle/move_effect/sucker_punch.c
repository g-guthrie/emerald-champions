#include "global.h"
#include "test/battle.h"

// Sucker Punch reads the command its target chose. A Fake Out that lands
// first makes the target flinch only when its own turn comes, so the chosen
// attack still stands when Sucker Punch strikes.
DOUBLE_BATTLE_TEST("Sucker Punch hits a target that its partner's Fake Out will flinch")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SUCKER_PUNCH) == EFFECT_SUCKER_PUNCH);
        PLAYER(SPECIES_GYARADOS) { Speed(40); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); }
        OPPONENT(SPECIES_PERRSERKER) { Speed(30); }
        OPPONENT(SPECIES_CACTURNE) { Speed(20); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WATERFALL, target: opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft);
            MOVE(opponentRight, MOVE_SUCKER_PUNCH, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUCKER_PUNCH, opponentRight);
        HP_BAR(playerLeft);
        MESSAGE("Gyarados flinched and couldn't move!");
    }
}
