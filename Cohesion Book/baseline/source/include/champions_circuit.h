#ifndef GUARD_CHAMPIONS_CIRCUIT_H
#define GUARD_CHAMPIONS_CIRCUIT_H

#include "global.h"

struct Pokemon;
#define CHAMPIONS_CIRCUIT_BASE_LEVEL 100
// Transient opponent levels fit both party and battle-controller byte fields.
#define CHAMPIONS_CIRCUIT_MAX_LEVEL 255

bool32 IsChampionsCircuitOpponent(const struct Pokemon *mon);
u8 GetChampionsCircuitOpponentLevel(u16 wins, u32 slot);
bool32 IsChampionsCircuitBattle(void);
void ChampionsCircuitCanEnter(void);
void ChampionsCircuitBegin(void);
void ChampionsCircuitGenerateOpponent(void);
void ChampionsCircuitHandleBattleResult(void);
void ChampionsCircuitTryGiveReward(void);
void ChampionsCircuitEnd(void);

#endif // GUARD_CHAMPIONS_CIRCUIT_H
