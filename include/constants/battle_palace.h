#ifndef GUARD_CONSTANTS_BATTLE_PALACE_H
#define GUARD_CONSTANTS_BATTLE_PALACE_H

// Emerald Champions: the Battle Palace facility is retired. The engine still
// carries the nature-based move-selection code (ChooseMoveAndTargetInBattlePalace
// in src/battle_gfx_sfx_util.c and the BATTLE_TYPE_PALACE branches around it),
// so its move groups stay.
#define PALACE_MOVE_GROUP_ATTACK  0
#define PALACE_MOVE_GROUP_DEFENSE 1
#define PALACE_MOVE_GROUP_SUPPORT 2

#endif //GUARD_CONSTANTS_BATTLE_PALACE_H
