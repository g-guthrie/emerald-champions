#include "global.h"
#include "main.h"
#include "crt0.h"
#include "gpu_regs.h"
#include "m4a.h"
#include "load_save.h"
#include "save.h"
#include "new_game.h"
#include "overworld.h"
#include "malloc.h"
#include "sound.h"
#include "text.h"

// Reloads the game, continuing from the point of the last save
// Used to gracefully exit after a link connection error
void ReloadSave(void)
{
    u16 imeBackup = REG_IME;
    REG_IME = 0;
    RegisterRamReset(RESET_EWRAM);
    ReInitializeEWRAM();
    ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_FORCED_BLANK);
    REG_IME = imeBackup;
    gMain.inBattle = FALSE;
    SetSaveBlocksPointers(GetSaveBlocksPointersBaseOffset());
    SetDefaultFontsPointer();
    ResetMenuAndMonGlobals();
    Save_ResetSaveCounters();
    LoadGameSave(SAVE_NORMAL);
    if (gSaveFileStatus == SAVE_STATUS_EMPTY || gSaveFileStatus == SAVE_STATUS_CORRUPT)
        Sav2_ClearSetDefault();
    SetPokemonCryStereo(gSaveBlock2Ptr->optionsSound);
    InitHeap(gHeap, HEAP_SIZE);
    SetMainCallback2(CB2_ContinueSavedGame);
}

// Reload restores the last disk save, independently of battle Retry.
// SAVE_STATUS_ERROR means loading recovered a valid redundant slot.
bool32 CanReloadLastSave(void)
{
    return gSaveFileStatus == SAVE_STATUS_OK || gSaveFileStatus == SAVE_STATUS_ERROR;
}

void ReloadLastSave(void)
{
    if (!CanReloadLastSave())
        return;
    StopMapMusic();
    m4aMPlayAllStop();
    ReloadSave();
}
