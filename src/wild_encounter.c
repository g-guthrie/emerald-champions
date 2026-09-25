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
static bool32 IsWildSlotLive(enum Species species)
{
    return IsWildSlotSpeciesAcquirable(species) && !IsWeatherAnomalyVisitorSlotInert(species);
}

static void ApplyFluteEncounterRateMod(u32 *encRate);
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

#define ROUTE_SIGN_MAX_METHOD_SPECIES NUM_LAND_MONS_ENCOUNTER_SLOTS
#define ROUTE_SIGN_MAX_LINE_WIDTH 200

struct RouteSignSpecies
{
    enum Species species;
};

static const u8 sText_RouteSignSpeciesHeader[] = _("Pokémon found here:");
static const u8 sText_RouteSignSpeciesSeparator[] = _(", ");
static const u8 sText_RouteSignSpeciesComma[] = _(",");
static const u8 sText_RouteSignSpeciesNewLine[] = _("\n");
static const u8 sText_RouteSignSpeciesLineBreak[] = _("\l");
static const u8 sText_RouteSignSpeciesPageBreak[] = _("\p");
static const u8 sText_RouteSignNoSpecies[] = _("No wild Pokémon are found here.");
static const u8 sText_RouteSignGrass[] = _("Grass: ");
static const u8 sText_RouteSignSurf[] = _("Surf: ");
static const u8 sText_RouteSignRockSmash[] = _("Rock Smash: ");
static const u8 sText_RouteSignOldRod[] = _("Old Rod: ");
static const u8 sText_RouteSignGoodRod[] = _("Good Rod: ");
static const u8 sText_RouteSignSuperRod[] = _("Super Rod: ");
static const u8 sText_RouteSignHoney[] = _("Honey: ");
static const u8 sText_RouteSignHidden[] = _("Hidden: ");
static const u8 sText_RouteSignCutTrees[] = _("Cut trees: ");

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
    {SPECIES_SKWOVET, 40},
    {SPECIES_PINECO, 30},
    {SPECIES_AIPOM, 15},
    {SPECIES_BURMY, 8},
    {SPECIES_APPLIN, 5},
    {SPECIES_PHANTUMP, 2},
};

extern const u8 EventScript_CutTree[];

u32 GetCutTreeSlotCount(void)
{
    return ARRAY_COUNT(sCutTreeHabitat);
}

enum Species GetCutTreeSlotSpecies(u32 slot)
{
    return slot < ARRAY_COUNT(sCutTreeHabitat) ? sCutTreeHabitat[slot].species : SPECIES_NONE;
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

u8 GetCutTreeEncounterLevelFromRoll(u32 roll)
{
    u32 cap = GetCurrentLevelCap();
    u32 low = cap > CUT_TREE_LEVELS_BELOW_CAP_MAX ? cap - CUT_TREE_LEVELS_BELOW_CAP_MAX : 1;
    u32 high = cap > CUT_TREE_LEVELS_BELOW_CAP_MIN ? cap - CUT_TREE_LEVELS_BELOW_CAP_MIN : 1;

    return low + roll % (high - low + 1);
}

// Does the map the player stands on have a Cut tree? The sign roster lists
// the tree habitat only where a tree grows.
static bool32 CurrentMapHasCutTrees(void)
{
    const struct MapEvents *events = Overworld_GetMapHeaderByGroupAndId(
        gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum)->events;

    if (events == NULL)
        return FALSE;
    for (u32 i = 0; i < events->objectEventCount; i++)
    {
        if (events->objectEvents[i].script == EventScript_CutTree)
            return TRUE;
    }
    return FALSE;
}

static u8 CollectRouteSignSpecies(
    struct RouteSignSpecies *entries,
    const struct WildPokemonInfo *info,
    u8 firstSlot,
    u8 slotCount)
{
    u8 count = 0;

    if (info == NULL)
        return 0;
    for (u32 i = 0; i < slotCount; i++)
    {
        enum Species species = info->wildPokemon[firstSlot + i].species;
        u32 j;

        if (species == SPECIES_NONE || species >= NUM_SPECIES)
            continue;
        for (j = 0; j < count; j++)
            // Forms that share a display name (Pumpkaboo sizes) list once.
            if (entries[j].species == species
             || StringCompare(GetLegendaryDisplayName(entries[j].species), GetLegendaryDisplayName(species)) == 0)
                break;
        if (j == count && count < ROUTE_SIGN_MAX_METHOD_SPECIES)
            entries[count++].species = species;
    }
    return count;
}

static u8 *AppendRouteSignMethod(
    u8 *dest,
    const u8 *methodName,
    const struct RouteSignSpecies *entries,
    u8 count,
    bool32 *hasMethod,
    bool32 *hasLegend)
{
    bool32 firstName = TRUE;
    u16 lineWidth;

    if (count == 0)
        return dest;
    dest = StringCopy(dest, *hasMethod ? sText_RouteSignSpeciesPageBreak : sText_RouteSignSpeciesNewLine);
    dest = StringCopy(dest, methodName);
    lineWidth = GetStringWidth(FONT_NORMAL, methodName, 0);
    for (u32 i = 0; i < count; i++)
    {
        u8 name[64];
        enum LegendarySignId id = GetLegendarySignIdBySpecies(entries[i].species);
        if (IsLegendaryEncounterSpecies(entries[i].species))
            *hasLegend = TRUE;
        StringCopy(name, GetLegendaryDisplayName(entries[i].species));
        if (id < LEGENDARY_SIGN_COUNT && IsLegendarySignCaught(id))
            StringAppend(name, COMPOUND_STRING(" (Caught)"));
        u16 nameWidth = GetStringWidth(FONT_NORMAL, name, 0);
        u16 separatorWidth = firstName ? 0 : GetStringWidth(FONT_NORMAL, sText_RouteSignSpeciesSeparator, 0);

        // Reserve the comma that will end this line if the next name wraps.
        u16 trailingWidth = i + 1 < count ? GetStringWidth(FONT_NORMAL, sText_RouteSignSpeciesComma, 0) : 0;
        if (lineWidth + separatorWidth + nameWidth + trailingWidth > ROUTE_SIGN_MAX_LINE_WIDTH)
        {
            if (!firstName)
                dest = StringCopy(dest, sText_RouteSignSpeciesComma);
            dest = StringCopy(dest, sText_RouteSignSpeciesLineBreak);
            lineWidth = 0;
            firstName = TRUE;
        }
        if (!firstName)
        {
            dest = StringCopy(dest, sText_RouteSignSpeciesSeparator);
            lineWidth += separatorWidth;
        }
        dest = StringCopy(dest, name);
        lineWidth += nameWidth;
        firstName = FALSE;
    }
    *hasMethod = TRUE;
    return dest;
}

static const struct WildPokemonInfo *GetRouteSignInfo(u32 headerId, enum WildPokemonArea area)
{
    enum TimeOfDay time = GetTimeOfDayForEncounters(headerId, area);
    const struct WildEncounterTypes *types = &gWildMonHeaders[headerId].encounterTypes[time];

    switch (area)
    {
    case WILD_AREA_LAND:
        return types->landMonsInfo;
    case WILD_AREA_WATER:
        return types->waterMonsInfo;
    case WILD_AREA_ROCKS:
        return types->rockSmashMonsInfo;
    case WILD_AREA_FISHING:
        return types->fishingMonsInfo;
    case WILD_AREA_HIDDEN:
        return types->hiddenMonsInfo;
    case WILD_AREA_HONEY:
        return types->honeyMonsInfo;
    default:
        return NULL;
    }
}

void BufferCurrentMapRouteSignSpecies(void)
{
    struct RouteSignSpecies entries[ROUTE_SIGN_MAX_METHOD_SPECIES];
    u32 headerId = GetCurrentMapWildMonHeaderId();
    bool32 hasMethod = FALSE, hasLegend = FALSE;
    u8 *dest = StringCopy(gStringVar4, sText_RouteSignSpeciesHeader);

    if (headerId != HEADER_NONE)
    {
        const struct WildPokemonInfo *info;
        u8 count;

        info = GetRouteSignInfo(headerId, WILD_AREA_LAND);
        count = CollectRouteSignSpecies(entries, info, 0, NUM_LAND_MONS_ENCOUNTER_SLOTS);
        dest = AppendRouteSignMethod(dest, sText_RouteSignGrass, entries, count, &hasMethod, &hasLegend);
        info = GetRouteSignInfo(headerId, WILD_AREA_WATER);
        count = CollectRouteSignSpecies(entries, info, 0, NUM_WATER_MONS_ENCOUNTER_SLOTS);
        dest = AppendRouteSignMethod(dest, sText_RouteSignSurf, entries, count, &hasMethod, &hasLegend);
        info = GetRouteSignInfo(headerId, WILD_AREA_ROCKS);
        count = CollectRouteSignSpecies(entries, info, 0, NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS);
        dest = AppendRouteSignMethod(dest, sText_RouteSignRockSmash, entries, count, &hasMethod, &hasLegend);
        info = GetRouteSignInfo(headerId, WILD_AREA_FISHING);
        count = CollectRouteSignSpecies(entries, info, 0, 2);
        dest = AppendRouteSignMethod(dest, sText_RouteSignOldRod, entries, count, &hasMethod, &hasLegend);
        count = CollectRouteSignSpecies(entries, info, 2, 3);
        dest = AppendRouteSignMethod(dest, sText_RouteSignGoodRod, entries, count, &hasMethod, &hasLegend);
        count = CollectRouteSignSpecies(entries, info, 5, 5);
        dest = AppendRouteSignMethod(dest, sText_RouteSignSuperRod, entries, count, &hasMethod, &hasLegend);
        info = GetRouteSignInfo(headerId, WILD_AREA_HONEY);
        count = CollectRouteSignSpecies(entries, info, 0, NUM_HONEY_MONS_ENCOUNTER_SLOTS);
        dest = AppendRouteSignMethod(dest, sText_RouteSignHoney, entries, count, &hasMethod, &hasLegend);
        if (DEXNAV_ENABLED)
        {
            info = GetRouteSignInfo(headerId, WILD_AREA_HIDDEN);
            count = CollectRouteSignSpecies(entries, info, 0, NUM_HIDDEN_MONS_ENCOUNTER_SLOTS);
            dest = AppendRouteSignMethod(dest, sText_RouteSignHidden, entries, count, &hasMethod, &hasLegend);
        }
        if (CurrentMapHasCutTrees())
        {
            for (count = 0; count < ARRAY_COUNT(sCutTreeHabitat); count++)
                entries[count].species = sCutTreeHabitat[count].species;
            dest = AppendRouteSignMethod(dest, sText_RouteSignCutTrees, entries, count, &hasMethod, &hasLegend);
        }
    }

    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_ROUTE119)
     && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE119))
        dest = StringCopy(dest, COMPOUND_STRING("\pFeebas hides in a few fishing spots.\nAny rod works if you find one."));
    if (headerId != HEADER_NONE)
    {
        enum LegendarySignId anomaly = GetLiveWeatherAnomalyOnMap(gSaveBlock1Ptr->location.mapGroup,
                                                                 gSaveBlock1Ptr->location.mapNum);
        if (anomaly < LEGENDARY_SIGN_COUNT)
        {
            dest = StringCopy(dest, sText_RouteSignSpeciesPageBreak);
            dest = StringCopy(dest, GetLegendaryDisplayName(gLegendaryGates[anomaly].species));
            dest = StringCopy(dest, COMPOUND_STRING(" has been sighted in\nthis weather."));
        }
    }
    if (hasMethod && hasLegend)
        dest = StringCopy(dest, COMPOUND_STRING("\pLegends and Ultra Beasts are rare\nhere. Sweet Scent draws them out."));
    if (hasMethod)
        StringCopy(dest, COMPOUND_STRING("\pSweet Scent reverses grass/Surf\nrarity: rare species become common."));
    else
        StringCopy(gStringVar4, sText_RouteSignNoSpecies);
}

// Route signs page through the roster in the ordinary field message box:
// A turns the page (the text printer's own prompt), B closes the sign at
// once, including any legend pages the map script shows after the roster.
EWRAM_DATA static bool8 sRouteSignClosed = FALSE;

static bool8 WaitForRouteSignPage(void)
{
    if (JOY_NEW(B_BUTTON))
    {
        HideFieldMessageBox();
        sRouteSignClosed = TRUE;
        return TRUE;
    }
    // After the last page, A continues like waitbuttonpress.
    return IsFieldMessageBoxHidden() && JOY_NEW(A_BUTTON);
}

static void ShowRouteSignPages(struct ScriptContext *ctx)
{
    if (sRouteSignClosed || !ShowFieldMessageFromBuffer())
        return;
    SetupNativeScript(ctx, WaitForRouteSignPage);
    ctx->waitAfterCallNative = TRUE;
}

// callnative from Common_EventScript_ShowRouteRoster.
void ShowRouteSignRoster(struct ScriptContext *ctx)
{
    sRouteSignClosed = FALSE;
    BufferCurrentMapRouteSignSpecies();
    ShowRouteSignPages(ctx);
}

// callnative from Common_EventScript_ShowRouteLegend (VAR_0x8004 = the
// resident's LEGENDARY_SIGN_* id). Skipped once B closed the sign.
void ShowRouteSignLegendPage(struct ScriptContext *ctx)
{
    if (sRouteSignClosed)
        return;
    ResearchSelectedLegendarySign();
    ShowRouteSignPages(ctx);
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

bool8 CheckFeebasAtCoords(s16 x, s16 y)
{
    u8 i;
    u16 feebasSpots[NUM_FEEBAS_SPOTS];
    u8 route119Section = 0;
    u16 spotId;

    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_ROUTE119)
     && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ROUTE119))
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

        // 50% chance of encountering Feebas (assuming this is a Feebas spot)
        if (Random() % 100 > 49)
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

static const u8 *GetEncounterBounds(const struct WildPokemonInfo *info, const u8 *defaults)
{
    return info != NULL && info->encounterBounds != NULL ? info->encounterBounds : defaults;
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

u32 ChooseWildMonIndex_Land(const struct WildPokemonInfo *info)
{
    return ChooseEncounterSlotWithLure(GetEncounterBounds(info, sLandEncounterBounds), ARRAY_COUNT(sLandEncounterBounds));
}

// Sweet Scent draws out eligible Legendary and Ultra Beast slots: each gets
// five times its table odds (1% -> 5%, 3% -> 15%), with their combined share
// scaled down to at most half of all outcomes. Gated or caught slots are inert
// and contribute nothing. The remaining mass reverses ordinary species
// probabilities, not slot positions: duplicate slots are combined first and
// tied species share their reversed probability equally.
#define SWEET_SCENT_LEGEND_MULTIPLIER 5

static bool32 IsSweetScentLegendSlot(enum Species species)
{
    return IsLegendaryEncounterSpecies(species);
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
                legendTotal += weight;
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

    legendMass = min(legendTotal * SWEET_SCENT_LEGEND_MULTIPLIER, total / 2);
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
    static const u8 sFishingDefaults[] = {
        ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0, ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_1,
        ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2, ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3,
        ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4,
        ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5, ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6,
        ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7, ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8,
        ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9,
    };
    const u8 *bounds;
    u32 count;
    bool32 segmentStart = slot == 0;

    switch (area)
    {
    case WILD_AREA_LAND:
        bounds = GetEncounterBounds(info, sLandEncounterBounds);
        count = ARRAY_COUNT(sLandEncounterBounds);
        break;
    case WILD_AREA_WATER:
        bounds = GetEncounterBounds(info, sWaterEncounterBounds);
        count = ARRAY_COUNT(sWaterEncounterBounds);
        break;
    case WILD_AREA_ROCKS:
        bounds = GetEncounterBounds(info, sRockEncounterBounds);
        count = ARRAY_COUNT(sRockEncounterBounds);
        break;
    case WILD_AREA_HONEY:
        bounds = GetEncounterBounds(info, sHoneyEncounterBounds);
        count = ARRAY_COUNT(sHoneyEncounterBounds);
        break;
    case WILD_AREA_FISHING:
        bounds = GetEncounterBounds(info, sFishingDefaults);
        count = ARRAY_COUNT(sFishingDefaults);
        // Each rod's cumulative odds restart at its first slot.
        segmentStart = slot == 0 || slot == 2 || slot == 5;
        break;
    default:
        return 0;
    }
    if (slot >= count)
        return 0;
    return bounds[slot] - (segmentStart ? 0 : bounds[slot - 1]);
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
    static const u8 defaults[] = {
        ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_0, ENCOUNTER_CHANCE_FISHING_MONS_OLD_ROD_SLOT_1,
        ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_2, ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_3,
        ENCOUNTER_CHANCE_FISHING_MONS_GOOD_ROD_SLOT_4,
        ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_5, ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_6,
        ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_7, ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_8,
        ENCOUNTER_CHANCE_FISHING_MONS_SUPER_ROD_SLOT_9,
    };
    static const u8 starts[] = {0, 2, 5};
    static const u8 counts[] = {2, 3, 5};
    if (rod > SUPER_ROD)
        return 0;
    return starts[rod] + ChooseEncounterSlotWithLure(GetEncounterBounds(info, defaults) + starts[rod], counts[rod]);
}

// Emerald Champions: a table authored for an early visit must not leave an
// area far below the Trainer who returns to it, and no evolved Pokemon is
// met below the level it evolves at. Levels already in range are untouched;
// every caller still applies the cap ceiling afterwards.
#define WILD_LEVELS_BELOW_CAP_FLOOR 12
#define WILD_LEVEL_FLOOR_SPREAD     4

#include "data/wild_evolution_floors.h"

static u8 GetWildEvolutionFloor(enum Species species)
{
    for (u32 i = 0; i < ARRAY_COUNT(sWildEvolutionFloors); i++)
        if (sWildEvolutionFloors[i].species == species)
            return sWildEvolutionFloors[i].level;
    return 1;
}

u8 ApplyWildLevelFloor(enum Species species, u8 level)
{
    u32 cap = GetCurrentLevelCap();
    u32 floor = cap > WILD_LEVELS_BELOW_CAP_FLOOR ? cap - WILD_LEVELS_BELOW_CAP_FLOOR : 1;
    if (level < floor)
        level = floor + Random() % WILD_LEVEL_FLOOR_SPREAD;
    level = max(level, GetWildEvolutionFloor(species));
    return min(level, min(cap, MAX_LEVEL));
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
    u16 i;

    for (i = 0; ; i++)
    {
        const struct WildPokemonHeader *wildHeader = &gWildMonHeaders[i];
        if (wildHeader->mapGroup == MAP_GROUP(MAP_UNDEFINED))
            break;

        if (gWildMonHeaders[i].mapGroup == gSaveBlock1Ptr->location.mapGroup &&
            gWildMonHeaders[i].mapNum == gSaveBlock1Ptr->location.mapNum)
        {
            if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_ALTERING_CAVE) &&
                gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_ALTERING_CAVE))
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

#ifdef BUGFIX
#define TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildPokemon, type, ability, ptr, count) TryGetAbilityInfluencedWildMonIndex(wildPokemon, type, ability, ptr, count)
#else
#define TRY_GET_ABILITY_INFLUENCED_WILD_MON_INDEX(wildPokemon, type, ability, ptr, count) TryGetAbilityInfluencedWildMonIndex(wildPokemon, type, ability, ptr)
#endif

bool8 TryGenerateWildMon(const struct WildPokemonInfo *wildMonInfo, enum WildPokemonArea area, u8 flags)
{
    u8 wildMonIndex = 0;
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

    species = wildMonInfo->wildPokemon[wildMonIndex].species;
    if (!IsWildSlotLive(species))
    {
        // A gated or caught Legendary/Ultra Beast slot is inert: the draw
        // moves on to the next acquirable slot of the same table.
        u32 slots = area == WILD_AREA_LAND ? NUM_LAND_MONS_ENCOUNTER_SLOTS
                  : area == WILD_AREA_WATER ? NUM_WATER_MONS_ENCOUNTER_SLOTS
                  : area == WILD_AREA_ROCKS ? NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS
                  : area == WILD_AREA_HONEY ? NUM_HONEY_MONS_ENCOUNTER_SLOTS : 1;
        u32 tried;
        for (tried = 0; tried < slots; tried++)
        {
            wildMonIndex = (wildMonIndex + 1) % slots;
            species = wildMonInfo->wildPokemon[wildMonIndex].species;
            if (IsWildSlotLive(species))
                break;
        }
        if (tried == slots)
            return FALSE;
    }

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

    CreateWildMon(species, level);
    if (legendary)
        ApplyLegendaryEncounterSet(&gParties[B_TRAINER_OPPONENT_A][0], ITEM_NONE);
    return TRUE;
}

// Rods follow the same slot rule as every other method: an inert Legendary or
// Ultra Beast slot passes to the next acquirable slot of the same rod, then of
// the whole fishing table.
static u16 GenerateFishingWildMon(const struct WildPokemonInfo *wildMonInfo, u8 rod)
{
    static const u8 starts[] = {0, 2, 5};
    static const u8 counts[] = {2, 3, 5};
    u8 wildMonIndex = ChooseWildMonIndex_Fishing(wildMonInfo, rod);
    enum Species wildMonSpecies = wildMonInfo->wildPokemon[wildMonIndex].species;
    u8 level;

    if (!IsWildSlotLive(wildMonSpecies) && rod <= SUPER_ROD)
    {
        u32 candidate = wildMonIndex;
        bool32 found = FALSE;
        for (u32 i = 1; i < counts[rod] && !found; i++)
        {
            candidate = starts[rod] + (wildMonIndex - starts[rod] + i) % counts[rod];
            found = IsWildSlotLive(wildMonInfo->wildPokemon[candidate].species);
        }
        for (u32 i = 1; i < NUM_FISHING_MONS_ENCOUNTER_SLOTS && !found; i++)
        {
            candidate = (wildMonIndex + i) % NUM_FISHING_MONS_ENCOUNTER_SLOTS;
            found = IsWildSlotLive(wildMonInfo->wildPokemon[candidate].species);
        }
        // A table made only of inert legends keeps its drawn slot.
        if (found)
            wildMonIndex = candidate;
        wildMonSpecies = wildMonInfo->wildPokemon[wildMonIndex].species;
    }
    if (IsLegendaryEncounterSpecies(wildMonSpecies))
        level = GetLegendaryEncounterLevel(wildMonSpecies);
    else
        level = ChooseWildMonLevel(wildMonInfo->wildPokemon, wildMonIndex, WILD_AREA_FISHING);

    UpdateChainFishingStreak();
    CreateWildMon(wildMonSpecies, level);
    if (IsLegendaryEncounterSpecies(wildMonSpecies))
        ApplyLegendaryEncounterSet(&gParties[B_TRAINER_OPPONENT_A][0], ITEM_NONE);
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
    ApplyFluteEncounterRateMod(&encounterRate);
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
    if (gSaveBlock1Ptr->location.mapGroup != MAP_GROUP(MAP_SOOTOPOLIS_CITY)
     || gSaveBlock1Ptr->location.mapNum != MAP_NUM(MAP_SOOTOPOLIS_CITY))
    {
        return FALSE;
    }

    return FlagGet(FLAG_LEGENDARIES_IN_SOOTOPOLIS);
}

// Imported underwater areas may author a full twelve-slot seabed roster.
// Keep its land-slot distribution instead of treating it as a five-slot Surf table.
static bool32 UsesLandEncounterTable(u32 headerId, u16 behavior)
{
    if (MetatileBehavior_IsLandWildEncounter(behavior))
        return TRUE;
    return gMapHeader.mapType == MAP_TYPE_UNDERWATER
        && MetatileBehavior_IsWaterWildEncounter(behavior)
        && GetRouteSignInfo(headerId, WILD_AREA_LAND) != NULL;
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

static void ApplyFluteEncounterRateMod(u32 *encRate)
{
    if (FlagGet(FLAG_SYS_ENC_UP_ITEM) == TRUE)
        *encRate += *encRate / 2;
    else if (FlagGet(FLAG_SYS_ENC_DOWN_ITEM) == TRUE)
        *encRate = *encRate / 2;
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

bool32 MapHasNoEncounterData(void)
{
    return (GetCurrentMapWildMonHeaderId() == HEADER_NONE);
}
