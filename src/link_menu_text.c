#include "global.h"
#include "bg.h"
#include "link_menu_text.h"
#include "main.h"
#include "menu.h"
#include "string_util.h"
#include "text.h"
#include "text_window.h"
#include "window.h"

// Message box, yes/no prompt and list-window helpers shared by the Union Room
// screens. They used to live in the Mystery Gift menu, which is removed.

#define WIN_MSG 1

#define DOWN_ARROW_X 208
#define DOWN_ARROW_Y 20

EWRAM_DATA static u8 sDownArrowCounterAndYCoordIdx[8] = {};

ALIGNED(2) static const u8 sTextColor[] = { TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY };

static const struct WindowTemplate sWindowTemplate_YesNoMsg_Wide = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 15,
    .width = 28,
    .height = 4,
    .paletteNum = 12,
    .baseBlock = 0x00e5
};

static const struct WindowTemplate sWindowTemplate_YesNoMsg = {
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 15,
    .width = 20,
    .height = 4,
    .paletteNum = 12,
    .baseBlock = 0x00e5
};

static const struct WindowTemplate sWindowTemplate_YesNoBox = {
    .bg = 0,
    .tilemapLeft = 23,
    .tilemapTop = 15,
    .width = 6,
    .height = 4,
    .paletteNum = 12,
    .baseBlock = 0x0155
};

void MG_DrawTextBorder(u8 windowId)
{
    DrawTextBorderOuter(windowId, 0x01, 0xF);
}

void MG_AddMessageTextPrinter(const u8 *str)
{
    StringExpandPlaceholders(gStringVar4, str);
    FillWindowPixelBuffer(WIN_MSG, 0x11);
    AddTextPrinterParameterized4(WIN_MSG, FONT_NORMAL, 0, 1, 0, 0, sTextColor, 0, gStringVar4);
    DrawTextBorderOuter(WIN_MSG, 0x001, 0xF);
    PutWindowTilemap(WIN_MSG);
    CopyWindowToVram(WIN_MSG, COPYWIN_FULL);
}

static void ClearMessage(void)
{
    rbox_fill_rectangle(WIN_MSG);
    ClearWindowTilemap(WIN_MSG);
    CopyWindowToVram(WIN_MSG, COPYWIN_MAP);
}

bool32 PrintMysteryGiftMenuMessage(u8 *textState, const u8 *str)
{
    switch (*textState)
    {
    case 0:
        MG_AddMessageTextPrinter(str);
        (*textState)++;
        break;
    case 1:
        DrawDownArrow(WIN_MSG, DOWN_ARROW_X, DOWN_ARROW_Y, 1, FALSE, &sDownArrowCounterAndYCoordIdx[0], &sDownArrowCounterAndYCoordIdx[1]);
        if (JOY_NEW(A_BUTTON | B_BUTTON))
            (*textState)++;
        break;
    case 2:
        DrawDownArrow(WIN_MSG, DOWN_ARROW_X, DOWN_ARROW_Y, 1, TRUE, &sDownArrowCounterAndYCoordIdx[0], &sDownArrowCounterAndYCoordIdx[1]);
        *textState = 0;
        ClearMessage();
        return TRUE;
    case 0xFF:
        *textState = 2;
        return FALSE;
    }
    return FALSE;
}

s8 DoMysteryGiftYesNo(u8 *textState, u16 *windowId, bool8 yesNoBoxPlacement, const u8 *str)
{
    struct WindowTemplate windowTemplate;
    s8 input;

    switch (*textState)
    {
    case 0:
        // Print question message
        StringExpandPlaceholders(gStringVar4, str);
        if (!yesNoBoxPlacement)
            *windowId = AddWindow(&sWindowTemplate_YesNoMsg_Wide);
        else
            *windowId = AddWindow(&sWindowTemplate_YesNoMsg);
        FillWindowPixelBuffer(*windowId, 0x11);
        AddTextPrinterParameterized4(*windowId, FONT_NORMAL, 0, 1, 0, 0, sTextColor, 0, gStringVar4);
        DrawTextBorderOuter(*windowId, 0x001, 0x0F);
        CopyWindowToVram(*windowId, COPYWIN_GFX);
        PutWindowTilemap(*windowId);
        (*textState)++;
        break;
    case 1:
        // Create Yes/No
        windowTemplate = sWindowTemplate_YesNoBox;
        if (!yesNoBoxPlacement)
            windowTemplate.tilemapTop = 9;
        else
            windowTemplate.tilemapTop = 15;
        CreateYesNoMenu(&windowTemplate, 10, 14, 0);
        (*textState)++;
        break;
    case 2:
        // Handle Yes/No input
        input = Menu_ProcessInputNoWrapClearOnChoose();
        if (input == MENU_B_PRESSED || input == 0 || input == 1)
        {
            *textState = 0;
            rbox_fill_rectangle(*windowId);
            ClearWindowTilemap(*windowId);
            CopyWindowToVram(*windowId, COPYWIN_MAP);
            RemoveWindow(*windowId);
            return input;
        }
        break;
    case 0xFF:
        *textState = 0;
        rbox_fill_rectangle(*windowId);
        ClearWindowTilemap(*windowId);
        CopyWindowToVram(*windowId, COPYWIN_MAP);
        RemoveWindow(*windowId);
        return MENU_B_PRESSED;
    }

    return MENU_NOTHING_CHOSEN;
}

u16 GetMysteryGiftBaseBlock(void)
{
    return 0x1A9;
}
