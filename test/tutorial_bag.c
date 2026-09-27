#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "event_data.h"
#include "item.h"
#include "item_menu.h"
#include "palette.h"
#include "task.h"
#include "test/test.h"

extern bool32 Test_PrepareInitialToolsBagTutorial(void);
extern void Test_RestoreTutorialBag(bool32 completed);
extern u8 Test_BeginWallyBagClose(u8 *listTaskId);

static void TutorialBagTestCallback(void)
{
}

TEST("Guided Bag: catching restores original cursors only after native list teardown")
{
    MainCallback savedCallback = gMain.callback2;
    struct PaletteFadeControl savedFade = gPaletteFade;
    enum Item savedItem = gSpecialVar_ItemId;
    ClearBag();
    EXPECT(AddBagItem(ITEM_POKE_BALL, 9));
    EXPECT(AddBagItem(ITEM_GREAT_BALL, 2));
    gBagPosition.location = ITEMMENULOCATION_SHOP;
    gBagPosition.pocket = POCKET_MEDICINE;
    gBagPosition.exitCallback = TutorialBagTestCallback;
    gBagPosition.pocketSwitchArrowPos = 3;
    for (u32 pocket = 0; pocket < POCKETS_COUNT; pocket++)
    {
        gBagPosition.cursorPosition[pocket] = pocket + 1;
        gBagPosition.scrollPosition[pocket] = pocket + 4;
    }
    struct BagPosition position = gBagPosition;
    gSpecialVar_ItemId = ITEM_POTION;
    u8 listTaskId;
    u8 taskId = Test_BeginWallyBagClose(&listTaskId);
    gPaletteFade.active = FALSE;
    gTasks[taskId].func(taskId);
    // Starting the fade must not destroy the list or restore the Bag early.
    EXPECT(gTasks[listTaskId].isActive);
    EXPECT_EQ(gBagPosition.location, ITEMMENULOCATION_WALLY);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POKE_BALL), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GREAT_BALL), 0);
    gPaletteFade.active = TRUE;
    gTasks[taskId].func(taskId);
    EXPECT(gTasks[listTaskId].isActive);
    gPaletteFade.active = FALSE;
    gTasks[taskId].func(taskId);
    EXPECT(!gTasks[listTaskId].isActive);
    EXPECT(!gTasks[taskId].isActive);
    EXPECT_EQ(gBagPosition.location, ITEMMENULOCATION_WALLY);
    // Run the actual post-close callback, stopping before battle re-entry.
    gMain.callback2();
    EXPECT_EQ(memcmp(&gBagPosition, &position, sizeof(position)), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POKE_BALL), 9);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GREAT_BALL), 2);
    EXPECT_EQ(gSpecialVar_ItemId, ITEM_POKE_BALL);
    EXPECT_EQ(gMain.callback2, CB2_SetUpReshowBattleScreenAfterMenu2);
    ClearBag();
    memset(&gBagPosition, 0, sizeof(gBagPosition));
    gMain.callback2 = savedCallback;
    gPaletteFade = savedFade;
    gSpecialVar_ItemId = savedItem;
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
