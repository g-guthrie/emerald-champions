#include "global.h"
#include "malloc.h"
#include "script.h"
#include "pokemon.h"
#include "pokemon_size_record.h"
#include "constants/party_menu.h"
#include "move.h"
#include "constants/moves.h"
#include "constants/decorations.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "money.h"
#include "item_menu.h"
#include "test/test.h"

TEST("Campaign gifts: shared pockets, duplicate items and cross-pocket failures")
{
    const struct ItemSlot pair[] = {{ITEM_LATIOSITE, 1}, {ITEM_LATIASITE, 1}};
    const struct ItemSlot repeated[] = {{ITEM_LATIOSITE, 1}, {ITEM_LATIOSITE, 1}};
    struct BagPocket *mega = &gBagPockets[GetItemPocket(ITEM_LATIOSITE)];
    struct BagPocket *keys = &gBagPockets[GetItemPocket(ITEM_MEGA_RING)];
    struct BagPocket *battle = &gBagPockets[GetItemPocket(ITEM_CHOICE_BAND)];
    struct BagPocket *caps = &gBagPockets[GetItemPocket(ITEM_BOTTLE_CAP)];

    for (u32 slot = 0; slot < mega->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(mega, slot, ITEM_GARDEVOIRITE, MAX_BAG_ITEM_CAPACITY);
    BagPocket_SetSlotItemIdAndCount(mega, 0, ITEM_NONE, 0);
    EXPECT(!CanReceiveLatiStones());
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LATIOSITE), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LATIASITE), 0);
    EXPECT(CheckBagHasSpaceForItemBundle(repeated, ARRAY_COUNT(repeated)));
    EXPECT(AddBagItem(ITEM_LATIOSITE, 2));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LATIOSITE), 2);
    BagPocket_SetSlotItemIdAndCount(mega, 1, ITEM_NONE, 0);
    EXPECT(CheckBagHasSpaceForItemBundle(pair, ARRAY_COUNT(pair)));
    EXPECT(AddBagItem(ITEM_LATIOSITE, 1));
    EXPECT(AddBagItem(ITEM_LATIASITE, 1));

    // The key fits, but Norman must not grant it if the stones he owes do not.
    for (u32 slot = 0; slot < keys->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(keys, slot, ITEM_NONE, 0);
    EXPECT(!CanReceiveNormanMegaGift());
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_MEGA_RING), 0);
    for (u32 slot = 0; slot < mega->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(mega, slot, ITEM_NONE, 0);
    EXPECT(CanReceiveNormanMegaGift());

    // A Choice item fitting does not permit a partial frontier prize.
    for (u32 slot = 0; slot < battle->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(battle, slot, ITEM_NONE, 0);
    for (u32 slot = 0; slot < caps->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(caps, slot, ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY);
    gSpecialVar_0x8004 = ITEM_CHOICE_BAND;
    gSpecialVar_0x8005 = 3;
    EXPECT(!CanReceiveFrontierReward());
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_CHOICE_BAND), 0);
    BagPocket_SetSlotItemIdAndCount(caps, 0, ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY - 3);
    EXPECT(CanReceiveFrontierReward());
    EXPECT(AddBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(AddBagItem(ITEM_BOTTLE_CAP, 3));
    for (u32 slot = 0; slot < caps->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(caps, slot, ITEM_NONE, 0);
    for (u32 slot = 0; slot < battle->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(battle, slot, ITEM_NONE, 0);
}

extern const u8 Route117_PokemonDayCare_EventScript_CheckEggSpace[];
extern const u8 Route117_PokemonDayCare_EventScript_EggReceived[];
extern const u8 Route117_PokemonDayCare_EventScript_NoRoomForEgg[];
extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];

TEST("Campaign gifts: Day Care egg delivery preserves full-party retries and teaches the gift move")
{
    for (u32 received = 0; received < 2; received++)
    for (u32 count = 0; count <= PARTY_SIZE; count++)
    {
        ZeroPlayerPartyMons();
        for (u32 slot = 0; slot < count; slot++)
            CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
        if (received)
            FlagSet(FLAG_RECEIVED_TOGEPI_EGG);
        else
            FlagClear(FLAG_RECEIVED_TOGEPI_EGG);
        FlagClear(FLAG_DAILY_RECEIVED_DAYCARE_EGG);
        gSpecialVar_0x8004 = SPECIES_TOGEPI;
        gSpecialVar_0x8005 = MOVE_EXTRASENSORY;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Route117_PokemonDayCare_EventScript_CheckEggSpace);
        for (u32 step = 0; step < 16
          && ctx.scriptPtr != Route117_PokemonDayCare_EventScript_EggReceived
          && ctx.scriptPtr != Route117_PokemonDayCare_EventScript_NoRoomForEgg; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        if (count == PARTY_SIZE)
        {
            EXPECT_EQ(ctx.scriptPtr, Route117_PokemonDayCare_EventScript_NoRoomForEgg);
            EXPECT(!FlagGet(FLAG_DAILY_RECEIVED_DAYCARE_EGG));
            EXPECT_EQ(FlagGet(FLAG_RECEIVED_TOGEPI_EGG), received);
            EXPECT_EQ(CalculatePlayerPartyCount(), PARTY_SIZE);
        }
        else
        {
            EXPECT_EQ(ctx.scriptPtr, Route117_PokemonDayCare_EventScript_EggReceived);
            EXPECT(FlagGet(FLAG_RECEIVED_TOGEPI_EGG));
            EXPECT(FlagGet(FLAG_DAILY_RECEIVED_DAYCARE_EGG));
            EXPECT_EQ(CalculatePlayerPartyCount(), count + 1);
            struct Pokemon *egg = &gParties[B_TRAINER_PLAYER][count];
            EXPECT(GetMonData(egg, MON_DATA_IS_EGG));
            EXPECT_EQ(GetMonData(egg, MON_DATA_SPECIES), gSpecialVar_0x8004);
            EXPECT(MonKnowsMove(egg, gSpecialVar_0x8005));
            if (!received)
            {
                EXPECT_EQ(gSpecialVar_0x8004, SPECIES_TOGEPI);
                EXPECT_EQ(gSpecialVar_0x8005, MOVE_EXTRASENSORY);
            }
        }
    }
    ZeroPlayerPartyMons();
    FlagClear(FLAG_RECEIVED_TOGEPI_EGG);
    FlagClear(FLAG_DAILY_RECEIVED_DAYCARE_EGG);
}

extern const u8 Route111_EventScript_CheckHealBallSpace[];
extern const u8 Route111_EventScript_ChanseyEscapeScene[];
extern const u8 Route111_EventScript_HealBallBagFull[];

TEST("Campaign gifts: Chansey intro requires room for its Heal Ball before the escape scene")
{
    ClearBag();
    struct BagPocket *balls = &gBagPockets[GetItemPocket(ITEM_HEAL_BALL)];
    for (u32 slot = 0; slot < balls->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(balls, slot, ITEM_POKE_BALL, MAX_BAG_ITEM_CAPACITY);
    for (u32 situation = 0; situation < 3; situation++)
    {
        if (situation == 1)
            BagPocket_SetSlotItemIdAndCount(balls, 0, ITEM_NONE, 0);
        else if (situation == 2)
            BagPocket_SetSlotItemIdAndCount(balls, 0, ITEM_HEAL_BALL, MAX_BAG_ITEM_CAPACITY - 1);
        VarSet(VAR_CHANSEY_NURSE_STATE, 0);
        FlagClear(FLAG_HIDE_ROUTE111_CHANSEY);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Route111_EventScript_CheckHealBallSpace);
        for (u32 step = 0; step < 4
          && ctx.scriptPtr != Route111_EventScript_ChanseyEscapeScene
          && ctx.scriptPtr != Route111_EventScript_HealBallBagFull; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, situation == 0 ? Route111_EventScript_HealBallBagFull : Route111_EventScript_ChanseyEscapeScene);
        EXPECT_EQ(VarGet(VAR_CHANSEY_NURSE_STATE), 0);
        EXPECT(!FlagGet(FLAG_HIDE_ROUTE111_CHANSEY));
        EXPECT_EQ(AddBagItem(ITEM_HEAL_BALL, 1), situation != 0);
    }
    ClearBag();
}

extern const u8 Route113_GlassWorkshop_EventScript_BlueFlute[];
extern const u8 Route113_GlassWorkshop_EventScript_YellowFlute[];
extern const u8 Route113_GlassWorkshop_EventScript_RedFlute[];
extern const u8 Route113_GlassWorkshop_EventScript_WhiteFlute[];
extern const u8 Route113_GlassWorkshop_EventScript_BlackFlute[];
extern const u8 Route113_GlassWorkshop_EventScript_PrettyChair[];
extern const u8 Route113_GlassWorkshop_EventScript_PrettyDesk[];
extern const u8 Route113_GlassWorkshop_EventScript_ConfirmGlassItem[];
extern const u8 Route113_GlassWorkshop_EventScript_NotEnoughAshForItem[];
extern const u8 Route113_GlassWorkshop_EventScript_ApplyGlassOrder[];
extern const u8 Route113_GlassWorkshop_EventScript_ChooseDifferentItem[];
extern const u8 Route113_GlassWorkshop_EventScript_MakeGlassItem[];
extern const u8 Route113_GlassWorkshop_EventScript_GiveItemAfterNoRoom[];
extern const u8 Route113_GlassWorkshop_EventScript_TryGiveItemAgain[];

static void RunGlassCommandsUntil(struct ScriptContext *ctx, const u8 *first, const u8 *second)
{
    for (u32 step = 0; step < 24 && ctx->scriptPtr != first && ctx->scriptPtr != second; step++)
    {
        u8 command = *ctx->scriptPtr++;
        EXPECT(!ctx->cmdTable[command](ctx));
    }
}

TEST("Campaign gifts: glass orders share prices and preserve pending receipts without charging twice")
{
    const struct {const u8 *script; u16 item, price; bool8 decor;} orders[] = {
        {Route113_GlassWorkshop_EventScript_BlueFlute, ITEM_BLUE_FLUTE, 250, FALSE},
        {Route113_GlassWorkshop_EventScript_YellowFlute, ITEM_YELLOW_FLUTE, 500, FALSE},
        {Route113_GlassWorkshop_EventScript_RedFlute, ITEM_RED_FLUTE, 500, FALSE},
        {Route113_GlassWorkshop_EventScript_WhiteFlute, ITEM_WHITE_FLUTE, 1000, FALSE},
        {Route113_GlassWorkshop_EventScript_BlackFlute, ITEM_BLACK_FLUTE, 1000, FALSE},
        {Route113_GlassWorkshop_EventScript_PrettyChair, DECOR_PRETTY_CHAIR, 6000, TRUE},
        {Route113_GlassWorkshop_EventScript_PrettyDesk, DECOR_PRETTY_DESK, 8000, TRUE},
    };
    for (u32 order = 0; order < ARRAY_COUNT(orders); order++)
    for (u32 scenario = 0; scenario < 3; scenario++)
    {
        u16 balance = scenario == 0 ? orders[order].price - 1 : orders[order].price + 17;
        VarSet(VAR_GLASS_WORKSHOP_STATE, 2);
        VarSet(VAR_ASH_GATHER_COUNT, balance);
        VarSet(VAR_EC_SOOT_PROGRESS, 9999);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, orders[order].script);
        RunGlassCommandsUntil(&ctx, Route113_GlassWorkshop_EventScript_ConfirmGlassItem,
            Route113_GlassWorkshop_EventScript_NotEnoughAshForItem);
        EXPECT_EQ(gSpecialVar_0x8008, orders[order].item);
        EXPECT_EQ(gSpecialVar_0x8009, orders[order].decor);
        EXPECT_EQ(gSpecialVar_0x800A, orders[order].price);
        EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), balance);
        EXPECT_EQ(VarGet(VAR_GLASS_WORKSHOP_STATE), 2);
        EXPECT_EQ(ctx.scriptPtr, scenario == 0 ? Route113_GlassWorkshop_EventScript_NotEnoughAshForItem
            : Route113_GlassWorkshop_EventScript_ConfirmGlassItem);
        if (scenario == 0)
            continue;
        SetupBytecodeScript(&ctx, Route113_GlassWorkshop_EventScript_ApplyGlassOrder);
        gSpecialVar_Result = scenario == 1 ? FALSE : TRUE;
        RunGlassCommandsUntil(&ctx, Route113_GlassWorkshop_EventScript_ChooseDifferentItem,
            Route113_GlassWorkshop_EventScript_MakeGlassItem);
        EXPECT_EQ(ctx.scriptPtr, scenario == 1 ? Route113_GlassWorkshop_EventScript_ChooseDifferentItem
            : Route113_GlassWorkshop_EventScript_MakeGlassItem);
        EXPECT_EQ(VarGet(VAR_GLASS_WORKSHOP_STATE), scenario == 1 ? 2 : 10 + order);
        EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), scenario == 1 ? balance : 17);
        EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), 9999);
        if (scenario == 1)
            continue;
        // Simulate reentry after failed item/decor delivery: only saved state survives.
        gSpecialVar_0x8008 = gSpecialVar_0x8009 = gSpecialVar_0x800A = gSpecialVar_0x800B = 0;
        SetupBytecodeScript(&ctx, Route113_GlassWorkshop_EventScript_GiveItemAfterNoRoom);
        RunGlassCommandsUntil(&ctx, Route113_GlassWorkshop_EventScript_TryGiveItemAgain,
            Route113_GlassWorkshop_EventScript_TryGiveItemAgain);
        EXPECT_EQ(ctx.scriptPtr, Route113_GlassWorkshop_EventScript_TryGiveItemAgain);
        EXPECT_EQ(gSpecialVar_0x8008, orders[order].item);
        EXPECT_EQ(gSpecialVar_0x8009, orders[order].decor);
        EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 17);
        EXPECT_EQ(VarGet(VAR_GLASS_WORKSHOP_STATE), 10 + order);
    }
}

extern const u8 LavaridgeTown_EventScript_CheckEggSpace[];
extern const u8 LavaridgeTown_EventScript_EggDelivered[];
extern const u8 LavaridgeTown_EventScript_NoRoomForEgg[];

TEST("Campaign gifts: Lavaridge Wynaut receipt follows successful party delivery")
{
    for (u32 count = 0; count <= PARTY_SIZE; count++)
    {
        ZeroPlayerPartyMons();
        for (u32 slot = 0; slot < count; slot++)
            CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
        CalculatePlayerPartyCount();
        FlagClear(FLAG_RECEIVED_LAVARIDGE_EGG);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, LavaridgeTown_EventScript_CheckEggSpace);
        for (u32 step = 0; step < 10
          && ctx.scriptPtr != LavaridgeTown_EventScript_EggDelivered
          && ctx.scriptPtr != LavaridgeTown_EventScript_NoRoomForEgg; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, count == PARTY_SIZE ? LavaridgeTown_EventScript_NoRoomForEgg : LavaridgeTown_EventScript_EggDelivered);
        EXPECT_EQ(FlagGet(FLAG_RECEIVED_LAVARIDGE_EGG), count < PARTY_SIZE);
        EXPECT_EQ(CalculatePlayerPartyCount(), min(count + 1, PARTY_SIZE));
        if (count < PARTY_SIZE)
        {
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][count], MON_DATA_SPECIES), SPECIES_WYNAUT);
            EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][count], MON_DATA_IS_EGG));
        }
    }
    ZeroPlayerPartyMons();
    FlagClear(FLAG_RECEIVED_LAVARIDGE_EGG);
}

TEST("Campaign gifts: fossil acceptance bag detection and revival agree for every item")
{
    const struct {u16 item, species;} fossils[] = {
        {ITEM_HELIX_FOSSIL, SPECIES_OMANYTE}, {ITEM_DOME_FOSSIL, SPECIES_KABUTO},
        {ITEM_OLD_AMBER, SPECIES_AERODACTYL}, {ITEM_ROOT_FOSSIL, SPECIES_LILEEP},
        {ITEM_CLAW_FOSSIL, SPECIES_ANORITH}, {ITEM_ARMOR_FOSSIL, SPECIES_SHIELDON},
        {ITEM_SKULL_FOSSIL, SPECIES_CRANIDOS}, {ITEM_COVER_FOSSIL, SPECIES_TIRTOUGA},
        {ITEM_PLUME_FOSSIL, SPECIES_ARCHEN}, {ITEM_SAIL_FOSSIL, SPECIES_AMAURA},
        {ITEM_JAW_FOSSIL, SPECIES_TYRUNT},
    };
    for (u32 item = ITEM_NONE; item < ITEMS_COUNT; item++)
    {
        u16 expected = SPECIES_NONE;
        for (u32 i = 0; i < ARRAY_COUNT(fossils); i++)
            if (item == fossils[i].item)
                expected = fossils[i].species;
        gSpecialVar_ItemId = item;
        EXPECT_EQ(IsItemFossil(), expected != SPECIES_NONE);
        gSpecialVar_0x8004 = item;
        gSpecialVar_0x8006 = SPECIES_PIKACHU;
        FossilToSpecies();
        EXPECT_EQ(gSpecialVar_0x8006, expected == SPECIES_NONE ? SPECIES_PIKACHU : expected);
    }
    ClearBag();
    EXPECT(!DoesPlayerHaveFossil());
    EXPECT(AddBagItem(ITEM_POTION, 1));
    EXPECT(!DoesPlayerHaveFossil());
    for (u32 i = 0; i < ARRAY_COUNT(fossils); i++)
    {
        EXPECT(AddBagItem(fossils[i].item, 1));
        EXPECT(DoesPlayerHaveFossil());
        EXPECT(RemoveBagItem(fossils[i].item, 1));
        EXPECT(!DoesPlayerHaveFossil());
    }
    ClearBag();
}

extern const u8 Route120_EventScript_StevenBattleKecleon[];
extern const u8 Route120_EventScript_StartKecleonScene[];
extern const u8 Route120_EventScript_DevonScopeNoRoom[];

TEST("Campaign gifts: Steven checks Devon Scope capacity before changing the bridge scene")
{
    ClearBag();
    struct BagPocket *keys = &gBagPockets[GetItemPocket(ITEM_DEVON_SCOPE)];
    for (u32 slot = 0; slot < keys->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(keys, slot, ITEM_OLD_ROD, MAX_BAG_ITEM_CAPACITY);
    for (u32 room = 0; room < 2; room++)
    {
        if (room)
            BagPocket_SetSlotItemIdAndCount(keys, 0, ITEM_NONE, 0);
        FlagClear(FLAG_RECEIVED_DEVON_SCOPE);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Route120_EventScript_StevenBattleKecleon);
        const u8 *expected = room ? Route120_EventScript_StartKecleonScene : Route120_EventScript_DevonScopeNoRoom;
        for (u32 step = 0; step < 4 && ctx.scriptPtr != expected; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, expected);
        EXPECT(!FlagGet(FLAG_RECEIVED_DEVON_SCOPE));
        EXPECT_EQ(AddBagItem(ITEM_DEVON_SCOPE, 1), room);
    }
    ClearBag();
}

extern const u8 LilycoveCity_DepartmentStoreRooftop_EventScript_FreshWater[];
extern const u8 LilycoveCity_DepartmentStoreRooftop_EventScript_SodaPop[];
extern const u8 LilycoveCity_DepartmentStoreRooftop_EventScript_Lemonade[];
extern const u8 LilycoveCity_DepartmentStoreRooftop_EventScript_DispensePaidDrink[];
extern const u8 LilycoveCity_DepartmentStoreRooftop_EventScript_NotEnoughMoneyForDrink[];
extern const u8 LilycoveCity_DepartmentStoreRooftop_EventScript_NoRoomForDrink[];

TEST("Campaign gifts: rooftop vending charges the selected price only with money and space")
{
    const struct {const u8 *script; u16 item, price;} drinks[] = {
        {LilycoveCity_DepartmentStoreRooftop_EventScript_FreshWater, ITEM_FRESH_WATER, 200},
        {LilycoveCity_DepartmentStoreRooftop_EventScript_SodaPop, ITEM_SODA_POP, 300},
        {LilycoveCity_DepartmentStoreRooftop_EventScript_Lemonade, ITEM_LEMONADE, 350},
    };
    u32 savedMoney = GetMoney(&gSaveBlock1Ptr->money);
    for (u32 drink = 0; drink < ARRAY_COUNT(drinks); drink++)
    for (u32 scenario = 0; scenario < 3; scenario++)
    {
        ClearBag();
        u32 money = drinks[drink].price - (scenario == 0);
        SetMoney(&gSaveBlock1Ptr->money, money);
        if (scenario == 1)
        {
            struct BagPocket *pocket = &gBagPockets[GetItemPocket(drinks[drink].item)];
            for (u32 slot = 0; slot < pocket->capacity; slot++)
                BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POTION, MAX_BAG_ITEM_CAPACITY);
        }
        const u8 *expected = scenario == 0 ? LilycoveCity_DepartmentStoreRooftop_EventScript_NotEnoughMoneyForDrink
            : scenario == 1 ? LilycoveCity_DepartmentStoreRooftop_EventScript_NoRoomForDrink
            : LilycoveCity_DepartmentStoreRooftop_EventScript_DispensePaidDrink;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, drinks[drink].script);
        for (u32 step = 0; step < 12 && ctx.scriptPtr != expected; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, expected);
        EXPECT_EQ(VarGet(VAR_TEMP_0), drinks[drink].item);
        EXPECT_EQ(gSpecialVar_0x8005, drinks[drink].price);
        EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), scenario == 2 ? 0 : money);
    }
    SetMoney(&gSaveBlock1Ptr->money, savedMoney);
    ClearBag();
}

extern const u8 Route124_DivingTreasureHuntersHouse_EventScript_CheckTradeSpace[];
extern const u8 Route124_DivingTreasureHuntersHouse_EventScript_TradeShard[];
extern const u8 Route124_DivingTreasureHuntersHouse_EventScript_BagFull[];

TEST("Campaign gifts: shard exchanges reuse the last shard slot without losing a reward")
{
    static const enum Item shards[] = {ITEM_RED_SHARD, ITEM_YELLOW_SHARD, ITEM_BLUE_SHARD, ITEM_GREEN_SHARD};
    for (u32 i = 0; i < ARRAY_COUNT(shards); i++)
    for (u32 quantity = 1; quantity <= 2; quantity++)
    for (u32 full = 0; full < 2; full++)
    {
        ClearBag();
        struct BagPocket *caps = &gBagPockets[GetItemPocket(ITEM_BOTTLE_CAP)];
        for (u32 slot = 0; slot < caps->capacity; slot++)
            BagPocket_SetSlotItemIdAndCount(caps, slot, ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY);
        EXPECT_EQ(GetItemPocket(shards[i]), GetItemPocket(ITEM_BOTTLE_CAP));
        BagPocket_SetSlotItemIdAndCount(caps, 0, shards[i], quantity);
        if (!full)
            BagPocket_SetSlotItemIdAndCount(caps, 1, ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY - 1);
        gSpecialVar_0x8008 = shards[i];
        gSpecialVar_0x8009 = ITEM_BOTTLE_CAP;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Route124_DivingTreasureHuntersHouse_EventScript_CheckTradeSpace);
        for (u32 step = 0; step < 12 && ctx.scriptPtr != Route124_DivingTreasureHuntersHouse_EventScript_TradeShard
            && ctx.scriptPtr != Route124_DivingTreasureHuntersHouse_EventScript_BagFull; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        bool32 canTrade = !full || quantity == 1;
        EXPECT_EQ(ctx.scriptPtr, canTrade ? Route124_DivingTreasureHuntersHouse_EventScript_TradeShard
                                         : Route124_DivingTreasureHuntersHouse_EventScript_BagFull);
        EXPECT_EQ(CountTotalItemQuantityInBag(shards[i]), quantity);
        if (canTrade)
        {
            u32 capsBefore = CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP);
            EXPECT(RemoveBagItem(shards[i], 1));
            EXPECT(AddBagItem(ITEM_BOTTLE_CAP, 1));
            EXPECT_EQ(CountTotalItemQuantityInBag(shards[i]), quantity - 1);
            EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), capsBefore + 1);
        }
        else
            EXPECT(!AddBagItem(ITEM_BOTTLE_CAP, 1));
    }
    ClearBag();
}

extern const u8 SootopolisCity_EventScript_KiriCheckBerryPair[];
extern const u8 SootopolisCity_EventScript_KiriGiveBerryPair[];
extern const u8 Common_EventScript_ShowBagIsFull[];

TEST("Campaign gifts: Kiri requires both berries to fit before spending the daily gift")
{
    struct BagPocket *berries = &gBagPockets[POCKET_BERRIES];
    static const enum Item second[] = {ITEM_FIGY_BERRY, ITEM_IAPAPA_BERRY};
    for (u32 first = FIRST_KIRI_BERRY; first <= LAST_KIRI_BERRY; first++)
    for (u32 choice = 0; choice < ARRAY_COUNT(second); choice++)
    for (u32 freeSlots = 0; freeSlots <= 2; freeSlots++)
    {
        ClearBag();
        for (u32 slot = freeSlots; slot < berries->capacity; slot++)
            BagPocket_SetSlotItemIdAndCount(berries, slot, ITEM_ORAN_BERRY, MAX_BAG_ITEM_CAPACITY);
        FlagClear(FLAG_DAILY_SOOTOPOLIS_RECEIVED_BERRY);
        gSpecialVar_0x8008 = first;
        gSpecialVar_0x8009 = second[choice];
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, SootopolisCity_EventScript_KiriCheckBerryPair);
        for (u32 step = 0; step < 8 && ctx.scriptPtr != SootopolisCity_EventScript_KiriGiveBerryPair
            && ctx.scriptPtr != Common_EventScript_ShowBagIsFull; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, freeSlots == 2 ? SootopolisCity_EventScript_KiriGiveBerryPair
                                              : Common_EventScript_ShowBagIsFull);
        EXPECT(!FlagGet(FLAG_DAILY_SOOTOPOLIS_RECEIVED_BERRY));
        EXPECT_EQ(CountTotalItemQuantityInBag(first), 0);
        EXPECT_EQ(CountTotalItemQuantityInBag(second[choice]), 0);
        if (freeSlots == 2)
        {
            EXPECT(AddBagItem(first, 1));
            EXPECT(AddBagItem(second[choice], 1));
        }
    }
    ClearBag();
}

extern const u8 BerryBlender_EventScript_CheckSpareBerry1[];
extern const u8 BerryBlender_EventScript_CheckSpareBerry2[];
extern const u8 BerryBlender_EventScript_UseBerryBlender1[];
extern const u8 BerryBlender_EventScript_ReceivedSpareBerry2[];

TEST("Campaign gifts: spare Berry receipts require successful delivery")
{
    static const struct { const u8 *gate, *success; } gifts[] = {
        {BerryBlender_EventScript_CheckSpareBerry1, BerryBlender_EventScript_UseBerryBlender1},
        {BerryBlender_EventScript_CheckSpareBerry2, BerryBlender_EventScript_ReceivedSpareBerry2},
    };
    for (u32 i = 0; i < ARRAY_COUNT(gifts); i++)
    for (u32 success = 0; success < 2; success++)
    {
        FlagClear(FLAG_DAILY_CONTEST_LOBBY_RECEIVED_BERRY);
        gSpecialVar_Result = success;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, gifts[i].gate);
        for (u32 step = 0; step < 8 && ctx.scriptPtr != gifts[i].success
            && ctx.scriptPtr != Common_EventScript_ShowBagIsFull; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, success ? gifts[i].success : Common_EventScript_ShowBagIsFull);
        EXPECT_EQ(FlagGet(FLAG_DAILY_CONTEST_LOBBY_RECEIVED_BERRY), success);
    }
    FlagClear(FLAG_DAILY_CONTEST_LOBBY_RECEIVED_BERRY);
}

extern const u8 SootopolisCity_LotadAndSeedotHouse_EventScript_NoRoomForElixir1[];
extern const u8 SootopolisCity_LotadAndSeedotHouse_EventScript_NoRoomForElixir2[];

TEST("Campaign gifts: size contests reject invalid entrants and preserve failed-prize retries")
{
    static const struct { enum Species species; u16 record; void (*compare)(void); const u8 *rollback; } contests[] = {
        {SPECIES_SEEDOT, VAR_SEEDOT_SIZE_RECORD, CompareSeedotSize, SootopolisCity_LotadAndSeedotHouse_EventScript_NoRoomForElixir1},
        {SPECIES_LOTAD, VAR_LOTAD_SIZE_RECORD, CompareLotadSize, SootopolisCity_LotadAndSeedotHouse_EventScript_NoRoomForElixir2},
    };
    for (u32 i = 0; i < ARRAY_COUNT(contests); i++)
    {
        ZeroPlayerPartyMons();
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], contests[i].species, 20, 0xFFFF, OTID_STRUCT_PLAYER_ID, 0);
        gSpecialVar_0x8004 = 0;
        gSpecialVar_Result = 0;
        VarSet(contests[i].record, 0x8000);
        gSpecialVar_0x8008 = 0x8000;
        contests[i].compare();
        EXPECT_EQ(gSpecialVar_Result, 3); // New record.
        EXPECT_EQ(VarGet(contests[i].record), 0xFFFF);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, contests[i].rollback);
        u8 command = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[command](&ctx)); // Actual rollback, before the UI message.
        EXPECT_EQ(VarGet(contests[i].record), 0x8000);
        gSpecialVar_Result = 0;
        contests[i].compare();
        EXPECT_EQ(gSpecialVar_Result, 3); // Same entrant remains eligible.
        contests[i].compare();
        EXPECT_EQ(gSpecialVar_Result, 2); // Equal size earns no second record.
        u32 egg = TRUE;
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &egg);
        contests[i].compare();
        EXPECT_EQ(gSpecialVar_Result, 1);
        EXPECT_EQ(VarGet(contests[i].record), 0xFFFF);
        gSpecialVar_Result = PARTY_NOTHING_CHOSEN;
        gSpecialVar_0x8004 = PARTY_NOTHING_CHOSEN;
        contests[i].compare();
        EXPECT_EQ(gSpecialVar_Result, 0);
        EXPECT_EQ(VarGet(contests[i].record), 0xFFFF);
        VarSet(contests[i].record, 0x8000);
    }
    ZeroPlayerPartyMons();
}

extern const u8 SootopolisCity_Gym_1F_EventScript_GiveScald2[];
extern const u8 Common_EventScript_BagIsFull[];
extern const u8 EventScript_PlayFanfareObtainedItem[];

TEST("Campaign gifts: Juan's delayed stone delivery keeps the badge and retries a full pocket")
{
    ClearBag();
    struct BagPocket *stones = &gBagPockets[POCKET_MEGA_STONES];
    for (u32 slot = 0; slot < stones->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(stones, slot, ITEM_ABOMASITE, MAX_BAG_ITEM_CAPACITY);
    FlagSet(FLAG_BADGE08_GET);
    FlagClear(FLAG_RECEIVED_TM03);
    for (u32 hasRoom = 0; hasRoom < 2; hasRoom++)
    {
        if (hasRoom)
            BagPocket_SetSlotItemIdAndCount(stones, 0, ITEM_NONE, 0);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, SootopolisCity_Gym_1F_EventScript_GiveScald2);
        for (u32 step = 0; step < 80 && ctx.scriptPtr != Common_EventScript_BagIsFull
            && ctx.scriptPtr != EventScript_PlayFanfareObtainedItem; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, hasRoom ? EventScript_PlayFanfareObtainedItem : Common_EventScript_BagIsFull);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_FERALIGITE), hasRoom);
        EXPECT(FlagGet(FLAG_BADGE08_GET));
        // Receipt follows the item popup; neither stopped path has reached it.
        EXPECT(!FlagGet(FLAG_RECEIVED_TM03));
    }
    ClearBag();
    FlagClear(FLAG_BADGE08_GET);
}

TEST("Campaign gifts: bundle simulation works without heap and never grants partial items")
{
    bool32 fits = FALSE;
    PARAMETRIZE { fits = FALSE; }
    PARAMETRIZE { fits = TRUE; }
    const struct ItemSlot gifts[] = {
        {ITEM_LEFTOVERS, 1}, {ITEM_LATIOSITE, 1}, {ITEM_LATIASITE, 1},
        {ITEM_ORAN_BERRY, 1}, {ITEM_ORAN_BERRY, 1}, {ITEM_POTION, 2},
    };
    ClearBag();
    struct BagPocket *battle = &gBagPockets[POCKET_BATTLE];
    for (u32 i = 0; i + 1 < battle->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(battle, i, ITEM_CHOICE_BAND, MAX_BAG_ITEM_CAPACITY);
    BagPocket_SetSlotItemIdAndCount(&gBagPockets[POCKET_BERRIES], 0, ITEM_ORAN_BERRY, fits ? 997 : 998);
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    bool32 result = CheckBagHasSpaceForItemBundle(gifts, ARRAY_COUNT(gifts));
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    EXPECT_EQ(result, fits);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LEFTOVERS), 0);
    EXPECT_EQ(GetBagItemId(POCKET_BATTLE, battle->capacity - 1), ITEM_NONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LATIOSITE), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LATIASITE), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POTION), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ORAN_BERRY), fits ? 997 : 998);
}

extern const u8 SouthernIsland_Interior_EventScript_Sign[];
extern const u8 SouthernIsland_Interior_EventScript_SignOnly[];
extern const u8 SouthernIsland_Interior_EventScript_SignThenStones[];
extern const u8 SouthernIsland_Interior_EventScript_SignStonesLost[];

TEST("Campaign gifts: Southern Island's Eon stones follow only a capture")
{
    bool8 saved[3] = {FlagGet(FLAG_RECEIVED_LATI_STONES), FlagGet(FLAG_CAUGHT_LATIAS_OR_LATIOS),
                      FlagGet(FLAG_DEFEATED_LATIAS_OR_LATIOS)};
    const u16 flags[] = {FLAG_RECEIVED_LATI_STONES, FLAG_CAUGHT_LATIAS_OR_LATIOS, FLAG_DEFEATED_LATIAS_OR_LATIOS};
    for (u32 mask = 0; mask < 8; mask++)
    {
        for (u32 i = 0; i < ARRAY_COUNT(flags); i++)
        {
            if (mask & (1u << i)) FlagSet(flags[i]);
            else FlagClear(flags[i]);
        }
        // Received stones end it; a capture earns them (or a Bag-full retry);
        // a knockout loses them with the Pokémon; before any result, nothing.
        const u8 *expected = (mask & 1) ? SouthernIsland_Interior_EventScript_SignOnly
            : (mask & 2) ? SouthernIsland_Interior_EventScript_SignThenStones
            : (mask & 4) ? SouthernIsland_Interior_EventScript_SignStonesLost
            : SouthernIsland_Interior_EventScript_SignOnly;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, SouthernIsland_Interior_EventScript_Sign);
        for (u32 step = 0; step < 8
          && ctx.scriptPtr != SouthernIsland_Interior_EventScript_SignOnly
          && ctx.scriptPtr != SouthernIsland_Interior_EventScript_SignThenStones
          && ctx.scriptPtr != SouthernIsland_Interior_EventScript_SignStonesLost; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, expected);
    }
    for (u32 i = 0; i < ARRAY_COUNT(flags); i++)
    {
        if (saved[i]) FlagSet(flags[i]);
        else FlagClear(flags[i]);
    }
}
