#include "global.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "list_menu.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_specialized.h"
#include "move.h"
#include "pokemon.h"
#include "script.h"
#include "sound.h"
#include "string_util.h"
#include "task.h"
#include "constants/characters.h"
#include "constants/emerald_champions.h"
#include "constants/songs.h"

#define CATALOGUE_VISIBLE_SPECIES 6
#define CATALOGUE_LESSONS_PER_PAGE 3

enum { WIN_SPECIES, WIN_LESSONS, WIN_HELP, CATALOGUE_WINDOWS };

struct IconicTutorCatalogue
{
    u16 scrollOffset;
    enum Species species;
    u8 page;
    u8 badges;
    u8 windows[CATALOGUE_WINDOWS];
    u8 listTask;
    u8 arrowTask;
    struct ListMenuItem items[];
};

static EWRAM_DATA struct IconicTutorCatalogue *sCatalogue = NULL;

// The dialogue is closed while these windows use its graphics tiles.
// All three buffers end below the dialogue/standard frame tiles at 0x200.
static const struct WindowTemplate sWindows[CATALOGUE_WINDOWS] =
{
    [WIN_SPECIES] = {0, 1, 1, 11, 12, 15, 1},
    [WIN_LESSONS] = {0, 14, 1, 15, 12, 15, 133},
    [WIN_HELP] = {0, 1, 14, 28, 5, 15, 313},
};

static u32 LessonCount(void)
{
    u32 count = 0;
    while (GetEmeraldChampionsIconicTutorLesson(sCatalogue->species, count, NULL, NULL))
        count++;
    return count;
}

static void DrawLessons(void)
{
    static const u8 readyColors[] = {TEXT_COLOR_WHITE, TEXT_COLOR_GREEN, TEXT_COLOR_LIGHT_GRAY};
    static const u8 lockedColors[] = {TEXT_COLOR_WHITE, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_GRAY};
    u8 text[64];
    u8 *dst;
    u8 window = sCatalogue->windows[WIN_LESSONS];
    u32 pages = (LessonCount() + CATALOGUE_LESSONS_PER_PAGE - 1) / CATALOGUE_LESSONS_PER_PAGE;

    FillWindowPixelBuffer(window, PIXEL_FILL(1));
    dst = StringCopy(text, GetSpeciesName(sCatalogue->species));
    dst = StringCopy(dst, COMPOUND_STRING(" "));
    dst = ConvertIntToDecimalStringN(dst, sCatalogue->page + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
    *dst++ = CHAR_SLASH;
    ConvertIntToDecimalStringN(dst, pages, STR_CONV_MODE_LEFT_ALIGN, 1);
    AddTextPrinterParameterized(window, FONT_SMALL, text, 0, 0, TEXT_SKIP_DRAW, NULL);

    for (u32 row = 0; row < CATALOGUE_LESSONS_PER_PAGE; row++)
    {
        enum Move move;
        u8 badges;
        if (!GetEmeraldChampionsIconicTutorLesson(sCatalogue->species,
                sCatalogue->page * CATALOGUE_LESSONS_PER_PAGE + row, &move, &badges))
            break;
        AddTextPrinterParameterized(window, FONT_SMALL, GetMoveName(move), 0, 16 + row * 24, TEXT_SKIP_DRAW, NULL);
        if (badges == 0)
            StringCopy(text, COMPOUND_STRING("No badges needed"));
        else
        {
            dst = ConvertIntToDecimalStringN(text, badges, STR_CONV_MODE_LEFT_ALIGN, 1);
            dst = StringCopy(dst, badges == 1 ? COMPOUND_STRING(" badge: ") : COMPOUND_STRING(" badges: "));
            StringCopy(dst, badges <= sCatalogue->badges ? COMPOUND_STRING("ready") : COMPOUND_STRING("locked"));
        }
        AddTextPrinterParameterized3(window, FONT_SMALL, 0, 28 + row * 24,
                badges <= sCatalogue->badges ? readyColors : lockedColors, TEXT_SKIP_DRAW, text);
    }
    CopyWindowToVram(window, COPYWIN_GFX);
}

static void MoveSpeciesCursor(s32 species, bool8 onInit, struct ListMenu *list)
{
    ListMenuDefaultCursorMoveFunc(species, onInit, list);
    sCatalogue->species = species;
    sCatalogue->page = 0;
    DrawLessons();
}

static void CloseCatalogue(u8 taskId)
{
    if (sCatalogue != NULL)
    {
        if (sCatalogue->arrowTask != TASK_NONE)
            RemoveScrollIndicatorArrowPair(sCatalogue->arrowTask);
        if (sCatalogue->listTask != TASK_NONE)
            DestroyListMenuTask(sCatalogue->listTask, NULL, NULL);
        for (u32 i = 0; i < CATALOGUE_WINDOWS; i++)
        {
            if (sCatalogue->windows[i] == WINDOW_NONE)
                continue;
            ClearStdWindowAndFrameToTransparent(sCatalogue->windows[i], TRUE);
            RemoveWindow(sCatalogue->windows[i]);
        }
        FREE_AND_SET_NULL(sCatalogue);
    }
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_BrowseCatalogue(u8 taskId)
{
    s32 input = ListMenu_ProcessInput(sCatalogue->listTask);
    ListMenuGetScrollAndRow(sCatalogue->listTask, &sCatalogue->scrollOffset, NULL);
    if (input == LIST_CANCEL)
    {
        PlaySE(SE_SELECT);
        CloseCatalogue(taskId);
        return;
    }
    if (input != LIST_NOTHING_CHOSEN || JOY_NEW(L_BUTTON | R_BUTTON))
    {
        u32 pages = (LessonCount() + CATALOGUE_LESSONS_PER_PAGE - 1) / CATALOGUE_LESSONS_PER_PAGE;
        sCatalogue->page = (sCatalogue->page + (JOY_NEW(L_BUTTON) ? pages - 1 : 1)) % pages;
        PlaySE(SE_SELECT);
        DrawLessons();
    }
}

static void Task_OpenCatalogue(u8 taskId)
{
    u32 count = 0;
    while (GetEmeraldChampionsIconicTutorSpecies(count) != SPECIES_NONE)
        count++;
    sCatalogue = AllocZeroed(sizeof(*sCatalogue) + count * sizeof(struct ListMenuItem));
    if (sCatalogue == NULL)
    {
        CloseCatalogue(taskId);
        return;
    }
    memset(sCatalogue->windows, WINDOW_NONE, sizeof(sCatalogue->windows));
    sCatalogue->listTask = sCatalogue->arrowTask = TASK_NONE;
    if (count == 0)
    {
        CloseCatalogue(taskId);
        return;
    }
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        sCatalogue->badges += FlagGet(flag);
    for (u32 i = 0; i < count; i++)
    {
        enum Species species = GetEmeraldChampionsIconicTutorSpecies(i);
        sCatalogue->items[i] = (struct ListMenuItem){GetSpeciesName(species), species};
        for (u32 j = i; j > 0 && StringCompare(sCatalogue->items[j].name, sCatalogue->items[j - 1].name) < 0; j--)
        {
            struct ListMenuItem swap = sCatalogue->items[j];
            sCatalogue->items[j] = sCatalogue->items[j - 1];
            sCatalogue->items[j - 1] = swap;
        }
    }
    for (u32 i = 0; i < CATALOGUE_WINDOWS; i++)
    {
        sCatalogue->windows[i] = AddWindow(&sWindows[i]);
        if (sCatalogue->windows[i] == WINDOW_NONE)
        {
            CloseCatalogue(taskId);
            return;
        }
        SetStandardWindowBorderStyle(sCatalogue->windows[i], FALSE);
    }
    u8 help = sCatalogue->windows[WIN_HELP];
    AddTextPrinterParameterized(help, FONT_SMALL,
            COMPOUND_STRING("New: " STR(EC_ICONIC_MOVE_CAP_COST) " Bottle Caps / " STR(EC_ICONIC_MOVE_GOLD_CAP_COST) " Gold Bottle Cap"),
            0, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(help, FONT_SMALL, COMPOUND_STRING("Purchased lessons: free to relearn"), 0, 13, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(help, FONT_SMALL, COMPOUND_STRING("A or L/R: moves    B: back"), 0, 26, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(help, COPYWIN_GFX);

    struct ListMenuTemplate list = {
        .items = sCatalogue->items,
        .moveCursorFunc = MoveSpeciesCursor,
        .totalItems = count,
        .maxShowed = min(count, CATALOGUE_VISIBLE_SPECIES),
        .windowId = sCatalogue->windows[WIN_SPECIES],
        .item_X = 8,
        .cursor_X = 0,
        .upText_Y = 1,
        .cursorPal = TEXT_COLOR_DARK_GRAY,
        .fillValue = 1,
        .cursorShadowPal = TEXT_COLOR_LIGHT_GRAY,
        .scrollMultiple = LIST_MULTIPLE_SCROLL_DPAD,
        .fontId = FONT_NORMAL,
        .cursorKind = CURSOR_BLACK_ARROW,
    };
    LockPlayerFieldControls();
    sCatalogue->listTask = ListMenuInit(&list, 0, 0);
    if (count > CATALOGUE_VISIBLE_SPECIES)
        sCatalogue->arrowTask = AddScrollIndicatorArrowPairParameterized(2, 100, 18, 100,
                count - CATALOGUE_VISIBLE_SPECIES, 2000, 100, &sCatalogue->scrollOffset);
    ScheduleBgCopyTilemapToVram(0);
    gTasks[taskId].func = Task_BrowseCatalogue;
}

void ShowIconicTutorCatalogue(void)
{
    CreateTask(Task_OpenCatalogue, 8);
}
