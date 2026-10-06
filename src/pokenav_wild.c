#include "global.h"
#include "battle_main.h"
#include "bg.h"
#include "decompress.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "pokenav.h"
#include "region_map.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "text.h"
#include "wild_places.h"
#include "wild_roster.h"
#include "window.h"
#include "constants/rgb.h"
#include "constants/songs.h"

// The PokeNav's Wild Pokemon list: every Pokemon a map cell's maps hold right
// now (include/wild_places.h), one heading per map and method, each over
// rows of icons. It covers the zoomed map in one framed window on BG1, over
// tiles the zoomed map's info window and city tiles use (they are put back
// on close), with the map dimmed behind it and the header slid away.
//
// Switching between the zoom view and the list never shows a tile the other
// screen is still drawing: the frame first shows empty (plain white tiles),
// then the new screen's tiles are written, then its tilemap goes up.

#define WILD_PALETTE        5
#define FRAME_TILE          0x42 // The info window's frame (LoadPokenavRegionMapGfx)
#define FRAME_PALETTE       4
#define BLANK_TILE          0x41 // White (LoadPokenavRegionMapGfx)
#define BLANK_PALETTE       1

#define FRAME_LEFT          1
#define FRAME_TOP           1
#define FRAME_WIDTH         28
#define FRAME_HEIGHT        16
#define WINDOW_PIXEL_WIDTH  (FRAME_WIDTH * 8)

// The title window's rule sits under the cell's name.
#define TITLE_RULE_Y        15
// Pixel rows inside the body window, which starts at screen y 24.
#define BODY_SCREEN_Y       24
#define LIST_Y              0
#define LIST_HEIGHT         80
#define RULE_Y              81
#define DETAIL_Y            82
#define DETAIL_LINE_2_Y     96
#define DETAIL_HEIGHT       30

// Text and icon cells start a little in from the frame. Each icon has a whole
// 32x32 cell (WILD_LIST_ICON_PITCH, WILD_LIST_ROW_HEIGHT in include/pokenav.h),
// so no two icons can touch, and a heading's text never reaches the cells
// under it. The cells leave a gutter on the right for the scroll arrows.
#define CONTENT_LEFT        4
#define CONTENT_WIDTH       (WINDOW_PIXEL_WIDTH - 2 * CONTENT_LEFT)
#define HEADING_HEIGHT      16
#define ICONS_PER_ROW       6
#define GRID_WIDTH          (ICONS_PER_ROW * WILD_LIST_ICON_PITCH)
#define ARROW_X             (8 + (CONTENT_LEFT + GRID_WIDTH + WINDOW_PIXEL_WIDTH) / 2) // screen x, mid-gutter
STATIC_ASSERT(CONTENT_LEFT + GRID_WIDTH + 16 <= WINDOW_PIXEL_WIDTH, wildListArrowGutterFits);
#define MAX_VISIBLE_ROWS    (LIST_HEIGHT / WILD_LIST_ROW_HEIGHT)
#define MAX_VISIBLE_ICONS   (MAX_VISIBLE_ROWS * ICONS_PER_ROW)

#define LINE_HEADING        0xFF

#define TAG_WILD_BALL       0x5A40
#define TAG_WILD_SCROLL     0x5A41
#define TAG_MOVE_TYPES      30002 // gSpriteSheet_MoveTypes

// Palette colors: the standard menu palette, plus the cursor's highlight.
#define COLOR_BG            TEXT_COLOR_WHITE
#define COLOR_RULE          TEXT_COLOR_LIGHT_GRAY
#define COLOR_BLACK         10
#define COLOR_HIGHLIGHT     11

enum {
    WIN_TITLE,
    WIN_BODY,
    WIN_COUNT,
};

struct WildListLine
{
    u8 section;
    u8 row; // LINE_HEADING for the section's heading
};

struct Pokenav_WildList
{
    struct WildPlaceList list;
    struct WildListLine *lines;
    u16 lineCount;
    u16 topLine;
    u16 maxTopLine;
    u16 scrollOffset; // topLine, for the scroll arrows
    u8 cursorSection;
    u8 cursorIndex;
    u8 windowIds[WIN_COUNT];
    u8 iconCount;
    u8 iconSpriteIds[MAX_VISIBLE_ICONS];
    u8 ballSpriteIds[MAX_VISIBLE_ICONS];
    u16 iconEntries[MAX_VISIBLE_ICONS];
    u8 typeSpriteIds[2];
    u8 typeCount;
    u8 scrollArrowsTaskId;
    bool8 spritesHidden;
};

static const struct WindowTemplate sWildWindowTemplates[WIN_COUNT] =
{
    [WIN_TITLE] = {
        .bg = 1,
        .tilemapLeft = FRAME_LEFT,
        .tilemapTop = FRAME_TOP,
        .width = FRAME_WIDTH,
        .height = 2,
        .paletteNum = WILD_PALETTE,
        .baseBlock = 0x000, // Over the city tiles
    },
    [WIN_BODY] = {
        .bg = 1,
        .tilemapLeft = FRAME_LEFT,
        .tilemapTop = FRAME_TOP + 2,
        .width = FRAME_WIDTH,
        .height = FRAME_HEIGHT - 2,
        .paletteNum = WILD_PALETTE,
        .baseBlock = 0x04C, // Over the info window
    },
};

static const u8 sColors_Black[] = {COLOR_BG, COLOR_BLACK, TEXT_COLOR_LIGHT_GRAY};
static const u8 sColors_Gray[]  = {COLOR_BG, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
static const u8 sColors_Blue[]  = {COLOR_BG, TEXT_COLOR_BLUE, TEXT_COLOR_LIGHT_BLUE};

static const u16 sHighlightColor = RGB(31, 28, 17);

static const u8 sCaughtBall_Gfx[] = INCGFX_U8("graphics/pokedex/caught_ball.png", ".4bpp");
static const u16 sCaughtBall_Pal[] = INCGFX_U16("graphics/pokedex/caught_ball.png", ".gbapal");

static const struct SpriteSheet sCaughtBallSheet = {sCaughtBall_Gfx, sizeof(sCaughtBall_Gfx), TAG_WILD_BALL};
static const struct SpritePalette sCaughtBallPalette = {sCaughtBall_Pal, TAG_WILD_BALL};

static const struct OamData sCaughtBallOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 1,
};

static const struct SpriteTemplate sCaughtBallTemplate =
{
    .tileTag = TAG_WILD_BALL,
    .paletteTag = TAG_WILD_BALL,
    .oam = &sCaughtBallOam,
    .anims = gDummySpriteAnimTable,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static struct Pokenav_WildList *GetWildList(void)
{
    return GetSubstructPtr(POKENAV_SUBSTRUCT_WILD_LIST);
}

static u32 GetSectionRows(const struct WildPlaceSection *section)
{
    return (section->count + ICONS_PER_ROW - 1) / ICONS_PER_ROW;
}

static u32 GetLineHeight(const struct WildListLine *line)
{
    return line->row == LINE_HEADING ? HEADING_HEIGHT : WILD_LIST_ROW_HEIGHT;
}

static bool32 BuildLines(struct Pokenav_WildList *wild)
{
    u32 count = 0, line = 0;

    for (u32 i = 0; i < wild->list.sectionCount; i++)
        count += 1 + GetSectionRows(&wild->list.sections[i]);
    wild->lines = Alloc(count * sizeof(*wild->lines));
    if (wild->lines == NULL)
        return FALSE;
    for (u32 i = 0; i < wild->list.sectionCount; i++)
    {
        wild->lines[line++] = (struct WildListLine){i, LINE_HEADING};
        for (u32 row = 0; row < GetSectionRows(&wild->list.sections[i]); row++)
            wild->lines[line++] = (struct WildListLine){i, row};
    }
    wild->lineCount = count;

    // The last top line that still fills the list to its end.
    wild->maxTopLine = count;
    for (u32 height = 0; wild->maxTopLine > 0; wild->maxTopLine--)
    {
        height += GetLineHeight(&wild->lines[wild->maxTopLine - 1]);
        if (height > LIST_HEIGHT)
            break;
    }
    return TRUE;
}

// How many lines show from the top line: only whole lines, and never a
// heading at the bottom without its first row of icons.
static u32 GetVisibleLineCount(struct Pokenav_WildList *wild, u32 topLine)
{
    u32 count = 0, height = 0;

    while (topLine + count < wild->lineCount)
    {
        height += GetLineHeight(&wild->lines[topLine + count]);
        if (height > LIST_HEIGHT)
            break;
        count++;
    }
    if (count > 1 && wild->lines[topLine + count - 1].row == LINE_HEADING)
        count--;
    return count;
}

static u32 GetVisibleHeight(struct Pokenav_WildList *wild, u32 topLine)
{
    u32 height = 0, count = GetVisibleLineCount(wild, topLine);

    for (u32 i = 0; i < count; i++)
        height += GetLineHeight(&wild->lines[topLine + i]);
    return height;
}

static u32 GetCursorLine(struct Pokenav_WildList *wild)
{
    for (u32 i = 0; i < wild->lineCount; i++)
    {
        if (wild->lines[i].section == wild->cursorSection && wild->lines[i].row == wild->cursorIndex / ICONS_PER_ROW)
            return i;
    }
    return 0;
}

static bool32 IsLineVisible(struct Pokenav_WildList *wild, u32 topLine, u32 line)
{
    return line >= topLine && line < topLine + GetVisibleLineCount(wild, topLine);
}

// Scrolls just enough to show the cursor; after a jump to a section, puts
// its heading at the top, then pulls earlier lines in while they still fit.
static void ScrollToCursor(struct Pokenav_WildList *wild, bool32 headingOnTop)
{
    u32 cursorLine = GetCursorLine(wild);

    // A section's first row brings its heading with it.
    if (headingOnTop && cursorLine > 0)
        wild->topLine = cursorLine - 1;
    else if (wild->cursorIndex < ICONS_PER_ROW && cursorLine > 0 && cursorLine - 1 < wild->topLine)
        wild->topLine = cursorLine - 1;
    else if (cursorLine < wild->topLine)
        wild->topLine = cursorLine;
    while (!IsLineVisible(wild, wild->topLine, cursorLine))
        wild->topLine++;
    while (wild->topLine > 0
        && GetVisibleHeight(wild, wild->topLine) + GetLineHeight(&wild->lines[wild->topLine - 1]) <= LIST_HEIGHT
        && IsLineVisible(wild, wild->topLine - 1, cursorLine)
        && GetVisibleLineCount(wild, wild->topLine - 1) > GetVisibleLineCount(wild, wild->topLine))
        wild->topLine--;
    if (wild->topLine > wild->maxTopLine)
        wild->topLine = wild->maxTopLine;
    wild->scrollOffset = wild->topLine;
}

static const struct WildRosterEntry *GetCursorEntry(struct Pokenav_WildList *wild)
{
    const struct WildPlaceSection *section = &wild->list.sections[wild->cursorSection];
    return &wild->list.entries[section->first + wild->cursorIndex];
}

// Prints text cut to width: a narrower font first, then an ellipsis.
static void PrintFitted(u32 windowId, u32 fontId, const u8 *str, u32 x, u32 y, u32 width, const u8 *colors, bool32 alignRight)
{
    u8 text[64];
    u32 length;

    length = min(StringLength(str), ARRAY_COUNT(text) - 1);
    memcpy(text, str, length);
    text[length] = EOS;
    fontId = GetFontIdToFit(text, fontId, 0, width);
    while (length > 1 && GetStringWidth(fontId, text, 0) > width)
    {
        text[--length] = EOS;
        text[length - 1] = CHAR_ELLIPSIS;
    }
    if (alignRight)
        x += width - GetStringWidth(fontId, text, 0);
    AddTextPrinterParameterized3(windowId, fontId, x, y, colors, TEXT_SKIP_DRAW, text);
}

static bool32 IsHeadingVisible(struct Pokenav_WildList *wild, u32 section)
{
    for (u32 i = 0; i < wild->lineCount; i++)
    {
        if (wild->lines[i].section == section && wild->lines[i].row == LINE_HEADING)
            return IsLineVisible(wild, wild->topLine, i);
    }
    return FALSE;
}

// The cell's name, and on the right the place the highlighted section is in
// ("Mt. Pyre 3F") while its heading has scrolled out of sight; blank in the
// cell's own main map.
static void DrawTitle(struct Pokenav_WildList *wild)
{
    u32 windowId = wild->windowIds[WIN_TITLE];
    const struct WildPlaceSection *section = &wild->list.sections[wild->cursorSection];
    u32 nameWidth;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(COLOR_BG));
    FillWindowPixelRect(windowId, PIXEL_FILL(COLOR_RULE), 0, TITLE_RULE_Y, WINDOW_PIXEL_WIDTH, 1);
    GetMapName(gStringVar1, wild->list.cell, 0);
    PrintFitted(windowId, FONT_NORMAL, gStringVar1, CONTENT_LEFT, 0, CONTENT_WIDTH, sColors_Black, FALSE);
    nameWidth = min(GetStringWidth(GetFontIdToFit(gStringVar1, FONT_NORMAL, 0, CONTENT_WIDTH), gStringVar1, 0), CONTENT_WIDTH);
    GetWildPlaceLabel(gStringVar2, wild->list.cell, section->mapGroup, section->mapNum);
    if (gStringVar2[0] != EOS && !IsHeadingVisible(wild, wild->cursorSection) && nameWidth + 12 < CONTENT_WIDTH)
        PrintFitted(windowId, FONT_NORMAL, gStringVar2, CONTENT_LEFT + nameWidth + 12, 0, CONTENT_WIDTH - nameWidth - 12, sColors_Gray, TRUE);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void DestroyIcons(struct Pokenav_WildList *wild)
{
    for (u32 i = 0; i < wild->iconCount; i++)
    {
        FreeAndDestroyMonIconSprite(&gSprites[wild->iconSpriteIds[i]]);
        if (wild->ballSpriteIds[i] != MAX_SPRITES)
            DestroySprite(&gSprites[wild->ballSpriteIds[i]]);
    }
    wild->iconCount = 0;
}

static void CreateIcon(struct Pokenav_WildList *wild, u32 entryId, u32 column, u32 y)
{
    enum Species species = wild->list.entries[entryId].species;
    s16 x = 8 + CONTENT_LEFT + column * WILD_LIST_ICON_PITCH + WILD_LIST_ICON_PITCH / 2;
    s16 iconY = BODY_SCREEN_Y + LIST_Y + y + 16;
    u32 icon = wild->iconCount++;

    wild->iconEntries[icon] = entryId;
    wild->iconSpriteIds[icon] = CreateMonIcon(species, SpriteCallbackDummy, x, iconY, 1, 0);
    gSprites[wild->iconSpriteIds[icon]].invisible = wild->spritesHidden;
    wild->ballSpriteIds[icon] = MAX_SPRITES;
    // Caught ones carry the Pokedex's caught ball in their cell's corner.
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT))
    {
        wild->ballSpriteIds[icon] = CreateSprite(&sCaughtBallTemplate, x + 12, iconY + 12, 0);
        gSprites[wild->ballSpriteIds[icon]].invisible = wild->spritesHidden;
    }
}

// Redraws the scrolling list: headings, the cursor's highlight, and icons
// for the rows that show when the list has scrolled.
static void DrawList(struct Pokenav_WildList *wild, bool32 scrolled)
{
    u32 windowId = wild->windowIds[WIN_BODY];
    u32 visible = GetVisibleLineCount(wild, wild->topLine);
    const struct WildPlaceSection *cursorSection = &wild->list.sections[wild->cursorSection];
    u32 y = 0;

    if (scrolled)
        DestroyIcons(wild);
    FillWindowPixelRect(windowId, PIXEL_FILL(COLOR_BG), 0, LIST_Y, WINDOW_PIXEL_WIDTH, LIST_HEIGHT);
    for (u32 i = 0; i < visible; i++)
    {
        const struct WildListLine *line = &wild->lines[wild->topLine + i];
        const struct WildPlaceSection *section = &wild->list.sections[line->section];

        if (line->row == LINE_HEADING)
        {
            GetWildPlaceSectionName(gStringVar4, wild->list.cell, section);
            PrintFitted(windowId, FONT_NORMAL, gStringVar4, CONTENT_LEFT, LIST_Y + y + 1, GRID_WIDTH, sColors_Blue, FALSE);
        }
        else
        {
            u32 first = line->row * ICONS_PER_ROW;
            u32 count = min(section->count - first, ICONS_PER_ROW);

            for (u32 column = 0; column < count; column++)
            {
                if (line->section == wild->cursorSection && first + column == wild->cursorIndex)
                    FillWindowPixelRect(windowId, PIXEL_FILL(COLOR_HIGHLIGHT), CONTENT_LEFT + column * WILD_LIST_ICON_PITCH + 1, LIST_Y + y + 1,
                                        WILD_LIST_ICON_PITCH - 2, WILD_LIST_ROW_HEIGHT - 2);
                if (scrolled && wild->iconCount < MAX_VISIBLE_ICONS)
                    CreateIcon(wild, section->first + first + column, column, y);
            }
        }
        y += GetLineHeight(line);
    }

    // Only the highlighted Pokemon moves.
    for (u32 i = 0; i < wild->iconCount; i++)
    {
        struct Sprite *icon = &gSprites[wild->iconSpriteIds[i]];
        icon->callback = wild->iconEntries[i] == cursorSection->first + wild->cursorIndex ? SpriteCB_MonIcon : SpriteCallbackDummy;
    }
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void SetTypeIcon(struct Pokenav_WildList *wild, u32 slot, enum Type type, s16 x, s16 y)
{
    struct Sprite *sprite = &gSprites[wild->typeSpriteIds[slot]];

    StartSpriteAnim(sprite, type);
    sprite->oam.paletteNum = gTypesInfo[type].palette;
    sprite->x = x + 16;
    sprite->y = y + 8;
    sprite->invisible = wild->spritesHidden;
}

// The highlighted Pokemon: its name and types, then how it is found here.
static void DrawDetail(struct Pokenav_WildList *wild)
{
    u32 windowId = wild->windowIds[WIN_BODY];
    const struct WildRosterEntry *entry = GetCursorEntry(wild);
    const struct WildPlaceSection *section = &wild->list.sections[wild->cursorSection];
    enum Type type1 = GetSpeciesType(entry->species, 0), type2 = GetSpeciesType(entry->species, 1);
    u32 nameWidth;
    u8 *end;

    FillWindowPixelRect(windowId, PIXEL_FILL(COLOR_BG), 0, DETAIL_Y, WINDOW_PIXEL_WIDTH, DETAIL_HEIGHT);
    PrintFitted(windowId, FONT_NORMAL, GetSpeciesName(entry->species), CONTENT_LEFT, DETAIL_Y, 120, sColors_Black, FALSE);
    nameWidth = min(GetStringWidth(FONT_NORMAL, GetSpeciesName(entry->species), 0), 120);
    SetTypeIcon(wild, 0, type1, 8 + CONTENT_LEFT + nameWidth + 4, BODY_SCREEN_Y + DETAIL_Y + 1);
    wild->typeCount = 1;
    if (type2 != type1)
    {
        SetTypeIcon(wild, 1, type2, 8 + CONTENT_LEFT + nameWidth + 4 + 34, BODY_SCREEN_Y + DETAIL_Y + 1);
        wild->typeCount = 2;
    }
    else
    {
        gSprites[wild->typeSpriteIds[1]].invisible = TRUE;
    }

    end = StringCopy(gStringVar4, GetWildRosterMethodName(section->method, section->mapGroup, section->mapNum));
    end = StringCopy(end, COMPOUND_STRING(" · "));
    end = StringCopy(end, GetWildRosterRarityName(entry->rarity));
    end = StringCopy(end, COMPOUND_STRING(" · Lv. "));
    end = ConvertIntToDecimalStringN(end, entry->minLevel, STR_CONV_MODE_LEFT_ALIGN, 3);
    if (entry->maxLevel != entry->minLevel)
    {
        *end++ = CHAR_HYPHEN;
        end = ConvertIntToDecimalStringN(end, entry->maxLevel, STR_CONV_MODE_LEFT_ALIGN, 3);
    }
    PrintFitted(windowId, FONT_NORMAL, gStringVar4, CONTENT_LEFT, DETAIL_LINE_2_Y, CONTENT_WIDTH, sColors_Gray, FALSE);
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void ShowSprites(struct Pokenav_WildList *wild)
{
    wild->spritesHidden = FALSE;
    for (u32 i = 0; i < wild->iconCount; i++)
    {
        gSprites[wild->iconSpriteIds[i]].invisible = FALSE;
        if (wild->ballSpriteIds[i] != MAX_SPRITES)
            gSprites[wild->ballSpriteIds[i]].invisible = FALSE;
    }
    for (u32 i = 0; i < wild->typeCount; i++)
        gSprites[wild->typeSpriteIds[i]].invisible = FALSE;
}

// The list's frame around plain white: shown between the two screens.
static void DrawEmptyFrame(void)
{
    u32 left = FRAME_LEFT, top = FRAME_TOP, width = FRAME_WIDTH, height = FRAME_HEIGHT;

    FillBgTilemapBufferRect(1, 0x40, 0, 0, 32, 32, BLANK_PALETTE);
    FillBgTilemapBufferRect(1, BLANK_TILE,     left,         top,          width, height, BLANK_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 0, left - 1,     top - 1,      1,     1,      FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 1, left,         top - 1,      width, 1,      FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 2, left + width, top - 1,      1,     1,      FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 3, left - 1,     top,          1,     height, FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 5, left + width, top,          1,     height, FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 6, left - 1,     top + height, 1,     1,      FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 7, left,         top + height, width, 1,      FRAME_PALETTE);
    FillBgTilemapBufferRect(1, FRAME_TILE + 8, left + width, top + height, 1,     1,      FRAME_PALETTE);
    CopyBgTilemapBufferToVram(1);
}

static void FreeWildListResources(struct Pokenav_WildList *wild)
{
    DestroyIcons(wild);
    for (u32 i = 0; i < ARRAY_COUNT(wild->typeSpriteIds); i++)
    {
        if (wild->typeSpriteIds[i] != MAX_SPRITES)
            DestroySprite(&gSprites[wild->typeSpriteIds[i]]);
    }
    FreeSpriteTilesByTag(TAG_MOVE_TYPES);
    FreeSpriteTilesByTag(TAG_WILD_BALL);
    FreeSpritePaletteByTag(TAG_WILD_BALL);
    FreeMonIconPalettes();
    if (wild->scrollArrowsTaskId != TASK_NONE)
        RemoveScrollIndicatorArrowPair(wild->scrollArrowsTaskId);
    for (u32 i = 0; i < WIN_COUNT; i++)
    {
        if (wild->windowIds[i] != WINDOW_NONE)
            RemoveWindow(wild->windowIds[i]);
    }
    FreeWildPlaceList(&wild->list);
    TRY_FREE_AND_SET_NULL(wild->lines);
}

u32 LoopedTask_OpenWildList(s32 taskState)
{
    struct Pokenav_WildList *wild = GetWildList();
    struct RegionMap *regionMap;

    switch (taskState)
    {
    case 0:
        PlaySE(SE_SELECT);
        wild = AllocSubstruct(POKENAV_SUBSTRUCT_WILD_LIST, sizeof(*wild));
        if (wild == NULL)
            return LT_FINISH;
        memset(wild, 0, sizeof(*wild));
        wild->windowIds[WIN_TITLE] = wild->windowIds[WIN_BODY] = WINDOW_NONE;
        wild->typeSpriteIds[0] = wild->typeSpriteIds[1] = MAX_SPRITES;
        wild->scrollArrowsTaskId = TASK_NONE;
        wild->spritesHidden = TRUE;
        regionMap = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP);
        if (!BuildWildPlaceList(GetRegionMapCell(regionMap->mapSecId), &wild->list)
         || wild->list.sectionCount == 0 || !BuildLines(wild))
        {
            FreeWildPlaceList(&wild->list);
            FreePokenavSubstruct(POKENAV_SUBSTRUCT_WILD_LIST);
            return LT_FINISH;
        }
        // Finish sliding the header away before covering the zoom view.
        HideMapHeader();
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsMapHeaderMoving())
            return LT_PAUSE;
        // The zoom view gives way to the list's empty frame first.
        HideRegionMapZoomView();
        DrawEmptyFrame();
        return LT_INC_AND_PAUSE;
    case 2:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        // Nothing on screen uses the info window's tiles now: draw over them.
        LoadPalette(gStandardMenuPalette, BG_PLTT_ID(WILD_PALETTE), PLTT_SIZE_4BPP);
        LoadPalette(&sHighlightColor, BG_PLTT_ID(WILD_PALETTE) + COLOR_HIGHLIGHT, PLTT_SIZEOF(1));
        for (u32 i = 0; i < WIN_COUNT; i++)
        {
            wild->windowIds[i] = AddWindow(&sWildWindowTemplates[i]);
            FillWindowPixelBuffer(wild->windowIds[i], PIXEL_FILL(COLOR_BG));
        }
        FillWindowPixelRect(wild->windowIds[WIN_BODY], PIXEL_FILL(COLOR_RULE), 0, RULE_Y, WINDOW_PIXEL_WIDTH, 1);

        LoadMonIconPalettes();
        LoadSpriteSheet(&sCaughtBallSheet);
        LoadSpritePalette(&sCaughtBallPalette);
        LoadCompressedSpriteSheet(&gSpriteSheet_MoveTypes);
        LoadPalette(gMoveTypes_Pal, OBJ_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
        for (u32 i = 0; i < ARRAY_COUNT(wild->typeSpriteIds); i++)
        {
            wild->typeSpriteIds[i] = CreateSprite(&gSpriteTemplate_MoveTypes, 0, 0, 2);
            gSprites[wild->typeSpriteIds[i]].invisible = TRUE;
        }

        ScrollToCursor(wild, FALSE);
        DrawTitle(wild);
        DrawList(wild, TRUE);
        DrawDetail(wild);
        return LT_INC_AND_PAUSE;
    case 3:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        // Its tiles are in: up goes the list, with its icons.
        for (u32 i = 0; i < WIN_COUNT; i++)
            PutWindowTilemap(wild->windowIds[i]);
        CopyBgTilemapBufferToVram(1);
        ShowSprites(wild);
        wild->scrollArrowsTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, ARROW_X, BODY_SCREEN_Y + LIST_Y + 8,
                                     BODY_SCREEN_Y + LIST_Y + LIST_HEIGHT - 8, wild->maxTopLine, TAG_WILD_SCROLL, TAG_WILD_SCROLL, &wild->scrollOffset);
        PrintHelpBarText(HELPBAR_WILD_LIST);
        return LT_INC_AND_PAUSE;
    case 4:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        // The map dims behind the list once the list is up.
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG2 | BLDCNT_EFFECT_DARKEN);
        SetGpuReg(REG_OFFSET_BLDY, 6);
        return LT_INC_AND_PAUSE;
    case 5:
        if (IsMapHeaderMoving())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

u32 LoopedTask_CloseWildList(s32 taskState)
{
    struct Pokenav_WildList *wild = GetWildList();

    switch (taskState)
    {
    case 0:
        PlaySE(SE_SELECT);
        if (wild != NULL)
        {
            // The list gives way to its empty frame first.
            FreeWildListResources(wild);
            FreePokenavSubstruct(POKENAV_SUBSTRUCT_WILD_LIST);
        }
        DrawEmptyFrame();
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy())
            return LT_PAUSE;
        // Nothing on screen uses the list's tiles now: the zoom view returns.
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetGpuReg(REG_OFFSET_BLDY, 0);
        RestoreRegionMapZoomView();
        UpdateRegionMapHelpBarText();
        ShowMapHeader(POKENAV_HEADER_MAP_ZOOMED_IN);
        return LT_INC_AND_PAUSE;
    case 2:
        if (IsDma3ManagerBusyWithBgCopy() || IsMapHeaderMoving())
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

static bool32 MoveCursor(struct Pokenav_WildList *wild, s32 section, s32 index)
{
    if (section < 0 || section >= wild->list.sectionCount)
        return FALSE;
    index = max(0, min(index, wild->list.sections[section].count - 1));
    if (section == wild->cursorSection && index == wild->cursorIndex)
        return FALSE;
    wild->cursorSection = section;
    wild->cursorIndex = index;
    return TRUE;
}

u32 HandleWildListInput(void)
{
    struct Pokenav_WildList *wild = GetWildList();
    const struct WildPlaceSection *sections;
    s32 section, index, row, column;
    bool32 moved = FALSE, jumped = FALSE;
    u32 topLine;

    // A list that could not open goes straight back to the map.
    if (wild == NULL || JOY_NEW(B_BUTTON))
        return POKENAV_MAP_FUNC_CLOSE_WILD_LIST;

    sections = wild->list.sections;
    section = wild->cursorSection;
    index = wild->cursorIndex;
    row = index / ICONS_PER_ROW;
    column = index % ICONS_PER_ROW;
    if (JOY_REPEAT(DPAD_RIGHT))
    {
        if (index + 1 < sections[section].count)
            moved = MoveCursor(wild, section, index + 1);
        else
            moved = MoveCursor(wild, section + 1, 0);
    }
    else if (JOY_REPEAT(DPAD_LEFT))
    {
        if (index > 0)
            moved = MoveCursor(wild, section, index - 1);
        else if (section > 0)
            moved = MoveCursor(wild, section - 1, sections[section - 1].count - 1);
    }
    else if (JOY_REPEAT(DPAD_DOWN))
    {
        if ((u32)row + 1 < GetSectionRows(&sections[section]))
            moved = MoveCursor(wild, section, (row + 1) * ICONS_PER_ROW + column);
        else
            moved = MoveCursor(wild, section + 1, column);
    }
    else if (JOY_REPEAT(DPAD_UP))
    {
        if (row > 0)
            moved = MoveCursor(wild, section, (row - 1) * ICONS_PER_ROW + column);
        else if (section > 0)
            moved = MoveCursor(wild, section - 1, (GetSectionRows(&sections[section - 1]) - 1) * ICONS_PER_ROW + column);
    }
    else if (JOY_REPEAT(R_BUTTON))
    {
        moved = jumped = MoveCursor(wild, section + 1, 0);
    }
    else if (JOY_REPEAT(L_BUTTON))
    {
        moved = jumped = MoveCursor(wild, max(section - 1, 0), 0);
    }

    if (moved)
    {
        PlaySE(SE_SELECT);
        topLine = wild->topLine;
        ScrollToCursor(wild, jumped);
        DrawTitle(wild);
        DrawList(wild, topLine != wild->topLine);
        DrawDetail(wild);
    }
    return POKENAV_MAP_FUNC_NONE;
}

#if TESTING
u32 Test_GetWildListIconsPerRow(void)
{
    return ICONS_PER_ROW;
}
#endif
