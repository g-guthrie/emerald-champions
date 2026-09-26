#include "global.h"
#include "data.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "sprite.h"
#include "test/test.h"
#include "constants/event_objects.h"
#include "constants/map_event_ids.h"
#include "constants/maps.h"

TEST("Overworld metadata: story actor IDs select the correct map objects")
{
    static const struct { u16 map; u8 localId; u16 gfx; } actors[] = {
        {MAP_PETALBURG_CITY, LOCALID_PETALBURG_WALLY, OBJ_EVENT_GFX_WALLY},
        {MAP_PETALBURG_CITY, LOCALID_PETALBURG_WALLYS_DAD, OBJ_EVENT_GFX_POKEFAN_M},
        {MAP_MT_PYRE_SUMMIT, LOCALID_MT_PYRE_SUMMIT_ARCHIE, OBJ_EVENT_GFX_ARCHIE},
        {MAP_MT_PYRE_SUMMIT, LOCALID_MT_PYRE_SUMMIT_MAXIE, OBJ_EVENT_GFX_MAXIE},
        {MAP_SEAFLOOR_CAVERN_ROOM9, LOCALID_SEAFLOOR_CAVERN_ARCHIE, OBJ_EVENT_GFX_ARCHIE},
        {MAP_SEAFLOOR_CAVERN_ROOM9, LOCALID_SEAFLOOR_CAVERN_MAXIE, OBJ_EVENT_GFX_MAXIE},
        {MAP_SEAFLOOR_CAVERN_ROOM9, LOCALID_SEAFLOOR_CAVERN_KYOGRE_SLEEPING, OBJ_EVENT_GFX_KYOGRE_ASLEEP},
        {MAP_EVER_GRANDE_CITY_CHAMPIONS_ROOM, LOCALID_CHAMPIONS_ROOM_WALLACE, OBJ_EVENT_GFX_WALLACE},
        {MAP_EVER_GRANDE_CITY_CHAMPIONS_ROOM, LOCALID_CHAMPIONS_ROOM_RIVAL, OBJ_EVENT_GFX_VAR_0},
        {MAP_EVER_GRANDE_CITY_CHAMPIONS_ROOM, LOCALID_CHAMPIONS_ROOM_BIRCH, OBJ_EVENT_GFX_PROF_BIRCH},
    };
    for (u32 i = 0; i < ARRAY_COUNT(actors); i++)
    {
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(actors[i].map), MAP_NUM(actors[i].map));
        const struct ObjectEventTemplate *object = FindObjectEventTemplateByLocalId(
            actors[i].localId, map->events->objectEvents, map->events->objectEventCount);
        EXPECT(object != NULL);
        EXPECT_EQ(object->graphicsId, actors[i].gfx);
    }
}

TEST("Overworld metadata: every mapped static graphics ID has usable sprite data")
{
    for (u32 id = 0; id < NUM_OBJ_EVENT_GFX; id++)
    {
        if (id == OBJ_EVENT_GFX_UNUSED_250)
            continue;
        // This contiguous bank held FireRed/LeafGreen graphics; the ids stay reserved without data.
        if (id >= OBJ_EVENT_GFX_RED_NORMAL && id <= OBJ_EVENT_GFX_BREAKABLE_ROCK_FRLG)
            continue;
        const struct ObjectEventGraphicsInfo *info = GetObjectEventGraphicsInfo(id);
        EXPECT(info != NULL);
        EXPECT_GT(info->size, 0);
        EXPECT_GT(info->width, 0);
        EXPECT_GT(info->height, 0);
        EXPECT(info->oam != NULL);
        EXPECT_LT((u32)info->oam->shape, 3);
        // OW_MON is an unmapped base template; species graphics are checked below.
        if (id == OBJ_EVENT_GFX_OW_MON)
        {
            EXPECT(info->images == NULL);
            continue;
        }
        EXPECT(info->images != NULL);
        EXPECT(info->images[0].data != NULL);
        EXPECT(info->anims != NULL);
        // Player sprites may reserve extra space for bikes; composites use
        // subsprites. Neither requires equality with width * height / 2.
        EXPECT_GE(info->size, info->images[0].size);
    }
}

TEST("Overworld metadata: defined species graphics resolve normal, shiny and female variants")
{
#if OW_POKEMON_OBJECT_EVENTS
    u32 checked = 0;
    for (u32 species = 1; species < NUM_SPECIES; species++)
    {
        if (gSpeciesInfo[species].overworldData.tileTag != TAG_NONE)
            continue; // Battle-only forms without overworld art are not map IDs.
        for (u32 variant = 0; variant < 4; variant++)
        {
            const struct ObjectEventGraphicsInfo *info = SpeciesToGraphicsInfo(species, variant & 1, variant & 2);
            EXPECT(info != NULL);
            EXPECT_GT(info->size, 0);
            EXPECT_GT(info->width, 0);
            EXPECT_GT(info->height, 0);
            EXPECT(info->oam != NULL);
            EXPECT(info->images != NULL);
            EXPECT(info->images[0].data != NULL);
            EXPECT(info->anims != NULL);
            checked++;
        }
    }
    EXPECT_GT(checked, 0);
#endif
}
