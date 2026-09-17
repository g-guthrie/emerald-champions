#ifndef GUARD_BATTLE_PYRAMID_H
#define GUARD_BATTLE_PYRAMID_H

#include "constants/battle_pyramid.h"

// Emerald Champions: the Battle Pyramid is retired and its floors are deleted.
// src/battle_util.c and src/battle_script_commands.c still ask these three
// questions; see src/retired_frontier.c.
u8 GetPyramidRunMultiplier(void);
u8 CurrentBattlePyramidLocation(void);
u16 GetBattlePyramidPickupItemId(void);

#endif // GUARD_BATTLE_PYRAMID_H
