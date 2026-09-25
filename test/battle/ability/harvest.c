#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_KNOCK_OFF) == EFFECT_KNOCK_OFF);
    ASSUME(GetMoveEffect(MOVE_SUPER_FANG) == EFFECT_FIXED_PERCENT_DAMAGE);
    ASSUME(GetItemHoldEffect(ITEM_SITRUS_BERRY) == HOLD_EFFECT_RESTORE_PCT_HP);
}

SINGLE_BATTLE_TEST("Harvest restores a Berry that was eaten")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EXEGGUTOR) { Ability(ABILITY_HARVEST); Item(ITEM_SITRUS_BERRY); HP(300); MaxHP(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_SUPER_FANG); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        ABILITY_POPUP(opponent, ABILITY_HARVEST);
        MESSAGE("The opposing Exeggutor harvested its Sitrus Berry!");
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
    }
}

SINGLE_BATTLE_TEST("Harvest does not restore a Berry removed by Knock Off")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EXEGGUTOR) { Ability(ABILITY_HARVEST); Item(ITEM_SITRUS_BERRY); HP(300); MaxHP(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_KNOCK_OFF); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet knocked off the opposing Exeggutor's Sitrus Berry!");
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_HARVEST);
            MESSAGE("The opposing Exeggutor harvested its Sitrus Berry!");
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Harvest does not restore a harvested Berry after Knock Off removes it")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EXEGGUTOR) { Ability(ABILITY_HARVEST); Item(ITEM_SITRUS_BERRY); HP(300); MaxHP(300); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_SUPER_FANG); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_KNOCK_OFF); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        // Turn 1: eaten, then harvested back.
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        MESSAGE("The opposing Exeggutor harvested its Sitrus Berry!");
        // Turn 2: the harvested Berry is knocked off, which is not consumption.
        MESSAGE("Wobbuffet knocked off the opposing Exeggutor's Sitrus Berry!");
        NONE_OF {
            MESSAGE("The opposing Exeggutor harvested its Sitrus Berry!");
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

// The benchmark shape: the Berry is eaten to an earlier hit in the same turn,
// so Knock Off finds nothing to remove and Harvest may restore the eaten Berry.
DOUBLE_BATTLE_TEST("Harvest restores a Berry eaten before Knock Off lands")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(30); }
        PLAYER(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_EXEGGUTOR) { Ability(ABILITY_HARVEST); Item(ITEM_SITRUS_BERRY); HP(300); MaxHP(300); Speed(40); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_SUNNY_DAY);
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, playerLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, playerRight);
        NOT MESSAGE("Wynaut knocked off the opposing Exeggutor's Sitrus Berry!");
        ABILITY_POPUP(opponentLeft, ABILITY_HARVEST);
        MESSAGE("The opposing Exeggutor harvested its Sitrus Berry!");
    } THEN {
        EXPECT_EQ(opponentLeft->item, ITEM_SITRUS_BERRY);
    }
}
