#ifndef GUARD_ITEM_H
#define GUARD_ITEM_H

#include "constants/item.h"
#include "constants/item_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/berries.h"
#include "constants/item_effects.h"
#include "constants/hold_effects.h"

// Run & Bun style key item registration: up to one item bound to each of
// SELECT, L and R, usable directly from the overworld.
enum RegisterButton
{
    REGISTER_BUTTON_SELECT,
    REGISTER_BUTTON_L,
    REGISTER_BUTTON_R,
    REGISTER_BUTTON_COUNT,
};

enum PACKED ItemSortType
{
    ITEM_TYPE_UNCATEGORIZED,
    ITEM_TYPE_FIELD_USE,
    ITEM_TYPE_LEVEL_UP_ITEM,
    ITEM_TYPE_HEALTH_RECOVERY,
    ITEM_TYPE_STATUS_RECOVERY,
    ITEM_TYPE_PP_RECOVERY,
    ITEM_TYPE_NATURE_MINT,
    ITEM_TYPE_STAT_BOOST_DRINK,
    ITEM_TYPE_STAT_BOOST_FEATHER,
    ITEM_TYPE_STAT_BOOST_MOCHI,
    ITEM_TYPE_BATTLE_ITEM,
    ITEM_TYPE_FLUTE,
    ITEM_TYPE_X_ITEM,
    ITEM_TYPE_AUX_ITEM,
    ITEM_TYPE_EVOLUTION_STONE,
    ITEM_TYPE_EVOLUTION_ITEM,
    ITEM_TYPE_SPECIAL_HELD_ITEM,
    ITEM_TYPE_MEGA_STONE,
    ITEM_TYPE_Z_CRYSTAL,
    ITEM_TYPE_HELD_ITEM,
    ITEM_TYPE_TYPE_BOOST_HELD_ITEM,
    ITEM_TYPE_CONTEST_HELD_ITEM,
    ITEM_TYPE_EV_BOOST_HELD_ITEM,
    ITEM_TYPE_GEM,
    ITEM_TYPE_PLATE,
    ITEM_TYPE_MEMORY,
    ITEM_TYPE_DRIVE,
    ITEM_TYPE_INCENSE,
    ITEM_TYPE_NECTAR,
    ITEM_TYPE_GROWTH,
    ITEM_TYPE_SHARD,
    ITEM_TYPE_SELLABLE,
    ITEM_TYPE_RELIC,
    ITEM_TYPE_FOSSIL,
    ITEM_TYPE_MAIL,
};

typedef void (*ItemUseFunc)(u8);
typedef bool32 (*ShopCriteriaFunc)(enum Item);

struct ItemInfo
{
    u32 price;
    u16 secondaryId;
    ItemUseFunc fieldUseFunc;
    const u8 *description;
    const u8 *effect;
    const u8 *name;
    const u8 *pluralName;
    u8 holdEffect;
    u8 holdEffectParam;
    u8 importance:2;
    u8 notConsumed:1;
    enum Pocket pocket:5;
    enum ItemSortType sortType;
    u8 type;
    u8 battleUsage;
    u8 flingPower;
    const u32 *iconPic;
    const u16 *iconPalette;
    ShopCriteriaFunc shopCriteriaFunc;
};

struct ALIGNED(2) BagPocket
{
    struct ItemSlot *itemSlots;
    struct ItemSlot *overflowSlots;
    u16 capacity:10;
    enum Pocket id:6;
    u16 primaryCapacity;
};

extern const u8 gQuestionMarksItemName[];
extern const struct ItemInfo gItemsInfo[];
extern struct BagPocket gBagPockets[];

#define GET_BERRY_ID(_berry) case ITEM_##_berry##_BERRY: return BERRY_ID_##_berry;
#define GET_BERRY_ITEM_ID(_berry) case BERRY_ID_##_berry: return ITEM_##_berry##_BERRY;

static inline enum BerryId ItemIdToBerryType(enum Item itemId)
{
    switch (itemId)
    {
    FOREACH_BERRY(GET_BERRY_ID)
    case ITEM_ENIGMA_BERRY_E_READER:
        return BERRY_ID_ENGIMA_E_READER;
    default:
        return BERRY_ID_NONE;
    }
};

static inline enum Item BerryTypeToItemId(enum BerryId berryId)
{
    switch (berryId)
    {
    FOREACH_BERRY(GET_BERRY_ITEM_ID)
    case BERRY_ID_ENGIMA_E_READER:
        return ITEM_ENIGMA_BERRY_E_READER;
    default:
        return ITEM_NONE;
    }
};

#undef GET_BERRY_ID
#undef GET_BERRY_ITEM_ID

void BagPocket_SetSlotData(struct BagPocket *pocket, u32 pocketPos, struct ItemSlot newSlot);
struct ItemSlot BagPocket_GetSlotData(struct BagPocket *pocket, u32 pocketPos);

static inline void BagPocket_SetSlotItemIdAndCount(struct BagPocket *pocket, u32 pocketPos, enum Item itemId, u16 quantity)
{
    BagPocket_SetSlotData(pocket, pocketPos, (struct ItemSlot) {itemId, quantity});
}

static inline enum Item GetBagItemId(enum Pocket pocketId, u32 pocketPos)
{
    return BagPocket_GetSlotData(&gBagPockets[pocketId], pocketPos).itemId;
}

static inline u16 GetBagItemQuantity(enum Pocket pocketId, u32 pocketPos)
{
    return BagPocket_GetSlotData(&gBagPockets[pocketId], pocketPos).quantity;
}

static inline struct ItemSlot GetBagItemIdAndQuantity(enum Pocket pocketId, u32 pocketPos)
{
    return BagPocket_GetSlotData(&gBagPockets[pocketId], pocketPos);
}

void ApplyNewEncryptionKeyToBagItems(u32 newKey);
void SetBagItemsPointers(void);
void MigrateBagPocketsIfNeeded(void);
u8 *CopyItemName(enum Item itemId, u8 *dst);
u8 *CopyItemNameHandlePlural(enum Item itemId, u8 *dst, u32 quantity);
bool32 IsBagPocketNonEmpty(enum Pocket pocketId);
bool32 CheckBagHasItem(enum Item itemId, u16 count);
bool32 HasAtLeastOneBerry(void);
bool32 HasAtLeastOnePokeBall(void);
bool32 CheckBagHasSpace(enum Item itemId, u16 count);
bool32 CheckBagHasSpaceForItemBundle(const struct ItemSlot *items, u32 count);
u32 GetFreeSpaceForItemInBag(enum Item itemId);
bool32 AddBagItem(enum Item itemId, u16 count);
bool32 RemoveBagItem(enum Item itemId, u16 count);
void RemoveBagItemFromSlot(struct BagPocket *pocket, u16 slotId, u16 count);
u8 CountUsedPCItemSlots(void);
bool32 CheckPCHasItem(enum Item itemId, u16 count);
bool32 AddPCItem(enum Item itemId, u16 count);
void RemovePCItem(u8 index, u16 count);
void CompactPCItems(void);
void SwapRegisteredBike(void);
u16 *GetRegisteredItemPtr(enum RegisterButton button);
bool8 GetRegisteredItemButton(u16 itemId, enum RegisterButton *button);
void RegisterKeyItemToButton(u16 itemId, enum RegisterButton button);
void DeselectRegisteredKeyItem(u16 itemId);
void CompactItemsInBagPocket(enum Pocket pocketId);
void MoveItemSlotInPocket(enum Pocket pocketId, u32 from, u32 to);
void MoveItemSlotInPC(struct ItemSlot *itemSlots, u32 from, u32 to);
void ClearBag(void);
u16 CountTotalItemQuantityInBag(enum Item itemId);
bool32 AddPyramidBagItem(enum Item itemId, u16 count);
bool32 RemovePyramidBagItem(enum Item itemId, u16 count);
const u8 *GetItemName(enum Item itemId);
u32 GetItemPrice(enum Item itemId);
const u8 *GetItemEffect(enum Item itemId);
enum HoldEffect GetItemHoldEffect(enum Item itemId);
u32 GetItemHoldEffectParam(enum Item itemId);
const u8 *GetItemDescription(enum Item itemId);
u8 GetItemImportance(enum Item itemId);
u8 GetItemConsumability(enum Item itemId);
enum Pocket GetItemPocket(enum Item itemId);
enum ItemType GetItemType(enum Item itemId);
ItemUseFunc GetItemFieldFunc(enum Item itemId);
enum EffectItem GetItemBattleUsage(enum Item itemId);
u32 GetItemSecondaryId(enum Item itemId);
u32 GetItemFlingPower(enum Item itemId);
u32 GetItemStatus1Mask(enum Item itemId);
u32 GetItemSellPrice(enum Item itemId);
bool32 PlayerOwnsItem(enum Item item);
u32 GetFiniteDuplicateRewardValue(enum Item item);
bool32 IsItemProtectedFromLoss(enum Item item);
bool32 IsHoldEffectChoice(enum HoldEffect holdEffect);
ShopCriteriaFunc GetItemShopCriteriaFunc(enum Item itemId);
bool32 IsItemShopCriteriaFulfilled(enum Item itemId);

#endif // GUARD_ITEM_H
