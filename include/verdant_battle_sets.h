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

#define VERDANT_BATTLE_SET_NAME_LENGTH 24

struct VerdantBattleSetRange
{
    u16 offset;
    u8 count;
};

struct VerdantBattleSetChoice
{
    u8 name[VERDANT_BATTLE_SET_NAME_LENGTH];
    struct VerdantBattleSetPreset preset;
};

extern const struct VerdantBattleSetPreset gVerdantBattleSetPresets[NUM_SPECIES];
extern const u8 *const gVerdantDefaultBattleSetNames[NUM_SPECIES];
extern const struct VerdantBattleSetRange gVerdantBattleSetRanges[NUM_SPECIES];
extern const struct VerdantBattleSetChoice gVerdantBattleSetAlternatives[];
bool8 ApplyVerdantBattleSetPreset(struct Pokemon *mon);
bool8 ApplyVerdantBattleSetChoice(struct Pokemon *mon, u8 choice);
u8 GetVerdantBattleSetCount(struct Pokemon *mon);
const u8 *GetVerdantBattleSetName(struct Pokemon *mon, u8 choice);
bool8 IsVerdantLegendarySpecies(u16 species);

#endif // GUARD_VERDANT_BATTLE_SETS_H
