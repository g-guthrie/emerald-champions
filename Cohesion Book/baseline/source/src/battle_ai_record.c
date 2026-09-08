#include "global.h"
#include "battle.h"
#include "battle_ai_record.h"
#include "battle_setup.h"
#include "battle_controllers.h"
#include "battle_factory.h"
#include "constants/abilities.h"
#include "constants/hold_effects.h"
#include "constants/battle_ai.h"

struct AiPartyMon *GetBattlerAiPartyMon(enum BattlerId battler)
{
    return &gAiPartyData->mons[GetBattlerTrainer(battler)][gBattlerPartyIndexes[battler]];
}

static bool32 HasTemporaryAbility(enum BattlerId battler)
{
    return gBattleMons[battler].volatiles.transformed || gBattleMons[battler].volatiles.overwrittenAbility;
}

enum Ability GetRecordedAbility(enum BattlerId battler)
{
    if (gBattleMons[battler].volatiles.overwrittenAbility)
        return gBattleMons[battler].volatiles.overwrittenAbility;
    if (HasTemporaryAbility(battler))
        return gBattleHistory->abilities[battler];
    return GetBattlerAiPartyMon(battler)->ability;
}

enum Move GetRecordedMove(enum BattlerId battler, u32 moveSlot)
{
    if (!MOVE_IS_PERMANENT(battler, moveSlot))
        return gBattleHistory->usedMoves[battler][moveSlot];
    return GetBattlerAiPartyMon(battler)->moves[moveSlot];
}

static void RecordMoveSlot(enum BattlerId battler, u32 moveSlot)
{
    enum Move move = gBattleMons[battler].moves[moveSlot];
    gBattleHistory->usedMoves[battler][moveSlot] = move;
    if (MOVE_IS_PERMANENT(battler, moveSlot))
        GetBattlerAiPartyMon(battler)->moves[moveSlot] = move;
}

void RecordLastUsedMoveBy(enum BattlerId battlerId, enum Move move)
{
    u8 *index = &gBattleHistory->moveHistoryIndex[battlerId];

    if (++(*index) >= AI_MOVE_HISTORY_COUNT)
        *index = 0;
    gBattleHistory->moveHistory[battlerId][*index] = move;
}

void RecordKnownMove(enum BattlerId battler, enum Move move)
{
    s32 moveIndex;

    for (moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
    {
        if (gBattleMons[battler].moves[moveIndex] == move)
            break;
    }

    if (moveIndex < MAX_MON_MOVES)
        RecordMoveSlot(battler, moveIndex);
}

void RecordAllMoves(enum BattlerId battler)
{
    for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
        RecordMoveSlot(battler, moveSlot);
}

void RecordAbilityBattle(enum BattlerId battlerId, enum Ability abilityId)
{
    gBattleHistory->abilities[battlerId] = abilityId;
    if (!HasTemporaryAbility(battlerId))
        GetBattlerAiPartyMon(battlerId)->ability = abilityId;
}

void RecordItemEffectBattle(enum BattlerId battlerId, enum HoldEffect itemEffect)
{
    gBattleHistory->itemEffects[battlerId] = itemEffect;
    GetBattlerAiPartyMon(battlerId)->heldEffect = itemEffect;
}

void ClearBattlerMoveHistory(enum BattlerId battlerId)
{
    memset(gBattleHistory->usedMoves[battlerId], 0, sizeof(gBattleHistory->usedMoves[battlerId]));
    memset(gBattleHistory->moveHistory[battlerId], 0, sizeof(gBattleHistory->moveHistory[battlerId]));
    gBattleHistory->moveHistoryIndex[battlerId] = 0;
}

void ClearBattlerItemEffectHistory(enum BattlerId battlerId)
{
    gBattleHistory->itemEffects[battlerId] = HOLD_EFFECT_NONE;
}

void ClearBattlerHistory(enum BattlerId battler)
{
    ClearBattlerMoveHistory(battler);
    gBattleHistory->abilities[battler] = ABILITY_NONE;
    ClearBattlerItemEffectHistory(battler);
}
