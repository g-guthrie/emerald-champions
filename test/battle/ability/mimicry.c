#include "global.h"
#include "test/battle.h"

// Mimicry takes the terrain's type while a terrain is up and keeps the
// species' own types otherwise. E0025 and E0001: Galarian Stunfisk entered a
// field with no terrain, and outlasted an Electric Terrain, and each time
// printed "Stunfisk's type changed to None!" and became typeless.

SINGLE_BATTLE_TEST("Mimicry keeps its own types when it enters a field with no terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_STUNFISK_GALAR) { Ability(ABILITY_MIMICRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); }
    } SCENE {
        MESSAGE("Go! Stunfisk!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MIMICRY);
            MESSAGE("Stunfisk's type changed to None!");
        }
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_GROUND);
        EXPECT_EQ(player->types[1], TYPE_STEEL);
    }
}

SINGLE_BATTLE_TEST("Mimicry returns to its own types when Electric Terrain ends")
{
    GIVEN {
        ASSUME(GetMoveTerrainType(MOVE_ELECTRIC_TERRAIN) == B_TERRAIN_ELECTRIC);
        PLAYER(SPECIES_STUNFISK_GALAR) { Ability(ABILITY_MIMICRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN {}
        TURN {}
        TURN {}
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        ABILITY_POPUP(player, ABILITY_MIMICRY);
        MESSAGE("Stunfisk's type changed to Electric!");
        MESSAGE("The electricity disappeared from the battlefield.");
        NONE_OF {
            MESSAGE("Stunfisk's type changed to None!");
        }
    } THEN {
        EXPECT_EQ(player->types[0], TYPE_GROUND);
        EXPECT_EQ(player->types[1], TYPE_STEEL);
    }
}
