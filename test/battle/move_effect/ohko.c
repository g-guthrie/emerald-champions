#include "global.h"
#include "test/battle.h"

// Owner rule: a one-hit KO hits with its accuracy plus one point per level the
// user leads by, or minus one per level it trails. A higher-level target is
// harder to hit, never immune.
ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_GUILLOTINE) == EFFECT_OHKO);
    ASSUME(GetMoveAccuracy(MOVE_GUILLOTINE) == 30);
    ASSUME(GetMoveEffect(MOVE_SHEER_COLD) == EFFECT_OHKO);
}

SINGLE_BATTLE_TEST("One-hit KO moves lose one point of accuracy per level the target leads by")
{
    PASSES_RANDOMLY(20, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_KINGLER) { Level(50); Moves(MOVE_GUILLOTINE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(60); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUILLOTINE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUILLOTINE, player);
        MESSAGE("It's a one-hit KO!");
    }
}

SINGLE_BATTLE_TEST("One-hit KO moves gain one point of accuracy per level the user leads by")
{
    PASSES_RANDOMLY(40, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_KINGLER) { Level(60); Moves(MOVE_GUILLOTINE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUILLOTINE); }
    } SCENE {
        MESSAGE("It's a one-hit KO!");
    }
}

SINGLE_BATTLE_TEST("One-hit KO moves cannot hit a target 30 or more levels above the user")
{
    GIVEN {
        PLAYER(SPECIES_KINGLER) { Level(20); Moves(MOVE_GUILLOTINE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUILLOTINE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_GUILLOTINE, player);
        MESSAGE("The opposing Wobbuffet avoided the attack!");
    }
}

SINGLE_BATTLE_TEST("Sheer Cold still loses ten more points when the user is not Ice type")
{
    PASSES_RANDOMLY(15, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) != TYPE_ICE);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); Moves(MOVE_SHEER_COLD); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(55); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHEER_COLD); }
    } SCENE {
        MESSAGE("It's a one-hit KO!");
    }
}

SINGLE_BATTLE_TEST("Sturdy still blocks a one-hit KO from a higher-level user")
{
    GIVEN {
        PLAYER(SPECIES_KINGLER) { Level(100); Moves(MOVE_GUILLOTINE); }
        OPPONENT(SPECIES_GEODUDE) { Level(5); Ability(ABILITY_STURDY); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUILLOTINE); }
    } SCENE {
        NOT MESSAGE("It's a one-hit KO!");
        ABILITY_POPUP(opponent, ABILITY_STURDY);
    }
}
