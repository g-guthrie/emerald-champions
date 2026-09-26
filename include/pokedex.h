#ifndef GUARD_POKEDEX_H
#define GUARD_POKEDEX_H

#include "bg.h"
#include "window.h"

extern void (*gPokedexVBlankCB)(void);

// An entry's pages are sibling tabs: B on any of them returns to the list.
// Pages that hand off through screenSwitchState use this value for that exit.
#define DEX_SCREEN_SWITCH_TO_LIST 4

void ResetPokedex(void);
u16 GetNationalPokedexCount(u8 caseID);
u32 GetRegionalPokedexCount(u8 caseID);
u16 GetHoennPokedexCount(u8 caseID);
u16 GetKantoPokedexCount(u8 caseID);
u8 DisplayCaughtMonDexPage(enum Species species, bool32 isShiny, u32 personality);
s8 GetSetPokedexFlag(enum NationalDexOrder nationalDexNo, u8 caseID);
void DrawFootprint(u8 windowId, enum Species species);
u16 CreateMonSpriteFromNationalDexNumber(enum NationalDexOrder nationalNum, s16 x, s16 y, u16 paletteSlot);
bool16 HasAllRegionalMons(void);
bool16 HasAllHoennMons(void);
bool16 HasAllKantoMons(void);
void ResetPokedexScrollPositions(void);
bool16 HasAllMons(void);
void CB2_OpenPokedex(void);
void PrintMonMeasurements(enum Species species, u32 owned);
u8* ConvertMonHeightToString(u32 height);
u8* ConvertMonWeightToString(u32 weight);
bool32 ShouldSkipPokedexListEntry(enum NationalDexOrder dexNum);

#if EC_HEADLESS_FIXTURES
bool32 IsPokedexHeadlessOnScreen(u32 currentPage, u32 selectedScreen, bool32 searchResults);
#endif

#endif // GUARD_POKEDEX_H
