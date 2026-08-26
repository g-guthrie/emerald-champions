#include "global.h"
#include "area_dex.h"
#include "wild_encounter.h"
#include "constants/maps.h"
#include "constants/species.h"

static bool8 AddMethod(
    struct AreaDexMethod *methods,
    u8 *count,
    u8 capacity,
    u8 id,
    const struct WildPokemonInfo *info,
    u8 slotType,
    u8 firstSlot,
    u8 slotCount)
{
    struct AreaDexMethod *method;

    if (*count >= capacity)
        return FALSE;

    method = &methods[(*count)++];
    method->info = info;
    method->id = id;
    method->slotType = slotType;
    method->firstSlot = firstSlot;
    method->slotCount = slotCount;
    return TRUE;
}

u8 AreaDex_BuildCurrentMapMethods(struct AreaDexMethod *methods, u8 capacity)
{
    u8 count = 0;
    u16 headerId = GetCurrentMapWildMonHeaderId();

    if (headerId != 0xFFFF)
    {
        const struct WildPokemonHeader *header = &gWildMonHeaders[headerId];

        if (header->landMonsInfo != NULL)
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_GRASS, header->landMonsInfo, WILD_SLOT_LAND, 0, LAND_WILD_COUNT);
        if (header->waterMonsInfo != NULL)
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_SURF, header->waterMonsInfo, WILD_SLOT_WATER, 0, WATER_WILD_COUNT);
        if (header->fishingMonsInfo != NULL)
        {
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_OLD_ROD, header->fishingMonsInfo, WILD_SLOT_OLD_ROD, 0, 2);
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_GOOD_ROD, header->fishingMonsInfo, WILD_SLOT_GOOD_ROD, 2, 3);
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_SUPER_ROD, header->fishingMonsInfo, WILD_SLOT_SUPER_ROD, 5, 5);
        }
        if (header->rockSmashMonsInfo != NULL)
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_ROCK_SMASH, header->rockSmashMonsInfo, WILD_SLOT_ROCK_SMASH, 0, ROCK_WILD_COUNT);
        if (header->honeyMonsInfo != NULL)
            AddMethod(methods, &count, capacity, AREA_DEX_METHOD_HONEY, header->honeyMonsInfo, WILD_SLOT_HONEY, 0, HONEY_WILD_COUNT);
    }

    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROUTE119)
     && gSaveBlock1Ptr->location.mapNum == MAP_NUM(ROUTE119))
        AddMethod(methods, &count, capacity, AREA_DEX_METHOD_SPECIAL, NULL, WILD_SLOT_OLD_ROD, 0, 1);

    return count;
}

static void SortEntriesByChance(struct AreaDexEntry *entries, u8 count)
{
    u8 i;

    for (i = 1; i < count; i++)
    {
        struct AreaDexEntry entry = entries[i];
        u8 j = i;

        while (j > 0 && entries[j - 1].chance < entry.chance)
        {
            entries[j] = entries[j - 1];
            j--;
        }
        entries[j] = entry;
    }
}

u8 AreaDex_CollectEntries(const struct AreaDexMethod *method, struct AreaDexEntry *entries, u8 capacity)
{
    u8 count = 0;
    u8 i;

    if (method == NULL || capacity == 0)
        return 0;

    if (method->id == AREA_DEX_METHOD_SPECIAL)
    {
        entries[0].species = SPECIES_FEEBAS;
        entries[0].chance = 100;
        return 1;
    }

    for (i = 0; i < method->slotCount; i++)
    {
        u8 j;
        u16 species = method->info->wildPokemon[method->firstSlot + i].species;
        u8 chance = GetWildEncounterSlotChance(method->slotType, i);

        for (j = 0; j < count; j++)
        {
            if (entries[j].species == species)
            {
                entries[j].chance += chance;
                break;
            }
        }

        if (j == count && count < capacity)
        {
            entries[count].species = species;
            entries[count].chance = chance;
            count++;
        }
    }

    SortEntriesByChance(entries, count);
    return count;
}
