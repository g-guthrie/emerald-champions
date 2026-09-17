#ifndef GUARD_BATTLE_ARENA_H
#define GUARD_BATTLE_ARENA_H

#include "constants/battle_arena.h"

// Emerald Champions: the Battle Arena facility is retired and BATTLE_TYPE_ARENA
// is never set any more, but the battle engine still calls these from its
// BATTLE_TYPE_ARENA branches. They are unreachable no-ops in
// src/retired_frontier.c.
u8 BattleArena_ShowJudgmentWindow(u8 *state);
void BattleArena_InitPoints(void);
void BattleArena_AddMindPoints(enum BattlerId battler);
void BattleArena_AddSkillPoints(enum BattlerId battlerAtk);
void BattleArena_DeductSkillPoints(enum BattlerId battler, enum StringID stringId);
void DrawArenaRefereeTextBox(void);
void EraseArenaRefereeTextBox(void);

#endif //GUARD_BATTLE_ARENA_H
