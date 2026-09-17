#include "global.h"
#include "task.h"
#include "event_object_movement.h"
#include "item_use.h"
#include "event_scripts.h"
#include "event_data.h"
#include "script.h"
#include "event_object_lock.h"
#include "field_specials.h"
#include "item.h"
#include "item_menu.h"
#include "field_effect.h"
#include "script_movement.h"
#include "battle.h"
#include "battle_setup.h"
#include "random.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "vs_seeker.h"
#include "menu.h"
#include "string_util.h"
#include "tv.h"
#include "malloc.h"
#include "field_screen_effect.h"
#include "gym_leader_rematch.h"
#include "sound.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/songs.h"
#include "constants/script_commands.h"
#include "constants/trainer_types.h"
#include "constants/field_effects.h"

// Charging configuration is defined in include/config/item.h.

enum
{
   VSSEEKER_NOT_CHARGED,
   VSSEEKER_NO_ONE_IN_RANGE,
   VSSEEKER_CAN_USE,
};

typedef enum
{
    VSSEEKER_SINGLE_RESP_RAND,
    VSSEEKER_SINGLE_RESP_NO,
    VSSEEKER_SINGLE_RESP_YES
} VsSeekerSingleRespCode;

typedef enum
{
    VSSEEKER_RESPONSE_NO_RESPONSE,
    VSSEEKER_RESPONSE_UNFOUGHT_TRAINERS,
    VSSEEKER_RESPONSE_FOUND_REMATCHES
} VsSeekerResponseCode;

struct VsSeekerTrainerInfo
{
    const u8 *script;
    u16 trainerIdx;
    u8 localId;
    u8 objectEventId;
    s16 xCoord;
    s16 yCoord;
    u8 graphicsId;
};

struct VsSeekerStruct
{
    struct VsSeekerTrainerInfo trainerInfo[OBJECT_EVENTS_COUNT];
    u8 trainerIdxArray[OBJECT_EVENTS_COUNT];
    u8 runningBehaviourEtcArray[OBJECT_EVENTS_COUNT];
    u8 numRematchableTrainers;
};

#define END_TRAINER_INFO 0xFF
// static declarations
static EWRAM_DATA struct VsSeekerStruct *sVsSeeker = NULL;

static void VsSeekerResetInBagStepCounter(void);
static void VsSeekerResetChargingStepCounter(void);
static void Task_ResetObjectsRematchWantedState(u8 taskId);
static void ResetMovementOfRematchableTrainers(void);
static void Task_VsSeekerFrameCountdown(u8 taskId);
static void Task_VsSeeker_PlaySoundAndGetResponseCode(u8 taskId);
static void GatherNearbyTrainerInfo(void);
static void Task_VsSeeker_ShowResponseToPlayer(u8 taskId);
static bool8 CanUseVsSeeker(void);
static u8 GetVsSeekerResponseInArea(void);
static u16 GetTrainerFlagFromScript(const u8 * script);
static void ClearAllTrainerRematchStates(void);
static void StartAllRespondantIdleMovements(void);
static bool8 ObjectEventIdIsSane(u8 objectEventId);
static u8 GetRandomFaceDirectionMovementType();

static const u8 sMovementScript_Wait48[] = {
    MOVEMENT_ACTION_DELAY_16,
    MOVEMENT_ACTION_DELAY_16,
    MOVEMENT_ACTION_DELAY_16,
    MOVEMENT_ACTION_STEP_END
};

static const u8 sMovementScript_TrainerUnfought[] = {
    MOVEMENT_ACTION_EMOTE_EXCLAMATION_MARK,
    MOVEMENT_ACTION_STEP_END
};

static const u8 sMovementScript_TrainerUnknown[] = {
    MOVEMENT_ACTION_EMOTE_QUESTION_MARK,
    MOVEMENT_ACTION_STEP_END
};

static const u8 sMovementScript_TrainerNoRematch[] = {
    MOVEMENT_ACTION_EMOTE_X,
    MOVEMENT_ACTION_STEP_END
};

static const u8 sMovementScript_TrainerRematch[] = {
    MOVEMENT_ACTION_WALK_IN_PLACE_FASTER_DOWN,
    MOVEMENT_ACTION_EMOTE_DOUBLE_EXCL_MARK,
    MOVEMENT_ACTION_STEP_END
};

static const u8 sFaceDirectionMovementTypeByFacingDirection[] = {
    MOVEMENT_TYPE_FACE_DOWN,
    MOVEMENT_TYPE_FACE_DOWN,
    MOVEMENT_TYPE_FACE_UP,
    MOVEMENT_TYPE_FACE_LEFT,
    MOVEMENT_TYPE_FACE_RIGHT
};

void VsSeekerFreezeObjectsAfterChargeComplete(void)
{
    CreateTask(Task_ResetObjectsRematchWantedState, 80);
}

#define tIsPlayerFrozen data[0]
#define tAreObjectsFrozen data[1]

static void Task_ResetObjectsRematchWantedState(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    u32 i;

    if ((!task->tIsPlayerFrozen) && IsPlayerStandingStill())
    {
        PlayerFreeze();
        task->tIsPlayerFrozen = TRUE;
    }

    if (!task->tAreObjectsFrozen)
    {
        for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            if (!ObjectEventIdIsSane(i))
                continue;

            if (gObjectEvents[i].singleMovementActive)
                return;

            FreezeObjectEvent(&gObjectEvents[i]);
        }
    }

    task->tAreObjectsFrozen = TRUE;
    if (task->tIsPlayerFrozen)
    {
        DestroyTask(taskId);
        StopPlayerAvatar();
        ScriptContext_Enable();
    }
}
#undef tIsPlayerFrozen
#undef tAreObjectsFrozen

u16 VsSeekerConvertLocalIdToTableId(u16 localId)
{
    u32 localIdIndex = 0;
    u32 trainerId = 0;

    for (localIdIndex = 0; localIdIndex < OBJECT_EVENTS_COUNT ; localIdIndex++)
    {
        if (sVsSeeker->trainerInfo[localIdIndex].localId == localId)
        {
            trainerId = sVsSeeker->trainerInfo[localIdIndex].trainerIdx;
            return TrainerIdToRematchTableId(gRematchTable,trainerId);
        }
    }
    return -1;
}

void VsSeekerResetObjectMovementAfterChargeComplete(void)
{
    struct ObjectEventTemplate * templates = gSaveBlock1Ptr->objectEventTemplates;
    u32 i;
    u32 movementType;
    u8 objEventId;
    struct ObjectEvent * objectEvent;

    for (i = 0; i < gMapHeader.events->objectEventCount; i++)
    {
        if (templates[i].trainerType != TRAINER_TYPE_NORMAL
        && templates[i].trainerType != TRAINER_TYPE_BURIED)
            continue;

        if (templates[i].movementType != MOVEMENT_TYPE_ROTATE_CLOCKWISE)
            continue;

        movementType = GetRandomFaceDirectionMovementType();
        TryGetObjectEventIdByLocalIdAndMap(templates[i].localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, &objEventId);
        objectEvent = &gObjectEvents[objEventId];

        if (!ObjectEventIdIsSane(objEventId))
            continue;

        SetTrainerMovementType(objectEvent, movementType);
        templates[i].movementType = movementType;
    }
}

bool8 UpdateVsSeekerStepCounter(void)
{

    return FALSE;
}

void MapResetTrainerRematches(u16 mapGroup, u16 mapNum)
{
    if (!I_VS_SEEKER_CHARGING) return;

    FlagClear(I_VS_SEEKER_CHARGING);
    VsSeekerResetChargingStepCounter();
    ClearAllTrainerRematchStates();
    ResetMovementOfRematchableTrainers();
}

static void ResetMovementOfRematchableTrainers(void)
{
    u32 i;
    u8 movementType = 0;

    for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
    {
        struct ObjectEvent * objectEvent = &gObjectEvents[i];
        if (objectEvent->movementType != MOVEMENT_TYPE_ROTATE_CLOCKWISE)
            continue;

        movementType = GetRandomFaceDirectionMovementType();

        if (!objectEvent->active || gSprites[objectEvent->spriteId].data[0] != i)
            continue;

        gSprites[objectEvent->spriteId].x2 = 0;
        gSprites[objectEvent->spriteId].y2 = 0;
        SetTrainerMovementType(objectEvent, movementType);
    }
}

static void VsSeekerResetInBagStepCounter(void)
{
}

static void VsSeekerResetChargingStepCounter(void)
{
}

#define tCountdown      data[0]
#define tBeepDelay      data[1]
#define tNumBeeps       data[2]
#define tResponseCode   data[3]

void Task_InitVsSeekerAndCheckForTrainersOnScreen(u8 taskId)
{
    u32 i;
    u32 respval;

    if (!I_VS_SEEKER_CHARGING) return;

    for (i = 0; i < 16; i++)
        gTasks[taskId].data[i] = 0;

    sVsSeeker = AllocZeroed(sizeof(struct VsSeekerStruct));
    GatherNearbyTrainerInfo();
    respval = CanUseVsSeeker();
    if (respval == VSSEEKER_NOT_CHARGED)
    {
        Free(sVsSeeker);
        DisplayItemMessageOnField(taskId, VSSeeker_Text_BatteryNotChargedNeedXSteps, Task_ItemUse_CloseMessageBoxAndReturnToField_VsSeeker);
    }
    else if (respval == VSSEEKER_NO_ONE_IN_RANGE)
    {
        Free(sVsSeeker);
        DisplayItemMessageOnField(taskId, VSSeeker_Text_NoTrainersWithinRange, Task_ItemUse_CloseMessageBoxAndReturnToField_VsSeeker);
    }
    else if (respval == VSSEEKER_CAN_USE)
    {
        FieldEffectStart(FLDEFF_USE_VS_SEEKER);
        gTasks[taskId].func = Task_VsSeekerFrameCountdown;
        gTasks[taskId].tCountdown = 15;
    }
}



static void Task_VsSeekerFrameCountdown(u8 taskId)
{
    if (--gTasks[taskId].tCountdown == 0)
    {
        gTasks[taskId].func = Task_VsSeeker_PlaySoundAndGetResponseCode;
        gTasks[taskId].tBeepDelay = 16;
    }
}

static void Task_VsSeeker_PlaySoundAndGetResponseCode(u8 taskId)
{
    s16 * data = gTasks[taskId].data;

    if (tNumBeeps != 2 && --tBeepDelay == 0)
    {
        PlaySE(SE_CONTEST_MONS_TURN);
        tBeepDelay = 11;
        tNumBeeps++;
    }

    if (!FieldEffectActiveListContains(FLDEFF_USE_VS_SEEKER))
    {
        VsSeekerResetInBagStepCounter();
        tResponseCode = GetVsSeekerResponseInArea();
        ScriptMovement_StartObjectMovementScript(LOCALID_PLAYER, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, sMovementScript_Wait48);
        gTasks[taskId].func = Task_VsSeeker_ShowResponseToPlayer;
    }
}

static void Task_VsSeeker_ShowResponseToPlayer(u8 taskId)
{
    if (!ScriptMovement_IsObjectMovementFinished(LOCALID_PLAYER, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup))
        return;

    if (gTasks[taskId].tResponseCode == VSSEEKER_RESPONSE_NO_RESPONSE)
    {
        DisplayItemMessageOnField(taskId, VSSeeker_Text_TrainersNotReady, Task_ItemUse_CloseMessageBoxAndReturnToField_VsSeeker);
    }
    else
    {
        if (gTasks[taskId].tResponseCode == VSSEEKER_RESPONSE_FOUND_REMATCHES)
            StartAllRespondantIdleMovements();

        ClearDialogWindowAndFrame(0, TRUE);
        ScriptUnfreezeObjectEvents();
        UnlockPlayerFieldControls();
        DestroyTask(taskId);
    }
    Free(sVsSeeker);
}

#undef tCountdown
#undef tBeepDelay
#undef tNumBeeps
#undef tResponseCode

static void GatherNearbyTrainerInfo(void)
{
    struct ObjectEventTemplate *templates = gSaveBlock1Ptr->objectEventTemplates;
    u8 objectEventId = 0;
    u8 vsSeekerObjectIdx = 0;
    s32 objectEventIdx;

    for (objectEventIdx = 0; objectEventIdx < gMapHeader.events->objectEventCount; objectEventIdx++)
    {
        u16 trainerIdx = GetTrainerFlagFromScript(templates[objectEventIdx].script);
        if (trainerIdx == TRAINER_NONE && (!I_SHOW_NO_ID_TRAINER || templates[objectEventIdx].trainerType == TRAINER_TYPE_NONE))
            continue;
        if (trainerIdx == TRAINER_NONE && templates[objectEventIdx].trainerType != TRAINER_TYPE_NONE)
            DebugPrintf("Object event with local id %d is not TRAINER_TYPE_NONE but doesn't have a visible trainerID", templates[objectEventIdx].localId);
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].script = templates[objectEventIdx].script;
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].trainerIdx = trainerIdx;
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].localId = templates[objectEventIdx].localId;
        TryGetObjectEventIdByLocalIdAndMap(templates[objectEventIdx].localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, &objectEventId);
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].objectEventId = objectEventId;
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].xCoord = gObjectEvents[objectEventId].currentCoords.x - MAP_OFFSET;
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].yCoord = gObjectEvents[objectEventId].currentCoords.y - MAP_OFFSET;
        sVsSeeker->trainerInfo[vsSeekerObjectIdx].graphicsId = templates[objectEventIdx].graphicsId;
        vsSeekerObjectIdx++;
    }
    sVsSeeker->trainerInfo[vsSeekerObjectIdx].localId = END_TRAINER_INFO;
}

static u8 CanUseVsSeeker(void)
{
    return VSSEEKER_NO_ONE_IN_RANGE;
}

static u8 GetVsSeekerResponseInArea(void)
{

    return VSSEEKER_RESPONSE_NO_RESPONSE;
}

static bool32 ShouldChangeMovementForTrainerType(u32 trainerType)
{
    if (trainerType == TRAINER_TYPE_NORMAL)
        return TRUE;
    return (trainerType == TRAINER_TYPE_BURIED);
}

void ClearRematchMovementByTrainerId(void)
{
    s32 i;
    u8 objEventId = 0;
    struct ObjectEventTemplate *objectEventTemplates = gSaveBlock1Ptr->objectEventTemplates;
    struct ObjectEvent *objectEvent;

    int vsSeekerDataIdx = TrainerIdToRematchTableId(gRematchTable, TRAINER_BATTLE_PARAM.opponentA);

    if (!I_VS_SEEKER_CHARGING) return;

    if (vsSeekerDataIdx == -1)
        return;

    for (i = 0; i < gMapHeader.events->objectEventCount; i++)
    {
        if (!ShouldChangeMovementForTrainerType(objectEventTemplates[i].trainerType)
        || vsSeekerDataIdx != TrainerIdToRematchTableId(gRematchTable, GetTrainerFlagFromScript(objectEventTemplates[i].script)))
            continue;

        TryGetObjectEventIdByLocalIdAndMap(objectEventTemplates[i].localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, &objEventId);
        objectEvent = &gObjectEvents[objEventId];
        TryOverrideTemplateCoordsForObjectEvent(objectEvent, sFaceDirectionMovementTypeByFacingDirection[objectEvent->facingDirection]);

        if (gSelectedObjectEvent == objEventId)
            objectEvent->movementType = sFaceDirectionMovementTypeByFacingDirection[objectEvent->facingDirection];
        else
            objectEvent->movementType = MOVEMENT_TYPE_FACE_DOWN;
    }
}

static u32 GetGameProgressFlags()
{
    const u32 gameProgressFlags[] = {
        FLAG_VISITED_LAVARIDGE_TOWN,
        FLAG_VISITED_FORTREE_CITY,
        FLAG_SYS_GAME_CLEAR,
        FLAG_DEFEATED_METEOR_FALLS_STEVEN
    };
    u32 i = 0, numGameProgressFlags = 0;
    u32 maxGameProgressFlags = ARRAY_COUNT(gameProgressFlags);

    for (i = 0; i < maxGameProgressFlags; i++)
    {
        if (FlagGet(gameProgressFlags[i]))
            numGameProgressFlags++;
    }

    return numGameProgressFlags;
}

u16 GetRematchTrainerIdVSSeeker(u16 trainerId)
{
    u32 tableId = FirstBattleTrainerIdToRematchTableId(gRematchTable, trainerId);
    u32 rematchTrainerIdx = GetGameProgressFlags();

    if (!I_VS_SEEKER_CHARGING)
        return 0;
    if (tableId == -1)
        return 0;
    if (tableId >= REMATCH_ELITE_FOUR_ENTRIES)
        return 0;
    if (tableId >= REMATCH_SPECIAL_TRAINER_START)
        return GetCurrentGymLeaderRematchLevel();

    while (!HasTrainerBeenFought(gRematchTable[tableId].trainerIds[rematchTrainerIdx-1]))
    {
        if (rematchTrainerIdx== 0)
            break;

        rematchTrainerIdx--;
    }

    return gRematchTable[tableId].trainerIds[rematchTrainerIdx];
}

bool32 IsVsSeekerEnabled(void)
{
    if (I_VS_SEEKER_CHARGING == 0)
        return FALSE;

    return (CheckBagHasItem(ITEM_VS_SEEKER, 1));
}

static bool8 ObjectEventIdIsSane(u8 objectEventId)
{
    struct ObjectEvent *objectEvent = &gObjectEvents[objectEventId];

    if (objectEvent->active && gMapHeader.events->objectEventCount >= objectEvent->localId && gSprites[objectEvent->spriteId].data[0] == objectEventId)
        return TRUE;
    return FALSE;
}

static u8 GetRandomFaceDirectionMovementType()
{
    u16 randomFacingDirection = Random() % 4;

    switch (randomFacingDirection)
    {
    case 0:
        return MOVEMENT_TYPE_FACE_UP;
    case 1:
        return MOVEMENT_TYPE_FACE_DOWN;
    case 2:
        return MOVEMENT_TYPE_FACE_LEFT;
    case 3:
        return MOVEMENT_TYPE_FACE_RIGHT;
    default:
        return MOVEMENT_TYPE_FACE_DOWN;
    }
}


static u16 GetTrainerFlagFromScript(const u8 *script)
{
    // The trainer flag is located 3 bytes (command + flags + localIdA) from the script pointer, assuming the trainerbattle command is first in the script.
    // Because scripts are unaligned, and because the ARM processor requires shorts to be 16-bit aligned, this function needs to perform explicit bitwise operations to get the correct flag.
    u16 trainerFlag = TRAINER_NONE;
    struct ScriptContext *ctx = AllocZeroed(sizeof(struct ScriptContext));
    if (script[0] == SCR_OP_TRAINERBATTLE)
    {
        ctx->scriptPtr = script + 3;
        trainerFlag = ScriptPeekHalfword(ctx);
    }
    else if (Script_MatchesCallNative(script, NativeVsSeekerRematchId, TRUE))
    {
        ctx->scriptPtr = script + 5;
        trainerFlag = ScriptPeekHalfword(ctx);
    }
    Free(ctx);
    return trainerFlag;
}

static void ClearAllTrainerRematchStates(void)
{
}


void NativeVsSeekerRematchId(struct ScriptContext *ctx)
{
    u16 trainerId = ScriptReadHalfword(ctx);
    if (ctx->breakOnTrainerBattle && HasTrainerBeenFought(trainerId) && !ShouldTryRematchBattleForTrainerId(trainerId))
        StopScript(ctx);
}


static void StartAllRespondantIdleMovements(void)
{
}
