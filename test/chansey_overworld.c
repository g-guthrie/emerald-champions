#include "global.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "constants/maps.h"
#include "constants/map_event_ids.h"
#include "test/test.h"

// Check the actual compiled map templates and renderer consumer. A valid enum
// name can still address a null slot in the graphics table.
TEST("Emerald Chansey quest map objects resolve renderable graphics")
{
    static const struct { u16 map; u8 localId; } objects[] =
    {
        {MAP_ROUTE111, LOCALID_ROUTE111_VIAL_CHANSEY},
        {MAP_ROUTE112, LOCALID_ROUTE112_VIAL_CHANSEY},
        // These maps bind Chansey by the script-local LOCALID_CHANSEY value.
        {MAP_JAGGED_PASS, 7},
        {MAP_ROUTE133, 9},
        {MAP_ASHEN_WOODS, 8},
    };
    u32 i, j;
    for (i = 0; i < ARRAY_COUNT(objects); i++)
    {
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(
            MAP_GROUP(objects[i].map), MAP_NUM(objects[i].map));
        u32 found = 0;
        for (j = 0; j < map->events->objectEventCount; j++)
        {
            const struct ObjectEventTemplate *object = &map->events->objectEvents[j];
            const struct ObjectEventGraphicsInfo *graphics;
            if (object->localId != objects[i].localId)
                continue;
            found++;
            graphics = GetObjectEventGraphicsInfo(object->graphicsId);
            EXPECT(graphics != NULL);
            if (graphics == NULL)
                continue;
            EXPECT(graphics->width > 0);
            EXPECT(graphics->height > 0);
            EXPECT(graphics->oam != NULL);
            EXPECT(graphics->images != NULL);
            EXPECT(graphics->anims != NULL);
        }
        EXPECT_EQ(found, 1);
    }
}
