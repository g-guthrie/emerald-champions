#include "global.h"
#include "test/battle.h"
#include "battle_util.h"
#include "random.h"

DOUBLE_BATTLE_TEST("Battle utility: speed sorting preserves ties and RNG in both directions")
{
    bool32 slowToFast;
    PARAMETRIZE { slowToFast = FALSE; }
    PARAMETRIZE { slowToFast = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            MOVE(opponentLeft, MOVE_SPLASH);
            MOVE(opponentRight, MOVE_SPLASH);
        }
    } THEN {
        enum BattlerId order[] = {B_BATTLER_0, B_BATTLER_2, B_BATTLER_3, B_BATTLER_1};
        rng_value_t rng = gRngValue;
        SortBattlersBySpeed(order, slowToFast);
        EXPECT_EQ(memcmp(&gRngValue, &rng, sizeof(rng)), 0);
        if (slowToFast)
        {
            EXPECT_EQ(order[0], B_BATTLER_3);
            EXPECT_EQ(order[1], B_BATTLER_0);
            EXPECT_EQ(order[2], B_BATTLER_2);
            EXPECT_EQ(order[3], B_BATTLER_1);
        }
        else
        {
            EXPECT_EQ(order[0], B_BATTLER_2);
            EXPECT_EQ(order[1], B_BATTLER_1);
            EXPECT_EQ(order[2], B_BATTLER_0);
            EXPECT_EQ(order[3], B_BATTLER_3);
        }
    }
}
