#include "global.h"
#include "region_map.h"
#include "sprite.h"
#include "malloc.h"
#include "main.h"
#include "field_effect.h"
#include "overworld.h"
#include "event_data.h"
#include "constants/heal_locations.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "test/test.h"

static void FlyMapAllocationFailureReturn(void) {}

TEST("Flight Beacon: Fly map allocation failure returns to its caller and clears the rider")
{
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    MainCallback oldCallback = gMain.callback2;
    IntrCallback oldVBlank = gMain.vblankCallback;
    u32 oldState = gMain.state;
    enum Species oldOverride = gFieldMoveShowMonSpeciesOverride;

    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);

    SetFlyMapCancelCallback(FlyMapAllocationFailureReturn);
    gFieldMoveShowMonSpeciesOverride = SPECIES_WINGULL;
    gMain.state = 0;
    CB2_OpenFlyMap();
    EXPECT(gMain.callback2 == FlyMapAllocationFailureReturn);
    EXPECT_EQ(gFieldMoveShowMonSpeciesOverride, SPECIES_NONE);

    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    SetMainCallback2(oldCallback);
    gMain.state = oldState;
    SetVBlankCallback(oldVBlank);
    gFieldMoveShowMonSpeciesOverride = oldOverride;
}

TEST("Map connections: absent headers and lists return no connection; directions respect count")
{
    const struct MapConnections *saved = gMapHeader.connections;
    const struct MapConnection entries[] = {
        {.direction = CONNECTION_DIVE, .mapGroup = 1, .mapNum = 2},
        {.direction = CONNECTION_EMERGE, .mapGroup = 3, .mapNum = 4},
    };
    struct MapConnections connections = {.count = 2, .connections = NULL};
    gMapHeader.connections = NULL;
    EXPECT(GetMapConnection(CONNECTION_DIVE) == NULL);
    gMapHeader.connections = &connections;
    EXPECT(GetMapConnection(CONNECTION_DIVE) == NULL);
    connections.connections = entries;
    EXPECT(GetMapConnection(CONNECTION_DIVE) == &entries[0]);
    EXPECT(GetMapConnection(CONNECTION_EMERGE) == &entries[1]);
    EXPECT(GetMapConnection(CONNECTION_NORTH) == NULL);
    connections.count = 1;
    EXPECT(GetMapConnection(CONNECTION_EMERGE) == NULL);
    connections.count = 0;
    EXPECT(GetMapConnection(CONNECTION_DIVE) == NULL);
    gMapHeader.connections = saved;
}

TEST("Dive destinations: a connectionless map uses its fixed warp and fails without one")
{
    struct MapHeader saved = gMapHeader;
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ABANDONED_SHIP_UNDERWATER1), MAP_NUM(MAP_ABANDONED_SHIP_UNDERWATER1));
    EXPECT(gMapHeader.connections == NULL);
    SetFixedDiveWarp(MAP_GROUP(MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS),
        MAP_NUM(MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS), WARP_ID_NONE, 0, 10);
    EXPECT(SetDiveWarpEmerge(0, 0));
    EXPECT(GetDestinationWarpMapHeader() == Overworld_GetMapHeaderByGroupAndId(
        MAP_GROUP(MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS), MAP_NUM(MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS)));
    SetFixedDiveWarp(MAP_GROUP(MAP_UNDEFINED), MAP_NUM(MAP_UNDEFINED), WARP_ID_NONE, -1, -1);
    EXPECT(!SetDiveWarpEmerge(0, 0));
    gMapHeader = saved;
}

TEST("Dive destinations: Sealed Chamber puzzle tile selects its room and adjacent tiles return to Route134")
{
    struct MapHeader savedHeader = gMapHeader;
    s16 savedPlayerX = gSaveBlock1Ptr->pos.x, savedPlayerY = gSaveBlock1Ptr->pos.y;
    u16 savedX = gSpecialVar_0x8004, savedY = gSpecialVar_0x8005;
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_UNDERWATER_SEALED_CHAMBER), MAP_NUM(MAP_UNDERWATER_SEALED_CHAMBER));
    EXPECT(gMapHeader.connections == NULL);
    for (s16 y = 43; y <= 45; y++)
    {
        for (s16 x = 11; x <= 13; x++)
        {
            u16 expectedMap = x == 12 && y == 44 ? MAP_SEALED_CHAMBER_OUTER_ROOM : MAP_ROUTE134;
            gSaveBlock1Ptr->pos.x = x;
            gSaveBlock1Ptr->pos.y = y;
            EXPECT(SetDiveWarpEmerge(x, y));
            EXPECT(GetDestinationWarpMapHeader() == Overworld_GetMapHeaderByGroupAndId(expectedMap >> 8, expectedMap & 0xFF));
        }
    }
    SetFixedDiveWarp(MAP_GROUP(MAP_UNDEFINED), MAP_NUM(MAP_UNDEFINED), WARP_ID_NONE, -1, -1);
    gSpecialVar_0x8004 = savedX;
    gSpecialVar_0x8005 = savedY;
    gSaveBlock1Ptr->pos.x = savedPlayerX;
    gSaveBlock1Ptr->pos.y = savedPlayerY;
    gMapHeader = savedHeader;
}

TEST("Fly destinations: home follows player identity and the League requires its landmark")
{
    struct RegionMap map = {0};
    u8 gender = gSaveBlock2Ptr->playerGender;
    bool32 league = FlagGet(FLAG_LANDMARK_POKEMON_LEAGUE);
    map.mapSecId = MAPSEC_LITTLEROOT_TOWN;
    gSaveBlock2Ptr->playerGender = MALE;
    EXPECT_EQ(FilterFlyDestination(&map), HEAL_LOCATION_LITTLEROOT_TOWN_BRENDANS_HOUSE);
    gSaveBlock2Ptr->playerGender = FEMALE;
    EXPECT_EQ(FilterFlyDestination(&map), HEAL_LOCATION_LITTLEROOT_TOWN_MAYS_HOUSE);
    map.mapSecId = MAPSEC_EVER_GRANDE_CITY;
    FlagClear(FLAG_LANDMARK_POKEMON_LEAGUE);
    EXPECT_EQ(FilterFlyDestination(&map), HEAL_LOCATION_EVER_GRANDE_CITY);
    FlagSet(FLAG_LANDMARK_POKEMON_LEAGUE);
    EXPECT_EQ(FilterFlyDestination(&map), HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE);
    map.posWithinMapSec = 1;
    EXPECT_EQ(FilterFlyDestination(&map), HEAL_LOCATION_EVER_GRANDE_CITY);
    gSaveBlock2Ptr->playerGender = gender;
    if (!league)
        FlagClear(FLAG_LANDMARK_POKEMON_LEAGUE);
}

extern struct RegionMap *Test_SetRegionMap(struct RegionMap *map);

TEST("Region map icons: creation owns tags and repeated cleanup cannot destroy a reused sprite")
{
    struct RegionMap *map = AllocZeroed(sizeof(*map));
    struct RegionMap *previous = Test_SetRegionMap(map);
    u32 oldSection = gMapHeader.regionMapSectionId;
    gMapHeader.regionMapSectionId = MAPSEC_LITTLEROOT_TOWN;
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    CreateRegionMapCursor(0x7100, 0x7100);
    CreateRegionMapPlayerIcon(0x7101, 0x7101);
    EXPECT_EQ(map->playerIconTileTag, 0x7101);
    EXPECT_EQ(map->playerIconPaletteTag, 0x7101);
    EXPECT(map->cursorSprite->template != map->playerIconSprite->template);
    EXPECT_EQ(map->cursorSprite->template->tileTag, 0x7100);
    EXPECT_EQ(map->playerIconSprite->template->tileTag, 0x7101);
    FreeRegionMapIconResources();
    EXPECT_EQ(map->cursorSprite, NULL);
    EXPECT_EQ(map->playerIconSprite, NULL);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7100), TAG_NONE);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7101), TAG_NONE);
    EXPECT_EQ(IndexOfSpritePaletteTag(0x7101), 0xFF);
    u32 reused = CreateSprite(&gDummySpriteTemplate, 0, 0, 0);
    FreeRegionMapIconResources();
    EXPECT(gSprites[reused].inUse);
    DestroySprite(&gSprites[reused]);
    Test_SetRegionMap(previous);
    Free(map);
    gMapHeader.regionMapSectionId = oldSection;
}
