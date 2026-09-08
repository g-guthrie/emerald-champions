#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Telepathy protects sound-team partners from allied Boomburst")
{
    enum Species species;
    enum Ability ability;

    PARAMETRIZE { species = SPECIES_NOIVERN; ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { species = SPECIES_NOIVERN; ability = ABILITY_INFILTRATOR; }
    PARAMETRIZE { species = SPECIES_GARDEVOIR; ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { species = SPECIES_GARDEVOIR; ability = ABILITY_SYNCHRONIZE; }

    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_SCRAPPY); }
        PLAYER(species) { Ability(ability); HP(900); MaxHP(900); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BOOMBURST);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOOMBURST, playerLeft);
        if (ability == ABILITY_TELEPATHY) {
            NONE_OF { HP_BAR(playerRight); }
        } else {
            HP_BAR(playerRight);
        }
    } THEN {
        // This checks damage prevention, not the timing of an ability popup
        // relative to a spread move's shared animation.
        if (ability == ABILITY_TELEPATHY)
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        else
            EXPECT_LT(playerRight->hp, playerRight->maxHP);
    }
}

DOUBLE_BATTLE_TEST("Telepathy does not protect a sound-team partner from enemy Boomburst")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); }
        PLAYER(SPECIES_NOIVERN) { Ability(ABILITY_TELEPATHY); HP(900); MaxHP(900); }
        OPPONENT(SPECIES_EXPLOUD) { Ability(ABILITY_SCRAPPY); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_BOOMBURST);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BOOMBURST, opponentLeft);
        HP_BAR(playerRight);
        NONE_OF { ABILITY_POPUP(playerRight, ABILITY_TELEPATHY); }
    }
}
