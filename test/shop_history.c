#include "global.h"
#include "item.h"
#include "shop.h"
#include "test/test.h"

extern void Test_ClearShopPurchaseHistory(void);
extern void Test_RecordShopPurchase(enum Item item, u16 quantity);

TEST("Shop history: bulk and repeated purchases use the same TV saturation")
{
    u32 quantity;
    PARAMETRIZE { quantity = 1; }
    PARAMETRIZE { quantity = 254; }
    PARAMETRIZE { quantity = 255; }
    PARAMETRIZE { quantity = 256; }
    PARAMETRIZE { quantity = MAX_BAG_ITEM_CAPACITY; }
    Test_ClearShopPurchaseHistory();
    Test_RecordShopPurchase(ITEM_POKE_BALL, quantity);
    EXPECT_EQ(gMartPurchaseHistory[0].quantity, min(quantity, 255));
    Test_RecordShopPurchase(ITEM_POKE_BALL, 1);
    EXPECT_EQ(gMartPurchaseHistory[0].quantity, min(quantity + 1, 255));
}

TEST("Shop history: full records still accumulate existing items and clear for the next shop")
{
    Test_ClearShopPurchaseHistory();
    Test_RecordShopPurchase(ITEM_POKE_BALL, 20);
    Test_RecordShopPurchase(ITEM_POTION, 30);
    Test_RecordShopPurchase(ITEM_ANTIDOTE, 40);
    Test_RecordShopPurchase(ITEM_REPEL, 50);
    Test_RecordShopPurchase(ITEM_POTION, 10);
    EXPECT_EQ(gMartPurchaseHistory[0].itemId, ITEM_POKE_BALL);
    EXPECT_EQ(gMartPurchaseHistory[0].quantity, 20);
    EXPECT_EQ(gMartPurchaseHistory[1].itemId, ITEM_POTION);
    EXPECT_EQ(gMartPurchaseHistory[1].quantity, 40);
    EXPECT_EQ(gMartPurchaseHistory[2].itemId, ITEM_ANTIDOTE);
    EXPECT_EQ(gMartPurchaseHistory[2].quantity, 40);
    Test_ClearShopPurchaseHistory();
    for (u32 i = 0; i < ARRAY_COUNT(gMartPurchaseHistory); i++)
    {
        EXPECT_EQ(gMartPurchaseHistory[i].itemId, ITEM_NONE);
        EXPECT_EQ(gMartPurchaseHistory[i].quantity, 0);
    }
    Test_RecordShopPurchase(ITEM_REPEL, 5);
    EXPECT_EQ(gMartPurchaseHistory[0].itemId, ITEM_REPEL);
    EXPECT_EQ(gMartPurchaseHistory[0].quantity, 5);
}
