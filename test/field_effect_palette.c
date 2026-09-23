#include "global.h"
#include "field_effect.h"
#include "field_weather.h"
#include "sprite.h"
#include "trainer_pokemon_sprites.h"
#include "task.h"
#include "script_menu.h"
#include "constants/field_effects.h"
#include "test/test.h"

TEST("Field effect palette: exhausted sprite palettes leave weather mappings unchanged")
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
    const u8 *mapping = SetPaletteColorMapType(15, COLOR_MAP_NONE);
    u8 before[32];
    memcpy(before, mapping, sizeof(before));
    palette.tag = 0x7200;
    FieldEffect_LoadFadedPalette(&palette, COLOR_MAP_DARK_CONTRAST);
    EXPECT_EQ(memcmp(before, mapping, sizeof(before)), 0);
    EXPECT_EQ(IndexOfSpritePaletteTag(palette.tag), 0xFF);
    // Invalid OBJ slots are also rejected at the shared weather entry point.
    UpdateSpritePaletteWithWeather(16, TRUE);
    UpdateSpritePaletteWithWeather(0xFF, TRUE);
    EXPECT_EQ(memcmp(before, mapping, sizeof(before)), 0);
    // The script form must consume its operands even when allocation fails.
    u8 script[9] = {0};
    u32 paletteAddress = (u32)&palette;
    memcpy(script, &paletteAddress, sizeof(paletteAddress));
    script[4] = COLOR_MAP_DARK_CONTRAST;
    u8 *cursor = script;
    FieldEffectScript_LoadFadedPalette(&cursor);
    EXPECT_EQ(cursor, script + 5);
    EXPECT_EQ(memcmp(before, mapping, sizeof(before)), 0);
    // Freeing one slot allows normal loading and weather mapping to resume.
    FreeSpritePaletteByTag(0x7100);
    FieldEffect_LoadFadedPalette(&palette, COLOR_MAP_DARK_CONTRAST);
    EXPECT_EQ(IndexOfSpritePaletteTag(palette.tag), 0);
    EXPECT_EQ(mapping[16], COLOR_MAP_DARK_CONTRAST);
    FreeAllSpritePalettes();
}

extern bool8 FldEff_FieldMoveShowMon(void);

extern u8 CreateMonSprite_FieldMove(enum Species species, bool8 isShiny, u32 personality, s16 x, s16 y, u8 subpriority);

TEST("Field move portraits: exhausted picture slots return the sentinel before palette access")
{
    ResetAllPicSprites();
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u16 ids[8];
    for (u32 i = 0; i < ARRAY_COUNT(ids); i++)
    {
        ids[i] = CreateMonPicSprite(SPECIES_EEVEE, FALSE, 0, TRUE, 0, 0, 0, SPECIES_EEVEE);
        EXPECT_NE(ids[i], 0xFFFF);
    }
    EXPECT_EQ(CreateMonSprite_FieldMove(SPECIES_EEVEE, FALSE, 0, 0, 0, 0), MAX_SPRITES);
    u32 tasksBefore = 0;
    for (u32 i = 0; i < NUM_TASKS; i++) tasksBefore += gTasks[i].isActive;
    FieldEffectActiveListAdd(FLDEFF_FIELD_MOVE_SHOW_MON);
    gFieldEffectArguments[0] = SPECIES_EEVEE;
    gFieldEffectArguments[1] = FALSE;
    gFieldEffectArguments[2] = 0;
    FldEff_FieldMoveShowMon();
    EXPECT(!FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON));
    EXPECT(!ScriptMenu_ShowPokemonPic(SPECIES_EEVEE, 0, 0));
    u32 tasksAfter = 0;
    for (u32 i = 0; i < NUM_TASKS; i++) tasksAfter += gTasks[i].isActive;
    EXPECT_EQ(tasksAfter, tasksBefore);
    for (u32 i = 0; i < ARRAY_COUNT(ids); i++)
        FreeAndDestroyMonPicSprite(ids[i]);
}

extern u32 gOamMatrixAllocBitmap;

TEST("Picture resources: custom palette tags remain until the last owner and inactive records cannot destroy reused sprites")
{
    ResetAllPicSprites();
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u16 first = CreateMonPicSprite(SPECIES_EEVEE, FALSE, 0, TRUE, 0, 0, 0, 0x7100);
    u16 second = CreateMonPicSprite(SPECIES_EEVEE, FALSE, 0, TRUE, 64, 0, 0, 0x7100);
    EXPECT_NE(first, 0xFFFF);
    EXPECT_NE(second, 0xFFFF);
    EXPECT_NE(IndexOfSpritePaletteTag(0x7100), 0xFF);
    EXPECT(gSprites[first].template != gSprites[second].template);
    EXPECT(gSprites[first].template->images == gSprites[first].images);
    EXPECT_EQ(FreeAndDestroyMonPicSprite(first), 0);
    EXPECT_NE(IndexOfSpritePaletteTag(0x7100), 0xFF);
    u32 reused = CreateSprite(&gDummySpriteTemplate, 0, 0, 0);
    EXPECT_EQ(reused, first);
    EXPECT_EQ(FreeAndDestroyMonPicSprite(first), 0xFFFF);
    EXPECT(gSprites[reused].inUse);
    DestroySprite(&gSprites[reused]);
    EXPECT_EQ(FreeAndDestroyMonPicSprite(second), 0);
    EXPECT_EQ(IndexOfSpritePaletteTag(0x7100), 0xFF);
    u32 matrices = gOamMatrixAllocBitmap;
    u16 affine = CreateMonPicSprite_Affine(SPECIES_EEVEE, FALSE, 0, MON_PIC_AFFINE_FRONT, 0, 0, 0, 0x7101);
    EXPECT_NE(affine, 0xFFFF);
    EXPECT_NE(gOamMatrixAllocBitmap, matrices);
    FreeAndDestroyMonPicSprite(affine);
    EXPECT_EQ(gOamMatrixAllocBitmap, matrices);
}
