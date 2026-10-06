#include "global.h"
#include "caps.h"
#include "fishing.h"
#include "field_move.h"
#include "item.h"
#include "legendary_signs.h"
#include "mass_outbreak.h"
#include "overworld.h"
#include "roamer.h"
#include "weather_anomaly.h"
#include "wild_encounter.h"
#include "wild_roster.h"
#include "constants/map_types.h"
#include "constants/items.h"
#include "constants/pokemon.h"

// Rarity words by share of the method's battles. Every table slot is at least
// 4% (Legendary, Ultra Beast and Paradox slots 5%), so Very Rare is left for
// Feebas's hidden spots and residents crowded out by a storm visitor, a
// roamer or an outbreak.
#define WILD_RARITY_COMMON_SHARE   1500 // 15% and up
#define WILD_RARITY_UNCOMMON_SHARE  800 // 8% to 15%
#define WILD_RARITY_RARE_SHARE      400 // 4% to 8%; below that, Very Rare

// A rule that takes its part of a method's draws before the table slots do,
// in the engine's order: roamers, then an outbreak, then a storm visitor.
struct RosterOverlay
{
    enum Species species;
    u8 minLevel;
    u8 maxLevel;
    u8 numerator;
    u8 denominator;
};

struct RosterBuild
{
    struct WildRosterEntry entries[WILD_ROSTER_MAX_ENTRIES];
    u32 count;
    u32 methodStart;
};

static void StartRosterMethod(struct RosterBuild *build)
{
    build->methodStart = build->count;
}

static void AddRosterShare(struct RosterBuild *build, u8 method, enum Species species, u32 share, u8 minLevel, u8 maxLevel)
{
    if (share == 0 || species == SPECIES_NONE)
        return;
    for (u32 i = build->methodStart; i < build->count; i++)
    {
        struct WildRosterEntry *entry = &build->entries[i];
        if (entry->species != species)
            continue;
        entry->share += share;
        entry->minLevel = min(entry->minLevel, minLevel);
        entry->maxLevel = max(entry->maxLevel, maxLevel);
        return;
    }
    if (build->count < WILD_ROSTER_MAX_ENTRIES)
    {
        build->entries[build->count++] = (struct WildRosterEntry){
            .species = species, .share = share, .method = method,
            .minLevel = minLevel, .maxLevel = maxLevel,
        };
    }
}

// Shares become parts of the battles the method actually starts (a draw that
// finds nothing starts none), get their words, and sort most common first.
static void FinishRosterMethod(struct RosterBuild *build)
{
    struct WildRosterEntry *entries = &build->entries[build->methodStart];
    u32 count = build->count - build->methodStart, total = 0;

    for (u32 i = 0; i < count; i++)
        total += entries[i].share;
    for (u32 i = 0; i < count; i++)
    {
        if (total != WILD_SLOT_SHARE_TOTAL)
            entries[i].share = (entries[i].share * WILD_SLOT_SHARE_TOTAL + total / 2) / total;
        entries[i].rarity = GetWildRosterRarity(entries[i].share);
    }
    for (u32 i = 1; i < count; i++)
    {
        struct WildRosterEntry entry = entries[i];
        u32 j = i;
        while (j != 0 && entries[j - 1].share < entry.share)
        {
            entries[j] = entries[j - 1];
            j--;
        }
        entries[j] = entry;
    }
}

static void AddRosterTable(struct RosterBuild *build, u8 method, const struct WildPokemonInfo *info,
                           enum WildPokemonArea area, u32 rod, const struct RosterOverlay *overlays, u32 overlayCount)
{
    u16 shares[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    u32 remaining = WILD_SLOT_SHARE_TOTAL, count;

    if (info == NULL)
        return;
    StartRosterMethod(build);
    for (u32 i = 0; i < overlayCount; i++)
    {
        u32 taken = remaining * overlays[i].numerator / overlays[i].denominator;
        AddRosterShare(build, method, overlays[i].species, taken, overlays[i].minLevel, overlays[i].maxLevel);
        remaining -= taken;
    }
    count = GetWildSlotShares(info, area, rod, shares);
    for (u32 slot = 0; slot < count; slot++)
    {
        u8 minLevel, maxLevel;

        if (shares[slot] == 0)
            continue;
        GetWildSlotLevelRange(&info->wildPokemon[slot], &minLevel, &maxLevel);
        AddRosterShare(build, method, info->wildPokemon[slot].species,
                       shares[slot] * remaining / WILD_SLOT_SHARE_TOTAL, minLevel, maxLevel);
    }
    FinishRosterMethod(build);
}

// Roamers (TryStartRoamerEncounter), an outbreak (land only) and a storm
// visitor (TryGenerateWildMon), each taking its part of what is left.
static u32 GetRosterOverlays(u8 mapGroup, u8 mapNum, enum WildPokemonArea area, struct RosterOverlay *overlays)
{
    u32 count = 0;
    enum Species visitor;

    for (u32 i = 0; i < ROAMER_COUNT; i++)
    {
        const struct Roamer *roamer = &gSaveBlock1Ptr->roamer[i];
        if (IsRoamerAt(i, mapGroup, mapNum))
            overlays[count++] = (struct RosterOverlay){roamer->species, roamer->level, roamer->level, 1, ROAMER_ENCOUNTER_ODDS};
    }
    if (area == WILD_AREA_LAND && IsMassOutbreakOnMap(mapGroup, mapNum))
    {
        u8 level = min(gSaveBlock1Ptr->outbreakPokemonLevel, GetCurrentLevelCap());
        overlays[count++] = (struct RosterOverlay){gSaveBlock1Ptr->outbreakPokemonSpecies, level, level,
                                                   gSaveBlock1Ptr->outbreakPokemonProbability, 100};
    }
    visitor = GetWeatherAnomalyEncounterSpecies(mapGroup, mapNum, area);
    if (visitor != SPECIES_NONE)
    {
        u8 level = GetLegendaryEncounterLevel(visitor);
        overlays[count++] = (struct RosterOverlay){visitor, level, level, WEATHER_ANOMALY_ENCOUNTER_PERCENT, 100};
    }
    return count;
}

// Every level of the map's band and both form rolls, through the encounter's
// own GetCutTreeSlotEncounter, so a band that straddles an evolution level
// lists both forms at the levels each comes at. Each draw weighs its slot's
// odds; FinishRosterMethod turns the weights into shares.
static void AddRosterCutTrees(struct RosterBuild *build, u16 headerId)
{
    u8 low, high;

    if (!GetCutTreeLevelBand(headerId, &low, &high))
        return;
    StartRosterMethod(build);
    for (u32 slot = 0; slot < GetCutTreeSlotCount(); slot++)
    {
        for (u32 bandLevel = low; bandLevel <= high; bandLevel++)
        {
            for (u32 formRoll = 0; formRoll < 2; formRoll++)
            {
                u8 level;
                enum Species species = GetCutTreeSlotEncounter(slot, bandLevel, formRoll, &level);

                AddRosterShare(build, WILD_ROSTER_CUT_TREES, species, GetCutTreeSlotOdds(slot), level, level);
            }
        }
    }
    FinishRosterMethod(build);
}

static void AddRosterFeebas(struct RosterBuild *build)
{
    u8 minLevel, maxLevel;

    GetWildSlotLevelRange(&gWildFeebas, &minLevel, &maxLevel);
    StartRosterMethod(build);
    AddRosterShare(build, WILD_ROSTER_FEEBAS, gWildFeebas.species, GetFeebasSpotShare(), minLevel, maxLevel);
    // Its own rarity, not a part of a method's battles: no rescaling.
    for (u32 i = build->methodStart; i < build->count; i++)
        build->entries[i].rarity = GetWildRosterRarity(build->entries[i].share);
}

static bool32 OwnsRosterTool(enum Item item)
{
    return CheckBagHasItem(item, 1) || CheckPCHasItem(item, 1);
}

u32 GetWildRosterForMap(u8 mapGroup, u8 mapNum, struct WildRosterEntry *entries, u32 max)
{
    struct RosterBuild build = {.count = 0};
    struct RosterOverlay overlays[ROAMER_COUNT + 2];
    const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum);
    const struct WildEncounterTypes *types;
    u16 headerId = GetWildMonHeaderIdForMap(mapGroup, mapNum);

    if (headerId == HEADER_NONE)
        return 0;
    types = gWildMonHeaders[headerId].encounterTypes;

    const struct WildPokemonInfo *land = types[GetTimeOfDayForEncounters(headerId, WILD_AREA_LAND)].landMonsInfo;
    AddRosterTable(&build, WILD_ROSTER_LAND, land, WILD_AREA_LAND, 0,
                   overlays, GetRosterOverlays(mapGroup, mapNum, WILD_AREA_LAND, overlays));

    // Underwater, every encounter tile uses the seabed's land table.
    if (IsFieldMoveUnlocked(FIELD_MOVE_SURF)
     && !AreSurfEncountersBlockedOnMap(mapGroup, mapNum)
     && !(mapHeader->mapType == MAP_TYPE_UNDERWATER && land != NULL))
    {
        AddRosterTable(&build, WILD_ROSTER_SURFING, types[GetTimeOfDayForEncounters(headerId, WILD_AREA_WATER)].waterMonsInfo,
                       WILD_AREA_WATER, 0, overlays, GetRosterOverlays(mapGroup, mapNum, WILD_AREA_WATER, overlays));
    }

    const struct WildPokemonInfo *fishing = types[GetTimeOfDayForEncounters(headerId, WILD_AREA_FISHING)].fishingMonsInfo;
    bool32 hasRod = FALSE;
    if (fishing != NULL)
    {
        if (OwnsRosterTool(ITEM_OLD_ROD))
        {
            AddRosterTable(&build, WILD_ROSTER_OLD_ROD, fishing, WILD_AREA_FISHING, OLD_ROD, NULL, 0);
            hasRod = TRUE;
        }
        if (OwnsRosterTool(ITEM_GOOD_ROD))
        {
            AddRosterTable(&build, WILD_ROSTER_GOOD_ROD, fishing, WILD_AREA_FISHING, GOOD_ROD, NULL, 0);
            hasRod = TRUE;
        }
        if (OwnsRosterTool(ITEM_SUPER_ROD))
        {
            AddRosterTable(&build, WILD_ROSTER_SUPER_ROD, fishing, WILD_AREA_FISHING, SUPER_ROD, NULL, 0);
            hasRod = TRUE;
        }
    }
    if (IsFieldMoveUnlocked(FIELD_MOVE_ROCK_SMASH) && MapHeaderHasRockSmash(mapHeader))
        AddRosterTable(&build, WILD_ROSTER_ROCK_SMASH, types[GetTimeOfDayForEncounters(headerId, WILD_AREA_ROCKS)].rockSmashMonsInfo,
                       WILD_AREA_ROCKS, 0, NULL, 0);
    if (OwnsRosterTool(ITEM_HONEY) || FieldMove_GetUserSlot(FIELD_MOVE_SWEET_SCENT, TRUE) != PARTY_SIZE)
        AddRosterTable(&build, WILD_ROSTER_HONEY, types[GetTimeOfDayForEncounters(headerId, WILD_AREA_HONEY)].honeyMonsInfo,
                       WILD_AREA_HONEY, 0, NULL, 0);
    if (IsFieldMoveUnlocked(FIELD_MOVE_CUT) && MapHeaderHasCutTrees(mapHeader))
        AddRosterCutTrees(&build, headerId);
    if (hasRod && MapHasFeebasSpots(mapGroup, mapNum))
        AddRosterFeebas(&build);

    for (u32 i = 0; entries != NULL && i < build.count && i < max; i++)
        entries[i] = build.entries[i];
    return build.count;
}

u32 GetWildRosterForCurrentMap(struct WildRosterEntry *entries, u32 max)
{
    return GetWildRosterForMap(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum, entries, max);
}

enum WildRosterRarity GetWildRosterRarity(u32 share)
{
    if (share >= WILD_RARITY_COMMON_SHARE)
        return WILD_RARITY_COMMON;
    if (share >= WILD_RARITY_UNCOMMON_SHARE)
        return WILD_RARITY_UNCOMMON;
    if (share >= WILD_RARITY_RARE_SHARE)
        return WILD_RARITY_RARE;
    return WILD_RARITY_VERY_RARE;
}

const u8 *GetWildRosterRarityName(enum WildRosterRarity rarity)
{
    switch (rarity)
    {
    case WILD_RARITY_COMMON:    return COMPOUND_STRING("Common");
    case WILD_RARITY_UNCOMMON:  return COMPOUND_STRING("Uncommon");
    case WILD_RARITY_RARE:      return COMPOUND_STRING("Rare");
    default:                    return COMPOUND_STRING("Very Rare");
    }
}

const u8 *GetWildRosterMethodName(enum WildRosterMethod method, u8 mapGroup, u8 mapNum)
{
    switch (method)
    {
    case WILD_ROSTER_LAND:
        switch (Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)->mapType)
        {
        case MAP_TYPE_UNDERGROUND: return COMPOUND_STRING("Cave");
        case MAP_TYPE_UNDERWATER:  return COMPOUND_STRING("Seaweed");
        case MAP_TYPE_INDOOR:      return COMPOUND_STRING("Walking");
        default:                   return COMPOUND_STRING("Tall Grass");
        }
    case WILD_ROSTER_SURFING:    return COMPOUND_STRING("Surfing");
    case WILD_ROSTER_OLD_ROD:    return COMPOUND_STRING("Old Rod");
    case WILD_ROSTER_GOOD_ROD:   return COMPOUND_STRING("Good Rod");
    case WILD_ROSTER_SUPER_ROD:  return COMPOUND_STRING("Super Rod");
    case WILD_ROSTER_ROCK_SMASH: return COMPOUND_STRING("Rock Smash");
    case WILD_ROSTER_HONEY:      return COMPOUND_STRING("Honey");
    case WILD_ROSTER_CUT_TREES:  return COMPOUND_STRING("Cut Trees");
    case WILD_ROSTER_FEEBAS:     return COMPOUND_STRING("Hidden Spots");
    default:                     return COMPOUND_STRING("");
    }
}
