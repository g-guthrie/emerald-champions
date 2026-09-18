#include "global.h"
#include "caps.h"
#include "coins.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "money.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Inclement integration: failed item delivery cannot unlock vendor stock")
{
    ClearBag();
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_CHOICE_BAND)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_LEFTOVERS, 1);
    EXPECT(!AddBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
    EXPECT(!AddBagItem(ITEM_CHOICE_BAND, 0));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    EXPECT(AddBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
    EXPECT(RemoveBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
}

TEST("Inclement integration: opening held items retry without duplicate gifts")
{
    static const enum Item items[] = {ITEM_CHOICE_BAND, ITEM_CHOICE_SPECS,
        ITEM_CHOICE_SCARF, ITEM_FOCUS_SASH, ITEM_EVIOLITE};
    ClearBag();
    FlagClear(FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS);
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_CHOICE_BAND)];
    EXPECT_GT((u32)pocket->capacity, 6);
    for (u32 i = 0; i < pocket->capacity - 2; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_LEFTOVERS, 1);
    GiveEmeraldChampionsStarterBattleItems();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT(!FlagGet(FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_CHOICE_BAND), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_CHOICE_SPECS), 1);
    for (u32 i = 0; i < 3; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_NONE, 0);
    GiveEmeraldChampionsStarterBattleItems();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(FlagGet(FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS));
    GiveEmeraldChampionsStarterBattleItems();
    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        EXPECT_EQ(CountTotalItemQuantityInBag(items[i]), 1);
        EXPECT(IsEmeraldChampionsBattleItemUnlocked(items[i]));
    }
}

TEST("Inclement integration: Leveler stops at the previous cap on repeated use")
{
    struct Pokemon mon;
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagClear(flag);
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    FlagClear(FLAG_IS_CHAMPION);
    FlagSet(FLAG_BADGE01_GET);
    EXPECT_EQ(GetCurrentLevelCap(), 20);
    EXPECT_EQ(GetPreviousLevelCap(), 14);
    CreateMon(&mon, SPECIES_ZIGZAGOON, 5, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(RaiseMonToLevelerTarget(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 14);
    EXPECT(!RaiseMonToLevelerTarget(&mon));
    EXPECT(!IsMonEligibleForLeveler(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 14);
    FlagSet(FLAG_BADGE02_GET);
    EXPECT(RaiseMonToLevelerTarget(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 20);
    EXPECT(!RaiseMonToLevelerTarget(&mon));
}

TEST("Inclement integration: Coins and cash are independent balances")
{
    SetMoney(&gSaveBlock1Ptr->money, 6000);
    SetCoins(0);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 6000);
    EXPECT_EQ(GetCoins(), 0);
    EXPECT(AddCoins(50));
    RemoveMoney(&gSaveBlock1Ptr->money, 500);
    EXPECT_EQ(GetCoins(), 50);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 5500);
    EXPECT(RemoveCoins(10));
    EXPECT_EQ(GetCoins(), 40);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 5500);
}

TEST("Inclement integration: natural IVs keep three perfect stats and fixed IVs stay exact")
{
    struct Pokemon mon;
    u32 imperfect = 0;
    for (u32 sample = 0; sample < 8; sample++)
    {
        CreateRandomMon(&mon, SPECIES_ZIGZAGOON, 5);
        u32 perfect = 0;
        for (u32 stat = 0; stat < NUM_STATS; stat++)
            perfect += GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS;
        EXPECT_GE(perfect, 3);
        imperfect += perfect < NUM_STATS;
    }
    EXPECT_GT(imperfect, 0);
    SetBoxMonIVs(&mon.box, 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IVS), 0);
    SetBoxMonIVs(&mon.box, MAX_PER_STAT_IVS);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV + stat), MAX_PER_STAT_IVS);
}

TEST("Inclement integration: owned captured and boxed held items unlock paid copies")
{
    struct Pokemon mon;
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    memset(gParties[B_TRAINER_PLAYER], 0, sizeof(gParties[B_TRAINER_PLAYER]));
    CreateRandomMon(&mon, SPECIES_ZIGZAGOON, 5);
    enum Item item = ITEM_LIFE_ORB;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    item = ITEM_ASSAULT_VEST;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT_EQ(CopyMonToPC(&mon), MON_GIVEN_TO_PC);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT_GT(GetItemPrice(ITEM_ADAMANT_CRYSTAL), 0);
    EXPECT(!IsEmeraldChampionsFreeCatalogueItem(ITEM_ADAMANT_CRYSTAL));
    EXPECT(IsEmeraldChampionsFreeCatalogueItem(ITEM_ROTOM_CATALOG));
}
