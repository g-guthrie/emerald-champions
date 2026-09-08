#include "global.h"
#include "battle.h"
#include "battle_ai_main.h"
#include "battle_main.h"
#include "battle_ai_util.h"
#include "battle_ai_switch.h"
#include "battle_controllers.h"
#include "battle_gimmick.h"
#include "battle_setup.h"
#include "constants/opponents.h"
#include "battle_script_commands.h"
#include "battle_stat_change.h"
#include "battle_util.h"
#include "item.h"
#include "malloc.h"
#include "pokemon.h"
#include "random.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/items.h"
#include "constants/battle_script_commands.h"

// Four moves, each with at most four legal targets. Switch recipients and
// Mega forms are board alternatives outside this list, never a shortlist.
#define PAIR_ACTIONS (MAX_MON_MOVES * MAX_BATTLERS_COUNT)
#define PAIR_IDLE MAX_MON_MOVES

struct PairAction
{
    enum Move move;
    s16 score;
    u8 index;
    u8 target;
    u16 koChance[MAX_BATTLERS_COUNT]; // per mille; only threshold-sensitive actions expand rolls
};

struct PairBoard
{
    struct SwitchCandidateSnapshot *state;
    struct { u16 hp; } mons[MAX_BATTLERS_COUNT];
    struct AiLogicData logic;
    s32 reserveValue[NUM_BATTLE_SIDES];
    u8 reserveLiving[MAX_BATTLE_TRAINERS];
    u8 owner[MAX_BATTLERS_COUNT];
    u8 side[MAX_BATTLERS_COUNT];
    u8 activeMask;
    u8 scoredActiveMask;
    bool8 playerCanUsePartner;
    bool8 opponentHasPartner;
};

struct PairEvaluation
{
    struct PairBoard board;
    struct PairAction choices[MAX_BATTLERS_COUNT][PAIR_ACTIONS];
    struct PairAction action[MAX_BATTLERS_COUNT];
    u8 count[MAX_BATTLERS_COUNT];
    u8 acted, stopped, changed, protected, wideGuard, quickGuard;
    u8 redirect[NUM_BATTLE_SIDES];
    u8 forceNext;
    u8 roundUsers;
    u8 executed;
    enum Move pledgeMove[NUM_BATTLE_SIDES];
    enum BattlerId pledgeUser[NUM_BATTLE_SIDES];
    bool8 fieldChanged;
    bool8 conservative;
    enum BattleSide side;
    s32 utility;
};

static void SavePairBoard(struct PairBoard *board)
{
    AI_CaptureCandidateState(board->state);
    u8 scoreLimit[MAX_BATTLE_TRAINERS] = {0};
    u8 ownerSide[MAX_BATTLE_TRAINERS] = {0};
    board->activeMask = board->scoredActiveMask = 0;
    memset(board->reserveValue, 0, sizeof(board->reserveValue));
    memset(board->reserveLiving, 0, sizeof(board->reserveLiving));
    board->playerCanUsePartner = (gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
        && !WillPlayerWhiteOutIfPartnerWinsAlone();
    board->opponentHasPartner = BattleSideHasTwoTrainers(B_SIDE_OPPONENT);
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        board->mons[actor].hp = gBattleMons[actor].hp;
        board->owner[actor] = GetBattlerTrainer(actor);
        board->side[actor] = GetBattlerSide(actor);
        scoreLimit[board->owner[actor]] = GetAILastPartyIndex(actor);
        ownerSide[board->owner[actor]] = board->side[actor];
    }
    // Reserve contributions cannot change inside a one-turn trial. Read and
    // decrypt each owned party slot once, then only active HP is dynamic.
    for (enum BattleTrainer trainer = 0; trainer < MAX_BATTLE_TRAINERS; trainer++)
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            struct Pokemon *mon = &gParties[trainer][slot];
            enum Species species = GetMonData(mon, MON_DATA_SPECIES);
            if (species == SPECIES_NONE || GetMonData(mon, MON_DATA_IS_EGG))
                continue;
            if ((gBattleTypeFlags & BATTLE_TYPE_ARENA)
                && ((trainer == B_TRAINER_PLAYER && (gBattleStruct->arenaLostPlayerMons & (1u << slot)))
                    || (trainer == B_TRAINER_OPPONENT_A && (gBattleStruct->arenaLostOpponentMons & (1u << slot)))))
                continue;
            enum BattlerId active;
            for (active = 0; active < gBattlersCount; active++)
                if (!(gAbsentBattlerFlags & (1u << active)) && !gBattleStruct->battlerState[active].notOnField
                    && board->owner[active] == trainer && gBattlerPartyIndexes[active] == slot)
                    break;
            if (active < gBattlersCount)
            {
                board->activeMask |= 1u << active;
                if (slot < scoreLimit[trainer])
                    board->scoredActiveMask |= 1u << active;
                continue;
            }
            u32 hp = GetMonData(mon, MON_DATA_HP);
            if (!hp)
                continue;
            // Native terminal checks scan all six slots, even when the AI's
            // selectable/scored party is a three-member multi selection.
            board->reserveLiving[trainer]++;
            if (slot < scoreLimit[trainer])
            {
                u32 maxHp = GetMonData(mon, MON_DATA_MAX_HP);
                board->reserveValue[ownerSide[trainer]] += 80 + hp * 100 / max(1, maxHp);
                if (species == SPECIES_PALAFIN_HERO)
                    board->reserveValue[ownerSide[trainer]] += 10;
            }
        }
}

static void RestorePairBoard(const struct PairBoard *board)
{
    AI_RestoreCandidateState(board->state);
    *gAiLogicData = board->logic;
}

static bool32 PairSupport(enum Move move)
{
    switch (GetMoveEffect(move))
    {
    case EFFECT_HELPING_HAND:
    case EFFECT_AFTER_YOU:
    case EFFECT_INSTRUCT:
    case EFFECT_FOLLOW_ME:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 PairTargetIsLegal(enum BattlerId actor, enum BattlerId target, enum Move move)
{
    if (!IsBattlerAlive(target))
        return FALSE;
    switch (AI_GetBattlerMoveTargetType(actor, move))
    {
    case TARGET_USER:
    case TARGET_USER_AND_ALLY:
    case TARGET_FIELD:
    case TARGET_OPPONENTS_FIELD:
    case TARGET_ALL_BATTLERS:
        return target == actor;
    case TARGET_ALLY:
        return target == GetPartnerBattler(actor);
    case TARGET_USER_OR_ALLY:
        return IsBattlerAlly(actor, target);
    case TARGET_BOTH:
    case TARGET_FOES_AND_ALLY:
    case TARGET_RANDOM:
        return target == (IsBattlerAlive(GetOppositeBattler(actor)) ? GetOppositeBattler(actor) : GetOppositeBattler(GetPartnerBattler(actor)));
    case TARGET_OPPONENT:
        return !IsBattlerAlly(actor, target);
    default:
        return actor != target && CanTargetBattler(actor, target, move);
    }
}

static void BuildPairActions(struct PairEvaluation *ev, enum BattlerId actor, u32 noActionMask)
{
    ev->count[actor] = 0;
    if (!(noActionMask & (1u << actor)) && IsBattlerAlive(actor))
    {
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = gBattleMons[actor].moves[index];
            if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[actor]))
                continue;
            u16 koChance[MAX_BATTLERS_COUNT] = {0};
            for (enum BattlerId defender = 0; defender < gBattlersCount; defender++)
            {
                if (!IsBattlerAlive(defender) || IsBattlerAlly(actor, defender))
                    continue;
                struct SimulatedDamage damage = gAiLogicData->simulatedDmg[actor][defender][index];
                u32 hp = gBattleMons[defender].hp;
                if (damage.maximum * 2 < hp || damage.minimum >= hp)
                    continue;
                struct AiCalcValues calc = { .move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain };
                struct AiKOChance chance = AI_CalcKOChance(&calc, actor, defender, hp);
                if (chance.exact)
                    koChance[defender] = chance.numerator * 1000 / chance.denominator;
            }
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
            {
                if (!PairTargetIsLegal(actor, target, move))
                    continue;
                enum BattlerId scoreTarget = target == actor ? GetOppositeBattler(actor) : target;
                if (!IsBattlerAlive(scoreTarget))
                    scoreTarget = GetPartnerBattler(scoreTarget);
                gAiLogicData->partnerMove = MOVE_NONE;
                s32 score = AI_ScoreMoveAgainstTarget(actor, scoreTarget, index);
                // Enabling actions are retained even when their independent
                // heuristic cannot yet see a concrete partner action.
                ev->choices[actor][ev->count[actor]++] = (struct PairAction){move, score, index, target};
                struct PairAction *action = &ev->choices[actor][ev->count[actor] - 1];
                memcpy(action->koChance, koChance, sizeof(koChance));
            }
        }
    }
    if (ev->count[actor] == 0)
    {
        if (!(noActionMask & (1u << actor)) && IsBattlerAlive(actor))
            ev->choices[actor][ev->count[actor]++] = (struct PairAction){MOVE_STRUGGLE, AI_SCORE_DEFAULT, 0, IsBattlerAlive(GetOppositeBattler(actor)) ? GetOppositeBattler(actor) : GetOppositeBattler(GetPartnerBattler(actor))};
        else
            ev->choices[actor][ev->count[actor]++] = (struct PairAction){MOVE_NONE, AI_SCORE_DEFAULT, PAIR_IDLE, actor};
    }
}

static bool32 PairSpread(enum Move move)
{
    enum MoveTarget target = GetMoveTarget(move);
    return target == TARGET_BOTH || target == TARGET_FOES_AND_ALLY || target == TARGET_ALL_BATTLERS;
}

static s32 PairActionForecast(enum BattlerId actor, const struct PairAction *action)
{
    if (action->index == PAIR_IDLE)
        return 0;
    s32 score = (action->score - AI_SCORE_DEFAULT) * 4;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (target == actor || !IsBattlerAlive(target))
            continue;
        if (PairSpread(action->move))
        {
            if (IsBattlerAlly(actor, target) && GetMoveTarget(action->move) == TARGET_BOTH)
                continue;
        }
        else if (target != action->target)
            continue;
        s32 damage = gAiLogicData->simulatedDmg[actor][target][action->index].median;
        damage = min(damage, gBattleMons[target].hp) * 100 / max(1, gBattleMons[target].maxHP);
        score += IsBattlerAlly(actor, target) ? -damage : damage;
        if (!IsBattlerAlly(actor, target))
            score += action->koChance[target] * 80 / 1000;
    }
    return score;
}

static void PairRaiseStat(struct PairEvaluation *ev, enum BattlerId actor, enum Stat stat, s32 amount)
{
    amount = GetAdjustedStatStage(amount, gAiLogicData->abilities[actor], FALSE);
    gBattleMons[actor].statStages[stat] = min(MAX_STAT_STAGE, max(MIN_STAT_STAGE, gBattleMons[actor].statStages[stat] + amount));
    gAiLogicData->speedStats[actor] = GetBattlerTotalSpeedStat(actor, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor]);
    ev->changed |= 1u << actor;
}

static void PairConsumeItem(struct PairEvaluation *ev, enum BattlerId target)
{
    gBattleMons[target].item = ITEM_NONE;
    gAiLogicData->items[target] = ITEM_NONE;
    gAiLogicData->holdEffects[target] = HOLD_EFFECT_NONE;
    ev->changed |= 1u << target;
}

static void PairHeal(enum BattlerId actor, u32 hp)
{
    if (!gBattleMons[actor].hp)
        return;
    if (!gBattleMons[actor].volatiles.healBlockTimer)
        gBattleMons[actor].hp = min(gBattleMons[actor].maxHP, gBattleMons[actor].hp + hp);
    gAiLogicData->hpPercents[actor] = gBattleMons[actor].hp * 100 / max(1, gBattleMons[actor].maxHP);
}

static void PairHealingBerry(struct PairEvaluation *ev, enum BattlerId target)
{
    enum HoldEffect effect = gAiLogicData->holdEffects[target];
    enum Item item = gBattleMons[target].item;
    enum Ability ability = gAiLogicData->abilities[target];
    if (!gBattleMons[target].hp || gBattleMons[target].volatiles.healBlockTimer || IsUnnerveBlocked(target, item))
        return;
    u32 hpFraction = effect == HOLD_EFFECT_CONFUSE_FLAVOR ? CONFUSE_BERRY_HP_FRACTION : 2;
    if ((effect == HOLD_EFFECT_RESTORE_HP || effect == HOLD_EFFECT_RESTORE_PCT_HP || effect == HOLD_EFFECT_CONFUSE_FLAVOR)
     && HasEnoughHpToEatBerry(target, ability, hpFraction, item))
    {
        u32 param = GetBattlerHoldEffectParam(target);
        u32 amount = effect == HOLD_EFFECT_RESTORE_PCT_HP ? max(1, GetNonDynamaxMaxHP(target) * param / 100)
            : effect == HOLD_EFFECT_CONFUSE_FLAVOR ? max(1, GetNonDynamaxMaxHP(target) / param) : param;
        if (ability == ABILITY_RIPEN && GetItemPocket(item) == POCKET_BERRIES)
            amount *= 2;
        PairHeal(target, amount);
        PairConsumeItem(ev, target);
    }
}

static void PairStatEffects(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, enum Move move, bool32 selfOnly)
{
    for (u32 index = 0; index < GetMoveAdditionalEffectCount(move); index++)
    {
        const struct AdditionalEffect *effect = GetMoveAdditionalEffectById(move, index);
        if ((effect->chance != 0 && effect->chance != 100)
         || (effect->moveEffect != STAT_CHANGE_EFFECT_PLUS && effect->moveEffect != STAT_CHANGE_EFFECT_MINUS)
         || (selfOnly && !effect->self))
            continue;
        enum BattlerId recipient = effect->self ? actor : target;
        if (!gBattleMons[recipient].hp || (recipient != actor && DoesSubstituteBlockMove(actor, recipient, move)))
            continue;
        for (enum Stat stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
        {
            s32 amount = GetStatStage(stat, effect);
            if (!amount)
                continue;
            if (effect->moveEffect == STAT_CHANGE_EFFECT_MINUS)
            {
                if (recipient != actor && !CanLowerStat(actor, recipient, gAiLogicData, stat))
                    continue;
                amount = -amount;
            }
            PairRaiseStat(ev, recipient, stat, amount);
        }
    }
}

static enum Type PairMoveType(enum BattlerId actor, enum Move move)
{
    bool32 ateBoost = gBattleStruct->battlerState[actor].ateBoost;
    enum Type type = GetDynamicMoveType(GetBattlerMon(actor), move, actor,
        gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor], MON_IN_BATTLE);
    gBattleStruct->battlerState[actor].ateBoost = ateBoost;
    if (type == TYPE_NONE)
        type = GetMoveType(move);
    if (((gFieldStatuses & STATUS_FIELD_ION_DELUGE) && type == TYPE_NORMAL) || gBattleMons[actor].volatiles.electrified)
        type = TYPE_ELECTRIC;
    return type;
}

static bool32 PairAbsorbs(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, enum Move move, enum Type type)
{
    enum Ability ability = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor], gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], move);
    if ((type == TYPE_WATER && (ability == ABILITY_WATER_ABSORB || ability == ABILITY_DRY_SKIN))
     || (type == TYPE_ELECTRIC && ability == ABILITY_VOLT_ABSORB)
     || (type == TYPE_GROUND && ability == ABILITY_EARTH_EATER))
    {
        PairHeal(target, max(1, gBattleMons[target].maxHP / 4));
        return TRUE;
    }
    if ((type == TYPE_WATER && ability == ABILITY_STORM_DRAIN)
     || (type == TYPE_ELECTRIC && ability == ABILITY_LIGHTNING_ROD))
    {
        PairRaiseStat(ev, target, STAT_SPATK, 1);
        return TRUE;
    }
    if (type == TYPE_ELECTRIC && ability == ABILITY_MOTOR_DRIVE)
    {
        PairRaiseStat(ev, target, STAT_SPEED, 1);
        return TRUE;
    }
    if (type == TYPE_GRASS && ability == ABILITY_SAP_SIPPER)
    {
        PairRaiseStat(ev, target, STAT_ATK, 1);
        return TRUE;
    }
    if (type == TYPE_FIRE && ability == ABILITY_FLASH_FIRE)
    {
        gBattleMons[target].volatiles.flashFireBoosted = TRUE;
        ev->changed |= 1u << target;
        return TRUE;
    }
    return FALSE;
}

static void PairAfterDamage(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, const struct PairAction *action, u32 damage, uq4_12_t effectiveness, u32 skippedStrikes, bool32 singleStrike)
{
    enum Move move = action->move;
    enum Type type = PairMoveType(actor, move);
    enum Ability ability = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor], gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], move);
    if (!damage || !gBattleMons[target].hp)
        return;
    if ((type == TYPE_FIRE || type == TYPE_WATER) && ability == ABILITY_STEAM_ENGINE)
        PairRaiseStat(ev, target, STAT_SPEED, 6);
    if (type == TYPE_DARK && ability == ABILITY_JUSTIFIED)
    {
        u32 hits = 1;
        if (!singleStrike && GetMoveEffect(move) == EFFECT_BEAT_UP)
            hits = AI_GetBeatUpHitCount(actor);
        PairRaiseStat(ev, target, STAT_ATK, singleStrike ? 1 : max(0, (s32)hits - (s32)skippedStrikes));
    }
    if (ability == ABILITY_ANGER_POINT && AI_MoveAlwaysCrits(actor, target, move))
        PairRaiseStat(ev, target, STAT_ATK, MAX_STAT_STAGE);
    if (ability == ABILITY_STAMINA)
        PairRaiseStat(ev, target, STAT_DEF, 1);
    if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_WEAKNESS_POLICY && effectiveness > Q_4_12(1.0))
    {
        PairRaiseStat(ev, target, STAT_ATK, 2);
        PairRaiseStat(ev, target, STAT_SPATK, 2);
        PairConsumeItem(ev, target);
    }
    PairHealingBerry(ev, target);
}

static u32 PairRollDamage(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, struct SimulatedDamage damage, u32 accuracy)
{
    bool32 paralyzed = (gBattleMons[actor].status1 & STATUS1_PARALYSIS)
        && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[actor] == ABILITY_MAGIC_GUARD);
    if (ev->conservative)
    {
        if (GetBattlerSide(actor) != ev->side || IsBattlerAlly(actor, target))
            return damage.maximum;
        return accuracy < 100 || paralyzed ? 0 : damage.minimum;
    }
    u32 amount = damage.median * accuracy / 100;
    if (paralyzed)
        amount = GetConfig(B_PARALYSIS_CHANCE) >= GEN_CHAMPIONS ? amount * 7 / 8 : amount * 3 / 4;
    if ((gBattleMons[actor].status1 & STATUS1_FREEZE) && !MoveThawsUser(ev->action[actor].move))
        amount = amount * (GetConfig(B_FREEZE_TURNS) >= GEN_CHAMPIONS ? 25 : 20) / 100;
    return amount;
}

static bool32 PairDamageConsumedItem(const struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, const struct SimulatedDamage *damage)
{
    u32 roll = AI_ITEM_CONSUMED_MEDIAN;
    if (ev->conservative)
        roll = GetBattlerSide(actor) != ev->side || IsBattlerAlly(actor, target)
            ? AI_ITEM_CONSUMED_MAXIMUM : AI_ITEM_CONSUMED_MINIMUM;
    return damage->consumedItem & roll;
}

static bool32 PairHasBarrier(enum BattlerId actor, enum BattlerId target, enum Move move, enum Ability ability)
{
    return (gBattleMons[target].volatiles.substitute && DoesSubstituteBlockMove(actor, target, move))
        || (!gBattleMons[target].volatiles.transformed
         && ((ability == ABILITY_DISGUISE && IsMimikyuDisguised(target))
          || (ability == ABILITY_ICE_FACE && gBattleMons[target].species == SPECIES_EISCUE_ICE && IsBattleMovePhysical(move))));
}

static struct SimulatedDamage PairBreakBarriers(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, enum Move move, enum Ability ability, u32 accuracy, u32 *skipped)
{
    struct AiCalcValues calc = { .move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain, .strikeLimit = 1 };
    struct SimulatedDamage damage = {0};
    // A move's strike-count field is four bits; Beat Up and variable multihit
    // counts are smaller. Only an intact barrier takes this per-strike path.
    while (*skipped < 15 && PairHasBarrier(actor, target, move, ability))
    {
        calc.skipStrikes = *skipped;
        damage = AI_CalcDamage(&calc, actor, target);
        u32 amount = PairRollDamage(ev, actor, target, damage, accuracy);
        if (!amount)
            return (struct SimulatedDamage){0};
        if (PairDamageConsumedItem(ev, actor, target, &damage))
            PairConsumeItem(ev, target);
        if (gBattleMons[target].volatiles.substitute && DoesSubstituteBlockMove(actor, target, move))
        {
            gBattleMons[target].volatiles.substituteHP -= min(amount, gBattleMons[target].volatiles.substituteHP);
            if (!gBattleMons[target].volatiles.substituteHP)
                gBattleMons[target].volatiles.substitute = FALSE;
        }
        else
        {
            enum Move savedMove = gCurrentMove;
            enum DamageCategory savedCategory = gBattleStruct->dynamicMoveCategory;
            gCurrentMove = move;
            gBattleStruct->dynamicMoveCategory = DAMAGE_CATEGORY_NONE;
            enum Species species = GetBattleFormChangeTargetSpecies(target, FORM_CHANGE_BATTLE_HIT_BY_MOVE_CATEGORY, ability);
            gCurrentMove = savedMove;
            gBattleStruct->dynamicMoveCategory = savedCategory;
            struct Pokemon mon = *GetBattlerMon(target);
            u32 hp = gBattleMons[target].hp;
            SetMonData(&mon, MON_DATA_HP, &hp);
            SetMonData(&mon, MON_DATA_SPECIES, &species);
            gBattleMons[target].species = species;
            RecalcBattlerStats(target, &mon, FALSE);
            if (ability == ABILITY_DISGUISE && GetConfig(B_DISGUISE_HP_LOSS) >= GEN_8)
                gBattleMons[target].hp -= min(gBattleMons[target].hp, max(1, gBattleMons[target].maxHP / 8));
            SetBattlerAiData(target, gAiLogicData);
        }
        (*skipped)++;
        ev->changed |= 1u << target;
        if (!gBattleMons[target].hp)
            return (struct SimulatedDamage){0};
    }
    calc.skipStrikes = *skipped;
    calc.strikeLimit = 0;
    return AI_CalcDamage(&calc, actor, target);
}

static void PairHit(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, const struct PairAction *action)
{
    enum Move move = action->move;
    bool32 foe = GetBattlerSide(actor) != ev->side;
    if (!gBattleMons[target].hp)
        return;
    bool32 pierces = AI_CanContactBypassProtect(actor, target, move);
    if (!MoveIgnoresProtect(move) && !pierces
     && ((ev->protected & (1u << target))
      || (PairSpread(move) && (ev->wideGuard & (1u << GetBattlerSide(target))))
      || (AI_GetMovePriority(actor, gAiLogicData->abilities[actor], move) > 0 && !IsBattlerAlly(actor, target) && (ev->quickGuard & (1u << GetBattlerSide(target))))))
        return;
    if (Ai_IsPriorityBlocked(actor, target, move, gAiLogicData))
        return;
    if (!CanBreakThroughSemiInvulnerablity(actor, target, gAiLogicData->abilities[actor], gAiLogicData->abilities[target], move))
        return;
    if (MoveHasAdditionalEffect(move, MOVE_EFFECT_BREAK_SCREEN))
    {
        enum BattleSide side = GetBattlerSide(target);
        gSideStatuses[side] &= ~(SIDE_STATUS_REFLECT | SIDE_STATUS_LIGHTSCREEN | SIDE_STATUS_AURORA_VEIL);
        gSideTimers[side].reflectTimer = gSideTimers[side].lightscreenTimer = gSideTimers[side].auroraVeilTimer = 0;
        ev->fieldChanged = TRUE;
    }
    struct SimulatedDamage damage = gAiLogicData->simulatedDmg[actor][target][action->index];
    uq4_12_t effectiveness = gAiLogicData->effectiveness[actor][target][action->index];
    bool32 recalculate = ev->fieldChanged || (ev->changed & ((1u << actor) | (1u << target)))
        || move != gBattleMons[actor].moves[action->index]
        || (gBattleMons[target].hp != ev->board.mons[target].hp && (gAiLogicData->abilities[target] == ABILITY_MULTISCALE || gAiLogicData->abilities[target] == ABILITY_SHADOW_SHIELD))
        || (gBattleMons[actor].hp != ev->board.mons[actor].hp
         && (GetMoveEffect(move) == EFFECT_FLAIL || GetMoveEffect(move) == EFFECT_POWER_BASED_ON_USER_HP))
        || (gBattleMons[target].hp != ev->board.mons[target].hp && GetMoveEffect(move) == EFFECT_POWER_BASED_ON_TARGET_HP);
    if (recalculate)
    {
        struct AiCalcValues calc = { .move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain };
        damage = AI_CalcDamage(&calc, actor, target);
        effectiveness = calc.typeEffectiveness;
    }
    if (PairAbsorbs(ev, actor, target, move, PairMoveType(actor, move)))
        return;
    u32 accuracy = min(100, recalculate ? AI_GetMoveAccuracy(gAiLogicData, actor, target, move) : gAiLogicData->moveAccuracy[actor][target][action->index]);
    enum Ability targetAbility = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor], gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], move);
    u32 skippedStrikes = 0;
    if (PairHasBarrier(actor, target, move, targetAbility))
        damage = PairBreakBarriers(ev, actor, target, move, targetAbility, accuracy, &skippedStrikes);
    bool32 paralyzed = (gBattleMons[actor].status1 & STATUS1_PARALYSIS)
        && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[actor] == ABILITY_MAGIC_GUARD);
    bool32 resolvesHit = accuracy != 0 && (!ev->conservative || foe || IsBattlerAlly(actor, target) || (accuracy >= 100 && !paralyzed));
    enum HoldEffect heldEffect = gAiLogicData->holdEffects[target];
    bool32 healingBetweenHits = (heldEffect == HOLD_EFFECT_RESTORE_HP || heldEffect == HOLD_EFFECT_RESTORE_PCT_HP || heldEffect == HOLD_EFFECT_CONFUSE_FLAVOR)
        && !gBattleMons[target].volatiles.healBlockTimer && !IsUnnerveBlocked(target, gBattleMons[target].item);
    bool32 multihit = GetMoveStrikeCount(move) > 1 || IsMultiHitMove(move) || GetMoveEffect(move) == EFFECT_BEAT_UP
        || gAiLogicData->abilities[actor] == ABILITY_PARENTAL_BOND;
    bool32 intermediateState = multihit && (healingBetweenHits
        || (targetAbility == ABILITY_STAMINA && gBattleMons[target].statStages[STAT_DEF] < MAX_STAT_STAGE));
    u32 amount = 0;
    if (intermediateState)
    {
        // Disguise can itself cross a berry threshold before the next strike.
        if (skippedStrikes && healingBetweenHits)
            PairHealingBerry(ev, target);
        for (u32 strike = skippedStrikes; resolvesHit && strike < 15 && gBattleMons[target].hp; strike++)
        {
            struct AiCalcValues calc = { .move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain,
                .skipStrikes = strike, .strikeLimit = 1 };
            struct SimulatedDamage hit = AI_CalcDamage(&calc, actor, target);
            u32 rawDamage = ev->conservative ? (foe || IsBattlerAlly(actor, target) ? hit.maximum : hit.minimum) : hit.median;
            bool32 consumed = PairDamageConsumedItem(ev, actor, target, &hit);
            if (!rawDamage && !consumed)
                break; // This native roll has no remaining strike.
            if (consumed)
                PairConsumeItem(ev, target);
            u32 hitDamage = min(PairRollDamage(ev, actor, target, hit, accuracy), gBattleMons[target].hp);
            gBattleMons[target].hp -= hitDamage;
            amount += hitDamage;
            gAiLogicData->hpPercents[target] = gBattleMons[target].hp * 100 / max(1, gBattleMons[target].maxHP);
            PairAfterDamage(ev, actor, target, action, hitDamage, calc.typeEffectiveness, 0, TRUE);
        }
    }
    else
    {
        amount = PairRollDamage(ev, actor, target, damage, accuracy);
        if (!ev->conservative && !IsBattlerAlly(actor, target) && gBattleMons[target].hp == ev->board.mons[target].hp && amount < gBattleMons[target].hp)
            ev->utility += (foe ? -1 : 1) * action->koChance[target] * 80 / 1000;
        if (resolvesHit && PairDamageConsumedItem(ev, actor, target, &damage))
            PairConsumeItem(ev, target);
        amount = min(amount, gBattleMons[target].hp);
        gBattleMons[target].hp -= amount;
        gAiLogicData->hpPercents[target] = gBattleMons[target].hp * 100 / max(1, gBattleMons[target].maxHP);
        PairAfterDamage(ev, actor, target, action, amount, effectiveness, skippedStrikes, FALSE);
    }
    if (!gBattleMons[target].hp)
        ev->fieldChanged = TRUE;
    if (GetMoveEffect(move) == EFFECT_RECOIL && gAiLogicData->abilities[actor] != ABILITY_ROCK_HEAD && gAiLogicData->abilities[actor] != ABILITY_MAGIC_GUARD)
        gBattleMons[actor].hp -= min(gBattleMons[actor].hp, max(1, amount * GetMoveRecoil(move) / 100));
    if (move == MOVE_FAKE_OUT && amount && gBattleStruct->battlerState[actor].isFirstTurn && targetAbility != ABILITY_INNER_FOCUS && targetAbility != ABILITY_SHIELD_DUST && gAiLogicData->holdEffects[target] != HOLD_EFFECT_COVERT_CLOAK)
        ev->stopped |= 1u << target;
}

static enum BattlerId PairNextActor(struct PairEvaluation *ev)
{
    enum BattlerId best = MAX_BATTLERS_COUNT;
    if (ev->forceNext < MAX_BATTLERS_COUNT && !(ev->acted & (1u << ev->forceNext)))
    {
        best = ev->forceNext;
        ev->forceNext = MAX_BATTLERS_COUNT;
        return best;
    }
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if ((ev->acted & (1u << actor)) || !gBattleMons[actor].hp)
            ev->roundUsers &= ~(1u << actor);
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        if (ev->roundUsers && !(ev->roundUsers & (1u << actor)))
            continue;
        if ((ev->acted & (1u << actor)) || !gBattleMons[actor].hp || ev->action[actor].index == PAIR_IDLE)
            continue;
        if (best == MAX_BATTLERS_COUNT)
        {
            best = actor;
            continue;
        }
        s32 priority = AI_GetMovePriority(actor, gAiLogicData->abilities[actor], ev->action[actor].move);
        s32 bestPriority = AI_GetMovePriority(best, gAiLogicData->abilities[best], ev->action[best].move);
        u32 speed = !ev->fieldChanged && !(ev->changed & (1u << actor)) ? gAiLogicData->speedStats[actor]
            : GetBattlerTotalSpeedStat(actor, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor]);
        u32 bestSpeed = !ev->fieldChanged && !(ev->changed & (1u << best)) ? gAiLogicData->speedStats[best]
            : GetBattlerTotalSpeedStat(best, gAiLogicData->abilities[best], gAiLogicData->holdEffects[best]);
        bool32 faster = gFieldStatuses & STATUS_FIELD_TRICK_ROOM ? speed < bestSpeed : speed > bestSpeed;
        if (priority > bestPriority || (priority == bestPriority && (faster || (speed == bestSpeed && GetBattlerSide(actor) != ev->side))))
            best = actor;
    }
    return best;
}

static void PairPledgeField(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, enum Move move)
{
    enum BattleSide side = move == MOVE_WATER_PLEDGE ? GetBattlerSide(actor) : GetBattlerSide(target);
    if (move == MOVE_WATER_PLEDGE && !(gSideStatuses[side] & SIDE_STATUS_RAINBOW))
    {
        gSideStatuses[side] |= SIDE_STATUS_RAINBOW;
        gSideTimers[side].rainbowTimer = 4;
    }
    else if (move == MOVE_FIRE_PLEDGE && !(gSideStatuses[side] & SIDE_STATUS_SEA_OF_FIRE))
    {
        gSideStatuses[side] |= SIDE_STATUS_SEA_OF_FIRE;
        gSideTimers[side].seaOfFireTimer = 4;
    }
    else if (move == MOVE_GRASS_PLEDGE && !(gSideStatuses[side] & SIDE_STATUS_SWAMP))
    {
        gSideStatuses[side] |= SIDE_STATUS_SWAMP;
        gSideTimers[side].swampTimer = 4;
    }
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (IsBattlerAlive(battler))
            gAiLogicData->speedStats[battler] = GetBattlerTotalSpeedStat(battler, gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler]);
    ev->fieldChanged = TRUE;
}

static void PairExecute(struct PairEvaluation *ev, enum BattlerId actor, struct PairAction action, bool32 instructed)
{
    enum Move move = action.move;
    enum BattlerId target = action.target;
    enum BattleMoveEffects effect = GetMoveEffect(move);
    enum BattleSide side = GetBattlerSide(actor);
    enum BattlerId partner = GetPartnerBattler(actor);
    u16 priorHp[MAX_BATTLERS_COUNT], priorSub[MAX_BATTLERS_COUNT];
    enum Species priorSpecies[MAX_BATTLERS_COUNT];
    bool32 combinedPledge = FALSE;
    if (action.index == PAIR_IDLE || !gBattleMons[actor].hp || (ev->stopped & (1u << actor)))
        return;
    if (action.score == 0 && !PairSupport(move) && (target == actor || !IsBattlerAlly(actor, target)))
        return;
    if (gBattleMons[actor].status1 & STATUS1_SLEEP)
    {
        u32 decrement = gAiLogicData->abilities[actor] == ABILITY_EARLY_BIRD ? 2 : 1;
        if ((gBattleMons[actor].status1 & STATUS1_SLEEP) <= decrement)
            gBattleMons[actor].status1 &= ~STATUS1_SLEEP;
        else if (!IsUsableWhileAsleepEffect(effect))
            return;
    }
    if (gBattleMons[actor].status1 & STATUS1_FREEZE)
    {
        if (MoveThawsUser(move) || (GetConfig(B_FREEZE_TURNS) >= GEN_CHAMPIONS && GetBattlerPartyState(actor)->freezeTurns >= 2))
            gBattleMons[actor].status1 &= ~STATUS1_FREEZE;
        else if (ev->conservative && side == ev->side)
            return;
    }
    if (gBattleMons[actor].volatiles.rechargeTimer || (gAiLogicData->abilities[actor] == ABILITY_TRUANT && gBattleMons[actor].volatiles.truantCounter))
        return;
    if (gBattleMoveEffects[effect].twoTurnEffect && !gBattleMons[actor].volatiles.multipleTurns)
    {
        if (gAiLogicData->holdEffects[actor] == HOLD_EFFECT_POWER_HERB && effect != EFFECT_SKY_DROP)
            PairConsumeItem(ev, actor);
        else if (IsTwoTurnNotSemiInvulnerableMove(actor, move) || effect == EFFECT_SEMI_INVULNERABLE || effect == EFFECT_SKY_DROP)
        {
            if (effect == EFFECT_SEMI_INVULNERABLE || effect == EFFECT_SKY_DROP)
                gBattleMons[actor].volatiles.semiInvulnerable = GetTwoTurnMoveSemiInvulnerability(move);
            ev->changed |= 1u << actor;
            return;
        }
    }
    ev->executed |= 1u << actor;
    gBattlerAttacker = actor;
    gBattlerTarget = target;
    gCurrentMove = move;
    gChosenMove = action.move;
    gCurrMovePos = gChosenMovePos = action.index;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        priorHp[battler] = gBattleMons[battler].hp;
        priorSub[battler] = gBattleMons[battler].volatiles.substituteHP;
        priorSpecies[battler] = gBattleMons[battler].species;
    }
    if (!PairSupport(move) && (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS || !IsBattlerAlly(actor, target)))
        ev->utility += (side == ev->side ? 1 : -1) * max(-100, action.score - AI_SCORE_DEFAULT) * 2;
    switch (effect)
    {
    case EFFECT_PLEDGE:
        if (ev->pledgeMove[side] != MOVE_NONE && ev->pledgeUser[side] == partner)
        {
            enum Move first = ev->pledgeMove[side];
            move = GetPledgeComboMove(move) == first ? GetPledgeResultMove(move) : GetPledgeResultMove(first);
            action.move = gCurrentMove = move;
            ev->pledgeMove[side] = MOVE_NONE;
            gBattleStruct->pledgeState = PLEDGE_COMBO_ATTACK;
            combinedPledge = TRUE;
            ev->changed |= 1u << actor;
        }
        else if (IsBattlerAlive(partner) && !(ev->acted & (1u << partner))
            && ev->action[partner].index != PAIR_IDLE && GetMoveEffect(ev->action[partner].move) == EFFECT_PLEDGE
            && move != ev->action[partner].move
            && (GetPledgeComboMove(move) == ev->action[partner].move || GetPledgeComboMove(ev->action[partner].move) == move))
        {
            ev->pledgeMove[side] = move;
            ev->pledgeUser[side] = actor;
            gBattleStruct->pledgeState = PLEDGE_COMBO_WAITING;
            ev->forceNext = partner;
            return;
        }
        break;
    case EFFECT_ROUND:
    case EFFECT_FUSION_COMBO:
        // Native power reads the preceding executed action in this trial.
        ev->changed |= 1u << actor;
        break;
    case EFFECT_WISH:
        if (gBattleStruct->wish[actor].counter == 0)
        {
            gBattleStruct->wish[actor].counter = 2;
            gBattleStruct->wish[actor].partyId = gBattlerPartyIndexes[actor];
        }
        return;
    case EFFECT_PROTECT:
    case EFFECT_MAT_BLOCK:
    {
        enum ProtectMethod method = GetMoveProtectMethod(move);
        if (method == PROTECT_WIDE_GUARD)
            ev->wideGuard |= 1u << side;
        else if (method == PROTECT_QUICK_GUARD)
            ev->quickGuard |= 1u << side;
        else if (gBattleMons[actor].volatiles.consecutiveMoveUses == 0)
            ev->protected |= 1u << actor;
        if (method == PROTECT_WIDE_GUARD || method == PROTECT_QUICK_GUARD || gBattleMons[actor].volatiles.consecutiveMoveUses == 0)
            gProtectStructs[actor].protected = method;
        ev->changed |= 1u << actor;
        return;
    }
    case EFFECT_HELPING_HAND:
        if (!(ev->acted & (1u << partner)) && ev->action[partner].index != PAIR_IDLE && GetMoveCategory(ev->action[partner].move) != DAMAGE_CATEGORY_STATUS)
        {
            gProtectStructs[partner].helpingHand++;
            ev->changed |= 1u << partner;
        }
        return;
    case EFFECT_FOLLOW_ME:
        ev->redirect[side] = actor;
        gSideTimers[side].followmeTimer = 1;
        gSideTimers[side].followmeTarget = actor;
        gSideTimers[side].followmePowder = move == MOVE_RAGE_POWDER;
        return;
    case EFFECT_AFTER_YOU:
        if (IsBattlerAlly(actor, target) && target != actor && !(ev->acted & (1u << target)))
            ev->forceNext = target;
        return;
    case EFFECT_INSTRUCT:
        if (!instructed && target != actor && IsBattlerAlly(actor, target) && gBattleMons[target].hp)
        {
            struct PairAction repeat = ev->action[target];
            if (!(ev->acted & (1u << target)))
            {
                repeat.index = PAIR_IDLE;
                for (u32 index = 0; index < MAX_MON_MOVES; index++)
                    if (gBattleMons[target].moves[index] == gLastMoves[target] && gBattleMons[target].pp[index])
                        repeat = (struct PairAction){gLastMoves[target], AI_SCORE_DEFAULT, index, GetOppositeBattler(target)};
            }
            if (repeat.index != PAIR_IDLE && !IsMoveInstructBanned(repeat.move) && !gBattleMons[target].volatiles.rechargeTimer)
                PairExecute(ev, target, repeat, TRUE);
        }
        return;
    case EFFECT_TRICK_ROOM:
        gFieldStatuses ^= STATUS_FIELD_TRICK_ROOM;
        gFieldTimers.trickRoomTimer = gFieldStatuses & STATUS_FIELD_TRICK_ROOM ? 5 : 0;
        ev->fieldChanged = TRUE;
        return;
    case EFFECT_TAILWIND:
        gSideStatuses[side] |= SIDE_STATUS_TAILWIND;
        gSideTimers[side].tailwindTimer = 4;
        ev->fieldChanged = TRUE;
        return;
    case EFFECT_REFLECT:
        gSideStatuses[side] |= SIDE_STATUS_REFLECT;
        gSideTimers[side].reflectTimer = 5;
        ev->fieldChanged = TRUE;
        return;
    case EFFECT_LIGHT_SCREEN:
        gSideStatuses[side] |= SIDE_STATUS_LIGHTSCREEN;
        gSideTimers[side].lightscreenTimer = 5;
        ev->fieldChanged = TRUE;
        return;
    case EFFECT_AURORA_VEIL:
        if (AI_GetWeather() & B_WEATHER_ICY_ANY)
        {
            gSideStatuses[side] |= SIDE_STATUS_AURORA_VEIL;
            gSideTimers[side].auroraVeilTimer = 5;
            ev->fieldChanged = TRUE;
        }
        return;
    case EFFECT_WEATHER:
    case EFFECT_WEATHER_AND_SWITCH:
        TryChangeBattleWeather(actor, GetMoveWeatherType(move), ABILITY_NONE);
        AI_RefreshCandidateFieldEffects();
        ev->fieldChanged = TRUE;
        return;
    case EFFECT_TERRAIN:
        TryChangeBattleTerrain(actor, GetMoveTerrainType(move));
        AI_RefreshCandidateFieldEffects();
        ev->fieldChanged = TRUE;
        return;
    default:
        break;
    }
    if (GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS)
    {
        if (effect == EFFECT_BELLY_DRUM)
        {
            u32 cost = max(1, gBattleMons[actor].maxHP / 2);
            if (gBattleMons[actor].hp <= cost)
                return;
            gBattleMons[actor].hp -= cost;
            PairRaiseStat(ev, actor, STAT_ATK, MAX_STAT_STAGE);
            PairHealingBerry(ev, actor);
        }
        else if (IsHealingMove(move))
        {
            PairHeal(target, max(1, gBattleMons[target].maxHP / 2));
            ev->changed |= 1u << target;
        }
        else if (GetMoveNonVolatileStatus(move) == MOVE_EFFECT_SLEEP && !IsBattlerAlly(actor, target)
              && AI_CanPutToSleep(actor, target, gAiLogicData->abilities[target], move, ev->action[partner].move))
        {
            u32 accuracy = gAiLogicData->moveAccuracy[actor][target][action.index];
            if ((!ev->conservative || side != ev->side || accuracy >= 100) && !(gBattleMons[target].status1) && !(gFieldTimers.terrain == B_TERRAIN_ELECTRIC && AI_IsBattlerGrounded(target)))
                ev->stopped |= 1u << target;
        }
        else
            PairStatEffects(ev, actor, target, move, FALSE);
        return;
    }
    if (PairSpread(move))
    {
        for (target = 0; target < gBattlersCount; target++)
            if (target != actor && (GetMoveTarget(move) != TARGET_BOTH || !IsBattlerAlly(actor, target)))
                PairHit(ev, actor, target, &action);
    }
    else
    {
        if (!IsBattlerAlly(actor, target))
        {
            enum BattlerId redirect = ev->redirect[GetBattlerSide(target)];
            if (redirect < MAX_BATTLERS_COUNT && gBattleMons[redirect].hp && IsAffectedByFollowMe(actor, GetBattlerSide(target), move)
             && (ev->action[redirect].move != MOVE_RAGE_POWDER || IsAffectedByPowderMove(actor, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor])))
                target = redirect;
            if (!gBattleMons[target].hp && gBattleMons[GetPartnerBattler(target)].hp)
                target = GetPartnerBattler(target);
        }
        PairHit(ev, actor, target, &action);
    }
    bool32 hit = FALSE;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (battler != actor && (gBattleMons[battler].hp < priorHp[battler]
            || gBattleMons[battler].volatiles.substituteHP < priorSub[battler]
            || gBattleMons[battler].species != priorSpecies[battler]))
            hit = TRUE;
    if (combinedPledge)
    {
        if (hit)
            PairPledgeField(ev, actor, target, move);
        gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;
    }
    if (effect == EFFECT_ROUND && hit)
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
            if (!(ev->acted & (1u << battler)) && ev->action[battler].move == MOVE_ROUND && IsBattlerAlive(battler))
                ev->roundUsers |= 1u << battler;
    if (!hit && (effect == EFFECT_ROUND || effect == EFFECT_FUSION_COMBO))
        ev->executed &= ~(1u << actor);
    if (IsExplosionMove(move))
        gBattleMons[actor].hp = 0;
    else
        PairStatEffects(ev, actor, action.target, move, TRUE);
}

static s32 PairPositionValue(const struct PairBoard *board, enum BattleSide side, bool32 *terminal)
{
    s32 value[NUM_BATTLE_SIDES] = {board->reserveValue[0], board->reserveValue[1]};
    u8 living[MAX_BATTLE_TRAINERS];
    memcpy(living, board->reserveLiving, sizeof(living));
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        if (!(board->activeMask & (1u << actor)) || !gBattleMons[actor].hp)
            continue;
        living[board->owner[actor]]++;
        if (board->scoredActiveMask & (1u << actor))
            value[board->side[actor]] += 80 + gBattleMons[actor].hp * 100 / max(1, gBattleMons[actor].maxHP);
    }
    bool32 defeated[NUM_BATTLE_SIDES];
    defeated[B_SIDE_PLAYER] = !living[B_TRAINER_PLAYER]
        && (!board->playerCanUsePartner || !living[B_TRAINER_PARTNER]);
    defeated[B_SIDE_OPPONENT] = !living[B_TRAINER_OPPONENT_A]
        && (!board->opponentHasPartner || !living[B_TRAINER_OPPONENT_B]);
    *terminal = defeated[B_SIDE_PLAYER] || defeated[B_SIDE_OPPONENT];
    if (defeated[side] && defeated[side ^ BIT_SIDE])
        return 0;
    if (defeated[side])
        return -10000;
    if (defeated[side ^ BIT_SIDE])
        return 10000;
    return value[side] - value[side ^ BIT_SIDE];
}

static void SetPairActionContext(struct PairEvaluation *ev, bool32 rescore)
{
    gAiLogicData->partnerMoveSimulation = FALSE;
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        gAiBattleData->chosenMoveIndex[actor] = ev->action[actor].index;
        gAiBattleData->chosenTarget[actor] = ev->action[actor].target;
        gChosenMoveByBattler[actor] = ev->action[actor].move;
        gChosenActionByBattler[actor] = ev->action[actor].index == PAIR_IDLE ? B_ACTION_FINISHED : B_ACTION_USE_MOVE;
        gBattleStruct->chosenMovePositions[actor] = ev->action[actor].index == PAIR_IDLE ? 0 : ev->action[actor].index;
        gAiLogicData->predictedMove[actor] = ev->action[actor].move;
        gAiLogicData->battlerMovesScored |= 1u << actor;
    }
    if (!rescore)
        return;
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        struct PairAction *action = &ev->action[actor];
        if (action->index == PAIR_IDLE || !IsBattlerAlive(actor))
            continue;
        enum BattleMoveEffects effect = GetMoveEffect(action->move);
        if (!PairSupport(action->move) && effect != EFFECT_ENTRAINMENT && action->move != MOVE_DECORATE
            && effect != EFFECT_PLEDGE && effect != EFFECT_ROUND && effect != EFFECT_FUSION_COMBO)
            continue;
        enum BattlerId target = action->target == actor ? GetOppositeBattler(actor) : action->target;
        if (!IsBattlerAlive(target))
            target = GetPartnerBattler(target);
        gAiLogicData->partnerMove = ev->action[GetPartnerBattler(actor)].move;
        action->score = AI_ScoreMoveAgainstTarget(actor, target, action->index);
    }
}

static void SetPairNativeTurnOrder(struct PairEvaluation *ev, enum BattlerId actor, u32 turn)
{
    gCurrentTurnActionNumber = turn;
    gBattlerByTurnOrder[turn] = actor;
    gActionsByTurnOrder[turn] = B_ACTION_USE_MOVE;
    u32 next = turn + 1;
    for (enum BattlerId pending = 0; pending < gBattlersCount; pending++)
        if (pending != actor && !(ev->acted & (1u << pending)))
        {
            gBattlerByTurnOrder[next] = pending;
            gActionsByTurnOrder[next++] = ev->action[pending].index == PAIR_IDLE ? B_ACTION_FINISHED : B_ACTION_USE_MOVE;
        }
}

static void PairResidualDamage(struct PairEvaluation *ev, enum BattlerId actor, u32 amount)
{
    if (!amount || !IsBattlerAlive(actor) || gAiLogicData->abilities[actor] == ABILITY_MAGIC_GUARD)
        return;
    gBattleMons[actor].hp -= min(amount, gBattleMons[actor].hp);
    PairHealingBerry(ev, actor);
}

static bool32 PairEndTurn(struct PairEvaluation *ev, s32 *terminalValue)
{
    bool32 hasEffects = gBattleWeather != B_WEATHER_NONE || gBattleStruct->weatherDuration == 1
        || gFieldTimers.terrain == B_TERRAIN_GRASSY
        || ((gSideStatuses[0] | gSideStatuses[1]) & SIDE_STATUS_SEA_OF_FIRE);
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        if (gBattleStruct->wish[actor].counter)
            hasEffects = TRUE;
        if (!IsBattlerAlive(actor))
            continue;
        enum HoldEffect item = gAiLogicData->holdEffects[actor];
        struct Volatiles *vol = &gBattleMons[actor].volatiles;
        if ((gBattleMons[actor].status1 & (STATUS1_PSN_ANY | STATUS1_BURN | STATUS1_FROSTBITE))
            || vol->aquaRing || vol->root || vol->leechSeed || vol->nightmare || vol->cursed
            || (vol->wrapped && vol->wrapTurns) || vol->perishSong
            || item == HOLD_EFFECT_LEFTOVERS || item == HOLD_EFFECT_BLACK_SLUDGE)
            hasEffects = TRUE;
    }
    if (!hasEffects)
    {
        if (gBattleStruct->weatherDuration)
            gBattleStruct->weatherDuration--;
        goto UpdatePledgeTimers;
    }
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        gBattlerByTurnOrder[actor] = actor;
    SortBattlersBySpeed(gBattlerByTurnOrder, FALSE);
    if (gBattleStruct->weatherDuration && --gBattleStruct->weatherDuration == 0)
    {
        gBattleWeather = B_WEATHER_NONE;
        for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
            gBattleMons[actor].volatiles.weatherAbilityDone = FALSE;
        AI_RefreshCandidateFieldEffects();
    }
    u32 actorMaxHp[MAX_BATTLERS_COUNT] = {0};
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if (IsBattlerAlive(actor))
            actorMaxHp[actor] = GetNonDynamaxMaxHP(actor);
    for (u32 phase = 0; phase < 11; phase++)
        for (u32 index = 0; index < gBattlersCount; index++)
        {
            enum BattlerId actor = gBattlerByTurnOrder[index];
            bool32 wasAlive = IsBattlerAlive(actor);
            u32 maxHp = actorMaxHp[actor];
            enum Ability ability = gAiLogicData->abilities[actor];
            if (phase == 1)
            {
                if (gBattleStruct->wish[actor].counter && --gBattleStruct->wish[actor].counter == 0 && wasAlive)
                {
                    u32 amount = GetConfig(B_WISH_HP_SOURCE) >= GEN_5
                        ? GetMonData(&GetBattlerParty(actor)[gBattleStruct->wish[actor].partyId], MON_DATA_MAX_HP) / 2 : maxHp / 2;
                    PairHeal(actor, max(1, amount));
                }
                continue;
            }
            if (!wasAlive)
                continue;
            switch (phase)
            {
            case 0: // Weather precedes the Wish phase.
            {
                if (ability != ABILITY_MAGIC_GUARD && (gBattleWeather & B_WEATHER_DAMAGING_ANY))
                    PairResidualDamage(ev, actor, GetWeatherDamage(actor));
                if ((gBattleWeather & B_WEATHER_SUN && (ability == ABILITY_DRY_SKIN || ability == ABILITY_SOLAR_POWER))
                    || (gBattleWeather & B_WEATHER_RAIN && (ability == ABILITY_DRY_SKIN || ability == ABILITY_RAIN_DISH))
                    || (gBattleWeather & B_WEATHER_ICY_ANY && ability == ABILITY_ICE_BODY))
                {
                    u32 weather = GetAttackerWeather(gAiLogicData->holdEffects[actor], ability, AI_GetWeather());
                    if ((weather & B_WEATHER_SUN) && (ability == ABILITY_DRY_SKIN || ability == ABILITY_SOLAR_POWER))
                        PairResidualDamage(ev, actor, max(1, maxHp / 8));
                    if ((weather & B_WEATHER_RAIN) && (ability == ABILITY_DRY_SKIN || ability == ABILITY_RAIN_DISH))
                        PairHeal(actor, max(1, maxHp / (ability == ABILITY_DRY_SKIN ? 8 : 16)));
                    if ((weather & B_WEATHER_ICY_ANY) && ability == ABILITY_ICE_BODY)
                        PairHeal(actor, max(1, maxHp / 16));
                }
                break;
            }
            case 2:
            {
                if ((gSideStatuses[GetBattlerSide(actor)] & SIDE_STATUS_SEA_OF_FIRE) && !IS_BATTLER_OF_TYPE(actor, TYPE_FIRE))
                    PairResidualDamage(ev, actor, max(1, maxHp / 8));
                if (gFieldTimers.terrain == B_TERRAIN_GRASSY && !IsSemiInvulnerable(actor, CHECK_ALL) && AI_IsBattlerGrounded(actor))
                    PairHeal(actor, max(1, maxHp / 16));
                if (ability == ABILITY_HYDRATION && gBattleMons[actor].status1 && (gBattleWeather & B_WEATHER_RAIN)
                    && (GetAttackerWeather(gAiLogicData->holdEffects[actor], ability, AI_GetWeather()) & B_WEATHER_RAIN))
                    gBattleMons[actor].status1 = 0;
                enum HoldEffect item = gAiLogicData->holdEffects[actor];
                if (item == HOLD_EFFECT_LEFTOVERS || (item == HOLD_EFFECT_BLACK_SLUDGE && IS_BATTLER_OF_TYPE(actor, TYPE_POISON)))
                    PairHeal(actor, max(1, maxHp / 16));
                else if (item == HOLD_EFFECT_BLACK_SLUDGE)
                    PairResidualDamage(ev, actor, max(1, maxHp / 8));
                break;
            }
            case 3:
                if (gBattleMons[actor].volatiles.aquaRing)
                    PairHeal(actor, max(1, GetDrainedBigRootHp(actor, maxHp / 16)));
                if (gBattleMons[actor].volatiles.root)
                    PairHeal(actor, max(1, GetDrainedBigRootHp(actor, maxHp / 16)));
                break;
            case 4:
            {
                u32 damage = ability != ABILITY_MAGIC_GUARD && gBattleMons[actor].volatiles.leechSeed
                    ? GetLeechSeedDamage(actor) : 0;
                PairResidualDamage(ev, actor, damage);
                if (damage)
                {
                    enum BattlerId recipient = gBattleMons[actor].volatiles.leechSeed - 1;
                    u32 amount = max(1, GetDrainedBigRootHp(recipient, damage));
                    if (ability == ABILITY_LIQUID_OOZE)
                        PairResidualDamage(ev, recipient, amount);
                    else
                        PairHeal(recipient, amount);
                }
                break;
            }
            case 5:
                if (gBattleMons[actor].status1 & STATUS1_PSN_ANY)
                {
                    if (ability == ABILITY_POISON_HEAL)
                        PairHeal(actor, max(1, maxHp / 8));
                    else if (ability != ABILITY_MAGIC_GUARD)
                        PairResidualDamage(ev, actor, GetPoisonDamage(actor));
                }
                break;
            case 6:
                if (ability != ABILITY_MAGIC_GUARD && (gBattleMons[actor].status1 & (STATUS1_BURN | STATUS1_FROSTBITE)))
                {
                    u32 amount = maxHp / ((GetConfig(B_BURN_DAMAGE) >= GEN_7 || GetConfig(B_BURN_DAMAGE) == GEN_1) ? 16 : 8);
                    if ((gBattleMons[actor].status1 & STATUS1_BURN) && ability == ABILITY_HEATPROOF)
                        amount /= 2;
                    PairResidualDamage(ev, actor, max(1, amount));
                }
                break;
            case 7:
                if (ability != ABILITY_MAGIC_GUARD && gBattleMons[actor].volatiles.nightmare)
                    PairResidualDamage(ev, actor, GetNightmareDamage(actor));
                break;
            case 8:
                if (ability != ABILITY_MAGIC_GUARD && gBattleMons[actor].volatiles.cursed)
                    PairResidualDamage(ev, actor, GetCurseDamage(actor));
                break;
            case 9:
                if (ability != ABILITY_MAGIC_GUARD && gBattleMons[actor].volatiles.wrapped && gBattleMons[actor].volatiles.wrapTurns)
                    PairResidualDamage(ev, actor, GetTrapDamage(actor));
                break;
            case 10:
                if (gBattleMons[actor].volatiles.perishSong)
                {
                    if (gBattleMons[actor].volatiles.perishSongTimer == 0)
                        gBattleMons[actor].hp = 0;
                    else
                        gBattleMons[actor].volatiles.perishSongTimer--;
                }
                break;
            }
            if (!gBattleMons[actor].hp)
            {
                bool32 terminal;
                *terminalValue = PairPositionValue(&ev->board, ev->side, &terminal);
                if (terminal)
                    return TRUE;
            }
        }
UpdatePledgeTimers:
    for (enum BattleSide side = 0; side < NUM_BATTLE_SIDES; side++)
    {
        if (gSideTimers[side].rainbowTimer && --gSideTimers[side].rainbowTimer == 0)
            gSideStatuses[side] &= ~SIDE_STATUS_RAINBOW;
        if (gSideTimers[side].seaOfFireTimer && --gSideTimers[side].seaOfFireTimer == 0)
            gSideStatuses[side] &= ~SIDE_STATUS_SEA_OF_FIRE;
        if (gSideTimers[side].swampTimer && --gSideTimers[side].swampTimer == 0)
            gSideStatuses[side] &= ~SIDE_STATUS_SWAMP;
    }
    return FALSE;
}

static s32 RunPairTurn(struct PairEvaluation *ev, bool32 conservative, bool32 rescore)
{
    RestorePairBoard(&ev->board);
    SetPairActionContext(ev, rescore);
    ev->acted = ev->stopped = ev->changed = ev->protected = ev->wideGuard = ev->quickGuard = 0;
    ev->redirect[0] = ev->redirect[1] = ev->forceNext = MAX_BATTLERS_COUNT;
    ev->fieldChanged = FALSE;
    ev->conservative = conservative;
    ev->utility = 0;
    ev->roundUsers = ev->executed = 0;
    memset(ev->pledgeMove, 0, sizeof(ev->pledgeMove));
    gBattleStruct->pledgeState = PLEDGE_COMBO_NONE;
    gLastUsedMove = MOVE_NONE;
    for (u32 turn = 0; turn < gBattlersCount; turn++)
    {
        enum BattlerId actor = PairNextActor(ev);
        if (actor == MAX_BATTLERS_COUNT)
            break;
        u32 livingBefore = 0;
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
            if (IsBattlerAlive(battler))
                livingBefore |= 1u << battler;
        SetPairNativeTurnOrder(ev, actor, turn);
        ev->acted |= 1u << actor;
        PairExecute(ev, actor, ev->action[actor], FALSE);
        if (ev->executed & (1u << actor))
            gLastUsedMove = gCurrentMove;
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
            if ((livingBefore & (1u << battler)) && !IsBattlerAlive(battler))
            {
                bool32 terminal;
                s32 value = PairPositionValue(&ev->board, ev->side, &terminal);
                if (terminal)
                    return value;
                break;
            }
    }
    s32 value;
    if (PairEndTurn(ev, &value))
        return value;
    bool32 terminal;
    value = PairPositionValue(&ev->board, ev->side, &terminal);
    return terminal ? value : value + ev->utility;
}

static s32 CheckPairTerminalResponses(struct PairEvaluation *ev, s32 value)
{
    struct PairAction original[MAX_BATTLERS_COUNT];
    memcpy(original, ev->action, sizeof(original));
    // Apparent terminal wins face every individual legal alternative,
    // including Protect and faster priority. This is still a one-turn bound.
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (GetBattlerSide(foe) == ev->side)
            continue;
        for (u32 choice = 0; choice < ev->count[foe]; choice++)
        {
            if (ev->choices[foe][choice].index == original[foe].index
                && ev->choices[foe][choice].target == original[foe].target)
                continue;
            memcpy(ev->action, original, sizeof(original));
            ev->action[foe] = ev->choices[foe][choice];
            value = min(value, RunPairTurn(ev, TRUE, TRUE));
            if (value <= -10000)
                break;
        }
    }
    // Also examine the two coordinated focus-fire responses. A different
    // greedy target on each foe must not conceal their combined knockout.
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (GetBattlerSide(target) != ev->side)
            continue;
        RestorePairBoard(&ev->board);
        if (!IsBattlerAlive(target))
            continue;
        memcpy(ev->action, original, sizeof(original));
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (GetBattlerSide(foe) == ev->side)
                continue;
            s32 best = INT_MIN;
            for (u32 choice = 0; choice < ev->count[foe]; choice++)
            {
                struct PairAction *action = &ev->choices[foe][choice];
                if (action->index == PAIR_IDLE || IsBattleMoveStatus(action->move)
                    || (action->target != target && !PairSpread(action->move)))
                    continue;
                s32 damage = gAiLogicData->simulatedDmg[foe][target][action->index].maximum;
                s32 score = damage + (AI_IsFaster(foe, target, action->move, original[target].move, CONSIDER_PRIORITY) ? gBattleMons[target].hp : 0);
                if (score > best)
                {
                    best = score;
                    ev->action[foe] = *action;
                }
            }
        }
        value = min(value, RunPairTurn(ev, TRUE, TRUE));
    }
    memcpy(ev->action, original, sizeof(original));
    return value;
}

static void RefreshPairMoveData(u32 noActionMask)
{
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if (IsBattlerAlive(actor))
            SetBattlerAiData(actor, gAiLogicData);
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        if (noActionMask & (1u << actor))
        {
            memset(gAiLogicData->simulatedDmg[actor], 0, sizeof(gAiLogicData->simulatedDmg[actor]));
            memset(gAiLogicData->effectiveness[actor], 0, sizeof(gAiLogicData->effectiveness[actor]));
            memset(gAiLogicData->moveAccuracy[actor], 0, sizeof(gAiLogicData->moveAccuracy[actor]));
            memset(gAiLogicData->resistBerryAffected[actor], 0, sizeof(gAiLogicData->resistBerryAffected[actor]));
            continue;
        }
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
            if (actor != target && IsBattlerAlive(actor) && IsBattlerAlive(target))
                CalcBattlerAiMovesData(gAiLogicData, actor, target, AI_GetWeather(), gFieldTimers.terrain);
    }
}

static s32 EvaluatePairBoard(enum BattlerId actor, u32 noActionMask, struct PairAction *chosen, struct PairEvaluation *ev)
{
    struct SwitchCandidateSnapshot *state = ev->board.state;
    memset(ev, 0, sizeof(*ev));
    ev->board.state = state;
    SavePairBoard(&ev->board);
    RefreshPairMoveData(noActionMask);
    enum BattlerId partner = GetPartnerBattler(actor);
    ev->side = GetBattlerSide(actor);
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        BuildPairActions(ev, battler, noActionMask);
    // Forecast from available moves and the observed board. The human's
    // unexecuted command and target buffers are never selection inputs.
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (IsBattlerAlly(actor, foe))
            continue;
        s32 best = INT_MIN;
        for (u32 index = 0; index < ev->count[foe]; index++)
        {
            s32 score = PairActionForecast(foe, &ev->choices[foe][index]);
            if (score > best)
            {
                best = score;
                ev->action[foe] = ev->choices[foe][index];
            }
        }
    }
    ev->board.logic = *gAiLogicData;
    s32 best = INT_MIN;
    for (u32 left = 0; left < ev->count[actor]; left++)
    {
        for (u32 right = 0; right < ev->count[partner]; right++)
        {
            ev->action[actor] = ev->choices[actor][left];
            ev->action[partner] = ev->choices[partner][right];
            s32 expected = RunPairTurn(ev, FALSE, TRUE);
            s32 lowRoll = RunPairTurn(ev, TRUE, FALSE);
            if (lowRoll >= 10000)
                lowRoll = CheckPairTerminalResponses(ev, lowRoll);
            // Outcomes are forecasts over the bounded response cases, not
            // access to a committed foe action or a proof about later turns.
            s32 score = lowRoll >= 10000 ? 10000 : (min(1000, max(-1000, expected)) * 3 + lowRoll) / 4;
            if (score > best)
            {
                best = score;
                if (chosen != NULL)
                {
                    chosen[0] = ev->action[actor];
                    chosen[1] = ev->action[partner];
                }
            }
        }
    }
    AI_RestoreCandidateState(ev->board.state);
    return best;
}

s32 AI_EvaluateDoublesPosition(enum BattlerId battler, u32 noActionMask)
{
    struct PairEvaluation *ev = AllocZeroed(sizeof(*ev));
    ev->board.state = AI_SaveCandidateState();
    s32 score = EvaluatePairBoard(battler, noActionMask, NULL, ev);
    AI_FreeCandidateState(ev->board.state);
    Free(ev);
    return score;
}

static bool32 PairCanSwitch(enum BattlerId actor)
{
    if (!IsBattlerAlive(actor) || (gBattleTypeFlags & BATTLE_TYPE_ARENA)
        || gBattleStruct->battlerState[actor].commanderSpecies != SPECIES_NONE
        || gBattleMons[actor].volatiles.semiInvulnerable == STATE_COMMANDER
        || (gAiThinkingStruct->aiFlags[actor] & AI_FLAG_SEQUENCE_SWITCHING)
        || (!CanBattlerEscape(actor) && GetBattlerHoldEffect(actor) != HOLD_EFFECT_SHED_SHELL)
        || (GetItemHoldEffect(gBattleMons[actor].item) != HOLD_EFFECT_SHED_SHELL && IsAbilityPreventingEscape(actor)))
        return FALSE;
    return CanBattlerSwitch(actor);
}

static bool32 PairLegalReserve(enum BattlerId actor, u32 slot)
{
    enum BattlerId first, second;
    GetActiveBattlerIds(actor, &first, &second);
    return slot < GetAILastPartyIndex(actor) && IsValidForBattle(&GetBattlerParty(actor)[slot])
        && !IsPartyMonOnFieldOrChosenToSwitch(actor, slot, first, second)
        && !IsPartyMonPlannedToBeSwitchedInByPartner(slot, actor);
}

// A reserve is not acting this turn. Estimate its future Mega opportunity
// from native move forecasts; the bounded preference does not warrant a
// second exhaustive turn search for each hypothetical future arrival.
static s32 PairMegaOpportunityValue(enum BattlerId actor)
{
    RefreshPairMoveData(0);
    s32 value = 0;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlive(battler))
            continue;
        s32 best = 0;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = gBattleMons[battler].moves[index];
            if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[battler]))
                continue;
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
            {
                if (!PairTargetIsLegal(battler, target, move))
                    continue;
                struct PairAction action = {move, AI_SCORE_DEFAULT, index, target};
                s32 forecast = PairActionForecast(battler, &action);
                if (target != battler)
                    forecast = forecast * min(100, gAiLogicData->moveAccuracy[battler][target][index]) / 100;
                best = max(best, forecast);
            }
        }
        value += IsBattlerAlly(actor, battler) ? best : -best;
    }
    return value;
}

static u32 PairReserveMegaCost(enum BattlerId actor, const u8 *reserves, u32 count,
    const struct SwitchCandidateSnapshot *initial)
{
    s32 bestGain = 0;
    if (HasTrainerUsedGimmick(actor, GIMMICK_MEGA))
        return 0;
    for (u32 index = 1; index < count; index++)
    {
        AI_RestoreCandidateState(initial);
        AI_LoadSwitchCandidate(actor, reserves[index], FALSE);
        if (!IsBattlerAlive(actor) || !CanMegaEvolve(actor))
            continue;
        s32 base = PairMegaOpportunityValue(actor);
        if (!AI_ApplyMegaCandidate(actor, FALSE))
            continue;
        s32 mega = PairMegaOpportunityValue(actor);
        bestGain = max(bestGain, mega - base);
    }
    AI_RestoreCandidateState(initial);
    return bestGain == 0 ? 0 : min(5, 1 + bestGain / 20);
}

static bool32 PairPreservesStevenMetagross(enum BattlerId actor)
{
    if (GetBattlerSide(actor) != B_SIDE_OPPONENT || gBattleMons[actor].species != SPECIES_RAYQUAZA)
        return FALSE;
    u32 trainerId = GetBattlerTrainer(actor) == B_TRAINER_OPPONENT_B ? TRAINER_BATTLE_PARAM.opponentB : TRAINER_BATTLE_PARAM.opponentA;
    if (trainerId != TRAINER_STEVEN)
        return FALSE;
    struct Pokemon *party = GetBattlerParty(actor);
    for (u32 slot = 0; slot < GetAILastPartyIndex(actor); slot++)
        if (GetMonData(&party[slot], MON_DATA_SPECIES) == SPECIES_METAGROSS
            && GetMonData(&party[slot], MON_DATA_HP) != 0
            && GetMonData(&party[slot], MON_DATA_HELD_ITEM) == ITEM_METAGROSSITE)
            return TRUE;
    return FALSE;
}

bool32 AI_ComputeDoublesDecisions(enum BattlerId actor)
{
    enum BattlerId partner = GetPartnerBattler(actor);
    if (!IsDoubleBattle() || !IsBattlerAlive(actor) || !IsBattlerAlive(partner) || !BattlerHasAi(partner)
        || gAiLogicData->aiPredictionInProgress
        || !(gAiThinkingStruct->aiFlags[actor] & AI_FLAG_SMART_MON_CHOICES)
        || !(gAiThinkingStruct->aiFlags[partner] & AI_FLAG_SMART_MON_CHOICES))
        return FALSE;
    if (gAiLogicData->battlerMovesScored & (1u << actor))
        return TRUE;
    struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
    struct PairEvaluation *ev = AllocZeroed(sizeof(*ev));
    ev->board.state = AI_SaveCandidateState();
    u8 reserves[2][PARTY_SIZE + 1] = {{PARTY_SIZE}, {PARTY_SIZE}};
    u32 count[2] = {1, 1};
    enum BattlerId actors[2] = {actor, partner};
    for (u32 index = 0; index < 2; index++)
        if (PairCanSwitch(actors[index]))
            for (u32 slot = 0; slot < GetAILastPartyIndex(actors[index]); slot++)
                if (PairLegalReserve(actors[index], slot))
                    reserves[index][count[index]++] = slot;
    u32 reserveCost[2];
    reserveCost[0] = PairReserveMegaCost(actor, reserves[0], count[0], state);
    reserveCost[1] = IsPartnerMonFromSameTrainer(actor) ? reserveCost[0] : PairReserveMegaCost(partner, reserves[1], count[1], state);
    struct PairAction chosen[2], bestActions[2];
    u32 bestReserves[2] = {PARTY_SIZE, PARTY_SIZE};
    u32 bestMega = 0, bestTieCost = UINT_MAX;
    s32 best = INT_MIN;
    for (u32 left = 0; left < count[0]; left++)
    {
        for (u32 right = 0; right < count[1]; right++)
        {
            u32 slots[2] = {reserves[0][left], reserves[1][right]};
            if (slots[0] < PARTY_SIZE && slots[0] == slots[1] && BattlersShareParty(actor, partner))
                continue;
            for (u32 mega = 0; mega < 4; mega++)
            {
                AI_RestoreCandidateState(state);
                if ((mega == 3 && IsPartnerMonFromSameTrainer(actor))
                    || ((mega & 1) && (slots[0] < PARTY_SIZE || !CanMegaEvolve(actor)))
                    || ((mega & 2) && (slots[1] < PARTY_SIZE || !CanMegaEvolve(partner))))
                    continue;
                u32 noActionMask = 0;
                u32 tieCost = (mega != 0) + (slots[0] < PARTY_SIZE) + (slots[1] < PARTY_SIZE);
                for (u32 index = 0; index < 2; index++)
                {
                    if (slots[index] < PARTY_SIZE)
                    {
                        noActionMask |= 1u << actors[index];
                        tieCost += IsAceMon(actors[index], slots[index]);
                    }
                    if ((mega & (1u << index)) && PairPreservesStevenMetagross(actors[index]))
                        tieCost++;
                }
                if (slots[0] < PARTY_SIZE && slots[1] < PARTY_SIZE)
                    AI_LoadSwitchCandidatePair(actor, slots[0], partner, slots[1], FALSE);
                else if (slots[0] < PARTY_SIZE)
                    AI_LoadSwitchCandidate(actor, slots[0], FALSE);
                else if (slots[1] < PARTY_SIZE)
                    AI_LoadSwitchCandidate(partner, slots[1], FALSE);
                bool32 valid = TRUE;
                for (u32 index = 0; index < 2; index++)
                    if ((mega & (1u << index)) && !AI_ApplyMegaCandidate(actors[index], FALSE))
                        valid = FALSE;
                if (!valid)
                    continue;
                s32 score = EvaluatePairBoard(actor, noActionMask, chosen, ev);
                if (score > -10000 && score < 10000)
                    for (u32 index = 0; index < 2; index++)
                        if (mega & (1u << index))
                            score -= reserveCost[index];
                if (score > best || (score == best && tieCost < bestTieCost))
                {
                    best = score;
                    bestTieCost = tieCost;
                    bestMega = mega;
                    memcpy(bestActions, chosen, sizeof(bestActions));
                    memcpy(bestReserves, slots, sizeof(bestReserves));
                }
            }
        }
    }
    AI_RestoreCandidateState(state);
    AI_FreeCandidateState(state);
    AI_FreeCandidateState(ev->board.state);
    Free(ev);
    for (u32 index = 0; index < 2; index++)
    {
        enum BattlerId battler = actors[index];
        gBattleStruct->prevTurnSpecies[battler] = gBattleMons[battler].species;
        gAiLogicData->shouldSwitch &= ~(1u << battler);
        gAiLogicData->mostSuitableMonId[battler] = bestReserves[index];
        gAiLogicData->monToSwitchInId[battler] = bestReserves[index];
        gBattleStruct->AI_monToSwitchIntoId[battler] = bestReserves[index];
        if (bestReserves[index] < PARTY_SIZE)
            gAiLogicData->shouldSwitch |= 1u << battler;
        gAiBattleData->chosenMoveIndex[battler] = bestActions[index].index == PAIR_IDLE ? 0 : bestActions[index].index;
        gAiBattleData->chosenTarget[battler] = bestActions[index].target;
        SetAIUsingGimmick(battler, bestMega & (1u << index) ? USE_GIMMICK : NO_GIMMICK);
        gAiLogicData->battlerMovesScored |= 1u << battler;
    }
    return TRUE;
}
