#include "global.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "emerald_champions_ai.h"
#include "move.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/trainers.h"

#define EC_AI_STRONG_PREFERENCE  12
#define EC_AI_STRONG_REJECTION  (-20)

static bool32 PartnerIsSettingUp(enum BattlerId battler)
{
    enum BattlerId partner = GetPartnerBattler(battler);
    enum Move move = gAiLogicData->partnerMove;

    // Cover the chosen setup action, not every turn of a Pokemon which happens
    // to know a setup move (including after its boosts have already maxed out).
    if (move == MOVE_TRICK_ROOM)
        return (!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM) && ShouldSetFieldStatus(partner, STATUS_FIELD_TRICK_ROOM))
            || ((gFieldStatuses & STATUS_FIELD_TRICK_ROOM) && ShouldClearFieldStatus(partner, STATUS_FIELD_TRICK_ROOM));
    if (move == MOVE_BELLY_DRUM)
        return GetHealthPercentage(partner) > 50 && gBattleMons[partner].statStages[STAT_ATK] < MAX_STAT_STAGE;
    return IsBattleMoveStatus(move) && IsStatRaisingMove(move) && AI_CanAnyStatChange(partner, partner, move);
}

s32 AI_EC_TrickRoomDiscipline(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum Move move = (enum Move)moveRaw;

    (void)battlerDefRaw;

    if (move == MOVE_TRICK_ROOM)
    {
        if (CanRefreshTrickRoom(battlerAtk))
            return score; // Shared doubles scoring owns the two-setter play.
        // Trick Room toggles off while active; it cannot refresh itself.
        // Keep cancellation available when the opposing side benefits instead.
        if ((!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM) && ShouldSetFieldStatus(battlerAtk, STATUS_FIELD_TRICK_ROOM))
         || ((gFieldStatuses & STATUS_FIELD_TRICK_ROOM) && gFieldTimers.trickRoomTimer > 1
          && ShouldClearFieldStatus(battlerAtk, STATUS_FIELD_TRICK_ROOM)))
            score += EC_AI_STRONG_PREFERENCE;
        else
            score += EC_AI_STRONG_REJECTION;
    }
    return score;
}

s32 AI_EC_FlanneryAfterYou(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum BattlerId battlerDef = (enum BattlerId)battlerDefRaw;
    enum Move move = (enum Move)moveRaw;

    if (move != MOVE_AFTER_YOU && move != MOVE_HELPING_HAND)
        return score;
    if (IsDoubleBattle()
     && IsBattlerAlly(battlerAtk, battlerDef)
     && battlerAtk != battlerDef
     && gAiLogicData->partnerMove == MOVE_ERUPTION
     && GetHealthPercentage(battlerDef) > 50
     && (move == MOVE_HELPING_HAND
      || AI_IsFaster(battlerAtk, battlerDef, move, MOVE_ERUPTION, CONSIDER_PRIORITY)))
        return score + EC_AI_STRONG_PREFERENCE;
    if (move == MOVE_AFTER_YOU)
        return score + EC_AI_STRONG_REJECTION;
    return score;
}

s32 AI_EC_QuincyTruant(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum BattlerId battlerDef = (enum BattlerId)battlerDefRaw;
    enum Move move = (enum Move)moveRaw;

    if (move != MOVE_ENTRAINMENT)
        return score;
    if (GetBattlerAbility(battlerAtk) != ABILITY_TRUANT
     || IsBattlerAlly(battlerAtk, battlerDef)
     || !CanEffectChangeAbility(battlerAtk, battlerDef, move, gAiLogicData)
     || DoesSubstituteBlockMove(battlerAtk, battlerDef, move))
        return score + EC_AI_STRONG_REJECTION;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlive(battler) || IsBattlerAlly(battlerAtk, battler)
         || !CanEffectChangeAbility(battlerAtk, battler, move, gAiLogicData)
         || DoesSubstituteBlockMove(battlerAtk, battler, move))
            continue;
        if (max(gBattleMons[battler].attack, gBattleMons[battler].spAttack)
          > max(gBattleMons[battlerDef].attack, gBattleMons[battlerDef].spAttack))
            return score;
    }
    return score + EC_AI_STRONG_PREFERENCE;
}

s32 AI_EC_SnowScreen(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum BattlerId battlerDef = (enum BattlerId)battlerDefRaw;
    enum Move move = (enum Move)moveRaw;

    if (move != MOVE_AURORA_VEIL)
        return score;
    if (gSideStatuses[GetBattlerSide(battlerAtk)] & SIDE_STATUS_AURORA_VEIL)
        return score + EC_AI_STRONG_REJECTION;
    if (ShouldSetScreen(battlerAtk, battlerDef, EFFECT_AURORA_VEIL)
     && GetMoveEffect(gAiLogicData->partnerMove) != EFFECT_AURORA_VEIL)
        return score + EC_AI_STRONG_PREFERENCE;
    return score;
}

s32 AI_EC_RedirectionSetup(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum Move move = (enum Move)moveRaw;
    enum BattlerId partner;

    (void)battlerDefRaw;

    if (GetMoveEffect(move) != EFFECT_FOLLOW_ME || !IsDoubleBattle())
        return score;
    partner = GetPartnerBattler(battlerAtk);
    if (!IsBattlerAlive(partner))
        return score + EC_AI_STRONG_REJECTION;
    if ((GetMoveEffect(gAiLogicData->partnerMove) == EFFECT_PROTECT
      && GetProtectType(GetMoveProtectMethod(gAiLogicData->partnerMove)) == PROTECT_TYPE_SINGLE)
     || IsBattlerIncapacitated(partner, gAiLogicData->abilities[partner])
     || gAiLogicData->shouldSwitch & (1u << partner))
        return score;
    if (PartnerIsSettingUp(battlerAtk) || IsBattlersFirstTurn(partner) || GetHealthPercentage(partner) <= 50)
        return score + EC_AI_STRONG_PREFERENCE;
    return score;
}

AiScoreFunc GetEmeraldChampionsDynamicAiFunc(u16 trainerId)
{
    switch (trainerId)
    {
    case TRAINER_ROXANNE_1:
    case TRAINER_TATE_AND_LIZA_1:
        return AI_EC_TrickRoomDiscipline;
    case TRAINER_FLANNERY_1:
        return AI_EC_FlanneryAfterYou;
    case TRAINER_QUINCY:
        return AI_EC_QuincyTruant;
    case TRAINER_SHELLY_WEATHER_INSTITUTE:
    case TRAINER_GLACIA:
        return AI_EC_SnowScreen;
    case TRAINER_BRAWLY_1:
    case TRAINER_WALLY_VR_1:
    case TRAINER_WALLY_VR_2:
    case TRAINER_LEAF_ALTERING_CAVE:
    case TRAINER_CYNTHIA_1:
        return AI_EC_RedirectionSetup;
    default:
        return NULL;
    }
}
