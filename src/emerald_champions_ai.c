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
#define EC_AI_PREFERENCE          6
#define EC_AI_STRONG_REJECTION  (-20)
#define EC_AI_TARGET_LOCK         30

static bool32 PartnerHasSetupMove(enum BattlerId battler)
{
    enum BattlerId partner = GetPartnerBattler(battler);

    if (!IsDoubleBattle() || !IsBattlerAlive(partner))
        return FALSE;
    return HasMove(partner, MOVE_SWORDS_DANCE)
        || HasMove(partner, MOVE_DRAGON_DANCE)
        || HasMove(partner, MOVE_CALM_MIND)
        || HasMove(partner, MOVE_NASTY_PLOT)
        || HasMove(partner, MOVE_TAIL_GLOW)
        || HasMove(partner, MOVE_SHELL_SMASH)
        || HasMove(partner, MOVE_TRICK_ROOM);
}

s32 AI_EC_TrickRoomDiscipline(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum Move move = (enum Move)moveRaw;

    (void)battlerDefRaw;

    if (move == MOVE_TRICK_ROOM)
    {
        if (!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM) || gFieldTimers.trickRoomTimer == 1)
            score += EC_AI_STRONG_PREFERENCE;
        else
            score += EC_AI_STRONG_REJECTION;
    }
    else if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
    {
        // Lowering the opposing side's Speed or setting Tailwind makes a slow
        // Trick Room composition worse, not better.
        if (move == MOVE_ICY_WIND || move == MOVE_ELECTROWEB || move == MOVE_TAILWIND)
            score += EC_AI_STRONG_REJECTION;
        else if (move == MOVE_HELPING_HAND && IsBattlerAlive(GetPartnerBattler(battlerAtk)))
            score += EC_AI_PREFERENCE;
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
     && HasMove(battlerDef, MOVE_ERUPTION)
     && GetHealthPercentage(battlerDef) > 50)
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
    if (IsBattlerAlly(battlerAtk, battlerDef) || GetBattlerAbility(battlerDef) == ABILITY_TRUANT)
        return score + EC_AI_STRONG_REJECTION;

    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlive(battler) || IsBattlerAlly(battlerAtk, battler))
            continue;
        if (gBattleMons[battler].attack > gBattleMons[battlerDef].attack)
            return score - EC_AI_TARGET_LOCK;
    }
    return score + EC_AI_TARGET_LOCK;
}

s32 AI_EC_SnowScreen(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerAtk = (enum BattlerId)battlerAtkRaw;
    enum Move move = (enum Move)moveRaw;

    (void)battlerDefRaw;

    if (move != MOVE_AURORA_VEIL)
        return score;
    if (gSideStatuses[GetBattlerSide(battlerAtk)] & SIDE_STATUS_AURORA_VEIL)
        return score + EC_AI_STRONG_REJECTION;
    if (AI_GetWeather() & B_WEATHER_ICY_ANY)
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
    if (PartnerHasSetupMove(battlerAtk) || IsBattlersFirstTurn(partner) || GetHealthPercentage(partner) <= 50)
        return score + EC_AI_STRONG_PREFERENCE;
    return score;
}

s32 AI_EC_WallaceTerrain(u32 battlerAtkRaw, u32 battlerDefRaw, u32 moveRaw, s32 score)
{
    enum BattlerId battlerDef = (enum BattlerId)battlerDefRaw;
    enum Move move = (enum Move)moveRaw;

    (void)battlerAtkRaw;

    if (move == MOVE_HYPNOSIS
     && gFieldTimers.terrain == B_TERRAIN_MISTY
     && AI_IsBattlerGrounded(battlerDef))
        return score + EC_AI_STRONG_REJECTION;
    return score;
}

AiScoreFunc GetEmeraldChampionsDynamicAiFunc(u16 trainerId)
{
    switch (trainerId)
    {
    case TRAINER_ROXANNE_1:
    case TRAINER_NORMAN_1:
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
    case TRAINER_WALLACE:
        return AI_EC_WallaceTerrain;
    default:
        return NULL;
    }
}
