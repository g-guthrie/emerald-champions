#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "item_menu.h"
#include "load_save.h"
#include "pokemon.h"
#include "test/overworld_script.h"
#include "test/test.h"

static const enum Item sEmeraldChampionsMegaStoneArchiveItems[] =
{
#include "../src/data/emerald_champions_mega_stones.h"
};

TEST("Inclement-style bag routes preparation, battle, and Mega items to dedicated pockets")
{
    EXPECT_EQ(GetItemPocket(ITEM_POTION), POCKET_MEDICINE);
    EXPECT_EQ(GetItemPocket(ITEM_RARE_CANDY), POCKET_MEDICINE);
    EXPECT_EQ(GetItemPocket(ITEM_ABILITY_CAPSULE), POCKET_MEDICINE);
    EXPECT_EQ(GetItemPocket(ITEM_LEFTOVERS), POCKET_BATTLE);
    EXPECT_EQ(GetItemPocket(ITEM_X_ATTACK), POCKET_BATTLE);
    EXPECT_EQ(GetItemPocket(ITEM_CHARIZARDITE_X), POCKET_MEGA_STONES);
    EXPECT_EQ(GetItemPocket(ITEM_RED_ORB), POCKET_MEGA_STONES);
    EXPECT_EQ(GetItemPocket(ITEM_FIRE_STONE), POCKET_ITEMS);
}

TEST("Mega pocket holds the complete Emerald Champions archive and both Primal Orbs")
{
    ClearBag();

    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsMegaStoneArchiveItems); i++)
        EXPECT(AddBagItem(sEmeraldChampionsMegaStoneArchiveItems[i], 1));
    EXPECT(AddBagItem(ITEM_RED_ORB, 1));
    EXPECT(AddBagItem(ITEM_BLUE_ORB, 1));

    EXPECT_EQ(ARRAY_COUNT(sEmeraldChampionsMegaStoneArchiveItems) + 2, BAG_MEGASTONES_COUNT);
    EXPECT_EQ(GetBagItemId(POCKET_MEGA_STONES, BAG_MEGASTONES_COUNT - 1), ITEM_BLUE_ORB);
}

TEST("Legacy five-pocket saves migrate items into the eight-pocket layout")
{
    ClearBag();

    gSaveBlock1Ptr->bag.items[0].itemId = ITEM_POTION;
    gSaveBlock1Ptr->bag.items[0].quantity = 3 ^ gSaveBlock2Ptr->encryptionKey;
    gSaveBlock1Ptr->bag.items[1].itemId = ITEM_LEFTOVERS;
    gSaveBlock1Ptr->bag.items[1].quantity = 2 ^ gSaveBlock2Ptr->encryptionKey;
    gSaveBlock1Ptr->bag.items[2].itemId = ITEM_CHARIZARDITE_X;
    gSaveBlock1Ptr->bag.items[2].quantity = 1 ^ gSaveBlock2Ptr->encryptionKey;
    gSaveBlock3Ptr->bagPocketLayoutMagic = 0;
    gSaveBlock3Ptr->bagPocketLayoutMagicInverse = 0;

    MigrateBagPocketsIfNeeded();

    EXPECT(CheckBagHasItem(ITEM_POTION, 3));
    EXPECT(CheckBagHasItem(ITEM_LEFTOVERS, 2));
    EXPECT(CheckBagHasItem(ITEM_CHARIZARDITE_X, 1));
    EXPECT_EQ(GetBagItemId(POCKET_MEDICINE, 0), ITEM_POTION);
    EXPECT_EQ(GetBagItemId(POCKET_BATTLE, 0), ITEM_LEFTOVERS);
    EXPECT_EQ(GetBagItemId(POCKET_MEGA_STONES, 0), ITEM_CHARIZARDITE_X);
}

TEST("Emerald Champions link-battle Bag restore preserves every segmented pocket region")
{
    static const u32 oldKey = 0x12345678;
    static const u32 newKey = 0x89ABCDEF;
    static const struct
    {
        enum Pocket pocket;
        u32 slot;
        enum Item item;
        u16 quantity;
    } cases[] =
    {
        {POCKET_ITEMS,       0,                                         ITEM_FIRE_STONE,    1},
        {POCKET_ITEMS,       BAG_LEGACY_ITEMS_COUNT,                    ITEM_METAL_COAT,    2},
        {POCKET_MEDICINE,    0,                                         ITEM_POTION,        3},
        {POCKET_BATTLE,      0,                                         ITEM_LEFTOVERS,     4},
        {POCKET_TM_HM,       0,                                         ITEM_TM01,           5},
        {POCKET_TM_HM,       BAG_LEGACY_TMHM_COUNT,                     ITEM_TM02,           6},
        {POCKET_BERRIES,     0,                                         ITEM_ORAN_BERRY,     7},
        {POCKET_BERRIES,     BAG_LEGACY_BERRIES_COUNT,                  ITEM_SITRUS_BERRY,   8},
        {POCKET_POKE_BALLS,  0,                                         ITEM_POKE_BALL,      9},
        {POCKET_POKE_BALLS,  BAG_LEGACY_POKEBALLS_COUNT,                ITEM_GREAT_BALL,    10},
        {POCKET_KEY_ITEMS,   0,                                         ITEM_MACH_BIKE,     11},
        {POCKET_KEY_ITEMS,   BAG_LEGACY_KEYITEMS_COUNT,                 ITEM_ACRO_BIKE,     12},
        {POCKET_MEGA_STONES, 0,                                         ITEM_CHARIZARDITE_X, 13},
        {POCKET_MEGA_STONES, BAG_MEGASTONES_PRIMARY_COUNT,              ITEM_VENUSAURITE,   14},
    };

    gSaveBlock2Ptr->encryptionKey = oldKey;
    ClearBag();
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        BagPocket_SetSlotItemIdAndCount(
            &gBagPockets[cases[i].pocket],
            cases[i].slot,
            cases[i].item,
            cases[i].quantity
        );
    }

    LoadPlayerBag();
    ApplyNewEncryptionKeyToBagItems(newKey);
    gSaveBlock2Ptr->encryptionKey = newKey;
    SavePlayerBag();

    EXPECT_EQ(gSaveBlock2Ptr->encryptionKey, newKey);
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        struct ItemSlot slot = BagPocket_GetSlotData(
            &gBagPockets[cases[i].pocket],
            cases[i].slot
        );

        EXPECT_EQ(slot.itemId, cases[i].item);
        EXPECT_EQ(slot.quantity, cases[i].quantity);
    }
}

TEST("TMs and HMs are sorted correctly in the bag")
{
    struct BagPocket *pocket = &gBagPockets[POCKET_TM_HM];

    ASSUME(GetItemPocket(ITEM_HM07) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_TM25) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_TM14) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_TM42) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_HM05) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_TM05) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_TM01) == POCKET_TM_HM);
    ASSUME(GetItemPocket(ITEM_HM02) == POCKET_TM_HM);

    /*
     * Note: I would add a test to make sure that TMs are sorted correctly by move name,
     * but downstream users are likely to rearrange TMs so this would just be a nuisance.
     */

    RUN_OVERWORLD_SCRIPT(
        additem ITEM_HM07;
        additem ITEM_TM25;
        additem ITEM_TM14;
        additem ITEM_TM42;
        additem ITEM_HM05;
        additem ITEM_TM05;
        additem ITEM_TM01;
        additem ITEM_HM02;
    );

    SortItemsInBag(&gBagPockets[POCKET_TM_HM], SORT_BY_INDEX);

    EXPECT_EQ(pocket->itemSlots[0].itemId, ITEM_TM01);
    EXPECT_EQ(pocket->itemSlots[1].itemId, ITEM_TM05);
    EXPECT_EQ(pocket->itemSlots[2].itemId, ITEM_TM14);
    EXPECT_EQ(pocket->itemSlots[3].itemId, ITEM_TM25);
    EXPECT_EQ(pocket->itemSlots[4].itemId, ITEM_TM42);
    EXPECT_EQ(pocket->itemSlots[5].itemId, ITEM_HM02);
    EXPECT_EQ(pocket->itemSlots[6].itemId, ITEM_HM05);
    EXPECT_EQ(pocket->itemSlots[7].itemId, ITEM_HM07);
    EXPECT_EQ(pocket->itemSlots[8].itemId, ITEM_NONE);
}

TEST("Berries are sorted correctly in the bag")
{
    struct BagPocket *pocket = &gBagPockets[POCKET_BERRIES];

    ASSUME(GetItemPocket(ITEM_POMEG_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_MAGOST_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_KELPSY_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_MICLE_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_CHARTI_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_GANLON_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_ORAN_BERRY) == POCKET_BERRIES);
    ASSUME(GetItemPocket(ITEM_CHERI_BERRY) == POCKET_BERRIES);

    RUN_OVERWORLD_SCRIPT(
        additem ITEM_POMEG_BERRY;
        additem ITEM_MAGOST_BERRY;
        additem ITEM_KELPSY_BERRY;
        additem ITEM_MICLE_BERRY;
        additem ITEM_CHARTI_BERRY;
        additem ITEM_GANLON_BERRY;
        additem ITEM_ORAN_BERRY;
        additem ITEM_CHERI_BERRY;
    );

    SortItemsInBag(&gBagPockets[POCKET_BERRIES], SORT_BY_INDEX);

    EXPECT_EQ(pocket->itemSlots[0].itemId, ITEM_CHERI_BERRY);
    EXPECT_EQ(pocket->itemSlots[1].itemId, ITEM_ORAN_BERRY);
    EXPECT_EQ(pocket->itemSlots[2].itemId, ITEM_POMEG_BERRY);
    EXPECT_EQ(pocket->itemSlots[3].itemId, ITEM_KELPSY_BERRY);
    EXPECT_EQ(pocket->itemSlots[4].itemId, ITEM_MAGOST_BERRY);
    EXPECT_EQ(pocket->itemSlots[5].itemId, ITEM_CHARTI_BERRY);
    EXPECT_EQ(pocket->itemSlots[6].itemId, ITEM_GANLON_BERRY);
    EXPECT_EQ(pocket->itemSlots[7].itemId, ITEM_MICLE_BERRY);
    EXPECT_EQ(pocket->itemSlots[8].itemId, ITEM_NONE);

    SortItemsInBag(&gBagPockets[POCKET_BERRIES], SORT_ALPHABETICALLY);

    EXPECT_EQ(pocket->itemSlots[0].itemId, ITEM_CHARTI_BERRY);
    EXPECT_EQ(pocket->itemSlots[1].itemId, ITEM_CHERI_BERRY);
    EXPECT_EQ(pocket->itemSlots[2].itemId, ITEM_GANLON_BERRY);
    EXPECT_EQ(pocket->itemSlots[3].itemId, ITEM_KELPSY_BERRY);
    EXPECT_EQ(pocket->itemSlots[4].itemId, ITEM_MAGOST_BERRY);
    EXPECT_EQ(pocket->itemSlots[5].itemId, ITEM_MICLE_BERRY);
    EXPECT_EQ(pocket->itemSlots[6].itemId, ITEM_ORAN_BERRY);
    EXPECT_EQ(pocket->itemSlots[7].itemId, ITEM_POMEG_BERRY);
    EXPECT_EQ(pocket->itemSlots[8].itemId, ITEM_NONE);
}

TEST("Items are correctly sorted and compacted in the bag")
{
    struct BagPocket *pocket = &gBagPockets[POCKET_ITEMS];
    memset(pocket->itemSlots, 0, sizeof(gSaveBlock1Ptr->bag.items));

    ASSUME(GetItemPocket(ITEM_NUGGET) == POCKET_ITEMS);
    ASSUME(GetItemPocket(ITEM_BIG_NUGGET) == POCKET_ITEMS);
    ASSUME(GetItemPocket(ITEM_TINY_MUSHROOM) == POCKET_ITEMS);
    ASSUME(GetItemPocket(ITEM_BIG_MUSHROOM) == POCKET_ITEMS);
    ASSUME(GetItemPocket(ITEM_PEARL) == POCKET_ITEMS);
    ASSUME(GetItemPocket(ITEM_BIG_PEARL) == POCKET_ITEMS);

    RUN_OVERWORLD_SCRIPT(
        additem ITEM_NUGGET;
        additem ITEM_BIG_NUGGET;
        additem ITEM_TINY_MUSHROOM;
        additem ITEM_BIG_MUSHROOM;
        additem ITEM_PEARL;
        additem ITEM_BIG_PEARL;
    );

    EXPECT_EQ(pocket->itemSlots[0].itemId, ITEM_NUGGET);
    EXPECT_EQ(pocket->itemSlots[0].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[1].itemId, ITEM_BIG_NUGGET);
    EXPECT_EQ(pocket->itemSlots[1].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[2].itemId, ITEM_TINY_MUSHROOM);
    EXPECT_EQ(pocket->itemSlots[2].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[3].itemId, ITEM_BIG_MUSHROOM);
    EXPECT_EQ(pocket->itemSlots[3].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[4].itemId, ITEM_PEARL);
    EXPECT_EQ(pocket->itemSlots[4].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[5].itemId, ITEM_BIG_PEARL);
    EXPECT_EQ(pocket->itemSlots[5].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[6].itemId, ITEM_NONE);

    SortItemsInBag(&gBagPockets[POCKET_ITEMS], SORT_ALPHABETICALLY);

    EXPECT_EQ(pocket->itemSlots[0].itemId, ITEM_BIG_MUSHROOM);
    EXPECT_EQ(pocket->itemSlots[1].itemId, ITEM_BIG_NUGGET);
    EXPECT_EQ(pocket->itemSlots[2].itemId, ITEM_BIG_PEARL);
    EXPECT_EQ(pocket->itemSlots[3].itemId, ITEM_NUGGET);
    EXPECT_EQ(pocket->itemSlots[4].itemId, ITEM_PEARL);
    EXPECT_EQ(pocket->itemSlots[5].itemId, ITEM_TINY_MUSHROOM);
    EXPECT_EQ(pocket->itemSlots[6].itemId, ITEM_NONE);

    // Try removing the big items, check that everything is compacted correctly

    RUN_OVERWORLD_SCRIPT(
        removeitem ITEM_BIG_NUGGET;
        removeitem ITEM_BIG_MUSHROOM;
        removeitem ITEM_BIG_PEARL;
    );

    CompactItemsInBagPocket(POCKET_ITEMS);

    EXPECT_EQ(pocket->itemSlots[0].itemId, ITEM_NUGGET);
    EXPECT_EQ(pocket->itemSlots[0].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[1].itemId, ITEM_PEARL);
    EXPECT_EQ(pocket->itemSlots[1].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[2].itemId, ITEM_TINY_MUSHROOM);
    EXPECT_EQ(pocket->itemSlots[2].quantity, 1);
    EXPECT_EQ(pocket->itemSlots[3].itemId, ITEM_NONE);
    EXPECT_EQ(pocket->itemSlots[4].itemId, ITEM_NONE);
    EXPECT_EQ(pocket->itemSlots[5].itemId, ITEM_NONE);
    EXPECT_EQ(pocket->itemSlots[6].itemId, ITEM_NONE);
}
