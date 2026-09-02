#include "global.h"
#include "data.h"
#include "emerald_champions_battle_sets.h"
#include "item.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "constants/battle.h"
#include "constants/hold_effects.h"
#include "constants/items.h"

#include "data/pokemon/emerald_champions_battle_sets.h"

static const u8 sRecommendedSetName[] = _("Recommended");
static const enum Item sEmeraldChampionsEvolutionItems[] =
{
#include "data/emerald_champions_evolution_items.h"
};

static bool32 HasMegaAccess(void)
{
    return CheckBagHasItem(ITEM_MEGA_RING, 1);
}

bool32 IsEmeraldChampionsProtectedProgressionItem(enum Item item)
{
    if (item == ITEM_NONE)
        return FALSE;

    // The complete archive is generated from the live solo-evolution tables.
    // Some entries (notably Deep Sea Tooth/Scale) are functional held battle
    // items rather than ITEM_TYPE_EVOLUTION_ITEM, so sort type alone cannot
    // protect the player's finite campaign reward.
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsEvolutionItems); i++)
    {
        if (item == sEmeraldChampionsEvolutionItems[i])
            return TRUE;
    }

    switch (gItemsInfo[item].sortType)
    {
    case ITEM_TYPE_MEGA_STONE:
    case ITEM_TYPE_Z_CRYSTAL:
    case ITEM_TYPE_TERA_SHARD:
    case ITEM_TYPE_PLATE:
    case ITEM_TYPE_MEMORY:
    case ITEM_TYPE_DRIVE:
        return TRUE;
    default:
        break;
    }

    switch (item)
    {
    case ITEM_RED_ORB:
    case ITEM_BLUE_ORB:
    case ITEM_RUSTED_SWORD:
    case ITEM_RUSTED_SHIELD:
    case ITEM_WELLSPRING_MASK:
    case ITEM_HEARTHFLAME_MASK:
    case ITEM_CORNERSTONE_MASK:
        return TRUE;
    default:
        return FALSE;
    }
}

bool32 IsEmeraldChampionsOrdinaryWildSpecies(enum Species species)
{
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return FALSE;
    if (gSpeciesInfo[species].baseHP == 0)
        return FALSE;

    // Ultra Beasts and Paradox Pokemon can be ordinary random encounters in
    // curated campaign maps. They therefore receive the same immediately
    // usable competitive loadouts as every other table-seeded wild species.
    // True legendary/mythical and temporary battle forms remain excluded;
    // Legendary Signs opt in separately when their quest calls for a wild
    // encounter.
    return !gSpeciesInfo[species].isRestrictedLegendary
        && !gSpeciesInfo[species].isSubLegendary
        && !gSpeciesInfo[species].isMythical
        && !gSpeciesInfo[species].isMegaEvolution
        && !gSpeciesInfo[species].isPrimalReversion
        && !gSpeciesInfo[species].isUltraBurst
        && !gSpeciesInfo[species].isGigantamax
        && !gSpeciesInfo[species].isTeraForm;
}

static bool32 FindAbilitySlot(enum Species species, enum Ability ability, u32 *slot)
{
    for (u32 i = 0; i < NUM_ABILITY_SLOTS; i++)
    {
        if (gSpeciesInfo[species].abilities[i] == ability)
        {
            *slot = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool32 FindFallbackAbilitySlot(enum Species species, u32 *slot)
{
    // Hidden Abilities are usually the most deliberately competitive fallback,
    // followed by the second and first ordinary slots.
    for (u32 i = NUM_NORMAL_ABILITY_SLOTS; i < NUM_ABILITY_SLOTS; i++)
    {
        if (gSpeciesInfo[species].abilities[i] != ABILITY_NONE)
        {
            *slot = i;
            return TRUE;
        }
    }
    for (s32 i = NUM_NORMAL_ABILITY_SLOTS - 1; i >= 0; i--)
    {
        if (gSpeciesInfo[species].abilities[i] != ABILITY_NONE)
        {
            *slot = i;
            return TRUE;
        }
    }
    return FALSE;
}

static bool32 IsValidBattleFormat(u8 format)
{
    return format < EC_BATTLE_FORMAT_COUNT;
}

static const struct EmeraldChampionsBattleSet *GetDefaultSetTable(u8 format)
{
    return format == EC_BATTLE_FORMAT_SINGLES
         ? gEmeraldChampionsSinglesDefaultBattleSets
         : gEmeraldChampionsDefaultBattleSets;
}

static const u8 *const *GetDefaultSetNameTable(u8 format)
{
    return format == EC_BATTLE_FORMAT_SINGLES
         ? gEmeraldChampionsSinglesDefaultBattleSetNames
         : gEmeraldChampionsDefaultBattleSetNames;
}

static const struct EmeraldChampionsBattleSetRange *GetSetRangeTable(u8 format)
{
    return format == EC_BATTLE_FORMAT_SINGLES
         ? gEmeraldChampionsSinglesBattleSetRanges
         : gEmeraldChampionsBattleSetRanges;
}

static const struct EmeraldChampionsBattleSetChoice *GetSetAlternativeTable(u8 format)
{
    return format == EC_BATTLE_FORMAT_SINGLES
         ? gEmeraldChampionsSinglesBattleSetAlternatives
         : gEmeraldChampionsBattleSetAlternatives;
}

static bool32 HasDirectBattleSet(enum Species species, u8 format)
{
    const struct EmeraldChampionsBattleSet *defaults = GetDefaultSetTable(format);

    return species > SPECIES_NONE
        && species < NUM_SPECIES
        && IsValidBattleFormat(format)
        && defaults[species].moves[0] != MOVE_NONE;
}

static enum Species ResolveBattleSetSpecies(enum Species species, u8 format)
{
    const u16 *formTable;

    if (HasDirectBattleSet(species, format))
        return species;
    if (species <= SPECIES_NONE || species >= NUM_SPECIES)
        return species;

    // Cosmetic, Totem, and temporary battle forms share the first authored
    // set in their native form table. Regional and mechanically distinct
    // forms receive explicit entries from the handbook supplement instead.
    formTable = gSpeciesInfo[species].formSpeciesIdTable;
    if (formTable != NULL)
    {
        for (u32 i = 0; formTable[i] != FORM_SPECIES_END; i++)
        {
            enum Species candidate = formTable[i];
            if (HasDirectBattleSet(candidate, format))
                return candidate;
        }
    }
    return species;
}

static u8 GetRawBattleSetCountForFormat(enum Species species, u8 format)
{
    const struct EmeraldChampionsBattleSet *defaults;
    const struct EmeraldChampionsBattleSetRange *ranges;

    if (!IsValidBattleFormat(format))
        return 0;
    species = ResolveBattleSetSpecies(species, format);
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return 0;
    defaults = GetDefaultSetTable(format);
    ranges = GetSetRangeTable(format);
    if (defaults[species].moves[0] == MOVE_NONE)
        return 0;
    return ranges[species].count + 1;
}

static const struct EmeraldChampionsBattleSet *GetRawBattleSetForFormat(
    enum Species species,
    u8 rawChoice,
    u8 format)
{
    const struct EmeraldChampionsBattleSetRange *range;
    const struct EmeraldChampionsBattleSet *defaults;
    const struct EmeraldChampionsBattleSetChoice *alternatives;

    if (GetRawBattleSetCountForFormat(species, format) == 0)
        return NULL;
    species = ResolveBattleSetSpecies(species, format);
    defaults = GetDefaultSetTable(format);
    alternatives = GetSetAlternativeTable(format);
    range = &GetSetRangeTable(format)[species];
    if (rawChoice == 0)
        return &defaults[species];
    if (rawChoice > range->count)
        return NULL;
    return &alternatives[range->offset + rawChoice - 1].preset;
}

u8 GetEmeraldChampionsRawBattleSetCount(enum Species species)
{
    return GetRawBattleSetCountForFormat(species, EC_BATTLE_FORMAT_DOUBLES);
}

const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSet(enum Species species, u8 rawChoice)
{
    return GetRawBattleSetForFormat(species, rawChoice, EC_BATTLE_FORMAT_DOUBLES);
}

static bool32 ResolveVisibleChoice(
    struct Pokemon *mon,
    u8 choice,
    u8 format,
    const struct EmeraldChampionsBattleSet **presetOut,
    const u8 **nameOut)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Species setSpecies;
    const struct EmeraldChampionsBattleSetRange *range;
    const struct EmeraldChampionsBattleSet *defaults;
    const u8 *const *defaultNames;
    const struct EmeraldChampionsBattleSetChoice *alternatives;
    u8 visibleChoice = 0;

    if (GetRawBattleSetCountForFormat(species, format) == 0)
        return FALSE;

    setSpecies = ResolveBattleSetSpecies(species, format);
    defaults = GetDefaultSetTable(format);
    defaultNames = GetDefaultSetNameTable(format);
    alternatives = GetSetAlternativeTable(format);
    range = &GetSetRangeTable(format)[setSpecies];
    for (u8 rawChoice = 0; rawChoice <= range->count; rawChoice++)
    {
        const struct EmeraldChampionsBattleSet *preset;
        const u8 *name;

        if (rawChoice == 0)
        {
            preset = &defaults[setSpecies];
            name = defaultNames[setSpecies] != NULL
                 ? defaultNames[setSpecies]
                 : sRecommendedSetName;
        }
        else
        {
            const struct EmeraldChampionsBattleSetChoice *alternative =
                &alternatives[range->offset + rawChoice - 1];
            preset = &alternative->preset;
            name = alternative->name;
        }

        if (preset->requiredItem != ITEM_NONE && !HasMegaAccess())
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

u8 GetEmeraldChampionsBattleSetCountForFormat(struct Pokemon *mon, u8 format)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Species setSpecies;
    const struct EmeraldChampionsBattleSetRange *range;
    u8 count = 0;

    if (GetRawBattleSetCountForFormat(species, format) == 0)
        return 0;
    setSpecies = ResolveBattleSetSpecies(species, format);
    range = &GetSetRangeTable(format)[setSpecies];
    for (u8 rawChoice = 0; rawChoice <= range->count; rawChoice++)
    {
        const struct EmeraldChampionsBattleSet *preset = GetRawBattleSetForFormat(species, rawChoice, format);
        if (preset->requiredItem == ITEM_NONE || HasMegaAccess())
            count++;
    }
    return count;
}

u8 GetEmeraldChampionsBattleSetCount(struct Pokemon *mon)
{
    return GetEmeraldChampionsBattleSetCountForFormat(mon, EC_BATTLE_FORMAT_DOUBLES);
}

const u8 *GetEmeraldChampionsBattleSetNameForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const u8 *name = sRecommendedSetName;
    ResolveVisibleChoice(mon, choice, format, NULL, &name);
    return name;
}

const u8 *GetEmeraldChampionsBattleSetName(struct Pokemon *mon, u8 choice)
{
    return GetEmeraldChampionsBattleSetNameForFormat(mon, choice, EC_BATTLE_FORMAT_DOUBLES);
}

enum Item GetEmeraldChampionsBattleSetItemForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const struct EmeraldChampionsBattleSet *preset = NULL;
    if (!ResolveVisibleChoice(mon, choice, format, &preset, NULL))
        return ITEM_NONE;
    return preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
}

enum Item GetEmeraldChampionsBattleSetItem(struct Pokemon *mon, u8 choice)
{
    return GetEmeraldChampionsBattleSetItemForFormat(mon, choice, EC_BATTLE_FORMAT_DOUBLES);
}

enum Item GetEmeraldChampionsBattleSetRequiredItemForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const struct EmeraldChampionsBattleSet *preset = NULL;
    if (!ResolveVisibleChoice(mon, choice, format, &preset, NULL))
        return ITEM_NONE;
    return preset->requiredItem;
}

enum Item GetEmeraldChampionsBattleSetRequiredItem(struct Pokemon *mon, u8 choice)
{
    return GetEmeraldChampionsBattleSetRequiredItemForFormat(mon, choice, EC_BATTLE_FORMAT_DOUBLES);
}

static bool32 DoesMonMatchPresetMoves(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    for (u32 monSlot = 0; monSlot < MAX_MON_MOVES; monSlot++)
    {
        enum Move monMove = GetMonData(mon, MON_DATA_MOVE1 + monSlot);
        bool32 found = FALSE;

        for (u32 presetSlot = 0; presetSlot < MAX_MON_MOVES; presetSlot++)
        {
            if (monMove == preset->moves[presetSlot])
            {
                found = TRUE;
                break;
            }
        }
        if (!found)
            return FALSE;
    }
    return TRUE;
}

static bool32 DoesMonMatchPresetAbility(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Species setSpecies = ResolveBattleSetSpecies(species, EC_BATTLE_FORMAT_DOUBLES);
    enum Ability actualAbility = GetMonAbility(mon);
    u32 slot;

    if (actualAbility == preset->ability)
        return TRUE;

    // Mega sets store the transformed Ability. Before Mega Evolution, applying
    // one deliberately leaves the base species on the same legal fallback used
    // by ApplyPreset.
    if (!FindAbilitySlot(species, preset->ability, &slot))
    {
        if (!FindAbilitySlot(species, gEmeraldChampionsDefaultBattleSets[setSpecies].ability, &slot)
         && !FindFallbackAbilitySlot(species, &slot))
            return FALSE;
        return actualAbility == GetAbilityBySpecies(species, slot);
    }
    return FALSE;
}

static bool32 DoesMonMatchPreset(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    static const s32 statPointFields[NUM_STATS] =
    {
        MON_DATA_HP_EV,
        MON_DATA_ATK_EV,
        MON_DATA_DEF_EV,
        MON_DATA_SPATK_EV,
        MON_DATA_SPDEF_EV,
        MON_DATA_SPEED_EV,
    };
    enum Item item = GetMonData(mon, MON_DATA_HELD_ITEM);

    if (GetMonData(mon, MON_DATA_HIDDEN_NATURE) != preset->nature
     || !DoesMonMatchPresetAbility(mon, preset)
     || !DoesMonMatchPresetMoves(mon, preset))
        return FALSE;

    if (preset->requiredItem == ITEM_NONE)
    {
        if (item != preset->item)
            return FALSE;
    }
    else if (item != preset->item && item != preset->requiredItem)
    {
        return FALSE;
    }

    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        if (GetMonData(mon, statPointFields[stat]) != preset->statPoints[stat])
            return FALSE;
    }
    return TRUE;
}

s16 GetEmeraldChampionsCurrentBattleSetChoiceForFormat(struct Pokemon *mon, u8 format)
{
    u8 count = GetEmeraldChampionsBattleSetCountForFormat(mon, format);

    for (u8 choice = 0; choice < count; choice++)
    {
        const struct EmeraldChampionsBattleSet *preset = NULL;

        if (ResolveVisibleChoice(mon, choice, format, &preset, NULL) && DoesMonMatchPreset(mon, preset))
            return choice;
    }
    return -1;
}

s16 GetEmeraldChampionsCurrentBattleSetChoice(struct Pokemon *mon)
{
    return GetEmeraldChampionsCurrentBattleSetChoiceForFormat(mon, EC_BATTLE_FORMAT_DOUBLES);
}

static u8 ApplyPreset(
    struct Pokemon *mon,
    const struct EmeraldChampionsBattleSet *preset,
    bool32 preserveProtectedItem,
    bool32 supplyRequiredItem,
    bool32 requireMegaAccess,
    bool32 preserveProtectedItemInPlace)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Species setSpecies = ResolveBattleSetSpecies(species, EC_BATTLE_FORMAT_DOUBLES);
    enum Item currentItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    bool32 protectedItemHeld = IsEmeraldChampionsProtectedProgressionItem(currentItem)
                            && currentItem != preset->requiredItem;
    u32 abilitySlot;
    u8 ppBonuses = 0;
    u8 perfectIv = MAX_PER_STAT_IVS;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES || preset == NULL)
        return EC_BATTLE_SET_FAILED;
    if (!FindAbilitySlot(species, preset->ability, &abilitySlot))
    {
        // Mega presets name the transformed Ability. The base Pokémon keeps a
        // legal base Ability until the engine performs Mega Evolution.
        if (!FindAbilitySlot(species, gEmeraldChampionsDefaultBattleSets[setSpecies].ability, &abilitySlot)
         && !FindFallbackAbilitySlot(species, &abilitySlot))
            return EC_BATTLE_SET_FAILED;
    }
    if (IsEmeraldChampionsProtectedProgressionItem(preset->item))
        return EC_BATTLE_SET_FAILED;
    if (preset->requiredItem != ITEM_NONE
     && (gItemsInfo[preset->requiredItem].sortType != ITEM_TYPE_MEGA_STONE
      || (requireMegaAccess && !HasMegaAccess())))
        return EC_BATTLE_SET_FAILED;
    if (preserveProtectedItem && protectedItemHeld)
        return EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED;

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        if (preset->moves[i] >= MOVES_COUNT)
            return EC_BATTLE_SET_FAILED;
        for (u32 j = 0; preset->moves[i] != MOVE_NONE && j < i; j++)
        {
            if (preset->moves[i] == preset->moves[j])
                return EC_BATTLE_SET_FAILED;
        }
    }

    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, preset->moves[i], i);
    // Normalize move-driven forms even when the authored move already occupied
    // the same slot and SetMonMoveSlot therefore had no transition to observe.
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        TryFormChangeOnMove(mon, preset->moves[i], B_TRAINER_PLAYER);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &preset->nature);
    SetMonData(mon, MON_DATA_ABILITY_NUM, &abilitySlot);
    SetMonData(mon, MON_DATA_HP_EV, &preset->statPoints[0]);
    SetMonData(mon, MON_DATA_ATK_EV, &preset->statPoints[1]);
    SetMonData(mon, MON_DATA_DEF_EV, &preset->statPoints[2]);
    SetMonData(mon, MON_DATA_SPATK_EV, &preset->statPoints[3]);
    SetMonData(mon, MON_DATA_SPDEF_EV, &preset->statPoints[4]);
    SetMonData(mon, MON_DATA_SPEED_EV, &preset->statPoints[5]);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(mon, MON_DATA_HP_IV + stat, &perfectIv);
    if (!(preserveProtectedItemInPlace && protectedItemHeld))
    {
        if (supplyRequiredItem && preset->requiredItem != ITEM_NONE)
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->requiredItem);
        else if (preset->requiredItem == ITEM_NONE || currentItem != preset->requiredItem)
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->item);
    }
    CalculateMonStats(mon);

    if (preset->requiredItem == ITEM_NONE)
        return EC_BATTLE_SET_SUCCESS;
    return currentItem == preset->requiredItem
         ? EC_BATTLE_SET_MEGA_STONE_HELD
         : EC_BATTLE_SET_MEGA;
}

u8 ApplyEmeraldChampionsBattleSetChoiceForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const struct EmeraldChampionsBattleSet *preset = NULL;
    if (!ResolveVisibleChoice(mon, choice, format, &preset, NULL))
        return EC_BATTLE_SET_FAILED;
    return ApplyPreset(mon, preset, TRUE, FALSE, TRUE, FALSE);
}

u8 ApplyEmeraldChampionsBattleSetChoice(struct Pokemon *mon, u8 choice)
{
    return ApplyEmeraldChampionsBattleSetChoiceForFormat(
        mon, choice, EC_BATTLE_FORMAT_DOUBLES
    );
}

u8 ApplyEmeraldChampionsRecommendedEvolutionSet(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);

    // Evolution always returns the campaign to its doubles-first orientation.
    // Select the first ordinary role explicitly: a small number of legacy
    // species arrays place a Mega role in raw slot zero.
    for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
    {
        const struct EmeraldChampionsBattleSet *preset =
            GetEmeraldChampionsRawBattleSet(species, choice);

        if (preset != NULL && preset->requiredItem == ITEM_NONE)
            return ApplyPreset(mon, preset, FALSE, FALSE, FALSE, TRUE);
    }
    return EC_BATTLE_SET_FAILED;
}

u8 ApplyEmeraldChampionsRandomWildSet(struct Pokemon *mon)
{
    // Wild loadouts are independent of campaign inventory.  In particular,
    // acquiring the Mega Ring must never add required-stone tutor roles to a
    // species' wild pool or let a wild Pokemon carry a progression item.
    return ApplyEmeraldChampionsRandomNonMegaSet(mon);
}

u8 ApplyEmeraldChampionsRandomNonMegaSet(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct EmeraldChampionsBattleSet *selected = NULL;
    u32 matches = 0;

    for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
    {
        const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsRawBattleSet(species, choice);

        if (preset == NULL || preset->requiredItem != ITEM_NONE)
            continue;
        if (RandomUniform(RNG_NONE, 0, ++matches - 1) == 0)
            selected = preset;
    }
    if (selected == NULL)
        return EC_BATTLE_SET_FAILED;
    return ApplyPreset(mon, selected, FALSE, FALSE, FALSE, FALSE);
}

u8 ApplyEmeraldChampionsOpponentSet(struct Pokemon *mon, u8 rawChoice)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    return ApplyPreset(
        mon,
        GetEmeraldChampionsRawBattleSet(species, rawChoice),
        FALSE,
        TRUE,
        FALSE,
        FALSE
    );
}
