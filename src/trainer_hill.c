#include "global.h"
#include "caps.h"
#include "champions_circuit.h"
#include "difficulty.h"
#include "malloc.h"
#include "battle.h"
#include "battle_tower.h"
#include "battle_setup.h"
#include "ereader_helpers.h"
#include "event_data.h"
#include "event_scripts.h"
#include "frontier_util.h"
#include "fieldmap.h"
#include "field_message_box.h"
#include "international_string_util.h"
#include "item.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokemon.h"
#include "script.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "trainer_hill.h"
#include "window.h"
#include "util.h"
#include "constants/battle_ai.h"
#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/layouts.h"
#include "constants/moves.h"
#include "constants/trainers.h"
#include "constants/trainer_hill.h"
#include "constants/trainer_types.h"

#define HILL_MAX_TIME 215999 // 60 * 60 * 60 - 1

struct FloorTrainers
{
    u8 name[HILL_TRAINERS_PER_FLOOR][TRAINER_NAME_LENGTH + 1];
    u8 facilityClass[HILL_TRAINERS_PER_FLOOR];
};

static EWRAM_DATA struct {
    u8 floorId;
    struct TrainerHillChallenge challenge;
    struct TrainerHillFloor floors[NUM_TRAINER_HILL_FLOORS];
} *sHillData = NULL;

static EWRAM_DATA struct FloorTrainers *sFloorTrainers = NULL;
EWRAM_DATA u32 *gTrainerHillVBlankCounter = NULL;

// This file's functions.
static void TrainerHillStartChallenge(void);
static void GetOwnerState(void);
static void GiveChallengePrize(void);
static void CheckFinalTime(void);
static void TrainerHillResumeTimer(void);
static void TrainerHillSetPlayerLost(void);
static void TrainerHillGetChallengeStatus(void);
static void BufferChallengeTime(void);
static void GetAllFloorsUsed(void);
static void GetInEReaderMode(void);
static void IsTrainerHillChallengeActive(void);
static void ShowTrainerHillPostBattleText(void);
static void SetAllTrainerFlags(void);
static void GetGameSaved(void);
static void SetGameSaved(void);
static void ClearGameSaved(void);
static void GetChallengeWon(void);
static void TrainerHillSetMode(void);
static void SetUpDataStruct(void);
static void FreeDataStruct(void);
static void TrainerHillDummy(void);
#if FREE_TRAINER_HILL == FALSE
static void SetTimerValue(u32 *dst, u32 val);
static u32 GetTimerValue(u32 *src);
#endif //FREE_TRAINER_HILL
// const data
#include "data/battle_frontier/trainer_hill.h"

struct
{
    u8 trainerClass;
    u8 musicId;
} static const sTrainerClassesAndMusic[] =
{
    {TRAINER_CLASS_TEAM_AQUA, TRAINER_ENCOUNTER_MUSIC_AQUA},
    {TRAINER_CLASS_AQUA_ADMIN, TRAINER_ENCOUNTER_MUSIC_AQUA},
    {TRAINER_CLASS_AQUA_LEADER, TRAINER_ENCOUNTER_MUSIC_AQUA},
    {TRAINER_CLASS_AROMA_LADY, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_BATTLE_GIRL, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_SWIMMER_F, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_POKEFAN, TRAINER_ENCOUNTER_MUSIC_TWINS},
    {TRAINER_CLASS_DRAGON_TAMER, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_COOLTRAINER, TRAINER_ENCOUNTER_MUSIC_COOL},
    {TRAINER_CLASS_GUITARIST, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_SAILOR, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_TWINS, TRAINER_ENCOUNTER_MUSIC_TWINS},
    {TRAINER_CLASS_INTERVIEWER, TRAINER_ENCOUNTER_MUSIC_INTERVIEWER},
    {TRAINER_CLASS_RUIN_MANIAC, TRAINER_ENCOUNTER_MUSIC_HIKER},
    {TRAINER_CLASS_GENTLEMAN, TRAINER_ENCOUNTER_MUSIC_RICH},
    {TRAINER_CLASS_SWIMMER_M, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_POKEMANIAC, TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS},
    {TRAINER_CLASS_BLACK_BELT, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_OLD_COUPLE, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_BUG_MANIAC, TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS},
    {TRAINER_CLASS_CAMPER, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_KINDLER, TRAINER_ENCOUNTER_MUSIC_HIKER},
    {TRAINER_CLASS_TEAM_MAGMA, TRAINER_ENCOUNTER_MUSIC_MAGMA},
    {TRAINER_CLASS_MAGMA_ADMIN, TRAINER_ENCOUNTER_MUSIC_MAGMA},
    {TRAINER_CLASS_MAGMA_LEADER, TRAINER_ENCOUNTER_MUSIC_MAGMA},
    {TRAINER_CLASS_LASS, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_BUG_CATCHER, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_NINJA_BOY, TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS},
    {TRAINER_CLASS_RICH_BOY, TRAINER_ENCOUNTER_MUSIC_RICH},
    {TRAINER_CLASS_HEX_MANIAC, TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS},
    {TRAINER_CLASS_BEAUTY, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_LADY, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_PARASOL_LADY, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_PICNICKER, TRAINER_ENCOUNTER_MUSIC_GIRL},
    {TRAINER_CLASS_PKMN_BREEDER, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_COLLECTOR, TRAINER_ENCOUNTER_MUSIC_SUSPICIOUS},
    {TRAINER_CLASS_PKMN_RANGER, TRAINER_ENCOUNTER_MUSIC_COOL},
    {TRAINER_CLASS_RIVAL, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_YOUNG_COUPLE, TRAINER_ENCOUNTER_MUSIC_GIRL},
    {TRAINER_CLASS_PSYCHIC, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_SR_AND_JR, TRAINER_ENCOUNTER_MUSIC_TWINS},
    {TRAINER_CLASS_ELITE_FOUR, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_YOUNGSTER, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_EXPERT, TRAINER_ENCOUNTER_MUSIC_INTENSE},
    {TRAINER_CLASS_TRIATHLETE, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_BIRD_KEEPER, TRAINER_ENCOUNTER_MUSIC_COOL},
    {TRAINER_CLASS_FISHERMAN, TRAINER_ENCOUNTER_MUSIC_HIKER},
    {TRAINER_CLASS_CHAMPION, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_TUBER_M, TRAINER_ENCOUNTER_MUSIC_MALE},
    {TRAINER_CLASS_TUBER_F, TRAINER_ENCOUNTER_MUSIC_GIRL},
    {TRAINER_CLASS_SIS_AND_BRO, TRAINER_ENCOUNTER_MUSIC_SWIMMER},
    {TRAINER_CLASS_HIKER, TRAINER_ENCOUNTER_MUSIC_HIKER},
    {TRAINER_CLASS_LEADER, TRAINER_ENCOUNTER_MUSIC_FEMALE},
    {TRAINER_CLASS_SCHOOL_KID, TRAINER_ENCOUNTER_MUSIC_MALE},
};

// Emerald Champions (C5/B6): the Trainer Hill prize lists are gone. Every entry in
// them was sellable treasure, and the Big Nugget made the Hill the game's only
// repeatable money faucet. The Hill itself is sealed at the Route 111 door.

static const u16 sEReader_Pal[] = INCGFX_U16("graphics/trainer_hill/ereader.pal", ".gbapal");
static const u8 sRecordWinColors[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};

static const struct TrainerHillChallenge *const sChallengeData[NUM_TRAINER_HILL_MODES] =
{
    [HILL_MODE_NORMAL]  = &sChallenge_Normal,
    [HILL_MODE_VARIETY] = &sChallenge_Variety,
    [HILL_MODE_UNIQUE]  = &sChallenge_Unique,
    [HILL_MODE_EXPERT]  = &sChallenge_Expert,
};

static const struct TrainerHillFloor *const sFloorData[NUM_TRAINER_HILL_MODES] =
{
    [HILL_MODE_NORMAL]  = &sFloors_Normal[0],
    [HILL_MODE_VARIETY] = &sFloors_Variety[0],
    [HILL_MODE_UNIQUE]  = &sFloors_Unique[0],
    [HILL_MODE_EXPERT]  = &sFloors_Expert[0],
};

// Unused.
static const u8 *const sFloorStrings[] =
{
    gText_TrainerHill1F,
    gText_TrainerHill2F,
    gText_TrainerHill3F,
    gText_TrainerHill4F,
};

static void (*const sHillFunctions[])(void) =
{
    [TRAINER_HILL_FUNC_START]                 = TrainerHillStartChallenge,
    [TRAINER_HILL_FUNC_GET_OWNER_STATE]       = GetOwnerState,
    [TRAINER_HILL_FUNC_GIVE_PRIZE]            = GiveChallengePrize,
    [TRAINER_HILL_FUNC_CHECK_FINAL_TIME]      = CheckFinalTime,
    [TRAINER_HILL_FUNC_RESUME_TIMER]          = TrainerHillResumeTimer,
    [TRAINER_HILL_FUNC_SET_LOST]              = TrainerHillSetPlayerLost,
    [TRAINER_HILL_FUNC_GET_CHALLENGE_STATUS]  = TrainerHillGetChallengeStatus,
    [TRAINER_HILL_FUNC_GET_CHALLENGE_TIME]    = BufferChallengeTime,
    [TRAINER_HILL_FUNC_GET_ALL_FLOORS_USED]   = GetAllFloorsUsed,
    [TRAINER_HILL_FUNC_GET_IN_EREADER_MODE]   = GetInEReaderMode,
    [TRAINER_HILL_FUNC_IN_CHALLENGE]          = IsTrainerHillChallengeActive,
    [TRAINER_HILL_FUNC_POST_BATTLE_TEXT]      = ShowTrainerHillPostBattleText,
    [TRAINER_HILL_FUNC_SET_ALL_TRAINER_FLAGS] = SetAllTrainerFlags,
    [TRAINER_HILL_FUNC_GET_GAME_SAVED]        = GetGameSaved,
    [TRAINER_HILL_FUNC_SET_GAME_SAVED]        = SetGameSaved,
    [TRAINER_HILL_FUNC_CLEAR_GAME_SAVED]      = ClearGameSaved,
    [TRAINER_HILL_FUNC_GET_WON]               = GetChallengeWon,
    [TRAINER_HILL_FUNC_SET_MODE]              = TrainerHillSetMode,
};

static const u8 *const sModeStrings[NUM_TRAINER_HILL_MODES] =
{
    [HILL_MODE_NORMAL]  = gText_NormalTagMatch,
    [HILL_MODE_VARIETY] = gText_VarietyTagMatch,
    [HILL_MODE_UNIQUE]  = gText_UniqueTagMatch,
    [HILL_MODE_EXPERT]  = gText_ExpertTagMatch,
};

static const struct ObjectEventTemplate sTrainerObjectEventTemplate =
{
    .graphicsId = OBJ_EVENT_GFX_RIVAL_BRENDAN_NORMAL,
    .elevation = ELEVATION_DEFAULT,
    .movementType = MOVEMENT_TYPE_LOOK_AROUND,
    .movementRangeX = 1,
    .movementRangeY = 1,
    .trainerType = TRAINER_TYPE_NORMAL,
};

static const u32 sNextFloorMapNum[NUM_TRAINER_HILL_FLOORS] =
{
    [TRAINER_HILL_1F - 1] = MAP_NUM(MAP_TRAINER_HILL_2F),
    [TRAINER_HILL_2F - 1] = MAP_NUM(MAP_TRAINER_HILL_3F),
    [TRAINER_HILL_3F - 1] = MAP_NUM(MAP_TRAINER_HILL_4F),
    [TRAINER_HILL_4F - 1] = MAP_NUM(MAP_TRAINER_HILL_ROOF)
};


void CallTrainerHillFunction(void)
{
    SetUpDataStruct();
    sHillFunctions[gSpecialVar_0x8004]();
    FreeDataStruct();
}

void ResetTrainerHillResults(void)
{
#if FREE_TRAINER_HILL == FALSE
    s32 i;
#endif //FREE_TRAINER_HILL

    gSaveBlock2Ptr->frontier.savedGame = 0;
    gSaveBlock2Ptr->frontier.unk_EF9 = 0;
#if FREE_TRAINER_HILL == FALSE
    gSaveBlock1Ptr->trainerHill.bestTime = 0;
    for (i = 0; i < NUM_TRAINER_HILL_MODES; i++)
        SetTimerValue(&gSaveBlock1Ptr->trainerHillTimes[i], HILL_MAX_TIME);
#endif //FREE_TRAINER_HILL
}

static u8 GetFloorId(void)
{
    return gMapHeader.mapLayoutId - LAYOUT_TRAINER_HILL_1F;
}

enum TrainerClassID GetTrainerHillOpponentClass(u16 trainerId)
{
    u8 id = trainerId - 1;

    return gFacilityClassToTrainerClass[sFloorTrainers->facilityClass[id]];
}

void GetTrainerHillTrainerName(u8 *dst, u16 trainerId)
{
    s32 i;
    u8 id = trainerId - 1;

    for (i = 0; i < TRAINER_NAME_LENGTH + 1; i++)
        dst[i] = sFloorTrainers->name[id][i];
}

u8 GetTrainerHillTrainerFrontSpriteId(u16 trainerId)
{
    u8 id, facilityClass;

    SetUpDataStruct();
    id = trainerId - 1;
    facilityClass = sHillData->floors[sHillData->floorId].trainers[id].facilityClass;
    FreeDataStruct();

    return gFacilityClassToPicIndex[facilityClass];
}

void InitTrainerHillBattleStruct(void)
{
    s32 i, j;

    SetUpDataStruct();
    sFloorTrainers = AllocZeroed(sizeof(*sFloorTrainers));

    for (i = 0; i < HILL_TRAINERS_PER_FLOOR; i++)
    {
        for (j = 0; j < TRAINER_NAME_LENGTH + 1; j++)
            sFloorTrainers->name[i][j] = sHillData->floors[sHillData->floorId].trainers[i].name[j];

        sFloorTrainers->facilityClass[i] = sHillData->floors[sHillData->floorId].trainers[i].facilityClass;
    }
#if FREE_TRAINER_HILL == FALSE
    SetTrainerHillVBlankCounter(&gSaveBlock1Ptr->trainerHill.timer);
#endif //FREE_TRAINER_HILL
    FreeDataStruct();
}

void FreeTrainerHillBattleStruct(void)
{
    TRY_FREE_AND_SET_NULL(sFloorTrainers);
}

static void SetUpDataStruct(void)
{
#if FREE_TRAINER_HILL == FALSE
    if (sHillData != NULL) return;

    sHillData = AllocZeroed(sizeof(*sHillData));
    sHillData->floorId = gMapHeader.mapLayoutId - LAYOUT_TRAINER_HILL_1F;

    CpuCopy32(sChallengeData[gSaveBlock1Ptr->trainerHill.mode], &sHillData->challenge, sizeof(sHillData->challenge));
    CpuCopy32(sFloorData[gSaveBlock1Ptr->trainerHill.mode], &sHillData->floors, sizeof(sHillData->floors));
#endif // FREE_TRAINER_HILL
}

static void FreeDataStruct(void)
{
    TRY_FREE_AND_SET_NULL(sHillData);
}

void CopyTrainerHillTrainerText(u8 which, u16 localId)
{
    u8 id, floorId;

    SetUpDataStruct();
    floorId = GetFloorId();
    id = localId - 1;

    switch (which)
    {
    case TRAINER_HILL_TEXT_INTRO:
        FrontierSpeechToString(sHillData->floors[floorId].trainers[id].speechBefore);
        break;
    case TRAINER_HILL_TEXT_PLAYER_LOST:
        FrontierSpeechToString(sHillData->floors[floorId].trainers[id].speechWin);
        break;
    case TRAINER_HILL_TEXT_PLAYER_WON:
        FrontierSpeechToString(sHillData->floors[floorId].trainers[id].speechLose);
        break;
    case TRAINER_HILL_TEXT_AFTER:
        FrontierSpeechToString(sHillData->floors[floorId].trainers[id].speechAfter);
        break;
    }

    FreeDataStruct();
}

static void TrainerHillStartChallenge(void)
{
    TrainerHillDummy();
#if FREE_TRAINER_HILL == FALSE
    if (!ReadTrainerHillAndValidate())
        gSaveBlock1Ptr->trainerHill.field_3D6E_0f = 1;
    else
        gSaveBlock1Ptr->trainerHill.field_3D6E_0f = 0;

    gSaveBlock1Ptr->trainerHill.unk_3D6C = 0;
    SetTrainerHillVBlankCounter(&gSaveBlock1Ptr->trainerHill.timer);
    gSaveBlock1Ptr->trainerHill.timer = 0;
    gSaveBlock1Ptr->trainerHill.spokeToOwner = 0;
    gSaveBlock1Ptr->trainerHill.checkedFinalTime = 0;
    gSaveBlock1Ptr->trainerHill.maybeECardScanDuringChallenge = 0;
    gSaveBlock1Ptr->trainerHill.hasLost = FALSE;
    gSaveBlock2Ptr->frontier.trainerFlags = 0;
    gBattleOutcome = 0;
    gSaveBlock1Ptr->trainerHill.receivedPrize = 0;
#endif //FREE_TRAINER_HILL
}

static void GetOwnerState(void)
{
#if FREE_TRAINER_HILL == FALSE
    u32 completedFloors = (1u << (sHillData->challenge.numFloors * HILL_TRAINERS_PER_FLOOR)) - 1;

    if (!gSaveBlock1Ptr->trainerHill.spokeToOwner
     && (VarGet(VAR_TRAINER_HILL_IS_ACTIVE) == 0 || gSaveBlock1Ptr->trainerHill.hasLost
         || (gSaveBlock2Ptr->frontier.trainerFlags & completedFloors) != completedFloors))
    {
        gSpecialVar_Result = 3;
        return;
    }
    ClearTrainerHillVBlankCounter();
    gSpecialVar_Result = 0;
    if (gSaveBlock1Ptr->trainerHill.spokeToOwner)
        gSpecialVar_Result++;
    if (gSaveBlock1Ptr->trainerHill.receivedPrize && gSaveBlock1Ptr->trainerHill.checkedFinalTime)
        gSpecialVar_Result++;

    gSaveBlock1Ptr->trainerHill.spokeToOwner = TRUE;
#endif //FREE_TRAINER_HILL
}

static void GiveChallengePrize(void)
{
    // No prize: the Hill awards nothing now that its money-valued prize lists are gone.
    gSpecialVar_Result = 2;
}

// If bestTime > timer, the challenge was completed faster and its a new record
// Otherwise the owner says it was a slow time and to complete it faster next time
static void CheckFinalTime(void)
{
#if FREE_TRAINER_HILL == FALSE
    if (gSaveBlock1Ptr->trainerHill.checkedFinalTime)
    {
        gSpecialVar_Result = 2;
    }
    else if (GetTimerValue(&gSaveBlock1Ptr->trainerHill.bestTime) > gSaveBlock1Ptr->trainerHill.timer)
    {
        SetTimerValue(&gSaveBlock1Ptr->trainerHill.bestTime, gSaveBlock1Ptr->trainerHill.timer);
        gSaveBlock1Ptr->trainerHillTimes[gSaveBlock1Ptr->trainerHill.mode] = gSaveBlock1Ptr->trainerHill.bestTime;
        gSpecialVar_Result = 0;
    }
    else
    {
        gSpecialVar_Result = 1;
    }

    gSaveBlock1Ptr->trainerHill.checkedFinalTime = TRUE;
#endif //FREE_TRAINER_HILL
}

static void TrainerHillResumeTimer(void)
{
#if FREE_TRAINER_HILL == FALSE
    if (VarGet(VAR_TRAINER_HILL_IS_ACTIVE) == 0 || gSaveBlock1Ptr->trainerHill.hasLost
     || gSaveBlock1Ptr->trainerHill.spokeToOwner)
        ClearTrainerHillVBlankCounter();
    else
    {
        if (gSaveBlock1Ptr->trainerHill.timer >= HILL_MAX_TIME)
            gSaveBlock1Ptr->trainerHill.timer = HILL_MAX_TIME;
        else
            SetTrainerHillVBlankCounter(&gSaveBlock1Ptr->trainerHill.timer);
    }
#endif //FREE_TRAINER_HILL
}

static void TrainerHillSetPlayerLost(void)
{
#if FREE_TRAINER_HILL == FALSE
    ClearTrainerHillVBlankCounter();
    gSaveBlock1Ptr->trainerHill.hasLost = TRUE;
#endif //FREE_TRAINER_HILL
}

static void TrainerHillGetChallengeStatus(void)
{
#if FREE_TRAINER_HILL == FALSE
    if (gSaveBlock1Ptr->trainerHill.hasLost)
    {
        // The player lost their last match.
        gSaveBlock1Ptr->trainerHill.hasLost = FALSE;
        gSpecialVar_Result = TRAINER_HILL_PLAYER_STATUS_LOST;
    }
    else if (gSaveBlock1Ptr->trainerHill.maybeECardScanDuringChallenge)
    {
        // Unreachable code. Something relating to eCards?
        gSaveBlock1Ptr->trainerHill.maybeECardScanDuringChallenge = 0;
        gSpecialVar_Result = TRAINER_HILL_PLAYER_STATUS_ECARD_SCANNED;
    }
    else
    {
        // Continue playing.
        gSpecialVar_Result = TRAINER_HILL_PLAYER_STATUS_NORMAL;
    }
#endif //FREE_TRAINER_HILL
}

static void BufferChallengeTime(void)
{
#if FREE_TRAINER_HILL == FALSE
    s32 total, minutes, secondsWhole, secondsFraction;

    total = gSaveBlock1Ptr->trainerHill.timer;
    if (total >= HILL_MAX_TIME)
        total = HILL_MAX_TIME;

    minutes = total / (60 * 60);
    total %= (60 * 60);
    secondsWhole = total / 60;
    total %= 60;
    secondsFraction = (total * 168) / 100;

    ConvertIntToDecimalStringN(gStringVar1, minutes, STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar2, secondsWhole, STR_CONV_MODE_RIGHT_ALIGN, 2);
    ConvertIntToDecimalStringN(gStringVar3, secondsFraction, STR_CONV_MODE_LEADING_ZEROS, 2);
#endif //FREE_TRAINER_HILL
}

// Returns TRUE if all 4 floors are used
// Returns FALSE otherwise, and buffers the number of floors used
// The only time fewer than all 4 floors are used is for the JP-exclusive E-Reader and Default modes
static void GetAllFloorsUsed(void)
{
    SetUpDataStruct();
    if (sHillData->challenge.numFloors != NUM_TRAINER_HILL_FLOORS)
    {
        ConvertIntToDecimalStringN(gStringVar1, sHillData->challenge.numFloors, STR_CONV_MODE_LEFT_ALIGN, 1);
        gSpecialVar_Result = FALSE;
    }
    else
    {
        gSpecialVar_Result = TRUE;
    }

    FreeDataStruct();
}

// May have been dummied. Every time this is called a conditional for var result occurs afterwards
// Relation to E-Reader is an assumption, most dummied Trainer Hill code seems to be JP E-Reader mode related
static void GetInEReaderMode(void)
{
    SetUpDataStruct();
    gSpecialVar_Result = FALSE;
    FreeDataStruct();
}

bool8 InTrainerHillChallenge(void)
{
#if FREE_TRAINER_HILL == FALSE
    if (VarGet(VAR_TRAINER_HILL_IS_ACTIVE) == 0)
        return FALSE;
    else if (gSaveBlock1Ptr->trainerHill.spokeToOwner)
        return FALSE;
    else if (GetCurrentTrainerHillMapId() != 0)
        return TRUE;
    else
        return FALSE;
#else
    return FALSE;
#endif //FREE_TRAINER_HILL
}

static void IsTrainerHillChallengeActive(void)
{
    if (!InTrainerHillChallenge())
        gSpecialVar_Result = FALSE;
    else
        gSpecialVar_Result = TRUE;
}

static void TrainerHillDummy(void)
{

}

void PrintOnTrainerHillRecordsWindow(void)
{
#if FREE_TRAINER_HILL == FALSE
    s32 i, x, y;
    u32 total, minutes, secondsWhole, secondsFraction;

    SetUpDataStruct();
    FillWindowPixelBuffer(0, PIXEL_FILL(0));
    x = GetStringCenterAlignXOffset(FONT_NORMAL, gText_TimeBoard, 0xD0);
    AddTextPrinterParameterized3(0, FONT_NORMAL, x, 2, sRecordWinColors, TEXT_SKIP_DRAW, gText_TimeBoard);

    y = 18;
    for (i = 0; i < NUM_TRAINER_HILL_MODES; i++)
    {
        AddTextPrinterParameterized3(0, FONT_NORMAL, 0, y, sRecordWinColors, TEXT_SKIP_DRAW, sModeStrings[i]);
        y += 15;
        total = GetTimerValue(&gSaveBlock1Ptr->trainerHillTimes[i]);
        minutes = total / (60 * 60);
        total %= (60 * 60);
        ConvertIntToDecimalStringN(gStringVar1, minutes, STR_CONV_MODE_RIGHT_ALIGN, 2);
        secondsWhole = total / 60;
        total %= 60;
        ConvertIntToDecimalStringN(gStringVar2, secondsWhole, STR_CONV_MODE_RIGHT_ALIGN, 2);
        secondsFraction = (total * 168) / 100;
        ConvertIntToDecimalStringN(gStringVar3, secondsFraction, STR_CONV_MODE_LEADING_ZEROS, 2);
        StringExpandPlaceholders(StringCopy(gStringVar4, gText_TimeCleared), gText_XMinYDotZSec);
        x = GetStringRightAlignXOffset(FONT_NORMAL, gStringVar4, 0xD0);
        AddTextPrinterParameterized3(0, FONT_NORMAL, x, y, sRecordWinColors, TEXT_SKIP_DRAW, gStringVar4);
        y += 17;
    }

    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
    FreeDataStruct();
#endif //FREE_TRAINER_HILL
}

// Leftover from Fire Red / Leaf Green as in these games,
// the timer had to be xored by the encryption key in Sav2.
#if FREE_TRAINER_HILL == FALSE
static u32 GetTimerValue(u32 *src)
{
    return *src;
}

static void SetTimerValue(u32 *dst, u32 val)
{
    *dst = val;
}
#endif //FREE_TRAINER_HILL

void LoadTrainerHillObjectEventTemplates(void)
{
    u8 i, floorId;
    struct ObjectEventTemplate *eventTemplates = gSaveBlock1Ptr->objectEventTemplates;

    if (!LoadTrainerHillFloorObjectEventScripts())
        return;

    SetUpDataStruct();
    for (i = 0; i < HILL_TRAINERS_PER_FLOOR; i++)
        gSaveBlock2Ptr->frontier.trainerIds[i] = 0xFFFF;
    CpuFill32(0, gSaveBlock1Ptr->objectEventTemplates, sizeof(gSaveBlock1Ptr->objectEventTemplates));

    floorId = GetFloorId();
    for (i = 0; i < HILL_TRAINERS_PER_FLOOR; i++)
    {
        u8 bits;

        eventTemplates[i] = sTrainerObjectEventTemplate;
        eventTemplates[i].localId = i + 1;
        eventTemplates[i].graphicsId = FacilityClassToGraphicsId(sHillData->floors[floorId].trainers[i].facilityClass);
        eventTemplates[i].x = sHillData->floors[floorId].map.trainerCoords[i] & 0xF;
        eventTemplates[i].y = ((sHillData->floors[floorId].map.trainerCoords[i] >> 4) & 0xF) + HILL_FLOOR_HEIGHT_MARGIN;
        bits = i << 2;
        eventTemplates[i].movementType = ((sHillData->floors[floorId].map.trainerDirections >> bits) & 0xF) + MOVEMENT_TYPE_FACE_UP;
        eventTemplates[i].trainerRange_berryTreeId = (sHillData->floors[floorId].map.trainerRanges >> bits) & 0xF;
        eventTemplates[i].script = TrainerHill_EventScript_TrainerBattle;
        gSaveBlock2Ptr->frontier.trainerIds[i] = i + 1;
    }

    FreeDataStruct();
}

bool32 LoadTrainerHillFloorObjectEventScripts(void)
{
    SetUpDataStruct();
    // Something may have been dummied here
    FreeDataStruct();
    return TRUE;
}

static u16 GetMapDataForFloor(u8 floorId, u32 x, u32 y, u32 floorWidth) // floorWidth is always 16
{
    bool8 impassable;
    u16 metatileId;
    u16 elevation;

    impassable = (sHillData->floors[floorId].map.collisionData[y] >> (15 - x) & 1);
    metatileId = sHillData->floors[floorId].map.metatileData[floorWidth * y + x] + NUM_METATILES_IN_PRIMARY;
    elevation = PACK_ELEVATION(ELEVATION_DEFAULT);

    return PACK_COLLISION(impassable) | elevation | PACK_METATILE(metatileId);
}

void GenerateTrainerHillFloorLayout(u16 *mapArg)
{
    s32 y, x;
    const u16 *src;
    u16 *dst;
    u8 mapId = GetCurrentTrainerHillMapId();

    if (mapId == TRAINER_HILL_ENTRANCE)
    {
        InitMapFromSavedGame();
        return;
    }

    SetUpDataStruct();
    if (mapId == TRAINER_HILL_ROOF)
    {
        InitMapFromSavedGame();
        FreeDataStruct();
        return;
    }

    mapId = GetFloorId();
    src = gMapHeader.mapLayout->map;
    gBackupMapLayout.map = mapArg;
    // Dimensions include border area loaded beyond map
    gBackupMapLayout.width = HILL_FLOOR_WIDTH + 15;
    gBackupMapLayout.height = HILL_FLOOR_HEIGHT + 14;
    dst = mapArg + 224;

    // First 5 rows of the map (Entrance / Exit) are always the same
    for (y = 0; y < HILL_FLOOR_HEIGHT_MARGIN; y++)
    {
        for (x = 0; x < HILL_FLOOR_WIDTH; x++)
            dst[x] = src[x];
        dst += 31;
        src += 16;
    }

    // Load the 16x16 floor-specific layout
    for (y = 0; y < HILL_FLOOR_HEIGHT_MAIN; y++)
    {
        for (x = 0; x < HILL_FLOOR_WIDTH; x++)
            dst[x] = GetMapDataForFloor(mapId, x, y, HILL_FLOOR_WIDTH);
        dst += 31;
    }

    RunOnLoadMapScript();
    FreeDataStruct();
}

bool32 InTrainerHill(void)
{
    bool32 ret;

    if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_1F
        || gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_2F
        || gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_3F
        || gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_4F)
        ret = TRUE;
    else
        ret = FALSE;

    return ret;
}

u8 GetCurrentTrainerHillMapId(void)
{
    u8 mapId;

    if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_1F)
        mapId = TRAINER_HILL_1F;
    else if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_2F)
        mapId = TRAINER_HILL_2F;
    else if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_3F)
        mapId = TRAINER_HILL_3F;
    else if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_4F)
        mapId = TRAINER_HILL_4F;
    else if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_ROOF)
        mapId = TRAINER_HILL_ROOF;
    else if (gMapHeader.mapLayoutId == LAYOUT_TRAINER_HILL_ENTRANCE)
        mapId = TRAINER_HILL_ENTRANCE;
    else
        mapId = 0;

    return mapId;
}

const struct WarpEvent* SetWarpDestinationTrainerHill4F(void)
{
    const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_TRAINER_HILL_4F), MAP_NUM(MAP_TRAINER_HILL_4F));

    return &header->events->warps[1];
}

// For warping from the roof in challenges where the 4F is not the final challenge floor
// This would only occur in the JP-exclusive Default and E-Reader challenges
const struct WarpEvent* SetWarpDestinationTrainerHillFinalFloor(u8 warpEventId)
{
    u8 numFloors;
    const struct MapHeader *header;

    if (warpEventId == 1)
        return &gMapHeader.events->warps[1];

    numFloors = GetNumFloorsInTrainerHillChallenge();
    if (numFloors == 0 || numFloors > NUM_TRAINER_HILL_FLOORS)
        numFloors = NUM_TRAINER_HILL_FLOORS;

    header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_TRAINER_HILL_4F), sNextFloorMapNum[numFloors - 1]);
    return &header->events->warps[0];
}

u16 LocalIdToHillTrainerId(u8 localId)
{
    return gSaveBlock2Ptr->frontier.trainerIds[localId - 1];
}

bool8 GetHillTrainerFlag(u8 objectEventId)
{
    u32 trainerIndexStart = GetFloorId() * HILL_TRAINERS_PER_FLOOR;
    u8 bitId = gObjectEvents[objectEventId].localId - 1 + trainerIndexStart;

    return gSaveBlock2Ptr->frontier.trainerFlags & (1u << bitId);
}

void SetHillTrainerFlag(void)
{
    u8 i;
    u8 trainerIndexStart = GetFloorId() * HILL_TRAINERS_PER_FLOOR;

    for (i = 0; i < HILL_TRAINERS_PER_FLOOR; i++)
    {
        if (gSaveBlock2Ptr->frontier.trainerIds[i] == TRAINER_BATTLE_PARAM.opponentA)
        {
            gSaveBlock2Ptr->frontier.trainerFlags |= 1u << (trainerIndexStart + i);
            break;
        }
    }

    if (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS)
    {
        for (i = 0; i < HILL_TRAINERS_PER_FLOOR; i++)
        {
            if (gSaveBlock2Ptr->frontier.trainerIds[i] == TRAINER_BATTLE_PARAM.opponentB)
            {
                gSaveBlock2Ptr->frontier.trainerFlags |= 1u << (trainerIndexStart + i);
                break;
            }
        }
    }
}

const u8 *GetTrainerHillTrainerScript(void)
{
    return TrainerHill_EventScript_TrainerBattle;
}

static void ShowTrainerHillPostBattleText(void)
{
    CopyTrainerHillTrainerText(TRAINER_HILL_TEXT_AFTER, gSpecialVar_LastTalked);
    ShowFieldMessageFromBuffer();
}

bool32 FillHillTrainersParties(void)
{
    u8 level = max(1, min(MAX_LEVEL, GetCurrentLevelCap()) - GetTrainerLevelReduction());

    if (!CreateChampionsExhibitionParty(level))
        return FALSE;
    // The generator supplies one coordinated team. Preserve both chosen leads
    // while splitting the four reserves between the two native owners.
    gParties[B_TRAINER_OPPONENT_B][0] = gParties[B_TRAINER_OPPONENT_A][1];
    gParties[B_TRAINER_OPPONENT_B][1] = gParties[B_TRAINER_OPPONENT_A][4];
    gParties[B_TRAINER_OPPONENT_B][2] = gParties[B_TRAINER_OPPONENT_A][5];
    gParties[B_TRAINER_OPPONENT_A][1] = gParties[B_TRAINER_OPPONENT_A][2];
    gParties[B_TRAINER_OPPONENT_A][2] = gParties[B_TRAINER_OPPONENT_A][3];
    for (u32 slot = MULTI_PARTY_SIZE; slot < PARTY_SIZE; slot++)
    {
        ZeroMonData(&gParties[B_TRAINER_OPPONENT_A][slot]);
        ZeroMonData(&gParties[B_TRAINER_OPPONENT_B][slot]);
    }
    CalculateEnemyPartyCount();
    return TRUE;
}

u8 GetTrainerEncounterMusicIdInTrainerHill(u16 trainerId)
{
    s32 i;
    u8 trId, facilityClass;

    SetUpDataStruct();
    trId = trainerId - 1;
    facilityClass = sHillData->floors[sHillData->floorId].trainers[trId].facilityClass;
    FreeDataStruct();

    for (i = 0; i < ARRAY_COUNT(sTrainerClassesAndMusic); i++)
    {
        if (sTrainerClassesAndMusic[i].trainerClass == gFacilityClassToTrainerClass[facilityClass])
            return sTrainerClassesAndMusic[i].musicId;
    }

    return 0;
}



u8 GetNumFloorsInTrainerHillChallenge(void)
{
    u8 floors;

    SetUpDataStruct();
    floors = sHillData->challenge.numFloors;
    FreeDataStruct();

    return floors;
}

static void SetAllTrainerFlags(void)
{
    gSaveBlock2Ptr->frontier.trainerFlags = 0xFF;
}

// Palette never loaded, OnTrainerHillEReaderChallengeFloor always FALSE
void TryLoadTrainerHillEReaderPalette(void)
{
    if (OnTrainerHillEReaderChallengeFloor() == TRUE)
        LoadPalette(sEReader_Pal, BG_PLTT_ID(7), PLTT_SIZE_4BPP);
}

static void GetGameSaved(void)
{
    gSpecialVar_Result = gSaveBlock2Ptr->frontier.savedGame;
}

static void SetGameSaved(void)
{
    gSaveBlock2Ptr->frontier.savedGame = TRUE;
}

static void ClearGameSaved(void)
{
    gSaveBlock2Ptr->frontier.savedGame = FALSE;
}

// Always FALSE
bool32 OnTrainerHillEReaderChallengeFloor(void)
{
    if (!InTrainerHillChallenge() || GetCurrentTrainerHillMapId() == TRAINER_HILL_ENTRANCE)
        return FALSE;

    GetInEReaderMode();
    if (gSpecialVar_Result == FALSE)
        return FALSE;
    else
        return TRUE;
}

static void GetChallengeWon(void)
{
#if FREE_TRAINER_HILL == FALSE
    if (gSaveBlock1Ptr->trainerHill.hasLost)
        gSpecialVar_Result = FALSE;
    else
        gSpecialVar_Result = TRUE;
#endif //FREE_TRAINER_HILL
}

static void TrainerHillSetMode(void)
{
#if FREE_TRAINER_HILL == FALSE
    gSaveBlock1Ptr->trainerHill.mode = gSpecialVar_0x8005;
    gSaveBlock1Ptr->trainerHill.bestTime = gSaveBlock1Ptr->trainerHillTimes[gSpecialVar_0x8005];
#endif //FREE_TRAINER_HILL
}



void TrainerHillHasPendingPrize(void)
{
    gSpecialVar_Result = gSaveBlock1Ptr->trainerHill.spokeToOwner
        && !gSaveBlock1Ptr->trainerHill.receivedPrize;
}

void AbortTrainerHillChallenge(void)
{
    ClearTrainerHillVBlankCounter();
    TrainerHillSetPlayerLost();
    SetAllTrainerFlags();
    gBattleOutcome = 0;
}
