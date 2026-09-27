#include "global.h"
#include "item.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "test/test.h"

extern void Test_StoragePlaceMon(struct Pokemon *mon, bool32 fromParty, bool32 toParty);

static void ResetStorageItems(void)
{
    ClearBag();
    ZeroPlayerPartyMons();
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            ZeroBoxMonAt(box, slot);
    gPokemonStoragePtr->currentBox = 0;
}

TEST("Storage items: depositing returns exactly one held item to its Bag pocket")
{
    bool32 capture = FALSE;
    enum Item item = ITEM_NONE;
    for (u32 route = 0; route < 2; route++)
    {
        PARAMETRIZE { capture = route; item = ITEM_LEFTOVERS; }
        PARAMETRIZE { capture = route; item = ITEM_SITRUS_BERRY; }
        PARAMETRIZE { capture = route; item = ITEM_CHARIZARDITE_X; }
    }
    ResetStorageItems();
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(AddBagItem(item, 2));
    if (capture)
        EXPECT_EQ(CopyMonToPC(&mon), MON_GIVEN_TO_PC);
    else
        Test_StoragePlaceMon(&mon, TRUE, FALSE);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_EEVEE);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 3);
    // Moving that stored Pokemon again cannot return a second copy.
    BoxMonAtToMon(0, 0, &mon);
    Test_StoragePlaceMon(&mon, FALSE, FALSE);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 3);
}

TEST("Storage items: a full Bag preserves the held item and still stores the Pokemon")
{
    bool32 capture;
    PARAMETRIZE { capture = FALSE; }
    PARAMETRIZE { capture = TRUE; }
    ResetStorageItems();
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(item)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_CHOICE_BAND, MAX_BAG_ITEM_CAPACITY);
    if (capture)
        EXPECT_EQ(CopyMonToPC(&mon), MON_GIVEN_TO_PC);
    else
        Test_StoragePlaceMon(&mon, TRUE, FALSE);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_EEVEE);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM), item);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 0);
}

TEST("Storage items: party moves and box rearrangement retain held items")
{
    bool32 fromParty, toParty;
    PARAMETRIZE { fromParty = TRUE; toParty = TRUE; }
    PARAMETRIZE { fromParty = FALSE; toParty = TRUE; }
    PARAMETRIZE { fromParty = FALSE; toParty = FALSE; }
    ResetStorageItems();
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    Test_StoragePlaceMon(&mon, fromParty, toParty);
    struct BoxPokemon *placed = toParty ? &gParties[B_TRAINER_PLAYER][0].box : GetBoxedMonPtr(0, 0);
    EXPECT_EQ(GetBoxMonData(placed, MON_DATA_HELD_ITEM), item);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 0);
}

TEST("Storage items: returning a form item updates the stored Pokemon's form")
{
    bool32 capture;
    PARAMETRIZE { capture = FALSE; }
    PARAMETRIZE { capture = TRUE; }
    ResetStorageItems();
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_GIRATINA_ORIGIN, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_GRISEOUS_CORE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    if (capture)
        EXPECT_EQ(CopyMonToPC(&mon), MON_GIVEN_TO_PC);
    else
        Test_StoragePlaceMon(&mon, TRUE, FALSE);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_GIRATINA_ALTERED);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 1);
}

TEST("Storage items: failed storage leaves the Pokemon and Bag untouched")
{
    ResetStorageItems();
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            SetBoxMonAt(box, slot, &mon.box);
    EXPECT_EQ(CopyMonToPC(&mon), MON_CANT_GIVE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), item);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 0);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_HELD_ITEM), item);
}

TEST("Storage items: empty hands and Mail are not added to the Bag")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_ORANGE_MAIL; }
    ResetStorageItems();
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!ReturnBoxMonHeldItemToBag(&mon.box));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), item);
    if (item != ITEM_NONE)
        EXPECT_EQ(CountTotalItemQuantityInBag(item), 0);
}
