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
static const struct { u16 flag; u8 cap; u16 stipend; } sCampaignMilestones[] =
{
    {FLAG_BADGE01_GET, 20, 0},
    {FLAG_BADGE02_GET, 30, 0},
    {FLAG_BADGE03_GET, 40, 0},
    {FLAG_BADGE04_GET, 45, 0},
    {FLAG_BADGE05_GET, 55, 0},
    {FLAG_BADGE06_GET, 60, 0},
    {FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 65, 0},
    {FLAG_BADGE07_GET, 70, 0},
    {FLAG_BADGE08_GET, 80, 0},
    {FLAG_IS_CHAMPION, 100, 0},
};

// Milestones not in the table above still need their flag set (story gates
// depend on it) but grant no stipend and move no cap.
bool32 CompleteCampaignMilestone(u16 flag)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCampaignMilestones); i++)
    {
        if (sCampaignMilestones[i].flag != flag)
            continue;
        if (FlagGet(flag))
            return FALSE;
        FlagSet(flag);
        AddMoney(&gSaveBlock1Ptr->money, sCampaignMilestones[i].stipend);
        return TRUE;
    }
    if (FlagGet(flag))
        return FALSE;
    FlagSet(flag);
    return TRUE;
}

// Given VAR_0x8003 = flag, buffers the milestone's new cap (gStringVar1) and
// stipend (gStringVar2), both as decimal numbers with no currency sign.
// Returns 0 = not a cap/stipend milestone, 1 = cap only, 2 = stipend only,
// 3 = both, via VAR_RESULT.
void BufferCampaignMilestoneText(void)
{
    u16 flag = gSpecialVar_0x8003;
    u32 result = 0;

    for (u32 i = 0; i < ARRAY_COUNT(sCampaignMilestones); i++)
    {
        if (sCampaignMilestones[i].flag != flag)
            continue;
        if (sCampaignMilestones[i].cap != 0)
        {
            ConvertIntToDecimalStringN(gStringVar1, sCampaignMilestones[i].cap, STR_CONV_MODE_LEFT_ALIGN, 3);
            result |= 1;
        }
        if (sCampaignMilestones[i].stipend != 0)
        {
            ConvertIntToDecimalStringN(gStringVar2, sCampaignMilestones[i].stipend, STR_CONV_MODE_LEFT_ALIGN, 5);
            result |= 2;
        }
        break;
    }
    gSpecialVar_Result = result;
}

void CompleteEmeraldChampionsMilestone(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    CompleteCampaignMilestone(gSpecialVar_0x8003);
}

#if EC_HEADLESS_FIXTURES
// Headless agent bridge only: put the campaign into the milestone state that
// yields `cap`, so authored trainer level offsets resolve exactly as in play.
// This reuses the single milestone table above; it is not a second formula.
void ApplyCampaignLevelCapMilestones(u32 cap)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCampaignMilestones); i++)
    {
        if (sCampaignMilestones[i].cap != 0 && sCampaignMilestones[i].cap <= cap)
            FlagSet(sCampaignMilestones[i].flag);
    }
}
#endif

u32 GetCurrentLevelCap(void)
{
    u32 i;

    if (B_LEVEL_CAP_TYPE == LEVEL_CAP_FLAG_LIST)
    {
        for (i = ARRAY_COUNT(sCampaignMilestones); i > 0; i--)
        {
            if (sCampaignMilestones[i - 1].cap != 0 && FlagGet(sCampaignMilestones[i - 1].flag))
                return sCampaignMilestones[i - 1].cap;
        }
        return 14;
    }
    else if (B_LEVEL_CAP_TYPE == LEVEL_CAP_VARIABLE)
    {
        return VarGet(B_LEVEL_CAP_VARIABLE);
    }

    return MAX_LEVEL;
}



u32 GetLevelCapForSpecies(enum Species species, u32 baseline)
{
    const struct SpeciesInfo *info;
    u32 total;

    baseline = max(1, min(MAX_LEVEL, baseline));
    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return baseline;
    info = &gSpeciesInfo[species];
    // Temporary battle transformations do not apply a second level penalty.
    // Stable fused/rider forms retain their own configured base-stat budget.
    if (info->isMegaEvolution || info->isPrimalReversion || info->isUltraBurst
        || info->isGigantamax)
        species = GET_BASE_SPECIES_ID(species);
    info = &gSpeciesInfo[species];
    if (!(info->isRestrictedLegendary || info->isSubLegendary || info->isMythical))
        return baseline;
    total = GetSpeciesBaseStatTotal(species);
    return total > 600 ? max(1, baseline * 600 / total) : baseline;
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
