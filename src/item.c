#include "global.h"
#include "field_specials.h"
#include "item.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "berry.h"
#include "pokeball.h"
#include "string_util.h"
#include "text.h"
#include "event_data.h"
#include "malloc.h"
#include "secret_base.h"
#include "item_menu.h"
#include "party_menu.h"
#include "strings.h"
#include "load_save.h"
#include "item_use.h"
#include "battle_pyramid.h"
#include "battle_pyramid_bag.h"
#include "graphics.h"
#include "shop_criteria.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/item_effects.h"
#include "constants/hold_effects.h"

#define DUMMY_PC_BAG_POCKET                 \
{                                           \
    .id = POCKET_DUMMY,                     \
    .capacity = PC_ITEMS_COUNT,             \
    .primaryCapacity = PC_ITEMS_COUNT,      \
    .itemSlots = gSaveBlock1Ptr->pcItems,   \
    .overflowSlots = NULL,                  \
}

#define BAG_POCKET_LAYOUT_MAGIC 0x45434237 // "ECB7"
#define LEGACY_BAG_SLOT_COUNT (BAG_LEGACY_ITEMS_COUNT     \
                             + BAG_LEGACY_KEYITEMS_COUNT  \
                             + BAG_LEGACY_POKEBALLS_COUNT \
                             + BAG_LEGACY_BERRIES_COUNT)

static bool32 CheckPyramidBagHasItem(enum Item itemId, u16 count);
static bool32 CheckPyramidBagHasSpace(enum Item itemId, u16 count);
static const u8 *GetItemPluralName(enum Item);
static bool32 DoesItemHavePluralName(enum Item);
static void NONNULL BagPocket_CompactItems(struct BagPocket *pocket);
static enum Item SanitizeItemId(enum Item itemId);
static enum Item SanitizeBagItemId(enum Item itemId);

EWRAM_DATA struct BagPocket gBagPockets[POCKETS_COUNT] = {0};
static EWRAM_DATA struct ItemSlot sLegacyBagMigrationBuffer[LEGACY_BAG_SLOT_COUNT] = {0};

#include "data/pokemon/item_effects.h"
#include "data/items.h"

static inline struct ItemSlot *NONNULL BagPocket_GetSlotPointer(struct BagPocket *pocket, u32 pocketPos)
{
    // Only the live Berry pocket spills into the save extension; a private
    // preview copy (overflowSlots == NULL) keeps every slot in its own buffer.
    if (pocket->id == POCKET_BERRIES && pocketPos >= BAG_BERRIES_PRIMARY_COUNT && pocket->overflowSlots != NULL)
        return &gSaveBlock1Ptr->bagExtension.berries[pocketPos - BAG_BERRIES_PRIMARY_COUNT];
    if (pocketPos < pocket->primaryCapacity)
        return &pocket->itemSlots[pocketPos];

    return &pocket->overflowSlots[pocketPos - pocket->primaryCapacity];
}

static inline struct ItemSlot NONNULL BagPocket_GetSlotDataGeneric(struct BagPocket *pocket, u32 pocketPos)
{
    struct ItemSlot *slot = BagPocket_GetSlotPointer(pocket, pocketPos);
    return (struct ItemSlot) {
        .itemId = slot->itemId,
        .quantity = slot->quantity ^ gSaveBlock2Ptr->encryptionKey,
    };
}

static inline struct ItemSlot NONNULL BagPocket_GetSlotDataPC(struct BagPocket *pocket, u32 pocketPos)
{
    struct ItemSlot *slot = BagPocket_GetSlotPointer(pocket, pocketPos);
    return (struct ItemSlot) {
        .itemId = slot->itemId,
        .quantity = slot->quantity,
    };
}

static inline void NONNULL BagPocket_SetSlotDataGeneric(struct BagPocket *pocket, u32 pocketPos, struct ItemSlot newSlot)
{
    struct ItemSlot *slot = BagPocket_GetSlotPointer(pocket, pocketPos);
    slot->itemId = newSlot.itemId;
    slot->quantity = newSlot.quantity ^ gSaveBlock2Ptr->encryptionKey;
}

static inline void NONNULL BagPocket_SetSlotDataPC(struct BagPocket *pocket, u32 pocketPos, struct ItemSlot newSlot)
{
    struct ItemSlot *slot = BagPocket_GetSlotPointer(pocket, pocketPos);
    slot->itemId = newSlot.itemId;
    slot->quantity = newSlot.quantity;
}

struct ItemSlot NONNULL BagPocket_GetSlotData(struct BagPocket *pocket, u32 pocketPos)
{
    switch (pocket->id)
    {
    case POCKET_ITEMS:
    case POCKET_MEDICINE:
    case POCKET_BATTLE:
    case POCKET_KEY_ITEMS:
    case POCKET_POKE_BALLS:
    case POCKET_BERRIES:
    case POCKET_MEGA_STONES:
        return BagPocket_GetSlotDataGeneric(pocket, pocketPos);
    case POCKET_DUMMY:
        return BagPocket_GetSlotDataPC(pocket, pocketPos);
    }

    return (struct ItemSlot) {0}; // Because compiler complains
}

void NONNULL BagPocket_SetSlotData(struct BagPocket *pocket, u32 pocketPos, struct ItemSlot newSlot)
{
    if (newSlot.itemId == ITEM_NONE || newSlot.quantity == 0) // Sets to zero if quantity or itemId is zero
    {
        newSlot.itemId = ITEM_NONE;
        newSlot.quantity = 0;
    }

    switch (pocket->id)
    {
    case POCKET_ITEMS:
    case POCKET_MEDICINE:
    case POCKET_BATTLE:
    case POCKET_KEY_ITEMS:
    case POCKET_POKE_BALLS:
    case POCKET_BERRIES:
    case POCKET_MEGA_STONES:
        BagPocket_SetSlotDataGeneric(pocket, pocketPos, newSlot);
        break;
    case POCKET_DUMMY:
        BagPocket_SetSlotDataPC(pocket, pocketPos, newSlot);
        break;
    }
}

void ApplyNewEncryptionKeyToBagItems(u32 newKey)
{
    enum Pocket pocketId;
    enum Item item;
    for (pocketId = 0; pocketId < POCKETS_COUNT; pocketId++)
    {
        for (item = ITEM_NONE; item < gBagPockets[pocketId].capacity; item++)
            ApplyNewEncryptionKeyToHword(&BagPocket_GetSlotPointer(&gBagPockets[pocketId], item)->quantity, newKey);
    }
}

void SetBagItemsPointers(void)
{
    gBagPockets[POCKET_ITEMS].itemSlots = gSaveBlock1Ptr->bag.items;
    gBagPockets[POCKET_ITEMS].overflowSlots = gSaveBlock1Ptr->bagExtension.items;
    gBagPockets[POCKET_ITEMS].capacity = BAG_ITEMS_COUNT;
    gBagPockets[POCKET_ITEMS].primaryCapacity = BAG_LEGACY_ITEMS_COUNT;
    gBagPockets[POCKET_ITEMS].id = POCKET_ITEMS;

    gBagPockets[POCKET_MEDICINE].itemSlots = gSaveBlock3Ptr->bagPocketMedicine;
    gBagPockets[POCKET_MEDICINE].overflowSlots = NULL;
    gBagPockets[POCKET_MEDICINE].capacity = BAG_MEDICINE_COUNT;
    gBagPockets[POCKET_MEDICINE].primaryCapacity = BAG_MEDICINE_COUNT;
    gBagPockets[POCKET_MEDICINE].id = POCKET_MEDICINE;

    gBagPockets[POCKET_BATTLE].itemSlots = gSaveBlock3Ptr->bagPocketBattle;
    gBagPockets[POCKET_BATTLE].overflowSlots = NULL;
    gBagPockets[POCKET_BATTLE].capacity = BAG_BATTLE_COUNT;
    gBagPockets[POCKET_BATTLE].primaryCapacity = BAG_BATTLE_COUNT;
    gBagPockets[POCKET_BATTLE].id = POCKET_BATTLE;

    gBagPockets[POCKET_BERRIES].itemSlots = gSaveBlock1Ptr->bag.berries;
    gBagPockets[POCKET_BERRIES].overflowSlots = gSaveBlock3Ptr->bagPocketBerries;
    gBagPockets[POCKET_BERRIES].capacity = BAG_BERRIES_COUNT;
    gBagPockets[POCKET_BERRIES].primaryCapacity = BAG_LEGACY_BERRIES_COUNT;
    gBagPockets[POCKET_BERRIES].id = POCKET_BERRIES;

    gBagPockets[POCKET_POKE_BALLS].itemSlots = gSaveBlock1Ptr->bag.pokeBalls;
    gBagPockets[POCKET_POKE_BALLS].overflowSlots = gSaveBlock2Ptr->bagPocketPokeBalls;
    gBagPockets[POCKET_POKE_BALLS].capacity = BAG_POKEBALLS_COUNT;
    gBagPockets[POCKET_POKE_BALLS].primaryCapacity = BAG_LEGACY_POKEBALLS_COUNT;
    gBagPockets[POCKET_POKE_BALLS].id = POCKET_POKE_BALLS;

    gBagPockets[POCKET_KEY_ITEMS].itemSlots = gSaveBlock1Ptr->bag.keyItems;
    gBagPockets[POCKET_KEY_ITEMS].overflowSlots = gSaveBlock3Ptr->bagPocketKeyItems;
    gBagPockets[POCKET_KEY_ITEMS].capacity = BAG_KEYITEMS_COUNT;
    gBagPockets[POCKET_KEY_ITEMS].primaryCapacity = BAG_LEGACY_KEYITEMS_COUNT;
    gBagPockets[POCKET_KEY_ITEMS].id = POCKET_KEY_ITEMS;

    gBagPockets[POCKET_MEGA_STONES].itemSlots = gSaveBlock3Ptr->bagPocketMegaStones;
    gBagPockets[POCKET_MEGA_STONES].overflowSlots = gSaveBlock1Ptr->bagExtension.megaStones;
    gBagPockets[POCKET_MEGA_STONES].capacity = BAG_MEGASTONES_COUNT;
    gBagPockets[POCKET_MEGA_STONES].primaryCapacity = BAG_MEGASTONES_PRIMARY_COUNT;
    gBagPockets[POCKET_MEGA_STONES].id = POCKET_MEGA_STONES;
}

static u32 SnapshotLegacyPocket(struct ItemSlot *dst, u32 dstPos, const struct ItemSlot *src, u32 count)
{
    for (u32 i = 0; i < count; i++)
    {
        dst[dstPos].itemId = src[i].itemId;
        dst[dstPos].quantity = src[i].quantity ^ gSaveBlock2Ptr->encryptionKey;
        dstPos++;
    }

    return dstPos;
}

static bool32 NONNULL AddMigratedBagItem(struct BagPocket *pocket, enum Item itemId, u16 quantity)
{
    struct ItemSlot slot;

    // Preserve duplicate legacy stacks if present, while respecting the normal
    // per-stack quantity cap.
    for (u32 i = 0; i < pocket->capacity && quantity != 0; i++)
    {
        slot = BagPocket_GetSlotData(pocket, i);
        if (slot.itemId == itemId && slot.quantity < MAX_BAG_ITEM_CAPACITY)
        {
            u16 added = min(quantity, MAX_BAG_ITEM_CAPACITY - slot.quantity);
            BagPocket_SetSlotItemIdAndCount(pocket, i, itemId, slot.quantity + added);
            quantity -= added;
        }
    }

    for (u32 i = 0; i < pocket->capacity && quantity != 0; i++)
    {
        slot = BagPocket_GetSlotData(pocket, i);
        if (slot.itemId == ITEM_NONE)
        {
            u16 added = min(quantity, MAX_BAG_ITEM_CAPACITY);
            BagPocket_SetSlotItemIdAndCount(pocket, i, itemId, added);
            quantity -= added;
        }
    }

    return quantity == 0;
}

void MigrateBagPocketsIfNeeded(void)
{
    u32 count = 0;

    if (gSaveBlock3Ptr->bagPocketLayoutMagic == BAG_POCKET_LAYOUT_MAGIC
     && gSaveBlock3Ptr->bagPocketLayoutMagicInverse == ~BAG_POCKET_LAYOUT_MAGIC)
        return;

    // Read the old five-pocket layout before clearing any arrays that the new
    // seven-pocket layout reuses as its primary storage.
    count = SnapshotLegacyPocket(sLegacyBagMigrationBuffer, count, gSaveBlock1Ptr->bag.items, BAG_LEGACY_ITEMS_COUNT);
    count = SnapshotLegacyPocket(sLegacyBagMigrationBuffer, count, gSaveBlock1Ptr->bag.keyItems, BAG_LEGACY_KEYITEMS_COUNT);
    count = SnapshotLegacyPocket(sLegacyBagMigrationBuffer, count, gSaveBlock1Ptr->bag.pokeBalls, BAG_LEGACY_POKEBALLS_COUNT);
    count = SnapshotLegacyPocket(sLegacyBagMigrationBuffer, count, gSaveBlock1Ptr->bag.berries, BAG_LEGACY_BERRIES_COUNT);

    ClearBag();

    for (u32 i = 0; i < count; i++)
    {
        enum Item itemId = sLegacyBagMigrationBuffer[i].itemId;
        u16 quantity = sLegacyBagMigrationBuffer[i].quantity;

        if (itemId == ITEM_NONE || itemId >= ITEMS_COUNT || quantity == 0)
            continue;

        assertf(AddMigratedBagItem(&gBagPockets[GetItemPocket(itemId)], itemId, quantity),
                "failed to migrate bag item: %S", GetItemName(itemId));
    }

    gSaveBlock3Ptr->bagPocketLayoutMagic = BAG_POCKET_LAYOUT_MAGIC;
    gSaveBlock3Ptr->bagPocketLayoutMagicInverse = ~BAG_POCKET_LAYOUT_MAGIC;
}

u8 *CopyItemName(enum Item itemId, u8 *dst)
{
    return StringCopy(dst, GetItemName(itemId));
}

const u8 sText_s[] =_("s");

u8 *CopyItemNameHandlePlural(enum Item itemId, u8 *dst, u32 quantity)
{
    if (quantity == 1)
    {
        return StringCopy(dst, GetItemName(itemId));
    }
    else if (DoesItemHavePluralName(itemId))
    {
        return StringCopy(dst, GetItemPluralName(itemId));
    }
    else
    {
        u8 *end = StringCopy(dst, GetItemName(itemId));
        return StringCopy(end, sText_s);
    }
}

bool32 IsBagPocketNonEmpty(enum Pocket pocketId)
{
    u8 i;

    for (i = 0; i < gBagPockets[pocketId].capacity; i++)
    {
        if (GetBagItemId(pocketId, i) != ITEM_NONE)
            return TRUE;
    }
    return FALSE;
}

static bool32 NONNULL BagPocket_CheckHasItem(struct BagPocket *pocket, enum Item itemId, u16 count)
{
    struct ItemSlot tempItem;

    // Check for item slots that contain the item
    for (u32 i = 0; i < pocket->capacity && count > 0; i++)
    {
        tempItem = BagPocket_GetSlotData(pocket, i);
        if (tempItem.itemId == itemId)
            count -= min(count, tempItem.quantity);
    }

    return count == 0;
}

bool32 CheckBagHasItem(enum Item itemId, u16 count)
{
    if (GetItemPocket(itemId) >= POCKETS_COUNT)
        return FALSE;
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
        return CheckPyramidBagHasItem(itemId, count);

    return BagPocket_CheckHasItem(&gBagPockets[GetItemPocket(itemId)], itemId, count);
}

bool32 HasAtLeastOneBerry(void)
{
    for (enum BerryId berryId = 1; berryId <= NUM_BERRIES; berryId++)
    {
        if (CheckBagHasItem(BerryTypeToItemId(berryId), 1) == TRUE)
            return (gSpecialVar_Result = TRUE);
    }

    return (gSpecialVar_Result = FALSE);
}

bool32 HasAtLeastOnePokeBall(void)
{
    for (enum PokeBall ballId = BALL_STRANGE; ballId < POKEBALL_COUNT; ballId++)
    {
        if (CheckBagHasItem(gPokeBalls[ballId].itemId, 1) == TRUE)
            return TRUE;
    }
    return FALSE;
}

bool32 CheckBagHasSpace(enum Item itemId, u16 count)
{
    if (GetItemPocket(itemId) >= POCKETS_COUNT)
        return FALSE;

    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
        return CheckPyramidBagHasSpace(itemId, count);

    return GetFreeSpaceForItemInBag(itemId) >= count;
}

static u32 NONNULL BagPocket_GetFreeSpaceForItem(struct BagPocket *pocket, enum Item itemId)
{
    u32 spaceForItem = 0;
    struct ItemSlot tempItem;

    // Check space in any existing item slots that already contain this item
    for (u32 i = 0; i < pocket->capacity; i++)
    {
        tempItem = BagPocket_GetSlotData(pocket, i);
        if (tempItem.itemId == ITEM_NONE || tempItem.itemId == itemId)
        {
            // Berry delivery uses only the first matching or empty slot;
            // counting later slots would approve a reward it cannot insert.
            if (pocket->id == POCKET_BERRIES)
                return tempItem.itemId == ITEM_NONE ? MAX_BAG_ITEM_CAPACITY : MAX_BAG_ITEM_CAPACITY - min(tempItem.quantity, MAX_BAG_ITEM_CAPACITY);
            spaceForItem += (tempItem.itemId ? (MAX_BAG_ITEM_CAPACITY - min(tempItem.quantity, MAX_BAG_ITEM_CAPACITY)) : MAX_BAG_ITEM_CAPACITY);
        }
    }

    return spaceForItem;
}

u32 GetFreeSpaceForItemInBag(enum Item itemId)
{
    if (GetItemPocket(itemId) >= POCKETS_COUNT)
        return 0;

    return BagPocket_GetFreeSpaceForItem(&gBagPockets[GetItemPocket(itemId)], itemId);
}

static bool32 NONNULL BagPocket_AddItem(struct BagPocket *pocket, enum Item itemId, u16 count)
{
    // Preflight the same first-slot Berry and split-stack rules without
    // allocating scratch memory. A refused delivery leaves every slot intact.
    if (BagPocket_GetFreeSpaceForItem(pocket, itemId) < count)
        return FALSE;

    for (u32 i = 0; i < pocket->capacity && count > 0; i++)
    {
        struct ItemSlot slot = BagPocket_GetSlotData(pocket, i);
        if (slot.itemId != ITEM_NONE && slot.itemId != itemId)
            continue;
        u16 quantity = slot.itemId == ITEM_NONE ? 0 : slot.quantity;
        u16 added = min(count, MAX_BAG_ITEM_CAPACITY - min(quantity, MAX_BAG_ITEM_CAPACITY));
        if (added == 0)
            continue;
        BagPocket_SetSlotItemIdAndCount(pocket, i, itemId, quantity + added);
        count -= added;
    }
    return count == 0;
}

// Plan a multi-item field gift using the real insertion rules on private pocket
// copies. Shared free slots, split pockets, Berry limits and existing stacks all
// behave exactly like delivery, without changing inventory or discovery stock.
bool32 CheckBagHasSpaceForItemBundle(const struct ItemSlot *items, u32 count)
{
    enum { MAX_PREVIEW_SLOTS = max(max(BAG_ITEMS_COUNT, BAG_MEDICINE_COUNT),
        max(max(BAG_BATTLE_COUNT, BAG_BERRIES_COUNT),
            max(BAG_POKEBALLS_COUNT, max(BAG_KEYITEMS_COUNT, BAG_MEGASTONES_COUNT)))) };
    struct ItemSlot previewSlots[MAX_PREVIEW_SLOTS];
    for (u32 i = 0; i < count; i++)
    {
        enum Pocket pocketId = GetItemPocket(items[i].itemId);
        if (items[i].itemId == ITEM_NONE || items[i].quantity == 0 || pocketId >= POCKETS_COUNT)
            return FALSE;
    }
    for (u32 i = 0; i < count; i++)
    {
        enum Pocket pocketId = GetItemPocket(items[i].itemId);
        u32 previous;
        for (previous = 0; previous < i; previous++)
            if (GetItemPocket(items[previous].itemId) == pocketId)
                break;
        if (previous != i)
            continue;

        struct BagPocket preview = gBagPockets[pocketId];
        if (preview.capacity > ARRAY_COUNT(previewSlots))
            return FALSE;
        preview.itemSlots = previewSlots;
        preview.primaryCapacity = preview.capacity;
        preview.overflowSlots = NULL;
        for (u32 slot = 0; slot < preview.capacity; slot++)
            BagPocket_SetSlotData(&preview, slot, BagPocket_GetSlotData(&gBagPockets[pocketId], slot));
        bool32 fits = TRUE;
        for (u32 j = i; j < count && fits; j++)
            if (GetItemPocket(items[j].itemId) == pocketId)
                fits = BagPocket_AddItem(&preview, items[j].itemId, items[j].quantity);
        if (!fits)
            return FALSE;
    }
    return TRUE;
}

bool32 AddBagItem(enum Item itemId, u16 count)
{
    itemId = SanitizeBagItemId(itemId);
    if (itemId == ITEM_NONE || count == 0)
        return FALSE;

    // Temporary Pyramid equipment never unlocks permanent vendor stock.
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG))
        return AddPyramidBagItem(itemId, count);

    if (!BagPocket_AddItem(&gBagPockets[GetItemPocket(itemId)], itemId, count))
        return FALSE;
    EmeraldChampions_UnlockBattleItem(itemId);
    return TRUE;
}

static bool32 NONNULL BagPocket_RemoveItem(struct BagPocket *pocket, enum Item itemId, u16 count)
{
    bool32 compact = count == 0;

    // Preflight before any mutation so an insufficient request is atomic.
    if (!BagPocket_CheckHasItem(pocket, itemId, count))
        return FALSE;

    if (CurMapIsSecretBase() == TRUE)
    {
        VarSet(VAR_SECRET_BASE_LOW_TV_FLAGS, VarGet(VAR_SECRET_BASE_LOW_TV_FLAGS) | SECRET_BASE_USED_BAG);
        VarSet(VAR_SECRET_BASE_LAST_ITEM_USED, itemId);
    }

    for (u32 i = 0; i < pocket->capacity && count > 0; i++)
    {
        struct ItemSlot slot = BagPocket_GetSlotData(pocket, i);
        if (slot.itemId != itemId)
            continue;

        u16 removed = min(count, slot.quantity);
        slot.quantity -= removed;
        count -= removed;
        BagPocket_SetSlotData(pocket, i, slot);
        // Any stack emptied along the way leaves a hole, not just the last one.
        compact |= slot.quantity == 0;
    }

    // Preserve existing slot order unless the final consumed stack was emptied
    // (or this was the existing zero-count compaction operation).
    if (compact)
        BagPocket_CompactItems(pocket);
    return TRUE;
}

bool32 RemoveBagItem(enum Item itemId, u16 count)
{
    itemId = SanitizeBagItemId(itemId);
    if (itemId == ITEM_NONE)
        return FALSE;

    // check Battle Pyramid Bag
    if (CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE || FlagGet(FLAG_STORING_ITEMS_IN_PYRAMID_BAG) == TRUE)
        return RemovePyramidBagItem(itemId, count);

    return BagPocket_RemoveItem(&gBagPockets[GetItemPocket(itemId)], itemId, count);
}

// Unsafe function: Only use with functions that already check the slot and count are valid
void RemoveBagItemFromSlot(struct BagPocket *pocket, u16 slotId, u16 count)
{
    struct ItemSlot itemSlot = BagPocket_GetSlotData(pocket, slotId);
    BagPocket_SetSlotItemIdAndCount(pocket, slotId, itemSlot.itemId, itemSlot.quantity - count);
}

static u8 NONNULL BagPocket_CountUsedItemSlots(struct BagPocket *pocket)
{
    u8 usedSlots = 0;

    for (u32 i = 0; i < pocket->capacity; i++)
    {
        if (BagPocket_GetSlotData(pocket, i).itemId != ITEM_NONE)
            usedSlots++;
    }
    return usedSlots;
}

u8 CountUsedPCItemSlots(void)
{
    struct BagPocket dummyPocket = DUMMY_PC_BAG_POCKET;
    return BagPocket_CountUsedItemSlots(&dummyPocket);
}

bool32 CheckPCHasItem(enum Item itemId, u16 count)
{
    struct BagPocket dummyPocket = DUMMY_PC_BAG_POCKET;
    if (itemId == ITEM_NONE || itemId >= ITEMS_COUNT || count == 0)
        return FALSE;
    return BagPocket_CheckHasItem(&dummyPocket, itemId, count);
}

bool32 AddPCItem(enum Item itemId, u16 count)
{
    struct BagPocket dummyPocket = DUMMY_PC_BAG_POCKET;
    if (itemId == ITEM_NONE || itemId >= ITEMS_COUNT || count == 0
     || !BagPocket_AddItem(&dummyPocket, itemId, count))
        return FALSE;
    EmeraldChampions_UnlockBattleItem(itemId);
    return TRUE;
}

static void NONNULL BagPocket_CompactItems(struct BagPocket *pocket)
{
    struct ItemSlot tempItem;
    u32 slotCursor = 0;
    for (u32 i = 0; i < pocket->capacity; i++)
    {
        tempItem = BagPocket_GetSlotData(pocket, i);
        if (tempItem.itemId == ITEM_NONE)
        {
            if (!slotCursor)
                slotCursor = i + 1;
        }
        else if (slotCursor > 0)
        {
            BagPocket_SetSlotData(pocket, slotCursor++ - 1, tempItem);
            BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_NONE, 0);
        }
    }
}

void RemovePCItem(u8 index, u16 count)
{
    struct BagPocket dummyPocket = DUMMY_PC_BAG_POCKET;

    if (index >= dummyPocket.capacity)
        return;
    // Get id, quantity at slot
    struct ItemSlot tempItem = BagPocket_GetSlotData(&dummyPocket, index);
    if (count > tempItem.quantity)
        return;

    // Remove quantity
    tempItem.quantity -= count;
    BagPocket_SetSlotData(&dummyPocket, index, tempItem);

    // Compact if necessary
    if (tempItem.quantity == 0)
        BagPocket_CompactItems(&dummyPocket);
}

void CompactPCItems(void)
{
    struct BagPocket dummyPocket = DUMMY_PC_BAG_POCKET;
    BagPocket_CompactItems(&dummyPocket);
}

// Returns a pointer to the SaveBlock1 slot backing the given register button.
u16 *GetRegisteredItemPtr(enum RegisterButton button)
{
    switch (button)
    {
    case REGISTER_BUTTON_L:
        return &gSaveBlock1Ptr->registeredItemL;
    case REGISTER_BUTTON_R:
        return &gSaveBlock1Ptr->registeredItemR;
    case REGISTER_BUTTON_SELECT:
    default:
        return &gSaveBlock1Ptr->registeredItem;
    }
}

// Finds which button (if any) itemId is currently registered to.
bool8 GetRegisteredItemButton(u16 itemId, enum RegisterButton *button)
{
    enum RegisterButton i;

    if (itemId == ITEM_NONE)
        return FALSE;

    for (i = 0; i < REGISTER_BUTTON_COUNT; i++)
    {
        if (*GetRegisteredItemPtr(i) == itemId)
        {
            if (button != NULL)
                *button = i;
            return TRUE;
        }
    }
    return FALSE;
}

// Registers itemId to the given button. If itemId was already registered to
// a different button, it is moved (unbound from the old button). If the
// target button already had an item registered, that item is replaced.
void RegisterKeyItemToButton(u16 itemId, enum RegisterButton button)
{
    enum RegisterButton existingButton;

    if (GetRegisteredItemButton(itemId, &existingButton) == TRUE)
        *GetRegisteredItemPtr(existingButton) = ITEM_NONE;
    *GetRegisteredItemPtr(button) = itemId;
}

// Unbinds itemId from whichever button (if any) it is currently registered to.
void DeselectRegisteredKeyItem(u16 itemId)
{
    enum RegisterButton existingButton;

    if (GetRegisteredItemButton(itemId, &existingButton) == TRUE)
        *GetRegisteredItemPtr(existingButton) = ITEM_NONE;
}

void SwapRegisteredBike(void)
{
    enum RegisterButton button;

    if (GetRegisteredItemButton(ITEM_MACH_BIKE, &button) == TRUE)
        *GetRegisteredItemPtr(button) = ITEM_ACRO_BIKE;
    else if (GetRegisteredItemButton(ITEM_ACRO_BIKE, &button) == TRUE)
        *GetRegisteredItemPtr(button) = ITEM_MACH_BIKE;
}

void CompactItemsInBagPocket(enum Pocket pocketId)
{
    BagPocket_CompactItems(&gBagPockets[pocketId]);
}

static inline void NONNULL BagPocket_MoveItemSlot(struct BagPocket *pocket, u32 from, u32 to)
{
    if (from != to)
    {
        s8 shift = (to > from) ? 1 : -1;
        if (to > from)
            to--;

        // Record the values at "from"
        struct ItemSlot fromSlot = BagPocket_GetSlotData(pocket, from);

        // Shuffle items between "to" and "from"
        for (u32 i = from; i != to; i += shift)
            BagPocket_SetSlotData(pocket, i, BagPocket_GetSlotData(pocket, i + shift));

        // Move the saved "from" to "to"
        BagPocket_SetSlotData(pocket, to, fromSlot);
    }
}

void MoveItemSlotInPocket(enum Pocket pocketId, u32 from, u32 to)
{
    BagPocket_MoveItemSlot(&gBagPockets[pocketId], from, to);
}

void MoveItemSlotInPC(struct ItemSlot *itemSlots, u32 from, u32 to)
{
    struct BagPocket dummyPocket = DUMMY_PC_BAG_POCKET;
    return BagPocket_MoveItemSlot(&dummyPocket, from, to);
}

void ClearBag(void)
{
    for (enum Pocket pocketId = POCKET_ITEMS; pocketId < POCKETS_COUNT; pocketId++)
    {
        for (u32 i = 0; i < gBagPockets[pocketId].capacity; i++)
            BagPocket_SetSlotData(&gBagPockets[pocketId], i, (struct ItemSlot) {0});
    }

    gSaveBlock3Ptr->bagPocketLayoutMagic = BAG_POCKET_LAYOUT_MAGIC;
    gSaveBlock3Ptr->bagPocketLayoutMagicInverse = ~BAG_POCKET_LAYOUT_MAGIC;
}

static inline u16 NONNULL BagPocket_CountTotalItemQuantity(struct BagPocket *pocket, enum Item itemId)
{
    u32 ownedCount = 0;
    struct ItemSlot tempItem;

    for (u32 i = 0; i < pocket->capacity; i++)
    {
        tempItem = BagPocket_GetSlotData(pocket, i);
        if (tempItem.itemId == itemId)
            ownedCount += tempItem.quantity;
    }

    return ownedCount;
}

u16 CountTotalItemQuantityInBag(enum Item itemId)
{
    return BagPocket_CountTotalItemQuantity(&gBagPockets[GetItemPocket(itemId)], itemId);
}

static bool32 CheckPyramidBagHasItem(enum Item itemId, u16 count)
{
    u32 mode = gSaveBlock2Ptr->frontier.lvlMode;
    if (mode >= FRONTIER_LVL_MODE_COUNT || itemId == ITEM_NONE || itemId >= ITEMS_COUNT)
        return FALSE;
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    for (u32 i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (bag->itemId[mode][i] == itemId)
        {
            if (bag->quantity[mode][i] >= count)
                return TRUE;
            count -= bag->quantity[mode][i];
        }
    }
    return FALSE;
}

static bool32 CheckPyramidBagHasSpace(enum Item itemId, u16 count)
{
    u32 mode = gSaveBlock2Ptr->frontier.lvlMode;
    if (mode >= FRONTIER_LVL_MODE_COUNT || itemId == ITEM_NONE || itemId >= ITEMS_COUNT)
        return FALSE;
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    u32 space = 0;
    for (u32 i = 0; i < PYRAMID_BAG_ITEMS_COUNT; i++)
    {
        if (bag->itemId[mode][i] == ITEM_NONE)
            space += MAX_PYRAMID_BAG_ITEM_CAPACITY;
        else if (bag->itemId[mode][i] == itemId)
            space += MAX_PYRAMID_BAG_ITEM_CAPACITY - min(bag->quantity[mode][i], MAX_PYRAMID_BAG_ITEM_CAPACITY);
    }
    return space >= count;
}

bool32 AddPyramidBagItem(enum Item itemId, u16 count)
{
    if (!CheckPyramidBagHasSpace(itemId, count))
        return FALSE;
    u32 mode = gSaveBlock2Ptr->frontier.lvlMode;
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    // Preserve Pyramid ordering: fill matching stacks before empty slots.
    for (u32 pass = 0; pass < 2 && count > 0; pass++)
    {
        for (u32 i = 0; i < PYRAMID_BAG_ITEMS_COUNT && count > 0; i++)
        {
            if (bag->itemId[mode][i] != (pass == 0 ? itemId : ITEM_NONE))
                continue;
            u32 quantity = pass == 0 ? bag->quantity[mode][i] : 0;
            u32 added = min(count, MAX_PYRAMID_BAG_ITEM_CAPACITY - min(quantity, MAX_PYRAMID_BAG_ITEM_CAPACITY));
            if (added == 0)
                continue;
            bag->itemId[mode][i] = itemId;
            bag->quantity[mode][i] = quantity + added;
            count -= added;
        }
    }
    return count == 0;
}

bool32 RemovePyramidBagItem(enum Item itemId, u16 count)
{
    if (!CheckPyramidBagHasItem(itemId, count))
        return FALSE;
    u32 mode = gSaveBlock2Ptr->frontier.lvlMode;
    struct PyramidBag *bag = &gSaveBlock2Ptr->frontier.pyramidBag;
    u32 selected = gPyramidBagMenuState.cursorPosition + gPyramidBagMenuState.scrollPosition;
    if (selected < PYRAMID_BAG_ITEMS_COUNT && bag->itemId[mode][selected] == itemId
     && bag->quantity[mode][selected] >= count)
    {
        bag->quantity[mode][selected] -= count;
        if (bag->quantity[mode][selected] == 0)
            bag->itemId[mode][selected] = ITEM_NONE;
        return TRUE;
    }
    // Otherwise consume matching stacks in their existing order.
    for (u32 i = 0; i < PYRAMID_BAG_ITEMS_COUNT && count > 0; i++)
    {
        if (bag->itemId[mode][i] == itemId)
        {
            u32 removed = min(count, bag->quantity[mode][i]);
            bag->quantity[mode][i] -= removed;
            count -= removed;
            if (bag->quantity[mode][i] == 0)
                bag->itemId[mode][i] = ITEM_NONE;
        }
    }
    return count == 0;
}

static enum Item SanitizeItemId(enum Item itemId)
{
    assertf(itemId < ITEMS_COUNT, "invalid item: %d", itemId)
    {
        return ITEM_NONE;
    }

    return itemId;
}

static enum Item SanitizeBagItemId(enum Item itemId)
{
    itemId = SanitizeItemId(itemId);

    assertf(itemId != ITEM_NONE, "invalid bag item: ITEM_NONE")
    {
        return ITEM_NONE;
    }

    assertf(GetItemPocket(itemId) < POCKETS_COUNT, "invalid bag item pocket: %S", gItemsInfo[itemId].name)
    {
        return ITEM_NONE;
    }

    return itemId;
}

const u8 *GetItemName(enum Item itemId)
{
    const u8 *name = gItemsInfo[SanitizeItemId(itemId)].name;

    return name == NULL ? gQuestionMarksItemName : name;
}

bool32 PlayerOwnsItem(enum Item item)
{
    if (item == ITEM_NONE)
        return FALSE;
    if (CheckBagHasItem(item, 1) || CheckPCHasItem(item, 1))
        return TRUE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES) != SPECIES_NONE
            && GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM) == item)
            return TRUE;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            if (GetBoxMonData(&gPokemonStoragePtr->boxes[box][slot], MON_DATA_SPECIES) != SPECIES_NONE
                && GetBoxMonData(&gPokemonStoragePtr->boxes[box][slot], MON_DATA_HELD_ITEM) == item)
                return TRUE;
    for (u32 slot = 0; slot < DAYCARE_MON_COUNT; slot++)
        if (GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[slot].mon, MON_DATA_SPECIES) != SPECIES_NONE
            && GetBoxMonData(&gSaveBlock1Ptr->daycare.mons[slot].mon, MON_DATA_HELD_ITEM) == item)
            return TRUE;
    return FALSE;
}

u32 GetItemPrice(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].price;
}

static bool32 DoesItemHavePluralName(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].pluralName != NULL;
}

static const u8 *GetItemPluralName(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].pluralName;
}

const u8 *GetItemEffect(enum Item itemId)
{
    if (itemId == ITEM_ENIGMA_BERRY_E_READER)
    #if FREE_ENIGMA_BERRY == FALSE
        return gSaveBlock1Ptr->enigmaBerry.itemEffect;
    #else
        return NULL;
    #endif //FREE_ENIGMA_BERRY
    else
        return gItemsInfo[SanitizeItemId(itemId)].effect;
}

enum HoldEffect GetItemHoldEffect(enum Item itemId)
{
    if (itemId == ITEM_ENIGMA_BERRY_E_READER)
    #if FREE_ENIGMA_BERRY == FALSE
        return gSaveBlock1Ptr->enigmaBerry.holdEffect;
    #else
        return HOLD_EFFECT_NONE;
    #endif //FREE_ENIGMA_BERRY
    else
        return gItemsInfo[SanitizeItemId(itemId)].holdEffect;
}

u32 GetItemHoldEffectParam(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].holdEffectParam;
}

const u8 *GetItemDescription(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].description;
}

u8 GetItemImportance(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].importance;
}

u8 GetItemConsumability(enum Item itemId)
{
    return !gItemsInfo[SanitizeItemId(itemId)].notConsumed;
}

enum Pocket GetItemPocket(enum Item itemId)
{
    itemId = SanitizeItemId(itemId);

    // These two are party-use preparation tools in Inclement Emerald.
    if (itemId == ITEM_ABILITY_CAPSULE || itemId == ITEM_ABILITY_PATCH)
        return POCKET_MEDICINE;

    // Primal Orbs share the progression-only Mega pocket in Inclement.
    if (itemId == ITEM_RED_ORB || itemId == ITEM_BLUE_ORB)
        return POCKET_MEGA_STONES;

    switch (gItemsInfo[itemId].sortType)
    {
    case ITEM_TYPE_LEVEL_UP_ITEM:
    case ITEM_TYPE_HEALTH_RECOVERY:
    case ITEM_TYPE_STATUS_RECOVERY:
    case ITEM_TYPE_PP_RECOVERY:
    case ITEM_TYPE_NATURE_MINT:
    case ITEM_TYPE_STAT_BOOST_DRINK:
    case ITEM_TYPE_STAT_BOOST_FEATHER:
    case ITEM_TYPE_STAT_BOOST_MOCHI:
        return POCKET_MEDICINE;

    case ITEM_TYPE_BATTLE_ITEM:
    case ITEM_TYPE_X_ITEM:
    case ITEM_TYPE_AUX_ITEM:
    case ITEM_TYPE_SPECIAL_HELD_ITEM:
    case ITEM_TYPE_HELD_ITEM:
    case ITEM_TYPE_TYPE_BOOST_HELD_ITEM:
    case ITEM_TYPE_EV_BOOST_HELD_ITEM:
    case ITEM_TYPE_GEM:
    case ITEM_TYPE_PLATE:
    case ITEM_TYPE_MEMORY:
    case ITEM_TYPE_DRIVE:
        return POCKET_BATTLE;

    case ITEM_TYPE_MEGA_STONE:
        return POCKET_MEGA_STONES;

    default:
        return gItemsInfo[itemId].pocket;
    }
}

enum ItemType GetItemType(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].type;
}

ItemUseFunc GetItemFieldFunc(enum Item itemId)
{
    // All ordinary berries are free equipment/seeds, never free field medicine.
    if (GetItemPocket(itemId) == POCKET_BERRIES)
        return ItemUseOutOfBattle_CannotUse;
    return gItemsInfo[SanitizeItemId(itemId)].fieldUseFunc;
}

// Returns an item's battle effect script ID.
enum EffectItem GetItemBattleUsage(enum Item itemId)
{
    enum Item item = SanitizeItemId(itemId);
    // Handle E-Reader berries.
    if (item == ITEM_ENIGMA_BERRY_E_READER)
    {
        switch (GetItemEffectType(gSpecialVar_ItemId))
        {
        case ITEM_EFFECT_X_ITEM:
            return EFFECT_ITEM_INCREASE_STAT;
        case ITEM_EFFECT_HEAL_HP:
            return EFFECT_ITEM_RESTORE_HP;
        case ITEM_EFFECT_CURE_POISON:
        case ITEM_EFFECT_CURE_SLEEP:
        case ITEM_EFFECT_CURE_BURN:
        case ITEM_EFFECT_CURE_FREEZE_FROSTBITE:
        case ITEM_EFFECT_CURE_PARALYSIS:
        case ITEM_EFFECT_CURE_ALL_STATUS:
        case ITEM_EFFECT_CURE_CONFUSION:
        case ITEM_EFFECT_CURE_INFATUATION:
            return EFFECT_ITEM_CURE_STATUS;
        case ITEM_EFFECT_HEAL_PP:
            return EFFECT_ITEM_RESTORE_PP;
        default:
            return 0;
        }
    }
    else
        return gItemsInfo[item].battleUsage;
}

u32 GetItemSecondaryId(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].secondaryId;
}

u32 GetItemFlingPower(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].flingPower;
}


u32 GetItemStatus1Mask(enum Item itemId)
{
    const u8 *effect = GetItemEffect(itemId);
    switch (effect[3])
    {
    case ITEM3_PARALYSIS:
        return STATUS1_PARALYSIS;
    case ITEM3_FREEZE:
        return STATUS1_ICY_ANY;
    case ITEM3_BURN:
        return STATUS1_BURN;
    case ITEM3_POISON:
        return STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER;
    case ITEM3_SLEEP:
        return STATUS1_SLEEP;
    case ITEM3_STATUS_ALL:
        return STATUS1_ANY | STATUS1_TOXIC_COUNTER;
    }
    return 0;
}

bool32 IsItemProtectedFromLoss(enum Item item)
{
    return GetItemImportance(item) || GetItemPocket(item) == POCKET_KEY_ITEMS
        || GetItemPocket(item) == POCKET_MEGA_STONES;
}

u32 GetItemSellPrice(enum Item itemId)
{
    if (IsItemProtectedFromLoss(itemId)
        || IsEmeraldChampionsFreeCatalogueItem(itemId))
        return 0;
    return GetItemPrice(itemId) / ITEM_SELL_FACTOR;
}

bool32 IsHoldEffectChoice(enum HoldEffect holdEffect)
{
    return holdEffect == HOLD_EFFECT_CHOICE_BAND
        || holdEffect == HOLD_EFFECT_CHOICE_SCARF
        || holdEffect == HOLD_EFFECT_CHOICE_SPECS;
}

ShopCriteriaFunc GetItemShopCriteriaFunc(enum Item itemId)
{
    return gItemsInfo[SanitizeItemId(itemId)].shopCriteriaFunc;
}

bool32 IsItemShopCriteriaFulfilled(enum Item itemId)
{
    ShopCriteriaFunc func = GetItemShopCriteriaFunc(itemId);

    if (!func)
        return TRUE;

    return func(SanitizeItemId(itemId));
}

// Call only from a finite authored entitlement, before its receipt is closed.
// Repeatable vendors and trades must not manufacture money through this rule.
u32 GetFiniteDuplicateRewardValue(enum Item item)
{
    if (item == ITEM_NONE || item >= ITEMS_COUNT)
        return 0;
    if (gItemsInfo[item].sortType == ITEM_TYPE_MEGA_STONE && PlayerOwnsItem(item))
        return 3000;
    if (gItemsInfo[item].sortType == ITEM_TYPE_EVOLUTION_ITEM
        && GetItemImportance(item) && PlayerOwnsItem(item))
        return 3000;
    return 0;
}
