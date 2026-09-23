#include "global.h"
#include "sprite.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "test/test.h"

extern void Test_RecreateObjectSprite(u8 objectEventId);
extern bool32 Test_RestoreObjectPalette(u8 objectEventId);
extern void Test_UpdateDynamicFollowerPalette(struct Sprite *sprite, enum Species species, bool32 shiny, bool32 female);
extern u8 Test_UpdateObjectSpritePalette(const struct SpritePalette *palette, struct Sprite *sprite);

TEST("Object palette recovery: restoring a moved object preserves identity and coordinates through exhaustion")
{
    u32 finish;
    PARAMETRIZE { finish = 0; } // Recover the original dynamic palette.
    PARAMETRIZE { finish = 1; } // Remove while still pending.
    PARAMETRIZE { finish = 2; } // Change to static graphics before recovery.
    static const u16 colors[16] = {0};
    struct SpritePalette palette = {colors, 0x7100};
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    for (u32 i = 0; i < 16; i++)
    {
        palette.tag = 0x7100 + i;
        EXPECT_EQ(LoadSpritePalette(&palette), i);
    }
    struct ObjectEvent *object = &gObjectEvents[0];
    ClearObjectEvent(object);
    object->active = TRUE;
    object->localId = 7;
    object->graphicsId = OBJ_EVENT_GFX_SPECIES(BULBASAUR);
    object->movementType = MOVEMENT_TYPE_FACE_DOWN;
    object->facingDirection = DIR_EAST;
    object->currentCoords.x = 23;
    object->currentCoords.y = 31;
    object->initialCoords.x = 9;
    object->initialCoords.y = 11;
    Test_RecreateObjectSprite(0);
    EXPECT_EQ((u32)object->active, TRUE);
    EXPECT(object->spriteId < MAX_SPRITES);
    EXPECT_EQ((u32)gSprites[object->spriteId].invisible, TRUE);
    EXPECT(!Test_RestoreObjectPalette(0));
    Test_UpdateDynamicFollowerPalette(&gSprites[object->spriteId], SPECIES_BULBASAUR, FALSE, FALSE);
    EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    palette.tag = 0x7200;
    EXPECT_EQ(Test_UpdateObjectSpritePalette(&palette, &gSprites[object->spriteId]), 0xFF);
    EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    EXPECT_EQ(object->currentCoords.x, 23);
    EXPECT_EQ(object->currentCoords.y, 31);
    EXPECT_EQ(object->localId, 7);
    EXPECT_EQ((u32)object->facingDirection, DIR_EAST);
    if (finish == 1)
    {
        RemoveObjectEvent(object);
        EXPECT_EQ((u32)object->active, FALSE);
        // The hidden placeholder referenced this slot but never owned it.
        EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    }
    else
    {
        if (finish == 2)
        {
            object->graphicsId = OBJ_EVENT_GFX_BOY_1;
            EXPECT(!Test_RestoreObjectPalette(0));
            EXPECT_EQ((u32)gSprites[object->spriteId].invisible, TRUE);
            EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
        }
        FreeSpritePaletteByTag(0x7101);
        EXPECT(Test_RestoreObjectPalette(0));
        EXPECT_EQ((u32)gSprites[object->spriteId].oam.paletteNum, 1);
        EXPECT_EQ(object->currentCoords.x, 23);
        EXPECT_EQ(object->initialCoords.x, 9);
        EXPECT_EQ(object->currentCoords.y, 31);
        EXPECT_EQ((u32)gSprites[object->spriteId].invisible, FALSE);
    }
    ClearObjectEvent(object);
    ResetSpriteData();
    FreeAllSpritePalettes();
}
