#include "global.h"
#include "emerald_champions_studio.h"

#if EC_HEADLESS_FIXTURES
#include "battle.h"
#include "battle_setup.h"
#include "caps.h"
#include "difficulty.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "fieldmap.h"
#include "load_save.h"
#include "main.h"
#include "overworld.h"
#include "pokemon.h"
#include "save.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "item.h"
#include "constants/event_objects.h"
#include "constants/characters.h"
#include "constants/opponents.h"

// Local Studio sessions only. Addresses are resolved from the matching ELF.
// Commands execute on the game thread at an idle field boundary, never mid-script.
EWRAM_DATA volatile u32 gEcStudioCommand = 0;
EWRAM_DATA volatile u32 gEcStudioResult = 0;
EWRAM_DATA volatile u32 gEcStudioArgs[8] = {0};
EWRAM_DATA volatile u32 gEcStudioState[32] = {0};
EWRAM_DATA volatile u32 gEcStudioActors[OBJECT_EVENTS_COUNT][6] = {{0}};
EWRAM_DATA volatile u8 gEcStudioText[512] = {0};
static EWRAM_DATA u32 sTextSerial = 0;
static EWRAM_DATA u32 sTextLength = 0;
static EWRAM_DATA u32 sFacingAfterWarp = 0;

void EmeraldChampionsStudioText(const u8 *text)
{
    if (text == NULL)
        return;
    u32 i;
    for (i = 0; i < sizeof(gEcStudioText) - 1 && text[i] != EOS; i++)
        gEcStudioText[i] = text[i];
    gEcStudioText[i] = EOS;
    sTextLength = i;
    sTextSerial++;
}

void EmeraldChampionsStudioPoll(void)
{
    if (gSaveBlock1Ptr == NULL || gSaveBlock2Ptr == NULL)
        return;
    bool32 field = gMain.callback2 == CB2_Overworld && !gMain.inBattle;
    bool32 ready = field && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled() && IsPlayerStandingStill();
    s16 x = 0, y = 0;
    if (field)
    {
        PlayerGetDestCoords(&x, &y);
        x -= MAP_OFFSET;
        y -= MAP_OFFSET;
    }
    gEcStudioState[0] = ready;
    gEcStudioState[1] = gMain.inBattle;
    gEcStudioState[2] = (u8)gSaveBlock1Ptr->location.mapGroup;
    gEcStudioState[3] = (u8)gSaveBlock1Ptr->location.mapNum;
    gEcStudioState[4] = x;
    gEcStudioState[5] = y;
    gEcStudioState[6] = field ? GetPlayerFacingDirection() : 0;
    gEcStudioState[7] = gSpecialVar_LastTalked;
    gEcStudioState[8] = GetCurrentLevelCap();
    gEcStudioState[9] = GetCurrentDifficultyLevel();
    gEcStudioState[10] = CalculatePlayerPartyCount();
    gEcStudioState[11] = ScriptContext_IsEnabled();
    gEcStudioState[30] = sTextSerial;
    gEcStudioState[31] = sTextLength;
    if (field)
    {
        for (u32 i = 0; i < OBJECT_EVENTS_COUNT; i++)
        {
            const struct ObjectEvent *object = &gObjectEvents[i];
            gEcStudioActors[i][0] = object->active ? object->localId + 1 : 0;
            gEcStudioActors[i][1] = object->graphicsId;
            gEcStudioActors[i][2] = object->currentCoords.x - MAP_OFFSET;
            gEcStudioActors[i][3] = object->currentCoords.y - MAP_OFFSET;
            gEcStudioActors[i][4] = object->facingDirection;
            gEcStudioActors[i][5] = object->invisible | (object->offScreen << 1) | (object->heldMovementActive << 2);
        }
    }
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        gEcStudioState[12 + i * 3] = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES);
        gEcStudioState[13 + i * 3] = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL);
        gEcStudioState[14 + i * 3] = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP);
    }
    if (ready && sFacingAfterWarp)
    {
        SetObjectEventDirection(&gObjectEvents[gPlayerAvatar.objectEventId], sFacingAfterWarp);
        sFacingAfterWarp = 0;
    }
    if (!gEcStudioCommand)
        return;
    gEcStudioResult = 0;
    if (!ready)
        gEcStudioResult = 2;
    else switch (gEcStudioCommand)
    {
    case 1: // Export a native battery save without opening the save menu.
        SaveMapView();
        gEcStudioResult = TrySavingData(SAVE_NORMAL) == SAVE_STATUS_OK ? 1 : 3;
        break;
    case 2: // Host validates map identity and destination against project data.
        SetWarpDestination(gEcStudioArgs[0], gEcStudioArgs[1], WARP_ID_NONE, gEcStudioArgs[2], gEcStudioArgs[3]);
        sFacingAfterWarp = gEcStudioArgs[4];
        WarpIntoMap();
        gFieldCallback = FieldCB_WarpExitFadeFromBlack;
        gFieldCallback2 = NULL;
        SetMainCallback2(CB2_LoadMap);
        gEcStudioResult = 1;
        break;
    case 3:
        HealPlayerParty();
        gEcStudioResult = 1;
        break;
    case 4:
        SetCurrentDifficultyLevel(gEcStudioArgs[0]);
        gEcStudioResult = 1;
        break;
    case 5: // Reuse the native debug battle lifecycle, without story receipts.
        if (gEcStudioArgs[0] == TRAINER_NONE || gEcStudioArgs[0] >= TRAINERS_COUNT
         || GetTrainerStructFromId(gEcStudioArgs[0])->partySize == 0 || CalculatePlayerPartyCount() < 2)
        {
            gEcStudioResult = 4;
            break;
        }
        memset(&gTrainerBattleParameter, 0, sizeof(gTrainerBattleParameter));
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        TRAINER_BATTLE_PARAM.opponentA = gEcStudioArgs[0];
        TRAINER_BATTLE_PARAM.opponentB = 0xFFFF;
        TRAINER_BATTLE_PARAM.isDoubleBattle = TRUE;
        CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_A], GetTrainerStructFromId(gEcStudioArgs[0]));
        gBattleEnvironment = BattleSetup_GetEnvironmentId();
        CalculateEnemyPartyCount();
        BattleSetup_StartTrainerBattle_Debug();
        gEcStudioResult = 1;
        break;
    case 6: // Explicit scenario prerequisites, available only in fixture ROMs.
        if (gEcStudioArgs[1])
            FlagSet(gEcStudioArgs[0]);
        else
            FlagClear(gEcStudioArgs[0]);
        gEcStudioResult = 1;
        break;
    case 7:
        VarSet(gEcStudioArgs[0], gEcStudioArgs[1]);
        gEcStudioResult = 1;
        break;
    case 8:
        gEcStudioResult = AddBagItem(gEcStudioArgs[0], gEcStudioArgs[1]) ? 1 : 4;
        break;
    default:
        gEcStudioResult = 4;
    }
    gEcStudioCommand = 0;
}
#endif
