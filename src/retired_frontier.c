// Emerald Champions: the Battle Dome, Palace, Arena, Factory, Pike and Pyramid
// are deleted, along with the Apprentice. Their maps, scripts, data and drivers
// are gone from the tree, so none of the battle types or locations below can be
// reached in play.
//
// The battle engine (src/battle_main.c, src/battle_util.c,
// src/battle_script_commands.c, src/battle_controllers.c,
// src/battle_move_resolution.c and src/battle_ai_main.c) still carries the
// BATTLE_TYPE_ARENA / BATTLE_TYPE_FACTORY / Pyramid / Pike branches that call
// into those facilities. Those files belong to the battle-AI session, so this
// file is the smallest stub that keeps them linking: every entry point answers
// "not in that facility" and does nothing.

#include "global.h"
#include "battle.h"
#include "battle_arena.h"
#include "battle_factory.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "constants/battle_arena.h"
#include "constants/battle_pyramid.h"
#include "constants/items.h"

u8 BattleArena_ShowJudgmentWindow(u8 *state)
{
    *state = 0;
    return ARENA_RESULT_TIE;
}

void BattleArena_InitPoints(void)
{
}

void BattleArena_AddMindPoints(enum BattlerId battler)
{
}

void BattleArena_AddSkillPoints(enum BattlerId battlerAtk)
{
}

void BattleArena_DeductSkillPoints(enum BattlerId battler, enum StringID stringId)
{
}

void DrawArenaRefereeTextBox(void)
{
}

void EraseArenaRefereeTextBox(void)
{
}

u64 GetAiScriptsInBattleFactory(void)
{
    return 0;
}

bool8 InBattlePike(void)
{
    return FALSE;
}

u8 GetPyramidRunMultiplier(void)
{
    return 1;
}

u8 CurrentBattlePyramidLocation(void)
{
    return PYRAMID_LOCATION_NONE;
}

u16 GetBattlePyramidPickupItemId(void)
{
    return ITEM_NONE;
}
