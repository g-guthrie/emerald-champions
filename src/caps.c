#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "caps.h"
#include "pokemon.h"
#include "money.h"
#include "script.h"


// A completed milestone owns both its cap and finite stipend. Capture and
// pending item delivery are separate. A zero cap leaves the current cap alone.
static const struct { u16 flag; u8 cap; u16 stipend; } sCampaignMilestones[] =
{
    {FLAG_BADGE01_GET, 20, 3000},
    {FLAG_SYS_POKENAV_GET, 0, 4000},
    {FLAG_EC_REPORT_C14_COMPLETE, 24, 0},
    {FLAG_DELIVERED_DEVON_GOODS, 30, 8000},
    {FLAG_BADGE03_GET, 34, 4000},
    {FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, 40, 6000},
    {FLAG_BADGE04_GET, 44, 0},
    {FLAG_EC_REPORT_C26_COMPLETE, 48, 8000},
    {FLAG_BADGE05_GET, 50, 5000},
    {FLAG_EC_REPORT_C28_COMPLETE, 52, 6000},
    {FLAG_EC_REPORT_C30_COMPLETE, 56, 6000},
    {FLAG_BADGE06_GET, 60, 8000},
    {FLAG_EC_REPORT_C36_COMPLETE, 64, 12000},
    {FLAG_EC_REPORT_C39_COMPLETE, 68, 10000},
    {FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, 72, 10000},
    {FLAG_EC_REPORT_C42_COMPLETE, 76, 12000},
    {FLAG_EC_REPORT_C43_COMPLETE, 78, 12000},
    {FLAG_EC_REPORT_C44_COMPLETE, 0, 12000},
    {FLAG_REGI_DOORS_OPENED, 82, 16000},
    {FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE, 86, 16000},
    {FLAG_BADGE08_GET, 90, 14000},
    {FLAG_EC_REPORT_C48_COMPLETE, 94, 20000},
    {FLAG_DEFEATED_WALLY_VICTORY_ROAD, 96, 0},
    {FLAG_IS_CHAMPION, 100, 25000},
};

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
    return FALSE;
}

void CompleteEmeraldChampionsMilestone(void)
{
    Script_RequestEffects(SCREFF_V1 | SCREFF_SAVE);
    CompleteCampaignMilestone(gSpecialVar_0x8003);
}

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
        || info->isGigantamax || info->isTeraForm)
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
