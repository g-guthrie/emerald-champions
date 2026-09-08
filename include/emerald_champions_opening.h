#ifndef GUARD_EMERALD_CHAMPIONS_OPENING_H
#define GUARD_EMERALD_CHAMPIONS_OPENING_H

#include "global.h"

bool32 GiveEmeraldChampionsStarterPair(u16 first, u16 second);
bool32 HasEmeraldChampionsSecondStarter(void);
u16 GetEmeraldChampionsSecondStarterIndex(void);
u16 GetEmeraldChampionsRivalStarterIndex(void);
bool32 IsEmeraldChampionsBirchRescueBattle(void);
void CreateEmeraldChampionsBirchRescueParty(void);
void ApplyEmeraldChampionsRegionalRivalSet(struct Pokemon *party, u32 slot, bool32 opening);

void StartEmeraldChampionsBirchRescue(void);
void BufferEmeraldChampionsRivalBranch(void);
void GiveEmeraldChampionsOpeningBalls(void);
void TopUpEmeraldChampionsOpeningBalls(void);
void BufferEmeraldChampionsStarterNames(void);
void SelectEmeraldChampionsStarterForNaming(void);

#endif
