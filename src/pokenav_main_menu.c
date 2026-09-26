#include "global.h"
#include "pokenav.h"
#include "constants/songs.h"
#include "sound.h"
#include "constants/rgb.h"
#include "palette.h"
#include "bg.h"
#include "window.h"
#include "strings.h"
#include "graphics.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "menu.h"
#include "dma3.h"

// The PokeNav frame around the Hoenn map: the header background with its help
// bar along the bottom, and the sliding "Hoenn Map" title at the top.

#define GFXTAG_MAP_HEADER 2
#define PALTAG_MAP_HEADER 1

struct Pokenav_Frame
{
    u32 loadTaskId;
    u32 helpBarWindowId;
    struct Sprite *headerSprites[2];
    ALIGNED(4) u8 tilemapBuffer[BG_SCREEN_SIZE];
};

static void InitHelpBar(void);
static void DrawHelpBar(u32);
static void CreateMapHeaderSprites(void);
static void MoveMapHeader(struct Sprite *, s32, s32, s32);
static void SpriteCB_MoveMapHeader(struct Sprite *);
static u32 LoopedTask_InitPokenavFrame(s32);

static const struct BgTemplate sPokenavFrameBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 5,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    }
};

static const struct WindowTemplate sHelpBarWindowTemplate[] =
{
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 22,
        .width = 16,
        .height = 2,
        .paletteNum = 0,
        .baseBlock = 0x36,
    },
    DUMMY_WIN_TEMPLATE
};

static const u8 *const sHelpBarTexts[HELPBAR_COUNT] =
{
    [HELPBAR_MAP_ZOOMED_OUT]        = COMPOUND_STRING("{A_BUTTON}Zoom {B_BUTTON}Close"),
    [HELPBAR_MAP_ZOOMED_IN]         = COMPOUND_STRING("{A_BUTTON}Full {B_BUTTON}Close"),
    [HELPBAR_MAP_ZOOMED_OUT_CANFLY] = COMPOUND_STRING("{A_BUTTON}Zoom {B_BUTTON}Close {R_BUTTON}Fly"),
    [HELPBAR_MAP_ZOOMED_IN_CANFLY]  = COMPOUND_STRING("{A_BUTTON}Full {B_BUTTON}Close {R_BUTTON}Fly"),
    [HELPBAR_MAP_ZOOM_DISABLED]     = COMPOUND_STRING("{B_BUTTON}Close"),
};

static const u8 sHelpBarTextColors[3] =
{
    TEXT_COLOR_RED, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY
};

// Three 64x32 frames: "Hoenn Map", then the "Full View" and "Zoomed" labels
// shown to its right.
static const struct CompressedSpriteSheet sMapHeaderSpriteSheet =
{
    .data = gPokenavLeftHeaderHoennMap_Gfx,
    .size = 0xC00,
    .tag = GFXTAG_MAP_HEADER
};

static const struct OamData sOamData_MapHeader =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(64x32),
    .x = 0,
    .size = SPRITE_SIZE(64x32),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
};

static const struct SpriteTemplate sMapHeaderSpriteTemplate =
{
    .tileTag = GFXTAG_MAP_HEADER,
    .paletteTag = PALTAG_MAP_HEADER,
    .oam = &sOamData_MapHeader,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

bool32 InitPokenavFrame(void)
{
    struct Pokenav_Frame *frame;

    frame = AllocSubstruct(POKENAV_SUBSTRUCT_FRAME, sizeof(struct Pokenav_Frame));
    if (frame == NULL)
        return FALSE;

    ResetSpriteData();
    FreeAllSpritePalettes();
    frame->loadTaskId = CreateLoopedTask(LoopedTask_InitPokenavFrame, 1);
    return TRUE;
}

u32 IsPokenavFrameLoading(void)
{
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);
    return IsLoopedTaskActive(frame->loadTaskId);
}

void ShutdownPokenav(void)
{
    PlaySE(SE_POKENAV_OFF);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    BeginNormalPaletteFade(PALETTES_ALL, -1, 0, 16, RGB_BLACK);
}

// Called once the switch-off fade has finished and the map is freed.
void FreePokenavFrame(void)
{
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);
    s32 i;

    for (i = 0; i < (s32)ARRAY_COUNT(frame->headerSprites); i++)
        DestroySprite(frame->headerSprites[i]);
    FreeSpriteTilesByTag(GFXTAG_MAP_HEADER);
    FreeSpritePaletteByTag(PALTAG_MAP_HEADER);
    FreeAllWindowBuffers();
}

static u32 LoopedTask_InitPokenavFrame(s32 state)
{
    struct Pokenav_Frame *frame;

    switch (state)
    {
    case 0:
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
        FreeAllWindowBuffers();
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sPokenavFrameBgTemplates, ARRAY_COUNT(sPokenavFrameBgTemplates));
        ResetBgPositions();
        // The header's top strip sits above the screen; its help bar shows
        // along the bottom, under the map.
        ChangeBgY(0, 0x2000, BG_COORD_SET);
        ResetTempTileDataBuffers();
        return LT_INC_AND_CONTINUE;
    case 1:
        frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);
        DecompressAndCopyTileDataToVram(0, &gPokenavHeader_Gfx, 0, 0, 0);
        SetBgTilemapBuffer(0, frame->tilemapBuffer);
        CopyToBgTilemapBuffer(0, &gPokenavHeader_Tilemap, 0, 0);
        CopyPaletteIntoBufferUnfaded(gPokenavHeader_Pal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        CopyBgTilemapBufferToVram(0);
        return LT_INC_AND_PAUSE;
    case 2:
        if (FreeTempTileDataBuffersIfPossible())
            return LT_PAUSE;

        InitHelpBar();
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;

        CreateMapHeaderSprites();
        ShowBg(0);
        return LT_FINISH;
    default:
        return LT_FINISH;
    }
}

void CopyPaletteIntoBufferUnfaded(const u16 *palette, u32 bufferOffset, u32 size)
{
    CpuCopy16(palette, &gPlttBufferUnfaded[bufferOffset], size);
}

void Pokenav_AllocAndLoadPalettes(const struct SpritePalette *palettes)
{
    const struct SpritePalette *current;
    u32 index;

    for (current = palettes; current->data != NULL; current++)
    {
        index = AllocSpritePalette(current->tag);
        if (index == 0xFF)
        {
            break;
        }
        else
        {
            index = OBJ_PLTT_ID(index);
            CopyPaletteIntoBufferUnfaded(current->data, index, PLTT_SIZE_4BPP);
        }
    }
}

bool32 IsPaletteFadeActive(void)
{
    return gPaletteFade.active;
}

// Excludes the first obj and bg palettes
void FadeToBlackExceptPrimary(void)
{
    BlendPalettes(PALETTES_ALL & ~(1 << 16 | 1), 16, RGB_BLACK);
}

void InitBgTemplates(const struct BgTemplate *templates, int count)
{
    int i;

    for (i = 0; i < count; i++)
        InitBgFromTemplate(templates++);
}

static void InitHelpBar(void)
{
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);

    InitWindows(&sHelpBarWindowTemplate[0]);
    frame->helpBarWindowId = 0;
    DrawHelpBar(frame->helpBarWindowId);
    PutWindowTilemap(frame->helpBarWindowId);
    CopyWindowToVram(frame->helpBarWindowId, COPYWIN_FULL);
}

void PrintHelpBarText(u32 textId)
{
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);

    DrawHelpBar(frame->helpBarWindowId);
    AddTextPrinterParameterized3(frame->helpBarWindowId, FONT_NORMAL, 0, 1, sHelpBarTextColors, 0, sHelpBarTexts[textId]);
}

bool32 WaitForHelpBar(void)
{
    return IsDma3ManagerBusyWithBgCopy();
}

static void DrawHelpBar(u32 windowId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(4));
    FillWindowPixelRect(windowId, PIXEL_FILL(5), 0, 0, 0x80, 1);
}

static void CreateMapHeaderSprites(void)
{
    s32 i, spriteId;
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);

    LoadCompressedSpriteSheet(&sMapHeaderSpriteSheet);
    AllocSpritePalette(PALTAG_MAP_HEADER);
    // The header palette file holds one palette per title; the map's is first.
    CopyPaletteIntoBufferUnfaded(gPokenavLeftHeader_Pal, OBJ_PLTT_ID(IndexOfSpritePaletteTag(PALTAG_MAP_HEADER)), PLTT_SIZE_4BPP);
    for (i = 0; i < (s32)ARRAY_COUNT(frame->headerSprites); i++)
    {
        spriteId = CreateSprite(&sMapHeaderSpriteTemplate, 0, 0, 1);
        frame->headerSprites[i] = &gSprites[spriteId];
        frame->headerSprites[i]->invisible = TRUE;
    }
    // "Hoenn Map", then its view label just to the right.
    frame->headerSprites[1]->x2 = 56;
}

// Updates the view label beside "Hoenn Map".
void UpdateMapHeader(u32 headerId)
{
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);

    if (headerId == POKENAV_HEADER_MAP_ZOOMED_OUT)
        frame->headerSprites[1]->oam.tileNum = GetSpriteTileStartByTag(GFXTAG_MAP_HEADER) + 32;
    else
        frame->headerSprites[1]->oam.tileNum = GetSpriteTileStartByTag(GFXTAG_MAP_HEADER) + 64;
}

// Slides the title in from the right edge.
void ShowMapHeader(u32 headerId)
{
    s32 i;
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);

    UpdateMapHeader(headerId);
    for (i = 0; i < (s32)ARRAY_COUNT(frame->headerSprites); i++)
    {
        frame->headerSprites[i]->y = 16;
        MoveMapHeader(frame->headerSprites[i], 256, 160, 12);
    }
}

bool32 IsMapHeaderMoving(void)
{
    struct Pokenav_Frame *frame = GetSubstructPtr(POKENAV_SUBSTRUCT_FRAME);

    return frame->headerSprites[0]->callback != SpriteCallbackDummy;
}

static void MoveMapHeader(struct Sprite *sprite, s32 startX, s32 endX, s32 duration)
{
    sprite->x = startX;
    sprite->data[0] = startX * 16;
    sprite->data[1] = (endX - startX) * 16 / duration;
    sprite->data[2] = duration;
    sprite->data[7] = endX;
    sprite->callback = SpriteCB_MoveMapHeader;
}

static void SpriteCB_MoveMapHeader(struct Sprite *sprite)
{
    if (sprite->data[2] != 0)
    {
        sprite->data[2]--;
        sprite->data[0] += sprite->data[1];
        sprite->x = sprite->data[0] >> 4;
        if (sprite->x < -16 || sprite->x > 256)
            sprite->invisible = TRUE;
        else
            sprite->invisible = FALSE;
    }
    else
    {
        sprite->x = sprite->data[7];
        sprite->callback = SpriteCallbackDummy;
    }
}
