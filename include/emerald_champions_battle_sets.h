#ifndef GUARD_EMERALD_CHAMPIONS_BATTLE_SETS_H
#define GUARD_EMERALD_CHAMPIONS_BATTLE_SETS_H

#include "global.h"
#include "constants/emerald_champions.h"

struct EmeraldChampionsBattleSet
{
    enum Move moves[MAX_MON_MOVES];
    enum Item item;
    enum Item requiredItem;
    enum Move requiredMove;
    u8 nature;
    enum Ability ability;
    u8 evs[NUM_STATS];
};

struct EmeraldChampionsBattleSetRange
{
    u16 offset;
    u8 count;
};

struct EmeraldChampionsBattleSetChoice
{
    const u8 *name;
    struct EmeraldChampionsBattleSet preset;
};

// EVs are shown and authored in display order (HP, Atk, Def, Sp. Atk,
// Sp. Def, Speed), which is not STAT_* order. This is the one table that maps
// display index -> STAT_* id; every editor, matcher and preset uses it so the
// six stats can never be paired differently in two places.
extern const u8 gEmeraldChampionsEvOrder[NUM_STATS];
#define EC_EV_DATA(displayIndex) (MON_DATA_HP_EV + gEmeraldChampionsEvOrder[displayIndex])
#define EC_IV_DATA(displayIndex) (MON_DATA_HP_IV + gEmeraldChampionsEvOrder[displayIndex])
#define EC_STAT_VALUE_DATA(displayIndex) (MON_DATA_MAX_HP + gEmeraldChampionsEvOrder[displayIndex])

extern const struct EmeraldChampionsBattleSetChoice gEmeraldChampionsBattleSets[];
extern const struct EmeraldChampionsBattleSetRange gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_COUNT][NUM_SPECIES];

u8 GetEmeraldChampionsBattleSetCount(struct Pokemon *mon);
u8 GetEmeraldChampionsBattleSetCountForFormat(struct Pokemon *mon, u8 format);
const u8 *GetEmeraldChampionsBattleSetName(struct Pokemon *mon, u8 choice);
const u8 *GetEmeraldChampionsBattleSetNameForFormat(struct Pokemon *mon, u8 choice, u8 format);
const struct EmeraldChampionsBattleSet *GetEmeraldChampionsBattleSetPresetForFormat(struct Pokemon *mon, u8 choice, u8 format);
enum Item GetEmeraldChampionsBattleSetItemForFormat(struct Pokemon *mon, u8 choice, u8 format);
enum Item GetEmeraldChampionsBattleSetRequiredItem(struct Pokemon *mon, u8 choice);
enum Item GetEmeraldChampionsBattleSetRequiredItemForFormat(struct Pokemon *mon, u8 choice, u8 format);
s16 GetEmeraldChampionsCurrentBattleSetChoice(struct Pokemon *mon);
s16 GetEmeraldChampionsCurrentBattleSetChoiceForFormat(struct Pokemon *mon, u8 format);
u8 ApplyEmeraldChampionsBattleSetChoice(struct Pokemon *mon, u8 choice);
u8 ApplyEmeraldChampionsBattleSetChoiceForFormat(struct Pokemon *mon, u8 choice, u8 format);
u8 ApplyEmeraldChampionsRandomWildSet(struct Pokemon *mon);
u8 ApplyEmeraldChampionsRandomNonMegaSet(struct Pokemon *mon);
u8 GetEmeraldChampionsRawBattleSetCount(enum Species species);
const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSet(enum Species species, u8 rawChoice);
u8 GetEmeraldChampionsRawBattleSetCountForFormat(enum Species species, u8 format);
const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSetForFormat(enum Species species, u8 rawChoice, u8 format);
u8 ApplyEmeraldChampionsOpponentSet(struct Pokemon *mon, u8 rawChoice);
u8 ApplyEmeraldChampionsScriptedSet(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *preset);
bool32 IsEmeraldChampionsProtectedProgressionItem(enum Item item);
bool32 IsEmeraldChampionsFreePresetItem(enum Item item);
bool32 IsEmeraldChampionsOrdinaryWildSpecies(enum Species species);

// Preparation move policy is shared by the tutor and form-change consumers.
const u16 *GetEmeraldChampionsPreparationMoves(enum Species species);
// Existing preparation pool plus both preset formats, excluding known moves.
// Pass NULL to count without writing a list.
u32 GetEmeraldChampionsPreparationMovesToLearn(struct BoxPokemon *mon, u16 *moves);
bool32 CanSpeciesUseEmeraldChampionsPreparationMove(enum Species species, enum Move move);
u32 GetEmeraldChampionsIconicMovesToLearn(struct BoxPokemon *mon, u16 *moves);
bool32 CanSpeciesKeepEmeraldChampionsUnfusionMove(enum Species species, enum Move move);

#endif // GUARD_EMERALD_CHAMPIONS_BATTLE_SETS_H
