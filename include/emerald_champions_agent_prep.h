#ifndef GUARD_EMERALD_CHAMPIONS_AGENT_PREP_H
#define GUARD_EMERALD_CHAMPIONS_AGENT_PREP_H

#include "global.h"

#if EC_HEADLESS_FIXTURES

#define EC_AGENT_PREP_KEEP UINT32_MAX
#define EC_AGENT_PREP_PARTY_SIZE 6
#define EC_AGENT_PREP_MOVE_COUNT 4
#define EC_AGENT_PREP_STAT_COUNT 6

enum EmeraldChampionsAgentPrepResult
{
    EC_AGENT_PREP_PENDING,
    EC_AGENT_PREP_SUCCESS,
    EC_AGENT_PREP_BAD_COMMAND,
    EC_AGENT_PREP_IN_BATTLE,
    EC_AGENT_PREP_BAD_PARTY,
    EC_AGENT_PREP_BAD_SPECIES,
    EC_AGENT_PREP_BAD_LEVEL,
    EC_AGENT_PREP_BAD_PRESET,
    EC_AGENT_PREP_BAD_MOVE,
    EC_AGENT_PREP_BAD_NATURE,
    EC_AGENT_PREP_BAD_ABILITY,
    EC_AGENT_PREP_BAD_ITEM,
    EC_AGENT_PREP_BAD_EVS,
    EC_AGENT_PREP_RESTRICTED_PARTY, // a second Legendary/Mythical, Ultra Beast or Paradox
    EC_AGENT_PREP_BAD_IVS,
    EC_AGENT_PREP_BAD_FRIENDSHIP,
    EC_AGENT_PREP_BAD_POKERUS,
    EC_AGENT_PREP_BAD_PP_BONUSES,
};

extern volatile u32 gEcAgentPrepCommand;
extern volatile u32 gEcAgentPrepResult;
extern volatile u32 gEcAgentPrepErrorSlot;
extern volatile u32 gEcAgentPrepPartyCount;
extern volatile u32 gEcAgentPrepExtended;
extern volatile u32 gEcAgentPrepSpecies[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepPreset[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepFormat[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepLevel[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepMoves[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_MOVE_COUNT];
extern volatile u32 gEcAgentPrepNature[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepAbility[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepItem[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepEvs[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_STAT_COUNT];
// Raw native values. Acquisition/service availability is checked by the host
// arsenal before preparation; KEEP retains the native creation defaults.
extern volatile u32 gEcAgentPrepIvs[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_STAT_COUNT];
extern volatile u32 gEcAgentPrepFriendship[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepPokerus[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepPpBonuses[EC_AGENT_PREP_PARTY_SIZE];
void EmeraldChampionsAgentPrepPoll(void);

#else

static inline void EmeraldChampionsAgentPrepPoll(void) {}

#endif

#endif
