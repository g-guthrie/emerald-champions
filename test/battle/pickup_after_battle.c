#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

// Pickup and Honey Gather hand an item to an empty-handed Pokemon as the battle
// ends. The end-of-battle held-item restoration runs after them and must treat
// that item as the holder's own, not wipe it back to the empty hand it started
// the battle with. Trainer battles share the same restoration; the test runner
// only reaches the wild victory script, so these are wild battles.

WILD_BATTLE_TEST("Pickup: an item picked up after the battle is still held afterwards")
{
    PASSES_RANDOMLY(1, 10, RNG_PICKUP_AFTER_BATTLE);
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); Level(5); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(5); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        MESSAGE("The wild Magikarp fainted!");
    } THEN {
        EXPECT_NE(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    }
}

// Force Pickup's successful roll on the knockout turn. Repeating an actual
// battle victory via PASSES_RANDOMLY can re-enter the test before another
// battle runs, so the preservation check uses one deterministic native fight.
WILD_BATTLE_TEST("Pickup: a Berry the Regenerator returns is not replaced by a picked-up item")
{
    GIVEN {
        GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); Speed(1); HP(120); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_DRAGON_RAGE, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(5); HP(40); MaxHP(40); Speed(200); Moves(MOVE_DRAGON_RAGE, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE, WITH_RNG(RNG_PICKUP_AFTER_BATTLE, 1)); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player, hp: 80);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player, hp: 130);
        MESSAGE("The wild Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
    }
}

WILD_BATTLE_TEST("Pickup: Honey Gather's Honey is still held after the battle")
{
    // Level 100 gathers Honey half the time.
    PASSES_RANDOMLY(50, 100, RNG_PICKUP_AFTER_BATTLE);
    GIVEN {
        PLAYER(SPECIES_COMBEE) { Ability(ABILITY_HONEY_GATHER); Level(100); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(5); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        MESSAGE("The wild Magikarp fainted!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_HONEY);
    }
}

WILD_BATTLE_TEST("Ball Fetch: a retrieved Ball remains held after battle")
{
    GIVEN {
        PLAYER(SPECIES_YAMPER) { Ability(ABILITY_BALL_FETCH); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(40); MaxHP(40); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL, WITH_RNG(RNG_BALLTHROW_SHAKE, MAX_u16)); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BALL_FETCH);
        MESSAGE("The wild Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_POKE_BALL);
    }
}

WILD_BATTLE_TEST("Ball Fetch: a returning Berry keeps its place after battle")
{
    GIVEN {
        GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        GIVE_PLAYER_ITEM(ITEM_POKE_BALL, 1);
        PLAYER(SPECIES_YAMPER) { Ability(ABILITY_BALL_FETCH); HP(120); MaxHP(200); Item(ITEM_SITRUS_BERRY); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(40); MaxHP(40); Speed(1); Moves(MOVE_DRAGON_RAGE, MOVE_SPLASH); }
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL, WITH_RNG(RNG_BALLTHROW_SHAKE, MAX_u16)); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player, hp: 130);
        NOT ABILITY_POPUP(player, ABILITY_BALL_FETCH);
        MESSAGE("The wild Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
    }
}
