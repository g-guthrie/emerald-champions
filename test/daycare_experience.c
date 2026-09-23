#include "global.h"
#include "daycare.h"
#include "event_data.h"
#include "caps.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"

TEST("Daycare experience: preview and withdrawal agree for ordinary and large step counts")
{
    u32 steps;
    PARAMETRIZE { steps = 0; }
    PARAMETRIZE { steps = 100; }
    PARAMETRIZE { steps = 1000000; }
    PARAMETRIZE { steps = UINT_MAX; }
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    u32 before = GetMonData(mon, MON_DATA_EXP);
    u32 cap = GetPlayerLevelCapForSpecies(SPECIES_EEVEE);
    u32 maximum = gExperienceTables[gSpeciesInfo[SPECIES_EEVEE].growthRate][cap];
    u32 expectedExp = before + min(steps, maximum - before);
    StorePokemonInDaycare(mon, &gSaveBlock1Ptr->daycare.mons[0]);
    gSaveBlock1Ptr->daycare.mons[0].steps = steps;
    gSpecialVar_0x8004 = 0;
    u32 gain = GetNumLevelsGainedFromDaycare();
    u8 nickname[sizeof(gStringVar1)], levels[sizeof(gStringVar2)];
    StringCopy(nickname, gStringVar1);
    StringCopy(levels, gStringVar2);
    gSpecialVar_0x8005 = 9999;
    GetDaycareCostAndPrepareString();
    EXPECT_EQ(gSpecialVar_0x8005, 0);
    EXPECT_EQ(StringCompare(gStringVar1, nickname), 0);
    EXPECT_EQ(StringCompare(gStringVar2, levels), 0);
    EXPECT_EQ(TakePokemonFromDaycare(), SPECIES_EEVEE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_EXP), expectedExp);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 5 + gain);
    EXPECT(GetMonData(mon, MON_DATA_LEVEL) <= cap);
    ZeroPlayerPartyMons();
}

TEST("Daycare experience: Hoopa preview uses its withdrawal form without changing the stored mon")
{
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_HOOPA_UNBOUND, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    StorePokemonInDaycare(mon, &gSaveBlock1Ptr->daycare.mons[0]);
    gSaveBlock1Ptr->daycare.mons[0].steps = 1000000;
    struct BoxPokemon original = gSaveBlock1Ptr->daycare.mons[0].mon;
    gSpecialVar_0x8004 = 0;
    u32 gain = GetNumLevelsGainedFromDaycare();
    u8 nickname[sizeof(gStringVar1)], levels[sizeof(gStringVar2)];
    StringCopy(nickname, gStringVar1);
    StringCopy(levels, gStringVar2);
    gSpecialVar_0x8005 = 9999;
    GetDaycareCostAndPrepareString();
    EXPECT_EQ(gSpecialVar_0x8005, 0);
    EXPECT_EQ(StringCompare(gStringVar1, nickname), 0);
    EXPECT_EQ(StringCompare(gStringVar2, levels), 0);
    EXPECT_EQ(memcmp(&original, &gSaveBlock1Ptr->daycare.mons[0].mon, sizeof(original)), 0);
    EXPECT_EQ(gain + 5, GetPlayerLevelCapForSpecies(SPECIES_HOOPA_CONFINED));
    EXPECT_EQ(TakePokemonFromDaycare(), SPECIES_HOOPA_CONFINED);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), gain + 5);
    ZeroPlayerPartyMons();
}
