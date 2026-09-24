#ifndef GUARD_CHAMPIONS_CIRCUIT_H
#define GUARD_CHAMPIONS_CIRCUIT_H

#include "global.h"

struct Pokemon;
#define CHAMPIONS_CIRCUIT_BASE_LEVEL 100
// Transient opponent levels fit both party and battle-controller byte fields.
#define CHAMPIONS_CIRCUIT_MAX_LEVEL 255

// ChampionsCircuitCanEnter results. The desk script in
// data/maps/BattleFrontier_BattleTowerLobby/scripts.inc uses these numbers.
#define CIRCUIT_ENTRY_NEEDS_SIX  0 // Fewer than six, an Egg, or a fainted member.
#define CIRCUIT_ENTRY_OK         1
#define CIRCUIT_ENTRY_PARTY_RULE 2 // More than one Legendary, Ultra Beast or Paradox.

bool32 IsChampionsCircuitOpponent(const struct Pokemon *mon);
u8 GetChampionsCircuitOpponentLevel(u16 wins, u32 slot);
bool32 IsChampionsCircuitBattle(void);
void ChampionsCircuitCanEnter(void);
void ChampionsCircuitBegin(void);
void ChampionsCircuitGenerateOpponent(void);
void ChampionsCircuitHandleBattleResult(void);
void ChampionsCircuitTryGiveReward(void);
void ChampionsCircuitEnd(void);

bool32 CreateChampionsExhibitionParty(u8 level);
bool32 IsChampionsTentBattle(void);
void ChampionsTentCanEnter(void);
void ChampionsTentBegin(void);
void ChampionsTentGenerateOpponent(void);
void ChampionsTentHandleBattleResult(void);
void ChampionsTentEnd(void);
void ChampionsCircuitBufferRecord(void);

#endif // GUARD_CHAMPIONS_CIRCUIT_H
