#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Stance Change keeps Blade Forme and its stats after a non-King's Shield status move")
{
    GIVEN {
        PLAYER(SPECIES_AEGISLASH_SHIELD) { Ability(ABILITY_STANCE_CHANGE); Moves(MOVE_SHADOW_BALL, MOVE_KINGS_SHIELD, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STANCE_CHANGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BALL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_AEGISLASH_BLADE);
        EXPECT_GT(player->attack, player->defense);
    }
}
