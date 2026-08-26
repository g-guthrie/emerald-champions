#ifndef GUARD_AREA_DEX_H
#define GUARD_AREA_DEX_H

#include "wild_encounter.h"

#define AREA_DEX_MAX_METHODS 8
#define AREA_DEX_MAX_ENTRIES LAND_WILD_COUNT

enum AreaDexMethodId
{
    AREA_DEX_METHOD_GRASS,
    AREA_DEX_METHOD_SURF,
    AREA_DEX_METHOD_OLD_ROD,
    AREA_DEX_METHOD_GOOD_ROD,
    AREA_DEX_METHOD_SUPER_ROD,
    AREA_DEX_METHOD_ROCK_SMASH,
    AREA_DEX_METHOD_HONEY,
    AREA_DEX_METHOD_SPECIAL,
};

struct AreaDexMethod
{
    const struct WildPokemonInfo *info;
    u8 id;
    u8 slotType;
    u8 firstSlot;
    u8 slotCount;
};

struct AreaDexEntry
{
    u16 species;
    u8 chance;
};

u8 AreaDex_BuildCurrentMapMethods(struct AreaDexMethod *methods, u8 capacity);
u8 AreaDex_CollectEntries(const struct AreaDexMethod *method, struct AreaDexEntry *entries, u8 capacity);

#endif // GUARD_AREA_DEX_H
