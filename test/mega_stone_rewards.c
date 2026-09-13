#include "global.h"
#include "berry.h"
#include "event_data.h"
#include "item.h"
#include "item_use.h"
#include "legendary_signs.h"
#include "mega_stone_rewards.h"
#include "test/test.h"
#include "constants/emerald_champions.h"

static void ResetHarvest(void)
{
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 0, NUM_BERRIES);
    gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked = FALSE;
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    FlagClear(FLAG_EC_BERRY_TRADE_DRAGONINITE);
    FlagClear(FLAG_EC_BERRY_TRADE_TYRANITARITE);
}

TEST("Harvest economy: ordinary free stock never pays a harvest recipe")
{
    ResetHarvest();
    for (u32 berry = 1; berry < NUM_BERRIES; berry++)
    {
        enum Item item = BerryTypeToItemId(berry);
        EXPECT(AddBagItem(item, 30));
        EXPECT_EQ(GetHarvestedBerryCount(berry), 0);
        EXPECT_EQ(GetItemSellPrice(item), 0);
        EXPECT(GetItemFieldFunc(item) == ItemUseOutOfBattle_CannotUse);
    }
    for (u32 choice = 0; choice < 3; choice++)
    {
        gSpecialVar_0x8004 = choice;
        TradeEmeraldChampionsGardenBerries();
        EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_NOT_ENOUGH);
    }
    ResetHarvest();
}

TEST("Harvest economy: each stone spends typed harvest once and leaves equipment untouched")
{
    u32 choice;
    enum Item stone;
    PARAMETRIZE { choice = 0; stone = ITEM_BAXCALIBRITE; }
    PARAMETRIZE { choice = 1; stone = ITEM_DRAGONINITE; }
    PARAMETRIZE { choice = 2; stone = ITEM_TYRANITARITE; }
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    EXPECT(AddBagItem(ITEM_LUM_BERRY, 6));
    gSpecialVar_0x8004 = choice;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT_EQ(CountTotalItemQuantityInBag(stone), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LUM_BERRY), 6);
    u32 remaining = 0;
    for (u32 berry = 1; berry <= NUM_BERRIES; berry++)
        remaining += GetHarvestedBerryCount(berry);
    EXPECT_EQ(remaining, 30 * NUM_BERRIES - (choice == 0 ? 20 : 24));
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    u32 after = 0;
    for (u32 berry = 1; berry <= NUM_BERRIES; berry++)
        after += GetHarvestedBerryCount(berry);
    EXPECT_EQ(after, remaining);
    ResetHarvest();
}

TEST("Harvest economy: missing one type cannot be replaced by a surplus of another")
{
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    gSaveBlock2Ptr->pokedex.harvestedBerries[BERRY_ID_BLUK - 1] = 5;
    AddHarvestedBerries(BERRY_ID_RAZZ, 200);
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_NOT_ENOUGH);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 230);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_BLUK), 5);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    ResetHarvest();
}

TEST("Harvest economy: full reward pocket preserves all payment and allows retry")
{
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BAXCALIBRITE)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_DRAGONINITE, 1);
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_BAG_FULL);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 30);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 24);
    ResetHarvest();
}

TEST("Harvest economy: existing PC ownership cannot charge harvest again")
{
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    EXPECT(AddPCItem(ITEM_BAXCALIBRITE, 1));
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 30);
    ResetHarvest();
}

TEST("Harvest economy: quantities do not wrap or spill into the next berry")
{
    ResetHarvest();
    AddHarvestedBerries(BERRY_ID_LUM, 254);
    EXPECT(!CanAddHarvestedBerries(BERRY_ID_LUM, 2));
    AddHarvestedBerries(BERRY_ID_LUM, 2);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 254);
    AddHarvestedBerries(BERRY_ID_LUM, 1);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 255);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_SITRUS), 0);
    EXPECT(!CanAddHarvestedBerries(0, 1));
    EXPECT(!CanAddHarvestedBerries(NUM_BERRIES + 1, 1));
    ResetHarvest();
}

TEST("Harvest economy: Celebi invitation is permanent and does not claim a capture")
{
    ResetHarvest();
    // A fresh encounter state, separate from the earned campaign.
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    FlagClear(FLAG_EC_CAUGHT_CELEBI);
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    gSpecialVar_0x8004 = 3;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI));
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 26);
    CheckEmeraldChampionsGardenCelebi();
    EXPECT_EQ(gSpecialVar_Result, 1);
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 26);
    MarkLegendarySignCaughtBySpecies(SPECIES_CELEBI);
    CheckEmeraldChampionsGardenCelebi();
    EXPECT_EQ(gSpecialVar_Result, 2);
    ResetHarvest();
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    FlagClear(FLAG_EC_CAUGHT_CELEBI);
}

TEST("Harvest economy: daily seed pair rolls back if only its first berry fits")
{
    ResetHarvest();
    struct BagPocket *pocket = &gBagPockets[POCKET_BERRIES];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_CHERI_BERRY, MAX_BAG_ITEM_CAPACITY);
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    gSpecialVar_0x8008 = ITEM_LUM_BERRY;
    gSpecialVar_0x8009 = ITEM_SITRUS_BERRY;
    GiveEmeraldChampionsBerryPair();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LUM_BERRY), 0);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 0);
    BagPocket_SetSlotItemIdAndCount(pocket, 1, ITEM_NONE, 0);
    GiveEmeraldChampionsBerryPair();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LUM_BERRY), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_SITRUS_BERRY), 1);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_SITRUS), 0);
    ResetHarvest();
}
