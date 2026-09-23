#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Pressure PP: opposing abilities stack, allies do not, and depletion never underflows")
{
    u32 foes = 0, initial = 10;
    for (u32 count = 0; count <= 2; count++)
    {
        PARAMETRIZE { foes = count; initial = 10; }
        PARAMETRIZE { foes = count; initial = 2; }
    }
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { MovesWithPP({MOVE_GROWL, initial}); }
        PLAYER(SPECIES_DUSKNOIR) { Ability(ABILITY_PRESSURE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DUSKNOIR) { Ability(foes > 0 ? ABILITY_PRESSURE : ABILITY_FRISK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DUSKNOIR) { Ability(foes > 1 ? ABILITY_PRESSURE : ABILITY_FRISK); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_GROWL); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        u32 expected = initial > 1 + foes ? initial - 1 - foes : 0;
        EXPECT_EQ(playerLeft->pp[0], expected);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), expected);
    }
}
