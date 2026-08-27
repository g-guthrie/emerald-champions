#ifndef GUARD_VERDANT_BATTLE_SETS_H
#define GUARD_VERDANT_BATTLE_SETS_H

#include "global.h"

// A deliberately small runtime record.  The companion JSON manifest records
// why each set was chosen; the ROM only needs the values it applies.
struct VerdantBattleSetPreset
{
    u16 moves[MAX_MON_MOVES];
    u8 nature;
    u8 abilitySlot;
};

extern const struct VerdantBattleSetPreset gVerdantBattleSetPresets[NUM_SPECIES];

#endif // GUARD_VERDANT_BATTLE_SETS_H
