#ifndef GUARD_VERDANT_BATTLE_SETS_H
#define GUARD_VERDANT_BATTLE_SETS_H

#include "global.h"

// A deliberately small runtime record.  The companion JSON manifest records
// why each set was chosen; the ROM only needs the values it applies.
struct VerdantBattleSetPreset
{
    u16 moves[MAX_MON_MOVES];
    u16 item;
    u16 requiredItem;
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
extern const u16 gVerdantFreeBattleItems[];
bool8 ApplyVerdantBattleSetPreset(struct Pokemon *mon);
u8 ApplyVerdantBattleSetChoice(struct Pokemon *mon, u8 choice);
u8 ApplyVerdantRandomWildBattleSet(struct Pokemon *mon);
u8 GetVerdantBattleSetCount(struct Pokemon *mon);
const u8 *GetVerdantBattleSetName(struct Pokemon *mon, u8 choice);
u16 GetVerdantBattleSetItem(struct Pokemon *mon, u8 choice);
u16 GetVerdantBattleSetRequiredItem(struct Pokemon *mon, u8 choice);
bool8 IsVerdantLegendarySpecies(u16 species);
bool8 IsVerdantProtectedProgressionItem(u16 item);
u8 GetVerdantBattleSetRawCount(u16 species);
const struct VerdantBattleSetPreset *GetVerdantBattleSetRawPreset(u16 species, u8 rawChoice);
u8 ApplyVerdantOpponentBattleSet(struct Pokemon *mon, u8 rawChoice);
u8 ApplyVerdantGiftBattleSet(struct Pokemon *mon, u8 rawChoice);

#endif // GUARD_VERDANT_BATTLE_SETS_H
