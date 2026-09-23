#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Flower Veil: a fainted partner cannot prevent a spread move's stat drop")
{
    u32 veilHp;
    PARAMETRIZE { veilHp = 1; }
    PARAMETRIZE { veilHp = 1000; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(50); SpAttack(200); Speed(200); Moves(MOVE_SNARL); }
        PLAYER(SPECIES_MAGIKARP) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_COMFEY) { Level(50); Ability(ABILITY_FLOWER_VEIL); HP(veilHp); MaxHP(1000); SpDefense(200); Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Level(50); HP(1000); MaxHP(1000); SpDefense(200); Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SNARL); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], veilHp == 1 ? DEFAULT_STAT_STAGE - 1 : DEFAULT_STAT_STAGE);
    }
}
