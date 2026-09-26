#ifndef GUARD_WILD_ROSTER_H
#define GUARD_WILD_ROSTER_H

// The wild roster: every species that can appear on a map right now, by
// encounter method, with its level range and its real share of that method's
// battles. It is computed at run time from gWildMonHeaders through the same
// rules the encounter engine uses (src/wild_encounter.c: the table odds, the
// inert-slot hand-off, legendary gates and captures, weather-anomaly storms,
// roamers and outbreaks), so it is the single source of truth for any screen
// that lists wild Pokémon. A slot that cannot start a battle right now is not
// in it.

enum WildRosterMethod
{
    WILD_ROSTER_LAND,       // The land table: tall grass, cave floors, seaweed.
    WILD_ROSTER_SURFING,
    WILD_ROSTER_OLD_ROD,
    WILD_ROSTER_GOOD_ROD,
    WILD_ROSTER_SUPER_ROD,
    WILD_ROSTER_ROCK_SMASH,
    WILD_ROSTER_HONEY,
    WILD_ROSTER_CUT_TREES,  // The shared Cut-tree habitat, where a Cut tree grows.
    WILD_ROSTER_FEEBAS,     // Route 119's hidden Feebas spots, with any rod.
    WILD_ROSTER_METHOD_COUNT,
};

// Words the player sees instead of numbers.
enum WildRosterRarity
{
    WILD_RARITY_COMMON,
    WILD_RARITY_UNCOMMON,
    WILD_RARITY_RARE,
    WILD_RARITY_VERY_RARE,
    WILD_RARITY_COUNT,
};

struct WildRosterEntry
{
    u16 species;  // enum Species
    // Hundredths of a percent of this method's battles on this map: a
    // method's entries total 10000 (to rounding). Feebas is the exception:
    // its share is the chance that a cast from a random Route 119 fishing
    // spot hooks it (GetFeebasSpotShare).
    u16 share;
    u8 method;    // enum WildRosterMethod
    u8 minLevel;
    u8 maxLevel;
    u8 rarity;    // enum WildRosterRarity
};

// Enough for every slot of every method on one map plus the roamer, outbreak
// and storm visitor overlays.
#define WILD_ROSTER_MAX_ENTRIES 48

// Fills entries (up to max) with the map's roster and returns how many
// entries the map has. Entries are grouped by method in enum order, most
// common first within a method, one entry per species and method. Pass
// entries NULL and max 0 to count.
u32 GetWildRosterForMap(u8 mapGroup, u8 mapNum, struct WildRosterEntry *entries, u32 max);
u32 GetWildRosterForCurrentMap(struct WildRosterEntry *entries, u32 max);

enum WildRosterRarity GetWildRosterRarity(u32 share);
const u8 *GetWildRosterRarityName(enum WildRosterRarity rarity);
// "Tall Grass", "Cave", "Seaweed" or "Walking" for the land table depending
// on the map, then "Surfing", "Old Rod", "Good Rod", "Super Rod",
// "Rock Smash", "Honey", "Cut Trees" and "Hidden Spots".
const u8 *GetWildRosterMethodName(enum WildRosterMethod method, u8 mapGroup, u8 mapNum);

#endif // GUARD_WILD_ROSTER_H
