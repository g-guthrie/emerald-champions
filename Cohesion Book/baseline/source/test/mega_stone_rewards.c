#include "global.h"
#include "event_data.h"
#include "item.h"
#include "mega_stone_rewards.h"
#include "test/test.h"
#include "constants/emerald_champions.h"

TEST("Mega berry trades accept mixed garden berries and charge once")
{
    enum Item item;
    u16 flag;
    PARAMETRIZE { item = ITEM_BAXCALIBRITE; flag = FLAG_EC_BERRY_TRADE_BAXCALIBRITE; }
    PARAMETRIZE { item = ITEM_DRAGONINITE; flag = FLAG_EC_BERRY_TRADE_DRAGONINITE; }
    PARAMETRIZE { item = ITEM_TYRANITARITE; flag = FLAG_EC_BERRY_TRADE_TYRANITARITE; }
    ClearBag();
    FlagClear(flag);
    EXPECT(AddBagItem(ITEM_RAZZ_BERRY, 12));
    EXPECT(AddBagItem(ITEM_NANAB_BERRY, 8));
    EXPECT(AddBagItem(ITEM_ORAN_BERRY, 30));
    CountEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, 20);
    EXPECT_EQ(gSpecialVar_0x8005, EC_MEGA_BERRY_TRADE_COST);
    gSpecialVar_0x8004 = item;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT(FlagGet(flag));
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_RAZZ_BERRY), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_NANAB_BERRY), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ORAN_BERRY), 30);
    EXPECT(AddBagItem(ITEM_PINAP_BERRY, 20));
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_PINAP_BERRY), 20);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 1);
    FlagClear(flag);
    ClearBag();
}

TEST("Mega berry trades leave insufficient berries and reward flags untouched")
{
    ClearBag();
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    EXPECT(AddBagItem(ITEM_RAZZ_BERRY, 19));
    gSpecialVar_0x8004 = ITEM_BAXCALIBRITE;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_NOT_ENOUGH);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_RAZZ_BERRY), 19);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BAXCALIBRITE), 0);
    ClearBag();
}

TEST("Mega berry trades cannot use free battle berries or request a different item")
{
    ClearBag();
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    EXPECT(AddBagItem(ITEM_LUM_BERRY, 100));
    EXPECT(AddBagItem(ITEM_SITRUS_BERRY, 100));
    CountEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, 0);
    gSpecialVar_0x8004 = ITEM_BAXCALIBRITE;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_NOT_ENOUGH);
    EXPECT(AddBagItem(ITEM_RAZZ_BERRY, 20));
    gSpecialVar_0x8004 = ITEM_POTION;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_INVALID);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_RAZZ_BERRY), 20);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POTION), 0);
    ClearBag();
}

TEST("Mega berry trades preserve payment when the reward pocket is full")
{
    ClearBag();
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    EXPECT(AddBagItem(ITEM_POMEG_BERRY, 11));
    EXPECT(AddBagItem(ITEM_KELPSY_BERRY, 9));
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BAXCALIBRITE)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_DRAGONINITE, 1);
    gSpecialVar_0x8004 = ITEM_BAXCALIBRITE;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_BAG_FULL);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POMEG_BERRY), 11);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_KELPSY_BERRY), 9);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POMEG_BERRY), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_KELPSY_BERRY), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BAXCALIBRITE), 1);
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    ClearBag();
}
