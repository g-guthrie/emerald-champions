#include "global.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "battle_controllers.h"
#include "battle_setup.h"
#include "emerald_champions_battle_plan.h"
#include "pokemon.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "data/emerald_champions_battle_plans.h"

static u32 GetCampaignTrainer(enum BattlerId battler)
{
    if (!(gBattleTypeFlags & BATTLE_TYPE_TRAINER)
        || gBattleTypeFlags & (BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER | BATTLE_TYPE_TRAINER_HILL
            | BATTLE_TYPE_EREADER_TRAINER | BATTLE_TYPE_SECRET_BASE | BATTLE_TYPE_RECORDED_LINK))
        return 0;

    // A single-player recording retains its original opponentA/B IDs. Only
    // recorded link battles use a different namespace; RECORDED alone does not.

    switch (GetBattlerTrainer(battler))
    {
    case B_TRAINER_OPPONENT_A:
        return TRAINER_BATTLE_PARAM.opponentA;
    case B_TRAINER_OPPONENT_B:
        return TRAINER_BATTLE_PARAM.opponentB;
    default:
        return 0;
    }
}

u32 EmeraldChampions_GetBattlePlan(enum BattlerId battler)
{
    u32 trainer = GetCampaignTrainer(battler);
    return trainer < ARRAY_COUNT(sEmeraldChampionsBattlePlans) ? sEmeraldChampionsBattlePlans[trainer] : 0;
}

bool32 EmeraldChampions_IsMegaAllowed(enum BattlerId battler)
{
    u32 trainer = GetCampaignTrainer(battler);
    u32 permissions = trainer < ARRAY_COUNT(sEmeraldChampionsMegaPermissions)
        ? sEmeraldChampionsMegaPermissions[trainer] : 0;

    // Unauthored/facility/player parties retain native eligibility. Authored
    // slots only grant permission; the native item/form/owner checks still apply.
    if (!(permissions & 0x80))
        return TRUE;
    return gBattlerPartyIndexes[battler] < PARTY_SIZE
        && (permissions & (1u << gBattlerPartyIndexes[battler]));
}

// Explicit endgame exceptions. Player, ordinary trainer and foreign battle
// namespaces retain the native one-Mega rule through GetCampaignTrainer.
// How many Mega Evolutions this battler's trainer may actually perform.
// One per side is the rule; a team that authors more than one Mega slot is
// licensed to use them all, which is how the Elite Four and the Champion get
// to break it. The count comes from the authored mega_slots line rather than a
// list here, so licensing a trainer is a data change.
u32 EmeraldChampions_GetMegaEvolutionLimit(enum BattlerId battler)
{
    u32 trainer = GetCampaignTrainer(battler);
    u32 permissions = trainer < ARRAY_COUNT(sEmeraldChampionsMegaPermissions)
        ? sEmeraldChampionsMegaPermissions[trainer] : 0;
    u32 slots, limit;

    // Unauthored/facility/player parties keep the native one-per-side rule.
    if (!(permissions & 0x80))
        return 1;

    for (slots = permissions & 0x3F, limit = 0; slots != 0; slots &= slots - 1)
        limit++;

    return limit > 0 ? limit : 1;
}

u32 EmeraldChampions_GetPartnerTactics(enum BattlerId battler, enum Species species, enum Species partnerSpecies)
{
    u32 trainer = GetCampaignTrainer(battler);
    u32 kinds = 0;
    if (trainer == TRAINER_NONE || trainer >= TRAINERS_COUNT)
        return 0;
    species = GET_BASE_SPECIES_ID(species);
    partnerSpecies = GET_BASE_SPECIES_ID(partnerSpecies);
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsBattleTactics); i++)
    {
        const struct EmeraldChampionsBattleTactic *tactic = &sEmeraldChampionsBattleTactics[i];
        if (tactic->trainer == trainer
         && ((tactic->actor == species && tactic->recipient == partnerSpecies)
             || (tactic->recipient == species && tactic->actor == partnerSpecies)))
            kinds |= tactic->kind;
    }
    return kinds;
}

u32 EmeraldChampions_GetTacticKind(enum BattlerId actor, enum BattlerId recipient, enum Move move)
{
    u32 trainer = GetCampaignTrainer(actor);
    if (trainer == TRAINER_NONE || trainer >= TRAINERS_COUNT || move == MOVE_NONE)
        return 0;
    enum Species species = GET_BASE_SPECIES_ID(gBattleMons[actor].species);
    enum Species recipientSpecies = GET_BASE_SPECIES_ID(gBattleMons[recipient].species);
    u32 kinds = 0;
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsBattleTactics); i++)
    {
        const struct EmeraldChampionsBattleTactic *tactic = &sEmeraldChampionsBattleTactics[i];
        if (tactic->trainer == trainer && tactic->actor == species
         && tactic->recipient == recipientSpecies && tactic->move == move)
            kinds |= tactic->kind;
    }
    return kinds;
}

bool32 EmeraldChampions_HasTacticActor(enum BattlerId actor, u32 kind)
{
    u32 trainer = GetCampaignTrainer(actor);
    if (trainer == TRAINER_NONE || trainer >= TRAINERS_COUNT)
        return FALSE;
    enum Species species = GET_BASE_SPECIES_ID(gBattleMons[actor].species);
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsBattleTactics); i++)
        if (sEmeraldChampionsBattleTactics[i].trainer == trainer
         && sEmeraldChampionsBattleTactics[i].actor == species
         && (sEmeraldChampionsBattleTactics[i].kind & kind))
            return TRUE;
    return FALSE;
}

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
