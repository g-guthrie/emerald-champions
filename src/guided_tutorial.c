#include "global.h"
#include "guided_tutorial.h"
#include "battle.h"
#include "caps.h"
#include "dexnav.h"
#include "emerald_champions_opening.h"
#include "event_data.h"
#include "event_scripts.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "fieldmap.h"
#include "field_screen_effect.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "pokerus.h"
#include "random.h"
#include "script.h"
#include "starter_choose.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/trainers.h"
#include "constants/event_objects.h"
#include "constants/maps.h"

enum RivalDemoPhase { DEMO_NONE, DEMO_STAGING, DEMO_DEXNAV, DEMO_SEARCH, DEMO_BATTLE, DEMO_SUMMARY, DEMO_AFTER_SUMMARY };
static EWRAM_DATA u8 sRivalTutorialPhase = DEMO_NONE;
static EWRAM_DATA u16 sRivalTutorialFrames = 0;
static EWRAM_DATA u8 sRivalSummaryPage = 0;
static EWRAM_DATA u8 sRivalSavedChain = 0;
static EWRAM_DATA u16 sRivalSavedSearchTarget = 0;
static EWRAM_DATA bool8 sReturnRivalToLab = FALSE;

bool32 IsRivalDexNavTutorialActive(void)
{
    return sRivalTutorialPhase != DEMO_NONE;
}

void PrepareRivalDexNavTutorial(void)
{
    if (IsRivalDexNavTutorialActive())
        return;
    SavePlayerParty();
    sReturnRivalToLab = VarGet(VAR_PETALBURG_GYM_STATE) == 0;
    sRivalSavedChain = gSaveBlock3Ptr->dexNavChain;
    sRivalSavedSearchTarget = VarGet(DN_VAR_SPECIES);
    gSaveBlock3Ptr->dexNavChain = 0;
    ZeroPlayerPartyMons();
    enum Species species = GetStarterPokemon(GetEmeraldChampionsRivalStarterIndex());
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], species, 5, Random32(), OTID_STRUCT_RANDOM_NO_SHINY, 31);
    GiveMonInitialMoveset(&gParties[B_TRAINER_PLAYER][0]);
    SetMonTrainerOwned(&gParties[B_TRAINER_PLAYER][0], TRUE);
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    sRivalTutorialPhase = DEMO_STAGING;
}

static void FieldCB_RivalTutorialWarp(void)
{
    // A full map load resets the old script context. Establish the authored
    // continuation before the standard fade callback resumes it.
    ScriptContext_SetupScript(EC_RivalDexNavTutorial_AtGrass);
    ScriptContext_Stop();
    FieldCB_ContinueScriptHandleMusic();
}

void WarpToRivalTutorialSpot(void)
{
    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 10, 12);
    DoWarp();
    gFieldCallback = FieldCB_RivalTutorialWarp;
    gFieldCallback2 = NULL;
    ResetInitialPlayerAvatarState();
}

void StartRivalDexNavTutorial(void)
{
    sRivalTutorialFrames = 0;
    sRivalTutorialPhase = DEMO_DEXNAV;
    gSpecialVar_Result = FALSE;
    UnlockPlayerFieldControls();
    OpenRivalTutorialDexNav();
}

u16 RivalTutorialDexNavKeys(void)
{
    if (sRivalTutorialPhase != DEMO_DEXNAV || ++sRivalTutorialFrames < 180)
        return 0;
    return A_BUTTON;
}

void RivalTutorialSearchStarted(void)
{
    sRivalTutorialPhase = DEMO_SEARCH;
    sRivalTutorialFrames = 120;
}

bool32 RivalTutorialGetSearchOrigin(s16 *x, s16 *y)
{
    u8 id;
    if (!IsRivalDexNavTutorialActive()
     || TryGetObjectEventIdByLocalIdAndMap(LOCALID_ROUTE101_RIVAL_TUTORIAL, MAP_NUM(MAP_ROUTE101), MAP_GROUP(MAP_ROUTE101), &id))
        return FALSE;
    *x = gObjectEvents[id].currentCoords.x;
    *y = gObjectEvents[id].currentCoords.y;
    return TRUE;
}

void RivalTutorialFieldStep(void)
{
    u8 id;
    if (sRivalTutorialPhase != DEMO_SEARCH || ScriptContext_IsEnabled())
        return;
    if (!FlagGet(DN_FLAG_SEARCHING)
     || TryGetObjectEventIdByLocalIdAndMap(LOCALID_ROUTE101_RIVAL_TUTORIAL, MAP_NUM(MAP_ROUTE101), MAP_GROUP(MAP_ROUTE101), &id))
    {
        ScriptContext_SetupScript(EC_RivalDexNavTutorial_Abort);
        return;
    }
    struct ObjectEvent *actor = &gObjectEvents[id];
    if (actor->heldMovementActive)
    {
        if (!ObjectEventClearHeldMovementIfFinished(actor))
            return;
        // The rival, not the observing player, is taking the slow steps.
        gPlayerAvatar.creeping = TRUE;
        if (OnStep_DexNavSearch())
            return;
        sRivalTutorialFrames = SNEAK_STEP_PAUSE_FRAMES;
    }
    if (sRivalTutorialFrames != 0)
    {
        sRivalTutorialFrames--;
        return;
    }
    s16 x = actor->currentCoords.x - MAP_OFFSET;
    s16 y = actor->currentCoords.y - MAP_OFFSET;
    if (x != RIVAL_TUTORIAL_TARGET_X)
        ObjectEventSetHeldMovement(actor, GetWalkSlowMovementAction(x < RIVAL_TUTORIAL_TARGET_X ? DIR_EAST : DIR_WEST));
    else if (y != RIVAL_TUTORIAL_TARGET_Y)
        ObjectEventSetHeldMovement(actor, GetWalkSlowMovementAction(y < RIVAL_TUTORIAL_TARGET_Y ? DIR_SOUTH : DIR_NORTH));
}

void PrepareRivalTutorialCatch(struct Pokemon *mon)
{
    // Wally-style authored demonstration. This is the rival's catch, not a
    // reward or a change to the player's ordinary search probabilities.
    u32 nature = NATURE_ADAMANT, shiny = FALSE;
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    SetMonData(mon, MON_DATA_IS_SHINY, &shiny);
    SetMonData(mon, MON_DATA_OT_NAME, RivalTutorialTrainerName());
    u32 gender = gSaveBlock2Ptr->playerGender == MALE ? FEMALE : MALE;
    SetMonData(mon, MON_DATA_OT_GENDER, &gender);
    GiveMonPokerus(mon, TRUE);
    CalculateMonStats(mon);
    gDexNavSpecies = SPECIES_NONE; // No player chain rewards for a demonstration.
}

void RivalTutorialBattleStarted(void)
{
    sRivalTutorialPhase = DEMO_BATTLE;
}

u32 RivalTutorialTrainerPic(void)
{
    return IsRivalDexNavTutorialActive() ? (gSaveBlock2Ptr->playerGender == MALE ? TRAINER_PIC_MAY : TRAINER_PIC_BRENDAN) : TRAINER_PIC_WALLY;
}

const u8 *RivalTutorialTrainerName(void)
{
    if (!IsRivalDexNavTutorialActive())
        return COMPOUND_STRING("Wally");
    return gSaveBlock2Ptr->playerGender == MALE ? COMPOUND_STRING("May") : COMPOUND_STRING("Brendan");
}

const u8 *RivalTutorialActionPrompt(void)
{
    if (!IsRivalDexNavTutorialActive())
        return COMPOUND_STRING("What will\nWally do?");
    return gSaveBlock2Ptr->playerGender == MALE ? COMPOUND_STRING("What will\nMay do?") : COMPOUND_STRING("What will\nBrendan do?");
}

static void CB2_RivalSummaryFinished(void)
{
    sRivalTutorialPhase = DEMO_AFTER_SUMMARY;
    SetMainCallback2(CB2_ReturnToFieldContinueScriptPlayMapMusic);
}

void ShowRivalPokerusTutorial(void)
{
    sRivalTutorialPhase = DEMO_SUMMARY;
    sRivalSummaryPage = 0xFF;
    sRivalTutorialFrames = 0;
    ShowPokemonSummaryScreen(SUMMARY_MODE_LOCK_MOVES, gParties[B_TRAINER_PLAYER], 0, 0, CB2_RivalSummaryFinished);
}

u16 RivalTutorialSummaryKeys(u32 page)
{
    if (sRivalTutorialPhase != DEMO_SUMMARY)
        return 0;
    if (sRivalSummaryPage != page)
    {
        sRivalSummaryPage = page;
        sRivalTutorialFrames = 0;
    }
    if (++sRivalTutorialFrames < 210)
        return 0;
    return page == PSS_PAGE_INFO ? DPAD_RIGHT : B_BUTTON;
}

void FinishRivalDexNavTutorial(void)
{
    if (!IsRivalDexNavTutorialActive())
        return;
    EndDexNavSearch();
    LoadPlayerParty();
    gSaveBlock3Ptr->dexNavChain = sRivalSavedChain;
    VarSet(DN_VAR_SPECIES, sRivalSavedSearchTarget);
    gDexNavSpecies = SPECIES_NONE;
    gPlayerAvatar.creeping = FALSE;
    if (sReturnRivalToLab && FlagGet(STORY_REACHED_DEXNAV_LESSON_DONE))
        FlagClear(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL);
    sRivalTutorialPhase = DEMO_NONE;
}
