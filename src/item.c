#include "global.h"
#include "item.h"
#include "berry.h"
#include "string_util.h"
#include "text.h"
#include "event_data.h"
#include "malloc.h"
#include "secret_base.h"
#include "item_menu.h"
#include "strings.h"
#include "load_save.h"
#include "item_use.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "constants/items.h"
#include "constants/hold_effects.h"

extern u16 gUnknown_0203CF30[];

// this file's functions
static bool8 CheckPyramidBagHasItem(u16 itemId, u16 count);
static bool8 CheckPyramidBagHasSpace(u16 itemId, u16 count);
static void UnlockBattleItem(u16 itemId);
static bool8 TryAddVerdantItemBundle(const u16 *itemIds, u8 count);
static bool8 TryAddItemQuantityBundle(const struct ItemSlot *items, u8 count);

struct BattleItemUnlock
{
    u16 itemId;
    u8 minimumBadges;
};

#define DISCOVERY_ONLY 0xFF

static const struct BattleItemUnlock sBattleItemUnlocks[] =
{
    {ITEM_MENTAL_HERB,      0},
    {ITEM_RED_CARD,         0},
    {ITEM_CELL_BATTERY,     0},
    {ITEM_EJECT_BUTTON,     1},
    {ITEM_ABSORB_BULB,      1},
    {ITEM_WHITE_HERB,       2},
    {ITEM_SNOWBALL,         2},
    {ITEM_LUMINOUS_MOSS,    3},
    {ITEM_POWER_HERB,       4},
    {ITEM_AIR_BALLOON,      4},
    {ITEM_ELECTRIC_SEED,    5},
    {ITEM_GRASSY_SEED,      5},
    {ITEM_MISTY_SEED,       5},
    {ITEM_PSYCHIC_SEED,     5},
    {ITEM_WEAKNESS_POLICY,  6},
    {ITEM_FOCUS_SASH,       0},
    {ITEM_BRIGHT_POWDER,    DISCOVERY_ONLY},
    {ITEM_QUICK_CLAW,       DISCOVERY_ONLY},
    {ITEM_CHOICE_BAND,      0},
    {ITEM_KINGS_ROCK,       DISCOVERY_ONLY},
    {ITEM_FOCUS_BAND,       DISCOVERY_ONLY},
    {ITEM_SCOPE_LENS,       DISCOVERY_ONLY},
    {ITEM_LEFTOVERS,        0},
    {ITEM_RAZOR_FANG,       DISCOVERY_ONLY},
    {ITEM_CHOICE_SCARF,     0},
    {ITEM_CHOICE_SPECS,     0},
    {ITEM_WIDE_LENS,        DISCOVERY_ONLY},
    {ITEM_ZOOM_LENS,        DISCOVERY_ONLY},
    {ITEM_METRONOME,        DISCOVERY_ONLY},
    {ITEM_MUSCLE_BAND,      DISCOVERY_ONLY},
    {ITEM_WISE_GLASSES,     DISCOVERY_ONLY},
    {ITEM_EXPERT_BELT,      DISCOVERY_ONLY},
    {ITEM_LIGHT_CLAY,       DISCOVERY_ONLY},
    {ITEM_LIFE_ORB,         0},
    {ITEM_TOXIC_ORB,        DISCOVERY_ONLY},
    {ITEM_FLAME_ORB,        DISCOVERY_ONLY},
    {ITEM_BLACK_SLUDGE,     DISCOVERY_ONLY},
    {ITEM_SHED_SHELL,       DISCOVERY_ONLY},
    {ITEM_EVIOLITE,         0},
    {ITEM_ROCKY_HELMET,     0},
    {ITEM_ASSAULT_VEST,     0},
    {ITEM_SAFETY_GOGGLES,   DISCOVERY_ONLY},
    {ITEM_ADRENALINE_ORB,   DISCOVERY_ONLY},
    {ITEM_TERRAIN_EXTENDER, DISCOVERY_ONLY},
    {ITEM_PROTECTIVE_PADS,  DISCOVERY_ONLY},
    {ITEM_THROAT_SPRAY,     DISCOVERY_ONLY},
    {ITEM_EJECT_PACK,       DISCOVERY_ONLY},
    {ITEM_HEAVY_DUTY_BOOTS, 0},
    {ITEM_BLUNDER_POLICY,   DISCOVERY_ONLY},
    {ITEM_ROOM_SERVICE,     DISCOVERY_ONLY},
    {ITEM_BOOSTER_ENERGY,   DISCOVERY_ONLY},
    {ITEM_WELLSPRING_MASK,  DISCOVERY_ONLY},
    {ITEM_HEARTHFLAME_MASK, DISCOVERY_ONLY},
    {ITEM_CORNERSTONE_MASK, DISCOVERY_ONLY},
    {ITEM_UTILITY_UMBRELLA, DISCOVERY_ONLY},
};

static u16 GetBattleItemUnlockFlag(u16 index)
{
    if (index <= FLAG_UNUSED_0x2BB - FLAG_UNUSED_0x293)
        return FLAG_UNUSED_0x293 + index;
    if (index == 41)
        return FLAG_UNUSED_0x8E3;
    if (index < 50)
        return FLAG_UNUSED_0x8EB + index - 42;
    if (index == 54)
        return FLAG_VERDANT_BATTLE_ITEM_UTILITY_UMBRELLA;
    return FLAG_UNUSED_0x900 + index - 50;
}

static u8 GetBadgeCount(void)
{
    u8 i;
    u8 count = 0;

    for (i = 0; i < NUM_BADGES; i++)
        if (FlagGet(FLAG_BADGE01_GET + i))
            count++;
    return count;
}

static void UnlockBattleItem(u16 itemId)
{
    u16 i;

    for (i = 0; i < ARRAY_COUNT(sBattleItemUnlocks); i++)
    {
        if (sBattleItemUnlocks[i].itemId == itemId)
        {
            FlagSet(GetBattleItemUnlockFlag(i));
            return;
        }
    }
}

bool8 PlayerOwnsItemAnywhere(u16 itemId)
{
    u8 i;
    u8 boxId;
    u8 boxPosition;

    if (CheckBagHasItem(itemId, 1) || CheckPCHasItem(itemId, 1))
        return TRUE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gPlayerParty[i], MON_DATA_SPECIES) != SPECIES_NONE
         && GetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM) == itemId)
            return TRUE;
    }

    for (i = 0; i < DAYCARE_MON_COUNT; i++)
    {
        if (GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[i].mon, MON_DATA_SPECIES) != SPECIES_NONE
         && GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[i].mon, MON_DATA_HELD_ITEM) == itemId)
            return TRUE;
    }

    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            if (GetBoxMonDataAt(boxId, boxPosition, MON_DATA_SPECIES) != SPECIES_NONE
             && GetBoxMonDataAt(boxId, boxPosition, MON_DATA_HELD_ITEM) == itemId)
                return TRUE;
        }
    }

    return FALSE;
}

static bool8 TryAddVerdantItemBundle(const u16 *itemIds, u8 count)
{
    bool8 added[5] = {FALSE};
    u8 i;

    if (count > ARRAY_COUNT(added))
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (PlayerOwnsItemAnywhere(itemIds[i]))
            continue;

        if (!AddBagItem(itemIds[i], 1))
        {
            while (i > 0)
            {
                i--;
                if (added[i])
                    RemoveBagItem(itemIds[i], 1);
            }
            return FALSE;
        }
        added[i] = TRUE;
    }

    return TRUE;
}

static bool8 TryAddItemQuantityBundle(const struct ItemSlot *items, u8 count)
{
    bool8 added[4] = {FALSE};
    u8 i;

    if (count > ARRAY_COUNT(added))
        return FALSE;

    for (i = 0; i < count; i++)
    {
        if (items[i].quantity == 0)
            continue;
        if (!AddBagItem(items[i].itemId, items[i].quantity))
        {
            while (i > 0)
            {
                i--;
                if (added[i])
                    RemoveBagItem(items[i].itemId, items[i].quantity);
            }
            return FALSE;
        }
        added[i] = TRUE;
    }

    return TRUE;
}

bool8 TryAddVerdantMegaKit(void)
{
    static const u16 sMegaKit[] =
    {
        ITEM_MEGA_BRACELET,
        ITEM_SCEPTILITE,
        ITEM_BLAZIKENITE,
        ITEM_SWAMPERTITE,
    };

    return TryAddVerdantItemBundle(sMegaKit, ARRAY_COUNT(sMegaKit));
}

bool8 TryAddVerdantStevenRewardBundle(void)
{
    static const u16 sStevenRewardBundle[] =
    {
        ITEM_WIDE_LENS,
        ITEM_MEGA_BRACELET,
        ITEM_SCEPTILITE,
        ITEM_BLAZIKENITE,
        ITEM_SWAMPERTITE,
    };

    return TryAddVerdantItemBundle(sStevenRewardBundle, ARRAY_COUNT(sStevenRewardBundle));
}

bool8 TryAddVerdantLatiStoneBundle(void)
{
    static const u16 sLatiStoneBundle[] =
    {
        ITEM_LATIOSITE,
        ITEM_LATIASITE,
    };

    return TryAddVerdantItemBundle(sLatiStoneBundle, ARRAY_COUNT(sLatiStoneBundle));
}

bool8 TryAddVerdantWeatherRockBundle(void)
{
    static const u16 sWeatherRockBundle[] =
    {
        ITEM_HEAT_ROCK,
        ITEM_DAMP_ROCK,
        ITEM_ICY_ROCK,
        ITEM_SMOOTH_ROCK,
    };

    return TryAddVerdantItemBundle(sWeatherRockBundle, ARRAY_COUNT(sWeatherRockBundle));
}

bool8 TryAddEmeraldChampionsGymRewardMigration(void)
{
    u16 items[4];
    u8 count = 0;

    if (FlagGet(FLAG_RECEIVED_TM50))
        items[count++] = ITEM_EJECT_PACK;
    if (FlagGet(FLAG_RECEIVED_TM51))
        items[count++] = ITEM_ADRENALINE_ORB;
    if (FlagGet(FLAG_RECEIVED_TM03))
        items[count++] = ITEM_UTILITY_UMBRELLA;
    if (FlagGet(FLAG_RECEIVED_LIFE_ORB))
        items[count++] = ITEM_DESTINY_KNOT;

    return TryAddVerdantItemBundle(items, count);
}

bool8 TryAddEmeraldChampionsItemBallMigration(void)
{
    struct ItemSlot items[4] = {0};

    items[0].itemId = ITEM_PP_MAX;
    items[0].quantity =
        FlagGet(FLAG_ITEM_ROUTE_109_RARE_CANDY)
      + FlagGet(FLAG_ITEM_SHOAL_CAVE_INNER_ROOM_RARE_CANDY)
      + FlagGet(FLAG_ITEM_ROUTE_114_TM53_PSYSHOCK)
      + FlagGet(FLAG_ITEM_JAGGED_PASS_TM69_ROCK_POLISH)
      + FlagGet(FLAG_ITEM_METEOR_FALLS_B1F_2R_TM_02)
      + FlagGet(FLAG_ITEM_SCORCHED_SLAB_TM_11)
      + FlagGet(FLAG_ITEM_MT_PYRE_EXTERIOR_TM_48)
      + FlagGet(FLAG_ITEM_SHOAL_CAVE_ICE_ROOM_TM_07)
      + FlagGet(FLAG_EMBER_PATH_SMACK_DOWN)
      + FlagGet(FLAG_TM21_FRUSTRATION);

    items[1].itemId = ITEM_GOLD_BOTTLE_CAP;
    items[1].quantity =
        FlagGet(FLAG_ITEM_LILYCOVE_CITY_LIGHT_CLAY)
      + FlagGet(FLAG_ITEM_TRICK_HOUSE_PUZZLE_8_DESTINY_KNOT)
      + FlagGet(FLAG_ITEM_ROUTE_105_ABILITY_PATCH)
      + FlagGet(FLAG_ITEM_ROUTE_115_TM_01)
      + FlagGet(FLAG_ITEM_ROUTE_123_TM99_DAZZLING_GLEAM)
      + FlagGet(FLAG_ITEM_METEOR_FALLS_1F_1R_TM_23)
      + FlagGet(FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_1_TM_18)
      + FlagGet(FLAG_ITEM_MT_PYRE_6F_TM_30)
      + FlagGet(FLAG_ITEM_SHOAL_CAVE_STAIRS_ROOM_TM70_AURORA_VEIL)
      + FlagGet(FLAG_ITEM_VICTORY_ROAD_B1F_TM_29)
      + FlagGet(FLAG_ITEM_AQUA_HIDEOUT_B1F_TM97_DARK_PULSE);

    items[2].itemId = ITEM_SPORT_BALL;
    items[2].quantity =
        FlagGet(FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_TM_13)
      + FlagGet(FLAG_ITEM_SAFARI_ZONE_NORTH_WEST_TM_22)
      + FlagGet(FLAG_ASHEN_WOODS_U_TURN);

    items[3].itemId = ITEM_BEAST_BALL;
    items[3].quantity =
        FlagGet(FLAG_ITEM_SAFARI_ZONE_SOUTH_EAST_TM53_ENERGY_BALL)
      + FlagGet(FLAG_ITEM_ROUTE_124_TM85_DREAM_EATER)
      + FlagGet(FLAG_TM93_WILD_CHARGE);

    return TryAddItemQuantityBundle(items, ARRAY_COUNT(items));
}

u16 BuildUnlockedBattleItemList(u16 *items, u16 capacity)
{
    u16 i;
    u16 count = 0;
    u8 partyIndex;
    u8 boxId;
    u8 boxPosition;
    u8 badgeCount = GetBadgeCount();

    for (partyIndex = 0; partyIndex < PARTY_SIZE; partyIndex++)
    {
        if (GetMonData(&gPlayerParty[partyIndex], MON_DATA_SPECIES) != SPECIES_NONE)
            UnlockBattleItem(GetMonData(&gPlayerParty[partyIndex], MON_DATA_HELD_ITEM));
    }
    for (partyIndex = 0; partyIndex < DAYCARE_MON_COUNT; partyIndex++)
    {
        if (GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[partyIndex].mon, MON_DATA_SPECIES) != SPECIES_NONE)
            UnlockBattleItem(GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[partyIndex].mon, MON_DATA_HELD_ITEM));
    }
    for (boxId = 0; boxId < TOTAL_BOXES_COUNT; boxId++)
    {
        for (boxPosition = 0; boxPosition < IN_BOX_COUNT; boxPosition++)
        {
            if (GetBoxMonDataAt(boxId, boxPosition, MON_DATA_SPECIES) != SPECIES_NONE)
                UnlockBattleItem(GetBoxMonDataAt(boxId, boxPosition, MON_DATA_HELD_ITEM));
        }
    }

    for (i = 0; i < ARRAY_COUNT(sBattleItemUnlocks); i++)
    {
        if (CheckBagHasItem(sBattleItemUnlocks[i].itemId, 1)
         || CheckPCHasItem(sBattleItemUnlocks[i].itemId, 1))
            FlagSet(GetBattleItemUnlockFlag(i));

        if ((FlagGet(GetBattleItemUnlockFlag(i))
          || (sBattleItemUnlocks[i].minimumBadges != DISCOVERY_ONLY
           && badgeCount >= sBattleItemUnlocks[i].minimumBadges))
         && count + 1 < capacity)
            items[count++] = sBattleItemUnlocks[i].itemId;
    }
    items[count] = ITEM_NONE;
    return count;
}

// EWRAM variables
EWRAM_DATA struct BagPocket gBagPockets[POCKETS_COUNT] = {0};

// rodata
#include "data/text/item_descriptions.h"
#include "data/items.h"

// code
static u16 GetBagItemQuantity(u16 *quantity)
{
    return gSaveBlock2Ptr->encryptionKey ^ *quantity;
}

static void SetBagItemQuantity(u16 *quantity, u16 newValue)
{
    *quantity =  newValue ^ gSaveBlock2Ptr->encryptionKey;
}

static u16 GetPCItemQuantity(u16 *quantity)
{
    return *quantity;
}

static void SetPCItemQuantity(u16 *quantity, u16 newValue)
{
    *quantity = newValue;
}

void ApplyNewEncryptionKeyToBagItems(u32 newKey)
{
    u32 pocket, item;
    for (pocket = 0; pocket < POCKETS_COUNT; pocket++)
    {
        for (item = 0; item < gBagPockets[pocket].capacity; item++)
            ApplyNewEncryptionKeyToHword(&(gBagPockets[pocket].itemSlots[item].quantity), newKey);
    }
}

void ApplyNewEncryptionKeyToBagItems_(u32 newKey) // really GF?
{
    ApplyNewEncryptionKeyToBagItems(newKey);
}

void SetBagItemsPointers(void)
{
    gBagPockets[ITEMS_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_Items;
    gBagPockets[ITEMS_POCKET].capacity = BAG_ITEMS_COUNT;

    gBagPockets[MEDICINE_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_Medicine;
    gBagPockets[MEDICINE_POCKET].capacity = BAG_MEDICINE_COUNT;

    gBagPockets[BATTLE_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_Battle;
    gBagPockets[BATTLE_POCKET].capacity = BAG_BATTLE_COUNT;

    gBagPockets[TMHM_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_TMHM;
    gBagPockets[TMHM_POCKET].capacity = BAG_TMHM_COUNT;

    gBagPockets[BERRIES_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_Berries;
    gBagPockets[BERRIES_POCKET].capacity = BAG_BERRIES_COUNT;

    gBagPockets[BALLS_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_PokeBalls;
    gBagPockets[BALLS_POCKET].capacity = BAG_POKEBALLS_COUNT;

    gBagPockets[KEYITEMS_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_KeyItems;
    gBagPockets[KEYITEMS_POCKET].capacity = BAG_KEYITEMS_COUNT;

    gBagPockets[MEGA_STONES_POCKET].itemSlots = gSaveBlock1Ptr->bagPocket_MegaStones;
    gBagPockets[MEGA_STONES_POCKET].capacity = BAG_MEGASTONES_COUNT;
}

void CopyItemName(u16 itemId, u8 *dst)
{
    StringCopy(dst, ItemId_GetName(itemId));
}

static const u8 sText_s[] = _("s");
void CopyItemNameHandlePlural(u16 itemId, u8 *dst, u32 quantity)
{
    StringCopy(dst, ItemId_GetName(itemId));
    if (quantity > 1)
    {
        if (ItemId_GetPocket(itemId) == POCKET_BERRIES)
            GetBerryCountString(dst, gBerries[itemId - ITEM_CHERI_BERRY].name, quantity);
        else
            StringAppend(dst, sText_s);
    }
}

void GetBerryCountString(u8 *dst, const u8 *berryName, u32 quantity)
{
    const u8 *berryString;
    u8 *txtPtr;

    if (quantity < 2)
        berryString = gText_Berry;
    else
        berryString = gText_Berries;

    txtPtr = StringCopy(dst, berryName);
    *txtPtr = CHAR_SPACE;
    StringCopy(txtPtr + 1, berryString);
}

bool8 IsBagPocketNonEmpty(u8 pocket)
{
    u8 i;

    for (i = 0; i < gBagPockets[pocket - 1].capacity; i++)
    {
        if (gBagPockets[pocket - 1].itemSlots[i].itemId != 0)
            return TRUE;
    }
    return FALSE;
}

bool8 CheckBagHasItem(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;

    if (ItemId_GetPocket(itemId) == 0)
        return FALSE;
    if (InBattlePyramid() || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
        return CheckPyramidBagHasItem(itemId, count);
    pocket = ItemId_GetPocket(itemId) - 1;
    // Check for item slots that contain the item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            u16 quantity;
            // Does this item slot contain enough of the item?
            quantity = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
            if (quantity >= count)
                return TRUE;
            count -= quantity;
            // Does this item slot and all previous slots contain enough of the item?
            if (count == 0)
                return TRUE;
        }
    }
    return FALSE;
}

bool8 HasAtLeastOneBerry(void)
{
    u16 i;

    for (i = FIRST_BERRY_INDEX; i < LAST_BERRY_INDEX + 1; i++)
    {
        if (CheckBagHasItem(i, 1) == TRUE)
        {
            gSpecialVar_Result = TRUE;
            return TRUE;
        }
    }
    gSpecialVar_Result = FALSE;
    return FALSE;
}

bool8 CheckBagHasSpace(u16 itemId, u16 count)
{
    u8 i;
    u8 pocket;
    u16 slotCapacity;
    u16 ownedCount;

    if (ItemId_GetPocket(itemId) == POCKET_NONE)
        return FALSE;

    if (InBattlePyramid() || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
    {
        return CheckPyramidBagHasSpace(itemId, count);
    }

    pocket = ItemId_GetPocket(itemId) - 1;
    if (pocket != BERRIES_POCKET)
        slotCapacity = MAX_BAG_ITEM_CAPACITY;
    else
        slotCapacity = MAX_BERRY_CAPACITY;

    // Check space in any existing item slots that already contain this item
    for (i = 0; i < gBagPockets[pocket].capacity; i++)
    {
        if (gBagPockets[pocket].itemSlots[i].itemId == itemId)
        {
            ownedCount = GetBagItemQuantity(&gBagPockets[pocket].itemSlots[i].quantity);
            if (ownedCount + count <= slotCapacity)
                return TRUE;
            if (pocket == TMHM_POCKET || pocket == BERRIES_POCKET)
                return FALSE;
            count -= (slotCapacity - ownedCount);
            if (count == 0)
                break; //should be return TRUE, but that doesn't match
        }
    }

    // Check space in empty item slots
    if (count > 0)
    {
        for (i = 0; i < gBagPockets[pocket].capacity; i++)
        {
            if (gBagPockets[pocket].itemSlots[i].itemId == 0)
            {
                if (count > slotCapacity)
                {
                    if (pocket == TMHM_POCKET || pocket == BERRIES_POCKET)
                        return FALSE;
                    count -= slotCapacity;
                }
                else
                {
                    count = 0; //should be return TRUE, but that doesn't match
                    break;
                }
            }
        }
        if (count > 0)
            return FALSE; // No more item slots. The bag is full
    }

    return TRUE;
}

bool8 AddBagItem(u16 itemId, u16 count)
{
    u8 i;

    if (ItemId_GetPocket(itemId) == POCKET_NONE)
        return FALSE;

    // check Battle Pyramid Bag
    if (InBattlePyramid() || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
    {
        return AddPyramidBagItem(itemId, count);
    }
    else
    {
        struct BagPocket *itemPocket;
        struct ItemSlot *newItems;
        u16 slotCapacity;
        u16 ownedCount;
        u8 pocket = ItemId_GetPocket(itemId) - 1;

        itemPocket = &gBagPockets[pocket];
        newItems = AllocZeroed(itemPocket->capacity * sizeof(struct ItemSlot));
        if (newItems == NULL)
            return FALSE;
        memcpy(newItems, itemPocket->itemSlots, itemPocket->capacity * sizeof(struct ItemSlot));

        if (pocket == BERRIES_POCKET)
        {
            slotCapacity = MAX_BERRY_CAPACITY;
        }
        else if (pocket == TMHM_POCKET)
        {
            slotCapacity = MAX_TMHM_CAPACITY;
        }
        else
        {
            slotCapacity = MAX_BAG_ITEM_CAPACITY;
        }    
  
        for (i = 0; i < itemPocket->capacity; i++)
        {
            if (newItems[i].itemId == itemId)
            {
                ownedCount = GetBagItemQuantity(&newItems[i].quantity);
                // check if won't exceed max slot capacity
                if (ownedCount + count <= slotCapacity)
                {
                    // successfully added to already existing item's count
                    SetBagItemQuantity(&newItems[i].quantity, ownedCount + count);
                    memcpy(itemPocket->itemSlots, newItems, itemPocket->capacity * sizeof(struct ItemSlot));
                    Free(newItems);
                    UnlockBattleItem(itemId);
                    return TRUE;
                }
                else
                {
                    // try creating another instance of the item if possible
                    if (pocket == TMHM_POCKET || pocket == BERRIES_POCKET)
                    {
                        Free(newItems);
                        return FALSE;
                    }
                    else
                    {
                        count -= slotCapacity - ownedCount;
                        SetBagItemQuantity(&newItems[i].quantity, slotCapacity);
                        // don't create another instance of the item if it's at max slot capacity and count is equal to 0
                        if (count == 0)
                        {
                            break;
                        }
                    }
                }
            }
        }

        // we're done if quantity is equal to 0
        if (count > 0)
        {
            // either no existing item was found or we have to create another instance, because the capacity was exceeded
            for (i = 0; i < itemPocket->capacity; i++)
            {
                if (newItems[i].itemId == ITEM_NONE)
                {
                    newItems[i].itemId = itemId;
                    if (count > slotCapacity)
                    {
                        // try creating a new slot with max capacity if duplicates are possible
                        if (pocket == TMHM_POCKET || pocket == BERRIES_POCKET)
                        {
                            Free(newItems);
                            return FALSE;
                        }
                        count -= slotCapacity;
                        SetBagItemQuantity(&newItems[i].quantity, slotCapacity);
                    }
                    else
                    {
                        // created a new slot and added quantity
                        SetBagItemQuantity(&newItems[i].quantity, count);
                        count = 0;
                        break;
                    }
                }
            }

            if (count > 0)
            {
                Free(newItems);
                return FALSE;
            }
        }
        memcpy(itemPocket->itemSlots, newItems, itemPocket->capacity * sizeof(struct ItemSlot));
        Free(newItems);
        UnlockBattleItem(itemId);
        return TRUE;
    }
}

bool8 RemoveBagItem(u16 itemId, u16 count)
{
    u8 i;
    u16 totalQuantity = 0;

    if (ItemId_GetPocket(itemId) == POCKET_NONE || itemId == ITEM_NONE)
        return FALSE;

    // check Battle Pyramid Bag
    if (InBattlePyramid() || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
    {
        return RemovePyramidBagItem(itemId, count);
    }
    else
    {
        u8 pocket;
        u8 var;
        u16 ownedCount;
        struct BagPocket *itemPocket;

        pocket = ItemId_GetPocket(itemId) - 1;
        itemPocket = &gBagPockets[pocket];

        for (i = 0; i < itemPocket->capacity; i++)
        {
            if (itemPocket->itemSlots[i].itemId == itemId)
                totalQuantity += GetBagItemQuantity(&itemPocket->itemSlots[i].quantity);
        }

        if (totalQuantity < count)
            return FALSE;   // We don't have enough of the item

        if (CurMapIsSecretBase() == TRUE)
        {
            VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_BAG);
            VarSet(VAR_SECRET_BASE_LAST_ITEM_USED, itemId);
        }

        var = GetItemListPosition(pocket);
        if (itemPocket->capacity > var
         && itemPocket->itemSlots[var].itemId == itemId)
        {
            ownedCount = GetBagItemQuantity(&itemPocket->itemSlots[var].quantity);
            if (ownedCount >= count)
            {
                SetBagItemQuantity(&itemPocket->itemSlots[var].quantity, ownedCount - count);
                count = 0;
            }
            else
            {
                count -= ownedCount;
                SetBagItemQuantity(&itemPocket->itemSlots[var].quantity, 0);
            }

            if (GetBagItemQuantity(&itemPocket->itemSlots[var].quantity) == 0)
                itemPocket->itemSlots[var].itemId = ITEM_NONE;

            if (count == 0)
                return TRUE;
        }

        for (i = 0; i < itemPocket->capacity; i++)
        {
            if (itemPocket->itemSlots[i].itemId == itemId)
            {
                ownedCount = GetBagItemQuantity(&itemPocket->itemSlots[i].quantity);
                if (ownedCount >= count)
                {
                    SetBagItemQuantity(&itemPocket->itemSlots[i].quantity, ownedCount - count);
                    count = 0;
                }
                else
                {
                    count -= ownedCount;
                    SetBagItemQuantity(&itemPocket->itemSlots[i].quantity, 0);
                }

                if (GetBagItemQuantity(&itemPocket->itemSlots[i].quantity) == 0)
                    itemPocket->itemSlots[i].itemId = ITEM_NONE;

                if (count == 0)
                    return TRUE;
            }
        }
        return TRUE;
    }
}

u8 GetPocketByItemId(u16 itemId)
{
    return ItemId_GetPocket(itemId);
}

void ClearItemSlots(struct ItemSlot *itemSlots, u8 itemCount)
{
    u16 i;

    for (i = 0; i < itemCount; i++)
    {
        itemSlots[i].itemId = ITEM_NONE;
        SetBagItemQuantity(&itemSlots[i].quantity, 0);
    }
}

static s32 FindFreePCItemSlot(void)
{
    s8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_NONE)
            return i;
    }
    return -1;
}

u8 CountUsedPCItemSlots(void)
{
    u8 usedSlots = 0;
    u8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId != ITEM_NONE)
            usedSlots++;
    }
    return usedSlots;
}

bool8 CheckPCHasItem(u16 itemId, u16 count)
{
    u8 i;

    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (gSaveBlock1Ptr->pcItems[i].itemId == itemId && GetPCItemQuantity(&gSaveBlock1Ptr->pcItems[i].quantity) >= count)
            return TRUE;
    }
    return FALSE;
}

bool8 AddPCItem(u16 itemId, u16 count)
{
    u8 i;
    s8 freeSlot;
    u16 ownedCount;
    struct ItemSlot *newItems;

    // Copy PC items
    newItems = AllocZeroed(sizeof(gSaveBlock1Ptr->pcItems));
    if (newItems == NULL)
        return FALSE;
    memcpy(newItems, gSaveBlock1Ptr->pcItems, sizeof(gSaveBlock1Ptr->pcItems));

    // Use any item slots that already contain this item
    for (i = 0; i < PC_ITEMS_COUNT; i++)
    {
        if (newItems[i].itemId == itemId)
        {
            ownedCount = GetPCItemQuantity(&newItems[i].quantity);
            if (ownedCount + count <= MAX_PC_ITEM_CAPACITY)
            {
                SetPCItemQuantity(&newItems[i].quantity, ownedCount + count);
                memcpy(gSaveBlock1Ptr->pcItems, newItems, sizeof(gSaveBlock1Ptr->pcItems));
                Free(newItems);
                return TRUE;
            }
            count += ownedCount - MAX_PC_ITEM_CAPACITY;
            SetPCItemQuantity(&newItems[i].quantity, MAX_PC_ITEM_CAPACITY);
            if (count == 0)
            {
                memcpy(gSaveBlock1Ptr->pcItems, newItems, sizeof(gSaveBlock1Ptr->pcItems));
                Free(newItems);
                return TRUE;
            }
        }
    }

    // Put any remaining items into a new item slot.
    if (count > 0)
    {
        freeSlot = FindFreePCItemSlot();
        if (freeSlot == -1)
        {
            Free(newItems);
            return FALSE;
        }
        else
        {
            newItems[freeSlot].itemId = itemId;
            SetPCItemQuantity(&newItems[freeSlot].quantity, count);
        }
    }

    // Copy items back to the PC
    memcpy(gSaveBlock1Ptr->pcItems, newItems, sizeof(gSaveBlock1Ptr->pcItems));
    Free(newItems);
    return TRUE;
}

void RemovePCItem(u8 index, u16 count)
{
    gSaveBlock1Ptr->pcItems[index].quantity -= count;
    if (gSaveBlock1Ptr->pcItems[index].quantity == 0)
    {
        gSaveBlock1Ptr->pcItems[index].itemId = ITEM_NONE;
        CompactPCItems();
    }
}

void CompactPCItems(void)
{
    u16 i;
    u16 j;

    for (i = 0; i < PC_ITEMS_COUNT - 1; i++)
    {
        for (j = i + 1; j < PC_ITEMS_COUNT; j++)
        {
            if (gSaveBlock1Ptr->pcItems[i].itemId == 0)
            {
                struct ItemSlot temp = gSaveBlock1Ptr->pcItems[i];
                gSaveBlock1Ptr->pcItems[i] = gSaveBlock1Ptr->pcItems[j];
                gSaveBlock1Ptr->pcItems[j] = temp;
            }
        }
    }
}

void SwapRegisteredBike(void)
{
    switch (gSaveBlock1Ptr->registeredItem)
    {
    case ITEM_MACH_BIKE:
        gSaveBlock1Ptr->registeredItem = ITEM_ACRO_BIKE;
        break;
    case ITEM_ACRO_BIKE:
        gSaveBlock1Ptr->registeredItem = ITEM_MACH_BIKE;
        break;
    }
}

u16 BagGetItemIdByPocketPosition(u8 pocketId, u16 pocketPos)
{
    return gBagPockets[pocketId - 1].itemSlots[pocketPos].itemId;
}

u16 BagGetQuantityByPocketPosition(u8 pocketId, u16 pocketPos)
{
    return GetBagItemQuantity(&gBagPockets[pocketId - 1].itemSlots[pocketPos].quantity);
}

static void SwapItemSlots(struct ItemSlot *a, struct ItemSlot *b)
{
    struct ItemSlot temp;
    SWAP(*a, *b, temp);
}

void CompactItemsInBagPocket(struct BagPocket *bagPocket)
{
    u16 i, j;

    for (i = 0; i < bagPocket->capacity - 1; i++)
    {
        for (j = i + 1; j < bagPocket->capacity; j++)
        {
            if (GetBagItemQuantity(&bagPocket->itemSlots[i].quantity) == 0)
                SwapItemSlots(&bagPocket->itemSlots[i], &bagPocket->itemSlots[j]);
        }
    }
}

void SortBerriesOrTMHMs(struct BagPocket *bagPocket)
{
    u16 i, j;

    for (i = 0; i < bagPocket->capacity - 1; i++)
    {
        for (j = i + 1; j < bagPocket->capacity; j++)
        {
            if (GetBagItemQuantity(&bagPocket->itemSlots[i].quantity) != 0)
            {
                if (GetBagItemQuantity(&bagPocket->itemSlots[j].quantity) == 0)
                    continue;
                if (bagPocket->itemSlots[i].itemId <= bagPocket->itemSlots[j].itemId)
                    continue;
            }
            SwapItemSlots(&bagPocket->itemSlots[i], &bagPocket->itemSlots[j]);
        }
    }
}

void MoveItemSlotInList(struct ItemSlot* itemSlots_, u32 from, u32 to_)
{
    // dumb assignments needed to match
    struct ItemSlot *itemSlots = itemSlots_;
    u32 to = to_;

    if (from != to)
    {
        s16 i, count;
        struct ItemSlot firstSlot = itemSlots[from];

        if (to > from)
        {
            to--;
            for (i = from, count = to; i < count; i++)
                itemSlots[i] = itemSlots[i + 1];
        }
        else
        {
            for (i = from, count = to; i > count; i--)
                itemSlots[i] = itemSlots[i - 1];
        }
        itemSlots[to] = firstSlot;
    }
}

void ClearBag(void)
{
    u16 i;

    for (i = 0; i < POCKETS_COUNT; i++)
    {
        ClearItemSlots(gBagPockets[i].itemSlots, gBagPockets[i].capacity);
    }
}

u16 CountTotalItemQuantityInBag(u16 itemId)
{
    u16 i;
    u16 ownedCount = 0;
    struct BagPocket *bagPocket = &gBagPockets[ItemId_GetPocket(itemId) - 1];

    for (i = 0; i < bagPocket->capacity; i++)
    {
        if (bagPocket->itemSlots[i].itemId == itemId)
            ownedCount += GetBagItemQuantity(&bagPocket->itemSlots[i].quantity);
    }

    return ownedCount;
}

static bool8 CheckPyramidBagHasItem(u16 itemId, u16 count)
{
    u8 i;
    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (items[i] == itemId)
        {
            if (quantities[i] >= count)
                return TRUE;

            count -= quantities[i];
            if (count == 0)
                return TRUE;
        }
    }

    return FALSE;
}

static bool8 CheckPyramidBagHasSpace(u16 itemId, u16 count)
{
    u8 i;
    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (items[i] == itemId || items[i] == ITEM_NONE)
        {
            if (quantities[i] + count <= MAX_PYRAMID_BAG_CAPACITY)
                return TRUE;

            count = (quantities[i] + count) - MAX_PYRAMID_BAG_CAPACITY;
            if (count == 0)
                return TRUE;
        }
    }

    return FALSE;
}

bool8 AddPyramidBagItem(u16 itemId, u16 count)
{
    u16 i;

    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    u16 *newItems = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
    u8 *newQuantities = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

    if (newItems == NULL || newQuantities == NULL)
    {
        Free(newItems);
        Free(newQuantities);
        return FALSE;
    }

    memcpy(newItems, items, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
    memcpy(newQuantities, quantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

    for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (newItems[i] == itemId && newQuantities[i] < MAX_PYRAMID_BAG_CAPACITY)
        {
            newQuantities[i] += count;
            if (newQuantities[i] > MAX_PYRAMID_BAG_CAPACITY)
            {
                count = newQuantities[i] - MAX_PYRAMID_BAG_CAPACITY;
                newQuantities[i] = MAX_PYRAMID_BAG_CAPACITY;
            }
            else
            {
                count = 0;
            }

            if (count == 0)
                break;
        }
    }

    if (count > 0)
    {
        for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
        {
            if (newItems[i] == ITEM_NONE)
            {
                newItems[i] = itemId;
                newQuantities[i] = count;
                if (newQuantities[i] > MAX_PYRAMID_BAG_CAPACITY)
                {
                    count = newQuantities[i] - MAX_PYRAMID_BAG_CAPACITY;
                    newQuantities[i] = MAX_PYRAMID_BAG_CAPACITY;
                }
                else
                {
                    count = 0;
                }

                if (count == 0)
                    break;
            }
        }
    }

    if (count == 0)
    {
        memcpy(items, newItems, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
        memcpy(quantities, newQuantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));
        Free(newItems);
        Free(newQuantities);
        return TRUE;
    }
    else
    {
        Free(newItems);
        Free(newQuantities);
        return FALSE;
    }
}

bool8 RemovePyramidBagItem(u16 itemId, u16 count)
{
    u16 i;

    u16 *items = gSaveBlock2Ptr->frontier.pyramidBag.itemId[gSaveBlock2Ptr->frontier.lvlMode];
    u8 *quantities = gSaveBlock2Ptr->frontier.pyramidBag.quantity[gSaveBlock2Ptr->frontier.lvlMode];

    i = gPyramidBagMenuState.cursorPosition + gPyramidBagMenuState.scrollPosition;
    if (items[i] == itemId && quantities[i] >= count)
    {
        quantities[i] -= count;
        if (quantities[i] == 0)
            items[i] = ITEM_NONE;
        return TRUE;
    }
    else
    {
        u16 *newItems = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
        u8 *newQuantities = Alloc(PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

        if (newItems == NULL || newQuantities == NULL)
        {
            Free(newItems);
            Free(newQuantities);
            return FALSE;
        }

        memcpy(newItems, items, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
        memcpy(newQuantities, quantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));

        for (i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
        {
            if (newItems[i] == itemId)
            {
                if (newQuantities[i] >= count)
                {
                    newQuantities[i] -= count;
                    count = 0;
                    if (newQuantities[i] == 0)
                        newItems[i] = ITEM_NONE;
                }
                else
                {
                    count -= newQuantities[i];
                    newQuantities[i] = 0;
                    newItems[i] = ITEM_NONE;
                }

                if (count == 0)
                    break;
            }
        }

        if (count == 0)
        {
            memcpy(items, newItems, PYRAMID_BAG_ITEMS_COUNT * sizeof(u16));
            memcpy(quantities, newQuantities, PYRAMID_BAG_ITEMS_COUNT * sizeof(u8));
            Free(newItems);
            Free(newQuantities);
            return TRUE;
        }
        else
        {
            Free(newItems);
            Free(newQuantities);
            return FALSE;
        }
    }
}

static u16 SanitizeItemId(u16 itemId)
{
    if (itemId >= ITEMS_COUNT)
        return ITEM_NONE;
    else
        return itemId;
}

const u8 *ItemId_GetName(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].name;
}

u16 ItemId_GetId(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].itemId;
}

u16 ItemId_GetPrice(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].price;
}

u8 ItemId_GetHoldEffect(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].holdEffect;
}

u8 ItemId_GetHoldEffectParam(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].holdEffectParam;
}

const u8 *ItemId_GetDescription(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].description;
}

u8 ItemId_GetImportance(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].importance;
}

// unused
u8 ItemId_GetUnknownValue(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].unk19;
}

u8 ItemId_GetPocket(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].pocket;
}

u8 ItemId_GetType(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].type;
}

ItemUseFunc ItemId_GetFieldFunc(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].fieldUseFunc;
}

u8 ItemId_GetBattleUsage(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].battleUsage;
}

ItemUseFunc ItemId_GetBattleFunc(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].battleUseFunc;
}

u8 ItemId_GetSecondaryId(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].secondaryId;
}

u8 ItemId_GetFlingPower(u16 itemId)
{
    return gItems[SanitizeItemId(itemId)].flingPower;
}

bool32 IsPinchBerryItemEffect(u16 holdEffect)
{
    switch (holdEffect)
    {
    case HOLD_EFFECT_ATTACK_UP:
    case HOLD_EFFECT_DEFENSE_UP:
    case HOLD_EFFECT_SPEED_UP:
    case HOLD_EFFECT_SP_ATTACK_UP:
    case HOLD_EFFECT_SP_DEFENSE_UP:
    case HOLD_EFFECT_CRITICAL_UP:
    case HOLD_EFFECT_RANDOM_STAT_UP:
    #ifdef HOLD_EFFECT_CUSTAP_BERRY
    case HOLD_EFFECT_CUSTAP_BERRY:
    #endif
    #ifdef HOLD_EFFECT_MICLE_BERRY
    case HOLD_EFFECT_MICLE_BERRY:
    #endif
        return TRUE;
    }

    return FALSE;
}
