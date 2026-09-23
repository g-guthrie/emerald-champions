#include "global.h"
#include "sprite.h"
#include "test/test.h"

extern bool32 Test_LoadReflectionPalette(struct ObjectEvent *object, struct Sprite *sprite, bool32 bridge);

TEST("Reflection palette: pond, ice and bridge allocation failures retain a valid slot and can retry")
{
    u32 kind;
    PARAMETRIZE { kind = 0; }
    PARAMETRIZE { kind = 1; }
    PARAMETRIZE { kind = 2; }
    static const u16 colors[16] = {0};
    struct SpritePalette palette = {colors, 0x7100};
    struct ObjectEvent object = {0};
    struct Sprite reflection = {0};
    struct Sprite savedMain = gSprites[0];
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    for (u32 i = 0; i < 16; i++)
    {
        palette.tag = 0x7100 + i;
        EXPECT_EQ(LoadSpritePalette(&palette), i);
    }
    memset(&gSprites[0], 0, sizeof(gSprites[0]));
    gSprites[0].inUse = TRUE;
    gSprites[0].oam.paletteNum = 0;
    object.spriteId = 0;
    reflection.inUse = TRUE;
    reflection.oam.paletteNum = 0;
    reflection.data[7] = kind == 1;
    EXPECT(!Test_LoadReflectionPalette(&object, &reflection, kind == 2));
    EXPECT_EQ((u32)reflection.oam.paletteNum, 0);
    EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    FreeSpritePaletteByTag(0x7101);
    EXPECT(Test_LoadReflectionPalette(&object, &reflection, kind == 2));
    EXPECT_EQ((u32)reflection.oam.paletteNum, 1);
    EXPECT(Test_LoadReflectionPalette(&object, &reflection, kind == 2));
    EXPECT_EQ((u32)reflection.oam.paletteNum, 1);
    gSprites[0] = savedMain;
    FreeAllSpritePalettes();
}
