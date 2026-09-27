#include "global.h"
#include "test/battle.h"
#include "event_data.h"
#include "field_weather.h"
#include "overworld.h"
#include "weather_anomaly.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/vars.h"
#include "constants/weather.h"

// Enter battle from the actual visitor's map weather, rather than injecting
// gBattleWeather or terrain. The runner replays even wild battles, so these
// tests explicitly opt into field weather while exercising the real handoff.
static void PrepareStorm(enum LegendarySignId sign)
{
    UseOverworldWeather();
    for (u32 i = 0; i < 8; i++)
        FlagSet(FLAG_BADGE01_GET + i);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    FlagSet(FLAG_RECEIVED_RED_OR_BLUE_ORB);
    FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    const u16 caught[] = {VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5};
    for (u32 i = 0; i < ARRAY_COUNT(caught); i++)
        VarSet(caught[i], 0);
    ClearWeatherAnomalies();
    if (sign < LEGENDARY_SIGN_COUNT)
    {
        u16 map = gLegendaryGates[sign].anomalyMap;
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(map);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(map);
        gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(map), MAP_NUM(map));
        SetWeatherAnomalySlot(0, sign, 1500);
        SetSavedWeatherFromCurrMapHeader();
        EXPECT_EQ(GetSavedWeather(), gLegendaryGates[sign].anomalyWeather);
        SetCurrentAndNextWeather(GetSavedWeather());
    }
    else
        SetCurrentAndNextWeather(WEATHER_SUNNY);
}

WILD_BATTLE_TEST("Weather anomalies battle: rain, thunderstorm, fog and downpour initialize their real effects")
{
    enum LegendarySignId sign;
    u32 weather, terrain;
    PARAMETRIZE { sign = LEGENDARY_SIGN_TAPU_KOKO; weather = B_WEATHER_RAIN_NORMAL; terrain = B_TERRAIN_NONE; }
    PARAMETRIZE { sign = LEGENDARY_SIGN_TORNADUS; weather = B_WEATHER_RAIN_NORMAL; terrain = B_TERRAIN_ELECTRIC; }
    PARAMETRIZE { sign = LEGENDARY_SIGN_YVELTAL; weather = B_WEATHER_NONE; terrain = B_TERRAIN_MISTY; }
    PARAMETRIZE { sign = LEGENDARY_SIGN_NECROZMA; weather = B_WEATHER_RAIN_NORMAL; terrain = B_TERRAIN_NONE; }
    GIVEN {
        PrepareStorm(sign);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleWeather, weather);
        EXPECT_EQ(gFieldTimers.terrain, terrain);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
        EXPECT(!gBattleStruct->overworldWeatherPresent);
        ClearWeatherAnomalies();
        SetCurrentAndNextWeather(WEATHER_NONE);
    }
}

WILD_BATTLE_TEST("Weather anomalies battle: rain boosts Water damage by half", s16 damage)
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        PrepareStorm(rain ? LEGENDARY_SIGN_TAPU_KOKO : LEGENDARY_SIGN_COUNT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        ClearWeatherAnomalies();
        SetCurrentAndNextWeather(WEATHER_NONE);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

WILD_BATTLE_TEST("Weather anomalies battle: rain Thunder bypasses a failing accuracy roll")
{
    GIVEN {
        PrepareStorm(LEGENDARY_SIGN_TAPU_KOKO);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        ClearWeatherAnomalies();
        SetCurrentAndNextWeather(WEATHER_NONE);
    }
}

WILD_BATTLE_TEST("Weather anomalies battle: fog Misty Terrain stops grounded status but permits airborne status")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_PIDOVE; }
    GIVEN {
        PrepareStorm(LEGENDARY_SIGN_YVELTAL);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); }
    } THEN {
        EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_MISTY);
        EXPECT_EQ(opponent->status1 & STATUS1_PARALYSIS, species == SPECIES_PIDOVE ? STATUS1_PARALYSIS : 0);
        EXPECT_EQ(gBattleWeather & B_WEATHER_FOG, 0);
        ClearWeatherAnomalies();
        SetCurrentAndNextWeather(WEATHER_NONE);
    }
}

WILD_BATTLE_TEST("Weather anomalies battle: battle weather can replace storm rain while its terrain remains")
{
    GIVEN {
        PrepareStorm(LEGENDARY_SIGN_TORNADUS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_ELECTRIC);
        EXPECT_EQ(gFieldTimers.terrainTimer, 0);
        ClearWeatherAnomalies();
        SetCurrentAndNextWeather(WEATHER_NONE);
    }
}
