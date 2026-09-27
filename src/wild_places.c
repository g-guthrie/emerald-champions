#include "global.h"
#include "landmark.h"
#include "malloc.h"
#include "mirage_tower.h"
#include "overworld.h"
#include "region_map.h"
#include "string_util.h"
#include "text.h"
#include "wild_encounter.h"
#include "wild_places.h"
#include "wild_roster.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"

struct WildPlaceFloor
{
    u16 map;
    const u8 *name;
};

// What each map of a place with several wild maps is called within it, in the
// order the view lists them. "" is the place's main map, called by the
// place's name alone. A test fails when a map with a wild table shares its
// place with another and is missing here, or when two floors share a name.
static const struct WildPlaceFloor sWildPlaceFloors[] =
{
    {MAP_ROUTE111,                              COMPOUND_STRING("")},
    {MAP_ROUTE111_RUINS_EXTERIOR,               COMPOUND_STRING("Ruins Exterior")},
    {MAP_SAFARI_ZONE_SOUTH,                     COMPOUND_STRING("South")},
    {MAP_SAFARI_ZONE_SOUTHWEST,                 COMPOUND_STRING("Southwest")},
    {MAP_SAFARI_ZONE_SOUTHEAST,                 COMPOUND_STRING("Southeast")},
    {MAP_SAFARI_ZONE_NORTH,                     COMPOUND_STRING("North")},
    {MAP_SAFARI_ZONE_NORTHWEST,                 COMPOUND_STRING("Northwest")},
    {MAP_SAFARI_ZONE_NORTHEAST,                 COMPOUND_STRING("Northeast")},
    {MAP_ABANDONED_SHIP_ROOMS_B1F,              COMPOUND_STRING("B1F")},
    {MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS, COMPOUND_STRING("Hidden Floor")},
    {MAP_ALTERING_CAVE,                         COMPOUND_STRING("Entrance")},
    {MAP_ALTERING_CAVE_1F,                      COMPOUND_STRING("1F")},
    {MAP_ALTERING_CAVE_B1F,                     COMPOUND_STRING("B1F")},
    {MAP_ARTISAN_CAVE_1F,                       COMPOUND_STRING("1F")},
    {MAP_ARTISAN_CAVE_B1F,                      COMPOUND_STRING("B1F")},
    {MAP_CAVE_OF_ORIGIN_ENTRANCE,               COMPOUND_STRING("Entrance")},
    {MAP_CAVE_OF_ORIGIN_1F,                     COMPOUND_STRING("1F")},
    {MAP_CAVE_OF_ORIGIN_B1F,                    COMPOUND_STRING("B1F")},
    {MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1, COMPOUND_STRING("Tunnel 1")},
    {MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP2, COMPOUND_STRING("Tunnel 2")},
    {MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3, COMPOUND_STRING("Tunnel 3")},
    {MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM,          COMPOUND_STRING("Deep Chamber")},
    {MAP_GRANITE_CAVE_1F,                       COMPOUND_STRING("1F")},
    {MAP_GRANITE_CAVE_B1F,                      COMPOUND_STRING("B1F")},
    {MAP_GRANITE_CAVE_B2F,                      COMPOUND_STRING("B2F")},
    {MAP_GRANITE_CAVE_STEVENS_ROOM,             COMPOUND_STRING("Steven's Room")},
    {MAP_MAGMA_HIDEOUT_1F,                      COMPOUND_STRING("1F")},
    {MAP_MAGMA_HIDEOUT_2F_1R,                   COMPOUND_STRING("2F Room 1")},
    {MAP_MAGMA_HIDEOUT_2F_2R,                   COMPOUND_STRING("2F Room 2")},
    {MAP_MAGMA_HIDEOUT_2F_3R,                   COMPOUND_STRING("2F Room 3")},
    {MAP_MAGMA_HIDEOUT_3F_1R,                   COMPOUND_STRING("3F Room 1")},
    {MAP_MAGMA_HIDEOUT_3F_2R,                   COMPOUND_STRING("3F Room 2")},
    {MAP_MAGMA_HIDEOUT_3F_3R,                   COMPOUND_STRING("3F Room 3")},
    {MAP_MAGMA_HIDEOUT_4F,                      COMPOUND_STRING("4F")},
    {MAP_METEOR_FALLS_1F_1R,                    COMPOUND_STRING("1F Room 1")},
    {MAP_METEOR_FALLS_1F_2R,                    COMPOUND_STRING("1F Room 2")},
    {MAP_METEOR_FALLS_B1F_1R,                   COMPOUND_STRING("B1F Room 1")},
    {MAP_METEOR_FALLS_B1F_2R,                   COMPOUND_STRING("B1F Room 2")},
    {MAP_METEOR_FALLS_STEVENS_CAVE,             COMPOUND_STRING("Steven's Cave")},
    {MAP_MIRAGE_TOWER_1F,                       COMPOUND_STRING("1F")},
    {MAP_MIRAGE_TOWER_2F,                       COMPOUND_STRING("2F")},
    {MAP_MIRAGE_TOWER_3F,                       COMPOUND_STRING("3F")},
    {MAP_MIRAGE_TOWER_4F,                       COMPOUND_STRING("4F")},
    {MAP_MIRAGE_TOWER_B1F,                      COMPOUND_STRING("B1F")},
    {MAP_MT_PYRE_1F,                            COMPOUND_STRING("1F")},
    {MAP_MT_PYRE_2F,                            COMPOUND_STRING("2F")},
    {MAP_MT_PYRE_3F,                            COMPOUND_STRING("3F")},
    {MAP_MT_PYRE_4F,                            COMPOUND_STRING("4F")},
    {MAP_MT_PYRE_5F,                            COMPOUND_STRING("5F")},
    {MAP_MT_PYRE_6F,                            COMPOUND_STRING("6F")},
    {MAP_MT_PYRE_EXTERIOR,                      COMPOUND_STRING("Exterior")},
    {MAP_MT_PYRE_SUMMIT,                        COMPOUND_STRING("Summit")},
    {MAP_NEW_MAUVILLE_ENTRANCE,                 COMPOUND_STRING("Entrance")},
    {MAP_NEW_MAUVILLE_INSIDE,                   COMPOUND_STRING("Inside")},
    {MAP_PETALBURG_WOODS,                       COMPOUND_STRING("")},
    {MAP_PETALBURG_WOODS_2,                     COMPOUND_STRING("Area 2")},
    {MAP_PETALBURG_WOODS_3,                     COMPOUND_STRING("Area 3")},
    {MAP_SANDSTREWN_RUINS,                      COMPOUND_STRING("1F")},
    {MAP_SANDSTREWN_RUINS_2F,                   COMPOUND_STRING("2F")},
    {MAP_SANDSTREWN_RUINS_3F,                   COMPOUND_STRING("3F")},
    {MAP_SANDSTREWN_RUINS_B1F,                  COMPOUND_STRING("B1F")},
    {MAP_SCORCHED_SLAB_B1F,                     COMPOUND_STRING("B1F")},
    {MAP_SCORCHED_SLAB_B2F,                     COMPOUND_STRING("B2F")},
    {MAP_SEAFLOOR_CAVERN_ENTRANCE,              COMPOUND_STRING("Entrance")},
    {MAP_SEAFLOOR_CAVERN_ROOM1,                 COMPOUND_STRING("Room 1")},
    {MAP_SEAFLOOR_CAVERN_ROOM2,                 COMPOUND_STRING("Room 2")},
    {MAP_SEAFLOOR_CAVERN_ROOM3,                 COMPOUND_STRING("Room 3")},
    {MAP_SEAFLOOR_CAVERN_ROOM4,                 COMPOUND_STRING("Room 4")},
    {MAP_SEAFLOOR_CAVERN_ROOM5,                 COMPOUND_STRING("Room 5")},
    {MAP_SEAFLOOR_CAVERN_ROOM6,                 COMPOUND_STRING("Room 6")},
    {MAP_SEAFLOOR_CAVERN_ROOM7,                 COMPOUND_STRING("Room 7")},
    {MAP_SEAFLOOR_CAVERN_ROOM8,                 COMPOUND_STRING("Room 8")},
    {MAP_SEASPRAY_CAVE,                         COMPOUND_STRING("1F")},
    {MAP_SEASPRAY_CAVE_B1F,                     COMPOUND_STRING("B1F")},
    {MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM,     COMPOUND_STRING("Low Tide Entrance")},
    {MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM,        COMPOUND_STRING("Low Tide Inner Room")},
    {MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM,       COMPOUND_STRING("Stairs Room")},
    {MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM,        COMPOUND_STRING("Lower Room")},
    {MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM,          COMPOUND_STRING("Ice Room")},
    {MAP_SKY_PILLAR_1F,                         COMPOUND_STRING("1F")},
    {MAP_SKY_PILLAR_3F,                         COMPOUND_STRING("3F")},
    {MAP_SKY_PILLAR_5F,                         COMPOUND_STRING("5F")},
    {MAP_VICTORY_ROAD_1F,                       COMPOUND_STRING("1F")},
    {MAP_VICTORY_ROAD_B1F,                      COMPOUND_STRING("B1F")},
    {MAP_VICTORY_ROAD_B2F,                      COMPOUND_STRING("B2F")},
};

#define NO_FLOOR 0xFFFF

static u32 GetFloorIndex(u8 mapGroup, u8 mapNum)
{
    u16 map = (mapGroup << 8) | mapNum;

    for (u32 i = 0; i < ARRAY_COUNT(sWildPlaceFloors); i++)
    {
        if (sWildPlaceFloors[i].map == map)
            return i;
    }
    return NO_FLOOR;
}

static mapsec_u16_t GetMapSection(u8 mapGroup, u8 mapNum)
{
    return Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)->regionMapSectionId;
}

mapsec_u16_t GetWildPlaceHomeCell(u8 mapGroup, u8 mapNum)
{
    return GetRegionMapCell(GetMapSection(mapGroup, mapNum));
}

// A place that no longer stands: the fallen Mirage Tower.
static bool32 IsWildPlaceGone(mapsec_u16_t mapSec)
{
    return mapSec == MAPSEC_MIRAGE_TOWER && IsMirageTowerGone();
}

static bool32 IsPlaceInList(mapsec_u16_t place, const mapsec_u16_t *places, u32 count)
{
    for (u32 i = 0; i < count; i++)
    {
        if (places[i] == place)
            return TRUE;
    }
    return FALSE;
}

// Each map with a wild table once: Altering Cave's several tables share one
// map, and GetWildRosterForMap picks the live one.
static bool32 IsFirstHeaderOfMap(u32 headerId)
{
    return headerId == 0
        || gWildMonHeaders[headerId - 1].mapGroup != gWildMonHeaders[headerId].mapGroup
        || gWildMonHeaders[headerId - 1].mapNum != gWildMonHeaders[headerId].mapNum;
}

u32 GetWildPlaceMaps(mapsec_u16_t cell, struct WildPlaceMap *maps, u32 max)
{
    mapsec_u16_t landmarkPlaces[16];
    mapsec_u16_t places[WILD_PLACE_MAX_MAPS + 1];
    struct {
        struct WildPlaceMap map;
        u16 order;
    } found[WILD_PLACE_MAX_MAPS];
    mapsec_u16_t lastSec = MAPSEC_NONE, lastCell = MAPSEC_NONE;
    u32 landmarkCount, placeCount = 0, count = 0;

    if (cell == MAPSEC_NONE)
        return 0;
    landmarkCount = min(GetLandmarkPlaces(cell, landmarkPlaces, ARRAY_COUNT(landmarkPlaces)), ARRAY_COUNT(landmarkPlaces));

    // The places, in order: the cell itself, then those its landmarks name,
    // then any other place it holds.
    places[placeCount++] = cell;
    for (u32 i = 0; i < landmarkCount; i++)
    {
        if (!IsPlaceInList(landmarkPlaces[i], places, placeCount) && placeCount < ARRAY_COUNT(places))
            places[placeCount++] = landmarkPlaces[i];
    }

    for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
    {
        u8 mapGroup = gWildMonHeaders[i].mapGroup, mapNum = gWildMonHeaders[i].mapNum;
        mapsec_u16_t mapSec = GetMapSection(mapGroup, mapNum);
        u32 place, floor;

        if (!IsFirstHeaderOfMap(i) || IsWildPlaceGone(mapSec))
            continue;
        if (mapSec != lastSec)
        {
            lastSec = mapSec;
            lastCell = GetRegionMapCell(mapSec);
        }
        if (lastCell != cell && !IsPlaceInList(mapSec, landmarkPlaces, landmarkCount))
            continue;

        for (place = 0; place < placeCount && places[place] != mapSec; place++)
            ;
        if (place == placeCount && placeCount < ARRAY_COUNT(places))
            places[placeCount++] = mapSec;
        // Within a place, its named floors in their order, then the rest.
        floor = GetFloorIndex(mapGroup, mapNum);
        if (count < ARRAY_COUNT(found))
        {
            found[count].map = (struct WildPlaceMap){mapGroup, mapNum};
            found[count].order = place * 512 + (floor != NO_FLOOR ? floor : 256 + min(i, 255));
        }
        count++;
    }

    for (u32 i = 1; i < count && i < ARRAY_COUNT(found); i++)
    {
        u32 j = i;
        typeof(found[0]) item = found[i];

        while (j != 0 && found[j - 1].order > item.order)
        {
            found[j] = found[j - 1];
            j--;
        }
        found[j] = item;
    }
    for (u32 i = 0; i < count && i < max && i < ARRAY_COUNT(found); i++)
        maps[i] = found[i].map;
    return count;
}

bool32 WildPlaceCellHasPokemon(mapsec_u16_t cell)
{
    struct WildPlaceMap maps[WILD_PLACE_MAX_MAPS];
    u32 count = min(GetWildPlaceMaps(cell, maps, ARRAY_COUNT(maps)), ARRAY_COUNT(maps));

    for (u32 i = 0; i < count; i++)
    {
        if (GetWildRosterForMap(maps[i].mapGroup, maps[i].mapNum, NULL, 0) != 0)
            return TRUE;
    }
    return FALSE;
}

const u8 *GetWildPlaceFloorName(u8 mapGroup, u8 mapNum)
{
    u32 i = GetFloorIndex(mapGroup, mapNum);
    return i == NO_FLOOR ? NULL : sWildPlaceFloors[i].name;
}

u8 *GetWildPlaceLabel(u8 *dest, mapsec_u16_t cell, u8 mapGroup, u8 mapNum)
{
    mapsec_u16_t mapSec = GetMapSection(mapGroup, mapNum);
    const u8 *floor = GetWildPlaceFloorName(mapGroup, mapNum);
    u8 *end = dest;

    *end = EOS;
    if (mapSec != cell)
        end = GetMapNameGeneric(dest, mapSec);
    if (floor != NULL && floor[0] != EOS)
    {
        if (end != dest)
            *end++ = CHAR_SPACE;
        end = StringCopy(end, floor);
    }
    return end;
}

u8 *GetWildPlaceSectionName(u8 *dest, mapsec_u16_t cell, const struct WildPlaceSection *section)
{
    u8 *end = GetWildPlaceLabel(dest, cell, section->mapGroup, section->mapNum);
    const u8 *method = GetWildRosterMethodName(section->method, section->mapGroup, section->mapNum);

    if (end == dest)
        return StringCopy(dest, method);
    if (section->method == WILD_ROSTER_LAND)
        return end;
    end = StringCopy(end, COMPOUND_STRING(" · "));
    return StringCopy(end, method);
}

bool32 BuildWildPlaceList(mapsec_u16_t cell, struct WildPlaceList *list)
{
    struct WildPlaceMap maps[WILD_PLACE_MAX_MAPS];
    u16 counts[WILD_PLACE_MAX_MAPS];
    u32 mapCount, total = 0, sectionCount = 0, first = 0;

    *list = (struct WildPlaceList){.cell = cell};
    mapCount = min(GetWildPlaceMaps(cell, maps, ARRAY_COUNT(maps)), ARRAY_COUNT(maps));
    for (u32 i = 0; i < mapCount; i++)
    {
        counts[i] = GetWildRosterForMap(maps[i].mapGroup, maps[i].mapNum, NULL, 0);
        total += counts[i];
    }
    if (total == 0)
        return TRUE;

    list->entries = Alloc(total * sizeof(*list->entries));
    if (list->entries == NULL)
        return FALSE;
    for (u32 i = 0; i < mapCount; i++)
    {
        GetWildRosterForMap(maps[i].mapGroup, maps[i].mapNum, &list->entries[first], counts[i]);
        // Each map's entries come grouped by method: a section per group.
        for (u32 j = 0; j < counts[i]; j++)
        {
            if (j == 0 || list->entries[first + j].method != list->entries[first + j - 1].method)
                sectionCount++;
        }
        first += counts[i];
    }
    list->entryCount = total;

    list->sections = Alloc(sectionCount * sizeof(*list->sections));
    if (list->sections == NULL)
    {
        FreeWildPlaceList(list);
        return FALSE;
    }
    first = 0;
    for (u32 i = 0; i < mapCount; i++)
    {
        for (u32 j = 0; j < counts[i]; j++)
        {
            const struct WildRosterEntry *entry = &list->entries[first + j];

            if (j == 0 || entry->method != entry[-1].method)
            {
                list->sections[list->sectionCount++] = (struct WildPlaceSection){
                    .mapGroup = maps[i].mapGroup, .mapNum = maps[i].mapNum,
                    .method = entry->method, .count = 0, .first = first + j,
                };
            }
            list->sections[list->sectionCount - 1].count++;
        }
        first += counts[i];
    }
    return TRUE;
}

void FreeWildPlaceList(struct WildPlaceList *list)
{
    TRY_FREE_AND_SET_NULL(list->entries);
    TRY_FREE_AND_SET_NULL(list->sections);
    list->entryCount = 0;
    list->sectionCount = 0;
}

#if TESTING
u32 Test_GetWildPlaceFloorCount(void)
{
    return ARRAY_COUNT(sWildPlaceFloors);
}

u16 Test_GetWildPlaceFloorMap(u32 index)
{
    return sWildPlaceFloors[index].map;
}
#endif
