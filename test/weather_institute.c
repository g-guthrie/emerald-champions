#include "global.h"
#include "field_specials.h"
#include "item.h"
#include "test/test.h"

TEST("Weather Institute: rock gift space accounts for the complete delivery")
{
    static const enum Item rocks[] = {
        ITEM_HEAT_ROCK, ITEM_DAMP_ROCK, ITEM_ICY_ROCK, ITEM_SMOOTH_ROCK,
    };
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_HEAT_ROCK)];

    for (u32 i = 0; i < ARRAY_COUNT(rocks); i++)
        EXPECT_EQ(GetItemPocket(rocks[i]), pocket->id);

    // No partial grant when only zero, one, two or three new stacks fit.
    for (u32 freeSlots = 0; freeSlots <= ARRAY_COUNT(rocks); freeSlots++)
    {
        for (u32 slot = 0; slot < pocket->capacity; slot++)
            BagPocket_SetSlotItemIdAndCount(pocket, slot,
                slot < freeSlots ? ITEM_NONE : ITEM_CHARCOAL,
                slot < freeSlots ? 0 : MAX_BAG_ITEM_CAPACITY);
        EXPECT_EQ(CanReceiveWeatherInstituteRocks(), freeSlots == ARRAY_COUNT(rocks));
        for (u32 i = 0; i < ARRAY_COUNT(rocks); i++)
            EXPECT_EQ(CountTotalItemQuantityInBag(rocks[i]), 0);
        if (freeSlots == ARRAY_COUNT(rocks))
            for (u32 i = 0; i < ARRAY_COUNT(rocks); i++)
                EXPECT(AddBagItem(rocks[i], 1));
    }

    // A full pocket can still accept the gift into existing non-full stacks.
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_CHARCOAL, MAX_BAG_ITEM_CAPACITY);
    for (u32 i = 0; i < ARRAY_COUNT(rocks); i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, rocks[i], MAX_BAG_ITEM_CAPACITY - 1);
    EXPECT(CanReceiveWeatherInstituteRocks());
    for (u32 i = 0; i < ARRAY_COUNT(rocks); i++)
    {
        EXPECT_EQ(CountTotalItemQuantityInBag(rocks[i]), MAX_BAG_ITEM_CAPACITY - 1);
        EXPECT(AddBagItem(rocks[i], 1));
    }
    EXPECT(!CanReceiveWeatherInstituteRocks());

    // An early hole is consumed before a later matching stack by AddBagItem.
    // Counting empty slots plus all matching stacks would incorrectly pass.
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    BagPocket_SetSlotItemIdAndCount(pocket, 1, ITEM_HEAT_ROCK, 1);
    BagPocket_SetSlotItemIdAndCount(pocket, 2, ITEM_ICY_ROCK, 1);
    BagPocket_SetSlotItemIdAndCount(pocket, 3, ITEM_SMOOTH_ROCK, 1);
    EXPECT(!CanReceiveWeatherInstituteRocks());
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_HEAT_ROCK), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_DAMP_ROCK), 0);

    // Retry succeeds after freeing another slot, without changing the old gift.
    BagPocket_SetSlotItemIdAndCount(pocket, 4, ITEM_NONE, 0);
    EXPECT(CanReceiveWeatherInstituteRocks());
    for (u32 i = 0; i < ARRAY_COUNT(rocks); i++)
        EXPECT(AddBagItem(rocks[i], 1));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_HEAT_ROCK), 2);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_DAMP_ROCK), 1);
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_NONE, 0);
}
