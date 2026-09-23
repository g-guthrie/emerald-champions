#include "global.h"
#include "berry.h"
#include "event_data.h"
#include "item.h"
#include "script.h"
#include "test/test.h"
#include "constants/emerald_champions.h"

extern const u8 BerryTree_EventScript_PickBerry[];
extern const u8 BerryTree_EventScript_HarvestSucceeded[];
extern const u8 BerryTree_EventScript_BerryPocketFull[];
extern ScrCmdFunc gScriptCmdTable[], gScriptCmdTableEnd[];

TEST("Berry harvest: a full harvest record still picks; a full Bag keeps the crop")
{
    u32 failure;
    PARAMETRIZE { failure = 0; } // Successful harvest.
    PARAMETRIZE { failure = 1; } // Harvest record already at its cap: picking still succeeds.
    PARAMETRIZE { failure = 2; } // Berry stack full.
    ClearBag();
    gSelectedObjectEvent = 0;
    gObjectEvents[0].trainerRange_berryTreeId = 0;
    struct BerryTree *tree = GetBerryTreeInfo(0);
    memset(tree, 0, sizeof(*tree));
    tree->berry = BERRY_ID_ORAN;
    tree->berryYield = 3;
    gSaveBlock2Ptr->pokedex.harvestedBerries[BERRY_ID_ORAN - 1] = failure == 1 ? EC_HARVEST_LIMIT : 0;
    if (failure == 2)
        EXPECT(AddBagItem(ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY));
    struct BerryTree before = *tree;
    const u8 *target = failure == 2 ? BerryTree_EventScript_BerryPocketFull : BerryTree_EventScript_HarvestSucceeded;
    struct ScriptContext ctx;
    InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
    SetupBytecodeScript(&ctx, BerryTree_EventScript_PickBerry);
    for (u32 step = 0; step < 10 && ctx.scriptPtr != target; step++)
    {
        EXPECT(ctx.scriptPtr != BerryTree_EventScript_HarvestSucceeded || failure != 2);
        u8 cmd = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[cmd](&ctx));
    }
    EXPECT_EQ(ctx.scriptPtr, target);
    EXPECT_EQ(memcmp(tree, &before, sizeof(before)), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ORAN_BERRY), failure == 2 ? MAX_BAG_ITEM_CAPACITY : 3);
    EXPECT_EQ(gSaveBlock2Ptr->pokedex.harvestedBerries[BERRY_ID_ORAN - 1], failure == 0 ? 3 : failure == 1 ? EC_HARVEST_LIMIT : 0);
    ClearBag();
    memset(tree, 0, sizeof(*tree));
    gSaveBlock2Ptr->pokedex.harvestedBerries[BERRY_ID_ORAN - 1] = 0;
}
