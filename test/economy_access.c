#include "global.h"
#include "test/test.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"

extern bool32 Test_TryTakeMonItem(struct Pokemon *mon);
extern void Test_StoragePlaceMon(struct Pokemon *mon, bool32 fromParty, bool32 toParty);

static void ResetEconomyDiscovery(void)
{
    ClearBag();
    ZeroPlayerPartyMons();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            ZeroBoxMonAt(box, slot);
    gPokemonStoragePtr->currentBox = 0;
}

TEST("Economy access: moving held gear through Bag and PC never introduces vendor stock")
{
    u32 route;
    PARAMETRIZE { route = 0; } // Party Take Item.
    PARAMETRIZE { route = 1; } // Direct held-item return.
    PARAMETRIZE { route = 2; } // Capture-swap / fusion boxing.
    PARAMETRIZE { route = 3; } // Storage deposit.
    ResetEconomyDiscovery();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_THROAT_SPRAY;
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    if (route == 0)
        EXPECT(Test_TryTakeMonItem(mon));
    else if (route == 1)
        EXPECT(ReturnBoxMonHeldItemToBag(&mon->box));
    else if (route == 2)
        EXPECT_EQ(CopyMonToPC(mon), MON_GIVEN_TO_PC);
    else
        Test_StoragePlaceMon(mon, TRUE, FALSE);
    EXPECT_EQ(CountTotalItemQuantityInBag(item), 1);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));

    EXPECT(AddPCItemWithoutDiscovery(item, 1));
    EXPECT(RemoveBagItem(item, 1));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT(AddBagItemWithoutDiscovery(item, 1));
    RemovePCItem(0, 1);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    // An ordinary world acquisition still unlocks the shelf; transfers cannot
    // revoke an earlier discovery even when a battle-found copy already exists.
    EXPECT(AddBagItem(item, 1));
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT(AddPCItemWithoutDiscovery(item, 1));
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    ResetEconomyDiscovery();
}

TEST("Economy access: actual captured and gifted held gear unlocks for both delivery destinations")
{
    bool32 captured, toPC;
    PARAMETRIZE { captured = FALSE; toPC = FALSE; }
    PARAMETRIZE { captured = FALSE; toPC = TRUE; }
    PARAMETRIZE { captured = TRUE; toPC = FALSE; }
    PARAMETRIZE { captured = TRUE; toPC = TRUE; }
    ResetEconomyDiscovery();
    if (toPC)
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
            CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_EEVEE, 10, slot, OTID_STRUCT_PLAYER_ID);
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_THROAT_SPRAY;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    u32 result = captured ? GiveCapturedMonToPlayer(&mon) : GiveScriptedMonToPlayer(&mon, PARTY_SIZE);
    EXPECT_EQ(result, toPC ? MON_GIVEN_TO_PC : MON_GIVEN_TO_PARTY);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    if (toPC)
        EXPECT_EQ(CountTotalItemQuantityInBag(item), 1);
    else
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), item);
    ResetEconomyDiscovery();
}

TEST("Economy access: species equipment has a paid first source without discovering an impossible item")
{
    u8 caught[NUM_DEX_FLAG_BYTES];
    u8 unlocked[sizeof(gSaveBlock1Ptr->battleItemsUnlocked)];
    memcpy(caught, gSaveBlock1Ptr->dexCaught, sizeof(caught));
    memcpy(unlocked, gSaveBlock1Ptr->battleItemsUnlocked, sizeof(unlocked));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(caught));
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(unlocked));

    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_LUCKY_PUNCH));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_FIRE_MEMORY));
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_TYPE_NULL), FLAG_SET_CAUGHT);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_FIRE_MEMORY));
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_CHANSEY), FLAG_SET_CAUGHT);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_LUCKY_PUNCH));
    EXPECT_EQ(gSpeciesInfo[SPECIES_CHANSEY].itemRare, ITEM_LUCKY_PUNCH);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_SILVALLY), FLAG_SET_CAUGHT);
    for (u32 item = 1; item < ITEMS_COUNT; item++)
        if (gItemsInfo[item].sortType == ITEM_TYPE_MEMORY)
            EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_THROAT_SPRAY));
    EmeraldChampions_UnlockBattleItem(ITEM_THROAT_SPRAY);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_THROAT_SPRAY));
    EXPECT_GT(GetItemPrice(ITEM_FIRE_MEMORY), 0);
    memcpy(gSaveBlock1Ptr->dexCaught, caught, sizeof(caught));
    memcpy(gSaveBlock1Ptr->battleItemsUnlocked, unlocked, sizeof(unlocked));
}

TEST("Economy access: finite stone bundle preflight reserves only missing copies")
{
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    struct BagPocket *mega = &gBagPockets[GetItemPocket(ITEM_LATIOSITE)];
    for (u32 slot = 0; slot < mega->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(mega, slot, ITEM_GARDEVOIRITE, MAX_BAG_ITEM_CAPACITY);
    EXPECT(!CanReceiveLatiStones());
    EXPECT(AddPCItem(ITEM_LATIOSITE, 1));
    EXPECT(!CanReceiveLatiStones()); // Latiasite still needs a slot.
    BagPocket_SetSlotItemIdAndCount(mega, 0, ITEM_NONE, 0);
    EXPECT(CanReceiveLatiStones());
    BagPocket_SetSlotItemIdAndCount(mega, 0, ITEM_GARDEVOIRITE, MAX_BAG_ITEM_CAPACITY);
    EXPECT(AddPCItem(ITEM_LATIASITE, 1));
    EXPECT(CanReceiveLatiStones()); // Both discoveries can pay money.

    FlagClear(FLAG_SHOALCAVE_SLOWBRONITE);
    EXPECT(!CanReceiveShellBellReward());
    EXPECT(AddPCItem(ITEM_SLOWBRONITE, 1));
    EXPECT(CanReceiveShellBellReward());
    struct BagPocket *battle = &gBagPockets[GetItemPocket(ITEM_SHELL_BELL)];
    for (u32 slot = 0; slot < battle->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(battle, slot, ITEM_CHOICE_BAND, MAX_BAG_ITEM_CAPACITY);
    EXPECT(!CanReceiveShellBellReward()); // The ordinary crafted item still needs room.
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
}
