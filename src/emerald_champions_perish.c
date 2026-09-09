#include "global.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "emerald_champions_battle_plan.h"
#include "emerald_champions_perish.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"

// Native end-turn handling prints the current count before decrementing it.
// Internal zero therefore means death at THIS turn's end, not an extra turn.
// Soundproof prevents application; gaining it does not clear an existing song.
bool32 EC_PerishMustEscape(enum BattlerId battler)
{
    return IsBattlerAlive(battler)
        && gBattleMons[battler].volatiles.perishSong
        && gBattleMons[battler].volatiles.perishSongTimer == 0;
}

static bool32 HasPerishPlan(enum BattlerId battler)
{
    return EmeraldChampions_GetBattlePlan(battler) & EC_BATTLE_PLAN_PERISH_TRAP;
}

static bool32 IsAffectedFoe(enum BattlerId battler, enum BattlerId foe)
{
    return IsBattlerAlive(foe) && !IsBattlerAlly(battler, foe)
        && gBattleMons[foe].volatiles.semiInvulnerable != STATE_COMMANDER
        && gBattleMons[foe].volatiles.perishSong;
}

bool32 EC_PerishShouldPivotEarly(enum BattlerId battler)
{
    enum BattlerId partner = GetPartnerBattler(battler);
    if (!HasPerishPlan(battler) || !IsBattlerAlive(battler)
        || !gBattleMons[battler].volatiles.perishSong
        || gBattleMons[battler].volatiles.perishSongTimer != 1
        || !IsBattlerAlive(partner)
        || gAiLogicData->abilities[partner] != ABILITY_SHADOW_TAG
        || gAiLogicData->abilities[battler] == ABILITY_SHADOW_TAG
        || CountUsablePartyMons(battler) == 0)
        return FALSE;

    // Move the singer out one turn before its deadline while the partner
    // maintains the trap. This also leaves two distinct reserves available
    // without routinely spending both actions on a last-turn double switch.
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        if (IsAffectedFoe(battler, foe) && IsBattlerTrapped(battler, foe))
            return TRUE;
    return FALSE;
}

s32 EC_PerishPlanScore(enum BattlerId battler, enum Move move)
{
    if (!HasPerishPlan(battler))
        return 0;

    enum BattleMoveEffects effect = GetMoveEffect(move);
    if (effect == EFFECT_PERISH_SONG)
    {
        u32 freshTargets = 0, trappedTargets = 0;
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (!IsBattlerAlive(foe) || IsBattlerAlly(battler, foe)
                || gBattleMons[foe].volatiles.semiInvulnerable == STATE_COMMANDER
                || gBattleMons[foe].volatiles.perishSong
                || gAiLogicData->abilities[foe] == ABILITY_SOUNDPROOF)
                continue;
            freshTargets++;
            if (IsBattlerTrapped(battler, foe))
                trappedTargets++;
        }
        if (!freshTargets)
            return -10000;
        // Do not invent a guaranteed three-turn win against freely switching
        // opponents. Native move scoring can still value forcing them out.
        if (!trappedTargets)
            return 0;
        // A trap is a plan, not permission to sacrifice the entire own side.
        // Leave last-Pokemon race decisions to ordinary native scoring.
        if (gAiLogicData->abilities[battler] != ABILITY_SOUNDPROOF
            && CountUsablePartyMons(battler) == 0)
            return 0;
        return trappedTargets * 55;
    }

    if (effect == EFFECT_PROTECT
        && GetProtectType(GetMoveProtectMethod(move)) == PROTECT_TYPE_SINGLE
        && !EC_PerishMustEscape(battler))
    {
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
            if (IsAffectedFoe(battler, foe) && IsBattlerTrapped(battler, foe))
                return 20;
    }
    return 0;
}
