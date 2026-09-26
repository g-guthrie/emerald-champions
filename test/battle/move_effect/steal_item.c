#include "global.h"
#include "test/battle.h"
#include "item.h"
#include "battle_util.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_THIEF) == EFFECT_STEAL_ITEM);
    ASSUME(GetMoveEffect(MOVE_COVET) == EFFECT_STEAL_ITEM);
}

SINGLE_BATTLE_TEST("Thief and Covet steal target's held item")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_HYPER_POTION); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_HYPER_POTION);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Thief and Covet steal player's held item if opponent is a trainer")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        ASSUME(B_TRAINERS_KNOCK_OFF_ITEMS == TRUE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_HYPER_POTION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, player);
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_HYPER_POTION);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

WILD_BATTLE_TEST("Thief and Covet don't steal player's held item if opponent is a wild mon")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        ASSUME(B_TRAINERS_KNOCK_OFF_ITEMS == TRUE);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_HYPER_POTION); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_HYPER_POTION);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Thief and Covet don't steal target's held item if user is holding an item")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_POTION); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_HYPER_POTION); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_POTION);
        EXPECT_EQ(opponent->item, ITEM_HYPER_POTION);
    }
}

SINGLE_BATTLE_TEST("Thief and Covet don't steal target's held item if target has no item")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    }
}

WILD_BATTLE_TEST("Thief and Covet steal target's held item and it's added to Bag in wild battles (Gen 9+)")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        WITH_CONFIG(B_STEAL_WILD_ITEMS, GEN_9);
        ClearBag();
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_HYPER_POTION); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_HYPER_POTION), 1);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = 0; // Same restoration endpoint used when a wild mon is caught.
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
        ClearBag();
    }
}


SINGLE_BATTLE_TEST("Thief and Covet can steal target's held item if user faints before (Champions)")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        WITH_CONFIG(B_FAINT_MOVE_EFFECT_TIMING, GEN_CHAMPIONS);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ROCKY_HELMET); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_ROCKY_HELMET);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Thief and Covet: Berries that activate on HP thresholds are stolen before they can activate")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }

    // Half-HP berries activate at floor(maxHP / 2) + 1 in this game.
    GIVEN {
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(200); HP(102); Item(ITEM_ORAN_BERRY); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_ORAN_BERRY);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Thief and Covet: Berries that activate on a Status activate before the item can be stolen")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }

    GIVEN {
        PLAYER(SPECIES_TOXICROAK) { Ability(ABILITY_POISON_TOUCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LUM_BERRY); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ABILITY_POPUP(player, ABILITY_POISON_TOUCH);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    }
}

WILD_BATTLE_TEST("Thief and Covet leave the wild held item intact when the Bag is full")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        WITH_CONFIG(B_STEAL_WILD_ITEMS, GEN_9);
        GIVE_PLAYER_ITEM(ITEM_SITRUS_BERRY, MAX_BAG_ITEM_CAPACITY);
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_STEAL, opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_SITRUS_BERRY), MAX_BAG_ITEM_CAPACITY);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_1)->heldItemOrigin, B_TRAINER_OPPONENT_A * PARTY_SIZE + 1);
        ClearBag();
    }
}
