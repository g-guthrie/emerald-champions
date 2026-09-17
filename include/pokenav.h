#ifndef GUARD_POKENAV_H
#define GUARD_POKENAV_H

#include "bg.h"
#include "main.h"
#include "pokemon_storage_system.h"

typedef u32 (*LoopedTask)(s32 state);

struct PokenavMonListItem
{
    u8 boxId;
    u8 monId;
    u16 data;
};

struct PokenavListItem
{
    union {
        struct PokenavMonListItem mon;
    } item;
};

typedef void (*PokenavListBufferItemFunc)(struct PokenavListItem *, u8 *);

struct PokenavListTemplate
{
    struct PokenavListItem *list;
    u16 count;
    u16 startIndex;
    u8 itemSize;
    u8 item_X;
    u8 windowWidth;
    u8 listTop;
    u8 maxShowed;
    u8 fillValue;
    u8 fontId;
    PokenavListBufferItemFunc bufferItemFunc;
    void (*iconDrawFunc)(u16 windowId, u32 listItemId, u32 baseTile);
};

struct PokenavMonList
{
    u16 listCount;
    u16 currIndex;
    struct PokenavMonListItem monData[TOTAL_BOXES_COUNT * IN_BOX_COUNT + PARTY_SIZE];
};

// Return values of LoopedTask functions.
#define LT_INC_AND_PAUSE 0
#define LT_INC_AND_CONTINUE 1
#define LT_PAUSE 2
#define LT_CONTINUE 3
#define LT_FINISH 4
#define LT_SET_STATE(newState) (newState + 5)

enum
{
    POKENAV_MODE_NORMAL,           // Chosen from Start menu.
};

enum
{
    POKENAV_SUBSTRUCT_MAIN_MENU,
    POKENAV_SUBSTRUCT_MAIN_MENU_HANDLER,
    POKENAV_SUBSTRUCT_MENU_GFX,
    POKENAV_SUBSTRUCT_REGION_MAP_STATE,
    POKENAV_SUBSTRUCT_REGION_MAP_ZOOM,
    POKENAV_SUBSTRUCT_CONDITION_SEARCH_RESULTS,
    POKENAV_SUBSTRUCT_CONDITION_SEARCH_RESULTS_GFX,
    POKENAV_SUBSTRUCT_RIBBONS_MON_LIST,
    POKENAV_SUBSTRUCT_RIBBONS_MON_MENU,
    POKENAV_SUBSTRUCT_CONDITION_GRAPH_MENU,
    POKENAV_SUBSTRUCT_CONDITION_GRAPH_MENU_GFX,
    POKENAV_SUBSTRUCT_RIBBONS_SUMMARY_LIST,
    POKENAV_SUBSTRUCT_RIBBONS_SUMMARY_MENU,
    POKENAV_SUBSTRUCT_UNUSED,
    POKENAV_SUBSTRUCT_REGION_MAP,
    POKENAV_SUBSTRUCT_LIST,
    POKENAV_SUBSTRUCT_MON_LIST,
    POKENAV_SUBSTRUCT_COUNT,
};

enum
{
    POKENAV_GFX_MAIN_MENU,
    POKENAV_GFX_CONDITION_MENU,
    POKENAV_GFX_RIBBONS_MENU,
    POKENAV_GFX_MAP_MENU_ZOOMED_OUT,
    POKENAV_GFX_MAP_MENU_ZOOMED_IN,
    POKENAV_GFX_PARTY_MENU,
    POKENAV_GFX_SEARCH_MENU,
    POKENAV_GFX_COOL_MENU,
    POKENAV_GFX_BEAUTY_MENU,
    POKENAV_GFX_CUTE_MENU,
    POKENAV_GFX_SMART_MENU,
    POKENAV_GFX_TOUGH_MENU,
    POKENAV_GFX_MENUS_END,
};

#define POKENAV_GFX_SUBMENUS_START POKENAV_GFX_PARTY_MENU

#define POKENAV_MENU_IDS_START 100000
enum
{
    POKENAV_MAIN_MENU = POKENAV_MENU_IDS_START, // The main menu where the player selects Hoenn Map/Ribbons
    POKENAV_MAIN_MENU_CURSOR_ON_MAP,
    POKENAV_CONDITION_MENU,                     // The first Condition screen where the player selects Party or Search
    POKENAV_CONDITION_SEARCH_MENU,              // The Condition search menu where the player selects a search parameter
    POKENAV_MAIN_MENU_CURSOR_ON_RIBBONS,
    POKENAV_REGION_MAP,
    POKENAV_CONDITION_GRAPH_PARTY,              // The Condition graph screen when Party has been selected
    POKENAV_CONDITION_SEARCH_RESULTS,           // The list of results from a Condition search
    POKENAV_CONDITION_GRAPH_SEARCH,             // The Condition graph screen when a search result has been selected
    POKENAV_RETURN_CONDITION_SEARCH,            // Exited the graph screen back to the list of Condition search results
    POKENAV_RIBBONS_MON_LIST,                   // The list of Pokémon with ribbons
    POKENAV_RIBBONS_SUMMARY_SCREEN,             // The ribbon summary screen shown when a Pokémon has been selected
    POKENAV_RIBBONS_RETURN_TO_MON_LIST,         // Exited the summary screen back to the ribbon list
};

enum
{
    POKENAV_MENU_TYPE_DEFAULT,
    POKENAV_MENU_TYPE_RIBBONS,
    POKENAV_MENU_TYPE_CONDITION,
    POKENAV_MENU_TYPE_CONDITION_SEARCH,
    POKENAV_MENU_TYPE_COUNT
};

// Global IDs for menu selections
// As opposed to the cursor position, which is only relative to the number of options for the current menu
enum
{
    POKENAV_MENUITEM_MAP,
    POKENAV_MENUITEM_CONDITION,
    POKENAV_MENUITEM_RIBBONS,
    POKENAV_MENUITEM_SWITCH_OFF,
    POKENAV_MENUITEM_CONDITION_PARTY,
    POKENAV_MENUITEM_CONDITION_SEARCH,
    POKENAV_MENUITEM_CONDITION_CANCEL,
    POKENAV_MENUITEM_CONDITION_SEARCH_COOL,
    POKENAV_MENUITEM_CONDITION_SEARCH_BEAUTY,
    POKENAV_MENUITEM_CONDITION_SEARCH_CUTE,
    POKENAV_MENUITEM_CONDITION_SEARCH_SMART,
    POKENAV_MENUITEM_CONDITION_SEARCH_TOUGH,
    POKENAV_MENUITEM_CONDITION_SEARCH_CANCEL,
};

// Max menu options (condition search uses 6)
#define MAX_POKENAV_MENUITEMS 6

enum
{
    HELPBAR_NONE,
    HELPBAR_MAP_ZOOMED_OUT,
    HELPBAR_MAP_ZOOMED_IN,
    HELPBAR_MAP_ZOOMED_OUT_CANFLY,
    HELPBAR_MAP_ZOOMED_IN_CANFLY,
    HELPBAR_CONDITION_MON_LIST,
    HELPBAR_CONDITION_MON_STATUS,
    HELPBAR_CONDITION_MARKINGS,
    HELPBAR_RIBBONS_MON_LIST,
    HELPBAR_RIBBONS_LIST,
    HELPBAR_RIBBONS_CHECK,
    HELPBAR_COUNT
};

// PokéNav Function IDs
// Indices into the LoopedTask tables for each of the main PokéNav features

enum RegionMapFuncIds
{
    POKENAV_MENU_FUNC_NONE,
    POKENAV_MENU_FUNC_MOVE_CURSOR,
    POKENAV_MENU_FUNC_OPEN_CONDITION,
    POKENAV_MENU_FUNC_RETURN_TO_MAIN,
    POKENAV_MENU_FUNC_OPEN_CONDITION_SEARCH,
    POKENAV_MENU_FUNC_RETURN_TO_CONDITION,
    POKENAV_MENU_FUNC_NO_RIBBON_WINNERS,
    POKENAV_MENU_FUNC_RESHOW_DESCRIPTION,
    POKENAV_MENU_FUNC_OPEN_FEATURE,
};

enum
{
    CONDITION_FUNC_NONE,
    CONDITION_FUNC_SLIDE_MON_IN,
    CONDITION_FUNC_RETURN,
    CONDITION_FUNC_NO_TRANSITION,
    CONDITION_FUNC_SLIDE_MON_OUT,
    CONDITION_FUNC_ADD_MARKINGS,
    CONDITION_FUNC_CLOSE_MARKINGS,
};

enum
{
    CONDITION_LOAD_MON_INFO,
    CONDITION_LOAD_GRAPH,
    CONDITION_LOAD_MON_PIC,
};

#define POKENAV_MENU_FUNC_EXIT  -1

enum
{
    POKENAV_MAP_FUNC_NONE,
    POKENAV_MAP_FUNC_CURSOR_MOVED,
    POKENAV_MAP_FUNC_ZOOM_OUT,
    POKENAV_MAP_FUNC_ZOOM_IN,
    POKENAV_MAP_FUNC_EXIT,
    POKENAV_MAP_FUNC_FLY
};

// Modes for PokenavFadeScreen
enum {
    POKENAV_FADE_TO_BLACK,
    POKENAV_FADE_FROM_BLACK,
    POKENAV_FADE_TO_BLACK_ALL,
    POKENAV_FADE_FROM_BLACK_ALL,
};

// pokenav.c
void SetSelectedConditionSearch(u32 cursorPos);
u32 GetSelectedConditionSearch(void);

void CB2_InitPokeNav(void);
u32 CreateLoopedTask(LoopedTask loopedTask, u32 priority);
bool32 FuncIsActiveLoopedTask(LoopedTask func);
void *GetSubstructPtr(u32 index);
void FreePokenavSubstruct(u32 index);
void *AllocSubstruct(u32 index, u32 size);
void Pokenav_AllocAndLoadPalettes(const struct SpritePalette *palettes);
bool32 IsLoopedTaskActive(u32 taskId);
void SetPokenavMode(u16 mode);
u32 GetPokenavMode(void);
bool32 CanViewRibbonsMenu(void);
void SetPokenavVBlankCallback(void);
void SetVBlankCallback_(IntrCallback callback);

// pokenav_list.c
bool32 CreatePokenavList(const struct BgTemplate *bgTemplate, struct PokenavListTemplate *listTemplate, u32 tileOffset);
bool32 IsCreatePokenavListTaskActive(void);
void DestroyPokenavList(void);
u32 PokenavList_GetSelectedIndex(void);
int PokenavList_MoveCursorUp(void);
int PokenavList_MoveCursorDown(void);
int PokenavList_PageDown(void);
int PokenavList_PageUp(void);
bool32 PokenavList_IsMoveWindowTaskActive(void);

// pokenav_main_menu.c
bool32 InitPokenavMainMenu(void);
void CopyPaletteIntoBufferUnfaded(const u16 *palette, u32 bufferOffset, u32 size);
void RunMainMenuLoopedTask(u32 state);
u32 IsActiveMenuLoopTaskActive(void);
void LoadLeftHeaderGfxForIndex(u32 menuGfxId);
void ShowLeftHeaderGfx(u32 menuGfxId, bool32 isMain, bool32 isOnRightSide);
void PokenavFadeScreen(s32 fadeType);
bool32 AreLeftHeaderSpritesMoving(void);
void InitBgTemplates(const struct BgTemplate *templates, int count);
bool32 IsPaletteFadeActive(void);
void PrintHelpBarText(u32 textId);
bool32 WaitForHelpBar(void);
void SlideMenuHeaderDown(void);
bool32 MainMenuLoopedTaskIsBusy(void);
void SetLeftHeaderSpritesInvisibility(void);
void PokenavCopyPalette(const u16 *src, const u16 *dest, int size, int a3, int a4, u16 *palette);
void FadeToBlackExceptPrimary(void);
struct Sprite *GetSpinningPokenavSprite(void);
void HideSpinningPokenavSprite(void);
void UpdateRegionMapRightHeaderTiles(u32 menuGfxId);
void HideMainOrSubMenuLeftHeader(u32 id, bool32 onRightSide);
void SlideMenuHeaderUp(void);
void PokenavFillPalette(u32 palIndex, u16 fillValue);
u32 PokenavMainMenuLoopedTaskIsActive(void);
bool32 WaitForPokenavShutdownFade(void);
void SetActiveMenuLoopTasks(void *createLoopTask, void *isLoopTaskActive); // Fix types later.
void ShutdownPokenav(void);

// pokenav_menu_handler.c
bool32 PokenavCallback_Init_MainMenuCursorOnMap(void);
bool32 PokenavCallback_Init_MainMenuCursorOnRibbons(void);
bool32 PokenavCallback_Init_ConditionMenu(void);
bool32 PokenavCallback_Init_ConditionSearchMenu(void);
u32 GetMenuHandlerCallback(void);
void FreeMenuHandlerSubstruct1(void);
int GetPokenavMenuType(void);
int GetPokenavCursorPos(void);
int GetCurrentMenuItemId(void);
u16 GetHelpBarTextId(void);

// pokenav_menu_handler_gfx.c
bool32 OpenPokenavMenuInitial(void);
bool32 OpenPokenavMenuNotInitial(void);
void CreateMenuHandlerLoopedTask(s32 ltIdx);
bool32 IsMenuHandlerLoopedTaskActive(void);
void FreeMenuHandlerSubstruct2(void);
void ResetBldCnt_(void);

// pokenav_region_map.c
u32 PokenavCallback_Init_RegionMap(void);
u32 GetRegionMapCallback(void);
bool32 OpenPokenavRegionMap(void);
void CreateRegionMapLoopedTask(s32 index);
bool32 IsRegionMapLoopedTaskActive(void);
void FreeRegionMapSubstruct1(void);
void FreeRegionMapSubstruct2(void);
void UpdateRegionMapHelpBarText(void);

// pokenav_conditions.c
bool32 PokenavCallback_Init_ConditionGraph_Party(void);
bool32 PokenavCallback_Init_ConditionGraph_Search(void);
u32 GetConditionGraphMenuCallback(void);
void FreeConditionGraphMenuSubstruct1(void);
bool32 LoadConditionGraphMenuGfx(void);
bool32 IsConditionMenuSearchMode(void);
struct ConditionGraph *GetConditionGraphPtr(void);
u16 GetConditionGraphCurrentListIndex(void);
u16 GetMonListCount(void);
u8 GetNumConditionMonSparkles(void);
bool32 LoadNextConditionMenuMonData(u8 mode);
u8 TryGetMonMarkId(void);
u8 *GetConditionMonNameText(u8 loadId);
u8 *GetConditionMonLocationText(u8 loadId);
u16 GetConditionMonDataBuffer(void);
void *GetConditionMonPicGfx(u8 loadId);
void *GetConditionMonPal(u8 loadId);

// pokenav_conditions_gfx.c
bool32 OpenConditionGraphMenu(void);
void CreateConditionGraphMenuLoopedTask(s32 id);
u32 IsConditionGraphMenuLoopedTaskActive(void);
void FreeConditionGraphMenuSubstruct2(void);
u8 GetMonMarkingsData(void);

// pokenav_conditions_search_results.c
bool32 PokenavCallback_Init_ConditionSearch(void);
bool32 PokenavCallback_Init_ReturnToMonSearchList(void);
u32 GetConditionSearchResultsCallback(void);
void FreeSearchResultSubstruct1(void);
bool32 OpenConditionSearchResults(void);
bool32 OpenConditionSearchListFromGraph(void);
void CreateSearchResultsLoopedTask(s32 idx);
bool32 IsSearchResultLoopedTaskActive(void);
void FreeSearchResultSubstruct2(void);

// pokenav_ribbons_list.c
bool32 PokenavCallback_Init_MonRibbonList(void);
bool32 PokenavCallback_Init_RibbonsMonListFromSummary(void);
u32 GetRibbonsMonListCallback(void);
void FreeRibbonsMonList(void);
bool32 OpenRibbonsMonList(void);
bool32 OpenRibbonsMonListFromRibbonsSummary(void);
void CreateRibbonsMonListLoopedTask(s32 idx);
bool32 IsRibbonsMonListLoopedTaskActive(void);
void FreeRibbonsMonMenu(void);

// pokenav_ribbons_summary.c
bool32 PokenavCallback_Init_RibbonsSummaryMenu(void);
u32 GetRibbonsSummaryMenuCallback(void);
void FreeRibbonsSummaryScreen1(void);
bool32 OpenRibbonsSummaryMenu(void);
void CreateRibbonsSummaryLoopedTask(s32 id);
u32 IsRibbonsSummaryLoopedTaskActive(void);
void FreeRibbonsSummaryScreen2(void);

#endif // GUARD_POKENAV_H
