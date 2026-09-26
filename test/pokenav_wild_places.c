#include "global.h"
#include "event_data.h"
#include "item.h"
#include "item_use.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokenav.h"
#include "landmark.h"
#include "malloc.h"
#include "overworld.h"
#include "region_map.h"
#include "string_util.h"
#include "wild_encounter.h"
#include "wild_places.h"
#include "wild_roster.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/vars.h"
#include "test/test.h"

// The PokeNav's Wild Pokemon view lists, for each region map cell, exactly
// what GetWildRosterForMap gives for the maps that cell holds (wild_places.c).

static mapsec_u16_t GetSection(u8 mapGroup, u8 mapNum)
{
    return Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)->regionMapSectionId;
}

static bool32 IsFirstHeader(u32 i)
{
    return i == 0 || gWildMonHeaders[i - 1].mapGroup != gWildMonHeaders[i].mapGroup
                  || gWildMonHeaders[i - 1].mapNum != gWildMonHeaders[i].mapNum;
}

#define FOR_EACH_WILD_MAP(i) \
    for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++) \
        if (IsFirstHeader(i))

static bool32 CellListsMap(mapsec_u16_t cell, u8 mapGroup, u8 mapNum)
{
    struct WildPlaceMap maps[WILD_PLACE_MAX_MAPS];
    u32 count = min(GetWildPlaceMaps(cell, maps, ARRAY_COUNT(maps)), ARRAY_COUNT(maps));

    for (u32 i = 0; i < count; i++)
    {
        if (maps[i].mapGroup == mapGroup && maps[i].mapNum == mapNum)
            return TRUE;
    }
    return FALSE;
}

static bool32 CellListsPlace(mapsec_u16_t cell, mapsec_u16_t place)
{
    struct WildPlaceMap maps[WILD_PLACE_MAX_MAPS];
    u32 count = min(GetWildPlaceMaps(cell, maps, ARRAY_COUNT(maps)), ARRAY_COUNT(maps));

    for (u32 i = 0; i < count; i++)
    {
        if (GetSection(maps[i].mapGroup, maps[i].mapNum) == place)
            return TRUE;
    }
    return FALSE;
}

TEST("Wild places: every map with a wild table has one PokeNav cell")
{
    FOR_EACH_WILD_MAP(i)
    {
        u8 mapGroup = gWildMonHeaders[i].mapGroup, mapNum = gWildMonHeaders[i].mapNum;
        mapsec_u16_t home = GetWildPlaceHomeCell(mapGroup, mapNum);

        if (!IsRegionMapCell(home))
            Test_MgbaPrintf("No cell: map %d.%d section %d", mapGroup, mapNum, GetSection(mapGroup, mapNum));
        EXPECT(IsRegionMapCell(home));
    }
}

TEST("Wild places: the region map's name and position for a place agree")
{
    FOR_EACH_WILD_MAP(i)
    {
        mapsec_u16_t mapSec = GetSection(gWildMonHeaders[i].mapGroup, gWildMonHeaders[i].mapNum);
        mapsec_u16_t byName, byPosition;

        if (IsRegionMapCell(mapSec))
            continue;
        byName = GetRegionMapCellByName(mapSec);
        byPosition = GetRegionMapCellByPosition(mapSec);
        // A special-place entry must reach a drawn cell: no dead chains.
        if (CorrectSpecialMapSecId(mapSec) != mapSec)
            EXPECT_NE(byName, MAPSEC_NONE);
        if (byName != MAPSEC_NONE && byPosition != MAPSEC_NONE && byName != byPosition)
            Test_MgbaPrintf("Section %d: named in %d, placed on %d", mapSec, byName, byPosition);
        if (byName != MAPSEC_NONE && byPosition != MAPSEC_NONE)
            EXPECT_EQ(byName, byPosition);
    }
}

TEST("Wild places: every map with a wild table is listed by its home cell")
{
    u16 towerState = VarGet(VAR_MIRAGE_TOWER_STATE);

    VarSet(VAR_MIRAGE_TOWER_STATE, 0);
    FOR_EACH_WILD_MAP(i)
    {
        u8 mapGroup = gWildMonHeaders[i].mapGroup, mapNum = gWildMonHeaders[i].mapNum;

        if (!CellListsMap(GetWildPlaceHomeCell(mapGroup, mapNum), mapGroup, mapNum))
            Test_MgbaPrintf("Unlisted: map %d.%d", mapGroup, mapNum);
        EXPECT(CellListsMap(GetWildPlaceHomeCell(mapGroup, mapNum), mapGroup, mapNum));
    }
    VarSet(VAR_MIRAGE_TOWER_STATE, towerState);
}

TEST("Wild places: no cell lists more maps than the view holds")
{
    u16 towerState = VarGet(VAR_MIRAGE_TOWER_STATE);

    VarSet(VAR_MIRAGE_TOWER_STATE, 0);
    for (mapsec_u16_t cell = 0; cell < MAPSEC_NONE; cell++)
    {
        if (IsRegionMapCell(cell))
            EXPECT_LE(GetWildPlaceMaps(cell, NULL, 0), WILD_PLACE_MAX_MAPS);
    }
    VarSet(VAR_MIRAGE_TOWER_STATE, towerState);
}

TEST("Wild places: a place's maps all have their own distinct names")
{
    FOR_EACH_WILD_MAP(i)
    {
        u8 mapGroup = gWildMonHeaders[i].mapGroup, mapNum = gWildMonHeaders[i].mapNum;
        mapsec_u16_t mapSec = GetSection(mapGroup, mapNum);
        u32 sharing = 0;

        FOR_EACH_WILD_MAP(j)
        {
            u8 otherGroup = gWildMonHeaders[j].mapGroup, otherNum = gWildMonHeaders[j].mapNum;
            u8 label[64], otherLabel[64];

            if (j == i || GetSection(otherGroup, otherNum) != mapSec)
                continue;
            sharing++;
            GetWildPlaceLabel(label, GetWildPlaceHomeCell(mapGroup, mapNum), mapGroup, mapNum);
            GetWildPlaceLabel(otherLabel, GetWildPlaceHomeCell(otherGroup, otherNum), otherGroup, otherNum);
            EXPECT_NE(StringCompare(label, otherLabel), 0);
        }
        if (sharing != 0 && GetWildPlaceFloorName(mapGroup, mapNum) == NULL)
            Test_MgbaPrintf("No floor name: map %d.%d", mapGroup, mapNum);
        if (sharing != 0)
            EXPECT(GetWildPlaceFloorName(mapGroup, mapNum) != NULL);
    }
    // And the names are all for maps with a wild table.
    for (u32 i = 0; i < Test_GetWildPlaceFloorCount(); i++)
    {
        u16 map = Test_GetWildPlaceFloorMap(i);
        EXPECT_NE(GetWildMonHeaderIdForMap(MAP_GROUP(map), MAP_NUM(map)), HEADER_NONE);
    }
}

TEST("Wild places: every heading in a cell's list is its own")
{
    for (mapsec_u16_t cell = 0; cell < MAPSEC_NONE; cell++)
    {
        struct WildPlaceList list;

        if (!IsRegionMapCell(cell))
            continue;
        EXPECT(BuildWildPlaceList(cell, &list));
        for (u32 a = 0; a < list.sectionCount; a++)
        {
            for (u32 b = a + 1; b < list.sectionCount; b++)
            {
                u8 nameA[64], nameB[64];

                GetWildPlaceSectionName(nameA, cell, &list.sections[a]);
                GetWildPlaceSectionName(nameB, cell, &list.sections[b]);
                EXPECT_NE(StringCompare(nameA, nameB), 0);
            }
        }
        FreeWildPlaceList(&list);
    }
}

// The view's list for each cell is its maps' rosters, entry for entry, and
// its sections cover those entries in order, one map and method each. Cells
// are checked in eight groups to keep each run short.
TEST("Wild places: each cell's list is exactly its maps' wild rosters")
{
    u32 group = 0;

    for (u32 i = 0; i < 8; i++)
        PARAMETRIZE { group = i; }

    for (mapsec_u16_t cell = group; cell < MAPSEC_NONE; cell += 8)
    {
        struct WildPlaceMap maps[WILD_PLACE_MAX_MAPS];
        struct WildPlaceList list;
        u32 mapCount, expected = 0, index = 0;

        if (!IsRegionMapCell(cell))
            continue;
        mapCount = min(GetWildPlaceMaps(cell, maps, ARRAY_COUNT(maps)), ARRAY_COUNT(maps));
        EXPECT(BuildWildPlaceList(cell, &list));
        for (u32 m = 0; m < mapCount; m++)
        {
            struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
            u32 count = GetWildRosterForMap(maps[m].mapGroup, maps[m].mapNum, roster, ARRAY_COUNT(roster));

            expected += count;
            for (u32 r = 0; r < count && index < list.entryCount; r++, index++)
            {
                EXPECT_EQ(list.entries[index].species, roster[r].species);
                EXPECT_EQ(list.entries[index].method, roster[r].method);
                EXPECT_EQ(list.entries[index].rarity, roster[r].rarity);
                EXPECT_EQ(list.entries[index].minLevel, roster[r].minLevel);
                EXPECT_EQ(list.entries[index].maxLevel, roster[r].maxLevel);
            }
        }
        EXPECT_EQ(list.entryCount, expected);
        index = 0;
        for (u32 s = 0; s < list.sectionCount; s++)
        {
            EXPECT_EQ(list.sections[s].first, index);
            EXPECT_NE(list.sections[s].count, 0);
            for (u32 e = 0; e < list.sections[s].count; e++)
                EXPECT_EQ(list.entries[index + e].method, list.sections[s].method);
            index += list.sections[s].count;
        }
        EXPECT_EQ(index, list.entryCount);
        FreeWildPlaceList(&list);
    }
}

// A map shows in its home cell, or where a landmark names its place, and
// nowhere else.
TEST("Wild places: a cell lists only its own maps and the places it names")
{
    for (mapsec_u16_t cell = 0; cell < MAPSEC_NONE; cell++)
    {
        struct WildPlaceMap maps[WILD_PLACE_MAX_MAPS];
        mapsec_u16_t places[16];
        u32 mapCount, placeCount;

        if (!IsRegionMapCell(cell))
            continue;
        mapCount = min(GetWildPlaceMaps(cell, maps, ARRAY_COUNT(maps)), ARRAY_COUNT(maps));
        placeCount = min(GetLandmarkPlaces(cell, places, ARRAY_COUNT(places)), ARRAY_COUNT(places));
        for (u32 m = 0; m < mapCount; m++)
        {
            mapsec_u16_t mapSec = GetSection(maps[m].mapGroup, maps[m].mapNum);
            bool32 named = FALSE;

            for (u32 p = 0; p < placeCount; p++)
                named |= places[p] == mapSec;
            EXPECT(GetWildPlaceHomeCell(maps[m].mapGroup, maps[m].mapNum) == cell || named);
        }
    }
}

// Every icon sits in a whole cell of the list's grid: whatever the art, no
// two icons in a row or in rows one above the other can touch.
TEST("Wild places: no two icons in the list can overlap")
{
    s32 minLeft = 32, maxRight = -1, minTop = 32, maxBottom = -1;

    for (enum Species species = 1; species < NUM_SPECIES; species++)
    {
        const u8 *icon;

        if (!IsSpeciesEnabled(species))
            continue;
        icon = GetMonIconPtr(species, 0);

        // Both frames of the icon's animation: 4x4 tiles of 8x8 4bpp pixels.
        for (u32 frame = 0; frame < 2; frame++)
        {
            for (u32 tile = 0; tile < 16; tile++)
            {
                const u8 *pixels = &icon[frame * 512 + tile * 32];

                for (u32 y = 0; y < 8; y++)
                {
                    for (u32 x = 0; x < 8; x++)
                    {
                        u8 byte = pixels[y * 4 + x / 2];
                        s32 px = (tile % 4) * 8 + x, py = (tile / 4) * 8 + y;

                        if (((x & 1) ? byte >> 4 : byte & 0xF) == 0)
                            continue;
                        minLeft = min(minLeft, px);
                        maxRight = max(maxRight, px);
                        minTop = min(minTop, py);
                        maxBottom = max(maxBottom, py);
                    }
                }
            }
        }
    }
    // Side by side: the widest reach right stays short of the next cell's
    // widest reach left; one above the other, likewise for height.
    EXPECT_LT(maxRight, WILD_LIST_ICON_PITCH + minLeft);
    EXPECT_LT(maxBottom, WILD_LIST_ROW_HEIGHT + minTop);
    EXPECT_LE(Test_GetWildListIconsPerRow() * WILD_LIST_ICON_PITCH, 28 * 8);
}

TEST("Wild places: landmarks that are places carry their section's name")
{
    EXPECT(Test_LandmarkPlacesMatchTheirSections());
}

TEST("Wild places: caves, towers and floors show on the cells that hold them")
{
    VarSet(VAR_MIRAGE_TOWER_STATE, 0);
    EXPECT(CellListsMap(MAPSEC_ROUTE_122, MAP_GROUP(MAP_MT_PYRE_3F), MAP_NUM(MAP_MT_PYRE_3F)));
    EXPECT(CellListsMap(MAPSEC_ROUTE_122, MAP_GROUP(MAP_MT_PYRE_SUMMIT), MAP_NUM(MAP_MT_PYRE_SUMMIT)));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_115, MAPSEC_METEOR_FALLS));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_114, MAPSEC_METEOR_FALLS));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_115, MAPSEC_SEASPRAY_CAVE));
    EXPECT(CellListsPlace(MAPSEC_EVER_GRANDE_CITY, MAPSEC_VICTORY_ROAD));
    EXPECT(CellListsPlace(MAPSEC_SOOTOPOLIS_CITY, MAPSEC_CAVE_OF_ORIGIN));
    EXPECT(CellListsPlace(MAPSEC_BATTLE_FRONTIER, MAPSEC_ARTISAN_CAVE));
    EXPECT(!CellListsPlace(MAPSEC_ROUTE_103, MAPSEC_ARTISAN_CAVE));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_103, MAPSEC_ALTERING_CAVE));
    EXPECT(CellListsPlace(MAPSEC_MT_CHIMNEY, MAPSEC_ASHEN_WOODS));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_112, MAPSEC_EMBER_PATH));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_112, MAPSEC_MAGMA_HIDEOUT));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_104, MAPSEC_PETALBURG_WOODS));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_124, MAPSEC_UNDERWATER_124));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_111, MAPSEC_MIRAGE_TOWER));
    EXPECT(CellListsMap(MAPSEC_ROUTE_111, MAP_GROUP(MAP_ROUTE111_RUINS_EXTERIOR), MAP_NUM(MAP_ROUTE111_RUINS_EXTERIOR)));
    EXPECT_EQ(GetWildPlaceMaps(MAPSEC_SAFARI_ZONE, NULL, 0), 6);
    EXPECT(WildPlaceCellHasPokemon(MAPSEC_ROUTE_110));
    EXPECT(!WildPlaceCellHasPokemon(MAPSEC_OLDALE_TOWN));
    EXPECT(!WildPlaceCellHasPokemon(MAPSEC_LITTLEROOT_TOWN));
}

TEST("Wild places: the fallen Mirage Tower leaves Route 111's list")
{
    u16 towerState = VarGet(VAR_MIRAGE_TOWER_STATE);

    VarSet(VAR_MIRAGE_TOWER_STATE, 0);
    EXPECT(CellListsPlace(MAPSEC_ROUTE_111, MAPSEC_MIRAGE_TOWER));
    VarSet(VAR_MIRAGE_TOWER_STATE, 2);
    EXPECT(!CellListsPlace(MAPSEC_ROUTE_111, MAPSEC_MIRAGE_TOWER));
    EXPECT(CellListsPlace(MAPSEC_ROUTE_111, MAPSEC_SANDSTREWN_RUINS));
    VarSet(VAR_MIRAGE_TOWER_STATE, towerState);
}

TEST("Wild places: headings name the map and the method")
{
    struct WildPlaceSection section = {MAP_GROUP(MAP_METEOR_FALLS_B1F_1R), MAP_NUM(MAP_METEOR_FALLS_B1F_1R), WILD_ROSTER_SURFING};
    u8 name[64];

    GetWildPlaceSectionName(name, MAPSEC_ROUTE_115, &section);
    EXPECT_EQ(StringCompare(name, COMPOUND_STRING("Meteor Falls B1F Room 1 · Surfing")), 0);
    section = (struct WildPlaceSection){MAP_GROUP(MAP_MT_PYRE_3F), MAP_NUM(MAP_MT_PYRE_3F), WILD_ROSTER_LAND};
    GetWildPlaceSectionName(name, MAPSEC_ROUTE_122, &section);
    EXPECT_EQ(StringCompare(name, COMPOUND_STRING("Mt. Pyre 3F")), 0);
    section = (struct WildPlaceSection){MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110), WILD_ROSTER_LAND};
    GetWildPlaceSectionName(name, MAPSEC_ROUTE_110, &section);
    EXPECT_EQ(StringCompare(name, COMPOUND_STRING("Tall Grass")), 0);
    section = (struct WildPlaceSection){MAP_GROUP(MAP_SAFARI_ZONE_NORTH), MAP_NUM(MAP_SAFARI_ZONE_NORTH), WILD_ROSTER_ROCK_SMASH};
    GetWildPlaceSectionName(name, MAPSEC_SAFARI_ZONE, &section);
    EXPECT_EQ(StringCompare(name, COMPOUND_STRING("North · Rock Smash")), 0);
}

// The PokeNav's Fly is the Flight Beacon's: offered to the fly map's places,
// exactly when the Beacon could fly the player from here.
TEST("Wild places: the PokeNav offers Fly exactly when the Flight Beacon would")
{
    static const u8 types[] = {MAPSECTYPE_NONE, MAPSECTYPE_ROUTE, MAPSECTYPE_CITY_CANFLY, MAPSECTYPE_CITY_CANTFLY, MAPSECTYPE_BATTLE_FRONTIER};
    bool8 badge = FlagGet(FLAG_BADGE06_GET), license = FlagGet(FLAG_RECEIVED_HM_FLY);
    struct MapHeader header = gMapHeader;

    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110));
    for (u32 state = 0; state < 8; state++)
    {
        bool32 hasBeacon = state & 1, unlocked = state & 2, flier = state & 4;

        RemoveBagItem(ITEM_FLIGHT_BEACON, CountTotalItemQuantityInBag(ITEM_FLIGHT_BEACON));
        if (hasBeacon)
            AddBagItem(ITEM_FLIGHT_BEACON, 1);
        if (unlocked)
        {
            FlagSet(FLAG_BADGE06_GET);
            FlagSet(FLAG_RECEIVED_HM_FLY);
        }
        else
        {
            FlagClear(FLAG_BADGE06_GET);
            FlagClear(FLAG_RECEIVED_HM_FLY);
        }
        ZeroPlayerPartyMons();
        CreateMon(&gParties[B_TRAINER_PLAYER][0], flier ? SPECIES_PIDGEOT : SPECIES_MAGIKARP, 30, 0, OTID_STRUCT_PLAYER_ID);

        EXPECT_EQ(CanFlyWithFlightBeacon(), hasBeacon && unlocked && flier);
        for (u32 t = 0; t < ARRAY_COUNT(types); t++)
            EXPECT_EQ(PokenavCanFlyTo(types[t]), IsFlyMapDestination(types[t]) && CanFlyWithFlightBeacon());
    }
    // Nowhere the Beacon cannot fly from, such as indoors.
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_MAUVILLE_CITY_POKEMON_CENTER_1F), MAP_NUM(MAP_MAUVILLE_CITY_POKEMON_CENTER_1F));
    EXPECT(!PokenavCanFlyTo(MAPSECTYPE_CITY_CANFLY));

    RemoveBagItem(ITEM_FLIGHT_BEACON, CountTotalItemQuantityInBag(ITEM_FLIGHT_BEACON));
    ZeroPlayerPartyMons();
    if (!badge)
        FlagClear(FLAG_BADGE06_GET);
    if (!license)
        FlagClear(FLAG_RECEIVED_HM_FLY);
    gMapHeader = header;
}
