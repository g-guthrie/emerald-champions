#include "global.h"
#include "move.h"
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
#include "data/pokemon/emerald_champions_preparation_learnsets.h"

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
        && !gSpeciesInfo[species].isGigantamax;
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

static bool32 HasDirectBattleSet(enum Species species, u8 format)
{
    return species > SPECIES_NONE
        && species < NUM_SPECIES
        && IsValidBattleFormat(format)
        && gEmeraldChampionsBattleSetRanges[format][species].count != 0;
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

static const struct EmeraldChampionsBattleSetRange *ResolveBattleSetRange(enum Species species, u8 format)
{
    if (!IsValidBattleFormat(format))
        return NULL;
    species = ResolveBattleSetSpecies(species, format);
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return NULL;
    return &gEmeraldChampionsBattleSetRanges[format][species];
}

u8 GetEmeraldChampionsRawBattleSetCountForFormat(enum Species species, u8 format)
{
    const struct EmeraldChampionsBattleSetRange *range = ResolveBattleSetRange(species, format);
    return range == NULL ? 0 : range->count;
}

const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSetForFormat(
    enum Species species,
    u8 rawChoice,
    u8 format)
{
    const struct EmeraldChampionsBattleSetRange *range = ResolveBattleSetRange(species, format);
    if (range == NULL || rawChoice >= range->count)
        return NULL;
    return &gEmeraldChampionsBattleSets[range->offset + rawChoice].preset;
}

u8 GetEmeraldChampionsRawBattleSetCount(enum Species species)
{
    return GetEmeraldChampionsRawBattleSetCountForFormat(species, EC_BATTLE_FORMAT_DOUBLES);
}

const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSet(enum Species species, u8 rawChoice)
{
    return GetEmeraldChampionsRawBattleSetForFormat(species, rawChoice, EC_BATTLE_FORMAT_DOUBLES);
}

static bool32 IsVisiblePreset(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    return (!PresetRequiresTransformation(preset) || HasTransformationAccess(mon, preset))
        && !PresetRequiresOwnedHeldItem(mon, preset);
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
    const struct EmeraldChampionsBattleSetRange *range = ResolveBattleSetRange(species, format);
    u8 visibleChoice = 0;

    if (range == NULL)
        return 0;
    for (u8 rawChoice = 0; rawChoice < range->count; rawChoice++)
    {
        const struct EmeraldChampionsBattleSetChoice *entry = &gEmeraldChampionsBattleSets[range->offset + rawChoice];
        const struct EmeraldChampionsBattleSet *preset = &entry->preset;

        if (!IsVisiblePreset(mon, preset))
            continue;
        if (visibleChoice++ == choice && (presetOut != NULL || nameOut != NULL))
        {
            if (presetOut != NULL)
                *presetOut = preset;
            if (nameOut != NULL)
                *nameOut = entry->name;
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
    const struct EmeraldChampionsBattleSet *fallback = GetEmeraldChampionsRawBattleSet(species, 0);
    return FindAbilitySlot(species, fallback == NULL ? ABILITY_NONE : fallback->ability, slot)
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

        if (diff != 0)
            return FALSE;
    }
    return TRUE;
}

s16 GetEmeraldChampionsCurrentBattleSetChoiceForFormat(struct Pokemon *mon, u8 format)
{
    const struct EmeraldChampionsBattleSetRange *range = ResolveBattleSetRange(GetMonData(mon, MON_DATA_SPECIES), format);
    u8 choice = 0;
    if (range == NULL)
        return -1;
    for (u8 raw = 0; raw < range->count; raw++)
    {
        const struct EmeraldChampionsBattleSet *preset = &gEmeraldChampionsBattleSets[range->offset + raw].preset;
        if (!IsVisiblePreset(mon, preset))
            continue;
        if (DoesMonMatchPreset(mon, preset))
            return choice;
        choice++;
    }
    return -1;
}

s16 GetEmeraldChampionsCurrentBattleSetChoice(struct Pokemon *mon)
{
    return GetEmeraldChampionsCurrentBattleSetChoiceForFormat(mon, EC_BATTLE_FORMAT_DOUBLES);
}

enum PresetApplication
{
    PRESET_TUTOR,
    PRESET_EVOLUTION,
    PRESET_WILD,
    PRESET_SCRIPTED,
};

static u8 ApplyPreset(
    struct Pokemon *mon,
    const struct EmeraldChampionsBattleSet *preset,
    enum PresetApplication application)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Item currentItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    bool32 protectedItemHeld;
    u32 abilitySlot;
    u8 perfectIv = MAX_PER_STAT_IVS;

    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES || preset == NULL)
        return EC_BATTLE_SET_FAILED;
    protectedItemHeld = IsEmeraldChampionsProtectedProgressionItem(currentItem)
                     && currentItem != preset->requiredItem
                     && currentItem != preset->item;
    if (!FindPresetAbilitySlot(species, preset, &abilitySlot))
        return EC_BATTLE_SET_FAILED;
    if (IsEmeraldChampionsProtectedProgressionItem(preset->item)
     && application != PRESET_SCRIPTED
     && currentItem != preset->item)
        return EC_BATTLE_SET_FAILED;
    if (preset->requiredItem != ITEM_NONE
     && !IsBattleSetTransformationItem(preset->requiredItem))
        return EC_BATTLE_SET_FAILED;
    if (PresetRequiresTransformation(preset)
     && application == PRESET_TUTOR
     && !HasTransformationAccess(mon, preset))
        return EC_BATTLE_SET_FAILED;
    if (application == PRESET_TUTOR && protectedItemHeld)
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
    if (application != PRESET_EVOLUTION)
        for (u32 stat = 0; stat < NUM_STATS; stat++)
            SetMonData(mon, MON_DATA_HP_IV + stat, &perfectIv);
    if (!(application == PRESET_EVOLUTION && protectedItemHeld))
    {
        if (application == PRESET_SCRIPTED && preset->requiredItem != ITEM_NONE)
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->requiredItem);
        else if (preset->requiredItem == ITEM_NONE || currentItem != preset->requiredItem)
            SetMonData(mon, MON_DATA_HELD_ITEM, &preset->item);
    }
    CalculateMonStats(mon);
    if (application == PRESET_TUTOR || application == PRESET_EVOLUTION)
        ClampMonToPlayerLevelCap(mon);

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
    return ApplyPreset(mon, preset, PRESET_TUTOR);
}

u8 ApplyEmeraldChampionsBattleSetChoice(struct Pokemon *mon, u8 choice)
{
    return ApplyEmeraldChampionsBattleSetChoiceForFormat(
        mon, choice, EC_BATTLE_FORMAT_DOUBLES
    );
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
    const struct EmeraldChampionsBattleSetRange *range = ResolveBattleSetRange(GetMonData(mon, MON_DATA_SPECIES), EC_BATTLE_FORMAT_DOUBLES);
    if (range == NULL)
        return EC_BATTLE_SET_FAILED;
    const struct EmeraldChampionsBattleSet *selected = NULL;
    u32 matches = 0;

    for (u8 choice = 0; choice < range->count; choice++)
    {
        const struct EmeraldChampionsBattleSet *preset = &gEmeraldChampionsBattleSets[range->offset + choice].preset;

        if (PresetRequiresTransformation(preset)
         || IsEmeraldChampionsProtectedProgressionItem(preset->item))
            continue;
        if (RandomUniform(RNG_NONE, 0, ++matches - 1) == 0)
            selected = preset;
    }
    if (selected == NULL)
        return EC_BATTLE_SET_FAILED;
    return ApplyPreset(mon, selected, PRESET_WILD);
}

u8 ApplyEmeraldChampionsScriptedSet(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset)
{
    return ApplyPreset(mon, preset, PRESET_SCRIPTED);
}

u8 ApplyEmeraldChampionsOpponentSet(struct Pokemon *mon, u8 rawChoice)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    return ApplyPreset(
        mon,
        GetEmeraldChampionsRawBattleSet(species, rawChoice),
        PRESET_SCRIPTED
    );
}

// Membership is generated from supplied presets; protection remains live item policy.
bool32 IsEmeraldChampionsFreePresetItem(enum Item item)
{
    return item < ITEMS_COUNT && sEmeraldChampionsPresetItems[item]
        && !IsEmeraldChampionsProtectedProgressionItem(item);
}

// Shared preparation access and form-retention policy. UI consumers query
// these owners; they do not assemble their own legality tables.
const u16 *GetEmeraldChampionsPreparationMoves(enum Species species)
{
    const u16 *moves;
    enum Species baseSpecies;

    if (species <= SPECIES_NONE || species >= NUM_SPECIES)
        return sEmeraldChampionsPreparationMoves_None;

    // This table is intentionally separate from SpeciesInfo.teachableLearnset:
    // only the Center's All Legal Moves service receives historical legality.
    moves = sEmeraldChampionsPreparationLearnsets[species];
    if (moves != NULL)
        return moves;

    baseSpecies = GET_BASE_SPECIES_ID(species);
    if (baseSpecies > SPECIES_NONE && baseSpecies < NUM_SPECIES)
    {
        moves = sEmeraldChampionsPreparationLearnsets[baseSpecies];
        if (moves != NULL)
            return moves;
    }

    return sEmeraldChampionsPreparationMoves_None;
}

static void BuildEmeraldChampionsPreparationMoveAccess(enum Species species, bool8 *availableMoves)
{
    const u16 *preparationMoves = GetEmeraldChampionsPreparationMoves(species);

    for (u32 i = 0; preparationMoves[i] != MOVE_UNAVAILABLE; i++)
    {
        enum Move move = preparationMoves[i];

        if (move > MOVE_NONE && move < MOVES_COUNT_ALL)
            availableMoves[move] = TRUE;
    }

    // Presets grant individual move access in both formats, including item-gated
    // roles. Reuse their species/form resolver without granting any held items.
    // Smeargle gets its preset moves here; everything else still needs Sketch.
    for (u8 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++)
    {
        const struct EmeraldChampionsBattleSetRange *range = ResolveBattleSetRange(species, format);
        if (range == NULL)
            continue;
        for (u32 choice = 0; choice < range->count; choice++)
        {
            const struct EmeraldChampionsBattleSet *preset =
                &gEmeraldChampionsBattleSets[range->offset + choice].preset;
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                enum Move move = preset->moves[slot];
                if (move > MOVE_NONE && move < MOVES_COUNT_ALL)
                    availableMoves[move] = TRUE;
            }
        }
    }

}

bool32 CanSpeciesUseEmeraldChampionsPreparationMove(enum Species species, enum Move move)
{
    bool8 availableMoves[MOVES_COUNT_ALL] = {FALSE};
    if (move <= MOVE_NONE || move >= MOVES_COUNT_ALL)
        return FALSE;
    BuildEmeraldChampionsPreparationMoveAccess(species, availableMoves);
    return availableMoves[move];
}

u32 GetEmeraldChampionsPreparationMovesToLearn(struct BoxPokemon *mon, u16 *moves)
{
    bool8 availableMoves[MOVES_COUNT_ALL] = {FALSE};
    u32 numMoves = 0;
    BuildEmeraldChampionsPreparationMoveAccess(GetBoxMonData(mon, MON_DATA_SPECIES), availableMoves);

    for (u32 move = MOVE_NONE + 1; move < MOVES_COUNT_ALL; move++)
    {
        if (availableMoves[move] && !BoxMonKnowsMove(mon, move))
        {
            if (moves != NULL)
                moves[numMoves] = move;
            numMoves++;
        }
    }

    return numMoves;
}

bool32 CanSpeciesKeepEmeraldChampionsUnfusionMove(enum Species species, enum Move move)
{
    if (CanSpeciesUseEmeraldChampionsPreparationMove(species, move))
        return TRUE;
    const struct LevelUpMove *levelMoves = GetSpeciesLevelUpLearnset(species);
    for (u32 i = 0; levelMoves[i].move != LEVEL_UP_MOVE_END; i++)
        if (levelMoves[i].move == move)
            return TRUE;
    const u16 *moves = GetSpeciesTeachableLearnset(species);
    for (u32 i = 0; moves[i] != MOVE_UNAVAILABLE; i++)
        if (moves[i] == move)
            return TRUE;
    moves = GetSpeciesEggMoves(species);
    for (u32 i = 0; moves[i] != MOVE_UNAVAILABLE; i++)
        if (moves[i] == move)
            return TRUE;
    return FALSE;
}
