#include "global.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "caps.h"
#include "dexnav.h"
#include "event_data.h"
#include "legendary_signs.h"
#include "pokemon.h"
#include "random.h"
#include "safari_zone.h"
#include "weather_anomaly.h"
#include "wild_encounter.h"
#include "constants/flags.h"
#include "constants/layouts.h"
#include "constants/maps.h"
#include "constants/vars.h"
#include "test/test.h"

extern u8 Test_DexNavGenerateMonLevel(enum Species species, enum EncounterType environment);
extern bool32 Test_DexNavListsSpecies(enum Species species);
extern bool32 Test_DexNavIsUsableHere(void);

static const u16 sCaughtVars[] = {
    VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static const u16 sCapFlags[] = {
    FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT,
    FLAG_BADGE07_GET, FLAG_BADGE08_GET, FLAG_IS_CHAMPION,
};

static void SetCaughtLegends(bool32 caught)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCaughtVars); i++)
        VarSet(sCaughtVars[i], caught ? 0xFFFF : 0);
}

static void SetMilestones(u32 count)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCapFlags); i++)
    {
        if (i < count)
            FlagSet(sCapFlags[i]);
        else
            FlagClear(sCapFlags[i]);
    }
}

static void SetLocation(u16 map)
{
    gSaveBlock1Ptr->location.mapGroup = map >> 8;
    gSaveBlock1Ptr->location.mapNum = map & 0xFF;
}

static const struct WildPokemonInfo *CurrentLandInfo(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return NULL;
    return gWildMonHeaders[headerId].encounterTypes[GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND)].landMonsInfo;
}

static const struct WildPokemonInfo *CurrentWaterInfo(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return NULL;
    return gWildMonHeaders[headerId].encounterTypes[GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER)].waterMonsInfo;
}

TEST("DexNav never lists or searches a Legendary, Ultra Beast or Paradox slot on any map")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u16 savedCave = VarGet(VAR_ALTERING_CAVE_WILD_SET);
    u32 restricted = 0, ordinary = 0;

    VarSet(VAR_ALTERING_CAVE_WILD_SET, 0);
    for (u32 caught = 0; caught < 2; caught++)
    {
        SetCaughtLegends(caught);
        for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
        {
            gSaveBlock1Ptr->location.mapGroup = gWildMonHeaders[header].mapGroup;
            gSaveBlock1Ptr->location.mapNum = gWildMonHeaders[header].mapNum;
            if (GetCurrentMapWildMonHeaderId() != header)
                continue; // Altering Cave variants share one map.
            const struct { const struct WildPokemonInfo *info; u32 slots; enum EncounterType type; } tables[] = {
                {CurrentLandInfo(), NUM_LAND_MONS_ENCOUNTER_SLOTS, ENCOUNTER_TYPE_LAND},
                {CurrentWaterInfo(), NUM_WATER_MONS_ENCOUNTER_SLOTS, ENCOUNTER_TYPE_WATER},
            };
            for (u32 t = 0; t < ARRAY_COUNT(tables); t++)
            {
                if (tables[t].info == NULL || tables[t].info->encounterRate == 0)
                    continue;
                for (u32 slot = 0; slot < tables[t].slots; slot++)
                {
                    enum Species species = tables[t].info->wildPokemon[slot].species;
                    if (species == SPECIES_NONE)
                        continue;
                    if (GetRestrictedPartyClass(species) != RESTRICTED_PARTY_NONE)
                    {
                        EXPECT(!IsDexNavSearchableSpecies(species));
                        EXPECT(!Test_DexNavListsSpecies(species));
                        EXPECT_EQ(Test_DexNavGenerateMonLevel(species, tables[t].type), MON_LEVEL_NONEXISTENT);
                        restricted++;
                    }
                    else if (caught == 0 && slot == 0)
                    {
                        EXPECT(Test_DexNavListsSpecies(species));
                        EXPECT_NE(Test_DexNavGenerateMonLevel(species, tables[t].type), MON_LEVEL_NONEXISTENT);
                        ordinary++;
                    }
                }
            }
        }
    }
    Test_MgbaPrintf("DexNav restricted slots=%d ordinary firsts=%d", restricted, ordinary);
    EXPECT_GT(restricted, 0);
    EXPECT_GT(ordinary, 0);
    SetCaughtLegends(FALSE);
    VarSet(VAR_ALTERING_CAVE_WILD_SET, savedCave);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav cannot produce a gated, caught or storm-held legend")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;

    SetCaughtLegends(FALSE);
    SetMilestones(0);
    ClearWeatherAnomalies();

    // Route 102: Shaymin's slot is live from the start, then caught.
    SetLocation(MAP_ROUTE102);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_SHAYMIN));
    EXPECT(!Test_DexNavListsSpecies(SPECIES_SHAYMIN));
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_SHAYMIN, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_SHAYMIN));
    EXPECT(!Test_DexNavListsSpecies(SPECIES_SHAYMIN));
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_SHAYMIN, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    // Its ordinary neighbours stay searchable.
    EXPECT(Test_DexNavListsSpecies(SPECIES_LOTAD));
    EXPECT_NE(Test_DexNavGenerateMonLevel(SPECIES_LOTAD, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);

    // Route 119: gated Galarian Articuno, storm-held Tornadus, Paradox Raging Bolt.
    SetCaughtLegends(FALSE);
    SetLocation(MAP_ROUTE119);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_ARTICUNO_GALAR));
    static const enum Species legends[] = {SPECIES_ARTICUNO_GALAR, SPECIES_TORNADUS, SPECIES_RAGING_BOLT};
    for (u32 milestones = 0; milestones <= ARRAY_COUNT(sCapFlags); milestones += ARRAY_COUNT(sCapFlags))
    {
        SetMilestones(milestones);
        for (u32 i = 0; i < ARRAY_COUNT(legends); i++)
        {
            EXPECT(!Test_DexNavListsSpecies(legends[i]));
            EXPECT_EQ(Test_DexNavGenerateMonLevel(legends[i], ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
        }
    }
    EXPECT(Test_DexNavListsSpecies(SPECIES_TROPIUS));

    SetMilestones(0);
    SetCaughtLegends(FALSE);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav levels obey the live cap and the wild level floor")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u8 savedChain = gSaveBlock3Ptr->dexNavChain;
    static const struct { u16 map; enum Species species; } cases[] = {
        {MAP_ROUTE101, SPECIES_ZIGZAGOON},
        {MAP_ROUTE102, SPECIES_LOTAD},
        {MAP_ROUTE119, SPECIES_TROPIUS},
    };

    for (u32 milestones = 0; milestones <= ARRAY_COUNT(sCapFlags); milestones++)
    {
        SetMilestones(milestones);
        u32 cap = GetCurrentLevelCap();
        u32 floor = cap > 12 ? cap - 12 : 1;
        for (u32 c = 0; c < ARRAY_COUNT(cases); c++)
        {
            SetLocation(cases[c].map);
            // No chain, a long chain (+20) and the 4% +10 roll all stay in range.
            for (u32 chain = 0; chain <= DEXNAV_CHAIN_MAX; chain += DEXNAV_CHAIN_MAX)
            {
                gSaveBlock3Ptr->dexNavChain = chain;
                for (u32 seed = 0; seed < 64; seed++)
                {
                    SeedRng(seed);
                    u32 level = Test_DexNavGenerateMonLevel(cases[c].species, ENCOUNTER_TYPE_LAND);
                    EXPECT_NE(level, MON_LEVEL_NONEXISTENT);
                    EXPECT_LE(level, cap);
                    EXPECT_GE(level, floor);
                }
            }
        }
    }
    SetMilestones(0);
    gSaveBlock3Ptr->dexNavChain = savedChain;
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav needs Birch's gift and stays out of the Safari Zone, Pike and Pyramid")
{
    u16 savedLayout = gMapHeader.mapLayoutId;

    gMapHeader.mapLayoutId = LAYOUT_ROUTE101;
    FlagClear(FLAG_SYS_SAFARI_MODE);
    FlagClear(FLAG_RECEIVED_DEXNAV);
    EXPECT(!Test_DexNavIsUsableHere());
    FlagSet(FLAG_RECEIVED_DEXNAV);
    EXPECT(Test_DexNavIsUsableHere());

    FlagSet(FLAG_SYS_SAFARI_MODE);
    EXPECT(!Test_DexNavIsUsableHere());
    FlagClear(FLAG_SYS_SAFARI_MODE);

    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS;
    EXPECT(!Test_DexNavIsUsableHere());
    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR;
    EXPECT(!Test_DexNavIsUsableHere());

    gMapHeader.mapLayoutId = savedLayout;
    FlagClear(FLAG_RECEIVED_DEXNAV);
}

TEST("DexNav arrives once on saves that already hold the Pokedex")
{
    FlagClear(FLAG_RECEIVED_DEXNAV);
    FlagClear(FLAG_SYS_POKEDEX_GET);
    VarSet(VAR_DEXNAV_SPECIES, 1);
    GiveDexNavIfNeeded();
    EXPECT(!FlagGet(FLAG_RECEIVED_DEXNAV));

    FlagSet(FLAG_SYS_POKEDEX_GET);
    GiveDexNavIfNeeded();
    EXPECT(FlagGet(FLAG_RECEIVED_DEXNAV));
    EXPECT_EQ(VarGet(VAR_DEXNAV_SPECIES), SPECIES_NONE);

    // Later loads keep the player's registration.
    VarSet(VAR_DEXNAV_SPECIES, SPECIES_ZIGZAGOON);
    GiveDexNavIfNeeded();
    EXPECT_EQ(VarGet(VAR_DEXNAV_SPECIES), SPECIES_ZIGZAGOON);

    FlagClear(FLAG_SYS_POKEDEX_GET);
    FlagClear(FLAG_RECEIVED_DEXNAV);
    VarSet(VAR_DEXNAV_SPECIES, SPECIES_NONE);
}
