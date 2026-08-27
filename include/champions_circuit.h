#ifndef GUARD_CHAMPIONS_CIRCUIT_H
#define GUARD_CHAMPIONS_CIRCUIT_H

#include "global.h"

bool8 IsChampionsCircuitBattle(void);
void ChampionsCircuitCanEnter(void);
void ChampionsCircuitBegin(void);
void ChampionsCircuitGenerateOpponent(void);
void ChampionsCircuitHandleBattleResult(void);
void ChampionsCircuitTryGiveReward(void);
void ChampionsCircuitEnd(void);

#endif // GUARD_CHAMPIONS_CIRCUIT_H
