#include "global.h"
#include "event_data.h"
#include "legendary_signs.h"
#include "overworld.h"
#include "random.h"
#include "region_map.h"
#include "string_util.h"
#include "weather_anomaly.h"
#include "wild_encounter.h"
#include "constants/characters.h"
#include "constants/game_stat.h"
#include "constants/maps.h"
#include "constants/vars.h"
#include "constants/weather.h"

// Weather anomalies (Emerald Champions).
//
// Window: FLAG_HIDE_ROUTE_119_TEAM_AQUA set and FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE
// clear. Inside it, four slots each hold one visiting legend (a gate row with
// an anomalyId) for WEATHER_ANOMALY_DURATION_STEPS steps. An expired slot is
// refilled at once by a uniform draw over visitors whose gate is met, who are
// uncaught, not live in another slot, not the one that just expired, and whose
// home map hosts no other live anomaly. A failed draw leaves the slot empty for
// one full cycle, after which it draws again. Leaving the window clears all.
//
// Save layout: 48 bits over VAR_WEATHER_ANOMALY_STATE_0..2 (little-endian).
//   bits 10*i .. 10*i+4 : slot i visitor save id (gate anomalyId, 0 = empty)
//   bits 10*i+5 .. +9   : slot i ticks remaining (1 tick = 50 steps, 30 = 1500)
//   bits 40..44         : cooldown visitor save id (0 = none)
// Ticks fall on steps where GAME_STAT_STEPS is a multiple of 50, so a fresh
// anomaly lasts 1451-1500 steps.

#define SLOT_BITS       10
#define ID_BITS         5
#define ID_MASK         ((1u << ID_BITS) - 1)
#define TICK_MASK       ((1u << (SLOT_BITS - ID_BITS)) - 1)
#define COOLDOWN_SHIFT  (SLOT_BITS * WEATHER_ANOMALY_SLOT_COUNT)

STATIC_ASSERT(WEATHER_ANOMALY_DURATION_TICKS <= TICK_MASK, WeatherAnomalyTicksFit);
STATIC_ASSERT(COOLDOWN_SHIFT + ID_BITS <= 48, WeatherAnomalyStateFits);

static u64 LoadState(void)
{
    return (u64)VarGet(VAR_WEATHER_ANOMALY_STATE_0)
         | ((u64)VarGet(VAR_WEATHER_ANOMALY_STATE_1) << 16)
         | ((u64)VarGet(VAR_WEATHER_ANOMALY_STATE_2) << 32);
}

static void StoreState(u64 state)
{
    VarSet(VAR_WEATHER_ANOMALY_STATE_0, state & 0xFFFF);
    VarSet(VAR_WEATHER_ANOMALY_STATE_1, (state >> 16) & 0xFFFF);
    VarSet(VAR_WEATHER_ANOMALY_STATE_2, (state >> 32) & 0xFFFF);
}

static u32 SlotId(u64 state, u32 slot)
{
    return (state >> (slot * SLOT_BITS)) & ID_MASK;
}

static u32 SlotTicks(u64 state, u32 slot)
{
    return (state >> (slot * SLOT_BITS + ID_BITS)) & TICK_MASK;
}

static u64 SetSlot(u64 state, u32 slot, u32 id, u32 ticks)
{
    u32 shift = slot * SLOT_BITS;
    state &= ~((u64)((1u << SLOT_BITS) - 1) << shift);
    return state | ((u64)((id & ID_MASK) | ((ticks & TICK_MASK) << ID_BITS)) << shift);
}

static u32 CooldownId(u64 state)
{
    return (state >> COOLDOWN_SHIFT) & ID_MASK;
}

static u64 SetCooldown(u64 state, u32 id)
{
    state &= ~((u64)ID_MASK << COOLDOWN_SHIFT);
    return state | ((u64)(id & ID_MASK) << COOLDOWN_SHIFT);
}

static enum LegendarySignId SignFromAnomalyId(u32 id)
{
    if (id == 0)
        return LEGENDARY_SIGN_COUNT;
    for (enum LegendarySignId sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
        if (gLegendaryGates[sign].anomalyId == id)
            return sign;
    return LEGENDARY_SIGN_COUNT;
}

bool32 IsWeatherAnomalyWindowOpen(void)
{
    return FlagGet(WEATHER_ANOMALY_WINDOW_START_FLAG) && !FlagGet(WEATHER_ANOMALY_WINDOW_END_FLAG);
}

bool32 IsWeatherAnomalyVisitor(enum LegendarySignId signId)
{
    return signId < LEGENDARY_SIGN_COUNT && gLegendaryGates[signId].anomalyId != 0;
}

static bool32 IsVisitorAvailable(enum LegendarySignId sign)
{
    return IsWeatherAnomalyVisitor(sign) && CanAcquireLegendarySignSpecies(gLegendaryGates[sign].species);
}

// The live visitor of a slot: counting down, gate met and still uncaught.
static enum LegendarySignId LiveSlotSign(u64 state, u32 slot)
{
    enum LegendarySignId sign;

    if (SlotTicks(state, slot) == 0)
        return LEGENDARY_SIGN_COUNT;
    sign = SignFromAnomalyId(SlotId(state, slot));
    if (!IsVisitorAvailable(sign))
        return LEGENDARY_SIGN_COUNT;
    return sign;
}

static bool32 IsMapHostingLiveSlot(u64 state, u16 map)
{
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        enum LegendarySignId live = LiveSlotSign(state, slot);
        if (live < LEGENDARY_SIGN_COUNT && gLegendaryGates[live].anomalyMap == map)
            return TRUE;
    }
    return FALSE;
}

static bool32 IsDrawEligible(u64 state, enum LegendarySignId sign, u32 excluded)
{
    u32 id = gLegendaryGates[sign].anomalyId;

    if (id == 0 || (excluded & (1u << id)) || !IsVisitorAvailable(sign))
        return FALSE;
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
        if (SlotTicks(state, slot) != 0 && SlotId(state, slot) == id)
            return FALSE;
    return !IsMapHostingLiveSlot(state, gLegendaryGates[sign].anomalyMap);
}

// Uniform draw over the eligible pool; 0 when the pool is empty.
static u32 DrawVisitor(u64 state, u32 excluded)
{
    u32 count = 0, pick;

    for (enum LegendarySignId sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
        count += IsDrawEligible(state, sign, excluded);
    if (count == 0)
        return 0;
    pick = Random() % count;
    for (enum LegendarySignId sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
    {
        if (!IsDrawEligible(state, sign, excluded))
            continue;
        if (pick-- == 0)
            return gLegendaryGates[sign].anomalyId;
    }
    return 0;
}

void UpdateWeatherAnomaliesOnStep(void)
{
    u64 state = LoadState();
    u32 expiredSlots = 0, expiredIds = 0, newCooldown = 0, excluded;
    bool32 tick, cooldownLapsed = FALSE;

    if (!IsWeatherAnomalyWindowOpen())
    {
        if (state != 0)
            StoreState(0);
        return;
    }

    tick = GetGameStat(GAME_STAT_STEPS) % WEATHER_ANOMALY_TICK_STEPS == 0;
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        u32 id = SlotId(state, slot);
        u32 ticks = SlotTicks(state, slot);
        enum LegendarySignId sign = SignFromAnomalyId(id);

        if (sign >= LEGENDARY_SIGN_COUNT)
            id = 0;
        if (ticks != 0 && tick)
            ticks--;
        if (id != 0 && IsLegendarySignCaught(sign))
        {
            // A caught visitor's anomaly ends at once; no cooldown is needed.
            expiredSlots |= 1u << slot;
            state = SetSlot(state, slot, 0, 0);
            continue;
        }
        if (ticks != 0)
        {
            state = SetSlot(state, slot, id, ticks);
            continue;
        }
        expiredSlots |= 1u << slot;
        if (id != 0)
        {
            // One-cycle cooldown: every visitor expiring on this step stays
            // out of these refills, and the last one is saved for later draws.
            expiredIds |= 1u << id;
            newCooldown = id;
        }
        else
        {
            // An empty slot has waited out a full cycle: the saved cooldown lapses.
            cooldownLapsed = TRUE;
        }
        state = SetSlot(state, slot, 0, 0);
    }
    if (expiredSlots == 0)
    {
        StoreState(state);
        return;
    }
    excluded = expiredIds;
    if (!cooldownLapsed && CooldownId(state) != 0)
        excluded |= 1u << CooldownId(state);
    if (newCooldown != 0)
        state = SetCooldown(state, newCooldown);
    else if (cooldownLapsed)
        state = SetCooldown(state, 0);
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        // A failed draw leaves the slot empty for one full cycle.
        if (expiredSlots & (1u << slot))
            state = SetSlot(state, slot, DrawVisitor(state, excluded), WEATHER_ANOMALY_DURATION_TICKS);
    }
    StoreState(state);
}

u8 GetWeatherAnomalySlotSignId(u32 slot)
{
    enum LegendarySignId sign;

    if (slot >= WEATHER_ANOMALY_SLOT_COUNT || !IsWeatherAnomalyWindowOpen())
        return WEATHER_ANOMALY_EMPTY;
    sign = LiveSlotSign(LoadState(), slot);
    return sign < LEGENDARY_SIGN_COUNT ? sign : WEATHER_ANOMALY_EMPTY;
}

u16 GetWeatherAnomalySlotStepsRemaining(u32 slot)
{
    if (GetWeatherAnomalySlotSignId(slot) == WEATHER_ANOMALY_EMPTY)
        return 0;
    return SlotTicks(LoadState(), slot) * WEATHER_ANOMALY_TICK_STEPS;
}

void SetWeatherAnomalySlot(u32 slot, u8 signId, u16 steps)
{
    u32 ticks = (steps + WEATHER_ANOMALY_TICK_STEPS - 1) / WEATHER_ANOMALY_TICK_STEPS;
    u32 id = IsWeatherAnomalyVisitor(signId) ? gLegendaryGates[signId].anomalyId : 0;

    if (slot >= WEATHER_ANOMALY_SLOT_COUNT)
        return;
    StoreState(SetSlot(LoadState(), slot, id, min(ticks, TICK_MASK)));
}

u8 GetWeatherAnomalyCooldownSignId(void)
{
    enum LegendarySignId sign = SignFromAnomalyId(CooldownId(LoadState()));
    return sign < LEGENDARY_SIGN_COUNT ? sign : WEATHER_ANOMALY_EMPTY;
}

void ClearWeatherAnomalies(void)
{
    StoreState(0);
}

bool32 IsWeatherAnomalyLive(enum LegendarySignId signId)
{
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
        if (GetWeatherAnomalySlotSignId(slot) == signId)
            return TRUE;
    return FALSE;
}

enum LegendarySignId GetLiveWeatherAnomalyOnMap(u8 mapGroup, u8 mapNum)
{
    u16 map = (mapGroup << 8) | mapNum;

    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        u8 sign = GetWeatherAnomalySlotSignId(slot);
        if (sign != WEATHER_ANOMALY_EMPTY && gLegendaryGates[sign].anomalyMap == map)
            return sign;
    }
    return LEGENDARY_SIGN_COUNT;
}

static bool32 IsAllowedAnomalyWeather(u8 weather)
{
    return weather == WEATHER_RAIN || weather == WEATHER_RAIN_THUNDERSTORM
        || weather == WEATHER_DOWNPOUR || weather == WEATHER_FOG_HORIZONTAL;
}

u8 GetWeatherAnomalyWeatherForCurrentMap(void)
{
    enum LegendarySignId sign = GetLiveWeatherAnomalyOnMap(gSaveBlock1Ptr->location.mapGroup,
                                                           gSaveBlock1Ptr->location.mapNum);

    if (sign >= LEGENDARY_SIGN_COUNT || !IsAllowedAnomalyWeather(gLegendaryGates[sign].anomalyWeather))
        return WEATHER_NONE;
    return gLegendaryGates[sign].anomalyWeather;
}

// Before and during the window a visitor is met only through its anomaly; its
// table slot rerolls like a gated slot. After the window it is a resident.
bool32 IsWeatherAnomalyVisitorSlotInert(enum Species species)
{
    return IsWeatherAnomalyVisitor(GetLegendarySignIdBySpecies(species))
        && !FlagGet(WEATHER_ANOMALY_WINDOW_END_FLAG);
}

enum Species TryRollWeatherAnomalyEncounter(enum WildPokemonArea area)
{
    enum LegendarySignId sign;

    if (area != WILD_AREA_LAND && area != WILD_AREA_WATER)
        return SPECIES_NONE;
    sign = GetLiveWeatherAnomalyOnMap(gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
    if (sign >= LEGENDARY_SIGN_COUNT || gLegendaryGates[sign].anomalyHabitat != area
     || !CanAcquireLegendarySignSpecies(gLegendaryGates[sign].species))
        return SPECIES_NONE;
    if (Random() % 100 >= WEATHER_ANOMALY_ENCOUNTER_PERCENT)
        return SPECIES_NONE;
    return gLegendaryGates[sign].species;
}

void BufferWeatherAnomalyReport(void)
{
    u8 *dest = gStringVar4;
    u32 count = 0;

    *dest = EOS;
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        u8 sign = GetWeatherAnomalySlotSignId(slot);
        u16 map;

        if (sign == WEATHER_ANOMALY_EMPTY)
            continue;
        map = gLegendaryGates[sign].anomalyMap;
        if (count == 1)
            dest = StringCopy(dest, COMPOUND_STRING("\n"));
        else if (count > 1)
            dest = StringCopy(dest, COMPOUND_STRING("\l"));
        dest = GetMapName(dest, Overworld_GetMapHeaderByGroupAndId(map >> 8, map & 0xFF)->regionMapSectionId, 0);
        dest = StringCopy(dest, COMPOUND_STRING(": "));
        dest = StringCopy(dest, GetLegendaryDisplayName(gLegendaryGates[sign].species));
        count++;
    }
    gSpecialVar_Result = count;
}
