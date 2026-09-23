#include "global.h"
#include "item.h"
#include "pokemon.h"
#include "battle_pyramid_bag.h"
#include "malloc.h"
#include "event_data.h"
#include "test/test.h"

TEST("PC items: quantity checks include split stacks")
{
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    EXPECT(AddPCItem(ITEM_POTION, MAX_BAG_ITEM_CAPACITY + 1));
    EXPECT(CheckPCHasItem(ITEM_POTION, MAX_BAG_ITEM_CAPACITY + 1));
    EXPECT(!CheckPCHasItem(ITEM_POTION, MAX_BAG_ITEM_CAPACITY + 2));
    EXPECT(!CheckPCHasItem(ITEM_NONE, 1));
    EXPECT(!CheckPCHasItem(ITEM_POTION, 0));
}

TEST("PC items: removal compacts only emptied slots and preserves remaining quantities")
{
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    EXPECT(AddPCItem(ITEM_POTION, 2));
    EXPECT(AddPCItem(ITEM_ANTIDOTE, 3));
    RemovePCItem(0, 1);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].itemId, ITEM_POTION);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].quantity, 1);
    RemovePCItem(0, 1);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].itemId, ITEM_ANTIDOTE);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].quantity, 3);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[1].itemId, ITEM_NONE);
    EXPECT_EQ(CountUsedPCItemSlots(), 1);
    RemovePCItem(PC_ITEMS_COUNT, 1);
    RemovePCItem(0, 4);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].quantity, 3);
}

TEST("Item delivery: split-stack capacity and Berry limits are atomic")
{
    u32 kind = 0, quantity = 1;
    bool32 empty = FALSE;
    static const u16 requests[] = {1, 2, 1000, 1001};
    for (u32 k = 0; k < 3; k++)
        for (u32 e = 0; e < 2; e++)
            for (u32 q = 0; q < ARRAY_COUNT(requests); q++)
                PARAMETRIZE { kind = k; empty = e; quantity = requests[q]; }
    struct BagPocket pc = {.id = POCKET_DUMMY, .capacity = PC_ITEMS_COUNT,
        .primaryCapacity = PC_ITEMS_COUNT, .itemSlots = gSaveBlock1Ptr->pcItems};
    enum Item target = kind == 1 ? ITEM_ORAN_BERRY : ITEM_POTION;
    enum Item filler = kind == 1 ? ITEM_CHERI_BERRY : ITEM_ANTIDOTE;
    struct BagPocket *pocket = kind == 2 ? &pc : &gBagPockets[GetItemPocket(target)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, filler, MAX_BAG_ITEM_CAPACITY);
    BagPocket_SetSlotItemIdAndCount(pocket, 0, target, 998);
    if (empty)
        BagPocket_SetSlotItemIdAndCount(pocket, 1, ITEM_NONE, 0);
    bool32 fits = quantity <= (empty && kind != 1 ? 1000 : 1);
    bool32 delivered = kind == 2 ? AddPCItem(target, quantity) : AddBagItem(target, quantity);
    EXPECT_EQ(delivered, fits);
    struct ItemSlot first = BagPocket_GetSlotData(pocket, 0);
    struct ItemSlot second = BagPocket_GetSlotData(pocket, 1);
    EXPECT_EQ(first.itemId, target);
    EXPECT_EQ(first.quantity, fits ? 999 : 998);
    EXPECT_EQ(second.itemId, fits && quantity > 1 ? target : empty ? ITEM_NONE : filler);
    EXPECT_EQ(second.quantity, fits && quantity > 1 ? quantity - 1 : empty ? 0 : MAX_BAG_ITEM_CAPACITY);
    for (u32 i = 2; i < pocket->capacity; i++)
    {
        struct ItemSlot slot = BagPocket_GetSlotData(pocket, i);
        EXPECT_EQ(slot.itemId, filler);
        EXPECT_EQ(slot.quantity, MAX_BAG_ITEM_CAPACITY);
    }
}

TEST("Item delivery: oversized existing stacks are preserved rather than counted as free space")
{
    ClearBag();
    struct BagPocket *pocket = &gBagPockets[POCKET_BERRIES];
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY + 1);
    EXPECT_EQ(GetFreeSpaceForItemInBag(ITEM_ORAN_BERRY), 0);
    EXPECT(!AddBagItem(ITEM_ORAN_BERRY, 1));
    EXPECT_EQ(GetBagItemQuantity(POCKET_BERRIES, 0), MAX_BAG_ITEM_CAPACITY + 1);
    EXPECT_EQ(GetBagItemId(POCKET_BERRIES, 1), ITEM_NONE);
}

TEST("Pyramid items: large additions split before narrowing quantities")
{
    u32 mode = 0;
    for (u32 m = 0; m < FRONTIER_LVL_MODE_COUNT; m++)
        PARAMETRIZE { mode = m; }
    memset(&gSaveBlock2Ptr->frontier.pyramidBag, 0, sizeof(gSaveBlock2Ptr->frontier.pyramidBag));
    gSaveBlock2Ptr->frontier.lvlMode = mode;
    EXPECT(AddPyramidBagItem(ITEM_POTION, 256));
    u32 total = 0;
    for (u32 i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        u32 quantity = gSaveBlock2Ptr->frontier.pyramidBag.quantity[mode][i];
        EXPECT(quantity <= MAX_PYRAMID_BAG_ITEM_CAPACITY);
        if (gSaveBlock2Ptr->frontier.pyramidBag.itemId[mode][i] == ITEM_POTION)
            total += quantity;
    }
    EXPECT_EQ(total, 256);
}

TEST("Pyramid items: Cancel cursor cannot remove from the other difficulty inventory")
{
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    memset(bag, 0, sizeof(*bag));
    gSaveBlock2Ptr->frontier.lvlMode = 0;
    bag->itemId[0][0] = bag->itemId[1][0] = ITEM_POTION;
    bag->quantity[0][0] = bag->quantity[1][0] = 1;
    gPyramidBagMenuState.cursorPosition = PYRAMID_BAG_ITEMS_COUNT;
    gPyramidBagMenuState.scrollPosition = 0;
    EXPECT(RemovePyramidBagItem(ITEM_POTION, 1));
    EXPECT_EQ(bag->itemId[0][0], ITEM_NONE);
    EXPECT_EQ(bag->quantity[0][0], 0);
    EXPECT_EQ(bag->itemId[1][0], ITEM_POTION);
    EXPECT_EQ(bag->quantity[1][0], 1);
}

TEST("Pyramid items: transactions preserve ordering and atomicity without heap memory")
{
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    memset(bag, 0, sizeof(*bag));
    gSaveBlock2Ptr->frontier.lvlMode = 0;
    bag->itemId[0][8] = ITEM_POTION;
    bag->quantity[0][8] = MAX_PYRAMID_BAG_ITEM_CAPACITY - 1;
    gPyramidBagMenuState.cursorPosition = 8;
    gPyramidBagMenuState.scrollPosition = 0;
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    bool32 added = AddPyramidBagItem(ITEM_POTION, 2);
    bool32 ordered = bag->quantity[0][8] == MAX_PYRAMID_BAG_ITEM_CAPACITY
        && bag->itemId[0][0] == ITEM_POTION && bag->quantity[0][0] == 1;
    bool32 removed = RemovePyramidBagItem(ITEM_POTION, 2);
    bool32 selectedFirst = bag->quantity[0][8] == MAX_PYRAMID_BAG_ITEM_CAPACITY - 2
        && bag->quantity[0][0] == 1;
    struct PyramidBag before = *bag;
    bool32 addFailed = !AddPyramidBagItem(ITEM_POTION, PYRAMID_BAG_ITEMS_COUNT * MAX_PYRAMID_BAG_ITEM_CAPACITY + 1);
    bool32 removeFailed = !RemovePyramidBagItem(ITEM_POTION, PYRAMID_BAG_ITEMS_COUNT * MAX_PYRAMID_BAG_ITEM_CAPACITY);
    bool32 atomic = memcmp(&before, bag, sizeof(before)) == 0;
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    EXPECT(added && ordered && removed && selectedFirst);
    EXPECT(addFailed && removeFailed && atomic);
}

TEST("Pyramid items: invalid difficulty rejects access without changing either inventory")
{
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    memset(bag, 0, sizeof(*bag));
    struct PyramidBag before = *bag;
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_MODE_COUNT;
    FlagSet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG);
    bool32 rejected = !CheckBagHasItem(ITEM_POTION, 1) && !CheckBagHasSpace(ITEM_POTION, 1)
        && !AddPyramidBagItem(ITEM_POTION, 1) && !RemovePyramidBagItem(ITEM_POTION, 1);
    FlagClear(FLAG_STORING_ITEMS_IN_PYRAMID_BAG);
    gSaveBlock2Ptr->frontier.lvlMode = 0;
    EXPECT(rejected);
    EXPECT_EQ(memcmp(&before, bag, sizeof(before)), 0);
}

TEST("Pyramid transfer: held items move atomically without heap or routing flags")
{
    u32 freeSlots = 0;
    bool32 routingFlag = FALSE;
    for (u32 slots = 0; slots <= FRONTIER_PARTY_SIZE; slots++)
        for (u32 flag = 0; flag < 2; flag++)
            PARAMETRIZE { freeSlots = slots; routingFlag = flag; }
    static const enum Item held[] = {ITEM_POTION, ITEM_ETHER, ITEM_REVIVE};
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    memset(bag, 0, sizeof(*bag));
    ClearBag();
    ZeroPlayerPartyMons();
    gSaveBlock2Ptr->frontier.lvlMode = 0;
    for (u32 i = 0; i < FRONTIER_PARTY_SIZE; i++)
    {
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 5, i, OTID_STRUCT_PLAYER_ID);
        SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM, &held[i]);
    }
    for (u32 i = freeSlots; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        bag->itemId[0][i] = ITEM_ANTIDOTE;
        bag->quantity[0][i] = MAX_PYRAMID_BAG_ITEM_CAPACITY;
    }
    bag->itemId[1][0] = ITEM_FULL_RESTORE;
    bag->quantity[1][0] = 7;
    struct PyramidBag before = *bag;
    if (routingFlag)
        FlagSet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG);
    else
        FlagClear(FLAG_STORING_ITEMS_IN_PYRAMID_BAG);
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    TryStoreHeldItemsInPyramidBag();
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    FlagClear(FLAG_STORING_ITEMS_IN_PYRAMID_BAG);
    bool32 success = freeSlots == FRONTIER_PARTY_SIZE;
    EXPECT_EQ(gSpecialVar_Result, success ? 0 : 1);
    if (!success)
        EXPECT_EQ(memcmp(&before, bag, sizeof(before)), 0);
    for (u32 i = 0; i < FRONTIER_PARTY_SIZE; i++)
    {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HELD_ITEM), success ? ITEM_NONE : held[i]);
        EXPECT_EQ(CountTotalItemQuantityInBag(held[i]), 0);
        if (success)
        {
            EXPECT_EQ(bag->itemId[0][i], held[i]);
            EXPECT_EQ(bag->quantity[0][i], 1);
        }
    }
    EXPECT_EQ(bag->itemId[1][0], ITEM_FULL_RESTORE);
    EXPECT_EQ(bag->quantity[1][0], 7);
}
