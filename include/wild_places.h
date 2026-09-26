#ifndef GUARD_WILD_PLACES_H
#define GUARD_WILD_PLACES_H

#include "wild_roster.h"

// Wild places: the maps whose wild Pokémon a region map cell lists, for the
// PokéNav's Wild Pokémon view. A cell is a map section drawn on the region
// map (GetRegionMapCell). Every map with a wild table has one home cell: its
// own section when that is drawn, else the cell the region map names it by,
// else the cell under its region map position. A place (a cave, a tower, a
// wood) is also listed in any other cell whose landmark list names it, as
// Meteor Falls is on Route 114. Each map's Pokémon come from
// GetWildRosterForMap, so the view shows what each map holds right now.

// The most maps any one cell lists (Route 112 with Magma Hideout's floors).
#define WILD_PLACE_MAX_MAPS 16

struct WildPlaceMap
{
    u8 mapGroup;
    u8 mapNum;
};

// One heading of the view: one map and one encounter method.
struct WildPlaceSection
{
    u8 mapGroup;
    u8 mapNum;
    u8 method;  // enum WildRosterMethod
    u8 count;   // Pokémon in the section
    u16 first;  // index of its first entry
};

struct WildPlaceList
{
    mapsec_u16_t cell;
    u16 entryCount;
    u16 sectionCount;
    struct WildPlaceSection *sections;
    struct WildRosterEntry *entries;
};

mapsec_u16_t GetWildPlaceHomeCell(u8 mapGroup, u8 mapNum);
// The maps a cell lists, in the order the view shows them: the cell's own
// maps, then its places in landmark order, then any others. Returns the count.
u32 GetWildPlaceMaps(mapsec_u16_t cell, struct WildPlaceMap *maps, u32 max);
bool32 WildPlaceCellHasPokemon(mapsec_u16_t cell);
// The part of its place a map is ("3F", "Summit"), or NULL when the map is
// its place's only one with wild Pokémon. "" marks a place's main map.
const u8 *GetWildPlaceFloorName(u8 mapGroup, u8 mapNum);
// "Mt. Pyre 3F" for a place's floor, "North" for one of a cell's own maps,
// "" for the cell's own main map.
u8 *GetWildPlaceLabel(u8 *dest, mapsec_u16_t cell, u8 mapGroup, u8 mapNum);
// A section's heading: "Tall Grass" for the cell's main map, "Mt. Pyre 3F"
// for a place's walking Pokémon, "Meteor Falls B1F · Surfing" otherwise.
u8 *GetWildPlaceSectionName(u8 *dest, mapsec_u16_t cell, const struct WildPlaceSection *section);

// Allocates the list's sections and entries; FreeWildPlaceList frees them.
bool32 BuildWildPlaceList(mapsec_u16_t cell, struct WildPlaceList *list);
void FreeWildPlaceList(struct WildPlaceList *list);

#if TESTING
u32 Test_GetWildPlaceFloorCount(void);
u16 Test_GetWildPlaceFloorMap(u32 index);
#endif

#endif // GUARD_WILD_PLACES_H
