#ifndef GUARD_BATTLE_AI_RECORD_H
#define GUARD_BATTLE_AI_RECORD_H

struct AiPartyMon *GetBattlerAiPartyMon(enum BattlerId battler);

enum Ability GetRecordedAbility(enum BattlerId battler);
enum Move GetRecordedMove(enum BattlerId battler, u32 moveSlot);

void RecordLastUsedMoveBy(enum BattlerId battlerId, enum Move move);
void RecordKnownMove(enum BattlerId battlerId, enum Move move);
void RecordAllMoves(enum BattlerId battler);
void RecordAbilityBattle(enum BattlerId battlerId, enum Ability abilityId);
void RecordItemEffectBattle(enum BattlerId battlerId, enum HoldEffect itemEffect);
void ClearBattlerMoveHistory(enum BattlerId battlerId);
void ClearBattlerHistory(enum BattlerId battler);
void ClearBattlerItemEffectHistory(enum BattlerId battlerId);

#endif // GUARD_BATTLE_AI_RECORD_H
