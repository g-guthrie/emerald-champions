#include "global.h"
#include "malloc.h"
#include "task.h"
#include "main.h"
#include "overworld.h"
#include "field_weather.h"
#include "palette.h"
#include "pokenav.h"

// The PokeNav is its Hoenn map. It opens straight to the map; B on the map
// switches it off and returns to the start menu (or, when a script opened it,
// to that script).

#define LOOPED_TASK_DECODE_STATE(action) (action - 5)

#define LOOPED_TASK_ID(primary, secondary) (((secondary) << 16) |(primary))
#define LOOPED_TASK_PRIMARY_ID(taskId) (taskId & 0xFFFF)
#define LOOPED_TASK_SECONDARY_ID(taskId) (taskId >> 16)

struct PokenavResources
{
    bool32 calledFromScript;
    bool32 mapOpen;
    void *substructPtrs[POKENAV_SUBSTRUCT_COUNT];
};

static bool32 OpenMap(void);
static void InitPokenavResources(struct PokenavResources *);
static void FreePokenavResources(void);
static void VBlankCB_Pokenav(void);
static void CB2_Pokenav(void);
static void Task_RunLoopedTask_LinkMode(u8);
static void Task_RunLoopedTask(u8);
static void Task_Pokenav(u8);

EWRAM_DATA u8 gNextLoopedTaskId = 0;
EWRAM_DATA struct PokenavResources *gPokenavResources = NULL;

u32 CreateLoopedTask(LoopedTask loopedTask, u32 priority)
{
    u16 taskId;

    if (!IsOverworldLinkActive())
        taskId = CreateTask(Task_RunLoopedTask, priority);
    else
        taskId = CreateTask(Task_RunLoopedTask_LinkMode, priority);

    SetWordTaskArg(taskId, 1, (u32)loopedTask);

    gTasks[taskId].data[3] = gNextLoopedTaskId;
    return LOOPED_TASK_ID(taskId, gNextLoopedTaskId++);
}

bool32 IsLoopedTaskActive(u32 taskId)
{
    u32 primaryId = LOOPED_TASK_PRIMARY_ID(taskId);
    u32 secondaryId = LOOPED_TASK_SECONDARY_ID(taskId);

    if (gTasks[primaryId].isActive
        && (gTasks[primaryId].func == Task_RunLoopedTask || gTasks[primaryId].func == Task_RunLoopedTask_LinkMode)
        && gTasks[primaryId].data[3] == secondaryId)
        return TRUE;
    else
        return FALSE;
}

bool32 FuncIsActiveLoopedTask(LoopedTask func)
{
    int i;
    for (i = 0; i < NUM_TASKS; i++)
    {
        if (gTasks[i].isActive
            && (gTasks[i].func == Task_RunLoopedTask || gTasks[i].func == Task_RunLoopedTask_LinkMode)
            && (LoopedTask)GetWordTaskArg(i, 1) == func)
            return TRUE;
    }
    return FALSE;
}

static void Task_RunLoopedTask(u8 taskId)
{
    LoopedTask loopedTask = (LoopedTask)GetWordTaskArg(taskId, 1);
    s16 *state = &gTasks[taskId].data[0];
    bool32 exitLoop = FALSE;

    while (!exitLoop)
    {
        u32 action = loopedTask(*state);
        switch (action)
        {
        case LT_INC_AND_CONTINUE:
            (*state)++;
            break;
        case LT_INC_AND_PAUSE:
            (*state)++;
            return;
        case LT_FINISH:
            DestroyTask(taskId);
            return;
        // case LT_SET_STATE:
        default:
            *state = LOOPED_TASK_DECODE_STATE(action);
            break;
        case LT_CONTINUE:
            break;
        case LT_PAUSE:
            return;
        }
    }
}

// Every "Continue" action pauses instead.
static void Task_RunLoopedTask_LinkMode(u8 taskId)
{
    LoopedTask task;
    s16 *state;
    u32 action;

    if (Overworld_IsRecvQueueAtMax())
        return;

    task = (LoopedTask)GetWordTaskArg(taskId, 1);
    state = &gTasks[taskId].data[0];
    action = task(*state);
    switch (action)
    {
    case LT_INC_AND_PAUSE:
    case LT_INC_AND_CONTINUE:
        (*state)++;
        break;
    case LT_FINISH:
        DestroyTask(taskId);
        break;
    // case: LT_SET_STATE:
    default:
        *state = LOOPED_TASK_DECODE_STATE(action);
        break;
    case LT_PAUSE:
    case LT_CONTINUE:
        break;
    }
}

static void StartPokenav(bool32 calledFromScript)
{
    InitPokenavResources(gPokenavResources);
    gPokenavResources->calledFromScript = calledFromScript;
    ResetTasks();
    SetVBlankCallback(NULL);
    CreateTask(Task_Pokenav, 0);
    SetMainCallback2(CB2_Pokenav);
    SetVBlankCallback(VBlankCB_Pokenav);
}

void CB2_InitPokeNav(void)
{
    gPokenavResources = Alloc(sizeof(*gPokenavResources));
    if (gPokenavResources == NULL)
        SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
    else
        StartPokenav(FALSE);
}

// Opens the PokeNav from a script, for the Rustboro tutorial. Switching it off
// returns through CB2_ReturnToFieldContinueScriptPlayMapMusic, which ends the
// script's waitstate.
static void CB2_InitPokenavForTutorial(void)
{
    UpdatePaletteFade();
    if (gPaletteFade.active)
        return;

    gPokenavResources = Alloc(sizeof(*gPokenavResources));
    if (gPokenavResources == NULL)
        SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
    else
        StartPokenav(TRUE);
}

void OpenPokenavForTutorial(void)
{
    SetMainCallback2(CB2_InitPokenavForTutorial);
    FadeScreen(FADE_TO_BLACK, 0);
}

static void FreePokenavResources(void)
{
    int i;

    for (i = 0; i < POKENAV_SUBSTRUCT_COUNT; i++)
        FreePokenavSubstruct(i);

    FREE_AND_SET_NULL(gPokenavResources);
    InitKeys();
}

static void InitPokenavResources(struct PokenavResources *resources)
{
    int i;

    for (i = 0; i < POKENAV_SUBSTRUCT_COUNT; i++)
        resources->substructPtrs[i] = NULL;

    resources->calledFromScript = FALSE;
    resources->mapOpen = FALSE;
}

static void CB2_Pokenav(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB_Pokenav(void)
{
    TransferPlttBuffer();
    LoadOam();
    ProcessSpriteCopyRequests();
}

#define tState data[0]

enum {
    STATE_LOAD_FRAME,
    STATE_OPEN_MAP,
    STATE_WAIT_MAP_TASK,
    STATE_HANDLE_INPUT,
    STATE_SWITCH_OFF,
};

static void Task_Pokenav(u8 taskId)
{
    u32 funcId;
    s16 *data = gTasks[taskId].data;

    switch (tState)
    {
    case STATE_LOAD_FRAME:
        InitPokenavFrame();
        tState = STATE_OPEN_MAP;
        break;
    case STATE_OPEN_MAP:
        if (IsPokenavFrameLoading())
            break;
        if (OpenMap())
        {
            tState = STATE_WAIT_MAP_TASK;
        }
        else
        {
            ShutdownPokenav();
            tState = STATE_SWITCH_OFF;
        }
        break;
    case STATE_WAIT_MAP_TASK:
        if (IsRegionMapLoopedTaskActive())
            break;
        tState = STATE_HANDLE_INPUT;
        // fallthrough
    case STATE_HANDLE_INPUT:
        funcId = GetRegionMapCallback();
        if (funcId == POKENAV_MAP_FUNC_EXIT)
        {
            ShutdownPokenav();
            tState = STATE_SWITCH_OFF;
        }
        else if (funcId != POKENAV_MAP_FUNC_NONE)
        {
            CreateRegionMapLoopedTask(funcId);
            tState = STATE_WAIT_MAP_TASK;
        }
        break;
    case STATE_SWITCH_OFF:
        if (!IsPaletteFadeActive())
        {
            bool32 calledFromScript = gPokenavResources->calledFromScript;

            if (gPokenavResources->mapOpen)
            {
                FreeRegionMapSubstruct2();
                FreeRegionMapSubstruct1();
            }
            FreePokenavFrame();
            FreePokenavResources();
            if (calledFromScript)
                SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
            else
                SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
        }
        break;
    }
}

#undef tState

static bool32 OpenMap(void)
{
    InitKeys();
    if (!PokenavCallback_Init_RegionMap())
        return FALSE;
    if (!OpenPokenavRegionMap())
        return FALSE;

    gPokenavResources->mapOpen = TRUE;
    return TRUE;
}

void SetVBlankCallback_(IntrCallback callback)
{
    SetVBlankCallback(callback);
}

void SetPokenavVBlankCallback(void)
{
    SetVBlankCallback(VBlankCB_Pokenav);
}

void *AllocSubstruct(u32 index, u32 size)
{
    gPokenavResources->substructPtrs[index] = Alloc(size);
    return gPokenavResources->substructPtrs[index];
}

void *GetSubstructPtr(u32 index)
{
    return gPokenavResources->substructPtrs[index];
}

void FreePokenavSubstruct(u32 index)
{
    TRY_FREE_AND_SET_NULL(gPokenavResources->substructPtrs[index]);
}
