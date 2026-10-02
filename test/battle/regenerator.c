#include "global.h"
#include "battle_util.h"
#include "test/battle.h"
#include "item.h"
#include "field_specials.h"

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

SINGLE_BATTLE_TEST("Regenerator: a stolen trainer berry returns even after its borrower eats it")
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
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_SITRUS_BERRY);
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

SINGLE_BATTLE_TEST("Regenerator: Knock Off and trainer theft return berries while direct destruction loses them")
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
        PARAMETRIZE { removal = MOVE_INCINERATE; tool = owned; }
    }
    GIVEN {
        if (tool)
            GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_SNORLAX) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEW) { Attack(1); SpAttack(1); Moves(removal); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, removal); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        // Incinerate records destruction rather than removal; both are losses.
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->originalBerryRemoved,
                  removal != MOVE_KNOCK_OFF && removal != MOVE_INCINERATE);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->originalBerryDestroyed,
                  removal == MOVE_INCINERATE || removal == MOVE_BUG_BITE || removal == MOVE_PLUCK);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0),
                  removal == MOVE_KNOCK_OFF || removal == MOVE_THIEF || removal == MOVE_COVET ? ITEM_SITRUS_BERRY : ITEM_NONE);
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

SINGLE_BATTLE_TEST("Regenerator: Magician and Pickpocket borrow trainer berries until cleanup")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MAGICIAN; }
    PARAMETRIZE { ability = ABILITY_PICKPOCKET; }
    GIVEN {
        GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_SNORLAX) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Attack(1); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(400); MaxHP(400); Attack(1); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        if (ability == ABILITY_MAGICIAN)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        else
            TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->originalBerryRemoved);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
    }
}

SINGLE_BATTLE_TEST("Item theft: destroying a borrowed trainer berry still returns it to its owner")
{
    enum Move theft, destroy;
    for (u32 i = 0; i < 2; i++)
    {
        PARAMETRIZE { theft = i ? MOVE_COVET : MOVE_THIEF; destroy = MOVE_INCINERATE; }
        PARAMETRIZE { theft = i ? MOVE_COVET : MOVE_THIEF; destroy = MOVE_BUG_BITE; }
        PARAMETRIZE { theft = i ? MOVE_COVET : MOVE_THIEF; destroy = MOVE_CORROSIVE_GAS; }
    }
    GIVEN {
        PLAYER(SPECIES_MEW) { HP(400); MaxHP(400); Attack(1); Moves(theft, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Attack(1); SpAttack(1); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE, destroy); }
    } WHEN {
        TURN { MOVE(player, theft); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, destroy); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_SITRUS_BERRY);
        TryRestoreHeldItems();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
        gBattleTypeFlags = flags;
    }
}

WILD_BATTLE_TEST("Item theft: wild Magician and Pickpocket deliver permanent Bag loot without shop discovery")
{
    enum Ability ability;
    enum Item item;
    bool32 fullBag;
    for (u32 full = 0; full < 2; full++)
    {
        PARAMETRIZE { ability = ABILITY_MAGICIAN; item = ITEM_THROAT_SPRAY; fullBag = full; }
        PARAMETRIZE { ability = ABILITY_PICKPOCKET; item = ITEM_THROAT_SPRAY; fullBag = full; }
        PARAMETRIZE { ability = ABILITY_MAGICIAN; item = ITEM_SITRUS_BERRY; fullBag = full; }
        PARAMETRIZE { ability = ABILITY_PICKPOCKET; item = ITEM_SITRUS_BERRY; fullBag = full; }
    }
    GIVEN {
        WITH_CONFIG(B_STEAL_WILD_ITEMS, GEN_9);
        ClearBag();
        memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
        if (fullBag)
            FILL_PLAYER_BAG_POCKET(item, ITEM_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(400); MaxHP(400); Attack(1); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        // Wild opponents choose randomly even in recorded test battles. One
        // contact move makes Pickpocket's trigger an observable certainty.
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Attack(1); Item(item); Moves(MOVE_TACKLE); }
    } WHEN {
        if (ability == ABILITY_MAGICIAN)
            TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (ability == ABILITY_MAGICIAN)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
            HP_BAR(opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
            HP_BAR(player);
        }
        if (fullBag)
            NOT ABILITY_POPUP(player, ability);
        else
            ABILITY_POPUP(player, ability);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, fullBag ? item : ITEM_NONE);
        EXPECT_EQ(CountTotalItemQuantityInBag(item), fullBag ? MAX_BAG_ITEM_CAPACITY : 1);
        EXPECT_EQ(IsEmeraldChampionsBattleItemUnlocked(item), item == ITEM_SITRUS_BERRY);
        for (u32 byte = 0; byte < sizeof(gSaveBlock1Ptr->battleItemsUnlocked); byte++)
            EXPECT_EQ(gSaveBlock1Ptr->battleItemsUnlocked[byte], 0);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = 0;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), fullBag ? item : ITEM_NONE);
        TryRestoreHeldItems();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
        gBattleTypeFlags = flags;
        ClearBag();
    }
}

WILD_BATTLE_TEST("Item theft: wild ability loot preserves the Regenerator's previously consumed original berry")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MAGICIAN; }
    PARAMETRIZE { ability = ABILITY_PICKPOCKET; }
    GIVEN {
        WITH_CONFIG(B_STEAL_WILD_ITEMS, GEN_9);
        ASSUME(MoveMakesContact(MOVE_SEISMIC_TOSS));
        GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(120); MaxHP(200); Attack(1); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Item(ITEM_THROAT_SPRAY); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SEISMIC_TOSS); }
        if (ability == ABILITY_MAGICIAN)
            TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ability);
    } THEN {
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->originalBerryConsumed);
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_THROAT_SPRAY), 1);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = 0;
        TryRestoreHeldItems();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_NONE);
        gBattleTypeFlags = flags;
    }
}

WILD_BATTLE_TEST("Item theft: Sticky Barb contact remains attached instead of becoming Bag loot")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { ability = ABILITY_MAGICIAN; }
    PARAMETRIZE { ability = ABILITY_PICKPOCKET; }
    GIVEN {
        WITH_CONFIG(B_STEAL_WILD_ITEMS, GEN_9);
        ClearBag();
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); HP(400); MaxHP(400); Attack(1); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Item(ITEM_STICKY_BARB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_STICKY_BARB);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_STICKY_BARB), 0);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = 0;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_STICKY_BARB);
        gBattleTypeFlags = flags;
    }
}

DOUBLE_BATTLE_TEST("Item theft: eating an owned partner's berry does not create a trainer loan refund")
{
    enum Move theft;
    bool32 tool;
    for (u32 owned = 0; owned < 2; owned++)
    {
        PARAMETRIZE { theft = MOVE_THIEF; tool = owned; }
        PARAMETRIZE { theft = MOVE_COVET; tool = owned; }
    }
    GIVEN {
        if (tool)
            GIVE_PLAYER_ITEM(ITEM_REGENERATOR, 1);
        PLAYER(SPECIES_MEW) { HP(100); MaxHP(200); Attack(1); Moves(theft); }
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, theft, target: playerRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 150);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_EQ(playerRight->item, ITEM_NONE);
        u32 flags = gBattleTypeFlags;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
        EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 1), ITEM_NONE);
        TryRestoreHeldItems();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM), ITEM_NONE);
        gBattleTypeFlags = flags;
    }
}
