#include "global.h"
#include "item_ball.h"
#include "event_data.h"
#include "script.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/flags.h"

static u32 GetItemBallAmountFromTemplate(u32);
static u32 GetItemBallIdFromTemplate(u32);

// A standard pickup can use a custom script and need not carry an item in its
// template. Only its persistent object flag makes duplicate compensation finite.
void CheckItemBallHasPermanentReceipt(void)
{
    Script_RequestEffects(SCREFF_V1);
    gSpecialVar_Result = FALSE;
    if (gMapHeader.events == NULL)
        return;
    for (u32 i = 0; i < gMapHeader.events->objectEventCount; i++)
    {
        const struct ObjectEventTemplate *object = &gMapHeader.events->objectEvents[i];
        if (object->localId == gSpecialVar_LastTalked)
        {
            gSpecialVar_Result = object->flagId > TEMP_FLAGS_END;
            return;
        }
    }
}

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
    gSpecialVar_0x800A = gMapHeader.events->objectEvents[itemBallId].flagId != 0;
}
