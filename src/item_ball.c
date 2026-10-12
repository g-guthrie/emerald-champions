#include "global.h"
#include "item_ball.h"
#include "event_data.h"
#include "item.h"
#include "constants/event_objects.h"
#include "constants/items.h"

static u32 GetItemBallAmountFromTemplate(u32);
static u32 GetItemBallIdFromTemplate(u32);

static u32 GetItemBallAmountFromTemplate(u32 itemBallId)
{
    u32 amount = gMapHeader.events->objectEvents[itemBallId].movementRangeX;

    if (amount > MAX_BAG_ITEM_CAPACITY)
        return MAX_BAG_ITEM_CAPACITY;

    return (amount == 0) ? 1 : amount;
}

static u32 GetItemBallIdFromTemplate(u32 itemBallId)
{
    enum Item itemId = gMapHeader.events->objectEvents[itemBallId].trainerRange_berryTreeId;

    return (itemId >= ITEMS_COUNT) ? (ITEM_NONE + 1) : itemId;
}

void GetItemBallIdAndAmountFromTemplate(void)
{
    u32 itemBallId = (gSpecialVar_LastTalked - 1);
    gSpecialVar_Result = GetItemBallIdFromTemplate(itemBallId);
    gSpecialVar_0x8009 = GetItemBallAmountFromTemplate(itemBallId);
}

// The Mega Stones of Pokemon only found after the POKeMON LEAGUE lie in item
// balls that appear once the game has been cleared.
bool32 IsPostGameItemBallHidden(u32 itemId)
{
    switch (itemId)
    {
    case ITEM_AMPHAROSITE:
    case ITEM_HOUNDOOMINITE:
    case ITEM_LATIASITE:
    case ITEM_LATIOSITE:
        return !FlagGet(FLAG_SYS_GAME_CLEAR);
    default:
        return FALSE;
    }
}
