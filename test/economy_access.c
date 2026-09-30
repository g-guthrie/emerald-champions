#include "global.h"
#include "test/test.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "pokedex.h"
#include "pokemon.h"

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
