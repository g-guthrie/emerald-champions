#include "global.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "legendary_signs.h"
#include "mega_stone_rewards.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/items.h"
#include "constants/service_vars.h"

// include/constants/service_vars.h: services pass arguments in VAR_0x8004-0x8007
// and keep their own menu state in VAR_0x8008-0x800B, which the natives they
// call never read or write.

extern void CheckChosenMonCanGainEVs(void);
extern void ChangeChosenMonHiddenPower(void);
extern void BufferEmeraldChampionsBattleItemStock(void);

static void SetScriptOwnedSentinels(void)
{
    gSpecialVar_0x8008 = 0xA5A8;
    gSpecialVar_0x8009 = 0xA5A9;
    gSpecialVar_0x800A = 0xA5AA;
    gSpecialVar_0x800B = 0xA5AB;
}

static void ExpectScriptOwnedSentinels(void)
{
    EXPECT_EQ(gSpecialVar_0x8008, 0xA5A8);
    EXPECT_EQ(gSpecialVar_0x8009, 0xA5A9);
    EXPECT_EQ(gSpecialVar_0x800A, 0xA5AA);
    EXPECT_EQ(gSpecialVar_0x800B, 0xA5AB);
}

TEST("Service vars: the aliases keep the documented special var layout")
{
    EXPECT_EQ(VAR_CLERK_SHELF_CURSOR, VAR_0x8008);
    EXPECT_EQ(VAR_CLERK_SERVICE_CURSOR, VAR_0x8009);
    EXPECT_EQ(VAR_CLERK_HELD_ITEMS_GREETED, VAR_0x800A);
    EXPECT_EQ(VAR_CLERK_FORM_ITEMS_GREETED, VAR_0x800B);
    // The tutor calls Bonding: their state must not overlap.
    EXPECT_NE(VAR_TUTOR_CURSOR, VAR_BONDING_CURSOR);
    EXPECT_NE(VAR_TUTOR_CURSOR, VAR_BONDING_MON);
    // The Berry Master calls the Harvest menu.
    EXPECT_NE(VAR_BERRY_MASTER_CURSOR, VAR_HARVEST_AT_BERRY_MASTER);
    EXPECT_NE(VAR_BERRY_MASTER_CURSOR, VAR_HARVEST_LIST_CURSOR);
    EXPECT_NE(VAR_BERRY_MASTER_CURSOR, VAR_HARVEST_REWARD);
}

TEST("Service vars: Bonding takes its Pokemon and value as arguments")
{
    static const u8 expected[] = {0, FRIENDSHIP_EVO_THRESHOLD, 255};
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_PICHU, 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    for (u32 value = 0; value < ARRAY_COUNT(expected); value++)
    {
        u32 start = 100;
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_FRIENDSHIP, &start);
        SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_FRIENDSHIP, &start);
        SetScriptOwnedSentinels();
        gSpecialVar_0x8004 = 1;
        gSpecialVar_0x8005 = value;
        BufferEmeraldChampionsBondingPreview();
        EXPECT_EQ(gSpecialVar_Result, TRUE);
        ApplyEmeraldChampionsBonding();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_FRIENDSHIP), expected[value]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_FRIENDSHIP), 100);
        ExpectScriptOwnedSentinels();
    }
    gSpecialVar_0x8005 = 3; // Back row: never a value.
    BufferEmeraldChampionsBondingPreview();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    ZeroPlayerPartyMons();
}

TEST("Service vars: harvest, berry, fossil, stock and stat services leave script state alone")
{
    ClearBag();
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();

    SetScriptOwnedSentinels();
    FlagSet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    gSpecialVar_0x8004 = 0;
    BufferEmeraldChampionsHarvestRecipe();
    EXPECT_EQ(gSpecialVar_Result, TRUE); // Claimed comes back in VAR_RESULT.
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    ClearBag();
    BufferEmeraldChampionsHarvestRecipe();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    ExpectScriptOwnedSentinels();

    gSpecialVar_0x8004 = ITEM_ORAN_BERRY;
    gSpecialVar_0x8005 = ITEM_PECHA_BERRY;
    EXPECT(CanReceiveBerryPair());
    ExpectScriptOwnedSentinels();

    gSpecialVar_0x8004 = ITEM_HELIX_FOSSIL;
    gSpecialVar_0x8006 = SPECIES_NONE;
    FossilToSpecies();
    EXPECT_EQ(gSpecialVar_0x8006, SPECIES_OMANYTE);
    ExpectScriptOwnedSentinels();

    gSpecialVar_0x8004 = 0;
    BufferEmeraldChampionsBattleItemStock();
    ExpectScriptOwnedSentinels();

    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = STAT_HP;
    gSpecialVar_0x8006 = 4;
    CheckChosenMonCanGainEVs();
    ExpectScriptOwnedSentinels();

    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = 0; // Fighting
    ChangeChosenMonHiddenPower();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_IV), 0);
    ExpectScriptOwnedSentinels();

    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = 1; // CENTER_GUIDE_TOPIC_TIPS
    BufferNextCenterLegendaryLead();
    ExpectScriptOwnedSentinels();

    ZeroPlayerPartyMons();
    ClearBag();
}
