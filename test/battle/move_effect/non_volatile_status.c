#include "global.h"
#include "test/battle.h"

// E0339: Will-O-Wisp into a Protect printed "But it failed!" after the
// protect line. Only the protect line belongs there.
DOUBLE_BATTLE_TEST("A status move stopped by Protect does not also fail")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    PARAMETRIZE { move = MOVE_TOXIC; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SPIRITOMB);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(opponentRight, move, target: playerLeft); }
    } SCENE {
        MESSAGE("Wobbuffet protected itself!");
        NONE_OF {
            MESSAGE("But it failed!");
            STATUS_ICON(playerLeft, burn: TRUE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("A status move blocked by a Substitute still fails")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_WILL_O_WISP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, player);
        MESSAGE("The opposing Wobbuffet used Will-O-Wisp!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
