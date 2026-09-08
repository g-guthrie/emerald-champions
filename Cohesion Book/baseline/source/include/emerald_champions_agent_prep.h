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
    EC_AGENT_PREP_BAD_STAT_POINTS,
};

extern volatile u32 gEcAgentPrepCommand;
extern volatile u32 gEcAgentPrepResult;
extern volatile u32 gEcAgentPrepErrorSlot;
extern volatile u32 gEcAgentPrepPartyCount;
extern volatile u32 gEcAgentPrepSpecies[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepPreset[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepFormat[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepLevel[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepMoves[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_MOVE_COUNT];
extern volatile u32 gEcAgentPrepNature[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepAbility[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepItem[EC_AGENT_PREP_PARTY_SIZE];
extern volatile u32 gEcAgentPrepStatPoints[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_STAT_COUNT];

void EmeraldChampionsAgentPrepPoll(void);

#else

static inline void EmeraldChampionsAgentPrepPoll(void) {}

#endif

#endif
