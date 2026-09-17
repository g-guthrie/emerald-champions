#include "global.h"
#include "random.h"
#include "event_data.h"
#include "battle_setup.h"
#include "gym_leader_rematch.h"

static void UpdateGymLeaderRematchFromArray(const u16 *data, size_t size, u32 maxRematch);

static const u16 GymLeaderRematches_AfterNewMauville[] = {
    REMATCH_ROXANNE,
    REMATCH_BRAWLY,
    REMATCH_WATTSON,
    REMATCH_FLANNERY,
    REMATCH_NORMAN,
    REMATCH_WINONA,
    REMATCH_TATE_AND_LIZA,
    REMATCH_JUAN
};

static const u16 GymLeaderRematches_BeforeNewMauville[] = {
    REMATCH_ROXANNE,
    REMATCH_BRAWLY,
    // Wattson isn't available at this time
    REMATCH_FLANNERY,
    REMATCH_NORMAN,
    REMATCH_WINONA,
    REMATCH_TATE_AND_LIZA,
    REMATCH_JUAN
};

void UpdateGymLeaderRematch(void)
{
    if (!OW_TRAINER_REMATCHES)
        return;

    if (FlagGet(FLAG_SYS_GAME_CLEAR) && (Random() % 100) <= 30)
    {
        if (FlagGet(FLAG_WATTSON_REMATCH_AVAILABLE))
            UpdateGymLeaderRematchFromArray(GymLeaderRematches_AfterNewMauville, ARRAY_COUNT(GymLeaderRematches_AfterNewMauville), 5);
        else
            UpdateGymLeaderRematchFromArray(GymLeaderRematches_BeforeNewMauville, ARRAY_COUNT(GymLeaderRematches_BeforeNewMauville), 1);
    }
}

s32 GetCurrentGymLeaderRematchLevel(void)
{
    u32 i, j;
    u32 maxLevel = REMATCHES_COUNT;
    if (!OW_TRAINER_REMATCHES)
        return 0;
    if (!FlagGet(FLAG_SYS_GAME_CLEAR))
        return 0;
    for (i = REMATCH_SPECIAL_TRAINER_START; i < REMATCH_ELITE_FOUR_ENTRIES; i++)
    {
        for (j = 0; j < REMATCHES_COUNT; j++)
        {
            if (!HasTrainerBeenFought(gRematchTable[i].trainerIds[j]))
                break;
        }
        if (maxLevel > j)
            j = maxLevel;
    }
    return maxLevel;
}

static void UpdateGymLeaderRematchFromArray(const u16 *data, size_t size, u32 maxRematch)
{
}

