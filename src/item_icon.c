#include "global.h"
#include "battle_main.h"
#include "decompress.h"
#include "graphics.h"
#include "item.h"
#include "item_icon.h"
#include "malloc.h"
#include "move.h"
#include "sprite.h"
#include "constants/items.h"

// EWRAM vars
EWRAM_DATA u8 *gItemIconDecompressionBuffer = NULL;
EWRAM_DATA u8 *gItemIcon4x4Buffer = NULL;

static const struct OamData sOamData_ItemIcon =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x32),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 2,
    .affineParam = 0
};

static const union AnimCmd sSpriteAnim_ItemIcon[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_ItemIcon[] =
{
    sSpriteAnim_ItemIcon
};

const struct SpriteTemplate gItemIconSpriteTemplate =
{
    .tileTag = 0,
    .paletteTag = 0,
    .oam = &sOamData_ItemIcon,
    .anims = sSpriteAnimTable_ItemIcon,
};

bool8 AllocItemIconTemporaryBuffers(void)
{
    // One owner for the decompressed 3x3 image and padded 4x4 image.
    gItemIconDecompressionBuffer = AllocZeroedUnchecked(0x120 + 0x200);
    gItemIcon4x4Buffer = gItemIconDecompressionBuffer == NULL
        ? NULL : gItemIconDecompressionBuffer + 0x120;
    return gItemIconDecompressionBuffer != NULL;
}

void FreeItemIconTemporaryBuffers(void)
{
    TRY_FREE_AND_SET_NULL(gItemIconDecompressionBuffer);
    gItemIcon4x4Buffer = NULL;
}

void CopyItemIconPicTo4x4Buffer(const void *src, void *dest)
{
    u8 i;

    for (i = 0; i < 3; i++)
        CpuCopy16(src + i * 96, dest + i * 128, 0x60);
}

u8 AddItemIconSprite(u16 tilesTag, u16 paletteTag, enum Item itemId)
{
    return AddCustomItemIconSprite(&gItemIconSpriteTemplate, tilesTag, paletteTag, itemId);
}

u8 AddCustomItemIconSprite(const struct SpriteTemplate *customSpriteTemplate, u16 tilesTag, u16 paletteTag, enum Item itemId)
{
    if (!AllocItemIconTemporaryBuffers())
        return MAX_SPRITES;

    struct SpriteTemplate spriteTemplate = *customSpriteTemplate;
    struct SpriteSheet spriteSheet = {gItemIcon4x4Buffer, 0x200, tilesTag};
    struct SpritePalette spritePalette = {GetItemIconPalette(itemId), paletteTag};
    bool32 newTiles = GetSpriteTileStartByTag(tilesTag) == TAG_NONE;
    bool32 newPalette = IndexOfSpritePaletteTag(paletteTag) == 0xFF;
    u8 spriteId = MAX_SPRITES;

    if (newTiles)
    {
        DecompressDataWithHeaderWram(GetItemIconPic(itemId), gItemIconDecompressionBuffer);
        CopyItemIconPicTo4x4Buffer(gItemIconDecompressionBuffer, gItemIcon4x4Buffer);
        LoadSpriteSheet(&spriteSheet);
    }
    LoadSpritePalette(&spritePalette);
    spriteTemplate.tileTag = tilesTag;
    spriteTemplate.paletteTag = paletteTag;
    if (GetSpriteTileStartByTag(tilesTag) != TAG_NONE && IndexOfSpritePaletteTag(paletteTag) != 0xFF)
        spriteId = CreateSpriteWithTemplateCopy(&spriteTemplate, 0, 0, 0);

    FreeItemIconTemporaryBuffers();
    if (spriteId == MAX_SPRITES)
    {
        if (newTiles)
            FreeSpriteTilesByTag(tilesTag);
        if (newPalette)
            FreeSpritePaletteByTag(paletteTag);
    }
    return spriteId;
}

const void *GetItemIconPic(enum Item itemId)
{
    if (itemId == ITEM_LIST_END)
        return gItemIcon_ReturnToFieldArrow; // Use last icon, the "return to field" arrow
    if (itemId >= ITEMS_COUNT)
        return gItemsInfo[0].iconPic;
    return gItemsInfo[itemId].iconPic;
}

const u16 *GetItemIconPalette(enum Item itemId)
{
    if (itemId == ITEM_LIST_END)
        return gItemIconPalette_ReturnToFieldArrow;
    if (itemId >= ITEMS_COUNT)
        return gItemsInfo[0].iconPalette;
    return gItemsInfo[itemId].iconPalette;
}
