#include "global.h"
#include "data.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "new_game.h"
#include "overworld.h"
#include "script.h"
#include "test/test.h"
#include "constants/maps.h"
#include "constants/map_scripts.h"

TEST("Overworld initial visibility: story actors and capture props wait for their reveal")
{
    static const struct { u16 map; u8 localId; } actors[] = {
        {MAP_RUSTBORO_CITY, 17},
        {MAP_RUSTBORO_CITY_FLAT2_2F, 4},
        {MAP_LAVARIDGE_TOWN_POKEMON_CENTER_1F, 8},
        {MAP_MOSSDEEP_CITY_HOUSE1, 1},
        {MAP_SCORCHED_SLAB_HEATRANS_ROOM, 1},
        {MAP_ASHEN_WOODS, 9},
    };
    NewGameInitData();
    EXPECT_EQ(VarGet(VAR_RUSTBORO_CITY_STATE), 0);
    EXPECT(!FlagGet(FLAG_SYS_GAME_CLEAR));
    for (u32 i = 0; i < ARRAY_COUNT(actors); i++)
    {
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(actors[i].map), MAP_NUM(actors[i].map));
        const struct ObjectEventTemplate *object = FindObjectEventTemplateByLocalId(
            actors[i].localId, map->events->objectEvents, map->events->objectEventCount);
        EXPECT(object != NULL);
        // Read the actual map binding, so stale/renamed initializer flags fail.
        EXPECT_NE(object->flagId, 0);
        EXPECT(FlagGet(object->flagId));
    }
}

TEST("Overworld initial visibility: the Float Stone receipt selects exactly one apartment actor on entry")
{
    u32 received;
    PARAMETRIZE { received = FALSE; }
    PARAMETRIZE { received = TRUE; }
    const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(
        MAP_GROUP(MAP_RUSTBORO_CITY_FLAT2_2F), MAP_NUM(MAP_RUSTBORO_CITY_FLAT2_2F));
    const u8 *entry = map->mapScripts;
    EXPECT_EQ(*entry, MAP_SCRIPT_ON_TRANSITION);
    const u8 *script = (const u8 *)(entry[1] | entry[2] << 8 | entry[3] << 16 | entry[4] << 24);
    const struct ObjectEventTemplate *ace = FindObjectEventTemplateByLocalId(3, map->events->objectEvents, map->events->objectEventCount);
    const struct ObjectEventTemplate *hiker = FindObjectEventTemplateByLocalId(4, map->events->objectEvents, map->events->objectEventCount);
    if (received)
    {
        FlagSet(FLAG_ITEM_RUSTBORO_FLOAT_STONE);
        FlagSet(hiker->flagId);
    }
    else
    {
        FlagClear(FLAG_ITEM_RUSTBORO_FLOAT_STONE);
        FlagClear(hiker->flagId);
    }
    RunScriptImmediately(script);
    EXPECT_EQ(FlagGet(ace->flagId), received);
    EXPECT_EQ(FlagGet(hiker->flagId), !received);
}
