#ifndef GUARD_LEGENDARY_SIGNS_H
#define GUARD_LEGENDARY_SIGNS_H

#include "global.h"
#include "wild_encounter.h"
#include "constants/species.h"
#include "emerald_champions_battle_sets.h"

#include "constants/legendary_signs.h"

// How a gated species is met. Presentation only: no engine rule reads it.
enum LegendaryKind
{
    LEGENDARY_KIND_WILD,   // Ordinary slot in src/data/wild_encounters.json.
    LEGENDARY_KIND_STATIC, // Map-script encounter (shared cap + set path).
    LEGENDARY_KIND_GIFT,
    LEGENDARY_KIND_QUEST,  // Gated wild slot unlocked by a local NPC quest.
    LEGENDARY_KIND_PRIZE,
    LEGENDARY_KIND_BREED,
};

// When a Legendary-class or Ultra Beast species may be acquired. A species
// without a row is always acquirable. Where it appears, and how often, is the
// wild data table's or the map script's business.
struct LegendaryGate
{
    enum Species species;
    u16 unlockFlag;            // 0 = none.
    enum Species requiredSpecies; // Family that must be caught in the Pokedex; NONE = none.
    u8 minimumBadges;
    u8 kind;                   // enum LegendaryKind
    // Weather-anomaly visitor data (src/weather_anomaly.c). A visitor is wild
    // only inside its own anomaly while the anomaly window is open, and an
    // ordinary gated resident of its home map afterwards. Residents keep 0.
    u8 anomalyId;              // 1-based, append-only save id; 0 = not a visitor.
    u8 anomalyHabitat;         // enum WildPokemonArea: WILD_AREA_LAND or WILD_AREA_WATER.
    u8 anomalyWeather;         // enum OverworldWeather shown on the home map; WEATHER_NONE otherwise.
    u16 anomalyMap;            // MAP_* home map (group << 8 | num).
};

extern const struct LegendaryGate gLegendaryGates[LEGENDARY_SIGN_COUNT];

// Hand-authored competitive sets for the eight legendaries listed in
// data/pokemon/legendary_authored_sets.h. Every Legendary-class or Ultra Beast
// encounter (wild slot, setwildbattle, CreateEventLegalEnemyMon, roamer) uses
// the authored set when one exists and otherwise
// ApplyEmeraldChampionsRandomNonMegaSet; see ApplyLegendaryEncounterSet.
struct LegendaryAuthoredSet
{
    enum Species species;
    struct EmeraldChampionsBattleSet set;
};

extern const struct LegendaryAuthoredSet gLegendaryAuthoredSets[];
extern const u32 gLegendaryAuthoredSetCount;

// Returns the authored set for species, or NULL when species has no row and
// should receive the random non-Mega set.
const struct EmeraldChampionsBattleSet *GetLegendaryAuthoredSet(enum Species species);

// Shared encounter rule: Legendary-class and Ultra Beast species spawn at the
// current level cap with a competitive set. Paradox and ordinary species keep
// their own level (clamped to the cap by the caller) and natural moves.
bool32 IsLegendaryEncounterSpecies(enum Species species);
u8 GetLegendaryEncounterLevel(enum Species species);
void ApplyLegendaryEncounterSet(struct Pokemon *mon, enum Item fallbackItem);
// Legendary/UB slots are acquirable only while their gate is open and the
// species is uncaught; everything else always is.
bool32 IsWildSlotSpeciesAcquirable(enum Species species);

bool32 IsLegendarySignUnlocked(enum LegendarySignId signId);
bool32 IsLegendarySignCaught(enum LegendarySignId signId);
bool32 CanAcquireLegendarySignSpecies(enum Species species);
void UnlockLegendarySign(enum LegendarySignId signId);
void RetryPendingLegendaryRelics(void);
void MarkLegendarySignCaughtBySpecies(enum Species species);
enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species);
bool32 PlayerPartyHasSpeciesFamily(enum Species species);
bool32 HasCaughtSpeciesFamily(enum Species species);
void TryUnlockSelectedLegendarySign(void);
u16 GetSelectedLegendarySignState(void);
void CreateSelectedLegendarySignEncounter(void);
void TryGiveSelectedLegendarySignReward(void);
void CreateEmeraldChampionsStaticLegendaryEncounter(void);
void TryUnlockDarkraiLegendarySign(void);
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
