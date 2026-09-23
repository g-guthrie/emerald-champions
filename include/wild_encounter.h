#ifndef GUARD_WILD_ENCOUNTER_H
#define GUARD_WILD_ENCOUNTER_H

#include "rtc.h"
#include "constants/wild_encounter.h"
#include "wild_encounter_ow.h"

#define HEADER_NONE 0xFFFF

enum WildPokemonArea {
    WILD_AREA_LAND,
    WILD_AREA_WATER,
    WILD_AREA_ROCKS,
    WILD_AREA_FISHING,
    WILD_AREA_HIDDEN,
    WILD_AREA_HONEY
};

struct WildPokemon
{
    u8 minLevel;
    u8 maxLevel;
    enum Species species;
};

struct WildPokemonInfo
{
    u8 encounterRate;
    const struct WildPokemon *wildPokemon;
    const u8 *encounterBounds; // Optional per-table cumulative odds; NULL uses method defaults.
};

struct WildEncounterTypes
{
    const struct WildPokemonInfo *landMonsInfo;
    const struct WildPokemonInfo *waterMonsInfo;
    const struct WildPokemonInfo *rockSmashMonsInfo;
    const struct WildPokemonInfo *fishingMonsInfo;
    const struct WildPokemonInfo *hiddenMonsInfo;
    const struct WildPokemonInfo *honeyMonsInfo;
};

struct WildPokemonHeader
{
    u8 mapGroup;
    u8 mapNum;
    const struct WildEncounterTypes encounterTypes[TIMES_OF_DAY_COUNT];
};


extern const struct WildPokemonHeader gWildMonHeaders[];
extern const struct WildPokemonHeader gBattlePikeWildMonHeaders[];
extern const struct WildPokemonHeader gBattlePyramidWildMonHeaders[];
extern const struct WildPokemon gWildFeebas;
bool8 CheckFeebasAtCoords(s16 x, s16 y);
extern bool8 gIsFishingEncounter;
extern bool8 gIsSurfingEncounter;
extern u8 gChainFishingDexNavStreak;

u32 ChooseWildMonIndex_Fishing(const struct WildPokemonInfo *info, u8 rod);
u8 ChooseWildMonLevel(const struct WildPokemon *wildPokemon, u8 wildMonIndex, enum WildPokemonArea area);
bool8 StandardWildEncounter(u16 curMetatileBehavior, u16 prevMetatileBehavior);
bool8 SweetScentWildEncounter(void);
bool8 CanUseHoneyHere(void);
u16 HoneyWildEncounter(void);
bool8 DoesCurrentMapHaveFishingMons(void);
void FishingWildEncounter(u8 rod);
// isWaterMon may be NULL when only the local species is needed.
u16 GetLocalWildMon(bool8 *isWaterMon);
u16 GetLocalWaterMon(void);
bool8 UpdateRepelCounter(void);
bool8 IsWildLevelAllowedByRepel(u8 wildLevel);
bool8 IsAbilityAllowingEncounter(u8 level);
bool8 TryDoDoubleWildBattle(void);
u32 CalculateChainFishingShinyRolls(void);
void CreateWildMon(enum Species species, u8 level);
bool8 TryGenerateWildMon(const struct WildPokemonInfo *wildMonInfo, enum WildPokemonArea area, u8 flags);
bool8 AreLegendariesInSootopolisPreventingEncounters(void);
u16 GetCurrentMapWildMonHeaderId(void);
u32 ChooseWildMonIndex_Land(const struct WildPokemonInfo *info);
u32 ChooseSweetScentWildMonIndex(const struct WildPokemonInfo *info, enum WildPokemonArea area);
u32 ChooseWildMonIndex_Water(const struct WildPokemonInfo *info);
u32 ChooseWildMonIndex_Rocks(const struct WildPokemonInfo *info);
u32 ChooseHiddenMonIndex(void);
bool32 MapHasNoEncounterData(void);
enum TimeOfDay GetTimeOfDayForEncounters(u32 headerId, enum WildPokemonArea area);
void BufferCurrentMapRouteSignSpecies(void);

u8 GetLandEncounterSlotForMatchCall(const struct WildPokemonInfo *info);
u8 GetWaterEncounterSlotForMatchCall(const struct WildPokemonInfo *info);

#endif // GUARD_WILD_ENCOUNTER_H
