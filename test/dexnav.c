#include "global.h"
#include "story.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "bg.h"
#include "caps.h"
#include "dexnav.h"
#include "event_data.h"
#include "field_effect.h"
#include "fieldmap.h"
#include "main.h"
#include "item.h"
#include "malloc.h"
#include "legendary_signs.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokedex.h"
#include "random.h"
#include "roamer.h"
#include "safari_zone.h"
#include "script.h"
#include "sprite.h"
#include "string_util.h"
#include "text.h"
#include "weather_anomaly.h"
#include "wild_encounter.h"
#include "wild_roster.h"
#include "window.h"
#include "constants/field_effects.h"
#include "constants/flags.h"
#include "constants/layouts.h"
#include "constants/map_types.h"
#include "constants/maps.h"
#include "constants/metatile_labels.h"
#include "constants/vars.h"
#include "test/test.h"

extern u8 Test_DexNavGenerateMonLevel(enum Species species, enum EncounterType environment);
extern u32 Test_DexNavGetList(struct WildRosterEntry *entries, u32 max);
extern bool32 Test_DexNavCanSearchFor(const struct WildRosterEntry *entry);
extern bool32 Test_DexNavCreateSearchMon(enum Species species, enum EncounterType environment);
extern const u8 *Test_DexNavGetEntryHint(const struct WildRosterEntry *entry);
extern bool32 Test_DexNavIsUsableHere(void);
extern void Test_DexNavGetIconLayout(u32 *pitch, u32 *rowHeight, s32 *spriteTop);
extern bool32 Test_DexNavStartTimedSearch(u32 startingTime, u8 windowId, u8 spriteId);
extern bool32 Test_DexNavHasSearchData(void);

static u8 BeginTimedSearch(u32 startingTime)
{
    static const struct BgTemplate bg = {.bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31};
    static const struct WindowTemplate windows[] = {
        {.bg = 0, .tilemapLeft = 1, .tilemapTop = 16, .width = 28, .height = 3, .paletteNum = 14, .baseBlock = 8},
        DUMMY_WIN_TEMPLATE,
    };
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, &bg, 1);
    EXPECT(InitWindows(windows));
    ResetSpriteData();
    u8 spriteId = CreateSprite(&gDummySpriteTemplate, 0, 0, 0);
    EXPECT_NE(spriteId, MAX_SPRITES);
    EXPECT(Test_DexNavStartTimedSearch(startingTime, 0, spriteId));
    gSaveBlock3Ptr->dexNavChain = 73;
    ScriptContext_Init();
    return spriteId;
}

TEST("DexNav timeout: the visible 15-second limit cancels on the next step and frees the search")
{
    u32 startingTime;
    PARAMETRIZE { startingTime = 1000; }
    PARAMETRIZE { startingTime = 0xFFFFFE00; }
    u32 savedTime = gMain.vblankCounter1;
    u8 savedChain = gSaveBlock3Ptr->dexNavChain;
    u8 spriteId = BeginTimedSearch(startingTime);

    // Fix the expected historical duration independently of the config macro.
    gMain.vblankCounter1 = startingTime + 899;
    EXPECT(!OnStep_DexNavSearch());
    EXPECT(FlagGet(DN_FLAG_SEARCHING));
    gMain.vblankCounter1 = startingTime + 900;
    EXPECT(!OnStep_DexNavSearch());
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 73);
    EXPECT(!ScriptContext_IsEnabled());
    gMain.vblankCounter1 = startingTime + 901;
    EXPECT(OnStep_DexNavSearch());
    EXPECT(!FlagGet(DN_FLAG_SEARCHING));
    EXPECT(!Test_DexNavHasSearchData());
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 0);
    EXPECT(!gSprites[spriteId].inUse);
    EXPECT(!FieldEffectActiveListContains(FLDEFF_SPARKLE));
    EXPECT(gWindows[0].tileData == NULL);
    EXPECT(ScriptContext_IsEnabled());
    EXPECT(!OnStep_DexNavSearch());

    ScriptContext_Init();
    FreeAllWindowBuffers();
    gMain.vblankCounter1 = savedTime;
    gSaveBlock3Ptr->dexNavChain = savedChain;
}

TEST("DexNav timeout: cancellation clears the old deadline and a new search gets its full duration")
{
    u32 savedTime = gMain.vblankCounter1;
    u8 savedChain = gSaveBlock3Ptr->dexNavChain;
    u8 spriteId = BeginTimedSearch(1000);
    gMain.vblankCounter1 = 1900;
    ResetDexNavSearch();
    EXPECT(!FlagGet(DN_FLAG_SEARCHING));
    EXPECT(!Test_DexNavHasSearchData());
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 0);
    EXPECT(!gSprites[spriteId].inUse);
    EXPECT(!FieldEffectActiveListContains(FLDEFF_SPARKLE));
    EXPECT(gWindows[0].tileData == NULL);
    gMain.vblankCounter1 = 1901;
    EXPECT(!OnStep_DexNavSearch());
    EXPECT(!ScriptContext_IsEnabled());
    FreeAllWindowBuffers();

    BeginTimedSearch(1901);
    gMain.vblankCounter1 = 2801;
    EXPECT(!OnStep_DexNavSearch());
    EXPECT(FlagGet(DN_FLAG_SEARCHING));
    gMain.vblankCounter1 = 2802;
    EXPECT(OnStep_DexNavSearch());
    EXPECT(!Test_DexNavHasSearchData());
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 0);

    ScriptContext_Init();
    FreeAllWindowBuffers();
    gMain.vblankCounter1 = savedTime;
    gSaveBlock3Ptr->dexNavChain = savedChain;
}

// The info panel's text column: a 68-pixel box less its padding.
#define DEXNAV_INFO_TEXT_WIDTH 60

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

// Story flags that open legend gates, beyond the badges.
static const u16 sGateFlags[] = {
    FLAG_VISITED_FORTREE_CITY, FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE,
    FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN,
    FLAG_GOT_TM24_FROM_WATTSON,
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

static void SetGates(bool32 open)
{
    for (u32 i = 0; i < ARRAY_COUNT(sGateFlags); i++)
    {
        if (open)
            FlagSet(sGateFlags[i]);
        else
            FlagClear(sGateFlags[i]);
    }
}

static void SetLocation(u16 map)
{
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(map);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(map);
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(map), MAP_NUM(map));
}

static void ResetWorld(void)
{
    SetCaughtLegends(FALSE);
    SetMilestones(0);
    SetGates(FALSE);
    DeactivateAllRoamers();
    ClearWeatherAnomalies();
    gSaveBlock1Ptr->outbreakDaysLeft = 0;
    gSaveBlock3Ptr->dexNavChain = 0;
}

#define NO_ENTRY 0xFF

static u32 FindEntry(const struct WildRosterEntry *list, u32 count, u8 method, enum Species species)
{
    for (u32 i = 0; i < count; i++)
        if (list[i].method == method && list[i].species == species)
            return i;
    return NO_ENTRY;
}

static bool32 DexNavLists(enum Species species)
{
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    u32 count = Test_DexNavGetList(list, ARRAY_COUNT(list));
    for (u32 i = 0; i < count; i++)
        if (list[i].species == species)
            return TRUE;
    return FALSE;
}

// Every wild map: the DexNav lists the live roster exactly, in its order,
// and a search on land or water yields a level exactly when the roster has
// the species there, inside the roster's range.
static u32 CheckDexNavAgainstRoster(void)
{
    u32 failures = 0;

    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES], list[WILD_ROSTER_MAX_ENTRIES];
        u16 map = (gWildMonHeaders[header].mapGroup << 8) | gWildMonHeaders[header].mapNum;
        u32 count, listed;

        SetLocation(map);
        if (GetCurrentMapWildMonHeaderId() != header)
            continue; // Altering Cave variants share one map.
        count = GetWildRosterForCurrentMap(roster, ARRAY_COUNT(roster));
        listed = Test_DexNavGetList(list, ARRAY_COUNT(list));
        if (listed != count || memcmp(list, roster, count * sizeof(roster[0])) != 0)
        {
            Test_MgbaPrintf("List: map %d.%d lists %d of %d", MAP_GROUP(map), MAP_NUM(map), listed, count);
            failures++;
        }

        const struct WildEncounterTypes *types = gWildMonHeaders[header].encounterTypes;
        const struct { const struct WildPokemonInfo *info; u32 slots; enum EncounterType type; u8 method; } tables[] = {
            {types[TIME_OF_DAY_DEFAULT].landMonsInfo, NUM_LAND_MONS_ENCOUNTER_SLOTS, ENCOUNTER_TYPE_LAND, WILD_ROSTER_LAND},
            {types[TIME_OF_DAY_DEFAULT].waterMonsInfo, NUM_WATER_MONS_ENCOUNTER_SLOTS, ENCOUNTER_TYPE_WATER, WILD_ROSTER_SURFING},
        };
        for (u32 t = 0; t < ARRAY_COUNT(tables); t++)
        {
            if (tables[t].info == NULL)
                continue;
            for (u32 slot = 0; slot < tables[t].slots; slot++)
            {
                enum Species species = tables[t].info->wildPokemon[slot].species;
                u32 entry = FindEntry(roster, count, tables[t].method, species);
                bool32 searchable = entry != NO_ENTRY
                                 && !(tables[t].method == WILD_ROSTER_LAND && gMapHeader.mapType == MAP_TYPE_UNDERWATER);
                u32 level;

                SeedRng(header * 16 + slot);
                level = Test_DexNavGenerateMonLevel(species, tables[t].type);
                if (!searchable)
                {
                    if (level != MON_LEVEL_NONEXISTENT)
                    {
                        Test_MgbaPrintf("Search: map %d.%d species %d level %d", MAP_GROUP(map), MAP_NUM(map), species, level);
                        failures++;
                    }
                    continue;
                }
                // Legend-class Pokémon come exactly at their slot's levels; others may get the chain bonus.
                if (level == MON_LEVEL_NONEXISTENT || level < roster[entry].minLevel || level > GetCurrentLevelCap()
                 || (GetRestrictedPartyClass(species) != RESTRICTED_PARTY_NONE && level > roster[entry].maxLevel))
                {
                    Test_MgbaPrintf("Level: map %d.%d species %d level %d range %d-%d", MAP_GROUP(map), MAP_NUM(map),
                                    species, level, roster[entry].minLevel, roster[entry].maxLevel);
                    failures++;
                }
                // The Battle Pike's rooms list their Pokémon but keep their own rules.
                if (Test_DexNavCanSearchFor(&roster[entry]) != Test_DexNavIsUsableHere())
                    failures++;
            }
        }
    }
    return failures;
}

enum { ROSTER_EARLY, ROSTER_LATE, ROSTER_ALL_CAUGHT };

static void CheckDexNavAgainstRosterAt(u32 when)
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u16 savedCave = VarGet(VAR_ALTERING_CAVE_WILD_SET);

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    FlagClear(FLAG_SYS_SAFARI_MODE);
    VarSet(VAR_ALTERING_CAVE_WILD_SET, 0);
    ResetWorld();
    if (when != ROSTER_EARLY)
    {
        SetMilestones(ARRAY_COUNT(sCapFlags));
        SetGates(TRUE);
    }
    if (when == ROSTER_ALL_CAUGHT)
        SetCaughtLegends(TRUE);
    EXPECT_EQ(CheckDexNavAgainstRoster(), 0);
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    VarSet(VAR_ALTERING_CAVE_WILD_SET, savedCave);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav reveals Route 103 residents with an entirely unseen Pokedex")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct MapHeader savedHeader = gMapHeader;
    bool32 hadDexNav = FlagGet(FLAG_RECEIVED_DEXNAV);
    u8 seen[sizeof(gSaveBlock1Ptr->dexSeen)];
    u8 caught[sizeof(gSaveBlock1Ptr->dexCaught)];
    struct WildRosterEntry unseen[WILD_ROSTER_MAX_ENTRIES], known[WILD_ROSTER_MAX_ENTRIES];
    memcpy(seen, gSaveBlock1Ptr->dexSeen, sizeof(seen));
    memcpy(caught, gSaveBlock1Ptr->dexCaught, sizeof(caught));
    memset(gSaveBlock1Ptr->dexSeen, 0, sizeof(seen));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(caught));
    ResetWorld();
    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    SetLocation(MAP_ROUTE103);

    EXPECT_EQ(GetNationalPokedexCount(FLAG_GET_SEEN), 0);
    u32 count = Test_DexNavGetList(unseen, ARRAY_COUNT(unseen));
    EXPECT_LT(FindEntry(unseen, count, WILD_ROSTER_LAND, SPECIES_WINGULL), count);
    EXPECT_LT(FindEntry(unseen, count, WILD_ROSTER_LAND, SPECIES_TOXEL), count);
    EXPECT_LT(FindEntry(unseen, count, WILD_ROSTER_LAND, SPECIES_SNUBBULL), count);
    EXPECT_LT(FindEntry(unseen, count, WILD_ROSTER_LAND, SPECIES_SHINX), count);

    for (u32 dex = 1; dex <= NATIONAL_DEX_COUNT; dex++)
        GetSetPokedexFlag(dex, FLAG_SET_SEEN);
    EXPECT_EQ(Test_DexNavGetList(known, ARRAY_COUNT(known)), count);
    EXPECT_EQ(memcmp(unseen, known, count * sizeof(unseen[0])), 0);

    memcpy(gSaveBlock1Ptr->dexSeen, seen, sizeof(seen));
    memcpy(gSaveBlock1Ptr->dexCaught, caught, sizeof(caught));
    if (!hadDexNav)
        StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
    gMapHeader = savedHeader;
}

// Before the gates open, most legend slots are inert and hand their draws on.
TEST("DexNav lists the live roster and searches its land and Surf Pokemon, early")
{
    CheckDexNavAgainstRosterAt(ROSTER_EARLY);
}

TEST("DexNav lists the live roster and searches its land and Surf Pokemon, late")
{
    CheckDexNavAgainstRosterAt(ROSTER_LATE);
}

TEST("DexNav lists the live roster and searches its land and Surf Pokemon, legends caught")
{
    CheckDexNavAgainstRosterAt(ROSTER_ALL_CAUGHT);
}

TEST("DexNav shows and searches a gated legend once its gate opens, and drops it once caught")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u8 savedChain = gSaveBlock3Ptr->dexNavChain;

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    SetLocation(MAP_ROUTE110);
    // Raikou waits for the third badge.
    SetMilestones(2);
    EXPECT(!DexNavLists(SPECIES_RAIKOU));
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_RAIKOU, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    EXPECT(!Test_DexNavCreateSearchMon(SPECIES_RAIKOU, ENCOUNTER_TYPE_LAND));
    SetMilestones(3);
    EXPECT(DexNavLists(SPECIES_RAIKOU));
    // At the cap, even with a long chain behind it.
    gSaveBlock3Ptr->dexNavChain = DEXNAV_CHAIN_MAX;
    for (u32 seed = 0; seed < 32; seed++)
    {
        SeedRng(seed);
        EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_RAIKOU, ENCOUNTER_TYPE_LAND), GetLegendaryEncounterLevel(SPECIES_RAIKOU));
    }
    gSaveBlock3Ptr->dexNavChain = savedChain;
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_RAIKOU, ENCOUNTER_TYPE_WATER), MON_LEVEL_NONEXISTENT);
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    u32 count = Test_DexNavGetList(list, ARRAY_COUNT(list));
    u32 raikou = FindEntry(list, count, WILD_ROSTER_LAND, SPECIES_RAIKOU);
    EXPECT_NE(raikou, NO_ENTRY);
    EXPECT(Test_DexNavCanSearchFor(&list[raikou]));
    EXPECT_EQ(StringCompare(Test_DexNavGetEntryHint(&list[raikou]), COMPOUND_STRING("{A_BUTTON} Search\n{B_BUTTON} Back")), 0);
    MarkLegendarySignCaughtBySpecies(SPECIES_RAIKOU);
    EXPECT(!Test_DexNavCreateSearchMon(SPECIES_RAIKOU, ENCOUNTER_TYPE_LAND));
    EXPECT(!DexNavLists(SPECIES_RAIKOU));
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_RAIKOU, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav storm guests stay visible but cannot bypass their ordinary encounter roll")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    SetMilestones(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    SetLocation(MAP_ROUTE119);
    EXPECT(!DexNavLists(SPECIES_TORNADUS));
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_TORNADUS, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    SetWeatherAnomalySlot(0, LEGENDARY_SIGN_TORNADUS, WEATHER_ANOMALY_DURATION_STEPS);
    EXPECT(DexNavLists(SPECIES_TORNADUS));
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    u32 count = Test_DexNavGetList(list, ARRAY_COUNT(list));
    u32 koko = FindEntry(list, count, WILD_ROSTER_LAND, SPECIES_TORNADUS);
    EXPECT_NE(koko, NO_ENTRY);
    EXPECT(!Test_DexNavCanSearchFor(&list[koko]));
    EXPECT_EQ(StringCompare(Test_DexNavGetEntryHint(&list[koko]), COMPOUND_STRING("Storm guest.\nNo search.")), 0);
    EXPECT_LE(GetStringWidth(FONT_SMALL, Test_DexNavGetEntryHint(&list[koko]), 0), DEXNAV_INFO_TEXT_WIDTH);
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_TORNADUS, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    EXPECT(!Test_DexNavCreateSearchMon(SPECIES_TORNADUS, ENCOUNTER_TYPE_LAND));
    // The normal storm roll remains available, and other legends on the
    // same route remain searchable.
    EXPECT_EQ(GetWeatherAnomalyEncounterSpecies(MAP_GROUP(MAP_ROUTE119), MAP_NUM(MAP_ROUTE119), WILD_AREA_LAND), SPECIES_TORNADUS);
    EXPECT(Test_DexNavCreateSearchMon(SPECIES_ARTICUNO_GALAR, ENCOUNTER_TYPE_LAND));
    SetLocation(MAP_ROUTE111);
    EXPECT(!DexNavLists(SPECIES_TORNADUS));
    SetLocation(MAP_ROUTE119);
    ClearWeatherAnomalies();
    EXPECT(!DexNavLists(SPECIES_TORNADUS));
    EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_TORNADUS, ENCOUNTER_TYPE_LAND), MON_LEVEL_NONEXISTENT);
    // After the storm period, the ordinary resident slot follows the usual
    // search rules; only the storm encounter was excluded.
    FlagSet(WEATHER_ANOMALY_WINDOW_END_FLAG);
    EXPECT(DexNavLists(SPECIES_TORNADUS));
    EXPECT(Test_DexNavCreateSearchMon(SPECIES_TORNADUS, ENCOUNTER_TYPE_LAND));
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

struct WildMonFields
{
    u16 species;
    u8 level;
    u8 abilityNum;
    u16 item;
    u16 moves[MAX_MON_MOVES];
};

static struct WildMonFields ReadWildMonFields(void)
{
    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
    struct WildMonFields fields = {
        .species = GetMonData(mon, MON_DATA_SPECIES),
        .level = GetMonData(mon, MON_DATA_LEVEL),
        .abilityNum = GetMonData(mon, MON_DATA_ABILITY_NUM),
        .item = GetMonData(mon, MON_DATA_HELD_ITEM),
    };
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        fields.moves[i] = GetMonData(mon, MON_DATA_MOVE1 + i);
    return fields;
}

static bool32 SameWildMonFields(const struct WildMonFields *a, const struct WildMonFields *b)
{
    if (a->species != b->species || a->level != b->level || a->abilityNum != b->abilityNum || a->item != b->item)
        return FALSE;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (a->moves[i] != b->moves[i])
            return FALSE;
    return TRUE;
}

// Every searched legend is one its wild slot rolls, field for field
// (species, level, set moves, Ability and item), and every set the slot
// rolls turns up in searches too.
static void CheckSearchedLegendMatchesSlot(u16 map, enum Species species, enum EncounterType type)
{
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    const struct WildPokemonInfo info = {.encounterRate = 20, .wildPokemon = mons};
    struct WildMonFields rolled[32];
    bool8 searched[ARRAY_COUNT(rolled)] = {0};
    u32 rolledCount = 0;

    SetLocation(map);
    for (u32 i = 0; i < ARRAY_COUNT(mons); i++)
        mons[i] = (struct WildPokemon){.minLevel = 5, .maxLevel = 5, .species = species};
    for (u32 seed = 0; seed < 64; seed++)
    {
        struct WildMonFields fields;
        u32 i;

        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&info, type == ENCOUNTER_TYPE_WATER ? WILD_AREA_WATER : WILD_AREA_LAND, 0));
        fields = ReadWildMonFields();
        EXPECT_EQ(fields.level, GetLegendaryEncounterLevel(species));
        for (i = 0; i < rolledCount; i++)
            if (SameWildMonFields(&rolled[i], &fields))
                break;
        if (i == rolledCount && rolledCount < ARRAY_COUNT(rolled))
            rolled[rolledCount++] = fields;
    }
    for (u32 seed = 0; seed < 64; seed++)
    {
        struct WildMonFields fields;
        u32 i;

        SeedRng(seed + 0x1000);
        EXPECT(Test_DexNavCreateSearchMon(species, type));
        fields = ReadWildMonFields();
        for (i = 0; i < rolledCount; i++)
            if (SameWildMonFields(&rolled[i], &fields))
                break;
        EXPECT_LT(i, rolledCount);
        if (i < rolledCount)
            searched[i] = TRUE;
    }
    for (u32 i = 0; i < rolledCount; i++)
        EXPECT(searched[i]);
    Test_MgbaPrintf("Legend sets rolled for species %d: %d", species, rolledCount);
}

TEST("DexNav makes a searched legend exactly as its wild slot does")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    SetMilestones(3);
    CheckSearchedLegendMatchesSlot(MAP_ROUTE110, SPECIES_RAIKOU, ENCOUNTER_TYPE_LAND);
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav lists a roamer where it roams but never searches it")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    u8 mapGroup, mapNum;
    u32 count, checked = 0;

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    gSpecialVar_0x8004 = 0;
    InitRoamer();
    GetRoamerLocation(0, &mapGroup, &mapNum);
    SetLocation((mapGroup << 8) | mapNum);
    count = Test_DexNavGetList(list, ARRAY_COUNT(list));
    for (u32 i = 0; i < count; i++)
    {
        if (list[i].species != SPECIES_LATIAS)
            continue;
        EXPECT(!Test_DexNavCanSearchFor(&list[i]));
        EXPECT_EQ(Test_DexNavGenerateMonLevel(SPECIES_LATIAS, list[i].method == WILD_ROSTER_SURFING ? ENCOUNTER_TYPE_WATER : ENCOUNTER_TYPE_LAND),
                  MON_LEVEL_NONEXISTENT);
        checked++;
    }
    EXPECT_GT(checked, 0);
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav shows every method: rods, Rock Smash, Honey, Cut trees and Feebas show how to find them")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    bool8 seen[WILD_ROSTER_METHOD_COUNT] = {0};
    const u16 licenses[] = {FLAG_RECEIVED_HM_SURF, FLAG_RECEIVED_HM_ROCK_SMASH, FLAG_RECEIVED_HM_CUT};
    const enum Item tools[] = {ITEM_OLD_ROD, ITEM_GOOD_ROD, ITEM_SUPER_ROD, ITEM_HONEY};
    bool8 savedLicenses[ARRAY_COUNT(licenses)], addedTools[ARRAY_COUNT(tools)];

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    // Every method is tested after its real license, badge and tool gates
    // are met. An early-game roster deliberately hides locked methods.
    SetMilestones(ARRAY_COUNT(sCapFlags));
    for (u32 i = 0; i < ARRAY_COUNT(licenses); i++)
    {
        savedLicenses[i] = FlagGet(licenses[i]);
        FlagSet(licenses[i]);
    }
    for (u32 i = 0; i < ARRAY_COUNT(tools); i++)
    {
        addedTools[i] = !CheckBagHasItem(tools[i], 1) && !CheckPCHasItem(tools[i], 1);
        if (addedTools[i])
            EXPECT(AddBagItem(tools[i], 1));
    }
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        SetLocation((gWildMonHeaders[header].mapGroup << 8) | gWildMonHeaders[header].mapNum);
        u32 count = Test_DexNavGetList(list, ARRAY_COUNT(list));
        for (u32 i = 0; i < count; i++)
        {
            seen[list[i].method] = TRUE;
            if (list[i].method != WILD_ROSTER_LAND && list[i].method != WILD_ROSTER_SURFING)
                EXPECT(!Test_DexNavCanSearchFor(&list[i]));
        }
    }
    for (u32 method = 0; method < WILD_ROSTER_METHOD_COUNT; method++)
        EXPECT(seen[method]);

    SetLocation(MAP_ROUTE119);
    u32 count = Test_DexNavGetList(list, ARRAY_COUNT(list));
    u32 feebas = FindEntry(list, count, WILD_ROSTER_FEEBAS, SPECIES_FEEBAS);
    EXPECT_NE(feebas, NO_ENTRY);
    EXPECT_EQ(StringCompare(Test_DexNavGetEntryHint(&list[feebas]), COMPOUND_STRING("Bites at only\nsix spots.")), 0);
    ResetWorld();
    for (u32 i = 0; i < ARRAY_COUNT(licenses); i++)
        if (!savedLicenses[i])
            FlagClear(licenses[i]);
    for (u32 i = 0; i < ARRAY_COUNT(tools); i++)
        if (addedTools[i])
            RemoveBagItem(tools[i], 1);
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

// Widens [left, right] x [top, bottom] to every opaque pixel of both frames
// of a 32x32 4bpp icon sheet.
static void AddIconExtent(const u8 *sheet, s32 *left, s32 *right, s32 *top, s32 *bottom)
{
    for (u32 frame = 0; frame < 2; frame++)
    {
        for (s32 y = 0; y < 32; y++)
        {
            for (s32 x = 0; x < 32; x++)
            {
                u8 byte = sheet[frame * 512 + ((y / 8) * 4 + x / 8) * 32 + (y % 8) * 4 + (x % 8) / 2];
                if (((x & 1) ? byte >> 4 : byte & 0xF) == 0)
                    continue;
                *left = min(*left, x);
                *right = max(*right, x);
                *top = min(*top, y);
                *bottom = max(*bottom, y);
            }
        }
    }
}

TEST("DexNav icons never overlap each other or a section band")
{
    u32 pitch, rowHeight;
    s32 spriteTop, left = 32, right = -1, top = 32, bottom = -1;
    const u8 *last = NULL;

    Test_DexNavGetIconLayout(&pitch, &rowHeight, &spriteTop);
    for (u32 species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        const u8 *sheets[] = {
            gSpeciesInfo[species].iconSprite,
#if P_GENDER_DIFFERENCES
            gSpeciesInfo[species].iconSpriteFemale,
#endif
        };
        for (u32 i = 0; i < ARRAY_COUNT(sheets); i++)
        {
            if (sheets[i] == NULL || sheets[i] == last)
                continue;
            AddIconExtent(sheets[i], &left, &right, &top, &bottom);
            last = sheets[i];
        }
    }
    Test_MgbaPrintf("Icon pixels span x %d-%d, y %d-%d", left, right, top, bottom);
    // Neighbors a pitch apart, across or down, never share a pixel.
    EXPECT_LT(right - left, (s32)pitch);
    EXPECT_LT(bottom - top, (s32)rowHeight);
    // Every icon stays inside its own row, clear of the band above it.
    EXPECT_GE(spriteTop + top, 0);
    EXPECT_LT(spriteTop + bottom, (s32)rowHeight);
}

// Each line of every panel hint fits the info column.
static bool32 HintFits(const u8 *hint)
{
    u8 line[64];
    u32 length = 0;

    for (u32 i = 0;; i++)
    {
        if (hint[i] == CHAR_NEWLINE || hint[i] == EOS)
        {
            line[length] = EOS;
            if (GetStringWidth(FONT_SMALL, line, 0) > DEXNAV_INFO_TEXT_WIDTH)
            {
                Test_MgbaPrintf("Hint line too wide: %S", line);
                return FALSE;
            }
            if (hint[i] == EOS)
                return TRUE;
            length = 0;
        }
        else if (length < ARRAY_COUNT(line) - 1)
        {
            line[length++] = hint[i];
        }
    }
}

TEST("DexNav panel hints fit the info column on every wild map, Safari Zone included")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    u32 checked = 0;

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    for (u32 safari = 0; safari < 2; safari++)
    {
        if (safari)
            FlagSet(FLAG_SYS_SAFARI_MODE);
        for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
        {
            SetLocation((gWildMonHeaders[header].mapGroup << 8) | gWildMonHeaders[header].mapNum);
            u32 count = Test_DexNavGetList(list, ARRAY_COUNT(list));
            for (u32 i = 0; i < count; i++, checked++)
                EXPECT(HintFits(Test_DexNavGetEntryHint(&list[i])));
        }
        FlagClear(FLAG_SYS_SAFARI_MODE);
    }
    EXPECT_GT(checked, 0);
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav only shows in the Safari Zone and lists nothing where no wild Pokemon live")
{
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct WildRosterEntry list[WILD_ROSTER_MAX_ENTRIES];
    u32 count;

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    ResetWorld();
    SetLocation(MAP_SAFARI_ZONE_SOUTHWEST);
    FlagSet(FLAG_SYS_SAFARI_MODE);
    count = Test_DexNavGetList(list, ARRAY_COUNT(list));
    EXPECT_GT(count, 0);
    for (u32 i = 0; i < count; i++)
        EXPECT(!Test_DexNavCanSearchFor(&list[i]));
    FlagClear(FLAG_SYS_SAFARI_MODE);
    EXPECT(Test_DexNavCanSearchFor(&list[0]));

    SetLocation(MAP_LITTLEROOT_TOWN);
    EXPECT_EQ(Test_DexNavGetList(list, ARRAY_COUNT(list)), 0);
    ResetWorld();
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav levels obey the live cap")
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
        for (u32 c = 0; c < ARRAY_COUNT(cases); c++)
        {
            SetLocation(cases[c].map);
            // Hunting chains must not change the normal wild-slot level roll.
            for (u32 chain = 0; chain <= DEXNAV_CHAIN_MAX; chain += DEXNAV_CHAIN_MAX)
            {
                gSaveBlock3Ptr->dexNavChain = chain;
                for (u32 seed = 0; seed < 12; seed++)
                {
                    SeedRng(seed);
                    u32 level = Test_DexNavGenerateMonLevel(cases[c].species, ENCOUNTER_TYPE_LAND);
                    gSaveBlock3Ptr->dexNavChain = 0;
                    SeedRng(seed);
                    EXPECT_EQ(level, Test_DexNavGenerateMonLevel(cases[c].species, ENCOUNTER_TYPE_LAND));
                    gSaveBlock3Ptr->dexNavChain = chain;
                    EXPECT_NE(level, MON_LEVEL_NONEXISTENT);
                    EXPECT_LE(level, cap);
                }
            }
        }
    }
    SetMilestones(0);
    gSaveBlock3Ptr->dexNavChain = savedChain;
    gSaveBlock1Ptr->location = savedLocation;
}

TEST("DexNav needs Birch's gift and searches nowhere in the Safari Zone, Pike and Pyramid")
{
    u16 savedLayout = gMapHeader.mapLayoutId;

    gMapHeader.mapLayoutId = LAYOUT_ROUTE101;
    FlagClear(FLAG_SYS_SAFARI_MODE);
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    EXPECT(!Test_DexNavIsUsableHere());
    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    EXPECT(Test_DexNavIsUsableHere());

    FlagSet(FLAG_SYS_SAFARI_MODE);
    EXPECT(!Test_DexNavIsUsableHere());
    FlagClear(FLAG_SYS_SAFARI_MODE);

    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS;
    EXPECT(!Test_DexNavIsUsableHere());
    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR;
    EXPECT(!Test_DexNavIsUsableHere());

    gMapHeader.mapLayoutId = savedLayout;
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
}

TEST("DexNav arrives once on saves that already hold the Pokedex")
{
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    VarSet(VAR_DEXNAV_SPECIES, 1);
    GiveDexNavIfNeeded();
    EXPECT(!FlagGet(FLAG_RECEIVED_DEXNAV));

    StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
    GiveDexNavIfNeeded();
    EXPECT(FlagGet(FLAG_RECEIVED_DEXNAV));
    EXPECT_EQ(VarGet(VAR_DEXNAV_SPECIES), SPECIES_NONE);

    // Later loads keep the player's registration.
    VarSet(VAR_DEXNAV_SPECIES, SPECIES_ZIGZAGOON);
    GiveDexNavIfNeeded();
    EXPECT_EQ(VarGet(VAR_DEXNAV_SPECIES), SPECIES_ZIGZAGOON);

    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    VarSet(VAR_DEXNAV_SPECIES, SPECIES_NONE);
}


extern bool32 Test_DexNavPickTile(enum EncounterType environment, bool32 relocating, u32 startingTime, s16 *x, s16 *y);

TEST("DexNav placement always finds reachable habitat and excludes sealed pockets and active objects")
{
    struct MapHeader savedHeader = gMapHeader;
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct Coords16 savedPosition = gSaveBlock1Ptr->pos;
    struct BackupMapLayout savedGrid = gBackupMapLayout;
    struct PlayerAvatar savedAvatar = gPlayerAvatar;
    struct ObjectEvent savedObjects[OBJECT_EVENTS_COUNT];
    u32 savedTime = gMain.vblankCounter1;
    memcpy(savedObjects, gObjectEvents, sizeof(savedObjects));
    u16 *grid = Alloc(40 * 40 * sizeof(u16));
    EXPECT(grid != NULL);
    SetLocation(MAP_ROUTE103);
    gBackupMapLayout = (struct BackupMapLayout){.width = 40, .height = 40, .map = grid};
    for (u32 i = 0; i < 40 * 40; i++)
        grid[i] = METATILE_General_Grass | (3 << 12);
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    gPlayerAvatar.objectEventId = 0;
    gPlayerAvatar.flags = PLAYER_AVATAR_FLAG_ON_FOOT;
    gObjectEvents[0].active = TRUE;
    gObjectEvents[0].currentElevation = 3;
    gObjectEvents[0].currentCoords = (struct Coords16){14, 14};
    gSaveBlock1Ptr->pos = (struct Coords16){14 - MAP_OFFSET, 14 - MAP_OFFSET};
    s16 x, y;
    // The only usable grass tile is three ordinary steps east. A second
    // pocket, five steps north, is sealed on all four sides.
    MapGridSetMetatileIdAt(17, 14, METATILE_General_TallGrass);
    MapGridSetMetatileIdAt(14, 9, METATILE_General_TallGrass);
    MapGridSetMetatileImpassabilityAt(13, 9, TRUE);
    MapGridSetMetatileImpassabilityAt(15, 9, TRUE);
    MapGridSetMetatileImpassabilityAt(14, 8, TRUE);
    MapGridSetMetatileImpassabilityAt(14, 10, TRUE);
    for (u32 i = 0; i < 128; i++)
    {
        EXPECT(Test_DexNavPickTile(ENCOUNTER_TYPE_LAND, FALSE, 0, &x, &y));
        EXPECT_EQ(x, 17);
        EXPECT_EQ(y, 14);
    }
    MapGridSetMetatileIdAt(17, 14, METATILE_General_Grass);
    EXPECT(!Test_DexNavPickTile(ENCOUNTER_TYPE_LAND, FALSE, 0, &x, &y));
    MapGridSetMetatileIdAt(17, 14, METATILE_General_TallGrass);
    gObjectEvents[1].active = TRUE;
    gObjectEvents[1].currentElevation = 3;
    gObjectEvents[1].currentCoords = (struct Coords16){17, 14};
    gObjectEvents[1].previousCoords = (struct Coords16){17, 14};
    EXPECT(!Test_DexNavPickTile(ENCOUNTER_TYPE_LAND, FALSE, 0, &x, &y));
    gObjectEvents[1].active = FALSE;
    EXPECT(Test_DexNavPickTile(ENCOUNTER_TYPE_LAND, FALSE, 0, &x, &y));
    // A relocation must leave time for the slow native approach. The
    // three-step target takes 120 frames including the sneak pauses.
    gMain.vblankCounter1 = 1781;
    EXPECT(!Test_DexNavPickTile(ENCOUNTER_TYPE_LAND, TRUE, 1000, &x, &y));
    gMain.vblankCounter1 = 1780;
    EXPECT(Test_DexNavPickTile(ENCOUNTER_TYPE_LAND, TRUE, 1000, &x, &y));
    // Surf targets are reachable only from water in the same movement mode.
    MapGridSetMetatileIdAt(17, 14, METATILE_General_CalmWater);
    EXPECT(!Test_DexNavPickTile(ENCOUNTER_TYPE_WATER, FALSE, 0, &x, &y));
    for (u32 i = 0; i < 40 * 40; i++)
        grid[i] = METATILE_General_CalmWater | (3 << 12);
    gPlayerAvatar.flags = PLAYER_AVATAR_FLAG_SURFING;
    EXPECT(Test_DexNavPickTile(ENCOUNTER_TYPE_WATER, FALSE, 0, &x, &y));
    Free(grid);
    gBackupMapLayout = savedGrid;
    gPlayerAvatar = savedAvatar;
    gMapHeader = savedHeader;
    gSaveBlock1Ptr->location = savedLocation;
    gSaveBlock1Ptr->pos = savedPosition;
    gMain.vblankCounter1 = savedTime;
    memcpy(gObjectEvents, savedObjects, sizeof(savedObjects));
}
