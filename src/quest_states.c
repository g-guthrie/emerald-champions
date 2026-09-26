#include "global.h"
#include "quest_states.h"
#include "event_data.h"
#include "item.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/vars.h"

// One reading of each quest's saved state, shared by the quest scripts and
// the Center local guide (see include/constants/quest_states.h).

// The Vial's capacity records the claimed rewards; Blob's position only
// matters until the Route 111 nurse has paid out.
u16 GetChanseyQuestStage(void)
{
    u16 capacity = VarGet(VAR_POKE_VIAL_MAX_CHARGES);
    u16 blob = VarGet(VAR_CHANSEY_NURSE_STATE);

    if (capacity >= POKE_VIAL_CAPACITY_ROUTE133)
        return CHANSEY_STAGE_COMPLETE;
    if (capacity == POKE_VIAL_CAPACITY_BLOB)
        return CHANSEY_STAGE_ROUTE133;
    if (blob == CHANSEY_NURSE_NEEDS_HELP)
        return CHANSEY_STAGE_NEEDS_HELP;
    if (blob < CHANSEY_NURSE_BLOB_CAUGHT)
        return CHANSEY_STAGE_CHASE;
    if (blob == CHANSEY_NURSE_BLOB_CAUGHT)
        return CHANSEY_STAGE_CAUGHT;
    return CHANSEY_STAGE_RETIRED;
}

bool32 IsChanseyVialRewardAvailable(void)
{
    return GetChanseyQuestStage() == CHANSEY_STAGE_CAUGHT;
}

u16 IsChanseyVialRewardClaimed(void)
{
    return VarGet(VAR_POKE_VIAL_MAX_CHARGES) >= POKE_VIAL_CAPACITY_BLOB;
}

u16 IsRoute133VialUpgradeAvailable(void)
{
    return GetChanseyQuestStage() == CHANSEY_STAGE_ROUTE133;
}

bool32 IsTrickHouseComplete(void)
{
    return VarGet(VAR_TRICK_HOUSE_LEVEL) >= TRICK_HOUSE_ALL_SOLVED;
}

// The ruins' object flag is set once the Keystone is picked up; the
// Abandoned Ship's trash can removes it from the Bag when it is spent.
bool32 IsOddKeystoneSpent(void)
{
    return FlagGet(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE)
        && !CheckBagHasItem(ITEM_ODD_KEYSTONE, 1) && !CheckPCHasItem(ITEM_ODD_KEYSTONE, 1);
}

bool32 HasRayquazaCalmedSootopolis(void)
{
    return VarGet(VAR_SOOTOPOLIS_CITY_STATE) >= SOOTOPOLIS_STATE_RAYQUAZA_AWAKE;
}
