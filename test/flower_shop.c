#include "global.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "test/test.h"

TEST("Flower shop: partial bundle failures roll back and a full bundle succeeds")
{
    static const enum Item gifts[] = {
        ITEM_PERSIM_BERRY, ITEM_POMEG_BERRY, ITEM_KELPSY_BERRY,
        ITEM_QUALOT_BERRY, ITEM_HONDEW_BERRY, ITEM_GREPA_BERRY, ITEM_TAMATO_BERRY,
    };
    struct BagPocket *pocket = &gBagPockets[POCKET_BERRIES];
    // Exercise each possible failure position, including six gifts that fit
    // before the last fails. Saturated filler stacks leave exactly n free slots.
    for (u32 freeSlots = 0; freeSlots <= ARRAY_COUNT(gifts); freeSlots++)
    {
        for (u32 slot = 0; slot < pocket->capacity; slot++)
            BagPocket_SetSlotItemIdAndCount(pocket, slot,
                slot < freeSlots ? ITEM_NONE : ITEM_ORAN_BERRY,
                slot < freeSlots ? 0 : MAX_BAG_ITEM_CAPACITY);
        gSpecialVar_0x8004 = ITEM_PERSIM_BERRY;
        GiveFlowerShopBerryBundle();
        bool32 success = freeSlots == ARRAY_COUNT(gifts);
        EXPECT_EQ(gSpecialVar_Result, success);
        for (u32 i = 0; i < ARRAY_COUNT(gifts); i++)
            EXPECT_EQ(CountTotalItemQuantityInBag(gifts[i]), success ? 1 : 0);
    }
    // Rollback must preserve a gift already owned before the attempted delivery.
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY);
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_PERSIM_BERRY, 3);
    gSpecialVar_0x8004 = ITEM_PERSIM_BERRY;
    GiveFlowerShopBerryBundle();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_PERSIM_BERRY), 3);
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_NONE, 0);
}
