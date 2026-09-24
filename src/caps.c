#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "caps.h"
#include "pokemon.h"
#include "money.h"
#include "script.h"
#include "string_util.h"


// The campaign runs on Inclement Emerald's Strict level caps, indexed by badge.
// One extra step sits at the Groudon awakening: the stretch between the sixth
// and seventh badges carries a fifth of the roster, and a single cap across all
// of it would fight Mossdeep's Gym at the level Route 121 was authored to.
// A milestone owns its cap. It pays no stipend: money comes from battles and
// from what the world is worth, the way it does in Inclement. A zero cap
// leaves the current cap alone.
// The pre-badge cap is the fallback in GetCurrentLevelCap below.
static const struct { u16 flag; u8 cap; } sCampaignMilestones[] =
{
    {FLAG_BADGE01_GET, 20},
    {FLAG_BADGE02_GET, 30},
    {FLAG_BADGE03_GET, 40},
    {FLAG_BADGE04_GET, 45},
    {FLAG_BADGE05_GET, 55},
    {FLAG_BADGE06_GET, 60},
    {FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 65},
    {FLAG_BADGE07_GET, 70},
    {FLAG_BADGE08_GET, 80},
    {FLAG_IS_CHAMPION, 100},
};

u32 GetCurrentLevelCap(void)
{
    u32 i;

    if (B_LEVEL_CAP_TYPE == LEVEL_CAP_FLAG_LIST)
    {
        // Milestones count in order: the cap stops at the first one not yet
        // reached. Nothing on Routes 120-121, Mt. Pyre or the Magma Hideout
        // checks Winona's badge, so a later flag alone must not skip a step.
        u32 cap = 14;
        for (i = 0; i < ARRAY_COUNT(sCampaignMilestones); i++)
        {
            if (!FlagGet(sCampaignMilestones[i].flag))
                break;
            if (sCampaignMilestones[i].cap != 0)
                cap = sCampaignMilestones[i].cap;
        }
        return cap;
    }
    else if (B_LEVEL_CAP_TYPE == LEVEL_CAP_VARIABLE)
    {
        return VarGet(B_LEVEL_CAP_VARIABLE);
    }

    return MAX_LEVEL;
}



u32 GetLevelCapForSpecies(enum Species species, u32 baseline)
{
    // Party composition, rather than species-specific level penalties, now
    // limits Legendary and Ultra Beast stacking.
    (void)species;
    baseline = max(1, min(MAX_LEVEL, baseline));
    return baseline;
}

u32 GetPlayerLevelCapForSpecies(enum Species species)
{
    return GetLevelCapForSpecies(species, GetCurrentLevelCap());
}

u32 GetSoftLevelCapExpValue(u32 level, u32 expValue)
{
    static const u32 sExpScalingDown[5] = { 4, 8, 16, 32, 64 };
    static const u32 sExpScalingUp[5]   = { 16, 8, 4, 2, 1 };

    u32 levelDifference;
    u32 currentLevelCap = GetCurrentLevelCap();

    if (B_EXP_CAP_TYPE == EXP_CAP_NONE)
        return expValue;

    if (level < currentLevelCap)
    {
        if (B_LEVEL_CAP_EXP_UP)
        {
            levelDifference = currentLevelCap - level;
            if (levelDifference > ARRAY_COUNT(sExpScalingUp) - 1)
                return expValue + (expValue / sExpScalingUp[ARRAY_COUNT(sExpScalingUp) - 1]);
            else
                return expValue + (expValue / sExpScalingUp[levelDifference]);
        }
        else
        {
            return expValue;
        }
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_HARD)
    {
        return 0;
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_SOFT)
    {
        levelDifference = level - currentLevelCap;
        if (levelDifference > ARRAY_COUNT(sExpScalingDown) - 1)
            return expValue / sExpScalingDown[ARRAY_COUNT(sExpScalingDown) - 1];
        else
            return expValue / sExpScalingDown[levelDifference];
    }
    else
    {
       return expValue;
    }
}

u32 GetCurrentEVCap(void)
{
    static const u16 sEvCapFlagMap[][2] = {
        // Define EV caps for each milestone
        {FLAG_BADGE01_GET, MAX_TOTAL_EVS *  1 / 17},
        {FLAG_BADGE02_GET, MAX_TOTAL_EVS *  3 / 17},
        {FLAG_BADGE03_GET, MAX_TOTAL_EVS *  5 / 17},
        {FLAG_BADGE04_GET, MAX_TOTAL_EVS *  7 / 17},
        {FLAG_BADGE05_GET, MAX_TOTAL_EVS *  9 / 17},
        {FLAG_BADGE06_GET, MAX_TOTAL_EVS * 11 / 17},
        {FLAG_BADGE07_GET, MAX_TOTAL_EVS * 13 / 17},
        {FLAG_BADGE08_GET, MAX_TOTAL_EVS * 15 / 17},
        {FLAG_IS_CHAMPION, MAX_TOTAL_EVS},
    };

    if (B_EV_CAP_TYPE == EV_CAP_FLAG_LIST)
    {
        for (u32 evCap = 0; evCap < ARRAY_COUNT(sEvCapFlagMap); evCap++)
        {
            if (!FlagGet(sEvCapFlagMap[evCap][0]))
                return sEvCapFlagMap[evCap][1];
        }
    }
    else if (B_EV_CAP_TYPE == EV_CAP_VARIABLE)
    {
        return VarGet(B_EV_CAP_VARIABLE);
    }
    else if (B_EV_CAP_TYPE == EV_CAP_NO_GAIN)
    {
        return 0;
    }

    return MAX_TOTAL_EVS;
}
