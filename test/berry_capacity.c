#include "global.h"
#include "item.h"
#include "event_data.h"
#include "test/test.h"

TEST("Berry capacity: preflight matches single-stack delivery without partial awards")
{
    FlagClear(FLAG_STORING_ITEMS_IN_PYRAMID_BAG);
    ClearBag();
    EXPECT_EQ(GetFreeSpaceForItemInBag(ITEM_ORAN_BERRY), MAX_BAG_ITEM_CAPACITY);
    EXPECT(!CheckBagHasSpace(ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY + 1));
    EXPECT(!AddBagItem(ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY + 1));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ORAN_BERRY), 0);
    EXPECT(AddBagItem(ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY - 1));
    EXPECT_EQ(GetFreeSpaceForItemInBag(ITEM_ORAN_BERRY), 1);
    EXPECT(!CheckBagHasSpace(ITEM_ORAN_BERRY, 2));
    EXPECT(!AddBagItem(ITEM_ORAN_BERRY, 2));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ORAN_BERRY), MAX_BAG_ITEM_CAPACITY - 1);
    EXPECT(CheckBagHasSpace(ITEM_ORAN_BERRY, 1));
    EXPECT(AddBagItem(ITEM_ORAN_BERRY, 1));
    EXPECT_EQ(GetFreeSpaceForItemInBag(ITEM_ORAN_BERRY), 0);
    EXPECT(!CheckBagHasSpace(ITEM_ORAN_BERRY, 1));
    // Other pockets still permit split stacks.
    EXPECT(GetFreeSpaceForItemInBag(ITEM_POTION) > MAX_BAG_ITEM_CAPACITY);
    EXPECT(CheckBagHasSpace(ITEM_POTION, MAX_BAG_ITEM_CAPACITY + 1));
    EXPECT(AddBagItem(ITEM_POTION, MAX_BAG_ITEM_CAPACITY + 1));
    ClearBag();
}
