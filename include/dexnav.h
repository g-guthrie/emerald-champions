#ifndef GUARD_DEXNAV_H
#define GUARD_DEXNAV_H

#include "config/dexnav.h"

// Where a search hides its Pokémon.
enum EncounterType
{
    ENCOUNTER_TYPE_LAND,
    ENCOUNTER_TYPE_WATER,
};

// SEARCH INFO
#define SCANSTART_X             0
#define SCANSTART_Y             0

#define MON_LEVEL_NONEXISTENT   255 // If mon not in area GetEncounterLevel returns this to exit the search

// GUI tags
#define SELECTION_CURSOR_TAG    0x4005
#define CAUGHT_MARK_TAG         0x4002

// Search tags
#define OWNED_ICON_TAG          0x4003
#define LIT_STAR_TILE_TAG       0x4010
#define HELD_ITEM_TAG           0xd750

// DexNav search variable
#define DEXNAV_MASK_SPECIES         0x3FFF  // First 14 bits
#define DEXNAV_MASK_ENVIRONMENT     0xC000  // Last two bit

void EndDexNavSearch(void);
void Task_OpenDexNavFromStartMenu(u8 taskId);
void OpenRivalTutorialDexNav(void);
bool32 TryStartDexNavSearch(void);
void ResetDexNavSearch(void);
u32 GetDexNavChain(void);
void ApplyDexNavChainRewards(struct Pokemon *mon);
void IncrementDexNavChain(void);
void GiveDexNavIfNeeded(void);
bool32 OnStep_DexNavSearch(void);

extern enum Species gDexNavSpecies;

#endif // GUARD_DEXNAV_H
