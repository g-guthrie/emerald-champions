#include "global.h"
#include "data.h"
#include "item.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "verdant_battle_sets.h"
#include "constants/abilities.h"
#include "constants/field_specials.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/species.h"

#include "data/pokemon/verdant_battle_sets.h"
#include "data/pokemon/verdant_multi_battle_sets.h"

static const u8 sRecommendedBattleSetName[] = _("Recommended");

static const struct VerdantBattleSetPreset *GetBattleSetPreset(struct Pokemon *mon, u8 choice)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, NULL);
    const struct VerdantBattleSetRange *range;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return NULL;
    if (choice == 0)
        return &gVerdantBattleSetPresets[species];
    range = &gVerdantBattleSetRanges[species];
    if (choice > range->count)
        return NULL;
    return &gVerdantBattleSetAlternatives[range->offset + choice - 1].preset;
}

static u8 ApplyValidatedBattleSetPreset(struct Pokemon *mon, const struct VerdantBattleSetPreset *preset, bool8 replaceSpecialItem)
{
    u16 species;
    u16 currentItem;
    u16 move;
    u8 ppBonuses = 0;
    u8 i;
    u8 j;
    bool8 sawEmptyMove = FALSE;

    species = GetMonData(mon, MON_DATA_SPECIES2, NULL);
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return BATTLE_SET_APPLY_FAILED;

    if (preset->nature >= NUM_NATURES
     || preset->abilitySlot >= NUM_ABILITY_SLOTS
     || gBaseStats[species].abilities[preset->abilitySlot] == ABILITY_NONE)
        return BATTLE_SET_APPLY_FAILED;
    if (IsVerdantProtectedProgressionItem(preset->item))
        return BATTLE_SET_APPLY_FAILED;

    currentItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    if (!replaceSpecialItem && IsVerdantProtectedProgressionItem(currentItem))
    {
        CopyItemName(currentItem, gStringVar2);
        return BATTLE_SET_APPLY_SPECIAL_ITEM;
    }

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = preset->moves[i];
        if (move == MOVE_NONE)
        {
            sawEmptyMove = TRUE;
            continue;
        }
        if (sawEmptyMove || move >= MOVES_COUNT)
            return BATTLE_SET_APPLY_FAILED;
        for (j = 0; j < i; j++)
        {
            if (preset->moves[j] == move)
                return BATTLE_SET_APPLY_FAILED;
        }
    }
    if (preset->moves[0] == MOVE_NONE)
        return BATTLE_SET_APPLY_FAILED;

    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, preset->moves[i], i);
    SetMonData(mon, MON_DATA_NATURE, &preset->nature);
    SetMonData(mon, MON_DATA_ABILITY_NUM, &preset->abilitySlot);
    SetMonData(mon, MON_DATA_HELD_ITEM, &preset->item);
    CalculateMonStats(mon);
    return BATTLE_SET_APPLY_SUCCESS;
}

bool8 ApplyVerdantBattleSetPreset(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, NULL);

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return FALSE;
    return ApplyValidatedBattleSetPreset(mon, &gVerdantBattleSetPresets[species], FALSE) == BATTLE_SET_APPLY_SUCCESS;
}

u8 GetVerdantBattleSetCount(struct Pokemon *mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, NULL);

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return 0;
    return 1 + gVerdantBattleSetRanges[species].count;
}

const u8 *GetVerdantBattleSetName(struct Pokemon *mon, u8 choice)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, NULL);
    const struct VerdantBattleSetRange *range;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return sRecommendedBattleSetName;
    range = &gVerdantBattleSetRanges[species];
    if (choice == 0)
        return gVerdantDefaultBattleSetNames[species] != NULL
             ? gVerdantDefaultBattleSetNames[species]
             : sRecommendedBattleSetName;
    if (choice > range->count)
        return sRecommendedBattleSetName;
    return gVerdantBattleSetAlternatives[range->offset + choice - 1].name;
}

u16 GetVerdantBattleSetItem(struct Pokemon *mon, u8 choice)
{
    const struct VerdantBattleSetPreset *preset = GetBattleSetPreset(mon, choice);

    return preset != NULL ? preset->item : ITEM_NONE;
}

u8 ApplyVerdantBattleSetChoice(struct Pokemon *mon, u8 choice)
{
    const struct VerdantBattleSetPreset *preset = GetBattleSetPreset(mon, choice);

    if (preset == NULL)
        return BATTLE_SET_APPLY_FAILED;
    return ApplyValidatedBattleSetPreset(mon, preset, FALSE);
}

u8 ApplyVerdantRandomWildBattleSet(struct Pokemon *mon)
{
    u8 count = GetVerdantBattleSetCount(mon);
    const struct VerdantBattleSetPreset *preset;

    if (count == 0)
        return BATTLE_SET_APPLY_FAILED;
    preset = GetBattleSetPreset(mon, Random() % count);
    if (preset == NULL)
        return BATTLE_SET_APPLY_FAILED;
    return ApplyValidatedBattleSetPreset(mon, preset, TRUE);
}

bool8 IsVerdantLegendarySpecies(u16 species)
{
    switch (species)
    {
#include "data/pokemon/verdant_legendary_species.h"
        return TRUE;
    default:
        return FALSE;
    }
}

bool8 IsVerdantProtectedProgressionItem(u16 item)
{
    if (item >= FIRST_MEGA_STONE_INDEX && item <= LAST_MEGA_STONE_INDEX)
        return TRUE;
    switch (item)
    {
#include "data/pokemon/verdant_protected_set_items.h"
        return TRUE;
    default:
        return FALSE;
    }
}
