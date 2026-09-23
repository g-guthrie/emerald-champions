#include "global.h"
#include "pokemon.h"
#include "sprite.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "test/test.h"

extern u32 Test_LoadDynamicFollowerPalette(enum Species species, bool32 shiny, bool32 female);
extern void Test_UpdateDynamicFollowerPalette(struct Sprite *sprite, enum Species species, bool32 shiny, bool32 female);

TEST("Dynamic palette: normal and shiny fallback variants use distinct reusable slots")
{
    enum Species species = SPECIES_NONE;
    for (enum Species i = 1; i < NUM_SPECIES; i++)
        if (GetSpeciesBaseHP(i) && !gSpeciesInfo[i].overworldPalette && !gSpeciesInfo[i].overworldShinyPalette)
        {
            species = i;
            break;
        }
    EXPECT_NE(species, SPECIES_NONE);
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    u32 normal = Test_LoadDynamicFollowerPalette(species, FALSE, FALSE);
    u32 shiny = Test_LoadDynamicFollowerPalette(species, TRUE, FALSE);
    EXPECT(normal < 16 && shiny < 16);
    EXPECT_NE(normal, shiny);
    EXPECT_EQ(Test_LoadDynamicFollowerPalette(species, FALSE, FALSE), normal);
    EXPECT_EQ(Test_LoadDynamicFollowerPalette(species, TRUE, FALSE), shiny);
    FreeAllSpritePalettes();
}

TEST("Dynamic palette: exhaustion is reported before sprite creation and refresh keeps its shared palette")
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
    gSprites[0].inUse = gSprites[1].inUse = TRUE;
    EXPECT_EQ(Test_LoadDynamicFollowerPalette(SPECIES_BULBASAUR, FALSE, FALSE), 0xFF);
    EXPECT_EQ(CreateObjectGraphicsSprite(OBJ_EVENT_GFX_SPECIES(BULBASAUR), SpriteCallbackDummy, 0, 0, 0), MAX_SPRITES);
    Test_UpdateDynamicFollowerPalette(&gSprites[0], SPECIES_BULBASAUR, FALSE, FALSE);
    EXPECT_EQ((u32)gSprites[0].oam.paletteNum, 0);
    EXPECT_EQ(GetSpritePaletteTagByPaletteNum(0), 0x7100);
    FreeSpritePaletteByTag(0x7101);
    Test_UpdateDynamicFollowerPalette(&gSprites[0], SPECIES_BULBASAUR, FALSE, FALSE);
    EXPECT_EQ((u32)gSprites[0].oam.paletteNum, 1);
    gSprites[0].inUse = gSprites[1].inUse = FALSE;
    FreeAllSpritePalettes();
}

TEST("Object construction: full sprite pool does not allocate a palette")
{
    u32 graphics;
    PARAMETRIZE { graphics = OBJ_EVENT_GFX_MART_EMPLOYEE; }
    PARAMETRIZE { graphics = OBJ_EVENT_GFX_SPECIES(BULBASAUR); }
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    for (u32 i = 0; i < MAX_SPRITES; i++)
        gSprites[i].inUse = TRUE;
    EXPECT_EQ(CreateObjectGraphicsSprite(graphics, SpriteCallbackDummy, 0, 0, 0), MAX_SPRITES);
    for (u32 i = 0; i < 16; i++)
        EXPECT_EQ(GetSpritePaletteTagByPaletteNum(i), TAG_NONE);
    ResetSpriteData();
}

TEST("Object construction: regular palette exhaustion returns failure without using the last palette")
{
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
    EXPECT_EQ(CreateObjectGraphicsSprite(OBJ_EVENT_GFX_MART_EMPLOYEE, SpriteCallbackDummy, 0, 0, 0), MAX_SPRITES);
    for (u32 i = 0; i < 16; i++)
        EXPECT_EQ(GetSpritePaletteTagByPaletteNum(i), 0x7100 + i);
    EXPECT_EQ((u32)gSprites[0].inUse, FALSE);
    FreeAllSpritePalettes();
}

TEST("Object construction: raw tile exhaustion rolls back only a newly allocated palette")
{
    bool32 shared;
    PARAMETRIZE { shared = FALSE; }
    PARAMETRIZE { shared = TRUE; }
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    const struct ObjectEventGraphicsInfo *info = GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_MART_EMPLOYEE);
    EXPECT_EQ(info->tileTag, TAG_NONE);
    EXPECT(!info->compressed);
    if (shared)
        EXPECT(LoadObjectEventPalette(info->paletteTag) < 16);
    EXPECT_EQ(AllocSpriteTiles(TOTAL_OBJ_TILE_COUNT), 0);
    EXPECT_EQ(CreateObjectGraphicsSprite(OBJ_EVENT_GFX_MART_EMPLOYEE, SpriteCallbackDummy, 0, 0, 0), MAX_SPRITES);
    EXPECT_EQ((bool32)(IndexOfSpritePaletteTag(info->paletteTag) != 0xFF), shared);
    EXPECT_EQ((u32)gSprites[0].inUse, FALSE);
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
}
