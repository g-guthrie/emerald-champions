#ifndef GUARD_EMERALD_CHAMPIONS_BATTLE_SETS_H
#define GUARD_EMERALD_CHAMPIONS_BATTLE_SETS_H

#include "global.h"
#include "constants/emerald_champions.h"

#define EMERALD_CHAMPIONS_SET_NAME_LENGTH 24

struct EmeraldChampionsBattleSet
{
    enum Move moves[MAX_MON_MOVES];
    enum Item item;
    enum Item requiredItem;
    enum Move requiredMove;
    u8 nature;
    enum Ability ability;
    u8 statPoints[NUM_STATS];
};

struct EmeraldChampionsBattleSetRange
{
    u16 offset;
    u8 count;
};

struct EmeraldChampionsBattleSetChoice
{
    u8 name[EMERALD_CHAMPIONS_SET_NAME_LENGTH];
    struct EmeraldChampionsBattleSet preset;
};

// Stat Points are shown and authored in display order (HP, Atk, Def, Sp. Atk,
// Sp. Def, Speed), which is not STAT_* order. This is the one table that maps
// display index -> STAT_* id; every editor, matcher and preset uses it so the
// six stats can never be paired differently in two places.
extern const u8 gEmeraldChampionsStatPointOrder[NUM_STATS];
#define EC_STAT_POINT_DATA(displayIndex) (MON_DATA_HP_EV + gEmeraldChampionsStatPointOrder[displayIndex])
#define EC_STAT_VALUE_DATA(displayIndex) (MON_DATA_MAX_HP + gEmeraldChampionsStatPointOrder[displayIndex])

extern const struct EmeraldChampionsBattleSet gEmeraldChampionsDefaultBattleSets[NUM_SPECIES];
extern const u8 *const gEmeraldChampionsDefaultBattleSetNames[NUM_SPECIES];
extern const struct EmeraldChampionsBattleSetRange gEmeraldChampionsBattleSetRanges[NUM_SPECIES];
extern const struct EmeraldChampionsBattleSetChoice gEmeraldChampionsBattleSetAlternatives[];
extern const struct EmeraldChampionsBattleSet gEmeraldChampionsSinglesDefaultBattleSets[NUM_SPECIES];
extern const u8 *const gEmeraldChampionsSinglesDefaultBattleSetNames[NUM_SPECIES];
extern const struct EmeraldChampionsBattleSetRange gEmeraldChampionsSinglesBattleSetRanges[NUM_SPECIES];
extern const struct EmeraldChampionsBattleSetChoice gEmeraldChampionsSinglesBattleSetAlternatives[];

u8 GetEmeraldChampionsBattleSetCount(struct Pokemon *mon);
u8 GetEmeraldChampionsBattleSetCountForFormat(struct Pokemon *mon, u8 format);
const u8 *GetEmeraldChampionsBattleSetName(struct Pokemon *mon, u8 choice);
const u8 *GetEmeraldChampionsBattleSetNameForFormat(struct Pokemon *mon, u8 choice, u8 format);
const struct EmeraldChampionsBattleSet *GetEmeraldChampionsBattleSetPresetForFormat(struct Pokemon *mon, u8 choice, u8 format);
enum Item GetEmeraldChampionsBattleSetItem(struct Pokemon *mon, u8 choice);
enum Item GetEmeraldChampionsBattleSetItemForFormat(struct Pokemon *mon, u8 choice, u8 format);
enum Item GetEmeraldChampionsBattleSetRequiredItem(struct Pokemon *mon, u8 choice);
enum Item GetEmeraldChampionsBattleSetRequiredItemForFormat(struct Pokemon *mon, u8 choice, u8 format);
s16 GetEmeraldChampionsCurrentBattleSetChoice(struct Pokemon *mon);
s16 GetEmeraldChampionsCurrentBattleSetChoiceForFormat(struct Pokemon *mon, u8 format);
u8 ApplyEmeraldChampionsBattleSetChoice(struct Pokemon *mon, u8 choice);
u8 ApplyEmeraldChampionsBattleSetChoiceForFormat(struct Pokemon *mon, u8 choice, u8 format);
u8 ApplyEmeraldChampionsRecommendedEvolutionSet(struct Pokemon *mon);
u8 ApplyEmeraldChampionsRandomWildSet(struct Pokemon *mon);
u8 ApplyEmeraldChampionsRandomNonMegaSet(struct Pokemon *mon);
u8 GetEmeraldChampionsRawBattleSetCount(enum Species species);
const struct EmeraldChampionsBattleSet *GetEmeraldChampionsRawBattleSet(enum Species species, u8 rawChoice);
u8 ApplyEmeraldChampionsOpponentSet(struct Pokemon *mon, u8 rawChoice);
bool32 IsEmeraldChampionsProtectedProgressionItem(enum Item item);
bool32 TryNormalizeEmeraldChampionsBellyDrumHpParity(struct Pokemon *mon);
bool32 IsEmeraldChampionsOrdinaryWildSpecies(enum Species species);

#endif // GUARD_EMERALD_CHAMPIONS_BATTLE_SETS_H
