#include "global.h"
#include "battle_util.h"
#include "test/battle.h"
#include "item.h"

SINGLE_BATTLE_TEST("Regenerator: destroyed berries are tracked without enabling Recycle")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_BUG_BITE; }
    PARAMETRIZE { move = MOVE_INCINERATE; }
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); SpAttack(1); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->originalBerryDestroyed, TRUE);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_0)->usedHeldItem, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Regenerator: Cud Chew heals twice with or without the tool; only postbattle restoration is gated")
{
    bool32 tool = FALSE;
    PARAMETRIZE { tool = FALSE; }
    PARAMETRIZE { tool = TRUE; }
    GIVEN {
        if (tool)
            GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_FARIGIRAF) { Ability(ABILITY_CUD_CHEW); HP(120); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 180); // 120 - 40 damage + 50 Sitrus + 50 Cud Chew.
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_0)->usedHeldItem, ITEM_NONE);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        // The test battle is recorded; evaluate normal campaign restoration.
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        EXPECT(AddBagItem(ITEM_REGENERATOR, 1));
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
        ClearBag();
    }
}

SINGLE_BATTLE_TEST("Regenerator: Knock Off returns a Recycled berry after battle")
{
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { HP(120); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_SPLASH, MOVE_RECYCLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE, MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_RECYCLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->originalBerryConsumed);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: a transferred original berry stays consumed by its new holder")
{
    enum Move transfer;
    PARAMETRIZE { transfer = MOVE_TRICK; }
    PARAMETRIZE { transfer = MOVE_BESTOW; }
    GIVEN {
        PLAYER(SPECIES_MEW) { HP(200); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(transfer); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(200); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, transfer); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_1)->usedHeldItem, ITEM_SITRUS_BERRY);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        EXPECT(AddBagItem(ITEM_REGENERATOR, 1));
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
        ClearBag();
    }
}

SINGLE_BATTLE_TEST("Regenerator: identical swapped berries retain separate original owners")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { HP(200); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_TRICK, MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(120); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_SITRUS_BERRY);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        EXPECT(GetBattlerPartyState(B_BATTLER_1)->originalBerryRemoved);
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: transferred berry recovery clears its original owner's consumption")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { HP(200); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_TRICK, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(200); Attack(1); Moves(MOVE_CELEBRATE, MOVE_RECYCLE, MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_RECYCLE); }
        TURN { MOVE(player, MOVE_TRICK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->originalBerryConsumed);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: destroying a transferred berry charges its original owner")
{
    enum Move destroy;
    PARAMETRIZE { destroy = MOVE_INCINERATE; }
    PARAMETRIZE { destroy = MOVE_BUG_BITE; }
    GIVEN {
        PLAYER(SPECIES_MEW) { Item(ITEM_SITRUS_BERRY); Attack(1); SpAttack(1); Moves(MOVE_TRICK, destroy); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, destroy); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->originalBerryDestroyed);
        EXPECT(!GetBattlerPartyState(B_BATTLER_1)->originalBerryDestroyed);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: stealing and eating a berry records the opposing original owner")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_THIEF; }
    PARAMETRIZE { move = MOVE_COVET; }
    GIVEN {
        PLAYER(SPECIES_MEW) { HP(100); MaxHP(200); Attack(1); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT(GetBattlerPartyState(B_BATTLER_1)->originalBerryConsumed);
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->originalBerryConsumed);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: berry destruction respects Sticky Hold and activates Unburden")
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_BUG_BITE; ability = ABILITY_STICKY_HOLD; }
    PARAMETRIZE { move = MOVE_BUG_BITE; ability = ABILITY_UNBURDEN; }
    PARAMETRIZE { move = MOVE_PLUCK; ability = ABILITY_STICKY_HOLD; }
    PARAMETRIZE { move = MOVE_PLUCK; ability = ABILITY_UNBURDEN; }
    PARAMETRIZE { move = MOVE_INCINERATE; ability = ABILITY_STICKY_HOLD; }
    PARAMETRIZE { move = MOVE_INCINERATE; ability = ABILITY_UNBURDEN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); SpAttack(1); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->item, ability == ABILITY_STICKY_HOLD ? ITEM_SITRUS_BERRY : ITEM_NONE);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->originalBerryDestroyed, ability != ABILITY_STICKY_HOLD);
        EXPECT_EQ((u32)player->volatiles.unburdenActive, ability == ABILITY_UNBURDEN);
        EXPECT_EQ(GetBattlerPartyState(B_BATTLER_0)->usedHeldItem, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Regenerator: Knock Off returns berries while theft and destruction remove them")
{
    enum Move removal = MOVE_KNOCK_OFF;
    bool32 tool = FALSE;
    for (u32 owned = 0; owned < 2; owned++)
    {
        PARAMETRIZE { removal = MOVE_KNOCK_OFF; tool = owned; }
        PARAMETRIZE { removal = MOVE_THIEF; tool = owned; }
        PARAMETRIZE { removal = MOVE_COVET; tool = owned; }
        PARAMETRIZE { removal = MOVE_TRICK; tool = owned; }
        PARAMETRIZE { removal = MOVE_SWITCHEROO; tool = owned; }
        PARAMETRIZE { removal = MOVE_BUG_BITE; tool = owned; }
        PARAMETRIZE { removal = MOVE_PLUCK; tool = owned; }
    }
    GIVEN {
        if (tool)
            GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_SNORLAX) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEW) { Attack(1); Moves(removal); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, removal); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->originalBerryRemoved, removal != MOVE_KNOCK_OFF);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), removal == MOVE_KNOCK_OFF ? ITEM_SITRUS_BERRY : ITEM_NONE);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: getting a stolen berry back preserves it normally")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Attack(1); Moves(MOVE_CELEBRATE, MOVE_THIEF); }
        OPPONENT(SPECIES_MEW) { HP(400); MaxHP(400); Attack(1); Moves(MOVE_THIEF, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THIEF); }
        TURN { MOVE(player, MOVE_THIEF); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_SITRUS_BERRY);
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->originalBerryRemoved);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Regenerator: a corroded berry is lost, not restored after battle")
{
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_SALANDIT) { Moves(MOVE_CORROSIVE_GAS); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_CORROSIVE_GAS); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT(AddBagItem(ITEM_REGENERATOR, 1));
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
        ClearBag();
    }
}

SINGLE_BATTLE_TEST("Regenerator: a foe's Pickup keeps an eaten berry from regenerating")
{
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { HP(120); MaxHP(200); Item(ITEM_SITRUS_BERRY); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
        ClearBag();
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT(AddBagItem(ITEM_REGENERATOR, 1));
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
        ClearBag();
    }
}
