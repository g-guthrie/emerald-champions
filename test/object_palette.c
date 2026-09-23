#include "global.h"
#include "sprite.h"
#include "test/test.h"

extern u8 Test_UpdateObjectSpritePalette(const struct SpritePalette *palette, struct Sprite *sprite);

TEST("Object palette: failed replacement preserves a shared palette and retries after capacity returns")
{
    static const u16 colors[16] = {0};
    struct SpritePalette palette = {colors, 0x7100};
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    for (u32 i = 0; i < 16; i++)
    {
        palette.tag = 0x7100 + i;
        EXPECT_EQ(LoadSpritePalette(&palette), i);
    }
    memset(&gSprites[0], 0, sizeof(gSprites[0]));
    memset(&gSprites[1], 0, sizeof(gSprites[1]));
    gSprites[0].inUse = TRUE;
    gSprites[1].inUse = TRUE;
    palette.tag = 0x7200;
    EXPECT_EQ(Test_UpdateObjectSpritePalette(&palette, &gSprites[0]), 0xFF);
    EXPECT_EQ((u32)gSprites[0].oam.paletteNum, 0);
    EXPECT_EQ((u32)gSprites[0].inUse, TRUE);
    EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    FreeSpritePaletteByTag(0x7101);
    EXPECT_EQ(Test_UpdateObjectSpritePalette(&palette, &gSprites[0]), 1);
    EXPECT_EQ((u32)gSprites[0].oam.paletteNum, 1);
    EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    EXPECT_EQ(Test_UpdateObjectSpritePalette(&palette, &gSprites[0]), 1);
    // An unused old palette is released when reusing another loaded palette.
    palette.tag = 0x7102;
    EXPECT_EQ(Test_UpdateObjectSpritePalette(&palette, &gSprites[0]), 2);
    EXPECT_EQ(IndexOfSpritePaletteTag(0x7200), 0xFF);
    gSprites[0].inUse = FALSE;
    gSprites[1].inUse = FALSE;
    FreeAllSpritePalettes();
}
