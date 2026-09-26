#ifndef GUARD_POKENAV_H
#define GUARD_POKENAV_H

#include "bg.h"
#include "main.h"
#include "sprite.h"

typedef u32 (*LoopedTask)(s32 state);

// Return values of LoopedTask functions.
#define LT_INC_AND_PAUSE 0
#define LT_INC_AND_CONTINUE 1
#define LT_PAUSE 2
#define LT_CONTINUE 3
#define LT_FINISH 4
#define LT_SET_STATE(newState) (newState + 5)

enum
{
    POKENAV_SUBSTRUCT_FRAME,
    POKENAV_SUBSTRUCT_REGION_MAP_STATE,
    POKENAV_SUBSTRUCT_REGION_MAP_ZOOM,
    POKENAV_SUBSTRUCT_REGION_MAP,
    POKENAV_SUBSTRUCT_COUNT,
};

// The map's title in the PokeNav header: "Hoenn Map" with its current view.
enum
{
    POKENAV_HEADER_MAP_ZOOMED_OUT,
    POKENAV_HEADER_MAP_ZOOMED_IN,
};

enum
{
    HELPBAR_MAP_ZOOMED_OUT,
    HELPBAR_MAP_ZOOMED_IN,
    HELPBAR_MAP_ZOOMED_OUT_CANFLY,
    HELPBAR_MAP_ZOOMED_IN_CANFLY,
    HELPBAR_MAP_ZOOM_DISABLED,
    HELPBAR_COUNT
};

// Results of the map's input handler; each one but NONE and EXIT runs a looped task.
enum
{
    POKENAV_MAP_FUNC_NONE,
    POKENAV_MAP_FUNC_CURSOR_MOVED,
    POKENAV_MAP_FUNC_ZOOM_OUT,
    POKENAV_MAP_FUNC_ZOOM_IN,
    POKENAV_MAP_FUNC_FLY,
    POKENAV_MAP_FUNC_EXIT,
};

// pokenav.c
void CB2_InitPokeNav(void);
void OpenPokenavForTutorial(void);
u32 CreateLoopedTask(LoopedTask loopedTask, u32 priority);
bool32 FuncIsActiveLoopedTask(LoopedTask func);
bool32 IsLoopedTaskActive(u32 taskId);
void *GetSubstructPtr(u32 index);
void FreePokenavSubstruct(u32 index);
void *AllocSubstruct(u32 index, u32 size);
void SetPokenavVBlankCallback(void);
void SetVBlankCallback_(IntrCallback callback);

// pokenav_main_menu.c: the PokeNav frame around the map (header, help bar,
// spinning PokeNav icon, header title) and the switch-off fade.
bool32 InitPokenavFrame(void);
u32 IsPokenavFrameLoading(void);
void FreePokenavFrame(void);
void ShutdownPokenav(void);
void CopyPaletteIntoBufferUnfaded(const u16 *palette, u32 bufferOffset, u32 size);
void Pokenav_AllocAndLoadPalettes(const struct SpritePalette *palettes);
void InitBgTemplates(const struct BgTemplate *templates, int count);
bool32 IsPaletteFadeActive(void);
void FadeToBlackExceptPrimary(void);
void PrintHelpBarText(u32 textId);
bool32 WaitForHelpBar(void);
void ShowMapHeader(u32 headerId);
void UpdateMapHeader(u32 headerId);
bool32 IsMapHeaderMoving(void);

// pokenav_region_map.c
u32 PokenavCallback_Init_RegionMap(void);
u32 GetRegionMapCallback(void);
bool32 OpenPokenavRegionMap(void);
void CreateRegionMapLoopedTask(s32 index);
bool32 IsRegionMapLoopedTaskActive(void);
void FreeRegionMapSubstruct1(void);
void FreeRegionMapSubstruct2(void);
void UpdateRegionMapHelpBarText(void);

#endif // GUARD_POKENAV_H
