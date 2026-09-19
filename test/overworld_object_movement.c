#include "global.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "sprite.h"
#include "constants/event_object_movement.h"
#include "constants/layouts.h"
#include "test/test.h"

TEST("Object movement: unsupported directions use the existing downward fallback")
{
    // Diagonals are supported by facing, but not by ordinary walk actions.
    EXPECT_EQ(GetFaceDirectionMovementAction(DIR_NORTHEAST), MOVEMENT_ACTION_FACE_RIGHT);
    EXPECT_EQ(GetWalkNormalMovementAction(DIR_EAST), MOVEMENT_ACTION_WALK_NORMAL_RIGHT);
    EXPECT_EQ(GetWalkNormalMovementAction(DIR_SOUTHWEST), MOVEMENT_ACTION_WALK_NORMAL_DOWN);
    EXPECT_EQ(GetWalkFastMovementAction(DIR_SOUTHWEST), MOVEMENT_ACTION_WALK_FAST_DOWN);
    EXPECT_EQ(GetFaceDirectionMovementAction(DIR_NORTHEAST + 1), MOVEMENT_ACTION_FACE_DOWN);
    EXPECT_EQ(GetFaceDirectionMovementAction(0xFFFFFFFF), MOVEMENT_ACTION_FACE_DOWN);
}

TEST("Object movement: object lookup agrees with generated facility templates")
{
    static const u8 script[] = {0x02};
    static const struct MapEvents oneStaticObject = {.objectEventCount = 1};
    static const struct MapEvents noStaticObjects = {0};
    struct MapHeader savedHeader = gMapHeader;
    struct ObjectEventTemplate savedTemplates[3];
    struct ObjectEvent savedObject = gObjectEvents[0];
    memcpy(savedTemplates, gSaveBlock1Ptr->objectEventTemplates, sizeof(savedTemplates));
    memset(gSaveBlock1Ptr->objectEventTemplates, 0, sizeof(savedTemplates));
    gSaveBlock1Ptr->objectEventTemplates[0].localId = 3;
    gSaveBlock1Ptr->objectEventTemplates[1].localId = 9;
    gSaveBlock1Ptr->objectEventTemplates[1].flagId = FLAG_TEMP_1;
    gSaveBlock1Ptr->objectEventTemplates[1].script = script;
    u8 mapNum = gSaveBlock1Ptr->location.mapNum;
    u8 mapGroup = gSaveBlock1Ptr->location.mapGroup;
    gObjectEvents[0].localId = 9;
    gObjectEvents[0].mapNum = mapNum;
    gObjectEvents[0].mapGroup = mapGroup;

    // Ordinary maps must not find stale entries beyond their current count.
    gMapHeader.mapLayoutId = LAYOUT_LITTLEROOT_TOWN;
    gMapHeader.events = &oneStaticObject;
    EXPECT_EQ(GetObjectEventScriptPointerByObjectEventId(0), NULL);
    EXPECT_EQ(GetObjectEventFlagIdByLocalIdAndMap(9, mapNum, mapGroup), 0);
    // A generated Pyramid floor can have no static map objects at all.
    gMapHeader.mapLayoutId = LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_FLOOR;
    gMapHeader.events = &noStaticObjects;
    EXPECT_EQ(GetObjectEventScriptPointerByObjectEventId(0), script);
    EXPECT_EQ(GetObjectEventFlagIdByLocalIdAndMap(9, mapNum, mapGroup), FLAG_TEMP_1);
    // Trainer Hill similarly owns its generated pair rather than the header count.
    gMapHeader.mapLayoutId = LAYOUT_TRAINER_HILL_1F;
    EXPECT_EQ(GetObjectEventScriptPointerByObjectEventId(0), script);
    // Missing event data is a failed lookup, not a dereference of NULL.
    gMapHeader.events = NULL;
    EXPECT_EQ(GetObjectEventScriptPointerByObjectEventId(0), NULL);
    EXPECT_EQ(GetObjectEventFlagIdByLocalIdAndMap(9, mapNum, mapGroup), 0);

    gMapHeader = savedHeader;
    gObjectEvents[0] = savedObject;
    memcpy(gSaveBlock1Ptr->objectEventTemplates, savedTemplates, sizeof(savedTemplates));
}

TEST("Object movement: changing graphics releases tile-zero sheets only when unshared")
{
    bool32 shared;
    PARAMETRIZE { shared = FALSE; }
    PARAMETRIZE { shared = TRUE; }
    static const u32 pixels[16] = {0};
    static const struct SpriteFrameImage frames[] = {{.data = pixels, .size = sizeof(pixels)}};
    static const struct OamData oam = {.shape = SPRITE_SHAPE(8x8), .size = SPRITE_SIZE(8x8)};
    static const struct ObjectEventGraphicsInfo first = {.tileTag = 6000, .size = sizeof(pixels), .oam = &oam, .images = frames};
    static const struct ObjectEventGraphicsInfo second = {.tileTag = 6001, .size = sizeof(pixels), .oam = &oam, .images = frames};
    ResetSpriteData();
    LoadSheetGraphicsInfo(&first, 0, NULL);
    EXPECT_EQ(GetSpriteTileStartByTag(first.tileTag), 0);
    gSprites[0].inUse = TRUE;
    gSprites[0].usingSheet = TRUE;
    gSprites[0].sheetTileStart = 0;
    if (shared)
        gSprites[1] = gSprites[0];
    LoadSheetGraphicsInfo(&second, 0, &gSprites[0]);
    EXPECT_EQ(GetSpriteTileStartByTag(first.tileTag), shared ? 0 : TAG_NONE);
    EXPECT_EQ(gSprites[0].sheetTileStart, GetSpriteTileStartByTag(second.tileTag));
    EXPECT(!gSprites[0].invisible);
    ResetSpriteData();
}
