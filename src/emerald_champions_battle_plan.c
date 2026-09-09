#include "global.h"
#include "battle.h"
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
