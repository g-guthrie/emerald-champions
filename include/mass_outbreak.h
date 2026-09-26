#ifndef GUARD_MASS_OUTBREAK_H
#define GUARD_MASS_OUTBREAK_H

#include "global.h"
#include "constants/mass_outbreak.h"

struct MassOutbreak
{
    enum Species species;
    enum Move moves[MAX_MON_MOVES];
    u8 level;
    u8 probability;
    u8 duration;
    u16 location;
};

void PrepareTvShowForRandomOutbreak(TVShow *show);
void UpdateMassOutbreakDaysLeft(u16 days);
bool32 IsMassOutbreakActive(void);
struct Pokemon;
void ApplyMassOutbreakMoves(struct Pokemon *mon);
bool8 SetUpMassOutbreakEncounter(u8 flags);
// An outbreak is running on this map: it takes outbreakPokemonProbability
// percent of land encounters there (DoMassOutbreakEncounterTest).
bool32 IsMassOutbreakOnMap(u8 mapGroup, u8 mapNum);
bool8 DoMassOutbreakEncounterTest(void);
void StartStaticMassOutbreak(enum MassOutbreakIndex outbreakIdx);
enum Species GetStaticOutbreakSpecies(enum MassOutbreakIndex outbreakIdx);
u8 *GetStaticOutbreakMapName(u8 *dest, enum MassOutbreakIndex outbreakIdx);

#endif // GUARD_MASS_OUTBREAK_H
