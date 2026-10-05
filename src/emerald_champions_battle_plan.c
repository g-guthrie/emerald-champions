#include "global.h"
#include "event_data.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_util.h"
#include "constants/abilities.h"
#include "constants/battle_move_effects.h"
#include "battle_controllers.h"
#include "battle_setup.h"
#include "emerald_champions_agent_battle.h"
#include "emerald_champions_battle_plan.h"
#include "pokemon.h"
#include "item.h"
#include "constants/items.h"
#include "constants/form_change_types.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "data/emerald_champions_battle_plans.h"

// The tactic rows a trainer's lookups scan. The headless benchmark reads them
// through a per-battle copy that a proposed team can replace
// (emerald_champions_agent_battle.c). The release build expands to the
// compiled table exactly as written before, so its code is unchanged.
#if EC_HEADLESS_FIXTURES
#define EC_TACTIC_TABLE(trainer)                                                         \
    const struct EmeraldChampionsBattleTactic *ecTactics = sEmeraldChampionsBattleTactics; \
    u32 ecTacticCount = ARRAY_COUNT(sEmeraldChampionsBattleTactics);                     \
    EmeraldChampionsAgentFoeTactics(trainer, &ecTactics, &ecTacticCount);
#define EC_TACTICS ecTactics
#define EC_TACTIC_COUNT ecTacticCount
#else
#define EC_TACTIC_TABLE(trainer)
#define EC_TACTICS sEmeraldChampionsBattleTactics
#define EC_TACTIC_COUNT ARRAY_COUNT(sEmeraldChampionsBattleTactics)
#endif

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
#if EC_HEADLESS_FIXTURES
    u32 plan = trainer < ARRAY_COUNT(sEmeraldChampionsBattlePlans) ? sEmeraldChampionsBattlePlans[trainer] : 0;
    EmeraldChampionsAgentFoePlan(trainer, &plan);
    return plan;
#else
    return trainer < ARRAY_COUNT(sEmeraldChampionsBattlePlans) ? sEmeraldChampionsBattlePlans[trainer] : 0;
#endif
}

#if EC_HEADLESS_FIXTURES
u32 EmeraldChampions_GetCompiledPlan(u32 trainer, u32 *plan, u32 *megaPermissions,
                                     struct EmeraldChampionsBattleTactic *tactics, u32 maxTactics)
{
    u32 count = 0;
    *plan = trainer < ARRAY_COUNT(sEmeraldChampionsBattlePlans) ? sEmeraldChampionsBattlePlans[trainer] : 0;
    *megaPermissions = trainer < ARRAY_COUNT(sEmeraldChampionsMegaPermissions)
        ? sEmeraldChampionsMegaPermissions[trainer] : 0;
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsBattleTactics); i++)
    {
        if (sEmeraldChampionsBattleTactics[i].trainer != trainer)
            continue;
        if (count < maxTactics)
            tactics[count] = sEmeraldChampionsBattleTactics[i];
        count++;
    }
    return count;
}
#endif

bool32 EmeraldChampions_IsMegaAllowed(enum BattlerId battler)
{
    // One player star: a restricted party member blocks other Pokemon's Megas,
    // including while fainted or in reserve. Its own Mega remains eligible.
    // Trainer opponents and an NPC partner retain their authored permissions.
    if (GetBattlerTrainer(battler) == B_TRAINER_PLAYER
     && GetRestrictedPartyClass(gBattleMons[battler].species) == RESTRICTED_PARTY_NONE
     && GetUniquePartyRestrictedSlot() != PARTY_SIZE)
        return FALSE;

    // A legend's trump form is the final act's: the player's Rayquaza keeps
    // Dragon Ascent but Mega Evolves only after the Hall of Fame, like the
    // Orbs, Rusted weapons and fusion tools (legendary_signs.c).
    if (!TESTING && GetBattlerSide(battler) == B_SIDE_PLAYER
     && GET_BASE_SPECIES_ID(gBattleMons[battler].species) == SPECIES_RAYQUAZA
     && !FlagGet(FLAG_IS_CHAMPION))
        return FALSE;

    u32 trainer = GetCampaignTrainer(battler);
    u32 permissions = trainer < ARRAY_COUNT(sEmeraldChampionsMegaPermissions)
        ? sEmeraldChampionsMegaPermissions[trainer] : 0;
#if EC_HEADLESS_FIXTURES
    EmeraldChampionsAgentFoeMegaPermissions(trainer, &permissions);
#endif

    // Unauthored/facility/player parties retain native eligibility. Authored
    // slots only grant permission; the native item/form/owner checks still apply.
    if (!(permissions & 0x80))
        return TRUE;
    return gBattlerPartyIndexes[battler] < PARTY_SIZE
        && (permissions & (1u << gBattlerPartyIndexes[battler]));
}

bool32 EmeraldChampions_PlayerHasBlockedMega(void)
{
    if (GetUniquePartyRestrictedSlot() == PARTY_SIZE || !CheckBagHasItem(ITEM_MEGA_RING, 1))
        return FALSE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES);
        if (species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG)
         || GetRestrictedPartyClass(species) != RESTRICTED_PARTY_NONE)
            continue;
        const struct FormChange *forms = GetSpeciesFormChanges(species);
        enum Item item = GetMonData(mon, MON_DATA_HELD_ITEM);
        for (u32 i = 0; forms != NULL && forms[i].method != FORM_CHANGE_TERMINATOR; i++)
            if (forms[i].method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM && forms[i].param1 == item)
                return TRUE;
    }
    return FALSE;
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
#if EC_HEADLESS_FIXTURES
    EmeraldChampionsAgentFoeMegaPermissions(trainer, &permissions);
#endif
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
    EC_TACTIC_TABLE(trainer)
    for (u32 i = 0; i < EC_TACTIC_COUNT; i++)
    {
        const struct EmeraldChampionsBattleTactic *tactic = &EC_TACTICS[i];
        enum Species actor = GET_BASE_SPECIES_ID(tactic->actor);
        enum Species recipient = GET_BASE_SPECIES_ID(tactic->recipient);
        if (tactic->trainer == trainer
         && ((actor == species && recipient == partnerSpecies)
             || (recipient == species && actor == partnerSpecies)))
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
    EC_TACTIC_TABLE(trainer)
    for (u32 i = 0; i < EC_TACTIC_COUNT; i++)
    {
        const struct EmeraldChampionsBattleTactic *tactic = &EC_TACTICS[i];
        // Authored entries may name a regional form (Paldean Tauros); the
        // battlers are compared by base species, so the entries must be too.
        if (tactic->trainer == trainer && GET_BASE_SPECIES_ID(tactic->actor) == species
         && GET_BASE_SPECIES_ID(tactic->recipient) == recipientSpecies && tactic->move == move)
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
    EC_TACTIC_TABLE(trainer)
    for (u32 i = 0; i < EC_TACTIC_COUNT; i++)
        if (EC_TACTICS[i].trainer == trainer
         && GET_BASE_SPECIES_ID(EC_TACTICS[i].actor) == species
         && (EC_TACTICS[i].kind & kind))
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
