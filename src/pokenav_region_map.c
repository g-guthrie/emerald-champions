#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "landmark.h"
#include "event_data.h"
#include "field_effect.h"
#include "item_use.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokenav.h"
#include "region_map.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text_window.h"
#include "window.h"
#include "wild_places.h"
#include "constants/rgb.h"
#include "constants/songs.h"
#include "constants/region_map_sections.h"

#define GFXTAG_CITY_ZOOM 6
#define PALTAG_CITY_ZOOM 11

#define NUM_CITY_MAPS 22

// What A offers on a place in the zoomed map.
enum {
    PLACE_CHOICE_WILD,
    PLACE_CHOICE_FLY,
    PLACE_CHOICE_FULL_MAP,
    PLACE_CHOICE_CANCEL,
    PLACE_CHOICE_COUNT,
};

struct Pokenav_RegionMapMenu
{
    bool32 zoomDisabled;
    u32 (*callback)(struct Pokenav_RegionMapMenu *);
    u8 choiceCount;
    u8 choices[PLACE_CHOICE_COUNT];
};

struct Pokenav_RegionMapGfx
{
    bool32 (*isTaskActiveCB)(void);
    u32 loopTaskId;
    u16 infoWindowId;
    struct Sprite *cityZoomTextSprites[3];
    u8 ALIGNED(2) tilemapBuffer[BG_SCREEN_SIZE];
    u8 cityZoomPics[NUM_CITY_MAPS][200];
};

struct CityMapEntry
{
    mapsec_u16_t mapSecId;
    u16 index;
    const u32 *tilemap;
};

static u32 HandleRegionMapInput(struct Pokenav_RegionMapMenu *);
static u32 HandleRegionMapInputZoomDisabled(struct Pokenav_RegionMapMenu *);
static u32 HandlePlaceChoiceInput(struct Pokenav_RegionMapMenu *);
static u32 HandleWildListInput_(struct Pokenav_RegionMapMenu *);
static u32 LoopedTask_OpenPlaceChoice(s32);
static bool32 CanFlyToCursor(struct RegionMap *);
static u32 LoopedTask_OpenRegionMap(s32);
static u32 LoopedTask_DecompressCityMaps(s32);
static bool32 GetCurrentLoopedTaskActive(void);
static void FreeCityZoomViewGfx(void);
static void LoadCityZoomViewGfx(void);
static void DecompressCityMaps(void);
static bool32 IsDecompressCityMapsActive(void);
static void LoadPokenavRegionMapGfx(struct Pokenav_RegionMapGfx *);
static bool32 TryFreeTempTileDataBuffers(void);
static void UpdateMapSecInfoWindow(struct Pokenav_RegionMapGfx *);
static bool32 IsDma3ManagerBusyWithBgCopy_(struct Pokenav_RegionMapGfx *);
static void ChangeBgYForZoom(bool32);
static bool32 IsChangeBgYForZoomActive(void);
static void CreateCityZoomTextSprites(void);
static void DrawCityMap(struct Pokenav_RegionMapGfx *, mapsec_s32_t, int);
static void PrintLandmarkNames(struct Pokenav_RegionMapGfx *, mapsec_s32_t, int);
static void SetCityZoomTextInvisibility(bool32);
static void Task_ChangeBgYForZoom(u8 taskId);
static void UpdateCityZoomTextPosition(void);
static void SpriteCB_CityZoomText(struct Sprite *sprite);
static u32 LoopedTask_UpdateInfoAfterCursorMove(s32);
static u32 LoopedTask_RegionMapZoomOut(s32);
static u32 LoopedTask_RegionMapZoomIn(s32);

extern const u16 gRegionMapCityZoomTiles_Pal[];
extern const u32 gRegionMapCityZoomText_Gfx[];

static const u16 sMapSecInfoWindow_Pal[] = INCGFX_U16("graphics/pokenav/region_map/info_window.pal", ".gbapal");
static const u32 sRegionMapCityZoomTiles_Gfx[] = INCGFX_U32("graphics/pokenav/region_map/zoom_tiles.png", ".4bpp.smol");

#include "data/region_map/city_map_tilemaps.h"

static const struct BgTemplate sRegionMapBgTemplates[3] =
{
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 0x1F,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 2,
        .mapBaseIndex = 0x06,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 2,
        .baseTile = 0
    },
    {
        .bg = 2,
        .charBaseIndex = 0,
        .mapBaseIndex = 0x00,
        .screenSize = 2,
        .paletteMode = 0,
        .priority = 3,
        .baseTile = 0
    },
};

static const LoopedTask sRegionMapLoopTaskFuncs[] =
{
    [POKENAV_MAP_FUNC_NONE]         = NULL,
    [POKENAV_MAP_FUNC_CURSOR_MOVED] = LoopedTask_UpdateInfoAfterCursorMove,
    [POKENAV_MAP_FUNC_ZOOM_OUT]     = LoopedTask_RegionMapZoomOut,
    [POKENAV_MAP_FUNC_ZOOM_IN]      = LoopedTask_RegionMapZoomIn,
    [POKENAV_MAP_FUNC_FLY]          = NULL, // Task_Pokenav switches off, then flies
    [POKENAV_MAP_FUNC_OPEN_CHOICE]  = LoopedTask_OpenPlaceChoice,
    [POKENAV_MAP_FUNC_OPEN_WILD_LIST]  = LoopedTask_OpenWildList,
    [POKENAV_MAP_FUNC_CLOSE_WILD_LIST] = LoopedTask_CloseWildList,
};

static const u8 *const sPlaceChoiceTexts[PLACE_CHOICE_COUNT] =
{
    [PLACE_CHOICE_WILD]     = COMPOUND_STRING("Wild Pokémon"),
    [PLACE_CHOICE_FLY]      = COMPOUND_STRING("Fly"),
    [PLACE_CHOICE_FULL_MAP] = COMPOUND_STRING("Full Map"),
    [PLACE_CHOICE_CANCEL]   = COMPOUND_STRING("Cancel"),
};

static const struct CompressedSpriteSheet sCityZoomTextSpriteSheet[1] =
{
    {gRegionMapCityZoomText_Gfx, 0x800, GFXTAG_CITY_ZOOM}
};

static const struct SpritePalette sCityZoomTilesSpritePalette[] =
{
    {gRegionMapCityZoomTiles_Pal, PALTAG_CITY_ZOOM},
    {}
};

static const struct WindowTemplate sMapSecInfoWindowTemplate =
{
    .bg = 1,
    .tilemapLeft = 17,
    .tilemapTop = 4,
    .width = 12,
    .height = 13,
    .paletteNum = 1,
    .baseBlock = 0x4C
};

#include "data/region_map/city_map_entries.h"

static const struct OamData sCityZoomTextSprite_OamData =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
};

static const struct SpriteTemplate sCityZoomTextSpriteTemplate =
{
    .tileTag = GFXTAG_CITY_ZOOM,
    .paletteTag = PALTAG_CITY_ZOOM,
    .oam = &sCityZoomTextSprite_OamData,
    .callback = SpriteCB_CityZoomText,
};

u32 PokenavCallback_Init_RegionMap(void)
{
    struct Pokenav_RegionMapMenu *state = AllocSubstruct(POKENAV_SUBSTRUCT_REGION_MAP_STATE, sizeof(struct Pokenav_RegionMapMenu));
    if (!state)
        return FALSE;

    if (!AllocSubstruct(POKENAV_SUBSTRUCT_REGION_MAP, sizeof(struct RegionMap)))
        return FALSE;

    state->zoomDisabled = IsEventIslandMapSecId(gMapHeader.regionMapSectionId);
    if (!state->zoomDisabled)
        state->callback = HandleRegionMapInput;
    else
        state->callback = HandleRegionMapInputZoomDisabled;

    return TRUE;
}

void FreeRegionMapSubstruct1(void)
{
    gSaveBlock2Ptr->regionMapZoom = IsRegionMapZoomed();
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_REGION_MAP);
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_REGION_MAP_STATE);
}

u32 GetRegionMapCallback(void)
{
    struct Pokenav_RegionMapMenu *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_STATE);
    return state->callback(state);
}

// Fly is offered on the terms of the Flight Beacon (CanFlyWithFlightBeacon),
// to the places the fly map offers, and flies as the Beacon does.
bool32 PokenavCanFlyTo(u8 mapSecType)
{
    return IsFlyMapDestination(mapSecType) && !IsPokenavOpenedByScript() && CanFlyWithFlightBeacon();
}

static bool32 CanFlyToCursor(struct RegionMap *regionMap)
{
    return PokenavCanFlyTo(regionMap->mapSecType);
}

void PrepareRegionMapFly(void)
{
    SetFlyDestination(GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP));
    PrepareFlightBeaconRider();
}

// The choices A offers on the place under the zoomed cursor: its wild
// Pokémon when it has any, Fly where R would fly, then the full map.
static void SetPlaceChoices(struct Pokenav_RegionMapMenu *state, struct RegionMap *regionMap)
{
    state->choiceCount = 0;
    if (WildPlaceCellHasPokemon(GetRegionMapCell(regionMap->mapSecId)))
        state->choices[state->choiceCount++] = PLACE_CHOICE_WILD;
    if (CanFlyToCursor(regionMap))
        state->choices[state->choiceCount++] = PLACE_CHOICE_FLY;
    state->choices[state->choiceCount++] = PLACE_CHOICE_FULL_MAP;
    state->choices[state->choiceCount++] = PLACE_CHOICE_CANCEL;
}

static u32 HandleRegionMapInput(struct Pokenav_RegionMapMenu *state)
{
    struct RegionMap* regionMap = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP);

    switch (DoRegionMapInputCallback())
    {
    case MAP_INPUT_MOVE_END:
        return POKENAV_MAP_FUNC_CURSOR_MOVED;
    case MAP_INPUT_A_BUTTON:
        if (!IsRegionMapZoomed())
            return POKENAV_MAP_FUNC_ZOOM_IN;
        // Off every place (open sea), A still returns to the full map.
        if (regionMap->mapSecType == MAPSECTYPE_NONE)
            return POKENAV_MAP_FUNC_ZOOM_OUT;
        SetPlaceChoices(state, regionMap);
        state->callback = HandlePlaceChoiceInput;
        return POKENAV_MAP_FUNC_OPEN_CHOICE;
    case MAP_INPUT_B_BUTTON:
        return POKENAV_MAP_FUNC_EXIT;
    case MAP_INPUT_R_BUTTON:
        if (CanFlyToCursor(regionMap))
            return POKENAV_MAP_FUNC_FLY;
    }

    return POKENAV_MAP_FUNC_NONE;
}

static u32 HandlePlaceChoiceInput(struct Pokenav_RegionMapMenu *state)
{
    u32 cursor = Menu_GetCursorPos();

    if (JOY_NEW(A_BUTTON))
    {
        state->callback = HandleRegionMapInput;
        switch (state->choices[cursor])
        {
        case PLACE_CHOICE_WILD:
            state->callback = HandleWildListInput_;
            return POKENAV_MAP_FUNC_OPEN_WILD_LIST;
        case PLACE_CHOICE_FLY:
            return POKENAV_MAP_FUNC_FLY;
        case PLACE_CHOICE_FULL_MAP:
            UpdateMapSecInfoWindow(GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM));
            return POKENAV_MAP_FUNC_ZOOM_OUT;
        default:
            PlaySE(SE_SELECT);
            return POKENAV_MAP_FUNC_CURSOR_MOVED;
        }
    }
    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        state->callback = HandleRegionMapInput;
        return POKENAV_MAP_FUNC_CURSOR_MOVED;
    }
    if (JOY_REPEAT(DPAD_UP) && cursor > 0)
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(-1);
    }
    else if (JOY_REPEAT(DPAD_DOWN) && cursor + 1 < state->choiceCount)
    {
        PlaySE(SE_SELECT);
        Menu_MoveCursor(1);
    }
    return POKENAV_MAP_FUNC_NONE;
}

static u32 HandleWildListInput_(struct Pokenav_RegionMapMenu *state)
{
    u32 func = HandleWildListInput();

    if (func == POKENAV_MAP_FUNC_CLOSE_WILD_LIST)
        state->callback = HandleRegionMapInput;
    return func;
}

static u32 HandleRegionMapInputZoomDisabled(struct Pokenav_RegionMapMenu *state)
{
    if (JOY_NEW(B_BUTTON))
        return POKENAV_MAP_FUNC_EXIT;

    return POKENAV_MAP_FUNC_NONE;
}

bool32 GetZoomDisabled(void)
{
    struct Pokenav_RegionMapMenu *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_STATE);
    return state->zoomDisabled;
}

bool32 OpenPokenavRegionMap(void)
{
    struct Pokenav_RegionMapGfx *state = AllocSubstruct(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM, sizeof(struct Pokenav_RegionMapGfx));
    if (!state)
        return FALSE;

    state->loopTaskId = CreateLoopedTask(LoopedTask_OpenRegionMap, 1);
    state->isTaskActiveCB = GetCurrentLoopedTaskActive;
    return TRUE;
}

void CreateRegionMapLoopedTask(s32 index)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    state->loopTaskId = CreateLoopedTask(sRegionMapLoopTaskFuncs[index], 1);
    state->isTaskActiveCB = GetCurrentLoopedTaskActive;
}

bool32 IsRegionMapLoopedTaskActive(void)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    return state->isTaskActiveCB();
}

void FreeRegionMapSubstruct2(void)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    FreeRegionMapIconResources();
    FreeCityZoomViewGfx();
    RemoveWindow(state->infoWindowId);
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_REGION_MAP);
    FreePokenavSubstruct(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    SetPokenavVBlankCallback();
    SetBgMode(0);
}

static void VBlankCB_RegionMap(void)
{
    TransferPlttBuffer();
    LoadOam();
    ProcessSpriteCopyRequests();
    UpdateRegionMapVideoRegs();
}

static bool32 GetCurrentLoopedTaskActive(void)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    return IsLoopedTaskActive(state->loopTaskId);
}

static bool8 ShouldOpenRegionMapZoomed(void)
{
    if (GetZoomDisabled())
        return FALSE;

    return gSaveBlock2Ptr->regionMapZoom == TRUE;
}

static u32 LoopedTask_OpenRegionMap(s32 taskState)
{
    struct RegionMap *regionMap;
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    switch (taskState)
    {
    case 0:
        SetVBlankCallback_(NULL);
        HideBg(1);
        HideBg(2);
        HideBg(3);
        SetBgMode(1);
        InitBgTemplates(sRegionMapBgTemplates, ARRAY_COUNT(sRegionMapBgTemplates) - 1);
        regionMap = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP);
        InitRegionMapData(regionMap, &sRegionMapBgTemplates[1], ShouldOpenRegionMapZoomed());
        LoadCityZoomViewGfx();
        return LT_INC_AND_PAUSE;
    case 1:
        if (LoadRegionMapGfx())
            return LT_PAUSE;

        if (!GetZoomDisabled())
        {
            CreateRegionMapPlayerIcon(4, 9);
            CreateRegionMapCursor(5, 10);
            TrySetPlayerIconBlink();
        }
        else
        {
            // Dim the region map when zoom is disabled
            // (when the player is off the map)
            BlendRegionMap(RGB_BLACK, 6);
        }
        return LT_INC_AND_PAUSE;
    case 2:
        DecompressCityMaps();
        return LT_INC_AND_CONTINUE;
    case 3:
        if (IsDecompressCityMapsActive())
            return LT_PAUSE;

        LoadPokenavRegionMapGfx(state);
        return LT_INC_AND_CONTINUE;
    case 4:
        if (TryFreeTempTileDataBuffers())
            return LT_PAUSE;

        UpdateMapSecInfoWindow(state);
        BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
        return LT_INC_AND_PAUSE;
    case 5:
        if (IsDma3ManagerBusyWithBgCopy_(state))
            return LT_PAUSE;

        ShowBg(1);
        ShowBg(2);
        SetVBlankCallback_(VBlankCB_RegionMap);
        return LT_INC_AND_PAUSE;
    case 6:
        UpdateRegionMapHelpBarText();
        ShowMapHeader(IsRegionMapZoomed() ? POKENAV_HEADER_MAP_ZOOMED_IN : POKENAV_HEADER_MAP_ZOOMED_OUT);
        // Switching the PokeNav on: the whole screen fades in from black.
        PlaySE(SE_POKENAV_ON);
        BeginNormalPaletteFade(PALETTES_ALL, -2, 16, 0, RGB_BLACK);
        return LT_INC_AND_PAUSE;
    case 7:
        if (IsPaletteFadeActive() || IsMapHeaderMoving())
            return LT_PAUSE;
        return LT_INC_AND_CONTINUE;
    default:
        return LT_FINISH;
    }
}

static u32 LoopedTask_UpdateInfoAfterCursorMove(s32 taskState)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    switch (taskState)
    {
    case 0:
        UpdateMapSecInfoWindow(state);
        UpdateRegionMapHelpBarText();
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy_(state))
            return LT_PAUSE;
        break;
    }

    return LT_FINISH;
}

static u32 LoopedTask_RegionMapZoomOut(s32 taskState)
{
    switch (taskState)
    {
    case 0:
        PlaySE(SE_SELECT);
        ChangeBgYForZoom(FALSE);
        SetRegionMapDataForZoom();
        return LT_INC_AND_PAUSE;
    case 1:
        if (UpdateRegionMapZoom() || IsChangeBgYForZoomActive())
            return LT_PAUSE;

        UpdateRegionMapHelpBarText();
        return LT_INC_AND_PAUSE;
    case 2:
        if (WaitForHelpBar())
            return LT_PAUSE;

        UpdateMapHeader(POKENAV_HEADER_MAP_ZOOMED_OUT);
        break;
    }

    return LT_FINISH;
}

static u32 LoopedTask_RegionMapZoomIn(s32 taskState)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    switch (taskState)
    {
    case 0:
        PlaySE(SE_SELECT);
        UpdateMapSecInfoWindow(state);
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy_(state))
            return LT_PAUSE;

        ChangeBgYForZoom(TRUE);
        SetRegionMapDataForZoom();
        return LT_INC_AND_PAUSE;
    case 2:
        if (UpdateRegionMapZoom() || IsChangeBgYForZoomActive())
            return LT_PAUSE;

        UpdateRegionMapHelpBarText();
        return LT_INC_AND_PAUSE;
    case 3:
        if (WaitForHelpBar())
            return LT_PAUSE;

        UpdateMapHeader(POKENAV_HEADER_MAP_ZOOMED_IN);
        break;
    }

    return LT_FINISH;
}

#define INFO_WINDOW_WIDTH  (12 * 8)
#define INFO_WINDOW_HEIGHT (13 * 8)
#define INFO_LINE_HEIGHT   16

// The choice fills the info window under the place's name and as many of its
// landmarks as still fit above a rule.
static void DrawPlaceChoice(struct Pokenav_RegionMapGfx *state, struct Pokenav_RegionMapMenu *menu)
{
    struct RegionMap *regionMap = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP);
    u32 cursorWidth = GetMenuCursorDimensionByFont(FONT_NARROW, 0);
    u32 menuHeight = menu->choiceCount * INFO_LINE_HEIGHT;
    u32 lines, top, i;

    SetCityZoomTextInvisibility(TRUE);
    FillWindowPixelBuffer(state->infoWindowId, PIXEL_FILL(1));
    PutWindowTilemap(state->infoWindowId);
    AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, regionMap->mapSecName, 0, 1, TEXT_SKIP_DRAW, NULL);
    for (lines = 1; (lines + 1) * INFO_LINE_HEIGHT + 7 + menuHeight <= INFO_WINDOW_HEIGHT; lines++)
    {
        const u8 *landmarkName = GetLandmarkName(regionMap->mapSecId, regionMap->posWithinMapSec, lines - 1);
        if (landmarkName == NULL)
            break;
        AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, landmarkName, 0, lines * INFO_LINE_HEIGHT + 1, TEXT_SKIP_DRAW, NULL);
    }
    top = lines * INFO_LINE_HEIGHT + 3;
    FillWindowPixelRect(state->infoWindowId, PIXEL_FILL(3), 2, top, INFO_WINDOW_WIDTH - 4, 1);
    top += 3;
    for (i = 0; i < menu->choiceCount; i++)
        AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, sPlaceChoiceTexts[menu->choices[i]], cursorWidth, top + i * INFO_LINE_HEIGHT, TEXT_SKIP_DRAW, NULL);
    InitMenuNormal(state->infoWindowId, FONT_NARROW, 0, top, INFO_LINE_HEIGHT, menu->choiceCount, 0);
    CopyWindowToVram(state->infoWindowId, COPYWIN_FULL);
}

static u32 LoopedTask_OpenPlaceChoice(s32 taskState)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    switch (taskState)
    {
    case 0:
        PlaySE(SE_SELECT);
        DrawPlaceChoice(state, GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_STATE));
        PrintHelpBarText(HELPBAR_MAP_PLACE_CHOICE);
        return LT_INC_AND_PAUSE;
    case 1:
        if (IsDma3ManagerBusyWithBgCopy_(state))
            return LT_PAUSE;
        break;
    }
    return LT_FINISH;
}

// The Wild Pokemon list takes BG1 over: the info window, the city key and
// the map's cursor and player icon go away while it shows.
void HideRegionMapZoomView(void)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);

    SetCityZoomTextInvisibility(TRUE);
    SetRegionMapIconsHidden(TRUE);
    CpuFill16(0x1040, state->tilemapBuffer, BG_SCREEN_SIZE);
}

// Puts the zoomed map's BG1 back as LoadPokenavRegionMapGfx left it: the
// city tiles the list drew over, the info window and its frame.
void RestoreRegionMapZoomView(void)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);

    DecompressDataWithHeaderVram(sRegionMapCityZoomTiles_Gfx, (void *)BG_CHAR_ADDR(1));
    CpuFill16(0x1040, state->tilemapBuffer, BG_SCREEN_SIZE);
    DrawTextBorderOuter(state->infoWindowId, 0x42, 4);
    UpdateMapSecInfoWindow(state);
    CopyBgTilemapBufferToVram(1);
    SetRegionMapIconsHidden(FALSE);
}

static void LoadCityZoomViewGfx(void)
{
    int i;
    for (i = 0; i < ARRAY_COUNT(sCityZoomTextSpriteSheet); i++)
        LoadCompressedSpriteSheet(&sCityZoomTextSpriteSheet[i]);

    Pokenav_AllocAndLoadPalettes(sCityZoomTilesSpritePalette);
    CreateCityZoomTextSprites();
}

static void FreeCityZoomViewGfx(void)
{
    int i;
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    FreeSpriteTilesByTag(GFXTAG_CITY_ZOOM);
    FreeSpritePaletteByTag(PALTAG_CITY_ZOOM);
    for (i = 0; i < (int)ARRAY_COUNT(state->cityZoomTextSprites); i++)
        DestroySprite(state->cityZoomTextSprites[i]);
}

static void LoadPokenavRegionMapGfx(struct Pokenav_RegionMapGfx *state)
{
    BgDmaFill(1, PIXEL_FILL(0), 0x40, 1);
    BgDmaFill(1, PIXEL_FILL(1), 0x41, 1);
    CpuFill16(0x1040, state->tilemapBuffer, 0x800);
    SetBgTilemapBuffer(1, state->tilemapBuffer);
    state->infoWindowId = AddWindow(&sMapSecInfoWindowTemplate);
    LoadUserWindowBorderGfx_(state->infoWindowId, 0x42, BG_PLTT_ID(4));
    DrawTextBorderOuter(state->infoWindowId, 0x42, 4);
    DecompressAndCopyTileDataToVram(1, sRegionMapCityZoomTiles_Gfx, 0, 0, 0);
    FillWindowPixelBuffer(state->infoWindowId, PIXEL_FILL(1));
    PutWindowTilemap(state->infoWindowId);
    CopyWindowToVram(state->infoWindowId, COPYWIN_FULL);
    CopyPaletteIntoBufferUnfaded(sMapSecInfoWindow_Pal, BG_PLTT_ID(1), sizeof(sMapSecInfoWindow_Pal));
    CopyPaletteIntoBufferUnfaded(gRegionMapCityZoomTiles_Pal, BG_PLTT_ID(3), PLTT_SIZE_4BPP);
    if (!IsRegionMapZoomed())
        ChangeBgY(1, -0x6000, BG_COORD_SET);
    else
        ChangeBgY(1, 0, BG_COORD_SET);

    ChangeBgX(1, 0, BG_COORD_SET);
}

static bool32 TryFreeTempTileDataBuffers(void)
{
    return FreeTempTileDataBuffersIfPossible();
}

static void UpdateMapSecInfoWindow(struct Pokenav_RegionMapGfx *state)
{
    struct RegionMap *regionMap = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP);
    switch (regionMap->mapSecType)
    {
    case MAPSECTYPE_CITY_CANFLY:
        FillWindowPixelBuffer(state->infoWindowId, PIXEL_FILL(1));
        PutWindowRectTilemap(state->infoWindowId, 0, 0, 12, 2);
        AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, regionMap->mapSecName, 0, 1, TEXT_SKIP_DRAW, NULL);
        DrawCityMap(state, regionMap->mapSecId, regionMap->posWithinMapSec);
        CopyWindowToVram(state->infoWindowId, COPYWIN_FULL);
        SetCityZoomTextInvisibility(FALSE);
        break;
    case MAPSECTYPE_CITY_CANTFLY:
        FillWindowPixelBuffer(state->infoWindowId, PIXEL_FILL(1));
        PutWindowRectTilemap(state->infoWindowId, 0, 0, 12, 2);
        AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, regionMap->mapSecName, 0, 1, TEXT_SKIP_DRAW, NULL);
        FillBgTilemapBufferRect(1, 0x1041, 17, 6, 12, 11, 17);
        CopyWindowToVram(state->infoWindowId, COPYWIN_FULL);
        SetCityZoomTextInvisibility(TRUE);
        break;
    case MAPSECTYPE_ROUTE:
    case MAPSECTYPE_BATTLE_FRONTIER:
        FillWindowPixelBuffer(state->infoWindowId, PIXEL_FILL(1));
        PutWindowTilemap(state->infoWindowId);
        AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, regionMap->mapSecName, 0, 1, TEXT_SKIP_DRAW, NULL);
        PrintLandmarkNames(state, regionMap->mapSecId, regionMap->posWithinMapSec);
        CopyWindowToVram(state->infoWindowId, COPYWIN_FULL);
        SetCityZoomTextInvisibility(TRUE);
        break;
    case MAPSECTYPE_NONE:
        FillBgTilemapBufferRect(1, 0x1041, 17, 4, 12, 13, 17);
        CopyBgTilemapBufferToVram(1);
        SetCityZoomTextInvisibility(TRUE);
        break;
    }
}

static bool32 IsDma3ManagerBusyWithBgCopy_(struct Pokenav_RegionMapGfx *state)
{
    return IsDma3ManagerBusyWithBgCopy();
}

#define tZoomIn data[0]

static void ChangeBgYForZoom(bool32 zoomIn)
{
    u8 taskId = CreateTask(Task_ChangeBgYForZoom, 3);
    gTasks[taskId].tZoomIn = zoomIn;
}

static bool32 IsChangeBgYForZoomActive(void)
{
    return FuncIsActiveTask(Task_ChangeBgYForZoom);
}

static void Task_ChangeBgYForZoom(u8 taskId)
{
    if (gTasks[taskId].tZoomIn)
    {
        if (ChangeBgY(1, 0x480, BG_COORD_ADD) >= 0)
        {
            ChangeBgY(1, 0, BG_COORD_SET);
            DestroyTask(taskId);
        }

        UpdateCityZoomTextPosition();
    }
    else
    {
        if (ChangeBgY(1, 0x480, BG_COORD_SUB) <= -0x6000)
        {
            ChangeBgY(1, -0x6000, BG_COORD_SET);
            DestroyTask(taskId);
        }

        UpdateCityZoomTextPosition();
    }
}

#undef tZoomIn

static void DecompressCityMaps(void)
{
    CreateLoopedTask(LoopedTask_DecompressCityMaps, 1);
}

static bool32 IsDecompressCityMapsActive(void)
{
    return FuncIsActiveLoopedTask(LoopedTask_DecompressCityMaps);
}

static u32 LoopedTask_DecompressCityMaps(s32 taskState)
{
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    if (taskState < NUM_CITY_MAPS)
    {
        DecompressDataWithHeaderWram(sPokenavCityMaps[taskState].tilemap, state->cityZoomPics[taskState]);
        return LT_INC_AND_CONTINUE;
    }

    return LT_FINISH;
}

static void DrawCityMap(struct Pokenav_RegionMapGfx *state, mapsec_s32_t mapSecId, int pos)
{
    int i;
    for (i = 0; i < NUM_CITY_MAPS && (sPokenavCityMaps[i].mapSecId != mapSecId || sPokenavCityMaps[i].index != pos); i++)
        ;

    if (i == NUM_CITY_MAPS)
        return;

    FillBgTilemapBufferRect_Palette0(1, 0x1041, 17, 6, 12, 11);
    CopyToBgTilemapBufferRect(1, state->cityZoomPics[i], 18, 6, 10, 10);
}

static void PrintLandmarkNames(struct Pokenav_RegionMapGfx *state, mapsec_s32_t mapSecId, int pos)
{
    int i = 0;
    while (1)
    {
        const u8 *landmarkName = GetLandmarkName(mapSecId, pos, i);
        if (!landmarkName)
            break;

        StringCopyPadded(gStringVar1, landmarkName, CHAR_SPACE, 12);
        AddTextPrinterParameterized(state->infoWindowId, FONT_NARROW, gStringVar1, 0, i * 16 + 17, TEXT_SKIP_DRAW, NULL);
        i++;
    }
}

static void CreateCityZoomTextSprites(void)
{
    int i;
    int y;
    struct Sprite *sprite;
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);

    // When not zoomed in the text is still created but its pushed off screen
    if (!IsRegionMapZoomed())
        y = 228;
    else
        y = 132;

    for (i = 0; i < (int)ARRAY_COUNT(state->cityZoomTextSprites); i++)
    {
        u8 spriteId = CreateSprite(&sCityZoomTextSpriteTemplate, 152 + i * 32, y, 8);
        sprite = &gSprites[spriteId];
        sprite->data[0] = 0;
        sprite->data[1] = i * 4;
        sprite->data[2] = sprite->oam.tileNum;
        sprite->data[3] = 150;
        sprite->data[4] = i * 4;
        sprite->oam.tileNum += i * 4;
        state->cityZoomTextSprites[i] = sprite;
    }
}

// Slide and cycle through the text key showing what the features on the zoomed city map are
static void SpriteCB_CityZoomText(struct Sprite *sprite)
{
    if (sprite->data[3])
    {
        sprite->data[3]--;
        return;
    }

    if (++sprite->data[0] > 11)
        sprite->data[0] = 0;

    if (++sprite->data[1] > 60)
        sprite->data[1] = 0;

    sprite->oam.tileNum = sprite->data[2] + sprite->data[1];
    if (sprite->data[5] < 4)
    {
        if (sprite->data[0] == 0)
        {
            sprite->data[5]++;
            sprite->data[3] = 120;
        }
    }
    else
    {
        if (sprite->data[1] == sprite->data[4])
        {
            sprite->data[5] = 0;
            sprite->data[0] = 0;
            sprite->data[3] = 120;
        }
    }
}

static void UpdateCityZoomTextPosition(void)
{
    int i;
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    int y = 132 - (GetBgY(1) >> 8);
    for (i = 0; i < (int)ARRAY_COUNT(state->cityZoomTextSprites); i++)
        state->cityZoomTextSprites[i]->y = y;
}

static void SetCityZoomTextInvisibility(bool32 invisible)
{
    int i;
    struct Pokenav_RegionMapGfx *state = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP_ZOOM);
    for (i = 0; i < (int)ARRAY_COUNT(state->cityZoomTextSprites); i++)
        state->cityZoomTextSprites[i]->invisible = invisible;
}

void UpdateRegionMapHelpBarText(void)
{
    struct RegionMap* regionMap = GetSubstructPtr(POKENAV_SUBSTRUCT_REGION_MAP);

    // Off the map (on an event island) the map can't zoom: B is the only button.
    if (GetZoomDisabled())
        PrintHelpBarText(HELPBAR_MAP_ZOOM_DISABLED);
    else if (!IsRegionMapZoomed())
        PrintHelpBarText(CanFlyToCursor(regionMap) ? HELPBAR_MAP_ZOOMED_OUT_CANFLY : HELPBAR_MAP_ZOOMED_OUT);
    // Zoomed in, A on a place offers its choices; off every place it returns
    // to the full map.
    else if (regionMap->mapSecType == MAPSECTYPE_NONE)
        PrintHelpBarText(HELPBAR_MAP_ZOOMED_IN);
    else
        PrintHelpBarText(CanFlyToCursor(regionMap) ? HELPBAR_MAP_ZOOMED_IN_PLACE_CANFLY : HELPBAR_MAP_ZOOMED_IN_PLACE);
}
