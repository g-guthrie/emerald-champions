#include "global.h"
#include "data.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "verdant_battle_sets.h"
#include "constants/abilities.h"
#include "constants/field_specials.h"
#include "constants/flags.h"
#include "constants/moves.h"
#include "constants/items.h"
#include "constants/species.h"

#include "data/pokemon/verdant_battle_sets.h"
#include "data/pokemon/verdant_multi_battle_sets.h"

static const u8 sRecommendedBattleSetName[] = _("Recommended");

static bool8 ResolveBattleSetChoice(
    struct Pokemon *mon,
    u8 choice,
    const struct VerdantBattleSetPreset **presetOut,
    const u8 **nameOut)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, NULL);
    const struct VerdantBattleSetRange *range;
    const struct VerdantBattleSetPreset *preset;
    const u8 *name;
    u8 rawChoice;
    u8 visibleChoice = 0;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return FALSE;
    range = &gVerdantBattleSetRanges[species];
    for (rawChoice = 0; rawChoice <= range->count; rawChoice++)
    {
        if (rawChoice == 0)
        {
            preset = &gVerdantBattleSetPresets[species];
            name = gVerdantDefaultBattleSetNames[species] != NULL
                 ? gVerdantDefaultBattleSetNames[species]
                 : sRecommendedBattleSetName;
        }
        else
        {
            const struct VerdantBattleSetChoice *alternative =
                &gVerdantBattleSetAlternatives[range->offset + rawChoice - 1];

            preset = &alternative->preset;
            name = alternative->name;
        }
        if (preset->requiredItem != ITEM_NONE && !FlagGet(FLAG_SYS_RECEIVED_KEYSTONE))
            continue;
        if (visibleChoice++ == choice)
        {
            if (presetOut != NULL)
                *presetOut = preset;
            if (nameOut != NULL)
                *nameOut = name;
            return TRUE;
        }
    }
    return FALSE;
}

static const struct VerdantBattleSetPreset *GetBattleSetPreset(struct Pokemon *mon, u8 choice)
{
    const struct VerdantBattleSetPreset *preset = NULL;

    ResolveBattleSetChoice(mon, choice, &preset, NULL);
    return preset;
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
    if (IsVerdantProtectedProgressionItem(preset->item)
     || (preset->requiredItem != ITEM_NONE
      && (preset->requiredItem < FIRST_MEGA_STONE_INDEX
       || preset->requiredItem > LAST_MEGA_STONE_INDEX)))
        return BATTLE_SET_APPLY_FAILED;
    if (preset->requiredItem != ITEM_NONE && !FlagGet(FLAG_SYS_RECEIVED_KEYSTONE))
        return BATTLE_SET_APPLY_FAILED;

    currentItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    if (!replaceSpecialItem
     && IsVerdantProtectedProgressionItem(currentItem)
     && currentItem != preset->requiredItem)
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
    if (currentItem != preset->requiredItem)
        SetMonData(mon, MON_DATA_HELD_ITEM, &preset->item);
    CalculateMonStats(mon);
    return preset->requiredItem != ITEM_NONE
         ? BATTLE_SET_APPLY_MEGA_SET
         : BATTLE_SET_APPLY_SUCCESS;
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
    const struct VerdantBattleSetRange *range;
    const struct VerdantBattleSetPreset *preset;
    u8 rawChoice;
    u8 count = 0;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return 0;
    range = &gVerdantBattleSetRanges[species];
    for (rawChoice = 0; rawChoice <= range->count; rawChoice++)
    {
        preset = rawChoice == 0
               ? &gVerdantBattleSetPresets[species]
               : &gVerdantBattleSetAlternatives[range->offset + rawChoice - 1].preset;
        if (preset->requiredItem == ITEM_NONE || FlagGet(FLAG_SYS_RECEIVED_KEYSTONE))
            count++;
    }
    return count;
}

const u8 *GetVerdantBattleSetName(struct Pokemon *mon, u8 choice)
{
    const u8 *name = sRecommendedBattleSetName;

    ResolveBattleSetChoice(mon, choice, NULL, &name);
    return name;
}

u16 GetVerdantBattleSetItem(struct Pokemon *mon, u8 choice)
{
    const struct VerdantBattleSetPreset *preset = GetBattleSetPreset(mon, choice);

    if (preset == NULL)
        return ITEM_NONE;
    return preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
}

u16 GetVerdantBattleSetRequiredItem(struct Pokemon *mon, u8 choice)
{
    const struct VerdantBattleSetPreset *preset = GetBattleSetPreset(mon, choice);

    return preset != NULL ? preset->requiredItem : ITEM_NONE;
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

u8 GetVerdantBattleSetRawCount(u16 species)
{
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return 0;
    if (gVerdantBattleSetPresets[species].moves[0] == MOVE_NONE)
        return 0;
    return gVerdantBattleSetRanges[species].count + 1;
}

const struct VerdantBattleSetPreset *GetVerdantBattleSetRawPreset(u16 species, u8 rawChoice)
{
    const struct VerdantBattleSetRange *range;

    if (GetVerdantBattleSetRawCount(species) == 0)
        return NULL;
    range = &gVerdantBattleSetRanges[species];
    if (rawChoice == 0)
        return &gVerdantBattleSetPresets[species];
    if (rawChoice > range->count)
        return NULL;
    return &gVerdantBattleSetAlternatives[range->offset + rawChoice - 1].preset;
}

u8 ApplyVerdantOpponentBattleSet(struct Pokemon *mon, u8 rawChoice)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES2, NULL);
    const struct VerdantBattleSetPreset *preset = GetVerdantBattleSetRawPreset(species, rawChoice);
    u16 item;
    u8 ppBonuses = 0;
    u8 i;

    if (preset == NULL
     || preset->nature >= NUM_NATURES
     || preset->abilitySlot >= NUM_ABILITY_SLOTS
     || gBaseStats[species].abilities[preset->abilitySlot] == ABILITY_NONE)
        return BATTLE_SET_APPLY_FAILED;

    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, preset->moves[i], i);
    SetMonData(mon, MON_DATA_NATURE, &preset->nature);
    SetMonData(mon, MON_DATA_ABILITY_NUM, &preset->abilitySlot);
    item = preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
    SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    TryUpdateMonFormForHeldItem(mon);
    CalculateMonStats(mon);
    return preset->requiredItem != ITEM_NONE
         ? BATTLE_SET_APPLY_MEGA_SET
         : BATTLE_SET_APPLY_SUCCESS;
}

u8 ApplyVerdantGiftBattleSet(struct Pokemon *mon, u8 rawChoice)
{
    u8 result = ApplyVerdantOpponentBattleSet(mon, rawChoice);
    u16 item;

    if (result == BATTLE_SET_APPLY_FAILED)
        return result;
    item = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);
    if (IsVerdantProtectedProgressionItem(item))
    {
        item = ITEM_NONE;
        SetMonData(mon, MON_DATA_HELD_ITEM, &item);
        TryUpdateMonFormForHeldItem(mon);
    }
    return result;
}
