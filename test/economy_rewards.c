#include "global.h"
#include "event_data.h"
#include "item.h"
#include "item_ball.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "test/test.h"
#include "constants/maps.h"

extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];
extern const u8 Std_FindItem[];
extern const u8 Std_ObtainItem[];
extern const u8 EventScript_CheckFiniteItem[];
extern const u8 EventScript_FindOrdinaryItem[];
extern const u8 EventScript_PickUpDuplicateFiniteItem[];
extern const u8 Common_EventScript_ObtainFiniteItem[];
extern const u8 SeafloorCavern_Room9_EventScript_ItemTM26[];
extern const u8 Seaspray_Cave_ItemStoneEdge[];
extern const u8 ScorchedSlab_EventScript_ItemTyranitarite[];
extern const u8 Route110_TrickHouseEntrance_EventScript_GivePuzzle1Reward[];
extern const u8 Route110_TrickHouseEntrance_EventScript_GivePuzzle3Reward[];
extern const u8 Route110_TrickHouseEntrance_EventScript_GivePuzzle5Reward[];

static void ClearRewardOwnership(void)
{
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
}

static void RunRewardCommand(struct ScriptContext *ctx)
{
    u8 command = *ctx->scriptPtr++;
    EXPECT(!ctx->cmdTable[command](ctx));
}

TEST("Economy rewards: only a persistent item-ball receipt permits compensation")
{
    static const struct ObjectEventTemplate objects[] = {
        {.localId = 22, .flagId = 0},
        {.localId = 37, .flagId = FLAG_TEMP_1},
        {.localId = 5, .flagId = FLAG_ITEM_ROUTE_119_RARE_CANDY},
    };
    static const struct MapEvents events = {
        .objectEventCount = ARRAY_COUNT(objects), .objectEvents = objects,
    };
    struct MapHeader saved = gMapHeader;
    gMapHeader.events = &events;
    const u8 ids[] = {22, 37, 5, 99};
    for (u32 i = 0; i < ARRAY_COUNT(ids); i++)
    {
        gSpecialVar_LastTalked = ids[i];
        CheckItemBallHasPermanentReceipt();
        EXPECT_EQ(gSpecialVar_Result, i == 2);
    }
    gMapHeader = saved;
}

TEST("Economy rewards: live garden-stone pickups enter the shared finite payout path")
{
    static const struct { u16 map, item; const u8 *script; } rewards[] = {
        {MAP_SEAFLOOR_CAVERN_ROOM9, ITEM_DRAGONINITE, SeafloorCavern_Room9_EventScript_ItemTM26},
        {MAP_SEASPRAY_CAVE, ITEM_BAXCALIBRITE, Seaspray_Cave_ItemStoneEdge},
        {MAP_SCORCHED_SLAB_B2F, ITEM_TYRANITARITE, ScorchedSlab_EventScript_ItemTyranitarite},
    };
    struct MapHeader saved = gMapHeader;
    u32 savedMoney = GetMoney(&gSaveBlock1Ptr->money);
    for (u32 row = 0; row < ARRAY_COUNT(rewards); row++)
    for (u32 owned = 0; owned < 2; owned++)
    {
        ClearRewardOwnership();
        if (owned)
            EXPECT(AddPCItem(rewards[row].item, 1));
        SetMoney(&gSaveBlock1Ptr->money, 100);
        gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(rewards[row].map), MAP_NUM(rewards[row].map));
        bool32 bound = FALSE;
        for (u32 i = 0; i < gMapHeader.events->objectEventCount; i++)
        {
            const struct ObjectEventTemplate *object = &gMapHeader.events->objectEvents[i];
            if (object->script == rewards[row].script)
            {
                bound = TRUE;
                EXPECT(object->flagId > TEMP_FLAGS_END);
                gSpecialVar_LastTalked = object->localId;
            }
        }
        EXPECT(bound);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, rewards[row].script);
        for (u32 step = 0; step < 4 && ctx.scriptPtr != Std_FindItem; step++)
            RunRewardCommand(&ctx);
        EXPECT_EQ(ctx.scriptPtr, Std_FindItem);
        EXPECT_EQ(gSpecialVar_0x8000, rewards[row].item);
        EXPECT_EQ(gSpecialVar_0x8001, 1);
        // Continue after lock/face/waitse: the native visual run checks those.
        SetupBytecodeScript(&ctx, EventScript_CheckFiniteItem);
        for (u32 step = 0; step < 8
            && ctx.scriptPtr != EventScript_FindOrdinaryItem
            && ctx.scriptPtr != EventScript_PickUpDuplicateFiniteItem; step++)
            RunRewardCommand(&ctx);
        EXPECT_EQ(ctx.scriptPtr, owned ? EventScript_PickUpDuplicateFiniteItem : EventScript_FindOrdinaryItem);
        EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), owned ? 3100 : 100);
        EXPECT_EQ(CountTotalItemQuantityInBag(rewards[row].item), 0);
    }
    SetMoney(&gSaveBlock1Ptr->money, savedMoney);
    gMapHeader = saved;
    ClearRewardOwnership();
}

TEST("Economy rewards: finite gifts preserve first-copy Bag failure and compensate owned devices only")
{
    u32 savedMoney = GetMoney(&gSaveBlock1Ptr->money);
    for (u32 owned = 0; owned < 2; owned++)
    {
        ClearRewardOwnership();
        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BAXCALIBRITE)];
        for (u32 i = 0; i < pocket->capacity; i++)
            BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_VENUSAURITE, MAX_BAG_ITEM_CAPACITY);
        if (owned)
            EXPECT(AddPCItem(ITEM_BAXCALIBRITE, 1));
        SetMoney(&gSaveBlock1Ptr->money, 0);
        gSpecialVar_0x8000 = ITEM_BAXCALIBRITE;
        gSpecialVar_0x8001 = 1;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Common_EventScript_ObtainFiniteItem);
        for (u32 step = 0; step < 3; step++)
            RunRewardCommand(&ctx);
        EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), owned ? 3000 : 0);
        if (!owned)
        {
            EXPECT_EQ(ctx.scriptPtr, Std_ObtainItem);
            for (u32 step = 0; step < 3; step++)
                RunRewardCommand(&ctx);
            EXPECT_EQ(gSpecialVar_0x8007, FALSE);
            EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BAXCALIBRITE), 0);
        }
    }
    ClearRewardOwnership();
    EXPECT(AddBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT_EQ(GetFiniteDuplicateRewardValue(ITEM_CHOICE_BAND), 0);
    EXPECT_EQ(GetFiniteDuplicateRewardValue(ITEM_NUGGET), 0);
    ClearRewardOwnership();
    SetMoney(&gSaveBlock1Ptr->money, savedMoney);
}

TEST("Economy rewards: deferred Trick House prizes match the completed puzzle")
{
    static const struct { const u8 *script; u16 amount; } prizes[] = {
        {Route110_TrickHouseEntrance_EventScript_GivePuzzle1Reward, 2},
        {Route110_TrickHouseEntrance_EventScript_GivePuzzle3Reward, 4},
        {Route110_TrickHouseEntrance_EventScript_GivePuzzle5Reward, 6},
    };
    for (u32 i = 0; i < ARRAY_COUNT(prizes); i++)
    {
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, prizes[i].script);
        for (u32 step = 0; step < 4 && ctx.scriptPtr != Std_ObtainItem; step++)
            RunRewardCommand(&ctx);
        EXPECT_EQ(ctx.scriptPtr, Std_ObtainItem);
        EXPECT_EQ(gSpecialVar_0x8000, ITEM_BOTTLE_CAP);
        EXPECT_EQ(gSpecialVar_0x8001, prizes[i].amount);
    }
}
