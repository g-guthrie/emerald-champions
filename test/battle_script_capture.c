#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "test/test.h"

u32 Test_ComputeCaptureOdds(u32 wildMonBattler, u32 playerBattler);

static void SetCaptureBoard(enum Species species)
{
    memset(gBattleMons, 0, sizeof(gBattleMons));
    gBattleTypeFlags = 0;
    gBattleMons[0].level = 50;
    gBattleMons[1].species = species;
    gBattleMons[1].level = 50;
    gBattleMons[1].hp = gBattleMons[1].maxHP = 100;
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagSet(flag);
}

TEST("Battle script capture: Heavy Ball penalties cannot wrap into guaranteed catches")
{
    SetCaptureBoard(SPECIES_AZELF);
    ASSUME(gSpeciesInfo[SPECIES_AZELF].catchRate < 20);
    ASSUME(gSpeciesInfo[SPECIES_AZELF].weight < 1000);
    gLastUsedItem = ITEM_POKE_BALL;
    u32 normal = Test_ComputeCaptureOdds(1, 0);
    gLastUsedItem = ITEM_HEAVY_BALL;
    u32 heavy = Test_ComputeCaptureOdds(1, 0);
    EXPECT_GT(heavy, 0);
    EXPECT_LE(heavy, normal);
    EXPECT_LT(heavy, 255);
}

TEST("Battle script capture: very low ball odds remain valid for the shake calculation")
{
    SetCaptureBoard(SPECIES_AZELF);
    gLastUsedItem = ITEM_BEAST_BALL;
    EXPECT_GT(Test_ComputeCaptureOdds(1, 0), 0);
    EXPECT_LT(Test_ComputeCaptureOdds(1, 0), 255);
    gLastUsedItem = ITEM_MASTER_BALL;
    EXPECT_EQ(Test_ComputeCaptureOdds(1, 0), (u32)-1);
}

TEST("Battle script capture: Heavy Balls still reward heavy targets")
{
    SetCaptureBoard(SPECIES_SNORLAX);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].weight >= 3000);
    gLastUsedItem = ITEM_POKE_BALL;
    u32 normal = Test_ComputeCaptureOdds(1, 0);
    gLastUsedItem = ITEM_HEAVY_BALL;
    EXPECT_GT(Test_ComputeCaptureOdds(1, 0), normal);
    EXPECT_LT(Test_ComputeCaptureOdds(1, 0), 255);
}
