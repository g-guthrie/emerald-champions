#include "global.h"
#include "bg.h"
#include "field_name_box.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "pokenav_call.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "constants/songs.h"

// The scripted PokeNav call presentation used by the `pokenavcall` script
// command. Match Call (random trainer calls, rematch requests, the PokeNav
// Match Call menu) is removed; only this window remains.

static void StartPokenavCall(void);
static void ExecutePokenavCall(u8);
static void DrawPokenavCallTextBoxBorder(u32, u32, u32);
static void Task_SpinPokenavIcon(u8);
static void InitPokenavCallTextPrinter(int, const u8 *);
static bool32 RunPokenavCallTextPrinter(int);
static bool32 PokenavCall_LoadGfx(u8);
static bool32 PokenavCall_DrawWindow(u8);
static bool32 PokenavCall_ReadyIntro(u8);
static bool32 PokenavCall_SlideWindowIn(u8);
static bool32 PokenavCall_PrintIntro(u8);
static bool32 PokenavCall_PrintMessage(u8);
static bool32 PokenavCall_SlideWindowOut(u8);
static bool32 PokenavCall_EndCall(u8);

void StartPokenavCallFromScript(const u8 *message)
{
    StartPokenavCall();
}

bool32 IsPokenavCallTaskActive(void)
{
    return FuncIsActiveTask(ExecutePokenavCall);
}

static void StartPokenavCall(void)
{
    PlaySE(SE_POKENAV_CALL);
    CreateTask(ExecutePokenavCall, 1);
}

static const u16 sPokenavCallWindow_Pal[] = INCGFX_U16("graphics/pokenav/match_call/window.png", ".gbapal");
static const u8 sPokenavCallWindow_Gfx[] = INCGFX_U8("graphics/pokenav/match_call/window.png", ".4bpp");
static const u16 sPokenavIcon_Pal[] = INCGFX_U16("graphics/pokenav/match_call/nav_icon.png", ".gbapal");
static const u32 sPokenavIcon_Gfx[] = INCGFX_U32("graphics/pokenav/match_call/nav_icon.png", ".4bpp.smol");

static const u8 sText_PokenavCallEllipsis[] = _("………………\p");

#define tState      data[0]
#define tWindowId   data[2]
#define tIconTaskId data[5]

static bool32 (*const sPokenavCallTaskFuncs[])(u8) =
{
    PokenavCall_LoadGfx,
    PokenavCall_DrawWindow,
    PokenavCall_ReadyIntro,
    PokenavCall_SlideWindowIn,
    PokenavCall_PrintIntro,
    PokenavCall_PrintMessage,
    PokenavCall_SlideWindowOut,
    PokenavCall_EndCall,
};

static void ExecutePokenavCall(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (sPokenavCallTaskFuncs[tState](taskId))
    {
        tState++;
        if ((u16)tState >= ARRAY_COUNT(sPokenavCallTaskFuncs))
            DestroyTask(taskId);
    }
}

static const struct WindowTemplate sPokenavCallTextWindow =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 15,
    .width = 28,
    .height = 4,
    .paletteNum = 15,
    .baseBlock = 0x200
};

#define TILE_MC_WINDOW    0x270
#define TILE_POKENAV_ICON 0x279

static bool32 PokenavCall_LoadGfx(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    tWindowId = AddWindow(&sPokenavCallTextWindow);
    if (tWindowId == WINDOW_NONE)
    {
        DestroyTask(taskId);
        return FALSE;
    }

    if (LoadBgTiles(0, sPokenavCallWindow_Gfx, sizeof(sPokenavCallWindow_Gfx), TILE_MC_WINDOW) == 0xFFFF)
    {
        RemoveWindow(tWindowId);
        DestroyTask(taskId);
        return FALSE;
    }

    if (!DecompressAndCopyTileDataToVram(0, sPokenavIcon_Gfx, 0, TILE_POKENAV_ICON, 0))
    {
        RemoveWindow(tWindowId);
        DestroyTask(taskId);
        return FALSE;
    }

    FillWindowPixelBuffer(tWindowId, PIXEL_FILL(8));
    LoadPalette(sPokenavCallWindow_Pal, BG_PLTT_ID(14), sizeof(sPokenavCallWindow_Pal));
    LoadPalette(sPokenavIcon_Pal, BG_PLTT_ID(15), sizeof(sPokenavIcon_Pal));
    ChangeBgY(0, -0x2000, BG_COORD_SET);
    return TRUE;
}

static bool32 PokenavCall_DrawWindow(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (FreeTempTileDataBuffersIfPossible())
        return FALSE;

    PutWindowTilemap(tWindowId);
    DrawPokenavCallTextBoxBorder(tWindowId, TILE_MC_WINDOW, 14);
    WriteSequenceToBgTilemapBuffer(0, (0xF << 12) | TILE_POKENAV_ICON, 1, 15, 4, 4, 17, 1);
    tIconTaskId = CreateTask(Task_SpinPokenavIcon, 10);
    CopyWindowToVram(tWindowId, COPYWIN_GFX);
    CopyBgTilemapBufferToVram(0);
    return TRUE;
}

static bool32 PokenavCall_ReadyIntro(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!IsDma3ManagerBusyWithBgCopy())
    {
        // Note that "..." is not printed yet, just readied
        InitPokenavCallTextPrinter(tWindowId, sText_PokenavCallEllipsis);
        return TRUE;
    }

    return FALSE;
}

static bool32 PokenavCall_SlideWindowIn(u8 taskId)
{
    if (ChangeBgY(0, 0x600, BG_COORD_ADD) >= 0)
    {
        ChangeBgY(0, 0, BG_COORD_SET);
        return TRUE;
    }

    return FALSE;
}

static bool32 PokenavCall_PrintIntro(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!RunPokenavCallTextPrinter(tWindowId))
    {
        FillWindowPixelBuffer(tWindowId, PIXEL_FILL(8));

        if (IsSpeakerBuffered(gStringVar4))
            TrySpawnAndShowNamebox(gSpeakerName, NAME_BOX_BASE_TILE_NUM);

        InitPokenavCallTextPrinter(tWindowId, gStringVar4);
        return TRUE;
    }

    return FALSE;
}

static bool32 PokenavCall_PrintMessage(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (!RunPokenavCallTextPrinter(tWindowId) && !IsSEPlaying() && JOY_NEW(A_BUTTON | B_BUTTON))
    {
        FillWindowPixelBuffer(tWindowId, PIXEL_FILL(8));
        CopyWindowToVram(tWindowId, COPYWIN_GFX);
        PlaySE(SE_POKENAV_HANG_UP);
        return TRUE;
    }

    return FALSE;
}

static bool32 PokenavCall_SlideWindowOut(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (ChangeBgY(0, 0x600, BG_COORD_SUB) <= -0x4000)
    {
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 12, 30, 8);
        DestroyTask(tIconTaskId);
        RemoveWindow(tWindowId);
        CopyBgTilemapBufferToVram(0);
        return TRUE;
    }

    return FALSE;
}

static bool32 PokenavCall_EndCall(u8 taskId)
{
    if (!IsDma3ManagerBusyWithBgCopy() && !IsSEPlaying())
    {
        DestroyNamebox();
        ChangeBgY(0, 0, BG_COORD_SET);
        return TRUE;
    }

    return FALSE;
}

static void DrawPokenavCallTextBoxBorder(u32 windowId, u32 tileOffset, u32 paletteId)
{
    int bg, x, y, width, height;
    int tileNum;

    bg = GetWindowAttribute(windowId, WINDOW_BG);
    x = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    y = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    height = GetWindowAttribute(windowId, WINDOW_HEIGHT);
    tileNum = tileOffset + GetBgAttribute(bg, BG_ATTR_BASETILE);

    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 0), x - 1, y - 1, 1, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 1), x, y - 1, width, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 2), x + width, y - 1, 1, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 3), x - 1, y, 1, height);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 4), x + width, y, 1, height);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 5), x - 1, y + height, 1, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 6), x, y + height, width, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 7), x + width, y + height, 1, 1);
}

static u8 GetPokenavCallWindowId(void)
{
    if (!IsPokenavCallTaskActive())
        return WINDOW_NONE;

    u32 taskId = FindTaskIdByFunc(ExecutePokenavCall);
    return gTasks[taskId].tWindowId;
}

// redraw only the top-half
void RedrawPokenavCallTextBoxBorder(void)
{
    u32 windowId = GetPokenavCallWindowId();
    u32 bg = GetWindowAttribute(windowId, WINDOW_BG);
    u32 x = GetWindowAttribute(windowId, WINDOW_TILEMAP_LEFT);
    u32 y = GetWindowAttribute(windowId, WINDOW_TILEMAP_TOP);
    u32 width = GetWindowAttribute(windowId, WINDOW_WIDTH);
    u32 tileNum = TILE_MC_WINDOW + GetBgAttribute(bg, BG_ATTR_BASETILE);
    u32 paletteId = 14;

    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 0), x - 1,     y - 1, 1,     1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 1), x,         y - 1, width, 1);
    FillBgTilemapBufferRect_Palette0(bg, ((paletteId << 12) & 0xF000) | (tileNum + 2), x + width, y - 1, 1,     1);
}

#undef tState
#undef tWindowId
#undef tIconTaskId

static void InitPokenavCallTextPrinter(int windowId, const u8 *str)
{
    struct TextPrinterTemplate printerTemplate;
    printerTemplate.currentChar = str;
    printerTemplate.type = WINDOW_TEXT_PRINTER;
    printerTemplate.windowId = windowId;
    printerTemplate.fontId = FONT_NORMAL;
    printerTemplate.x = 32;
    printerTemplate.y = 1;
    printerTemplate.currentX = 32;
    printerTemplate.currentY = 1;
    printerTemplate.letterSpacing = 0;
    printerTemplate.lineSpacing = 0;
    printerTemplate.color.accent = TEXT_COLOR_BLUE;
    printerTemplate.color.foreground = TEXT_DYNAMIC_COLOR_1;
    printerTemplate.color.background = TEXT_COLOR_BLUE;
    printerTemplate.color.shadow = TEXT_DYNAMIC_COLOR_5;
    gTextFlags.useAlternateDownArrow = FALSE;

    AddTextPrinter(&printerTemplate, GetPlayerTextSpeedDelay(), NULL);
}

static bool32 RunPokenavCallTextPrinter(int windowId)
{
    if (JOY_HELD(A_BUTTON))
        gTextFlags.canABSpeedUpPrint = TRUE;
    else
        gTextFlags.canABSpeedUpPrint = FALSE;

    RunTextPrinters();
    return IsTextPrinterActiveOnWindow(windowId);
}

#define tTimer     data[0]
#define tSpinStage data[1]
#define tTileNum   data[2]

static void Task_SpinPokenavIcon(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (++tTimer > 8)
    {
        tTimer = 0;
        if (++tSpinStage > 7)
            tSpinStage = 0;

        tTileNum = (tSpinStage * 16) + TILE_POKENAV_ICON;
        WriteSequenceToBgTilemapBuffer(0, tTileNum | ~0xFFF, 1, 15, 4, 4, 17, 1);
        CopyBgTilemapBufferToVram(0);
    }
}

#undef tTimer
#undef tSpinStage
#undef tTileNum
