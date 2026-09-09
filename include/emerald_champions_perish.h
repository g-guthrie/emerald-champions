#ifndef GUARD_EMERALD_CHAMPIONS_PERISH_H
#define GUARD_EMERALD_CHAMPIONS_PERISH_H

#include "global.h"
#include "constants/battle.h"
#include "constants/moves.h"

bool32 EC_PerishMustEscape(enum BattlerId battler);
bool32 EC_PerishShouldPivotEarly(enum BattlerId battler);
s32 EC_PerishPlanScore(enum BattlerId battler, enum Move move);

#endif
