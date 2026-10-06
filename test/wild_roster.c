#include "global.h"
#include "caps.h"
#include "event_data.h"
#include "fishing.h"
#include "field_move.h"
#include "item.h"
#include "legendary_signs.h"
#include "overworld.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "roamer.h"
#include "string_util.h"
#include "weather_anomaly.h"
#include "wild_encounter.h"
#include "wild_roster.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/vars.h"
#include "test/test.h"

extern u16 Test_GenerateFishingWildMon(const struct WildPokemonInfo *info, u8 rod);
extern bool32 Test_GenerateCutTreeWildMon(void);

static const u16 sRosterCaughtVars[] = {
    VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static const u16 sRosterProgressFlags[] = {
    FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET,
    FLAG_VISITED_FORTREE_CITY, FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE,
    FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN,
    FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, FLAG_GOT_TM24_FROM_WATTSON,
    FLAG_LEGENDARIES_IN_SOOTOPOLIS,
    FLAG_RECEIVED_HM_CUT, FLAG_RECEIVED_HM_SURF, FLAG_RECEIVED_HM_ROCK_SMASH,
};

struct RosterTestState
{
    bool8 flags[ARRAY_COUNT(sRosterProgressFlags)];
    u16 caught[ARRAY_COUNT(sRosterCaughtVars)];
    struct WarpData location;
    u16 repel;
    u8 outbreakDays;
};

static void SaveRosterTestState(struct RosterTestState *state)
{
    for (u32 i = 0; i < ARRAY_COUNT(sRosterProgressFlags); i++)
    {
        state->flags[i] = FlagGet(sRosterProgressFlags[i]);
        FlagClear(sRosterProgressFlags[i]);
    }
    for (u32 i = 0; i < ARRAY_COUNT(sRosterCaughtVars); i++)
    {
        state->caught[i] = VarGet(sRosterCaughtVars[i]);
        VarSet(sRosterCaughtVars[i], 0);
    }
    state->location = gSaveBlock1Ptr->location;
    state->repel = VarGet(VAR_REPEL_STEP_COUNT);
    state->outbreakDays = gSaveBlock1Ptr->outbreakDaysLeft;
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    gSaveBlock1Ptr->outbreakDaysLeft = 0;
    DeactivateAllRoamers();
    ClearWeatherAnomalies();
    ZeroPlayerPartyMons();
}

static void RestoreRosterTestState(const struct RosterTestState *state)
{
    for (u32 i = 0; i < ARRAY_COUNT(sRosterProgressFlags); i++)
    {
        if (state->flags[i])
            FlagSet(sRosterProgressFlags[i]);
        else
            FlagClear(sRosterProgressFlags[i]);
    }
    for (u32 i = 0; i < ARRAY_COUNT(sRosterCaughtVars); i++)
        VarSet(sRosterCaughtVars[i], state->caught[i]);
    gSaveBlock1Ptr->location = state->location;
    VarSet(VAR_REPEL_STEP_COUNT, state->repel);
    gSaveBlock1Ptr->outbreakDaysLeft = state->outbreakDays;
    DeactivateAllRoamers();
    ClearWeatherAnomalies();
    ZeroEnemyPartyMons();
}

static void SetRosterBadges(u32 count)
{
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
    {
        if (badge < count)
            FlagSet(FLAG_BADGE01_GET + badge);
        else
            FlagClear(FLAG_BADGE01_GET + badge);
    }
}

static void SetRosterLocation(u16 map)
{
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(map);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(map);
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(map), MAP_NUM(map));
}

#define NO_ENTRY 0xFF

static u32 FindRosterEntry(const struct WildRosterEntry *roster, u32 count, u8 method, enum Species species)
{
    for (u32 i = 0; i < count; i++)
        if (roster[i].method == method && roster[i].species == species)
            return i;
    return NO_ENTRY;
}

static bool32 RosterHas(u16 map, u8 method, enum Species species)
{
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(map), MAP_NUM(map), roster, ARRAY_COUNT(roster));
    return FindRosterEntry(roster, count, method, species) != NO_ENTRY;
}

static u32 GetRosterShare(u16 map, u8 method, enum Species species)
{
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(map), MAP_NUM(map), roster, ARRAY_COUNT(roster));
    u32 i = FindRosterEntry(roster, count, method, species);
    return i == NO_ENTRY ? 0 : roster[i].share;
}

// Draws the engine's own encounters for every table method of the current
// map and checks each result against the roster: the species is listed for
// that method, its level is inside the listed range, every listed species
// turns up, and each count sits within four standard deviations of its share.
static u32 CheckRosterAgainstEngine(u16 map, u32 draws)
{
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count, failures = 0;
    u16 headerId = GetWildMonHeaderIdForMap(MAP_GROUP(map), MAP_NUM(map));
    const struct WildEncounterTypes *types;

    SetRosterLocation(map);
    count = GetWildRosterForMap(MAP_GROUP(map), MAP_NUM(map), roster, ARRAY_COUNT(roster));
    EXPECT_NE(headerId, HEADER_NONE);
    EXPECT_LE(count, WILD_ROSTER_MAX_ENTRIES);
    types = &gWildMonHeaders[headerId].encounterTypes[TIME_OF_DAY_DEFAULT];
    const struct { u8 method; const struct WildPokemonInfo *info; enum WildPokemonArea area; u8 rod; } methods[] = {
        {WILD_ROSTER_LAND, types->landMonsInfo, WILD_AREA_LAND, 0},
        {WILD_ROSTER_SURFING, types->waterMonsInfo, WILD_AREA_WATER, 0},
        {WILD_ROSTER_OLD_ROD, types->fishingMonsInfo, WILD_AREA_FISHING, OLD_ROD},
        {WILD_ROSTER_GOOD_ROD, types->fishingMonsInfo, WILD_AREA_FISHING, GOOD_ROD},
        {WILD_ROSTER_SUPER_ROD, types->fishingMonsInfo, WILD_AREA_FISHING, SUPER_ROD},
        {WILD_ROSTER_ROCK_SMASH, types->rockSmashMonsInfo, WILD_AREA_ROCKS, 0},
        {WILD_ROSTER_HONEY, types->honeyMonsInfo, WILD_AREA_HONEY, 0},
    };
    for (u32 m = 0; m < ARRAY_COUNT(methods); m++)
    {
        u16 hits[WILD_ROSTER_MAX_ENTRIES] = {0};
        u32 battles = 0, listed = 0;

        for (u32 i = 0; i < count; i++)
            listed += roster[i].method == methods[m].method;
        if (methods[m].info == NULL)
        {
            if (listed != 0)
                failures++;
            continue;
        }
        // A table alone does not enable the method. The independent access
        // tests below cover the positive and negative tool/license cases.
        if (listed == 0 && methods[m].method != WILD_ROSTER_LAND)
            continue;
        for (u32 seed = 0; seed < draws; seed++)
        {
            enum Species species;
            u32 level, entry;

            SeedRng(seed * 2654435761u + m);
            if (methods[m].area == WILD_AREA_FISHING)
                species = Test_GenerateFishingWildMon(methods[m].info, methods[m].rod);
            else if (TryGenerateWildMon(methods[m].info, methods[m].area, 0))
                species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES);
            else
                continue;
            battles++;
            level = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL);
            entry = FindRosterEntry(roster, count, methods[m].method, species);
            if (entry == NO_ENTRY)
            {
                Test_MgbaPrintf("Unlisted: map %d.%d method %d species %d", MAP_GROUP(map), MAP_NUM(map), methods[m].method, species);
                failures++;
                continue;
            }
            hits[entry]++;
            if (level < roster[entry].minLevel || level > roster[entry].maxLevel)
            {
                Test_MgbaPrintf("Level: map %d.%d method %d species %d level %d range %d-%d", MAP_GROUP(map), MAP_NUM(map),
                    methods[m].method, species, level, roster[entry].minLevel, roster[entry].maxLevel);
                failures++;
            }
        }
        for (u32 i = 0; i < count; i++)
        {
            if (roster[i].method != methods[m].method)
                continue;
            s64 error = (s64)hits[i] * WILD_SLOT_SHARE_TOTAL - (s64)battles * roster[i].share;
            s64 variance = (s64)battles * roster[i].share * (WILD_SLOT_SHARE_TOTAL - roster[i].share);
            if (hits[i] == 0 || error * error > 16 * variance + (s64)3 * WILD_SLOT_SHARE_TOTAL * 3 * WILD_SLOT_SHARE_TOTAL)
            {
                Test_MgbaPrintf("Share: map %d.%d method %d species %d hits %d of %d share %d", MAP_GROUP(map), MAP_NUM(map),
                    methods[m].method, roster[i].species, hits[i], battles, roster[i].share);
                failures++;
            }
        }
    }
    return failures;
}

// Early in the game most legend slots are gated and hand their draws on;
// late, most gates are open and the storm visitors are residents.
static void CheckRosterAgainstEngineAt(u16 map, bool32 late)
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    ClearBag();
    EXPECT(AddBagItem(ITEM_OLD_ROD, 1));
    EXPECT(AddBagItem(ITEM_GOOD_ROD, 1));
    EXPECT(AddBagItem(ITEM_SUPER_ROD, 1));
    EXPECT(AddBagItem(ITEM_HONEY, 1));
    FlagSet(FLAG_RECEIVED_HM_CUT);
    FlagSet(FLAG_RECEIVED_HM_SURF);
    FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
    if (late)
    {
        SetRosterBadges(NUM_BADGES);
        FlagSet(FLAG_VISITED_FORTREE_CITY);
        FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
        FlagSet(FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT);
        FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
        FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
    }
    EXPECT_EQ(CheckRosterAgainstEngine(map, 300), 0);
    RestoreRosterTestState(&state);
}

TEST("Wild roster: the engine agrees on Route 102, early and late")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE102, FALSE);
    CheckRosterAgainstEngineAt(MAP_ROUTE102, TRUE);
}

TEST("Wild roster: the engine agrees on Route 110, early")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE110, FALSE);
}

TEST("Wild roster: the engine agrees on Route 110, late")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE110, TRUE);
}

TEST("Wild roster: the engine agrees on Route 114, early")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE114, FALSE);
}

TEST("Wild roster: the engine agrees on Route 114, late")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE114, TRUE);
}

TEST("Wild roster: the engine agrees on Route 119, early")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE119, FALSE);
}

TEST("Wild roster: the engine agrees on Route 119, late")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE119, TRUE);
}

TEST("Wild roster: the engine agrees in Petalburg Woods, early")
{
    CheckRosterAgainstEngineAt(MAP_PETALBURG_WOODS_3, FALSE);
}

TEST("Wild roster: the engine agrees in Petalburg Woods, late")
{
    CheckRosterAgainstEngineAt(MAP_PETALBURG_WOODS_3, TRUE);
}

TEST("Wild roster: the engine agrees at the Route 111 ruins and New Mauville")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE111_RUINS_EXTERIOR, FALSE);
    CheckRosterAgainstEngineAt(MAP_ROUTE111_RUINS_EXTERIOR, TRUE);
    CheckRosterAgainstEngineAt(MAP_NEW_MAUVILLE_INSIDE, FALSE);
    CheckRosterAgainstEngineAt(MAP_NEW_MAUVILLE_INSIDE, TRUE);
}

TEST("Wild roster: the engine agrees in Altering Cave B1F")
{
    CheckRosterAgainstEngineAt(MAP_ALTERING_CAVE_B1F, FALSE);
    CheckRosterAgainstEngineAt(MAP_ALTERING_CAVE_B1F, TRUE);
}

TEST("Wild roster: the engine agrees on Route 127's water")
{
    CheckRosterAgainstEngineAt(MAP_ROUTE127, FALSE);
    CheckRosterAgainstEngineAt(MAP_ROUTE127, TRUE);
}

TEST("Wild roster: a gated legend is listed only once its gate opens, at the cap")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    // Raikou on Route 110 waits for the third badge.
    SetRosterBadges(2);
    EXPECT(!RosterHas(MAP_ROUTE110, WILD_ROSTER_LAND, SPECIES_RAIKOU));
    SetRosterBadges(3);
    EXPECT(RosterHas(MAP_ROUTE110, WILD_ROSTER_LAND, SPECIES_RAIKOU));
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110), roster, ARRAY_COUNT(roster));
    u32 raikou = FindRosterEntry(roster, count, WILD_ROSTER_LAND, SPECIES_RAIKOU);
    EXPECT_NE(raikou, NO_ENTRY);
    EXPECT_EQ(roster[raikou].minLevel, GetLegendaryEncounterLevel(SPECIES_RAIKOU));
    EXPECT_EQ(roster[raikou].maxLevel, GetLegendaryEncounterLevel(SPECIES_RAIKOU));
    RestoreRosterTestState(&state);
}

TEST("Wild roster: a caught legend drops out and the method still totals a whole")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    SetRosterBadges(3);
    EXPECT(RosterHas(MAP_ROUTE110, WILD_ROSTER_LAND, SPECIES_RAIKOU));
    MarkLegendarySignCaughtBySpecies(SPECIES_RAIKOU);
    EXPECT(!RosterHas(MAP_ROUTE110, WILD_ROSTER_LAND, SPECIES_RAIKOU));
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110), roster, ARRAY_COUNT(roster));
    u32 total = 0;
    for (u32 i = 0; i < count; i++)
        if (roster[i].method == WILD_ROSTER_LAND)
            total += roster[i].share;
    EXPECT(total >= WILD_SLOT_SHARE_TOTAL - 12 && total <= WILD_SLOT_SHARE_TOTAL + 12);
    RestoreRosterTestState(&state);
}

TEST("Wild roster: a storm visitor is listed only while its storm is on that map")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    SetRosterBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    // Tapu Bulu's home is Route 123. With the window open and no storm, its
    // own slot waits: it is not listed anywhere.
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_TAPU_BULU));
    EXPECT(!RosterHas(MAP_ROUTE123, WILD_ROSTER_LAND, SPECIES_TAPU_BULU));
    SetWeatherAnomalySlot(0, LEGENDARY_SIGN_TAPU_BULU, WEATHER_ANOMALY_DURATION_STEPS);
    EXPECT(RosterHas(MAP_ROUTE123, WILD_ROSTER_LAND, SPECIES_TAPU_BULU));
    EXPECT_EQ(GetRosterShare(MAP_ROUTE123, WILD_ROSTER_LAND, SPECIES_TAPU_BULU), 2500);
    EXPECT(!RosterHas(MAP_ROUTE123, WILD_ROSTER_SURFING, SPECIES_TAPU_BULU));
    EXPECT(!RosterHas(MAP_ROUTE111, WILD_ROSTER_LAND, SPECIES_TAPU_BULU));
    // The engine agrees while the storm rages, storm share included.
    EXPECT_EQ(CheckRosterAgainstEngine(MAP_ROUTE123, 400), 0);
    ClearWeatherAnomalies();
    EXPECT(!RosterHas(MAP_ROUTE123, WILD_ROSTER_LAND, SPECIES_TAPU_BULU));
    // After the sky calms in Sootopolis, it is an ordinary resident.
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    EXPECT(RosterHas(MAP_ROUTE123, WILD_ROSTER_LAND, SPECIES_TAPU_BULU));
    RestoreRosterTestState(&state);
}

TEST("Wild roster: roamers and outbreaks take their part of the land draws")
{
    struct RosterTestState state;
    u8 mapGroup, mapNum;
    SaveRosterTestState(&state);
    FlagSet(FLAG_BADGE05_GET);
    FlagSet(FLAG_RECEIVED_HM_SURF);
    gSpecialVar_0x8004 = 0;
    InitRoamer();
    GetRoamerLocation(0, &mapGroup, &mapNum);
    u16 map = (mapGroup << 8) | mapNum;
    u16 headerId = GetWildMonHeaderIdForMap(mapGroup, mapNum);
    EXPECT_NE(headerId, HEADER_NONE);
    if (gWildMonHeaders[headerId].encounterTypes[TIME_OF_DAY_DEFAULT].landMonsInfo != NULL)
        EXPECT_EQ(GetRosterShare(map, WILD_ROSTER_LAND, SPECIES_LATIAS), WILD_SLOT_SHARE_TOTAL / ROAMER_ENCOUNTER_ODDS);
    if (gWildMonHeaders[headerId].encounterTypes[TIME_OF_DAY_DEFAULT].waterMonsInfo != NULL)
        EXPECT_EQ(GetRosterShare(map, WILD_ROSTER_SURFING, SPECIES_LATIAS), WILD_SLOT_SHARE_TOTAL / ROAMER_ENCOUNTER_ODDS);
    DeactivateAllRoamers();
    EXPECT(!RosterHas(map, WILD_ROSTER_LAND, SPECIES_LATIAS));

    gSaveBlock1Ptr->outbreakPokemonSpecies = SPECIES_SEEDOT;
    gSaveBlock1Ptr->outbreakPokemonLevel = 3;
    gSaveBlock1Ptr->outbreakPokemonProbability = 50;
    gSaveBlock1Ptr->outbreakLocationMapGroup = MAP_GROUP(MAP_ROUTE102);
    gSaveBlock1Ptr->outbreakLocationMapNum = MAP_NUM(MAP_ROUTE102);
    gSaveBlock1Ptr->outbreakDaysLeft = 1;
    EXPECT_GE(GetRosterShare(MAP_ROUTE102, WILD_ROSTER_LAND, SPECIES_SEEDOT), WILD_SLOT_SHARE_TOTAL / 2);
    EXPECT(!RosterHas(MAP_ROUTE102, WILD_ROSTER_SURFING, SPECIES_SEEDOT));
    EXPECT(!RosterHas(MAP_ROUTE103, WILD_ROSTER_LAND, SPECIES_SEEDOT));
    RestoreRosterTestState(&state);
}

TEST("Wild roster: Feebas keeps its hidden spots and Cut trees their habitat")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    ClearBag();
    EXPECT(AddBagItem(ITEM_OLD_ROD, 1));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_RECEIVED_HM_CUT);
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    u32 feebas = FindRosterEntry(roster, count, WILD_ROSTER_FEEBAS, SPECIES_FEEBAS);
    EXPECT_NE(feebas, NO_ENTRY);
    EXPECT_EQ(roster[feebas].rarity, WILD_RARITY_VERY_RARE);
    EXPECT_EQ(roster[feebas].share, GetFeebasSpotShare());
    EXPECT(!RosterHas(MAP_ROUTE118, WILD_ROSTER_FEEBAS, SPECIES_FEEBAS));
    // Every Cut-tree species is listed where a Cut tree grows, and only there.
    for (u32 slot = 0; slot < GetCutTreeSlotCount(); slot++)
    {
        EXPECT(RosterHas(MAP_PETALBURG_WOODS, WILD_ROSTER_CUT_TREES, GetCutTreeSlotSpecies(slot)));
        EXPECT(!RosterHas(MAP_ROUTE101, WILD_ROSTER_CUT_TREES, GetCutTreeSlotSpecies(slot)));
    }
    RestoreRosterTestState(&state);
}

// A felled tree's own draws (Test_GenerateCutTreeWildMon) against the
// roster's Cut-tree entries, as CheckRosterAgainstEngine does for tables.
static u32 CheckCutTreeRosterAgainstEngine(u16 map, u32 draws)
{
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u16 hits[WILD_ROSTER_MAX_ENTRIES] = {0};
    u32 count, battles = 0, failures = 0;

    SetRosterLocation(map);
    count = GetWildRosterForMap(MAP_GROUP(map), MAP_NUM(map), roster, ARRAY_COUNT(roster));
    for (u32 seed = 0; seed < draws; seed++)
    {
        enum Species species;
        u32 level, entry;

        SeedRng(seed * 2654435761u);
        if (!Test_GenerateCutTreeWildMon())
            continue;
        battles++;
        species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES);
        level = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL);
        entry = FindRosterEntry(roster, count, WILD_ROSTER_CUT_TREES, species);
        if (entry == NO_ENTRY)
        {
            Test_MgbaPrintf("Unlisted: map %d.%d Cut tree species %d", MAP_GROUP(map), MAP_NUM(map), species);
            failures++;
            continue;
        }
        hits[entry]++;
        if (level < roster[entry].minLevel || level > roster[entry].maxLevel)
        {
            Test_MgbaPrintf("Level: map %d.%d Cut tree species %d level %d range %d-%d", MAP_GROUP(map), MAP_NUM(map),
                species, level, roster[entry].minLevel, roster[entry].maxLevel);
            failures++;
        }
    }
    for (u32 i = 0; i < count; i++)
    {
        if (roster[i].method != WILD_ROSTER_CUT_TREES)
            continue;
        s64 error = (s64)hits[i] * WILD_SLOT_SHARE_TOTAL - (s64)battles * roster[i].share;
        s64 variance = (s64)battles * roster[i].share * (WILD_SLOT_SHARE_TOTAL - roster[i].share);
        if (hits[i] == 0 || error * error > 16 * variance + (s64)3 * WILD_SLOT_SHARE_TOTAL * 3 * WILD_SLOT_SHARE_TOTAL)
        {
            Test_MgbaPrintf("Share: map %d.%d Cut tree species %d hits %d of %d share %d", MAP_GROUP(map), MAP_NUM(map),
                roster[i].species, hits[i], battles, roster[i].share);
            failures++;
        }
    }
    return failures;
}

// The share of a Cut-tree slot's Pokemon on a map, base and evolved forms
// together: each slot keeps its habitat odds whatever form it arrives in.
static u32 GetCutTreeSlotRosterShare(u16 map, u32 slot)
{
    static const enum Species evolved[][2] = {
        {SPECIES_GREEDENT, SPECIES_GREEDENT},
        {SPECIES_FORRETRESS, SPECIES_FORRETRESS},
        {SPECIES_AMBIPOM, SPECIES_AMBIPOM},
        {SPECIES_WORMADAM_PLANT, SPECIES_MOTHIM},
        {SPECIES_FLAPPLE, SPECIES_APPLETUN},
        {SPECIES_TREVENANT, SPECIES_TREVENANT},
    };
    u32 share = GetRosterShare(map, WILD_ROSTER_CUT_TREES, GetCutTreeSlotSpecies(slot))
              + GetRosterShare(map, WILD_ROSTER_CUT_TREES, evolved[slot][0]);

    if (evolved[slot][1] != evolved[slot][0])
        share += GetRosterShare(map, WILD_ROSTER_CUT_TREES, evolved[slot][1]);
    return share;
}

TEST("Wild roster: Cut trees list the forms their map's grass levels bring, as the tree does")
{
    struct RosterTestState state;
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count, entry;

    SaveRosterTestState(&state);
    FlagSet(FLAG_RECEIVED_HM_CUT);
    // Route 117's grass is in the high twenties. Under the first badge's cap
    // of 20 its tree Pokemon are held at 20: Burmy, which evolves there,
    // comes as Wormadam or Mothim; Skwovet stays itself.
    SetRosterBadges(1);
    EXPECT_EQ(GetCurrentLevelCap(), 20);
    EXPECT(RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_SKWOVET));
    EXPECT(!RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_GREEDENT));
    EXPECT(!RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_BURMY));
    EXPECT(RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_WORMADAM_PLANT));
    EXPECT(RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_MOTHIM));
    EXPECT_EQ(CheckCutTreeRosterAgainstEngine(MAP_ROUTE117, 1500), 0);
    // From the second badge they arrive at the grass levels: Skwovet as
    // Greedent, Pineco still below 31.
    SetRosterBadges(2);
    EXPECT(!RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_SKWOVET));
    EXPECT(RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_GREEDENT));
    EXPECT(RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_PINECO));
    EXPECT(!RosterHas(MAP_ROUTE117, WILD_ROSTER_CUT_TREES, SPECIES_FORRETRESS));
    EXPECT_EQ(CheckCutTreeRosterAgainstEngine(MAP_ROUTE117, 1500), 0);
    // Route 118's band straddles Phantump's 45: both forms are listed, each
    // at the levels it comes at.
    SetRosterBadges(4);
    EXPECT_GE(GetCurrentLevelCap(), 45);
    count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE118), MAP_NUM(MAP_ROUTE118), roster, ARRAY_COUNT(roster));
    entry = FindRosterEntry(roster, count, WILD_ROSTER_CUT_TREES, SPECIES_PHANTUMP);
    EXPECT_NE(entry, NO_ENTRY);
    EXPECT_LE(roster[entry].maxLevel, 44);
    entry = FindRosterEntry(roster, count, WILD_ROSTER_CUT_TREES, SPECIES_TREVENANT);
    EXPECT_NE(entry, NO_ENTRY);
    EXPECT_GE(roster[entry].minLevel, 45);
    EXPECT_EQ(CheckCutTreeRosterAgainstEngine(MAP_ROUTE118, 2000), 0);
    // Whatever form it arrives in, each slot keeps its habitat odds.
    for (u32 slot = 0; slot < GetCutTreeSlotCount(); slot++)
    {
        EXPECT_GE(GetCutTreeSlotRosterShare(MAP_ROUTE118, slot) + 2, GetCutTreeSlotOdds(slot) * WILD_SLOT_SHARE_TOTAL / 100);
        EXPECT_LE(GetCutTreeSlotRosterShare(MAP_ROUTE118, slot), GetCutTreeSlotOdds(slot) * WILD_SLOT_SHARE_TOTAL / 100 + 2);
    }
    RestoreRosterTestState(&state);
}

TEST("Wild roster: rods and field licenses expose only their usable methods")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    EXPECT(!RosterHas(MAP_ROUTE119, WILD_ROSTER_FEEBAS, SPECIES_FEEBAS));
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    for (u32 i = 0; i < count; i++)
    {
        EXPECT_NE(roster[i].method, WILD_ROSTER_SURFING);
        EXPECT_NE(roster[i].method, WILD_ROSTER_OLD_ROD);
        EXPECT_NE(roster[i].method, WILD_ROSTER_GOOD_ROD);
        EXPECT_NE(roster[i].method, WILD_ROSTER_SUPER_ROD);
        EXPECT_NE(roster[i].method, WILD_ROSTER_HONEY);
        EXPECT_NE(roster[i].method, WILD_ROSTER_CUT_TREES);
    }
    EXPECT(AddBagItem(ITEM_OLD_ROD, 1));
    EXPECT(RosterHas(MAP_ROUTE119, WILD_ROSTER_FEEBAS, SPECIES_FEEBAS));
    count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    u32 oldRod = 0, goodRod = 0;
    for (u32 i = 0; i < count; i++)
    {
        oldRod += roster[i].method == WILD_ROSTER_OLD_ROD;
        goodRod += roster[i].method == WILD_ROSTER_GOOD_ROD;
    }
    EXPECT_GT(oldRod, 0);
    EXPECT_EQ(goodRod, 0);
    EXPECT(AddPCItem(ITEM_GOOD_ROD, 1));
    count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    goodRod = 0;
    for (u32 i = 0; i < count; i++)
        goodRod += roster[i].method == WILD_ROSTER_GOOD_ROD;
    EXPECT_GT(goodRod, 0);

    FlagSet(FLAG_BADGE05_GET);
    count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    for (u32 i = 0; i < count; i++)
        EXPECT_NE(roster[i].method, WILD_ROSTER_SURFING);
    FlagSet(FLAG_RECEIVED_HM_SURF);
    count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    u32 surf = 0;
    for (u32 i = 0; i < count; i++)
        surf += roster[i].method == WILD_ROSTER_SURFING;
    EXPECT_GT(surf, 0);

    EXPECT(!RosterHas(MAP_PETALBURG_WOODS, WILD_ROSTER_CUT_TREES, SPECIES_SKWOVET));
    FlagSet(FLAG_BADGE01_GET);
    EXPECT(!RosterHas(MAP_PETALBURG_WOODS, WILD_ROSTER_CUT_TREES, SPECIES_SKWOVET));
    FlagSet(FLAG_RECEIVED_HM_CUT);
    EXPECT(RosterHas(MAP_PETALBURG_WOODS, WILD_ROSTER_CUT_TREES, SPECIES_SKWOVET));
    EXPECT(!RosterHas(MAP_ROUTE106, WILD_ROSTER_ROCK_SMASH, SPECIES_DWEBBLE));
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
    EXPECT(RosterHas(MAP_ROUTE106, WILD_ROSTER_ROCK_SMASH, SPECIES_DWEBBLE));
    EXPECT(!MapHeaderHasRockSmash(Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ROUTE109), MAP_NUM(MAP_ROUTE109))));
    EXPECT(!RosterHas(MAP_ROUTE109, WILD_ROSTER_ROCK_SMASH, SPECIES_DWEBBLE));
    EXPECT(AddBagItem(ITEM_HONEY, 1));
    count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    u32 honey = 0;
    for (u32 i = 0; i < count; i++)
        honey += roster[i].method == WILD_ROSTER_HONEY;
    EXPECT_GT(honey, 0);
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    RestoreRosterTestState(&state);
}

TEST("Wild roster: entries are grouped by method, most common first, with plain words")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), roster, ARRAY_COUNT(roster));
    EXPECT_GT(count, 0);
    EXPECT_EQ(GetWildRosterForMap(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), NULL, 0), count);
    for (u32 i = 1; i < count; i++)
    {
        EXPECT_LE(roster[i - 1].method, roster[i].method);
        if (roster[i - 1].method == roster[i].method)
        {
            EXPECT_GE(roster[i - 1].share, roster[i].share);
            EXPECT_NE(roster[i - 1].species, roster[i].species);
        }
        EXPECT_EQ(roster[i].rarity, GetWildRosterRarity(roster[i].share));
    }
    EXPECT_EQ(GetWildRosterForMap(MAP_GROUP(MAP_LITTLEROOT_TOWN), MAP_NUM(MAP_LITTLEROOT_TOWN), roster, ARRAY_COUNT(roster)), 0);
    EXPECT_EQ(StringCompare(GetWildRosterMethodName(WILD_ROSTER_LAND, MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101)), COMPOUND_STRING("Tall Grass")), 0);
    EXPECT_EQ(StringCompare(GetWildRosterMethodName(WILD_ROSTER_LAND, MAP_GROUP(MAP_GRANITE_CAVE_1F), MAP_NUM(MAP_GRANITE_CAVE_1F)), COMPOUND_STRING("Cave")), 0);
    EXPECT_EQ(StringCompare(GetWildRosterMethodName(WILD_ROSTER_SUPER_ROD, 0, 0), COMPOUND_STRING("Super Rod")), 0);
    EXPECT_EQ(StringCompare(GetWildRosterRarityName(WILD_RARITY_VERY_RARE), COMPOUND_STRING("Very Rare")), 0);
    EXPECT_EQ(GetWildRosterRarity(1500), WILD_RARITY_COMMON);
    EXPECT_EQ(GetWildRosterRarity(800), WILD_RARITY_UNCOMMON);
    EXPECT_EQ(GetWildRosterRarity(400), WILD_RARITY_RARE);
    EXPECT_EQ(GetWildRosterRarity(399), WILD_RARITY_VERY_RARE);
    // Sootopolis's water is empty while Groudon and Kyogre clash.
    FlagSet(FLAG_BADGE05_GET);
    FlagSet(FLAG_RECEIVED_HM_SURF);
    FlagSet(FLAG_LEGENDARIES_IN_SOOTOPOLIS);
    count = GetWildRosterForMap(MAP_GROUP(MAP_SOOTOPOLIS_CITY), MAP_NUM(MAP_SOOTOPOLIS_CITY), roster, ARRAY_COUNT(roster));
    for (u32 i = 0; i < count; i++)
        EXPECT_NE(roster[i].method, WILD_ROSTER_SURFING);
    RestoreRosterTestState(&state);
}

// Every map: with no storm, roamer or outbreak about, no listed species is
// below 4% of its method's battles, and every Legendary, Ultra Beast and
// Paradox Pokemon sits at exactly 5%: a gated neighbor's draws go to ordinary
// residents, never to another restricted slot.
static u32 CheckRosterFloors(void)
{
    u32 failures = 0;
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
        u8 mapGroup = gWildMonHeaders[header].mapGroup, mapNum = gWildMonHeaders[header].mapNum;
        u32 count = GetWildRosterForMap(mapGroup, mapNum, roster, ARRAY_COUNT(roster));
        for (u32 i = 0; i < count; i++)
        {
            // A Cut-tree slot keeps its habitat odds, but its evolved forms
            // share them by level and by an even form roll.
            if (roster[i].method == WILD_ROSTER_FEEBAS || roster[i].method == WILD_ROSTER_CUT_TREES)
                continue;
            bool32 restricted = GetRestrictedPartyClass(roster[i].species) != RESTRICTED_PARTY_NONE;
            if (roster[i].share < 400 || (restricted && roster[i].share != 500))
            {
                Test_MgbaPrintf("Floor: map %d.%d method %d species %d share %d", mapGroup, mapNum,
                    roster[i].method, roster[i].species, roster[i].share);
                failures++;
            }
        }
    }
    return failures;
}

TEST("Wild roster: nothing is grind-rare: every species at least 4%, restricted ones at 5%")
{
    struct RosterTestState state;
    SaveRosterTestState(&state);
    ClearBag();
    EXPECT(AddBagItem(ITEM_OLD_ROD, 1));
    EXPECT(AddBagItem(ITEM_GOOD_ROD, 1));
    EXPECT(AddBagItem(ITEM_SUPER_ROD, 1));
    EXPECT(AddBagItem(ITEM_HONEY, 1));
    FlagSet(FLAG_RECEIVED_HM_CUT);
    FlagSet(FLAG_RECEIVED_HM_SURF);
    FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
    EXPECT_EQ(CheckRosterFloors(), 0);
    SetRosterBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
    EXPECT_EQ(CheckRosterFloors(), 0);
    SetRosterBadges(NUM_BADGES);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    FlagSet(FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT);
    FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
    FlagSet(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    EXPECT_EQ(CheckRosterFloors(), 0);
    RestoreRosterTestState(&state);
}

static bool32 EngineCanRoll(u16 map, enum Species species)
{
    u16 headerId = GetWildMonHeaderIdForMap(MAP_GROUP(map), MAP_NUM(map));
    const struct WildPokemonInfo *land = gWildMonHeaders[headerId].encounterTypes[TIME_OF_DAY_DEFAULT].landMonsInfo;
    SetRosterLocation(map);
    for (u32 seed = 0; seed < 300; seed++)
    {
        SeedRng(seed);
        if (TryGenerateWildMon(land, WILD_AREA_LAND, 0)
         && GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES) == species)
            return TRUE;
    }
    return FALSE;
}

TEST("Wild roster: a caught Legendary, Ultra Beast or Paradox Pokemon never appears in the wild again")
{
    struct RosterTestState state;
    u8 dexCaught[NUM_DEX_FLAG_BYTES];
    SaveRosterTestState(&state);
    memcpy(dexCaught, gSaveBlock1Ptr->dexCaught, sizeof(dexCaught));
    // Paradox: Iron Leaves on Route 119 has no gate; the Pokedex
    // record of a catch closes its slot.
    EXPECT(RosterHas(MAP_ROUTE119, WILD_ROSTER_LAND, SPECIES_IRON_LEAVES));
    EXPECT(EngineCanRoll(MAP_ROUTE119, SPECIES_IRON_LEAVES));
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_IRON_LEAVES), FLAG_SET_CAUGHT);
    EXPECT(!IsWildSlotSpeciesAcquirable(SPECIES_IRON_LEAVES));
    EXPECT(!RosterHas(MAP_ROUTE119, WILD_ROSTER_LAND, SPECIES_IRON_LEAVES));
    EXPECT(!EngineCanRoll(MAP_ROUTE119, SPECIES_IRON_LEAVES));
    // Ultra Beast: Poipole in Seaspray Cave B1F opens at five badges; its Sign
    // closes it.
    SetRosterBadges(5);
    EXPECT(RosterHas(MAP_SEASPRAY_CAVE_B1F, WILD_ROSTER_LAND, SPECIES_POIPOLE));
    MarkLegendarySignCaughtBySpecies(SPECIES_POIPOLE);
    EXPECT(!RosterHas(MAP_SEASPRAY_CAVE_B1F, WILD_ROSTER_LAND, SPECIES_POIPOLE));
    EXPECT(!EngineCanRoll(MAP_SEASPRAY_CAVE_B1F, SPECIES_POIPOLE));
    // Legendary: Shaymin on Route 117 opens at four badges.
    EXPECT(RosterHas(MAP_ROUTE117, WILD_ROSTER_LAND, SPECIES_SHAYMIN));
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    EXPECT(!RosterHas(MAP_ROUTE117, WILD_ROSTER_LAND, SPECIES_SHAYMIN));
    EXPECT(!EngineCanRoll(MAP_ROUTE117, SPECIES_SHAYMIN));
    memcpy(gSaveBlock1Ptr->dexCaught, dexCaught, sizeof(dexCaught));
    RestoreRosterTestState(&state);
}
