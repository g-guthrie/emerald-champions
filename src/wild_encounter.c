#include "global.h"
#include "battle_setup.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "caps.h"
#include "event_data.h"
#include "emerald_champions_battle_sets.h"
#include "legendary_signs.h"
#include "field_message_box.h"
#include "fieldmap.h"
#include "fishing.h"
#include "follower_npc.h"
#include "item.h"
#include "random.h"
#include "field_player_avatar.h"
#include "link.h"
#include "main.h"
#include "mass_outbreak.h"
#include "metatile_behavior.h"
#include "overworld.h"
#include "ow_abilities.h"
#include "pokeblock.h"
#include "pokemon.h"
#include "roamer.h"
#include "safari_zone.h"
#include "script.h"
#include "string_util.h"
#include "text.h"
#include "tv.h"
#include "wild_encounter.h"
#include "weather_anomaly.h"
#include "battle_debug.h"
#include "constants/abilities.h"
#include "constants/game_stat.h"
#include "constants/item.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/weather.h"

extern const u8 EventScript_SprayWoreOff[];
extern const u8 EmeraldChampions_EventScript_RepelSprayWoreOff[];

#define MAX_ENCOUNTER_RATE 2880

#define NUM_FEEBAS_SPOTS 6

// Number of accessible fishing spots in each section of Route 119
// Each section is an area of the route between the y coordinates in sRoute119WaterTileData
#define NUM_FISHING_SPOTS_1 131
#define NUM_FISHING_SPOTS_2 167
#define NUM_FISHING_SPOTS_3 149
#define NUM_FISHING_SPOTS (NUM_FISHING_SPOTS_1 + NUM_FISHING_SPOTS_2 + NUM_FISHING_SPOTS_3)

static u16 FeebasRandom(void);
static void FeebasSeedRng(u16 seed);
static bool8 sSweetScentActive = FALSE;
static bool8 sGeneratingSecondWildMon = FALSE;

// A table slot can yield its species: Legendary/UB gate and capture rules,
// plus weather-anomaly visitors, whose own slot waits for the window to close.
bool32 IsWildSlotLive(enum Species species)
{
    return IsWildSlotSpeciesAcquirable(species) && !IsWeatherAnomalyVisitorSlotInert(species);
}

static void ApplyCleanseTagEncounterRateMod(u32 *encRate);
static u8 GetMaxLevelOfSpeciesInWildTable(const struct WildPokemon *wildMon, enum Species species, enum WildPokemonArea area);
#ifdef BUGFIX
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex, u32 size);
#else
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex);
#endif

EWRAM_DATA static u8 sWildEncountersDisabled = 0;
EWRAM_DATA static u32 sFeebasRngValue = 0;
EWRAM_DATA bool8 gIsFishingEncounter = 0;
EWRAM_DATA bool8 gIsSurfingEncounter = 0;
EWRAM_DATA u8 gChainFishingDexNavStreak = 0;

#include "data/wild_encounters.h"

// Every Cut tree in the wild shares this one habitat, and these six species
// live nowhere else (scripts/verify_wild_distribution.py reads this table and
// keeps them out of every map table). Odds are percent of tree encounters.
// A felled tree hides a Pokemon one time in three: the "sometimes" of
// Headbutt trees and of Sword/Shield's shaken Berry trees, and Inclement's
// own tree rule. The level sits just under the live cap like every land
// table: cap minus 8 to cap minus 4. Maps with no wild Pokemon at all (the
// Trick House puzzle rooms) never roll.
#define CUT_TREE_ENCOUNTER_ODDS 3
#define CUT_TREE_LEVELS_BELOW_CAP_MIN 4
#define CUT_TREE_LEVELS_BELOW_CAP_MAX 8

static const struct CutTreeHabitatSlot
{
    enum Species species;
    u8 odds;
} sCutTreeHabitat[] =
{
    {SPECIES_SKWOVET, 38},
    {SPECIES_PINECO, 30},
    {SPECIES_AIPOM, 15},
    {SPECIES_BURMY, 8},
    {SPECIES_APPLIN, 5},
    {SPECIES_PHANTUMP, 4},
};

extern const u8 EventScript_CutTree[];
extern const u8 EventScript_RockSmash[];

u32 GetCutTreeSlotCount(void)
{
    return ARRAY_COUNT(sCutTreeHabitat);
}

enum Species GetCutTreeSlotSpecies(u32 slot)
{
    return slot < ARRAY_COUNT(sCutTreeHabitat) ? sCutTreeHabitat[slot].species : SPECIES_NONE;
}

bool32 IsCutTreeHabitatSpecies(enum Species species)
{
    for (u32 slot = 0; slot < ARRAY_COUNT(sCutTreeHabitat); slot++)
        if (sCutTreeHabitat[slot].species == species)
            return TRUE;
    return FALSE;
}

u32 GetCutTreeSlotOdds(u32 slot)
{
    return slot < ARRAY_COUNT(sCutTreeHabitat) ? sCutTreeHabitat[slot].odds : 0;
}

u32 ChooseCutTreeSlotFromRoll(u32 roll)
{
    u32 slot;

    for (slot = 0; slot + 1 < ARRAY_COUNT(sCutTreeHabitat); slot++)
    {
        if (roll < sCutTreeHabitat[slot].odds)
            return slot;
        roll -= sCutTreeHabitat[slot].odds;
    }
    return slot;
}

void GetCutTreeEncounterLevelRange(u8 *minLevel, u8 *maxLevel)
{
    u32 cap = GetCurrentLevelCap();

    *minLevel = cap > CUT_TREE_LEVELS_BELOW_CAP_MAX ? cap - CUT_TREE_LEVELS_BELOW_CAP_MAX : 1;
    *maxLevel = cap > CUT_TREE_LEVELS_BELOW_CAP_MIN ? cap - CUT_TREE_LEVELS_BELOW_CAP_MIN : 1;
}

u8 GetCutTreeEncounterLevelFromRoll(u32 roll)
{
    u8 low, high;

    GetCutTreeEncounterLevelRange(&low, &high);
    return low + roll % (high - low + 1);
}

// Does this map have a Cut tree? The Pokedex area page marks the tree
// habitat wherever one grows.
static bool32 MapHeaderHasEncounterObject(const struct MapHeader *header, const u8 *script)
{
    const struct MapEvents *events = header->events;

    if (events == NULL)
        return FALSE;
    for (u32 i = 0; i < events->objectEventCount; i++)
    {
        if (events->objectEvents[i].script == script)
            return TRUE;
    }
    return FALSE;
}

bool32 MapHeaderHasCutTrees(const struct MapHeader *header)
{
    return MapHeaderHasEncounterObject(header, EventScript_CutTree);
}

bool32 MapHeaderHasRockSmash(const struct MapHeader *header)
{
    return MapHeaderHasEncounterObject(header, EventScript_RockSmash);
}

const struct WildPokemon gWildFeebas = {20, 25, SPECIES_FEEBAS};

static const u16 sRoute119WaterTileData[] =
{
//yMin, yMax, numSpots in previous sections
     0,  45,  0,
    46,  91,  NUM_FISHING_SPOTS_1,
    92, 139,  NUM_FISHING_SPOTS_1 + NUM_FISHING_SPOTS_2,
};

// Each fishing spot on Route 119 is given a number between 1 and NUM_FISHING_SPOTS inclusive.
// The number is determined by counting the valid fishing spots left to right top to bottom.
// The map is divided into three sections, with each section having a pre-counted number of
// fishing spots to start from to avoid counting a large number of spots at the bottom of the map.
// Note that a spot is considered valid if it is surfable and not a waterfall. To exclude all
// of the inaccessible water metatiles (so that they can't be selected as a Feebas spot) they
// use a different metatile that isn't actually surfable because it has MB_NORMAL instead.
// This function is given the coordinates and section of a fishing spot and returns which number it is.
static u16 GetFeebasFishingSpotId(s16 targetX, s16 targetY, u8 section)
{
    u16 x, y;
    u16 yMin = sRoute119WaterTileData[section * 3 + 0];
    u16 yMax = sRoute119WaterTileData[section * 3 + 1];
    u16 spotId = sRoute119WaterTileData[section * 3 + 2];

    for (y = yMin; y <= yMax; y++)
    {
        for (x = 0; x < gMapHeader.mapLayout->width; x++)
        {
            u8 behavior = MapGridGetMetatileBehaviorAt(x + MAP_OFFSET, y + MAP_OFFSET);
            if (MetatileBehavior_IsSurfableAndNotWaterfall(behavior) == TRUE)
            {
                spotId++;
                if (targetX == x && targetY == y)
                    return spotId;
            }
        }
    }
    return spotId + 1;
}

// A cast from one of Feebas's hidden spots hooks it this often, with any rod.
#define FEEBAS_SPOT_BITE_PERCENT 50

bool32 MapHasFeebasSpots(u8 mapGroup, u8 mapNum)
{
    return mapGroup == MAP_GROUP(MAP_ROUTE119) && mapNum == MAP_NUM(MAP_ROUTE119);
}

// Feebas keeps its own rarity: the chance that a cast from a random Route 119
// fishing spot hooks it (its hidden spots among every fishing spot, times its
// bite chance there), in hundredths of a percent.
u32 GetFeebasSpotShare(void)
{
    return NUM_FEEBAS_SPOTS * FEEBAS_SPOT_BITE_PERCENT * 100 / NUM_FISHING_SPOTS;
}

bool8 CheckFeebasAtCoords(s16 x, s16 y)
{
    u8 i;
    u16 feebasSpots[NUM_FEEBAS_SPOTS];
    u8 route119Section = 0;
    u16 spotId;

    if (MapHasFeebasSpots(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum))
    {
        x -= MAP_OFFSET;
        y -= MAP_OFFSET;

        // Get which third of the map the player is in
        if (y >= sRoute119WaterTileData[3 * 0 + 0] && y <= sRoute119WaterTileData[3 * 0 + 1])
            route119Section = 0;
        if (y >= sRoute119WaterTileData[3 * 1 + 0] && y <= sRoute119WaterTileData[3 * 1 + 1])
            route119Section = 1;
        if (y >= sRoute119WaterTileData[3 * 2 + 0] && y <= sRoute119WaterTileData[3 * 2 + 1])
            route119Section = 2;

        // Chance of encountering Feebas (assuming this is a Feebas spot)
        if (Random() % 100 >= FEEBAS_SPOT_BITE_PERCENT)
            return FALSE;

        FeebasSeedRng(gSaveBlock1Ptr->dewfordTrends[0].rand);

        // Assign each Feebas spot to a random fishing spot.
        // Randomness is fixed depending on the seed above.
        for (i = 0; i != NUM_FEEBAS_SPOTS;)
        {
            feebasSpots[i] = FeebasRandom() % NUM_FISHING_SPOTS;
            if (feebasSpots[i] == 0)
                feebasSpots[i] = NUM_FISHING_SPOTS;

            // < 1 below is a pointless check, it will never be TRUE.
            // >= 4 to skip fishing spots 1-3, because these are inaccessible
            // spots at the top of the map, at (9,7), (7,13), and (15,16).
            // The first accessible fishing spot is spot 4 at (18,18).
            if (feebasSpots[i] < 1 || feebasSpots[i] >= 4)
                i++;
        }

        // Check which fishing spot the player is at, and see if
        // it matches any of the Feebas spots.
        spotId = GetFeebasFishingSpotId(x, y, route119Section);
        for (i = 0; i < NUM_FEEBAS_SPOTS; i++)
        {
            if (spotId == feebasSpots[i])
                return TRUE;
        }
    }
    return FALSE;
}

static u16 FeebasRandom(void)
{
    sFeebasRngValue = ISO_RANDOMIZE2(sFeebasRngValue);
    return sFeebasRngValue >> 16;
}

static void FeebasSeedRng(u16 seed)
{
    sFeebasRngValue = seed;
}

static const u8 sLandEncounterBounds[] = {
    ENCOUNTER_CHANCE_LAND_MONS_SLOT_0, ENCOUNTER_CHANCE_LAND_MONS_SLOT_1,
    ENCOUNTER_CHANCE_LAND_MONS_SLOT_2, ENCOUNTER_CHANCE_LAND_MONS_SLOT_3,
    ENCOUNTER_CHANCE_LAND_MONS_SLOT_4, ENCOUNTER_CHANCE_LAND_MONS_SLOT_5,
    ENCOUNTER_CHANCE_LAND_MONS_SLOT_6, ENCOUNTER_CHANCE_LAND_MONS_SLOT_7,
    ENCOUNTER_CHANCE_LAND_MONS_SLOT_8, ENCOUNTER_CHANCE_LAND_MONS_SLOT_9,
    ENCOUNTER_CHANCE_LAND_MONS_SLOT_10, ENCOUNTER_CHANCE_LAND_MONS_SLOT_11,
};
static const u8 sWaterEncounterBounds[] = {
    ENCOUNTER_CHANCE_WATER_MONS_SLOT_0, ENCOUNTER_CHANCE_WATER_MONS_SLOT_1,
    ENCOUNTER_CHANCE_WATER_MONS_SLOT_2, ENCOUNTER_CHANCE_WATER_MONS_SLOT_3,
};

static const u8 sRockEncounterBounds[] = {
    ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_0, ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_1,
    ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_2, ENCOUNTER_CHANCE_ROCK_SMASH_MONS_SLOT_3,
};
static const u8 sHoneyEncounterBounds[] = {
    ENCOUNTER_CHANCE_HONEY_MONS_SLOT_0, ENCOUNTER_CHANCE_HONEY_MONS_SLOT_1,
    ENCOUNTER_CHANCE_HONEY_MONS_SLOT_2, ENCOUNTER_CHANCE_HONEY_MONS_SLOT_3,
    ENCOUNTER_CHANCE_HONEY_MONS_SLOT_4, ENCOUNTER_CHANCE_HONEY_MONS_SLOT_5,
};
// Each rod's cumulative odds restart at its first slot.
static const u8 sFishingEncounterBounds[] = {
    ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0, ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_1,
    ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2, ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3,
    ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4,
    ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5, ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6,
    ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7, ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8,
    ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9,
};
static const u8 sRodFirstSlot[] = {[OLD_ROD] = 0, [GOOD_ROD] = 2, [SUPER_ROD] = 5};
static const u8 sRodSlotCount[] = {[OLD_ROD] = 2, [GOOD_ROD] = 3, [SUPER_ROD] = 5};

static const u8 *GetEncounterBounds(const struct WildPokemonInfo *info, const u8 *defaults)
{
    return info != NULL && info->encounterBounds != NULL ? info->encounterBounds : defaults;
}

// A method's cumulative odds (the table's own, or the method defaults) and
// its slot count. Hidden (DexNav) tables have no draw odds.
static const u8 *GetWildAreaBounds(const struct WildPokemonInfo *info, enum WildPokemonArea area, u32 *count)
{
    switch (area)
    {
    case WILD_AREA_LAND:
        *count = ARRAY_COUNT(sLandEncounterBounds);
        return GetEncounterBounds(info, sLandEncounterBounds);
    case WILD_AREA_WATER:
        *count = ARRAY_COUNT(sWaterEncounterBounds);
        return GetEncounterBounds(info, sWaterEncounterBounds);
    case WILD_AREA_ROCKS:
        *count = ARRAY_COUNT(sRockEncounterBounds);
        return GetEncounterBounds(info, sRockEncounterBounds);
    case WILD_AREA_HONEY:
        *count = ARRAY_COUNT(sHoneyEncounterBounds);
        return GetEncounterBounds(info, sHoneyEncounterBounds);
    case WILD_AREA_FISHING:
        *count = ARRAY_COUNT(sFishingEncounterBounds);
        return GetEncounterBounds(info, sFishingEncounterBounds);
    default:
        *count = 0;
        return NULL;
    }
}

static u32 GetRodOfFishingSlot(u32 slot)
{
    return slot < sRodFirstSlot[GOOD_ROD] ? OLD_ROD : slot < sRodFirstSlot[SUPER_ROD] ? GOOD_ROD : SUPER_ROD;
}

// A slot that can take an inert slot's draw: live, and an ordinary resident
// when `ordinaryOnly`, so a Legendary, Ultra Beast or Paradox slot keeps its
// own 5% instead of growing with its gated neighbors' odds.
static bool32 CanTakeWildDraw(enum Species species, bool32 ordinaryOnly)
{
    return IsWildSlotLive(species) && (!ordinaryOnly || GetRestrictedPartyClass(species) == RESTRICTED_PARTY_NONE);
}

// Where a draw that lands on `slot` ends up. A slot whose species cannot be
// met right now (IsWildSlotLive: a gated or caught legend, a caught Paradox,
// or a weather visitor's own slot) is inert and hands its draw to the next
// live ordinary slot of the same table, or to the next live slot of any kind
// when no ordinary one is left; a rod looks through its own slots first, then
// the whole fishing table. WILD_SLOT_NONE: nothing in the table can be met. A
// rod always hooks something, so a fishing table of inert slots keeps the
// drawn one (fishing tables hold no legends: scripts/verify_wild_distribution.py).
// The engine's slot choice and the wild roster both go through here.
u32 GetLiveWildSlot(const struct WildPokemonInfo *info, enum WildPokemonArea area, u32 slot)
{
    const struct WildPokemon *mons = info->wildPokemon;
    u32 count;

    if (IsWildSlotLive(mons[slot].species))
        return slot;
    count = area == WILD_AREA_LAND ? NUM_LAND_MONS_ENCOUNTER_SLOTS
          : area == WILD_AREA_WATER ? NUM_WATER_MONS_ENCOUNTER_SLOTS
          : area == WILD_AREA_ROCKS ? NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS
          : area == WILD_AREA_HONEY ? NUM_HONEY_MONS_ENCOUNTER_SLOTS
          : area == WILD_AREA_FISHING ? NUM_FISHING_MONS_ENCOUNTER_SLOTS : 1;
    for (u32 ordinaryOnly = TRUE; ; ordinaryOnly = FALSE)
    {
        if (area == WILD_AREA_FISHING)
        {
            u32 rod = GetRodOfFishingSlot(slot);
            u32 first = sRodFirstSlot[rod];

            for (u32 i = 1; i < sRodSlotCount[rod]; i++)
            {
                u32 candidate = first + (slot - first + i) % sRodSlotCount[rod];
                if (CanTakeWildDraw(mons[candidate].species, ordinaryOnly))
                    return candidate;
            }
        }
        for (u32 i = 1; i < count; i++)
        {
            u32 candidate = (slot + i) % count;
            if (CanTakeWildDraw(mons[candidate].species, ordinaryOnly))
                return candidate;
        }
        if (!ordinaryOnly)
            break;
    }
    return area == WILD_AREA_FISHING ? slot : WILD_SLOT_NONE;
}

static u32 ChooseEncounterSlotFromRoll(const u8 *bounds, u32 count, u32 roll)
{
    for (u32 slot = 0; slot + 1 < count; slot++)
        if (roll < bounds[slot])
            return slot;
    return count - 1;
}

static u32 ChooseEncounterSlot(const u8 *bounds, u32 count)
{
    return ChooseEncounterSlotFromRoll(bounds, count, Random() % bounds[count - 1]);
}

static u32 ChooseEncounterSlotWithLure(const u8 *bounds, u32 count)
{
    u32 slot = ChooseEncounterSlot(bounds, count);
    // Keep the optional Lure draw after the ordinary slot draw.
    if (LURE_STEP_COUNT != 0 && Random() % 10 < 2)
        slot = count - 1 - slot;
    return slot;
}

// Every slot is met at its table odds: Legendary, Ultra Beast and Paradox slots
// are authored at 5%, so no resident needs extra chances to be found.
u32 ChooseWildMonIndex_Land(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlotWithLure(GetEncounterBounds(info, sLandEncounterBounds), ARRAY_COUNT(sLandEncounterBounds));
}

// Sweet Scent is a lure, never a guarantee. Each live Legendary, Ultra Beast
// or Paradox slot becomes three times as likely as its table odds (5% -> 15%),
// and together they take at most 30% of outcomes, however many a table holds.
// Gated or caught slots are inert and contribute nothing. The remaining mass
// reverses ordinary species probabilities, not slot positions: duplicate slots
// are combined first and tied species share their reversed probability equally.
#define SWEET_SCENT_RESTRICTED_MULTIPLIER  3
#define SWEET_SCENT_RESTRICTED_CAP_PERCENT 30

static bool32 IsSweetScentLegendSlot(enum Species species)
{
    return GetRestrictedPartyClass(species) != RESTRICTED_PARTY_NONE;
}

u32 ChooseSweetScentWildMonIndex(const struct WildPokemonInfo *info, enum WildPokemonArea area)
{
    const struct WildPokemon *mons = info->wildPokemon;
    struct ScentSpecies { enum Species species; u32 weight; } entries[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    const u8 *bounds = GetEncounterBounds(info, area == WILD_AREA_WATER ? sWaterEncounterBounds : sLandEncounterBounds);
    u32 slots = area == WILD_AREA_WATER ? ARRAY_COUNT(sWaterEncounterBounds) : ARRAY_COUNT(sLandEncounterBounds);
    u32 total = bounds[slots - 1];
    u32 count = 0, ordinaryTotal = 0, legendTotal = 0, legendMass;
    u32 roll = RandomUniform(RNG_NONE, 0, total - 1);
    u32 ordinaryRoll;

    for (u32 i = 0; i < slots; i++)
    {
        u32 weight = bounds[i] - (i == 0 ? 0 : bounds[i - 1]);
        if (IsSweetScentLegendSlot(mons[i].species))
        {
            if (IsWildSlotLive(mons[i].species))
                legendTotal += weight * SWEET_SCENT_RESTRICTED_MULTIPLIER;
            continue;
        }
        u32 j;
        for (j = 0; j < count; j++)
            if (entries[j].species == mons[i].species)
                break;
        if (j == count)
            entries[count++] = (struct ScentSpecies){mons[i].species, 0};
        entries[j].weight += weight;
        ordinaryTotal += weight;
    }

    legendMass = min(legendTotal, total * SWEET_SCENT_RESTRICTED_CAP_PERCENT / 100);
    // A table with no ordinary residents gives every outcome to its legends.
    if (count == 0 && legendTotal != 0)
        legendMass = total;
    if (roll < legendMass)
    {
        // Spread the boosted share over eligible legends by their table odds.
        u32 target = roll * legendTotal / legendMass;
        for (u32 i = 0; i < slots; i++)
        {
            u32 weight = bounds[i] - (i == 0 ? 0 : bounds[i - 1]);
            if (!IsSweetScentLegendSlot(mons[i].species) || !IsWildSlotLive(mons[i].species))
                continue;
            weight *= SWEET_SCENT_RESTRICTED_MULTIPLIER;
            if (target < weight)
                return i;
            target -= weight;
        }
    }
    // Only inert legends: let the acquisition walk in TryGenerateWildMon decide.
    if (count == 0)
        return ChooseEncounterSlotFromRoll(bounds, slots, roll);
    ordinaryRoll = (roll - legendMass) * ordinaryTotal / (total - legendMass);

    for (u32 i = 1; i < count; i++)
    {
        struct ScentSpecies entry = entries[i];
        u32 j = i;
        while (j != 0 && entries[j - 1].weight < entry.weight)
        {
            entries[j] = entries[j - 1];
            j--;
        }
        entries[j] = entry;
    }
    u32 selected = 0;
    while (ordinaryRoll >= entries[count - 1 - selected].weight)
    {
        ordinaryRoll -= entries[count - 1 - selected].weight;
        selected++;
    }
    u32 first = selected, last = selected;
    while (first != 0 && entries[first - 1].weight == entries[selected].weight)
        first--;
    while (last + 1 < count && entries[last + 1].weight == entries[selected].weight)
        last++;
    if (first != last)
        selected = RandomUniform(RNG_WILD_MON_TARGET, first, last);

    // Preserve the chosen species' original distribution of encounter levels.
    roll = RandomUniform(RNG_WILD_MON_TARGET, 0, entries[selected].weight - 1);
    for (u32 i = 0; i < slots; i++)
    {
        if (mons[i].species != entries[selected].species)
            continue;
        u32 weight = bounds[i] - (i == 0 ? 0 : bounds[i - 1]);
        if (roll < weight)
            return i;
        roll -= weight;
    }
    return ChooseEncounterSlotFromRoll(bounds, slots, roll);
}

// The authored odds of one slot, in percent of its method (or rod) total.
u32 GetWildSlotOdds(const struct WildPokemonInfo *info, enum WildPokemonArea area, u32 slot)
{
    u32 count;
    const u8 *bounds = GetWildAreaBounds(info, area, &count);
    bool32 segmentStart = slot == 0
        || (area == WILD_AREA_FISHING && sRodFirstSlot[GetRodOfFishingSlot(slot)] == slot);

    if (bounds == NULL || slot >= count)
        return 0;
    return bounds[slot] - (segmentStart ? 0 : bounds[slot - 1]);
}

// The one share function: each slot's real share of its method's draws, in
// hundredths of a percent (WILD_SLOT_SHARE_TOTAL), written to shares[] for
// every slot of the table. A slot's share is its authored odds plus the draws
// inert slots hand it (GetLiveWildSlot). For fishing, `rod` says whose draws
// are shared out. Returns the table's slot count. The shares total
// WILD_SLOT_SHARE_TOTAL (to rounding), or 0 when nothing can be met.
// Lead Abilities, Lures and Sweet Scent bend a single draw and are not here.
u32 GetWildSlotShares(const struct WildPokemonInfo *info, enum WildPokemonArea area, u32 rod, u16 *shares)
{
    u32 count, first = 0, drawn, total;
    const u8 *bounds = GetWildAreaBounds(info, area, &count);

    if (bounds == NULL)
        return 0;
    for (u32 i = 0; i < count; i++)
        shares[i] = 0;
    drawn = count;
    if (area == WILD_AREA_FISHING)
    {
        if (rod > SUPER_ROD)
            return count;
        first = sRodFirstSlot[rod];
        drawn = sRodSlotCount[rod];
    }
    total = bounds[first + drawn - 1];
    if (total == 0)
        return count;
    for (u32 i = first; i < first + drawn; i++)
    {
        u32 weight = bounds[i] - (i == first ? 0 : bounds[i - 1]);
        u32 target = GetLiveWildSlot(info, area, i);

        if (target != WILD_SLOT_NONE)
            shares[target] += weight * WILD_SLOT_SHARE_TOTAL / total;
    }
    return count;
}

// Mostly equivalent to ChooseWildMonIndex_Land
// NUM_LAND_MONS_ENCOUNTER_SLOTS
u8 GetLandEncounterSlotForMatchCall(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlot(GetEncounterBounds(info, sLandEncounterBounds), ARRAY_COUNT(sLandEncounterBounds));
}

u32 ChooseWildMonIndex_Water(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlotWithLure(GetEncounterBounds(info, sWaterEncounterBounds), ARRAY_COUNT(sWaterEncounterBounds));
}

// Match Call uses ordinary odds without the optional Lure reversal.
u8 GetWaterEncounterSlotForMatchCall(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlot(GetEncounterBounds(info, sWaterEncounterBounds), ARRAY_COUNT(sWaterEncounterBounds));
}


u32 ChooseWildMonIndex_Rocks(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlotWithLure(GetEncounterBounds(info, sRockEncounterBounds), ARRAY_COUNT(sRockEncounterBounds));
}

static u32 ChooseWildMonIndex_Honey(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlot(GetEncounterBounds(info, sHoneyEncounterBounds), ARRAY_COUNT(sHoneyEncounterBounds));
}

u32 ChooseWildMonIndex_Fishing(const struct WildPokemonInfo *info, u8 rod)
{
    if (rod > SUPER_ROD)
        return 0;
    return sRodFirstSlot[rod] + ChooseEncounterSlotWithLure(GetEncounterBounds(info, sFishingEncounterBounds) + sRodFirstSlot[rod], sRodSlotCount[rod]);
}

// Emerald Champions: each area's table sets its levels, as in Vanilla
// (scripts/assign_wild_levels.py writes them in route order). Two guards stay:
// no evolved Pokemon is met below the level it evolves at, and nothing is met
// above the live cap.
#include "data/wild_evolution_floors.h"

static u8 GetWildEvolutionFloor(enum Species species)
{
    for (u32 i = 0; i < ARRAY_COUNT(sWildEvolutionFloors); i++)
        if (sWildEvolutionFloors[i].species == species)
            return sWildEvolutionFloors[i].level;
    return 1;
}

// The levels a slot's Pokémon can arrive at right now, from the same rules
// as ChooseWildMonLevel and TryGenerateWildMon: Legendary-class and Ultra
// Beast slots come at their cap, others at their table levels after the
// evolution floor and the cap. A Lure's +1 is not included.
void GetWildSlotLevelRange(const struct WildPokemon *mon, u8 *minLevel, u8 *maxLevel)
{
    u32 low, high, evolution, ceiling;

    if (IsLegendaryEncounterSpecies(mon->species))
    {
        *minLevel = *maxLevel = GetLegendaryEncounterLevel(mon->species);
        return;
    }
    low = min(mon->minLevel, mon->maxLevel);
    high = max(mon->minLevel, mon->maxLevel);
    evolution = GetWildEvolutionFloor(mon->species);
    ceiling = min(GetCurrentLevelCap(), MAX_LEVEL);
    *minLevel = min(max(low, evolution), ceiling);
    *maxLevel = min(max(high, evolution), ceiling);
}

u8 ApplyWildLevelFloor(enum Species species, u8 level)
{
    level = max(level, GetWildEvolutionFloor(species));
    return min(level, min(GetCurrentLevelCap(), MAX_LEVEL));
}

static u8 ChooseTableWildMonLevel(const struct WildPokemon *wildPokemon, u8 wildMonIndex, enum WildPokemonArea area);

u8 ChooseWildMonLevel(const struct WildPokemon *wildPokemon, u8 wildMonIndex, enum WildPokemonArea area)
{
    return ApplyWildLevelFloor(wildPokemon[wildMonIndex].species, ChooseTableWildMonLevel(wildPokemon, wildMonIndex, area));
}

static u8 ChooseTableWildMonLevel(const struct WildPokemon *wildPokemon, u8 wildMonIndex, enum WildPokemonArea area)
{
    u8 min;
    u8 max;
    u8 range;
    u8 rand;

    if (LURE_STEP_COUNT == 0)
    {
        // Make sure minimum level is less than maximum level
        if (wildPokemon[wildMonIndex].maxLevel >= wildPokemon[wildMonIndex].minLevel)
        {
            min = wildPokemon[wildMonIndex].minLevel;
            max = wildPokemon[wildMonIndex].maxLevel;
        }
        else
        {
            min = wildPokemon[wildMonIndex].maxLevel;
            max = wildPokemon[wildMonIndex].minLevel;
        }
        range = max - min + 1;
        rand = Random() % range;

        // check ability for max level mon
        if (!GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_EGG))
        {
            enum Ability ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);
            if (ability == ABILITY_HUSTLE || ability == ABILITY_VITAL_SPIRIT || ability == ABILITY_PRESSURE)
            {
                if (Random() % 2 == 0)
                    return max;

                if (rand != 0)
                    rand--;
            }
        }
        return min + rand;
    }
    else
    {
        // Looks for the max level of all slots that share the same species as the selected slot.
        max = GetMaxLevelOfSpeciesInWildTable(wildPokemon, wildPokemon[wildMonIndex].species, area);
        if (max > 0)
            return max + 1;
        else // Failsafe
            return wildPokemon[wildMonIndex].maxLevel + 1;
    }
}

u16 GetCurrentMapWildMonHeaderId(void)
{
    return GetWildMonHeaderIdForMap(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
}

u16 GetWildMonHeaderIdForMap(u8 mapGroup, u8 mapNum)
{
    u16 i;

    for (i = 0; ; i++)
    {
        const struct WildPokemonHeader *wildHeader = &gWildMonHeaders[i];
        if (wildHeader->mapGroup == MAP_GROUP(MAP_UNDEFINED))
            break;

        if (gWildMonHeaders[i].mapGroup == mapGroup && gWildMonHeaders[i].mapNum == mapNum)
        {
            if (mapGroup == MAP_GROUP(MAP_ALTERING_CAVE) && mapNum == MAP_NUM(MAP_ALTERING_CAVE))
            {
                u16 alteringCaveId = VarGet(VAR_ALTERING_CAVE_WILD_SET);
                if (alteringCaveId >= NUM_ALTERING_CAVE_TABLES)
                    alteringCaveId = 0;

                i += alteringCaveId;
            }

            return i;
        }
    }

    return HEADER_NONE;
}

enum TimeOfDay GetTimeOfDayForEncounters(u32 headerId, enum WildPokemonArea area)
{
    const struct WildPokemonInfo *wildMonInfo;
    enum TimeOfDay timeOfDay = GetTimeOfDay();

    if (!OW_TIME_OF_DAY_ENCOUNTERS)
        return TIME_OF_DAY_DEFAULT;

    if (InBattlePike() || CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        return OW_TIME_OF_DAY_FALLBACK;

    switch (area)
    {
    default:
    case WILD_AREA_LAND:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo;
        break;
    case WILD_AREA_WATER:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;
        break;
    case WILD_AREA_ROCKS:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].rockSmashMonsInfo;
        break;
    case WILD_AREA_FISHING:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].fishingMonsInfo;
        break;
    case WILD_AREA_HIDDEN:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].hiddenMonsInfo;
        break;
    case WILD_AREA_HONEY:
        wildMonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].honeyMonsInfo;
        break;
    }

    if (wildMonInfo == NULL && !OW_TIME_OF_DAY_DISABLE_FALLBACK)
        return OW_TIME_OF_DAY_FALLBACK;
    else
        return GenConfigTimeOfDay(timeOfDay);
}

static u8 PickWildMonNature(enum Species species)
{
    u8 i;
    struct Pokeblock *safariPokeblock;
    u8 natures[NUM_NATURES];

    if (GetSafariZoneFlag() == TRUE && Random() % 100 < 80)
    {
        safariPokeblock = SafariZoneGetActivePokeblock();
        if (safariPokeblock != NULL)
        {
            for (i = 0; i < NUM_NATURES; i++)
                natures[i] = i;
            Shuffle(natures, NUM_NATURES, sizeof(natures[0]));
            for (i = 0; i < NUM_NATURES; i++)
            {
                if (PokeblockGetGain(natures[i], safariPokeblock) > 0)
                    return natures[i];
            }
        }
    }

    return GetSynchronizedNature(WILDMON_ORIGIN, species);
}

void CreateWildMon(enum Species species, u8 level)
{
    // All wild creation paths share the campaign ceiling, including DexNav,
    // outbreaks and Feebas paths that bypass ordinary slot generation.
    level = min(level, GetCurrentLevelCap());
    ZeroEnemyPartyMons();
    u32 personality = GetMonPersonality(species, GetSynchronizedGender(WILDMON_ORIGIN, species), PickWildMonNature(species), RANDOM_UNOWN_LETTER);
    CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][0], species, level, personality, OTID_STRUCT_PLAYER_ID, USE_RANDOM_IVS);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);
    if (B_EC_WILD_BATTLE_SETS && !InBattlePike()
     && CurrentBattlePyramidLocation() == PYRAMID_LOCATION_NONE
     && IsEmeraldChampionsOrdinaryWildSpecies(species))
        ApplyEmeraldChampionsRandomWildSet(&gParties[B_TRAINER_OPPONENT_A][0]);

    // The manor's singers are the nearby solution to its meadow quest.
    // Apply this after the random battle set so every local Jigglypuff keeps
    // Sing, without changing tutor sets or Jigglypuff from other habitats.
    if (species == SPECIES_JIGGLYPUFF && gMapHeader.mapLayoutId == LAYOUT_DEWFORD_MANOR_1F)
    {
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            if (GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MOVE1 + slot) == MOVE_SING)
                return;
        SetMonMoveSlot(&gParties[B_TRAINER_OPPONENT_A][0], MOVE_SING, MAX_MON_MOVES - 1);
    }
}

// How a wild table slot's Pokémon is made, whatever found it (a step in the
// grass, a rod, a storm, a DexNav search): Legendary-class and Ultra Beast
// Pokémon at the cap with their authored or competitive set, the rest at the
// level given.
void CreateWildSlotMon(enum Species species, u8 level)
{
    bool32 legendary = IsLegendaryEncounterSpecies(species);

    CreateWildMon(species, legendary ? GetLegendaryEncounterLevel(species) : level);
    if (legendary)
        ApplyLegendaryEncounterSet(&gParties[B_TRAINER_OPPONENT_A][0], ITEM_NONE);
}

#ifdef BUGFIX
#define TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildPokemon, type, ability, ptr, count) TryGetAbilityInfluencedWildMonIndex(wildPokemon, type, ability, ptr, count)
#else
#define TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildPokemon, type, ability, ptr, count) TryGetAbilityInfluencedWildMonIndex(wildPokemon, type, ability, ptr)
#endif

bool8 TryGenerateWildMon(const struct WildPokemonInfo *wildMonInfo, enum WildPokemonArea area, u8 flags)
{
    u8 wildMonIndex = 0;
    u32 liveIndex;
    u8 level;
    enum Species species;
    bool32 legendary;

    // A live weather anomaly on this map takes a flat share of land or Surf
    // encounters before any slot draw (Sweet Scent and abilities included).
    // The second mon of a double battle never repeats the visitor.
    species = sGeneratingSecondWildMon ? SPECIES_NONE : TryRollWeatherAnomalyEncounter(area);
    if (species != SPECIES_NONE)
        goto CREATE;

    if (sSweetScentActive && (area == WILD_AREA_LAND || area == WILD_AREA_WATER))
        wildMonIndex = ChooseSweetScentWildMonIndex(wildMonInfo, area);
    else
    switch (area)
    {
    case WILD_AREA_LAND:
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_STEEL, ABILITY_MAGNET_PULL, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_STATIC, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_LIGHTNING_ROD >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_LIGHTNING_ROD, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_FLASH_FIRE >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_FIRE, ABILITY_FLASH_FIRE, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_HARVEST >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_GRASS, ABILITY_HARVEST, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_STORM_DRAIN >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_WATER, ABILITY_STORM_DRAIN, &wildMonIndex, NUM_LAND_MONS_ENCOUNTER_SLOTS))
            break;

        wildMonIndex = ChooseWildMonIndex_Land(wildMonInfo);
        break;
    case WILD_AREA_WATER:
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_STEEL, ABILITY_MAGNET_PULL, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_STATIC, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_LIGHTNING_ROD >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_ELECTRIC, ABILITY_LIGHTNING_ROD, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_FLASH_FIRE >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_FIRE, ABILITY_FLASH_FIRE, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_HARVEST >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_GRASS, ABILITY_HARVEST, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;
        if (OW_STORM_DRAIN >= GEN_8 && TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildMonInfo->wildPokemon, TYPE_WATER, ABILITY_STORM_DRAIN, &wildMonIndex, NUM_WATER_MONS_ENCOUNTER_SLOTS))
            break;

        wildMonIndex = ChooseWildMonIndex_Water(wildMonInfo);
        break;
    case WILD_AREA_ROCKS:
        wildMonIndex = ChooseWildMonIndex_Rocks(wildMonInfo);
        break;
    case WILD_AREA_HONEY:
        wildMonIndex = ChooseWildMonIndex_Honey(wildMonInfo);
        break;
    default:
    case WILD_AREA_FISHING:
    case WILD_AREA_HIDDEN:
        break;
    }

    // An inert slot (gated, caught or storm-held) hands its draw on.
    liveIndex = GetLiveWildSlot(wildMonInfo, area, wildMonIndex);
    if (liveIndex == WILD_SLOT_NONE)
        return FALSE;
    wildMonIndex = liveIndex;
    species = wildMonInfo->wildPokemon[wildMonIndex].species;

CREATE:
    legendary = IsLegendaryEncounterSpecies(species);
    // Emerald Champions: nothing in the wild is ever above the live level cap.
    // Table levels describe the route; an early Old Rod cannot pull a Lv 45
    // Qwilfish out of Route 103 when the cap is 14. Legendary and Ultra Beast
    // slots always meet the player at the cap.
    if (legendary)
        level = GetLegendaryEncounterLevel(species);
    else
        level = min(ChooseWildMonLevel(wildMonInfo->wildPokemon, wildMonIndex, area), GetCurrentLevelCap());
    if (flags & WILD_CHECK_REPEL && !IsWildLevelAllowedByRepel(level))
        return FALSE;
    if (gMapHeader.mapLayoutId != LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS && flags & WILD_CHECK_KEEN_EYE && !IsAbilityAllowingEncounter(level))
        return FALSE;

    CreateWildSlotMon(species, level);
    return TRUE;
}

// Rods follow the same slot rule as every other method (GetLiveWildSlot): an
// inert slot passes to the next live slot of the same rod, then of the whole
// fishing table.
static u16 GenerateFishingWildMon(const struct WildPokemonInfo *wildMonInfo, u8 rod)
{
    u8 wildMonIndex = GetLiveWildSlot(wildMonInfo, WILD_AREA_FISHING, ChooseWildMonIndex_Fishing(wildMonInfo, rod));
    enum Species wildMonSpecies = wildMonInfo->wildPokemon[wildMonIndex].species;
    u8 level;

    if (IsLegendaryEncounterSpecies(wildMonSpecies))
        level = GetLegendaryEncounterLevel(wildMonSpecies);
    else
        level = ChooseWildMonLevel(wildMonInfo->wildPokemon, wildMonIndex, WILD_AREA_FISHING);

    UpdateChainFishingStreak();
    CreateWildSlotMon(wildMonSpecies, level);
    return wildMonSpecies;
}

static bool8 EncounterOddsCheck(u16 encounterRate)
{
    if (Random() % MAX_ENCOUNTER_RATE < encounterRate)
        return TRUE;
    else
        return FALSE;
}

// Returns true if it will try to create a wild encounter.
static bool8 WildEncounterCheck(u32 encounterRate, bool8 ignoreAbility)
{
    encounterRate *= 16;
    if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_MACH_BIKE | PLAYER_AVATAR_FLAG_ACRO_BIKE))
        encounterRate = encounterRate * 80 / 100;
    ApplyCleanseTagEncounterRateMod(&encounterRate);
    if (LURE_STEP_COUNT != 0)
        encounterRate *= 2;
    if (!ignoreAbility && !GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_EGG))
    {
        enum Ability ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);

        if (ability == ABILITY_STENCH && gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR)
            encounterRate = encounterRate * 3 / 4;
        else if (ability == ABILITY_STENCH)
            encounterRate /= 2;
        else if (ability == ABILITY_ILLUMINATE)
            encounterRate *= 2;
        else if (ability == ABILITY_WHITE_SMOKE)
            encounterRate /= 2;
        else if (ability == ABILITY_ARENA_TRAP)
            encounterRate *= 2;
        else if (ability == ABILITY_SAND_VEIL && gSaveBlock1Ptr->weather == WEATHER_SANDSTORM)
            encounterRate /= 2;
        else if (ability == ABILITY_SNOW_CLOAK && gSaveBlock1Ptr->weather == WEATHER_SNOW)
            encounterRate /= 2;
        else if (ability == ABILITY_QUICK_FEET)
            encounterRate /= 2;
        else if (ability == ABILITY_INFILTRATOR && OW_INFILTRATOR >= GEN_8)
            encounterRate /= 2;
        else if (ability == ABILITY_NO_GUARD)
            encounterRate *= 2;
    }
    if (encounterRate > MAX_ENCOUNTER_RATE)
        encounterRate = MAX_ENCOUNTER_RATE;
    return EncounterOddsCheck(encounterRate);
}

// When you first step on a different type of metatile, there's a 40% chance it
// skips the wild encounter check entirely.
static bool8 AllowWildCheckOnNewMetatile(void)
{
    if (Random() % 100 >= 60)
        return FALSE;
    else
        return TRUE;
}

bool8 AreLegendariesInSootopolisPreventingEncounters(void)
{
    return AreSurfEncountersBlockedOnMap(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
}

// Sootopolis's water stays empty while Groudon and Kyogre clash there.
bool32 AreSurfEncountersBlockedOnMap(u8 mapGroup, u8 mapNum)
{
    return mapGroup == MAP_GROUP(MAP_SOOTOPOLIS_CITY) && mapNum == MAP_NUM(MAP_SOOTOPOLIS_CITY)
        && FlagGet(FLAG_LEGENDARIES_IN_SOOTOPOLIS);
}

// Imported underwater areas may author a full twelve-slot seabed roster.
// Keep its land-slot distribution instead of treating it as a five-slot Surf table.
static bool32 UsesLandEncounterTable(u32 headerId, u16 behavior)
{
    if (MetatileBehavior_IsLandWildEncounter(behavior))
        return TRUE;
    return gMapHeader.mapType == MAP_TYPE_UNDERWATER
        && MetatileBehavior_IsWaterWildEncounter(behavior)
        && gWildMonHeaders[headerId].encounterTypes[GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND)].landMonsInfo != NULL;
}

static bool32 TryGenerateSecondWildMon(const struct WildPokemonInfo *info, enum WildPokemonArea area, u8 flags)
{
    struct Pokemon first = gParties[B_TRAINER_OPPONENT_A][0];
    bool32 generated;

    sGeneratingSecondWildMon = TRUE;
    generated = TryGenerateWildMon(info, area, flags);
    sGeneratingSecondWildMon = FALSE;
    if (!generated)
        return FALSE;
    gParties[B_TRAINER_OPPONENT_A][1] = first;
    return TRUE;
}

#ifdef TESTING
bool32 Test_TryGenerateSecondWildMon(const struct WildPokemonInfo *info, enum WildPokemonArea area, u8 flags)
{
    return TryGenerateSecondWildMon(info, area, flags);
}

u16 Test_GenerateFishingWildMon(const struct WildPokemonInfo *info, u8 rod)
{
    return GenerateFishingWildMon(info, rod);
}
#endif

static void StartGeneratedWildBattle(const struct WildPokemonInfo *info, enum WildPokemonArea area, u8 flags)
{
    if (TryDoDoubleWildBattle() && TryGenerateSecondWildMon(info, area, flags))
        BattleSetup_StartDoubleWildBattle();
    else
        BattleSetup_StartWildBattle();
}

bool8 StandardWildEncounter(u16 curMetatileBehavior, u16 prevMetatileBehavior)
{
    u32 headerId;
    enum TimeOfDay timeOfDay;
    struct Roamer *roamer;

    if (sWildEncountersDisabled == TRUE)
        return FALSE;

    // Emerald Champions: while the Repel Spray is active (EC_REPEL_SPRAY_STEPS per
    // use, see UpdateRepelCounter) no step-based encounter can start. Fishing,
    // Rock Smash and Sweet Scent are deliberate actions and are unaffected. The
    // spray never counts down in the Battle Pike or Pyramid, so it does not apply there.
    if (FlagGet(FLAG_EC_REPEL_SPRAY_ACTIVE)
     && !InBattlePike() && CurrentBattlePyramidLocation() == PYRAMID_LOCATION_NONE)
        return FALSE;

    headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
    {
        if (gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS)
        {
            headerId = GetBattlePikeWildMonHeaderId();
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (prevMetatileBehavior != curMetatileBehavior && !AllowWildCheckOnNewMetatile())
                return FALSE;
            else if (WildEncounterCheck(gBattlePikeWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo->encounterRate, FALSE) != TRUE)
                return FALSE;
            else if (TryGenerateWildMon(gBattlePikeWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, WILD_CHECK_KEEN_EYE) != TRUE)
                return FALSE;
            else if (!TryGenerateBattlePikeWildMon(TRUE))
                return FALSE;

            BattleSetup_StartBattlePikeWildBattle();
            return TRUE;
        }
        if (gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR)
        {
            headerId = gSaveBlock2Ptr->frontier.curChallengeBattleNum;
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (prevMetatileBehavior != curMetatileBehavior && !AllowWildCheckOnNewMetatile())
                return FALSE;
            else if (WildEncounterCheck(gBattlePyramidWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo->encounterRate, FALSE) != TRUE)
                return FALSE;
            else if (TryGenerateWildMon(gBattlePyramidWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, WILD_CHECK_KEEN_EYE) != TRUE)
                return FALSE;

            GenerateBattlePyramidWildMon(SPECIES_NONE);
            BattleSetup_StartWildBattle();
            return TRUE;
        }
    }
    else
    {
        if (UsesLandEncounterTable(headerId, curMetatileBehavior))
        {
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo == NULL)
                return FALSE;
            else if (prevMetatileBehavior != curMetatileBehavior && !AllowWildCheckOnNewMetatile())
                return FALSE;
            else if (WildEncounterCheck(gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo->encounterRate, FALSE) != TRUE)
                return FALSE;

            if (TryStartRoamerEncounter())
            {
                roamer = &gSaveBlock1Ptr->roamer[gEncounteredRoamerIndex];
                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;

                BattleSetup_StartRoamerBattle();
                return TRUE;
            }
            else
            {
                if (DoMassOutbreakEncounterTest() == TRUE && SetUpMassOutbreakEncounter(WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE) == TRUE)
                {
                    BattleSetup_StartWildBattle();
                    return TRUE;
                }

                // try a regular wild land encounter
                if (TryGenerateWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE) == TRUE)
                {
                    StartGeneratedWildBattle(gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, WILD_CHECK_KEEN_EYE);
                    return TRUE;
                }

                return FALSE;
            }
        }
        else if (MetatileBehavior_IsWaterWildEncounter(curMetatileBehavior) == TRUE
                 || (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_SURFING) && MetatileBehavior_IsBridgeOverWater(curMetatileBehavior) == TRUE))
        {
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);

            if (AreLegendariesInSootopolisPreventingEncounters() == TRUE)
                return FALSE;
            else if (gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo == NULL)
                return FALSE;
            else if (prevMetatileBehavior != curMetatileBehavior && !AllowWildCheckOnNewMetatile())
                return FALSE;
            else if (WildEncounterCheck(gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo->encounterRate, FALSE) != TRUE)
                return FALSE;

            if (TryStartRoamerEncounter())
            {
                roamer = &gSaveBlock1Ptr->roamer[gEncounteredRoamerIndex];
                if (!IsWildLevelAllowedByRepel(roamer->level))
                    return FALSE;

                BattleSetup_StartRoamerBattle();
                return TRUE;
            }
            else // try a regular surfing encounter
            {
                if (TryGenerateWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo, WILD_AREA_WATER, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE) == TRUE)
                {
                    gIsSurfingEncounter = TRUE;
                    StartGeneratedWildBattle(gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo, WILD_AREA_WATER, WILD_CHECK_KEEN_EYE);
                    return TRUE;
                }

                return FALSE;
            }
        }
    }

    return FALSE;
}

void RockSmashWildEncounter(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    enum TimeOfDay timeOfDay;

    if (headerId != HEADER_NONE)
    {
        timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_ROCKS);

        const struct WildPokemonInfo *wildPokemonInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].rockSmashMonsInfo;

        if (wildPokemonInfo == NULL)
        {
            gSpecialVar_Result = FALSE;
        }
        else if (WildEncounterCheck(wildPokemonInfo->encounterRate, TRUE) == TRUE
         && TryGenerateWildMon(wildPokemonInfo, WILD_AREA_ROCKS, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE) == TRUE)
        {
            StartGeneratedWildBattle(wildPokemonInfo, WILD_AREA_ROCKS, WILD_CHECK_REPEL | WILD_CHECK_KEEN_EYE);
            gSpecialVar_Result = TRUE;
        }
        else
        {
            gSpecialVar_Result = FALSE;
        }
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

// A felled Cut tree (data/scripts/field_move_scripts.inc) may hide one of the
// tree habitat's species. VAR_RESULT is TRUE when a battle starts; the
// script then waits for it with waitstate.
void CutTreeWildEncounter(void)
{
    enum Species species;
    u8 level;

    gSpecialVar_Result = FALSE;
    if (GetCurrentMapWildMonHeaderId() == HEADER_NONE)
        return;
    if (Random() % CUT_TREE_ENCOUNTER_ODDS != 0)
        return;
    species = sCutTreeHabitat[ChooseCutTreeSlotFromRoll(Random() % 100)].species;
    level = GetCutTreeEncounterLevelFromRoll(Random());
    if (!IsWildLevelAllowedByRepel(level) || !IsAbilityAllowingEncounter(level))
        return;
    CreateWildMon(species, level);
    BattleSetup_StartWildBattle();
    gSpecialVar_Result = TRUE;
}

static bool8 SweetScentWildEncounterInner(void)
{
    s16 x, y;
    u32 headerId;
    enum TimeOfDay timeOfDay;

    PlayerGetDestCoords(&x, &y);
    headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
    {
        if (gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS)
        {
            headerId = GetBattlePikeWildMonHeaderId();
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (TryGenerateWildMon(gBattlePikeWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, 0) != TRUE)
                return FALSE;

            TryGenerateBattlePikeWildMon(FALSE);
            BattleSetup_StartBattlePikeWildBattle();
            return TRUE;
        }
        if (gMapHeader.mapLayoutId == LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR)
        {
            headerId = gSaveBlock2Ptr->frontier.curChallengeBattleNum;
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (TryGenerateWildMon(gBattlePyramidWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, 0) != TRUE)
                return FALSE;

            GenerateBattlePyramidWildMon(SPECIES_NONE);
            BattleSetup_StartWildBattle();
            return TRUE;
        }
    }
    else
    {
        if (UsesLandEncounterTable(headerId, MapGridGetMetatileBehaviorAt(x, y)))
        {
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);

            if (gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo == NULL)
                return FALSE;

            if (TryGenerateWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo, WILD_AREA_LAND, 0) != TRUE)
                return FALSE;

            BattleSetup_StartWildBattle();
            return TRUE;
        }
        else if (MetatileBehavior_IsWaterWildEncounter(MapGridGetMetatileBehaviorAt(x, y)) == TRUE)
        {
            timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);

            if (AreLegendariesInSootopolisPreventingEncounters() == TRUE)
                return FALSE;
            if (gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo == NULL)
                return FALSE;

            if (TryGenerateWildMon(gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo, WILD_AREA_WATER, 0) != TRUE)
                return FALSE;
            BattleSetup_StartWildBattle();
            return TRUE;
        }
    }

    return FALSE;
}

bool8 SweetScentWildEncounter(void)
{
    bool8 encountered;

    sSweetScentActive = TRUE;
    encountered = SweetScentWildEncounterInner();
    sSweetScentActive = FALSE;
    return encountered;
}

static const struct WildPokemonInfo *GetCurrentHoneyMonsInfo(void)
{
    s16 x, y;
    u32 headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return NULL;

    PlayerGetDestCoords(&x, &y);
    if (!MetatileBehavior_IsLandWildEncounter(MapGridGetMetatileBehaviorAt(x, y)))
        return NULL;

    enum TimeOfDay time = GetTimeOfDayForEncounters(headerId, WILD_AREA_HONEY);
    return gWildMonHeaders[headerId].encounterTypes[time].honeyMonsInfo;
}

bool8 CanUseHoneyHere(void)
{
    return GetCurrentHoneyMonsInfo() != NULL;
}

u16 HoneyWildEncounter(void)
{
    const struct WildPokemonInfo *info = GetCurrentHoneyMonsInfo();
    if (info == NULL || !TryGenerateWildMon(info, WILD_AREA_HONEY, 0))
        return FALSE;

    BattleSetup_StartWildBattle();
    return TRUE;
}


bool8 DoesCurrentMapHaveFishingMons(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return FALSE;
    enum TimeOfDay timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_FISHING);
    return gWildMonHeaders[headerId].encounterTypes[timeOfDay].fishingMonsInfo != NULL;
}

void FishingWildEncounter(u8 rod)
{
    enum Species species;
    u32 headerId;
    enum TimeOfDay timeOfDay;

    gIsFishingEncounter = TRUE;
    headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return;
    timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_FISHING);
    const struct WildPokemonInfo *info = gWildMonHeaders[headerId].encounterTypes[timeOfDay].fishingMonsInfo;
    if (info == NULL)
        return;
    s16 x, y;
    GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
    if (CheckFeebasAtCoords(x, y))
    {
        species = gWildFeebas.species;
        CreateWildMon(species, ChooseWildMonLevel(&gWildFeebas, 0, WILD_AREA_FISHING));
    }
    else
        species = GenerateFishingWildMon(info, rod);

    IncrementGameStat(GAME_STAT_FISHING_ENCOUNTERS);
    SetPokemonAnglerSpecies(species);
    BattleSetup_StartWildBattle();
}

u16 GetLocalWildMon(bool8 *isWaterMon)
{
    bool8 ignoredWater;
    u32 headerId;
    enum TimeOfDay timeOfDay;
    const struct WildPokemonInfo *landMonsInfo;
    const struct WildPokemonInfo *waterMonsInfo;

    if (isWaterMon == NULL)
        isWaterMon = &ignoredWater;
    *isWaterMon = FALSE;
    headerId = GetCurrentMapWildMonHeaderId();
    if (headerId == HEADER_NONE)
        return SPECIES_NONE;

    timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND);
    landMonsInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].landMonsInfo;

    timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);
    waterMonsInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;

    // Neither
    if (landMonsInfo == NULL && waterMonsInfo == NULL)
        return SPECIES_NONE;
    // Land Pokémon
    else if (landMonsInfo != NULL && waterMonsInfo == NULL)
        return landMonsInfo->wildPokemon[ChooseWildMonIndex_Land(landMonsInfo)].species;
    // Water Pokémon
    else if (landMonsInfo == NULL && waterMonsInfo != NULL)
    {
        *isWaterMon = TRUE;
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_Water(waterMonsInfo)].species;
    }
    // Either land or water Pokémon
    if ((Random() % 100) < 80)
    {
        return landMonsInfo->wildPokemon[ChooseWildMonIndex_Land(landMonsInfo)].species;
    }
    else
    {
        *isWaterMon = TRUE;
        return waterMonsInfo->wildPokemon[ChooseWildMonIndex_Water(waterMonsInfo)].species;
    }
}

u16 GetLocalWaterMon(void)
{
    u32 headerId = GetCurrentMapWildMonHeaderId();
    enum TimeOfDay timeOfDay;

    if (headerId != HEADER_NONE)
    {
        timeOfDay = GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER);

        const struct WildPokemonInfo *waterMonsInfo = gWildMonHeaders[headerId].encounterTypes[timeOfDay].waterMonsInfo;

        if (waterMonsInfo)
            return waterMonsInfo->wildPokemon[ChooseWildMonIndex_Water(waterMonsInfo)].species;
    }
    return SPECIES_NONE;
}

bool8 UpdateRepelCounter(void)
{
    u16 repelLureVar = VarGet(VAR_REPEL_STEP_COUNT);
    u16 steps = REPEL_LURE_STEPS(repelLureVar);
    bool32 isLure = IS_LAST_USED_LURE(repelLureVar);

    if (InBattlePike() || CurrentBattlePyramidLocation() != PYRAMID_LOCATION_NONE)
        return FALSE;
    if (InUnionRoom() == TRUE)
        return FALSE;

    // Emerald Champions: Repel Spray countdown. When it reaches zero the spray
    // wears off and the player is asked whether to mist the air again.
    if (FlagGet(FLAG_EC_REPEL_SPRAY_ACTIVE))
    {
        u16 spraySteps = VarGet(VAR_EC_REPEL_SPRAY_STEPS);

        if (spraySteps == 0) // save from before the counter existed
            spraySteps = EC_REPEL_SPRAY_STEPS;
        spraySteps--;
        VarSet(VAR_EC_REPEL_SPRAY_STEPS, spraySteps);
        if (spraySteps == 0)
        {
            FlagClear(FLAG_EC_REPEL_SPRAY_ACTIVE);
            ScriptContext_SetupScript(EmeraldChampions_EventScript_RepelSprayWoreOff);
            return TRUE;
        }
    }

    if (steps != 0)
    {
        steps--;
        if (!isLure)
        {
            VarSet(VAR_REPEL_STEP_COUNT, steps);
            if (steps == 0)
            {
                ScriptContext_SetupScript(EventScript_SprayWoreOff);
                return TRUE;
            }
        }
        else
        {
            VarSet(VAR_REPEL_STEP_COUNT, steps | REPEL_LURE_MASK);
            if (steps == 0)
            {
                ScriptContext_SetupScript(EventScript_SprayWoreOff);
                return TRUE;
            }
        }

    }
    return FALSE;
}

bool8 IsWildLevelAllowedByRepel(u8 wildLevel)
{
    u8 i;

    if (!REPEL_STEP_COUNT)
        return TRUE;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (I_REPEL_INCLUDE_FAINTED == GEN_1 || I_REPEL_INCLUDE_FAINTED >= GEN_6 || GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP))
        {
            if (!GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_IS_EGG))
                return wildLevel >= GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL);
        }
    }

    return FALSE;
}

bool8 IsAbilityAllowingEncounter(u8 level)
{
    enum Ability ability;

    if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_EGG))
        return TRUE;

    ability = GetMonAbility(&gParties[B_TRAINER_PLAYER][0]);
    if (ability == ABILITY_KEEN_EYE || ability == ABILITY_INTIMIDATE)
    {
        u8 playerMonLevel = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL);
        if (playerMonLevel > 5 && level <= playerMonLevel - 5 && !(Random() % 2))
            return FALSE;
    }

    return TRUE;
}

static bool8 TryGetRandomWildMonIndexByType(const struct WildPokemon *wildMon, enum Type type, u8 numMon, u8 *monIndex)
{
    u8 validIndexes[numMon]; // variable length array, an interesting feature
    u8 i, validMonCount;

    for (i = 0; i < numMon; i++)
        validIndexes[i] = 0;

    for (validMonCount = 0, i = 0; i < numMon; i++)
    {
        if (GetSpeciesType(wildMon[i].species, 0) == type || GetSpeciesType(wildMon[i].species, 1) == type)
            validIndexes[validMonCount++] = i;
    }

    if (validMonCount == 0 || validMonCount == numMon)
        return FALSE;

    *monIndex = validIndexes[Random() % validMonCount];
    return TRUE;
}

#include "data.h"

static u8 GetMaxLevelOfSpeciesInWildTable(const struct WildPokemon *wildMon, enum Species species, enum WildPokemonArea area)
{
    u8 i, maxLevel = 0, numMon = 0;

    switch (area)
    {
    case WILD_AREA_LAND:
        numMon = NUM_LAND_MONS_ENCOUNTER_SLOTS;
        break;
    case WILD_AREA_WATER:
        numMon = NUM_WATER_MONS_ENCOUNTER_SLOTS;
        break;
    case WILD_AREA_ROCKS:
        numMon = NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS;
        break;
    case WILD_AREA_HONEY:
        numMon = NUM_HONEY_MONS_ENCOUNTER_SLOTS;
        break;
    default:
    case WILD_AREA_FISHING:
    case WILD_AREA_HIDDEN:
        break;
    }

    for (i = 0; i < numMon; i++)
    {
        if (wildMon[i].species == species && wildMon[i].maxLevel > maxLevel)
            maxLevel = wildMon[i].maxLevel;
    }

    return maxLevel;
}

#ifdef BUGFIX
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex, u32 size)
#else
static bool8 TryGetAbilityInfluencedWildMonIndex(const struct WildPokemon *wildMon, enum Type type, enum Ability ability, u8 *monIndex)
#endif
{
    if (GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_EGG))
        return FALSE;
    else if (GetMonAbility(&gParties[B_TRAINER_PLAYER][0]) != ability)
        return FALSE;
    else if (Random() % 2 != 0)
        return FALSE;

#ifdef BUGFIX
    return TryGetRandomWildMonIndexByType(wildMon, type, size, monIndex);
#else
    return TryGetRandomWildMonIndexByType(wildMon, type, NUM_LAND_MONS_ENCOUNTER_SLOTS, monIndex);
#endif
}

static void ApplyCleanseTagEncounterRateMod(u32 *encRate)
{
    enum Item heldItem = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM);
    if (gItemsInfo[heldItem].holdEffect == HOLD_EFFECT_REPEL)
        *encRate = *encRate * 2 / 3;
}

bool8 TryDoDoubleWildBattle(void)
{
    if (GetSafariZoneFlag()
      || (WE_DOUBLE_WILD_REQUIRE_2_MONS && GetMonsStateToDoubles() != PLAYER_HAS_TWO_USABLE_MONS))
        return FALSE;
    if (FollowerNPCIsBattlePartner() && FNPC_FLAG_PARTNER_WILD_BATTLES != 0
     && (FNPC_FLAG_PARTNER_WILD_BATTLES == FNPC_ALWAYS || FlagGet(FNPC_FLAG_PARTNER_WILD_BATTLES)) && FNPC_NPC_FOLLOWER_WILD_BATTLE_VS_2 == TRUE)
        return TRUE;
    else if (FlagGet(WE_FLAG_FORCE_DOUBLE_WILD))
        return TRUE;
    else if (RandomPercentage(RNG_NONE, WE_DOUBLE_WILD_CHANCE))
        return TRUE;
    return FALSE;
}

u32 ChooseHiddenMonIndex(void)
{
    #ifdef ENCOUNTER_CHANCE_HIDDEN_MONS_TOTAL
        u8 rand = Random() % ENCOUNTER_CHANCE_HIDDEN_MONS_TOTAL;

        if (rand < ENCOUNTER_CHANCE_HIDDEN_MONS_SLOT_0)
            return 0;
        else if (rand >= ENCOUNTER_CHANCE_HIDDEN_MONS_SLOT_0 && rand < ENCOUNTER_CHANCE_HIDDEN_MONS_SLOT_1)
            return 1;
        else
            return 2;
    #else
        return 0xFF;
    #endif
}
