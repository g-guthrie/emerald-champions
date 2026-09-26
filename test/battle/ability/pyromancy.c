#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Pyromancy makes a Fire move's burn five times as likely")
{
    PASSES_RANDOMLY(50, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(GetMoveType(MOVE_FLAMETHROWER) == TYPE_FIRE);
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_FLAMETHROWER, MOVE_EFFECT_BURN, 10));
        PLAYER(SPECIES_DELPHOX) { Ability(ABILITY_PYROMANCY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAMETHROWER, player);
        HP_BAR(opponent);
        STATUS_ICON(opponent, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Pyromancy leaves the burn chance of non-Fire moves unchanged")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(GetMoveType(MOVE_SCALD) == TYPE_WATER);
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_SCALD, MOVE_EFFECT_BURN, 30));
        PLAYER(SPECIES_DELPHOX) { Ability(ABILITY_PYROMANCY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCALD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCALD, player);
        HP_BAR(opponent);
        STATUS_ICON(opponent, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Pyromancy leaves a Fire move's other added effects unchanged")
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT_2);
    GIVEN {
        ASSUME(GetMoveType(MOVE_FIRE_FANG) == TYPE_FIRE);
        ASSUME(GetMoveAdditionalEffectById(MOVE_FIRE_FANG, 1)->moveEffect == MOVE_EFFECT_FLINCH);
        ASSUME(GetMoveAdditionalEffectById(MOVE_FIRE_FANG, 1)->chance == 10);
        PLAYER(SPECIES_DELPHOX) { Ability(ABILITY_PYROMANCY); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_FANG); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_FANG, player);
        MESSAGE("The opposing Wobbuffet flinched and couldn't move!");
    }
}
