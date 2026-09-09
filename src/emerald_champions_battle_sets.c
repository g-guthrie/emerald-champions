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

static bool32 PresetRequiresTransformation(const struct EmeraldChampionsBattleSet *preset)
{
    return preset->requiredItem != ITEM_NONE || preset->requiredMove != MOVE_NONE;
}

static bool32 PresetRequiresOwnedHeldItem(
    struct Pokemon *mon,
    const struct EmeraldChampionsBattleSet *preset)
{
    return IsEmeraldChampionsProtectedProgressionItem(preset->item)
        && GetMonData(mon, MON_DATA_HELD_ITEM) != preset->item;
}
static const enum Item sEmeraldChampionsEvolutionItems[] =
{
#include "data/emerald_champions_evolution_items.h"
};

static bool32 HasMegaAccess(void)
{
    return CheckBagHasItem(ITEM_MEGA_RING, 1);
}

// Transformation presets name the form's Ability and required item. Mega
// Evolution needs a stone and Ring; Primal, Crowned and Ogerpon mask forms
// use their own held item. Item classification does not waive ownership.
static bool32 IsBattleSetTransformationItem(enum Item item)
{
    if (item == ITEM_NONE)
        return FALSE;
    if (gItemsInfo[item].sortType == ITEM_TYPE_MEGA_STONE
     || gItemsInfo[item].holdEffect == HOLD_EFFECT_OGERPON_MASK)
        return TRUE;
    return item == ITEM_RED_ORB
        || item == ITEM_BLUE_ORB
        || item == ITEM_RUSTED_SWORD
        || item == ITEM_RUSTED_SHIELD;
}

static bool32 HasTransformationAccess(
    struct Pokemon *mon,
    const struct EmeraldChampionsBattleSet *preset)
{
    // Move-driven Mega Evolution (Rayquaza) still answers to the Ring.
    if (preset->requiredItem == ITEM_NONE
     || gItemsInfo[preset->requiredItem].sortType == ITEM_TYPE_MEGA_STONE)
        return HasMegaAccess();
    return CheckBagHasItem(preset->requiredItem, 1)
        || GetMonData(mon, MON_DATA_HELD_ITEM) == preset->requiredItem;
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

u8 GetEmeraldChampionsRawBattleSetCountForFormat(enum Species species, u8 format)
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

const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSetForFormat(
    enum Species species,
    u8 rawChoice,
    u8 format)
{
    const struct EmeraldChampionsBattleSetRange *range;
    const struct EmeraldChampionsBattleSet *defaults;
    const struct EmeraldChampionsBattleSetChoice *alternatives;

    if (GetEmeraldChampionsRawBattleSetCountForFormat(species, format) == 0)
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
    return GetEmeraldChampionsRawBattleSetCountForFormat(species, EC_BATTLE_FORMAT_DOUBLES);
}

const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSet(enum Species species, u8 rawChoice)
{
    return GetEmeraldChampionsRawBattleSetForFormat(species, rawChoice, EC_BATTLE_FORMAT_DOUBLES);
}

// With no outputs, count every visible set; otherwise stop at the requested choice.
static u8 ScanVisibleBattleSets(
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

    if (GetEmeraldChampionsRawBattleSetCountForFormat(species, format) == 0)
        return 0;

    setSpecies = ResolveBattleSetSpecies(species, format);
    defaults = GetDefaultSetTable(format);
    defaultNames = GetDefaultSetNameTable(format);
    alternatives = GetSetAlternativeTable(format);
    range = &GetSetRangeTable(format)[setSpecies];
    for (u8 rawChoice = 0; rawChoice <= range->count; rawChoice++)
    {
        const struct EmeraldChampionsBattleSet *preset = rawChoice == 0
            ? &defaults[setSpecies]
            : &alternatives[range->offset + rawChoice - 1].preset;

        if ((PresetRequiresTransformation(preset) && !HasTransformationAccess(mon, preset))
         || PresetRequiresOwnedHeldItem(mon, preset))
            continue;
        if (visibleChoice++ == choice && (presetOut != NULL || nameOut != NULL))
        {
            if (presetOut != NULL)
                *presetOut = preset;
            if (nameOut != NULL)
            {
                *nameOut = rawChoice == 0
                    ? defaultNames[setSpecies]
                    : alternatives[range->offset + rawChoice - 1].name;
                if (rawChoice == 0 && *nameOut == NULL)
                    *nameOut = sRecommendedSetName;
            }
            return visibleChoice;
        }
    }
    return visibleChoice;
}

static bool32 ResolveVisibleChoice(
    struct Pokemon *mon,
    u8 choice,
    u8 format,
    const struct EmeraldChampionsBattleSet **presetOut,
    const u8 **nameOut)
{
    return ScanVisibleBattleSets(mon, choice, format, presetOut, nameOut) > choice;
}

u8 GetEmeraldChampionsBattleSetCountForFormat(struct Pokemon *mon, u8 format)
{
    return ScanVisibleBattleSets(mon, 0, format, NULL, NULL);
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

const struct EmeraldChampionsBattleSet *GetEmeraldChampionsBattleSetPresetForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const struct EmeraldChampionsBattleSet *preset = NULL;
    if (!ResolveVisibleChoice(mon, choice, format, &preset, NULL))
        return NULL;
    return preset;
}

enum Item GetEmeraldChampionsBattleSetItemForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsBattleSetPresetForFormat(mon, choice, format);
    if (preset == NULL)
        return ITEM_NONE;
    return preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
}

enum Item GetEmeraldChampionsBattleSetRequiredItemForFormat(struct Pokemon *mon, u8 choice, u8 format)
{
    const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsBattleSetPresetForFormat(mon, choice, format);
    if (preset == NULL)
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

// Mega presets name the transformed Ability; application and recognition
// share the same legal base Ability, including the doubles-default fallback.
static bool32 FindPresetAbilitySlot(enum Species species, const struct EmeraldChampionsBattleSet *preset, u32 *slot)
{
    if (FindAbilitySlot(species, preset->ability, slot))
        return TRUE;
    enum Species setSpecies = ResolveBattleSetSpecies(species, EC_BATTLE_FORMAT_DOUBLES);
    return FindAbilitySlot(species, gEmeraldChampionsDefaultBattleSets[setSpecies].ability, slot)
        || FindFallbackAbilitySlot(species, slot);
}

static bool32 DoesMonMatchPresetAbility(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Ability actualAbility = GetMonAbility(mon);
    u32 slot;

    if (actualAbility == preset->ability)
        return TRUE;
    return !FindAbilitySlot(species, preset->ability, &slot)
        && FindPresetAbilitySlot(species, preset, &slot)
        && actualAbility == GetAbilityBySpecies(species, slot);
}

// Other half-HP items still need even HP after Belly Drum. Sitrus rounds
// its activation threshold up and never needs EV adjustments.
static bool32 DoesItemNeedEvenHp(enum Item item, enum Ability ability)
{
    enum HoldEffect holdEffect = GetItemHoldEffect(item);

    if (item == ITEM_SITRUS_BERRY)
        return FALSE;
    if (holdEffect == HOLD_EFFECT_RESTORE_HP)
        return TRUE;
    // The confusion-flavor berries fire at a quarter of max HP, which Belly
    // Drum never reaches on its own. Gluttony moves that trigger to half,
    // which is exactly where Belly Drum leaves the user.
    return holdEffect == HOLD_EFFECT_CONFUSE_FLAVOR && ability == ABILITY_GLUTTONY;
}

static bool32 DoesPresetWantEvenHp(const struct EmeraldChampionsBattleSet *preset)
{
    if (!DoesItemNeedEvenHp(preset->item, preset->ability))
        return FALSE;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        if (preset->moves[i] == MOVE_BELLY_DRUM)
            return TRUE;
    }
    return FALSE;
}

static bool32 DoesMonWantEvenHp(struct Pokemon *mon)
{
    if (!DoesItemNeedEvenHp(GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon)))
        return FALSE;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        if (GetMonData(mon, MON_DATA_MOVE1 + i) == MOVE_BELLY_DRUM)
            return TRUE;
    }
    return FALSE;
}

// At the opening cap, up to 24 EVs may be needed to change HP by one.
// Keep this bound stable across levels to prevent preset recognition drift.
#define BELLY_DRUM_HP_EV_TOLERANCE 24

bool32 TryNormalizeEmeraldChampionsBellyDrumHpParity(struct Pokemon *mon)
{
    u32 hpPoints;
    u32 anchorHpPoints;
    s32 choice;
    u32 total = 0;
    u32 currentHp;
    u32 oldMaxHp;
    u8 value;

    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE
     || GetMonData(mon, MON_DATA_IS_EGG)
     || !DoesMonWantEvenHp(mon))
        return FALSE;
    oldMaxHp = GetMonData(mon, MON_DATA_MAX_HP);
    if ((oldMaxHp & 1) == 0)
        return FALSE;

    hpPoints = GetMonData(mon, MON_DATA_HP_EV);
    anchorHpPoints = hpPoints;
    choice = GetEmeraldChampionsCurrentBattleSetChoice(mon);
    if (choice >= 0)
        anchorHpPoints = GetEmeraldChampionsBattleSetPresetForFormat(
            mon, choice, EC_BATTLE_FORMAT_DOUBLES)->evs[STAT_HP];
    else
    {
        choice = GetEmeraldChampionsCurrentBattleSetChoiceForFormat(mon, EC_BATTLE_FORMAT_SINGLES);
        if (choice >= 0)
            anchorHpPoints = GetEmeraldChampionsBattleSetPresetForFormat(
                mon, choice, EC_BATTLE_FORMAT_SINGLES)->evs[STAT_HP];
    }
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        total += GetMonData(mon, MON_DATA_HP_EV + stat);
    currentHp = GetMonData(mon, MON_DATA_HP);

    // Anchor recognized sets to their authored investment. Using the current
    // investment lets successive Leveler visits accumulate deviations beyond
    // the recognition tolerance. Custom spreads retain their current anchor.
    for (u32 i = 0; i <= 2 * BELLY_DRUM_HP_EV_TOLERANCE / 4; i++)
    {
        s32 offset = 4 * ((i + 1) / 2);
        s32 target = (s32)anchorHpPoints + (i & 1 ? offset : -offset);
        s32 nudge = target - (s32)hpPoints;

        if (target < 0 || target > MAX_PER_STAT_EVS || (s32)total + nudge > MAX_TOTAL_EVS)
            continue;
        value = target;
        SetMonData(mon, MON_DATA_HP_EV, &value);
        CalculateMonStats(mon);
        if ((GetMonData(mon, MON_DATA_MAX_HP) & 1) == 0)
        {
            u32 newMaxHp = GetMonData(mon, MON_DATA_MAX_HP);
            u32 newHp = currentHp == oldMaxHp ? newMaxHp : min(currentHp, newMaxHp);

            SetMonData(mon, MON_DATA_HP, &newHp);
            return TRUE;
        }
    }

    value = hpPoints;
    SetMonData(mon, MON_DATA_HP_EV, &value);
    CalculateMonStats(mon);
    SetMonData(mon, MON_DATA_HP, &currentHp);
    return FALSE;
}

const u8 gEmeraldChampionsEvOrder[NUM_STATS] =
{
    STAT_HP,
    STAT_ATK,
    STAT_DEF,
    STAT_SPATK,
    STAT_SPDEF,
    STAT_SPEED,
};

static bool32 DoesMonMatchPreset(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
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
        s32 diff = (s32)GetMonData(mon, EC_EV_DATA(stat)) - preset->evs[stat];

        // Use the same bounded adjustment as parity normalization, so repeated
        // Leveler visits remain anchored to the authored spread.
        if (stat == 0 && diff >= -BELLY_DRUM_HP_EV_TOLERANCE
         && diff <= BELLY_DRUM_HP_EV_TOLERANCE && DoesPresetWantEvenHp(preset))
            continue;
        if (diff != 0)
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
    enum Item currentItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    bool32 protectedItemHeld;
    u32 abilitySlot;
    u8 ppBonuses = 0;
    u8 perfectIv = MAX_PER_STAT_IVS;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES || preset == NULL)
        return EC_BATTLE_SET_FAILED;
    protectedItemHeld = IsEmeraldChampionsProtectedProgressionItem(currentItem)
                     && currentItem != preset->requiredItem
                     && currentItem != preset->item;
    if (!FindPresetAbilitySlot(species, preset, &abilitySlot))
        return EC_BATTLE_SET_FAILED;
    if (IsEmeraldChampionsProtectedProgressionItem(preset->item)
     && !supplyRequiredItem
     && currentItem != preset->item)
        return EC_BATTLE_SET_FAILED;
    if (preset->requiredItem != ITEM_NONE
     && !IsBattleSetTransformationItem(preset->requiredItem))
        return EC_BATTLE_SET_FAILED;
    if (PresetRequiresTransformation(preset)
     && requireMegaAccess
     && !HasTransformationAccess(mon, preset))
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
    if (preset->requiredMove != MOVE_NONE)
    {
        // The authored requirement must be one of the moves about to be
        // applied. Check the preset directly because the current mon has not
        // received those moves yet.
        bool32 foundRequiredMove = FALSE;
        for (u32 i = 0; i < MAX_MON_MOVES; i++)
            foundRequiredMove |= preset->moves[i] == preset->requiredMove;
        if (!foundRequiredMove)
            return EC_BATTLE_SET_FAILED;
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
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(mon, EC_EV_DATA(stat), &preset->evs[stat]);
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
    TryNormalizeEmeraldChampionsBellyDrumHpParity(mon);

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

        if (preset != NULL
         && !PresetRequiresTransformation(preset)
         && !IsEmeraldChampionsProtectedProgressionItem(preset->item))
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

        if (preset == NULL
         || PresetRequiresTransformation(preset)
         || IsEmeraldChampionsProtectedProgressionItem(preset->item))
            continue;
        if (RandomUniform(RNG_NONE, 0, ++matches - 1) == 0)
            selected = preset;
    }
    if (selected == NULL)
        return EC_BATTLE_SET_FAILED;
    return ApplyPreset(mon, selected, FALSE, FALSE, FALSE, FALSE);
}

u8 ApplyEmeraldChampionsScriptedSet(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    return ApplyPreset(mon, preset, FALSE, TRUE, FALSE, FALSE);
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
