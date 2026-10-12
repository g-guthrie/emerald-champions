#include "global.h"
#include "event_data.h"
#include "item.h"
#include "constants/items.h"

// Helpers for EventScript_RepelWoreOff (data/scripts/repel.inc).
// Each family's strengths are consecutive item ids: REPEL, SUPER REPEL, MAX REPEL
// (and LURE, SUPER LURE, MAX LURE).
#define NUM_SPRAY_STRENGTHS 3

u16 GetRepelToOffer(void);
void UseRepelFromPrompt(void);

static enum Item GetSprayFamily(void)
{
    return IS_LAST_USED_LURE(VarGet(VAR_REPEL_STEP_COUNT)) ? ITEM_LURE : ITEM_REPEL;
}

// Another of the kind that just wore off, otherwise the strongest kind left in the bag.
u16 GetRepelToOffer(void)
{
    enum Item lastUsed = VarGet(VAR_LAST_REPEL_LURE_USED);
    enum Item family = GetSprayFamily();

    if (lastUsed != ITEM_NONE && CheckBagHasItem(lastUsed, 1))
        return lastUsed;

    for (s32 i = NUM_SPRAY_STRENGTHS - 1; i >= 0; i--)
    {
        if (CheckBagHasItem(family + i, 1))
            return family + i;
    }
    return ITEM_NONE;
}

// Uses the spray in VAR_0x8004 exactly as using it from the bag would.
void UseRepelFromPrompt(void)
{
    enum Item spray = gSpecialVar_0x8004;
    u32 lureMask = (GetSprayFamily() == ITEM_LURE) ? REPEL_LURE_MASK : 0;

    VarSet(VAR_REPEL_STEP_COUNT, GetItemHoldEffectParam(spray) | lureMask);
    VarSet(VAR_LAST_REPEL_LURE_USED, spray);
    RemoveBagItem(spray, 1);
}
