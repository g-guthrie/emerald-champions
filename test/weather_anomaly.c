#include "global.h"
#include "caps.h"
#include "event_data.h"
#include "field_weather.h"
#include "legendary_signs.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "region_map.h"
#include "string_util.h"
#include "text.h"
#include "weather_anomaly.h"
#include "wild_encounter.h"
#include "constants/characters.h"
#include "constants/flags.h"
#include "constants/game_stat.h"
#include "constants/maps.h"
#include "constants/map_types.h"
#include "constants/vars.h"
#include "constants/weather.h"
#include "test/test.h"

static const u16 sCaughtVars[] = {
    VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static const u16 sProgressFlags[] = {
    FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET,
    FLAG_VISITED_FORTREE_CITY, FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE,
    FLAG_RECEIVED_RED_OR_BLUE_ORB, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN,
    FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, FLAG_IS_CHAMPION,
};

// The eight visitors of the Weather Institute rescue (five badges).
static const enum LegendarySignId sFirstVisitors[] = {
    LEGENDARY_SIGN_TAPU_KOKO, LEGENDARY_SIGN_TAPU_BULU, LEGENDARY_SIGN_TAPU_LELE,
    LEGENDARY_SIGN_TORNADUS, LEGENDARY_SIGN_THUNDURUS, LEGENDARY_SIGN_ENAMORUS,
    LEGENDARY_SIGN_ZARUDE, LEGENDARY_SIGN_SUICUNE,
};

static void ResetAnomalyState(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCaughtVars); i++)
        VarSet(sCaughtVars[i], 0);
    for (u32 i = 0; i < ARRAY_COUNT(sProgressFlags); i++)
        FlagClear(sProgressFlags[i]);
    ClearWeatherAnomalies();
    SetGameStat(GAME_STAT_STEPS, 1);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
}

static void SetBadges(u32 count)
{
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
    {
        if (badge < count)
            FlagSet(FLAG_BADGE01_GET + badge);
        else
            FlagClear(FLAG_BADGE01_GET + badge);
    }
}

static void TakeSteps(u32 steps)
{
    for (u32 i = 0; i < steps; i++)
    {
        IncrementGameStat(GAME_STAT_STEPS);
        UpdateWeatherAnomaliesOnStep();
    }
}

// The next step lands on a tick boundary.
static void AlignToTick(void)
{
    u32 stat = GetGameStat(GAME_STAT_STEPS);
    SetGameStat(GAME_STAT_STEPS, stat - stat % WEATHER_ANOMALY_TICK_STEPS + WEATHER_ANOMALY_TICK_STEPS - 1);
}

static void SetLocation(u16 map)
{
    gSaveBlock1Ptr->location.mapGroup = map >> 8;
    gSaveBlock1Ptr->location.mapNum = map & 0xFF;
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(map >> 8, map & 0xFF);
}

static u32 CountLive(void)
{
    u32 live = 0;
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
        live += GetWeatherAnomalySlotSignId(slot) != WEATHER_ANOMALY_EMPTY;
    return live;
}

// No two live anomalies share a visitor or a home map.
static bool32 LiveSlotsAreDistinct(void)
{
    for (u32 a = 0; a < WEATHER_ANOMALY_SLOT_COUNT; a++)
    {
        u8 first = GetWeatherAnomalySlotSignId(a);
        if (first == WEATHER_ANOMALY_EMPTY)
            continue;
        for (u32 b = a + 1; b < WEATHER_ANOMALY_SLOT_COUNT; b++)
        {
            u8 second = GetWeatherAnomalySlotSignId(b);
            if (second == WEATHER_ANOMALY_EMPTY)
                continue;
            if (first == second || gLegendaryGates[first].anomalyMap == gLegendaryGates[second].anomalyMap)
                return FALSE;
        }
    }
    return TRUE;
}

static bool32 IsFirstVisitor(u8 sign)
{
    for (u32 i = 0; i < ARRAY_COUNT(sFirstVisitors); i++)
        if (sFirstVisitors[i] == sign)
            return TRUE;
    return FALSE;
}

static bool32 BufferContains(const u8 *haystack, const u8 *needle)
{
    u32 length = StringLength(haystack);
    u32 needleLength = StringLength(needle);
    for (u32 i = 0; i + needleLength <= length; i++)
        if (StringCompareN(haystack + i, needle, needleLength) == 0)
            return TRUE;
    return FALSE;
}

static void MarkAllVisitorsCaughtExcept(enum LegendarySignId keep)
{
    for (enum LegendarySignId sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
        if (IsWeatherAnomalyVisitor(sign) && sign != keep)
            MarkLegendarySignCaughtBySpecies(gLegendaryGates[sign].species);
}

TEST("Weather anomalies: visitor rows are complete, unique and use only anomaly weathers")
{
    static const enum Species visitors[] = {
        SPECIES_TAPU_KOKO, SPECIES_TAPU_BULU, SPECIES_TAPU_LELE, SPECIES_TORNADUS,
        SPECIES_THUNDURUS, SPECIES_ENAMORUS, SPECIES_ZARUDE, SPECIES_SUICUNE,
        SPECIES_ZEKROM, SPECIES_RESHIRAM, SPECIES_KORAIDON, SPECIES_MIRAIDON,
        SPECIES_XERNEAS, SPECIES_YVELTAL, SPECIES_GIRATINA, SPECIES_CALYREX,
        SPECIES_SPECTRIER, SPECIES_TAPU_FINI, SPECIES_KELDEO, SPECIES_PALKIA,
        SPECIES_MANAPHY, SPECIES_NECROZMA,
    };
    u32 seenIds = 0, count = 0, failures = 0;

    for (u32 i = 0; i < ARRAY_COUNT(visitors); i++)
        EXPECT(IsWeatherAnomalyVisitor(GetLegendarySignIdBySpecies(visitors[i])));
    for (enum LegendarySignId sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
    {
        const struct LegendaryGate *gate = &gLegendaryGates[sign];
        if (gate->anomalyId == 0)
        {
            EXPECT_EQ(gate->anomalyWeather, WEATHER_NONE);
            EXPECT_EQ(gate->anomalyMap, 0);
            continue;
        }
        count++;
        EXPECT_LT(gate->anomalyId, 32);
        EXPECT_EQ(seenIds & (1u << gate->anomalyId), 0);
        seenIds |= 1u << gate->anomalyId;
        EXPECT(gate->anomalyWeather == WEATHER_RAIN || gate->anomalyWeather == WEATHER_RAIN_THUNDERSTORM
            || gate->anomalyWeather == WEATHER_DOWNPOUR || gate->anomalyWeather == WEATHER_FOG_HORIZONTAL);
        EXPECT(gate->anomalyHabitat == WILD_AREA_LAND || gate->anomalyHabitat == WILD_AREA_WATER);
        EXPECT_EQ(gate->requiredSpecies, SPECIES_NONE);
        EXPECT_EQ(gate->kind, LEGENDARY_KIND_WILD);

        // Home map: outdoors, never a town or city, header weather differs,
        // and it has the habitat's table with the visitor's own slot.
        const struct MapHeader *home = Overworld_GetMapHeaderByGroupAndId(gate->anomalyMap >> 8, gate->anomalyMap & 0xFF);
        if (!IsMapTypeOutdoors(home->mapType) || home->mapType == MAP_TYPE_TOWN || home->mapType == MAP_TYPE_CITY
         || home->weather == gate->anomalyWeather)
        {
            Test_MgbaPrintf("Anomaly home: sign %d map %d type %d weather %d", sign, gate->anomalyMap, home->mapType, home->weather);
            failures++;
        }
        bool32 hasTable = FALSE, hasSlot = FALSE;
        for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
        {
            if (gWildMonHeaders[header].mapGroup != (gate->anomalyMap >> 8)
             || gWildMonHeaders[header].mapNum != (gate->anomalyMap & 0xFF))
                continue;
            for (u32 time = 0; time < TIMES_OF_DAY_COUNT; time++)
            {
                const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[time];
                const struct WildPokemonInfo *info = gate->anomalyHabitat == WILD_AREA_LAND ? types->landMonsInfo : types->waterMonsInfo;
                u32 slots = gate->anomalyHabitat == WILD_AREA_LAND ? NUM_LAND_MONS_ENCOUNTER_SLOTS : NUM_WATER_MONS_ENCOUNTER_SLOTS;
                if (info == NULL)
                    continue;
                hasTable = TRUE;
                for (u32 slot = 0; slot < slots; slot++)
                    hasSlot |= info->wildPokemon[slot].species == gate->species;
            }
        }
        if (!hasTable || !hasSlot)
        {
            Test_MgbaPrintf("Anomaly table: sign %d map %d habitat %d table %d slot %d",
                sign, gate->anomalyMap, gate->anomalyHabitat, hasTable, hasSlot);
            failures++;
        }
    }
    EXPECT_EQ(count, ARRAY_COUNT(visitors));
    Test_MgbaPrintf("Anomaly visitors=%d failures=%d", count, failures);
    EXPECT_EQ(failures, 0);

    // Downpour visitors wait for Kyogre's awakening, which precedes the window end.
    EXPECT_EQ(gLegendaryGates[LEGENDARY_SIGN_TAPU_FINI].unlockFlag, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
    EXPECT_EQ(gLegendaryGates[LEGENDARY_SIGN_KELDEO].minimumBadges, 7);
    EXPECT_EQ(gLegendaryGates[LEGENDARY_SIGN_TAPU_KOKO].unlockFlag, FLAG_VISITED_FORTREE_CITY);
    EXPECT_EQ(gLegendaryGates[LEGENDARY_SIGN_KORAIDON].unlockFlag, FLAG_BADGE06_GET);
    EXPECT_EQ(gLegendaryGates[LEGENDARY_SIGN_XERNEAS].unlockFlag, FLAG_RECEIVED_RED_OR_BLUE_ORB);
}

TEST("Weather anomalies: none before the window opens or after it closes")
{
    ResetAnomalyState();
    SetBadges(5);
    TakeSteps(200);
    EXPECT_EQ(CountLive(), 0);
    EXPECT_EQ(VarGet(VAR_WEATHER_ANOMALY_STATE_0) | VarGet(VAR_WEATHER_ANOMALY_STATE_1) | VarGet(VAR_WEATHER_ANOMALY_STATE_2), 0);

    // The first step inside the window fills all four slots.
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    EXPECT(IsWeatherAnomalyWindowOpen());
    EXPECT_EQ(CountLive(), 0);
    TakeSteps(1);
    EXPECT_EQ(CountLive(), WEATHER_ANOMALY_SLOT_COUNT);
    EXPECT(LiveSlotsAreDistinct());
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        EXPECT(IsFirstVisitor(GetWeatherAnomalySlotSignId(slot)));
        // The opening fill is staggered: one slot turns over every quarter cycle.
        EXPECT_EQ(GetWeatherAnomalySlotStepsRemaining(slot),
                  WEATHER_ANOMALY_DURATION_TICKS * (slot + 1) / WEATHER_ANOMALY_SLOT_COUNT * WEATHER_ANOMALY_TICK_STEPS);
    }

    // Once the sky calms, nothing is live, even before the next step, and the
    // save vars are cleared on that step.
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    EXPECT(!IsWeatherAnomalyWindowOpen());
    EXPECT_EQ(CountLive(), 0);
    TakeSteps(1);
    EXPECT_EQ(VarGet(VAR_WEATHER_ANOMALY_STATE_0) | VarGet(VAR_WEATHER_ANOMALY_STATE_1) | VarGet(VAR_WEATHER_ANOMALY_STATE_2), 0);
    TakeSteps(3000);
    EXPECT_EQ(CountLive(), 0);
    ResetAnomalyState();
}

TEST("Weather anomalies: expiry refills at once with a cooldown and never two per map")
{
    ResetAnomalyState();
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    TakeSteps(1);
    u8 first[WEATHER_ANOMALY_SLOT_COUNT];
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
        first[slot] = GetWeatherAnomalySlotSignId(slot);

    // The opening fill is staggered: slot 0 expires after a quarter cycle and
    // is replaced at once by another eligible visitor; the slots always stay
    // full and distinct while the pool has enough visitors.
    const u32 quarter = WEATHER_ANOMALY_DURATION_TICKS / WEATHER_ANOMALY_SLOT_COUNT * WEATHER_ANOMALY_TICK_STEPS;
    u32 steps = 0;
    while (GetWeatherAnomalySlotSignId(0) == first[0] && steps < 2000)
    {
        TakeSteps(1);
        steps++;
        EXPECT_EQ(CountLive(), WEATHER_ANOMALY_SLOT_COUNT);
    }
    EXPECT_GE(steps, quarter - WEATHER_ANOMALY_TICK_STEPS + 1);
    EXPECT_LE(steps, quarter);
    EXPECT_EQ(GetWeatherAnomalyCooldownSignId(), first[0]);
    EXPECT_EQ(CountLive(), WEATHER_ANOMALY_SLOT_COUNT);
    EXPECT(LiveSlotsAreDistinct());
    // By the end of the first full cycle every opening visitor has rotated
    // out and the other four eligible visitors hold the slots.
    while (steps < WEATHER_ANOMALY_DURATION_STEPS)
    {
        TakeSteps(1);
        steps++;
        EXPECT_EQ(CountLive(), WEATHER_ANOMALY_SLOT_COUNT);
    }
    EXPECT(LiveSlotsAreDistinct());
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
    {
        u8 now = GetWeatherAnomalySlotSignId(slot);
        EXPECT(IsFirstVisitor(now));
        for (u32 old = 0; old < WEATHER_ANOMALY_SLOT_COUNT; old++)
            EXPECT_NE(now, first[old]);
    }
    EXPECT_EQ(GetWeatherAnomalyCooldownSignId(), first[WEATHER_ANOMALY_SLOT_COUNT - 1]);

    // Every visitor eligible: shared home maps (Route 118, 121, 123, Mt. Pyre)
    // never host two anomalies, and a lone expiry never redraws itself.
    SetBadges(8);
    FlagSet(FLAG_RECEIVED_RED_OR_BLUE_ORB);
    FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
    u32 seenVisitors = 0;
    for (u32 round = 0; round < 200; round++)
    {
        u32 slot = round % WEATHER_ANOMALY_SLOT_COUNT;
        u8 expiring = GetWeatherAnomalySlotSignId(slot);
        EXPECT_NE(expiring, WEATHER_ANOMALY_EMPTY);
        SetWeatherAnomalySlot(slot, expiring, 1);
        AlignToTick();
        TakeSteps(1);
        u8 refill = GetWeatherAnomalySlotSignId(slot);
        EXPECT_NE(refill, WEATHER_ANOMALY_EMPTY);
        EXPECT_NE(refill, expiring);
        EXPECT_EQ(GetWeatherAnomalyCooldownSignId(), expiring);
        EXPECT_EQ(GetWeatherAnomalySlotStepsRemaining(slot), WEATHER_ANOMALY_DURATION_STEPS);
        EXPECT_EQ(CountLive(), WEATHER_ANOMALY_SLOT_COUNT);
        EXPECT(LiveSlotsAreDistinct());
        seenVisitors |= 1u << gLegendaryGates[refill].anomalyId;
        // Keep the other slots from expiring in this loop.
        for (u32 other = 0; other < WEATHER_ANOMALY_SLOT_COUNT; other++)
            SetWeatherAnomalySlot(other, GetWeatherAnomalySlotSignId(other), WEATHER_ANOMALY_DURATION_STEPS);
    }
    // The uniform draw reaches the late visitors too.
    EXPECT(seenVisitors & (1u << gLegendaryGates[LEGENDARY_SIGN_NECROZMA].anomalyId));
    EXPECT(seenVisitors & (1u << gLegendaryGates[LEGENDARY_SIGN_CALYREX].anomalyId));

    // A caught visitor's anomaly ends on the next step and is replaced.
    u8 caught = GetWeatherAnomalySlotSignId(2);
    MarkLegendarySignCaughtBySpecies(gLegendaryGates[caught].species);
    EXPECT(!IsWeatherAnomalyLive(caught));
    TakeSteps(1);
    EXPECT_EQ(CountLive(), WEATHER_ANOMALY_SLOT_COUNT);
    EXPECT(!IsWeatherAnomalyLive(caught));
    ResetAnomalyState();
}

TEST("Weather anomalies: an empty pool leaves slots empty until a cycle passes")
{
    ResetAnomalyState();
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    MarkAllVisitorsCaughtExcept(LEGENDARY_SIGN_TAPU_KOKO);
    TakeSteps(1);
    EXPECT_EQ(CountLive(), 1);
    EXPECT(IsWeatherAnomalyLive(LEGENDARY_SIGN_TAPU_KOKO));

    // Koko expires: it is on cooldown and nothing else is eligible.
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
        if (GetWeatherAnomalySlotSignId(slot) == LEGENDARY_SIGN_TAPU_KOKO)
            SetWeatherAnomalySlot(slot, LEGENDARY_SIGN_TAPU_KOKO, 1);
    AlignToTick();
    TakeSteps(1);
    EXPECT_EQ(CountLive(), 0);
    EXPECT_EQ(GetWeatherAnomalyCooldownSignId(), LEGENDARY_SIGN_TAPU_KOKO);
    // When an empty slot has waited out its cycle, the cooldown lapses and
    // Koko returns. The opening fill is staggered, so the first empty slot
    // (slot 1) waits half a cycle; never sooner than that, never later.
    const u32 emptyWait = WEATHER_ANOMALY_DURATION_TICKS * 2 / WEATHER_ANOMALY_SLOT_COUNT * WEATHER_ANOMALY_TICK_STEPS;
    u32 waited = 0;
    while (CountLive() == 0 && waited < 2 * WEATHER_ANOMALY_DURATION_STEPS)
    {
        TakeSteps(1);
        waited++;
    }
    Test_MgbaPrintf("Empty-pool wait: %d steps", waited);
    EXPECT_GE(waited, emptyWait - WEATHER_ANOMALY_TICK_STEPS);
    EXPECT_LE(waited, emptyWait);
    EXPECT_EQ(CountLive(), 1);
    EXPECT(IsWeatherAnomalyLive(LEGENDARY_SIGN_TAPU_KOKO));
    EXPECT_EQ(GetWeatherAnomalyCooldownSignId(), WEATHER_ANOMALY_EMPTY);
    ResetAnomalyState();
}

TEST("Weather anomalies: the anomaly weather replaces the header only on its home map")
{
    ResetAnomalyState();
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    struct MapHeader savedHeader = gMapHeader;
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    SetWeatherAnomalySlot(0, LEGENDARY_SIGN_TAPU_KOKO, WEATHER_ANOMALY_DURATION_STEPS);
    SetWeatherAnomalySlot(1, LEGENDARY_SIGN_TAPU_FINI, WEATHER_ANOMALY_DURATION_STEPS); // Gate closed: never live.

    u8 route110Weather = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110))->weather;
    EXPECT_NE(route110Weather, WEATHER_RAIN);
    SetLocation(MAP_ROUTE110);
    EXPECT_EQ(GetWeatherAnomalyWeatherForCurrentMap(), WEATHER_RAIN);
    SetSavedWeatherFromCurrMapHeader();
    EXPECT_EQ(GetSavedWeather(), WEATHER_RAIN);
    // The header is never rewritten.
    EXPECT_EQ(Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ROUTE110), MAP_NUM(MAP_ROUTE110))->weather, route110Weather);
    EXPECT_EQ(gMapHeader.weather, route110Weather);

    // Scripted sky weather on the home map (a transition's setweather, a
    // cloud/sun trigger) cannot clear the storm; terrain weather (desert
    // sandstorm, volcanic ash) always wins; off the home map all apply as usual.
    SetSavedWeather(WEATHER_SUNNY);
    EXPECT_EQ(GetSavedWeather(), WEATHER_RAIN);
    SetSavedWeather(WEATHER_RAIN_THUNDERSTORM);
    EXPECT_EQ(GetSavedWeather(), WEATHER_RAIN);
    SetSavedWeather(WEATHER_SANDSTORM);
    EXPECT_EQ(GetSavedWeather(), WEATHER_SANDSTORM);
    SetSavedWeather(WEATHER_VOLCANIC_ASH);
    EXPECT_EQ(GetSavedWeather(), WEATHER_VOLCANIC_ASH);
    SetSavedWeatherFromCurrMapHeader();
    EXPECT_EQ(GetSavedWeather(), WEATHER_RAIN);

    // Another map keeps its header weather.
    SetLocation(MAP_ROUTE111);
    SetSavedWeather(WEATHER_SANDSTORM);
    EXPECT_EQ(GetSavedWeather(), WEATHER_SANDSTORM);
    EXPECT_EQ(GetWeatherAnomalyWeatherForCurrentMap(), WEATHER_NONE);
    SetSavedWeatherFromCurrMapHeader();
    EXPECT_EQ(GetSavedWeather(), gMapHeader.weather);
    SetLocation(MAP_ROUTE126);
    EXPECT_EQ(GetWeatherAnomalyWeatherForCurrentMap(), WEATHER_NONE);

    // Back on Route 110 after the window closes: the header weather again.
    SetLocation(MAP_ROUTE110);
    SetSavedWeatherFromCurrMapHeader();
    EXPECT_EQ(GetSavedWeather(), WEATHER_RAIN);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    EXPECT_EQ(GetWeatherAnomalyWeatherForCurrentMap(), WEATHER_NONE);
    SetSavedWeatherFromCurrMapHeader();
    EXPECT_EQ(GetSavedWeather(), route110Weather);

    gSaveBlock1Ptr->location = savedLocation;
    gMapHeader = savedHeader;
    ResetAnomalyState();
}

TEST("Weather anomalies: the visitor takes a quarter of encounters on its own map only")
{
    ResetAnomalyState();
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    SetWeatherAnomalySlot(0, LEGENDARY_SIGN_TAPU_KOKO, WEATHER_ANOMALY_DURATION_STEPS);
    SetWeatherAnomalySlot(1, LEGENDARY_SIGN_SUICUNE, WEATHER_ANOMALY_DURATION_STEPS);

    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i] = (struct WildPokemon){5, 5, SPECIES_ZIGZAGOON};
    const struct WildPokemonInfo table = {.encounterRate = 20, .wildPokemon = mons};
    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];

    // Route 110 grass: about 25%, at the cap.
    SetLocation(MAP_ROUTE110);
    u32 koko = 0;
    for (u32 seed = 0; seed < 1000; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&table, WILD_AREA_LAND, 0));
        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_TAPU_KOKO)
        {
            koko++;
            EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
        }
    }
    Test_MgbaPrintf("Tapu Koko anomaly encounters: %d/1000", koko);
    EXPECT_GT(koko, 200);
    EXPECT_LT(koko, 300);

    // Surf, Rock Smash and Honey on Route 110 never meet the land visitor.
    for (u32 seed = 0; seed < 200; seed++)
    {
        static const enum WildPokemonArea others[] = {WILD_AREA_WATER, WILD_AREA_ROCKS, WILD_AREA_HONEY};
        for (u32 i = 0; i < ARRAY_COUNT(others); i++)
        {
            SeedRng(seed);
            EXPECT(TryGenerateWildMon(&table, others[i], 0));
            EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        }
    }

    // Suicune is a Surf visitor on Route 125: land encounters there never meet it.
    SetLocation(MAP_ROUTE125);
    u32 suicune = 0;
    for (u32 seed = 0; seed < 500; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&table, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&table, WILD_AREA_WATER, 0));
        suicune += GetMonData(mon, MON_DATA_SPECIES) == SPECIES_SUICUNE;
    }
    EXPECT_GT(suicune, 0);

    // Any other map: never.
    SetLocation(MAP_ROUTE111);
    for (u32 seed = 0; seed < 500; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&table, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    }
    // Caught: its anomaly no longer yields it on its own map either.
    SetLocation(MAP_ROUTE110);
    MarkLegendarySignCaughtBySpecies(SPECIES_TAPU_KOKO);
    for (u32 seed = 0; seed < 200; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&table, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    }
    // Window closed: no anomaly roll at all.
    ResetAnomalyState();
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    SetWeatherAnomalySlot(0, LEGENDARY_SIGN_TAPU_KOKO, WEATHER_ANOMALY_DURATION_STEPS);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    for (u32 seed = 0; seed < 200; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&table, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    }

    gSaveBlock1Ptr->location = savedLocation;
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
    ResetAnomalyState();
}

TEST("Weather anomalies: a visitor's own slot is inert until the window closes, then a resident")
{
    ResetAnomalyState();
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    SetLocation(MAP_ROUTE111);
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i] = (struct WildPokemon){5, 5, SPECIES_ZIGZAGOON};
    mons[NUM_LAND_MONS_ENCOUNTER_SLOTS - 1].species = SPECIES_TAPU_KOKO;
    const struct WildPokemonInfo land = {.encounterRate = 20, .wildPokemon = mons};

    // Window open, gate met, not live here: the slot rerolls.
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_TAPU_KOKO));
    EXPECT(IsWeatherAnomalyVisitorSlotInert(SPECIES_TAPU_KOKO));
    for (u32 seed = 0; seed < 512; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&land, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    }
    // Sweet Scent gives the inert visitor nothing (no fivefold boost).
    static const u8 onePercent[] = {60, 90, 99, 100};
    struct WildPokemon water[NUM_WATER_MONS_ENCOUNTER_SLOTS] = {
        {5, 5, SPECIES_MAGIKARP}, {5, 5, SPECIES_GOLDEEN}, {5, 5, SPECIES_TENTACOOL}, {5, 5, SPECIES_SUICUNE},
    };
    struct WildPokemonInfo scent = {.wildPokemon = water, .encounterBounds = onePercent};
    u32 hits = 0;
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        hits += ChooseSweetScentWildMonIndex(&scent, WILD_AREA_WATER) == 3;
    }
    EXPECT_EQ(hits, 0);

    // After the window: an ordinary gated 1% resident, which Sweet Scent
    // draws out at a storm's share.
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    EXPECT(!IsWeatherAnomalyVisitorSlotInert(SPECIES_TAPU_KOKO));
    hits = 0;
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        hits += ChooseSweetScentWildMonIndex(&scent, WILD_AREA_WATER) == 3;
    }
    EXPECT_EQ(hits, 25);
    ResetAnomalyState();
    gSaveBlock1Ptr->location = savedLocation;
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
}

TEST("Weather anomalies: the resident visitor slot rolls like any legend after the window")
{
    ResetAnomalyState();
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    SetLocation(MAP_ROUTE110);
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i] = (struct WildPokemon){5, 5, SPECIES_ZIGZAGOON};
    mons[NUM_LAND_MONS_ENCOUNTER_SLOTS - 1].species = SPECIES_TAPU_KOKO;
    const struct WildPokemonInfo land = {.encounterRate = 20, .wildPokemon = mons};
    u32 koko = 0;
    for (u32 seed = 0; seed < 512; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&land, WILD_AREA_LAND, 0));
        if (GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES) == SPECIES_TAPU_KOKO)
        {
            koko++;
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), GetCurrentLevelCap());
        }
    }
    // A default 4% slot, boosted x5 (capped +20 points) like any resident legend.
    EXPECT_GT(koko, 64);
    EXPECT_LT(koko, 160);
    gSaveBlock1Ptr->location = savedLocation;
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
    ResetAnomalyState();
}

TEST("Weather anomalies: the Institute report lists each live anomaly and counts them")
{
    ResetAnomalyState();
    SetBadges(5);
    BufferWeatherAnomalyReport();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(gStringVar4[0], EOS);

    FlagSet(FLAG_VISITED_FORTREE_CITY);
    // The first report inside the window draws the storms itself, so the
    // scientist is never "quiet" for the one step before the first draw.
    BufferWeatherAnomalyReport();
    EXPECT_EQ(gSpecialVar_Result, 4);
    TakeSteps(1);
    BufferWeatherAnomalyReport();
    EXPECT_EQ(gSpecialVar_Result, 4);
    for (u32 slot = 0; slot < WEATHER_ANOMALY_SLOT_COUNT; slot++)
        EXPECT(BufferContains(gStringVar4, GetLegendaryDisplayName(gLegendaryGates[GetWeatherAnomalySlotSignId(slot)].species)));
    u32 breaks = 0;
    for (u32 i = 0; gStringVar4[i] != EOS; i++)
        breaks += gStringVar4[i] == CHAR_NEWLINE || gStringVar4[i] == CHAR_PROMPT_SCROLL;
    EXPECT_EQ(breaks, 3);
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("%")));

    ClearWeatherAnomalies();
    SetWeatherAnomalySlot(2, LEGENDARY_SIGN_TAPU_KOKO, WEATHER_ANOMALY_DURATION_STEPS);
    SetWeatherAnomalySlot(3, LEGENDARY_SIGN_SUICUNE, WEATHER_ANOMALY_DURATION_STEPS);
    BufferWeatherAnomalyReport();
    EXPECT_EQ(gSpecialVar_Result, 2);
    u8 expected[64];
    // Map names print in title case, not the region map's capitals.
    u8 *end = StringCopy(expected, COMPOUND_STRING("Route 110: "));
    StringCopy(end, GetSpeciesName(SPECIES_TAPU_KOKO));
    EXPECT(BufferContains(gStringVar4, expected));
    EXPECT(GetStringWidth(FONT_NORMAL, expected, 0) <= 200);

    MarkLegendarySignCaughtBySpecies(SPECIES_SUICUNE);
    BufferWeatherAnomalyReport();
    EXPECT_EQ(gSpecialVar_Result, 1);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    BufferWeatherAnomalyReport();
    EXPECT_EQ(gSpecialVar_Result, 0);
    ResetAnomalyState();
}

TEST("Weather anomalies: research, Center leads and the route sign follow the storms")
{
    ResetAnomalyState();
    struct WarpData savedLocation = gSaveBlock1Ptr->location;
    SetBadges(5);
    FlagSet(FLAG_VISITED_FORTREE_CITY);

    gSpecialVar_0x8004 = LEGENDARY_SIGN_TAPU_KOKO;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("It rides the weather anomalies.")));
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("Available now!")));

    SetWeatherAnomalySlot(0, LEGENDARY_SIGN_TAPU_KOKO, WEATHER_ANOMALY_DURATION_STEPS);
    ResearchSelectedLegendarySign();
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("It rides the weather anomalies.")));
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Available now!")));

    // The live anomaly adds one page to its route sign.
    SetLocation(MAP_ROUTE110);
    BufferCurrentMapRouteSignSpecies();
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING(" has been sighted in\nthis weather.")));
    SetLocation(MAP_ROUTE111);
    BufferCurrentMapRouteSignSpecies();
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING(" has been sighted in")));

    // Gate not met yet, window open: requirements, then the storms line.
    gSpecialVar_0x8004 = LEGENDARY_SIGN_TAPU_FINI;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Seafloor Cavern")));
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("It rides the weather anomalies.")));

    // Window closed: the ordinary resident text.
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_TAPU_KOKO;
    ResearchSelectedLegendarySign();
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("It rides the weather anomalies.")));
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Available now!")));

    gSaveBlock1Ptr->location = savedLocation;
    ResetAnomalyState();
}
