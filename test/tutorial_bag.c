#include "global.h"
#include "event_data.h"
#include "item.h"
#include "item_menu.h"
#include "test/test.h"

extern bool32 Test_PrepareInitialToolsBagTutorial(void);
extern void Test_RestoreTutorialBag(bool32 completed);

static void TutorialBagTestCallback(void)
{
}

TEST("Guided Bag: completion and abort restore full pockets, cursors and selection")
{
    bool32 completed = FALSE;
    PARAMETRIZE { completed = FALSE; }
    PARAMETRIZE { completed = TRUE; }
    ClearBag();
    struct ItemSlot keyItems[BAG_KEYITEMS_COUNT];
    for (u32 slot = 0; slot < BAG_KEYITEMS_COUNT; slot++)
    {
        keyItems[slot] = (struct ItemSlot){slot & 1 ? ITEM_BICYCLE : ITEM_DEVON_SCOPE, 1};
        BagPocket_SetSlotData(&gBagPockets[POCKET_KEY_ITEMS], slot, keyItems[slot]);
    }
    // The presentation can show a tool delivered to the PC without taking it.
    struct ItemSlot pcItem = {ITEM_POKE_VIAL, 1};
    gSaveBlock1Ptr->pcItems[0] = pcItem;
    BagPocket_SetSlotItemIdAndCount(&gBagPockets[POCKET_MEDICINE], 3, ITEM_POTION, 7);
    gBagPosition.location = ITEMMENULOCATION_SHOP;
    gBagPosition.pocket = POCKET_MEDICINE;
    gBagPosition.exitCallback = TutorialBagTestCallback;
    gBagPosition.pocketSwitchArrowPos = 2;
    for (u32 pocket = 0; pocket < POCKETS_COUNT; pocket++)
    {
        gBagPosition.cursorPosition[pocket] = pocket;
        gBagPosition.scrollPosition[pocket] = 2 * pocket;
    }
    struct BagPosition position = gBagPosition;
    gSpecialVar_ItemId = ITEM_POTION;
    EXPECT(Test_PrepareInitialToolsBagTutorial());
    static const enum Item shown[] = {ITEM_POKE_VIAL, ITEM_LEVELER, ITEM_REGENERATOR, ITEM_REPEL_SPRAY, ITEM_FLIGHT_BEACON};
    for (u32 slot = 0; slot < BAG_KEYITEMS_COUNT; slot++)
    {
        EXPECT_EQ(GetBagItemId(POCKET_KEY_ITEMS, slot), slot < ARRAY_COUNT(shown) ? shown[slot] : ITEM_NONE);
        EXPECT_EQ(GetBagItemQuantity(POCKET_KEY_ITEMS, slot), slot < ARRAY_COUNT(shown) ? 1 : 0);
    }
    // Simulate native selection and exit overwriting the transient Bag state.
    memset(&gBagPosition, 0, sizeof(gBagPosition));
    gSpecialVar_ItemId = ITEM_LEVELER;
    Test_RestoreTutorialBag(completed);
    EXPECT_EQ(gSpecialVar_Result, completed);
    EXPECT_EQ(gSpecialVar_ItemId, ITEM_POTION);
    EXPECT_EQ(memcmp(&gBagPosition, &position, sizeof(position)), 0);
    for (u32 slot = 0; slot < BAG_KEYITEMS_COUNT; slot++)
    {
        struct ItemSlot restored = GetBagItemIdAndQuantity(POCKET_KEY_ITEMS, slot);
        EXPECT_EQ(restored.itemId, keyItems[slot].itemId);
        EXPECT_EQ(restored.quantity, keyItems[slot].quantity);
    }
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].itemId, pcItem.itemId);
    EXPECT_EQ(gSaveBlock1Ptr->pcItems[0].quantity, pcItem.quantity);
    EXPECT_EQ(GetBagItemId(POCKET_MEDICINE, 3), ITEM_POTION);
    EXPECT_EQ(GetBagItemQuantity(POCKET_MEDICINE, 3), 7);
    ClearBag();
    memset(&gBagPosition, 0, sizeof(gBagPosition));
    gSaveBlock1Ptr->pcItems[0] = (struct ItemSlot){0};
}
