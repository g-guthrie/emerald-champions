#ifndef GUARD_LEGENDARY_SIGNS_H
#define GUARD_LEGENDARY_SIGNS_H

#include "global.h"
#include "wild_encounter.h"
#include "constants/species.h"

#include "constants/legendary_signs.h"

enum LegendarySignSource
{
    LEGENDARY_SOURCE_RARE_WILD,
    LEGENDARY_SOURCE_VISIBLE,
    LEGENDARY_SOURCE_BREEDING,
    LEGENDARY_SOURCE_GAME_CORNER,
    LEGENDARY_SOURCE_CIRCUIT,
    LEGENDARY_SOURCE_MASTERY,
    LEGENDARY_SOURCE_ORDINARY_WILD,
    LEGENDARY_SOURCE_NATIVE_WILD,
};

struct LegendarySignDefinition
{
    enum Species species;
    u16 mapId;
    enum Species requiredSpecies;
    u16 requiredFlag;
    enum LegendarySignSource source;
    u8 minimumBadges;
    s8 levelOffset;
};

extern const struct LegendarySignDefinition gLegendarySignDefinitions[LEGENDARY_SIGN_COUNT];

bool32 IsLegendarySignUnlocked(enum LegendarySignId signId);
bool32 IsLegendarySignCaught(enum LegendarySignId signId);
bool32 IsNativeWildLegendarySpecies(enum Species species);
bool32 CanAcquireLegendarySignSpecies(enum Species species);
void UnlockLegendarySign(enum LegendarySignId signId);
void RetryPendingLegendaryRelics(void);
void MarkLegendarySignCaughtBySpecies(enum Species species);
enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species);
bool32 PlayerPartyHasSpeciesFamily(enum Species species);
bool32 IsLegendarySignOrdinaryWildSpecies(enum Species species);
void DoesPlayerPartyHaveSelectedSpeciesFamily(void);
void TryUnlockSelectedLegendarySign(void);
u16 GetSelectedLegendarySignState(void);
u16 ShouldShowSelectedLegendarySignObject(void);
u16 GetSelectedLegendarySignLevel(void);
void CreateSelectedLegendarySignEncounter(void);
void TryGiveSelectedLegendarySignReward(void);
void CreateEmeraldChampionsStaticLegendaryEncounter(void);
void TryUnlockDarkraiLegendarySign(void);
enum Species ChooseRareWildLegendarySpecies(enum WildPokemonArea area, bool32 sweetScent);
void BufferNextLocalLegendaryRequirement(void);
void BufferNextCenterLegendaryLead(void);
u16 GetHeatranDiscoveryState(void);
const u8 *GetLegendaryDisplayName(enum Species species);
void TryUnlockLocalLegendaryDiscovery(void);
void ResetLegendaryEncounterVisits(void);
void FinishLegendaryLandmarkEncounter(void);
void HideRestingLegendaryObject(void);
void ResearchSelectedLegendarySign(void);
void TryGiveArceusLegendarySignMasteryReward(void);
u8 GiveLegendarySignReward(enum Species species, u8 level);
#define LEGENDARY_REWARD_UNAVAILABLE 3

#endif // GUARD_LEGENDARY_SIGNS_H
