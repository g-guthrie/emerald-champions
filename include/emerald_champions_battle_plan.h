#ifndef GUARD_EMERALD_CHAMPIONS_BATTLE_PLAN_H
#define GUARD_EMERALD_CHAMPIONS_BATTLE_PLAN_H

#include "global.h"
#include "constants/battle.h"
#include "constants/species.h"
#include "constants/moves.h"

enum EmeraldChampionsBattlePlan
{
    EC_BATTLE_PLAN_TRICK_ROOM = 1 << 0,
    EC_BATTLE_PLAN_RAIN = 1 << 1,
    EC_BATTLE_PLAN_SUN = 1 << 2,
    EC_BATTLE_PLAN_SAND = 1 << 3,
    EC_BATTLE_PLAN_SNOW = 1 << 4,
    EC_BATTLE_PLAN_REDIRECTION = 1 << 5,
    EC_BATTLE_PLAN_SETUP = 1 << 6,
    EC_BATTLE_PLAN_TAILWIND = 1 << 7,
    EC_BATTLE_PLAN_ALLY_COMBO = 1 << 8,
    EC_BATTLE_PLAN_PERISH_TRAP = 1 << 9,
    EC_BATTLE_PLAN_PRESSURE = 1 << 10,
};

enum EmeraldChampionsBattleTacticKind
{
    EC_BATTLE_TACTIC_ACTIVATE = 1 << 0,
    EC_BATTLE_TACTIC_AFTER_YOU = 1 << 1,
    EC_BATTLE_TACTIC_INSTRUCT = 1 << 2,
    EC_BATTLE_TACTIC_COMMANDER = 1 << 3,
    EC_BATTLE_TACTIC_SUPPRESS = 1 << 4,
};

struct EmeraldChampionsBattleTactic
{
    u16 trainer;
    u16 actor;
    u16 recipient;
    u16 move;
    u16 kind;
};

u32 EmeraldChampions_GetBattlePlan(enum BattlerId battler);
// Cheap reserve-pair screening. A match only opens a candidate for evaluation;
// native legality, activation survival and resulting payoff still decide it.
u32 EmeraldChampions_GetPartnerTactics(enum BattlerId battler, enum Species species, enum Species partnerSpecies);
bool32 EmeraldChampions_HasTacticActor(enum BattlerId actor, u32 kind);

#endif
