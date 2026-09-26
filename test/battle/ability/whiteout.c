#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Whiteout boosts Ice moves by 50% in snow and hail", s16 damage)
{
    enum Ability ability;
    enum Move weatherMove;

    PARAMETRIZE { ability = ABILITY_SNOW_CLOAK; weatherMove = MOVE_SNOWSCAPE; }
    PARAMETRIZE { ability = ABILITY_WHITEOUT; weatherMove = MOVE_SNOWSCAPE; }
    PARAMETRIZE { ability = ABILITY_SNOW_CLOAK; weatherMove = MOVE_HAIL; }
    PARAMETRIZE { ability = ABILITY_WHITEOUT; weatherMove = MOVE_HAIL; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_ICE_BEAM) == TYPE_ICE);
        PLAYER(SPECIES_GLACEON) { Ability(ability); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, weatherMove); MOVE(player, MOVE_ICE_BEAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, weatherMove, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Whiteout does nothing without snow or hail, or for non-Ice moves", s16 damage)
{
    enum Ability ability;
    enum Move weatherMove, move;

    PARAMETRIZE { ability = ABILITY_SNOW_CLOAK; weatherMove = MOVE_CELEBRATE; move = MOVE_ICE_BEAM; }
    PARAMETRIZE { ability = ABILITY_WHITEOUT; weatherMove = MOVE_CELEBRATE; move = MOVE_ICE_BEAM; }
    PARAMETRIZE { ability = ABILITY_SNOW_CLOAK; weatherMove = MOVE_SNOWSCAPE; move = MOVE_SHADOW_BALL; }
    PARAMETRIZE { ability = ABILITY_WHITEOUT; weatherMove = MOVE_SNOWSCAPE; move = MOVE_SHADOW_BALL; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_SHADOW_BALL) != TYPE_ICE);
        PLAYER(SPECIES_GLACEON) { Ability(ability); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, weatherMove); MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
