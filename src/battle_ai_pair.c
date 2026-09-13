#include "global.h"
#include "main.h"
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
#include "emerald_champions_battle_plan.h"
#include "emerald_champions_perish.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/items.h"
#include "constants/battle_script_commands.h"

// Four moves, each with at most four legal targets. Switch recipients and
// Mega forms are board alternatives outside this list. Voluntary reserves use
// a bounded, ability-aware shortlist; forced replacements retain every slot.
#define PAIR_ACTIONS (MAX_MON_MOVES * MAX_BATTLERS_COUNT)
#define PAIR_IDLE MAX_MON_MOVES

// Stop optional comparisons after one second, leaving room under the 1.2s
// complete-decision limit for the current native calculation and restoration.
// Always establish a legal fallback; urgent Perish exits retain their search.
static bool32 PairDecisionBudgetExpired(void)
{
    return (u32)(gMain.vblankCounter1 - gAiLogicData->decisionStartFrame) >= 60;
}

enum PairAccuracyWeather
{
    PAIR_ACCURACY_CLEAR,
    PAIR_ACCURACY_SUN,
    PAIR_ACCURACY_RAIN,
    PAIR_ACCURACY_SAND,
    PAIR_ACCURACY_ICE,
    PAIR_ACCURACY_WEATHERS,
};

struct PairAction
{
    enum Move move;
    s16 score;
    u8 index;
    u8 target;
    s16 planScore;
    s8 priority;
    enum Move executedMove; // Nature Power's called move; move/index remain the command.
};

struct PairBoard
{
    struct SwitchCandidateSnapshot *state;
    s32 reserveValue[NUM_BATTLE_SIDES];
    u8 owner[MAX_BATTLERS_COUNT];
    u8 side[MAX_BATTLERS_COUNT];
    u8 activeMask;
    u8 healthValue[MAX_BATTLE_TRAINERS];
    uq4_12_t inverseHpMultiplier[MAX_BATTLERS_COUNT];
};

struct PairDamageRange
{
    u16 minimum, median, maximum;
};

struct PairRetaliation
{
    struct PairDamageRange damage;
    u8 source;
    u8 chance; // Zero means no qualifying hit this turn.
    bool8 knownDamage; // Variable/unequal multi-strike sequences retain the old estimate.
};

struct PairEvaluation
{
    struct PairBoard board;
    struct PairAction choices[MAX_BATTLERS_COUNT][PAIR_ACTIONS];
    struct PairAction action[MAX_BATTLERS_COUNT];
    u8 count[MAX_BATTLERS_COUNT];
    enum BattleSide side;
    u32 weather;
    // Only the two demonstrated partner stat interactions, cached per board.
    s8 statDelta[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT][3];
    s8 attackDropDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    s8 smashDefenseDelta[MAX_BATTLERS_COUNT][2];
    s8 selfDefenseDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // Native immediate Def/SpDef changes; see PairSelfDefenseStat.
    s8 speedDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 sleepDenialChance[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 paralysisTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 paralysisChance[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 paralysisRedirectors[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u16 paralyzedSpeed[MAX_BATTLERS_COUNT];
    u16 paralysisActionChance; // Basis points: native Champions 7/8, older 3/4.
    u16 dueWishHeal[MAX_BATTLERS_COUNT]; // Current slot, this end turn only.
    bool8 wishProtectOption[MAX_BATTLERS_COUNT];
    u8 drainPercent[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    // Accuracy + 1; zero means this move has no weather-sensitive cache.
    u8 weatherAccuracy[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT][PAIR_ACCURACY_WEATHERS];
    u16 contactDamage[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 screenTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 screenBreakerMoves[MAX_BATTLERS_COUNT];
    // Native raw endpoints, before Sash/Sturdy. Only HP-powered attacks use
    // these; two calculations per legal target, never inside candidate pairs.
    struct PairDamageRange hpPowerLow[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    struct PairDamageRange hpPowerHigh[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 hpPowerTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    // Reuse anchors for disjoint partner/item/Defeatist boosts, not a combined tree.
    struct PairDamageRange conditionalBoosted[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    struct PairDamageRange conditionalUnboosted[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 conditionalBoostTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 itemBoost[MAX_BATTLERS_COUNT]; // Move-slot mask; other slots can use Plus/Minus.
    u8 itemRemovers; // Living actors with a currently usable Knock Off.
    u8 protectChance[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 encoreGuardIndex[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT]; // Move slot + 1; zero is ineligible.
    s8 encoreGuardPriority[MAX_BATTLERS_COUNT];
    bool8 sleepClause;
};

static u32 PairMonValue(const struct PairBoard *board, enum BattleTrainer owner, u32 hp, u32 maxHp)
{
    if (!hp)
        return 0;
    // Authored pressure accepts chip trades, not cheaper knockouts. A healthy
    // member is worth 180 in either style; shifting value from HP to staying
    // alive makes an injured member MORE costly to lose, not expendable.
    u32 healthValue = board->healthValue[owner];
    return 180 - healthValue + hp * healthValue / max(1, maxHp);
}

static void SavePairBoard(struct PairBoard *board, enum BattleSide evaluatingSide)
{
    u8 scoreLimit[MAX_BATTLE_TRAINERS] = {0};
    u8 ownerSide[MAX_BATTLE_TRAINERS] = {0};
    board->activeMask = 0;
    memset(board->reserveValue, 0, sizeof(board->reserveValue));
    memset(board->healthValue, 100, sizeof(board->healthValue));
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        board->inverseHpMultiplier[actor] = UQ_4_12(1.0);
        if (GetActiveGimmick(actor) == GIMMICK_DYNAMAX && !HasShedinjaHPHandling(gBattleMons[actor].species))
            board->inverseHpMultiplier[actor] = GetDynamaxLevelHPMultiplier(GetMonData(GetBattlerMon(actor), MON_DATA_DYNAMAX_LEVEL), TRUE);
        board->owner[actor] = GetBattlerTrainer(actor);
        if (GetBattlerSide(actor) == evaluatingSide
         && (EmeraldChampions_GetBattlePlan(actor) & EC_BATTLE_PLAN_PRESSURE))
            board->healthValue[board->owner[actor]] = 25;
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
                continue;
            }
            u32 hp = GetMonData(mon, MON_DATA_HP);
            if (!hp)
                continue;
            if (slot < scoreLimit[trainer])
            {
                u32 maxHp = GetMonData(mon, MON_DATA_MAX_HP);
                board->reserveValue[ownerSide[trainer]] += PairMonValue(board, trainer, hp, maxHp);
                if (species == SPECIES_PALAFIN_HERO)
                    board->reserveValue[ownerSide[trainer]] += 10;
            }
        }
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
    if (!IsBattlerAlive(target) || gBattleMons[target].volatiles.semiInvulnerable == STATE_COMMANDER)
        return FALSE;
    if (PairSupport(move) && GetMoveEffect(move) != EFFECT_FOLLOW_ME)
        return target == GetPartnerBattler(actor);
    switch (AI_GetBattlerMoveTargetType(actor, move))
    {
    case TARGET_USER:
    case TARGET_USER_AND_ALLY:
    case TARGET_FIELD:
    case TARGET_ALL_BATTLERS:
        return target == actor;
    case TARGET_ALLY:
        return target == GetPartnerBattler(actor);
    case TARGET_USER_OR_ALLY:
        return IsBattlerAlly(actor, target);
    case TARGET_BOTH:
    case TARGET_FOES_AND_ALLY:
    case TARGET_RANDOM:
    case TARGET_OPPONENTS_FIELD:
        // The native hazard scripts use the target's side, not the user's.
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
    if (IsBattlerActionCommitted(actor))
    {
        struct PairAction action = {MOVE_NONE, AI_SCORE_DEFAULT, PAIR_IDLE, actor};
        if (!(noActionMask & (1u << actor)) && IsBattlerAlive(actor)
         && gChosenActionByBattler[actor] == B_ACTION_USE_MOVE
         && gBattleMons[actor].volatiles.semiInvulnerable != STATE_COMMANDER
         && !gBattleMons[actor].volatiles.rechargeTimer)
        {
            action.index = gBattleStruct->chosenMovePositions[actor];
            action.move = GetCommittedMove(actor);
            action.target = gBattleStruct->moveTarget[actor];
            if (gProtectStructs[actor].noValidMoves)
            {
                action.index = 0;
            }
            else if (!gBattleMons[actor].volatiles.multipleTurns
                  && GetActiveGimmick(actor) != GIMMICK_Z_MOVE
                  && gBattleMons[actor].volatiles.encoredMove != MOVE_NONE)
            {
                action.index = gBattleMons[actor].volatiles.encoredMovePos;
            }
            action.executedMove = GetMoveEffect(action.move) == EFFECT_NATURE_POWER ? GetNaturePowerMove() : action.move;
            if (action.move == MOVE_NONE)
                action.index = PAIR_IDLE;
        }
        // A locked command may fail or hit an ally. Neither permits inventing
        // a different move/target; non-move commands spend this actor's turn.
        ev->choices[actor][ev->count[actor]++] = action;
        return;
    }
    if (!(noActionMask & (1u << actor)) && IsBattlerAlive(actor)
     && gBattleMons[actor].volatiles.semiInvulnerable != STATE_COMMANDER)
    {
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = gBattleMons[actor].moves[index];
            if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[actor]))
                continue;
            enum Move executedMove = GetMoveEffect(move) == EFFECT_NATURE_POWER ? GetNaturePowerMove() : move;
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
            {
                if (!PairTargetIsLegal(actor, target, move))
                    continue;
                enum BattlerId scoreTarget = target == actor ? GetOppositeBattler(actor) : target;
                if (!IsBattlerAlive(scoreTarget))
                    scoreTarget = GetPartnerBattler(scoreTarget);
                enum MoveTarget executedTarget = GetMoveTarget(executedMove);
                if (GetBattlerSide(actor) == ev->side && !IsBattleMoveStatus(executedMove)
                 && (executedTarget == TARGET_BOTH || executedTarget == TARGET_FOES_AND_ALLY)
                 && gAiLogicData->simulatedDmg[actor][scoreTarget][index].maximum == 0)
                {
                    // The nominal target only deduplicates spread actions.
                    // Its immunity must not label a hit on the other foe as
                    // useless. Keep the real action target and evaluate all
                    // recipients (including ally damage) in the pair trial.
                    for (enum BattlerId recipient = 0; recipient < gBattlersCount; recipient++)
                    {
                        if (IsBattlerAlive(recipient) && !IsBattlerAlly(actor, recipient)
                         && gBattleMons[recipient].volatiles.semiInvulnerable != STATE_COMMANDER
                         && gAiLogicData->simulatedDmg[actor][recipient][index].maximum != 0)
                        {
                            scoreTarget = recipient;
                            break;
                        }
                    }
                }
                gAiLogicData->partnerMove = MOVE_NONE;
                // Opposing actions are damage forecasts, not a second full AI
                // decision. Rescoring the human's entire menu on every switch
                // candidate wastes time without revealing their actual command.
                s32 score = GetBattlerSide(actor) == ev->side ? AI_ScoreMoveAgainstTarget(actor, scoreTarget, index) : AI_SCORE_DEFAULT;
                if (actor != target && IsBattlerAlly(actor, target) && !IsBattleMoveStatus(executedMove)
                 && GetMoveEffect(executedMove) != EFFECT_HIT_ENEMY_HEAL_ALLY
                 && (score < AI_SCORE_DEFAULT || GetBattlerSide(actor) != ev->side))
                    continue;
                // Enabling actions are retained even when their independent
                // heuristic cannot yet see a concrete partner action.
                ev->choices[actor][ev->count[actor]++] = (struct PairAction){move, score, index, target, .executedMove = executedMove};
            }
        }
    }
    if (GetBattlerSide(actor) == ev->side)
    {
        // Known single-target failures are not enabling actions on a foe.
        // Preserve allied activation and multi-recipient/retargeting moves:
        // other recipients may be vulnerable. If every option fails, retain the actual
        // legal moves rather than inventing Struggle or an unavailable action.
        u32 useful = 0;
        for (u32 index = 0; index < ev->count[actor]; index++)
        {
            const struct PairAction *action = &ev->choices[actor][index];
            enum MoveTarget targetType = GetMoveTarget(action->executedMove);
            if (!IsBattlerAlly(actor, action->target)
             && targetType != TARGET_BOTH && targetType != TARGET_FOES_AND_ALLY
             && targetType != TARGET_ALL_BATTLERS && targetType != TARGET_RANDOM
             && targetType != TARGET_SMART)
            {
                if (!IsBattleMoveStatus(action->executedMove)
                 && gAiLogicData->effectiveness[actor][action->target][action->index] == UQ_4_12(0.0))
                    continue;
                // The native opinion only penalizes a spent status. That
                // failed move can otherwise become a repeatable fake guard
                // against Sucker Punch. Exclude pure immediate status here;
                // keep damaging secondaries, delayed Yawn and spread users.
                if (IsBattleMoveStatus(action->executedMove)
                 && GetMoveEffect(action->executedMove) == EFFECT_NON_VOLATILE_STATUS
                 && gBattleMons[action->target].status1)
                    continue;
            }
            ev->choices[actor][useful++] = *action;
        }
        if (useful)
            ev->count[actor] = useful;
    }
    if (ev->count[actor] == 0)
    {
        if (!(noActionMask & (1u << actor)) && IsBattlerAlive(actor)
         && gBattleMons[actor].volatiles.semiInvulnerable != STATE_COMMANDER)
            ev->choices[actor][ev->count[actor]++] = (struct PairAction){MOVE_STRUGGLE, AI_SCORE_DEFAULT, 0, IsBattlerAlive(GetOppositeBattler(actor)) ? GetOppositeBattler(actor) : GetOppositeBattler(GetPartnerBattler(actor)), .executedMove = MOVE_STRUGGLE};
        else
            ev->choices[actor][ev->count[actor]++] = (struct PairAction){MOVE_NONE, AI_SCORE_DEFAULT, PAIR_IDLE, actor};
    }
}

static bool32 PairSpread(enum Move move)
{
    enum MoveTarget target = GetMoveTarget(move);
    return target == TARGET_BOTH || target == TARGET_FOES_AND_ALLY || target == TARGET_ALL_BATTLERS;
}

static bool32 PairForecastHitsTarget(enum BattlerId actor, const struct PairAction *action, enum BattlerId target)
{
    if (action->index == PAIR_IDLE || actor == target || !IsBattlerAlive(target) || IsBattleMoveStatus(action->executedMove))
        return FALSE;
    if (PairSpread(action->executedMove))
        return !IsBattlerAlly(actor, target) || GetMoveTarget(action->executedMove) != TARGET_BOTH;
    return target == action->target;
}

static bool32 PairHasWishProtectOption(enum BattlerId actor)
{
    enum Ability ability = gAiLogicData->abilities[actor];
    if (ability == ABILITY_TRUANT || ability == ABILITY_GORILLA_TACTICS
     || (IsHoldEffectChoice(gAiLogicData->holdEffects[actor]) && IsBattlerItemEnabled(actor))
     || (gBattleMons[actor].status1 & STATUS1_PARALYSIS)
     || gBattleMons[actor].volatiles.yawn
     || ((gBattleMons[actor].status1 & STATUS1_BURN) && ability != ABILITY_MAGIC_GUARD))
        return FALSE;
    bool32 wish = FALSE, protect = FALSE;
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
    {
        enum Move move = gBattleMons[actor].moves[index];
        if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[actor]))
            continue;
        wish |= GetMoveEffect(move) == EFFECT_WISH;
        protect |= move == MOVE_PROTECT;
    }
    // This bounded option is self-recovery, not speculative Wish passing or
    // a second-turn search. With residual damage, defer to actual next state.
    if (!wish || !protect || GetBattlerSecondaryDamage(actor))
        return FALSE;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
            continue;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = gBattleMons[foe].moves[index];
            if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[foe]))
                continue;
            enum BattleMoveEffects effect = GetMoveEffect(move);
            if ((!IsBattleMoveStatus(move) && (MoveIgnoresProtect(move) || AI_CanContactBypassProtect(foe, actor, move)))
             || effect == EFFECT_TAUNT || effect == EFFECT_ENCORE || effect == EFFECT_HEAL_BLOCK)
                return FALSE;
        }
    }
    return TRUE;
}

// Scoring is deliberately bounded: cached native damage, four actions and
// explicit partner coordination. Never run the battle engine for every pair.
static u32 PairWeatherMask(enum Move move)
{
    switch (GetMoveWeatherType(move))
    {
    case BATTLE_WEATHER_RAIN: return B_WEATHER_RAIN;
    case BATTLE_WEATHER_SUN: return B_WEATHER_SUN;
    case BATTLE_WEATHER_SANDSTORM: return B_WEATHER_SANDSTORM;
    case BATTLE_WEATHER_HAIL:
    case BATTLE_WEATHER_SNOW: return B_WEATHER_ICY_ANY;
    default: return 0;
    }
}

static u32 PairWeatherPlan(enum Move move)
{
    switch (GetMoveWeatherType(move))
    {
    case BATTLE_WEATHER_RAIN: return EC_BATTLE_PLAN_RAIN;
    case BATTLE_WEATHER_SUN: return EC_BATTLE_PLAN_SUN;
    case BATTLE_WEATHER_SANDSTORM: return EC_BATTLE_PLAN_SAND;
    case BATTLE_WEATHER_HAIL:
    case BATTLE_WEATHER_SNOW: return EC_BATTLE_PLAN_SNOW;
    default: return 0;
    }
}

static bool32 PairWeatherSpeed(enum Ability ability, enum HoldEffect holdEffect, u32 weather)
{
    return (ability == ABILITY_SWIFT_SWIM && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA && (weather & B_WEATHER_RAIN))
        || (ability == ABILITY_CHLOROPHYLL && holdEffect != HOLD_EFFECT_UTILITY_UMBRELLA && (weather & B_WEATHER_SUN))
        || (ability == ABILITY_SAND_RUSH && (weather & B_WEATHER_SANDSTORM))
        || (ability == ABILITY_SLUSH_RUSH && (weather & B_WEATHER_ICY_ANY));
}

static u32 PairWeatherPower(enum BattlerId actor, enum BattlerId target, enum Move move, u32 weather)
{
    // Match native GetWeatherDamageModifier: the defender's Umbrella blocks
    // ordinary rain/sun damage modifiers. The attacker's Umbrella does not
    // remove the global weather's outgoing damage modifier.
    enum Type type = GetMoveType(move);
    u32 attackerWeather = GetAttackerWeather(gAiLogicData->holdEffects[actor], gAiLogicData->abilities[actor], weather);
    if (GetMoveEffect(move) == EFFECT_HYDRO_STEAM && (attackerWeather & B_WEATHER_SUN))
        return 150;
    if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_UTILITY_UMBRELLA)
        return 100;
    if ((weather | attackerWeather) & B_WEATHER_SUN)
        return type == TYPE_FIRE ? 150 : type == TYPE_WATER ? 50 : 100;
    if ((weather | attackerWeather) & B_WEATHER_RAIN)
        return type == TYPE_WATER ? 150 : type == TYPE_FIRE ? 50 : 100;
    return 100;
}

static enum PairAccuracyWeather PairAccuracyWeatherIndex(u32 weather)
{
    if (weather & B_WEATHER_SUN)
        return PAIR_ACCURACY_SUN;
    if (weather & B_WEATHER_RAIN)
        return PAIR_ACCURACY_RAIN;
    if (weather & B_WEATHER_SANDSTORM)
        return PAIR_ACCURACY_SAND;
    if (weather & B_WEATHER_ICY_ANY)
        return PAIR_ACCURACY_ICE;
    return PAIR_ACCURACY_CLEAR;
}

static void CachePairWeatherAccuracy(struct PairEvaluation *ev, enum BattlerId actor, u32 index, enum Move move)
{
    if (!IsBattlerAlive(actor)
     || IsMoveUnusable(index, gBattleMons[actor].moves[index], gAiLogicData->moveLimitations[actor])
     || !(MoveAlwaysHitsInRain(move) || MoveHas50AccuracyInSun(move) || MoveAlwaysHitsInHailSnow(move)))
        return;
    static const u32 weatherKinds[PAIR_ACCURACY_WEATHERS] = {
        B_WEATHER_NONE, B_WEATHER_SUN, B_WEATHER_RAIN, B_WEATHER_SANDSTORM, B_WEATHER_ICY_ANY,
    };
    struct BattleCalcValues cv = {.battlerAtk = actor, .move = move, .moveEffect = GetMoveEffect(move)};
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        cv.abilities[battler] = battler == actor ? gAiLogicData->abilities[battler]
            : AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
                gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler], move);
        cv.holdEffects[battler] = gAiLogicData->holdEffects[battler];
    }
    enum BattlerId savedItemBattler = gPotentialItemEffectBattler;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (target == actor || !IsBattlerAlive(target))
            continue;
        cv.battlerDef = target;
        for (u32 kind = 0; kind < PAIR_ACCURACY_WEATHERS; kind++)
        {
            // Native accuracy incorporates No Guard, stages, attacker Umbrella,
            // lenses and weather. Never run it inside candidate-pair scoring.
            u32 accuracy = CanMoveSkipAccuracyCalc(&cv, weatherKinds[kind], AI_CHECK)
                ? 100 : min(100, GetTotalAccuracy(&cv, weatherKinds[kind]));
            ev->weatherAccuracy[actor][index][target][kind] = accuracy + 1;
        }
    }
    gPotentialItemEffectBattler = savedItemBattler;
}

static bool32 PairActsBefore(enum BattlerId left, const struct PairAction *leftAction, enum BattlerId right, const struct PairAction *rightAction, const u32 *speed, bool32 trickRoom)
{
    s32 leftPriority = leftAction->priority;
    s32 rightPriority = rightAction->priority;
    if (leftPriority != rightPriority)
        return leftPriority > rightPriority;
    if (trickRoom)
        return speed[left] < speed[right];
    return speed[left] > speed[right];
}

static u32 PairNonDynamaxHP(const struct PairEvaluation *ev, enum BattlerId target, u32 hp)
{
    // Same inverse-Dynamax rounding as GetNonDynamaxHP, but applied to the
    // trial's remaining HP rather than the unchanged live battle object.
    return UQ_4_12_TO_INT(hp * ev->board.inverseHpMultiplier[target] + UQ_4_12_ROUND);
}

static u32 PairFixedPercentDamage(const struct PairEvaluation *ev, enum BattlerId target, enum Move move, u32 hp)
{
    return max(1, PairNonDynamaxHP(ev, target, hp) * GetMoveDamagePercentage(move) / 100);
}

static void PairApplyDamage(u32 *hp, u32 *survival, u32 minimum, u32 median, u32 maximum, u32 hitChance)
{
    if (maximum < *hp)
    {
        *hp -= median * hitChance / 100;
        return;
    }
    u32 surviveHit = 0, survivingHp = 0;
    if (minimum < *hp)
    {
        // The cache has three damage anchors, not the native roll histogram.
        // Interpolate the uncertain KO range; never turn a median KO into a
        // certain KO. Surviving hits leave 1..(HP - minimum) HP.
        u32 remainingRange = *hp - minimum;
        surviveHit = max(1, min(99, remainingRange * 100 / (maximum - minimum + 1)));
        survivingHp = (remainingRange + 2) / 2;
    }
    u32 survivingHitChance = hitChance && surviveHit ? max(1, hitChance * surviveHit / 100) : 0;
    u32 aliveChance = 100 - hitChance + survivingHitChance;
    *survival = aliveChance ? max(1, *survival * aliveChance / 100) : 0;
    if (!*survival)
    {
        *hp = 0;
        return;
    }
    // HP is conditional on still being alive. A missed lethal attack leaves
    // full HP on the surviving branch, not a fictitious sliver that any later
    // chip move can certainly finish. Accuracy/actor survival enter only once.
    *hp = DIV_ROUND_UP(*hp * (100 - hitChance) + survivingHp * survivingHitChance, aliveChance);
}

static void PairApplyDamageWhenActing(u32 *hp, u32 *survival, u32 minimum, u32 median, u32 maximum,
    u32 hitChance, u32 actionChance)
{
    if (actionChance == 10000)
    {
        PairApplyDamage(hp, survival, minimum, median, maximum, hitChance);
        return;
    }
    // Paralysis is one whole-action event, separate from move accuracy.
    // Preserve the ordinary forecast exactly when execution is certain.
    u32 actingHp = *hp, actingSurvival = 100;
    PairApplyDamage(&actingHp, &actingSurvival, minimum, median, maximum, hitChance);
    u32 aliveMass = actingSurvival * actionChance / 100;
    u32 hpMass = actingHp * aliveMass + *hp * (10000 - actionChance);
    aliveMass += 10000 - actionChance;
    *hp = aliveMass ? DIV_ROUND_UP(hpMass, aliveMass) : 0;
    *survival = aliveMass ? max(1, *survival * aliveMass / 10000) : 0;
}

static void PairTryHealingBerry(const struct PairEvaluation *ev, enum BattlerId target,
    u32 *hp, u32 *speed, u32 *usedItems)
{
    enum Item item = gBattleMons[target].item;
    enum HoldEffect effect = gAiLogicData->holdEffects[target];
    enum Ability ability = gAiLogicData->abilities[target];
    if (!hp[target] || (*usedItems & (1u << target))
     || hp[target] >= gBattleMons[target].maxHP
     || (B_HEAL_BLOCKING >= GEN_5 && gBattleMons[target].volatiles.healBlockTimer))
        return;
    u32 threshold, amount;
    if (effect == HOLD_EFFECT_RESTORE_PCT_HP)
    {
        threshold = GetBerryActivationThreshold(gBattleMons[target].maxHP, 2, ability, item);
        amount = PairNonDynamaxHP(ev, target, gBattleMons[target].maxHP)
            * GetItemHoldEffectParam(item) / 100;
    }
    else if (effect == HOLD_EFFECT_RESTORE_HP)
    {
        threshold = GetBerryActivationThreshold(gBattleMons[target].maxHP, 2, ability, item);
        amount = GetItemHoldEffectParam(item);
    }
    else if (effect == HOLD_EFFECT_CONFUSE_FLAVOR
          && GetFlavorRelationByPersonality(gBattleMons[target].personality, GetItemSecondaryId(item)) >= 0)
    {
        u32 fraction = B_CONFUSE_BERRIES_HEAL >= GEN_7 ? 4 : 2;
        threshold = GetBerryActivationThreshold(gBattleMons[target].maxHP, fraction, ability, item);
        amount = PairNonDynamaxHP(ev, target, gBattleMons[target].maxHP)
            / max(1, GetItemHoldEffectParam(item));
        // Disliked flavors need confusion continuation, not free healing.
    }
    else
        return;
    if (hp[target] > threshold)
        return;
    // The effective item cache includes Klutz, Embargo and Magic Room. Only
    // locally surviving, known Unnerve users block natural berry consumption.
    bool32 isBerry = GetItemPocket(item) == POCKET_BERRIES;
    for (enum BattlerId foe = 0; isBerry && foe < gBattlersCount; foe++)
    {
        if (hp[foe] && !IsBattlerAlly(target, foe)
         && (gAiLogicData->abilities[foe] == ABILITY_UNNERVE
             || gAiLogicData->abilities[foe] == ABILITY_AS_ONE_ICE_RIDER
             || gAiLogicData->abilities[foe] == ABILITY_AS_ONE_SHADOW_RIDER))
            return;
    }
    if (ability == ABILITY_RIPEN && isBerry)
        amount *= 2;
    hp[target] = min(gBattleMons[target].maxHP, hp[target] + max(1, amount));
    *usedItems |= 1u << target;
    if (gAiLogicData->abilities[target] == ABILITY_UNBURDEN && !gBattleMons[target].volatiles.unburdenActive)
        speed[target] *= 2;
}

static u32 PairForecastDamage(const struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target, const struct PairAction *action, u32 hp, u32 damage)
{
    enum Move move = action->executedMove;
    if (damage && GetMoveEffect(move) == EFFECT_FIXED_PERCENT_DAMAGE)
        damage = PairFixedPercentDamage(ev, target, move, hp);
    bool32 singleHit = GetMoveStrikeCount(move) <= 1 && !IsMultiHitMove(move)
        && GetMoveEffect(move) != EFFECT_BEAT_UP && gAiLogicData->abilities[actor] != ABILITY_PARENTAL_BOND;
    if (damage >= hp && hp == gBattleMons[target].maxHP && singleHit
     && (gAiLogicData->holdEffects[target] == HOLD_EFFECT_FOCUS_SASH
         || AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor], gAiLogicData->abilities[target],
             gAiLogicData->holdEffects[target], move) == ABILITY_STURDY))
        return hp - 1;
    return min(hp, damage);
}

static s32 PairJointFoeForecast(const struct PairEvaluation *ev, enum BattlerId first, const struct PairAction *firstAction,
    enum BattlerId second, const struct PairAction *secondAction)
{
    u32 speed[MAX_BATTLERS_COUNT];
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        speed[battler] = gAiLogicData->speedStats[battler];
    if (PairActsBefore(second, secondAction, first, firstAction, speed,
        gFieldStatuses & STATUS_FIELD_TRICK_ROOM))
    {
        enum BattlerId tempBattler = first;
        const struct PairAction *tempAction = firstAction;
        first = second;
        firstAction = secondAction;
        second = tempBattler;
        secondAction = tempAction;
    }
    const enum BattlerId actors[] = {first, second};
    const struct PairAction *actions[] = {firstAction, secondAction};
    s32 score = 0;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (!IsBattlerAlive(target))
            continue;
        u32 chance[2] = {0}, median[2] = {0}, minimum[2] = {0};
        for (u32 i = 0; i < 2; i++)
        {
            if (!PairForecastHitsTarget(actors[i], actions[i], target))
                continue;
            const struct SimulatedDamage *damage = &gAiLogicData->simulatedDmg[actors[i]][target][actions[i]->index];
            chance[i] = min(100, gAiLogicData->moveAccuracy[actors[i]][target][actions[i]->index]);
            median[i] = damage->median;
            minimum[i] = damage->minimum;
        }
        u32 weightedDamage = 0, weightedKO = 0;
        // Two cached attacks, four arithmetic hit/miss combinations. Cap each
        // target's combined HP and KO credit, rather than rewarding overkill
        // twice. This is not a second battle simulation or command prediction.
        for (u32 hits = 0; hits < 4; hits++)
        {
            u32 weight = (hits & 1 ? chance[0] : 100 - chance[0])
                * (hits & 2 ? chance[1] : 100 - chance[1]);
            if (!weight)
                continue;
            u32 hp = gBattleMons[target].hp, guaranteedHp = hp;
            for (u32 i = 0; i < 2; i++)
            {
                if (!(hits & (1u << i)))
                    continue;
                if (hp)
                    hp -= PairForecastDamage(ev, actors[i], target, actions[i], hp, median[i]);
                if (guaranteedHp)
                    guaranteedHp -= PairForecastDamage(ev, actors[i], target, actions[i], guaranteedHp, minimum[i]);
            }
            weightedDamage += (gBattleMons[target].hp - hp) * weight;
            if (!guaranteedHp)
                weightedKO += weight;
        }
        s32 value = weightedDamage / (100 * max(1, gBattleMons[target].maxHP)) + weightedKO * 80 / 10000;
        score += IsBattlerAlly(first, target) ? -value : value;
    }
    return score;
}

static u32 ChooseJointFoeForecast(struct PairEvaluation *ev, enum BattlerId actor, struct PairAction forecasts[2][2])
{
    enum BattlerId first = GetOppositeBattler(actor);
    enum BattlerId second = GetPartnerBattler(first);
    enum BattlerId partner = GetPartnerBattler(actor);
    if (ev->count[first] == 1 && ev->count[second] == 1)
    {
        forecasts[0][0] = ev->choices[first][0];
        forecasts[0][1] = ev->choices[second][0];
        return 1;
    }
    u8 masks[2][PAIR_ACTIONS] = {0};
    const enum BattlerId foes[2] = {first, second};
    const enum BattlerId targets[2] = {actor, partner};
    for (u32 foe = 0; foe < 2; foe++)
        for (u32 choice = 0; choice < ev->count[foes[foe]]; choice++)
            for (u32 target = 0; target < 2; target++)
            {
                const struct PairAction *action = &ev->choices[foes[foe]][choice];
                if (PairForecastHitsTarget(foes[foe], action, targets[target])
                 && gAiLogicData->simulatedDmg[foes[foe]][targets[target]][action->index].maximum
                 && gAiLogicData->moveAccuracy[foes[foe]][targets[target]][action->index])
                    masks[foe][choice] |= 1u << target;
            }
    s32 maskScores[4] = {INT_MIN, INT_MIN, INT_MIN, INT_MIN};
    struct PairAction maskChoices[4][2];
    u32 bestMask = 0;
    s32 best = INT_MIN;
    for (u32 left = 0; left < ev->count[first]; left++)
        for (u32 right = 0; right < ev->count[second]; right++)
        {
            s32 score = PairJointFoeForecast(ev, first, &ev->choices[first][left], second, &ev->choices[second][right]);
            u32 mask = masks[0][left] | masks[1][right];
            if (score > maskScores[mask])
            {
                maskScores[mask] = score;
                maskChoices[mask][0] = ev->choices[first][left];
                maskChoices[mask][1] = ev->choices[second][right];
            }
            if (score > best)
            {
                best = score;
                bestMask = mask;
                forecasts[0][0] = ev->choices[first][left];
                forecasts[0][1] = ev->choices[second][right];
            }
        }
    // A revealed redirector is a credible alternative to the strongest
    // damage forecast. Reuse the second slot, not another forecast/turn tree.
    // Otherwise repeated Follow Me can keep feeding an immune recipient
    // while every trial assumes the foe attacks instead. Choices already
    // exclude exhausted, Taunted and otherwise unavailable moves. Retain the
    // primary damage forecast: last turn's move is not the pending command.
    for (u32 foe = 0; foe < ARRAY_COUNT(foes); foe++)
    {
        enum BattlerId redirector = foes[foe];
        enum Move previous = gAiLogicData->lastUsedMove[redirector];
        if (!HasPartner(redirector)
         || (previous != MOVE_FOLLOW_ME && previous != MOVE_RAGE_POWDER)
         || IsBattlerIncapacitated(redirector, gAiLogicData->abilities[redirector]))
            continue;
        for (u32 choice = 0; choice < ev->count[redirector]; choice++)
        {
            const struct PairAction *action = &ev->choices[redirector][choice];
            if (action->index == PAIR_IDLE || action->move != previous)
                continue;
            forecasts[1][0] = forecasts[0][0];
            forecasts[1][1] = forecasts[0][1];
            forecasts[1][foe] = *action;
            return 2;
        }
    }
    // A fixed target forecast can make Protect look like it shields both
    // allies: the opponent could simply focus the unprotected partner. Keep
    // one strongest different damage-target pattern from the same enumeration.
    // This is bounded uncertainty, not access to pending human commands.
    u32 alternate = 0;
    for (u32 mask = 1; mask < ARRAY_COUNT(maskScores); mask++)
        if (mask != bestMask && maskScores[mask] > 0
         && (alternate == 0 || maskScores[mask] > maskScores[alternate]))
            alternate = mask;
    if (alternate != 0 && bestMask != 0)
    {
        forecasts[1][0] = maskChoices[alternate][0];
        forecasts[1][1] = maskChoices[alternate][1];
        return 2;
    }
    return 1;
}

static bool32 PairIsPassiveGuard(const struct PairAction *action)
{
    if (action->index == PAIR_IDLE || GetMoveEffect(action->move) != EFFECT_PROTECT)
        return FALSE;
    enum ProtectMethod method = GetMoveProtectMethod(action->move);
    // Contact shields can themselves damage, poison or debuff an attacker.
    return method == PROTECT_NORMAL || method == PROTECT_MAX_GUARD
        || method == PROTECT_WIDE_GUARD || method == PROTECT_QUICK_GUARD
        || method == PROTECT_CRAFTY_SHIELD || method == PROTECT_MAT_BLOCK;
}

static bool32 PairWaitingHasPayoff(const struct PairEvaluation *ev, enum BattlerId actor)
{
    // This permits passive waiting; it does not reward or force a guard.
    // Inspect public state once per board, not inside the pair enumeration.
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlive(battler))
            continue;
        enum Ability ability = gAiLogicData->abilities[battler];
        enum HoldEffect item = gAiLogicData->holdEffects[battler];
        if (!IsBattlerAlly(actor, battler))
        {
            bool32 committed = IsBattlerActionCommitted(battler);
            enum Move move = committed && gChosenActionByBattler[battler] == B_ACTION_USE_MOVE
                ? GetCommittedMove(battler) : MOVE_NONE;
            u32 index = GetMoveIndex(battler, move);
            if (GetBattlerSecondaryDamage(battler)
             || ((gBattleMons[battler].status1 & STATUS1_BURN) && ability != ABILITY_MAGIC_GUARD)
             || gBattleMons[battler].volatiles.yawn
             || (ability == ABILITY_TRUANT && !gBattleMons[battler].volatiles.truantCounter)
             || (gBattleStruct->battlerState[battler].isFirstTurn
                 && (committed ? move == MOVE_FAKE_OUT : HasMove(battler, MOVE_FAKE_OUT)))
             || (move != MOVE_NONE && GetMoveEffect(move) == EFFECT_SUCKER_PUNCH)
             || (move != MOVE_NONE && !IsBattleMoveStatus(move) && index < MAX_MON_MOVES
                 && gBattleMons[battler].pp[index] == 1)
             || (gBattleMons[battler].volatiles.semiInvulnerable
                 && gBattleMons[battler].volatiles.semiInvulnerable != STATE_COMMANDER))
                return TRUE;
            continue;
        }
        if (EC_PerishPlanScore(battler, MOVE_PROTECT) > 0)
            return TRUE;
        if (!gBattleMons[battler].volatiles.healBlockTimer && gBattleMons[battler].hp < gBattleMons[battler].maxHP
         && (ev->dueWishHeal[battler] || item == HOLD_EFFECT_LEFTOVERS
             || (item == HOLD_EFFECT_BLACK_SLUDGE && IS_BATTLER_OF_TYPE(battler, TYPE_POISON))
             || gBattleMons[battler].volatiles.aquaRing || gBattleMons[battler].volatiles.root
             || (ability == ABILITY_POISON_HEAL && (gBattleMons[battler].status1 & STATUS1_PSN_ANY))
             || (ability == ABILITY_RAIN_DISH && (ev->weather & B_WEATHER_RAIN))
             || (ability == ABILITY_ICE_BODY && (ev->weather & B_WEATHER_ICY_ANY))
             || (gFieldTimers.terrain == B_TERRAIN_GRASSY && AI_IsBattlerGrounded(battler))))
            return TRUE;
        if (!gBattleMons[battler].status1
         && (ability == ABILITY_GUTS || ability == ABILITY_QUICK_FEET || ability == ABILITY_FLARE_BOOST
             || ability == ABILITY_POISON_HEAL || HasMove(battler, MOVE_FACADE))
         && ((item == HOLD_EFFECT_FLAME_ORB && CanBeBurned(battler, battler, ability))
             || (item == HOLD_EFFECT_TOXIC_ORB && CanBePoisoned(battler, battler, ability, ability))))
            return TRUE;
        if (ability == ABILITY_MOODY || (ability == ABILITY_SLOW_START && gBattleMons[battler].volatiles.slowStartTimer))
            return TRUE;
        if (ability == ABILITY_SPEED_BOOST && !BattlerJustSwitchedIn(battler)
         && gBattleMons[battler].statStages[STAT_SPEED] < MAX_STAT_STAGE)
        {
            u32 stage = gBattleMons[battler].statStages[STAT_SPEED];
            u32 speed = gAiLogicData->speedStats[battler];
            u32 nextSpeed = speed * gStatStageRatios[stage + 1][0] * gStatStageRatios[stage][1]
                / (gStatStageRatios[stage + 1][1] * gStatStageRatios[stage][0]);
            if (!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM))
                for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                    if (IsBattlerAlive(foe) && !IsBattlerAlly(actor, foe)
                     && speed <= gAiLogicData->speedStats[foe] && nextSpeed > gAiLogicData->speedStats[foe])
                        return TRUE;
        }
    }
    u32 foeSide = GetBattlerSide(actor) ^ 1;
    return ((gSideStatuses[foeSide] & SIDE_STATUS_TAILWIND) && gSideTimers[foeSide].tailwindTimer == 1)
        || ((gFieldStatuses & STATUS_FIELD_TRICK_ROOM) && ShouldClearFieldStatus(actor, STATUS_FIELD_TRICK_ROOM))
        || (gBattleWeather && gBattleStruct->weatherDuration == 1);
}

static s32 PairPlanScore(enum BattlerId actor, const struct PairAction *action)
{
    if (action->index == PAIR_IDLE)
        return 0;
    enum Move move = action->move;
    enum BattleMoveEffects effect = GetMoveEffect(move);
    u32 plan = EmeraldChampions_GetBattlePlan(actor);
    // A last-turn "refresh" costs both actions and can cancel itself when one
    // setter is interrupted. Let the room expire and establish it next turn.
    if (effect == EFFECT_TRICK_ROOM)
    {
        if ((gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
             && (gFieldTimers.trickRoomTimer == 1
                 || !ShouldClearFieldStatus(actor, STATUS_FIELD_TRICK_ROOM)))
            return -10000;
        if (!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM)
         && ShouldSetFieldStatus(actor, STATUS_FIELD_TRICK_ROOM))
            return plan & EC_BATTLE_PLAN_TRICK_ROOM ? 100 : 70;
    }
    if (effect == EFFECT_WEATHER)
    {
        if ((gBattleWeather & PairWeatherMask(move)) || (gBattleWeather & B_WEATHER_PRIMAL_ANY))
            return -10000;
        if (HasWeatherEffect() && (plan & PairWeatherPlan(move)) && ShouldSetWeather(actor, PairWeatherMask(move)))
            return 90;
    }
    if (effect == EFFECT_TAILWIND)
    {
        if (gSideStatuses[GetBattlerSide(actor)] & SIDE_STATUS_TAILWIND)
            return -10000;
        if ((plan & EC_BATTLE_PLAN_TAILWIND) && !(gFieldStatuses & STATUS_FIELD_TRICK_ROOM))
        {
            u32 crossings = 0;
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                    if (IsBattlerAlive(ally) && IsBattlerAlly(actor, ally) && IsBattlerAlive(foe)
                     && !IsBattlerAlly(actor, foe) && gAiLogicData->speedStats[ally] <= gAiLogicData->speedStats[foe]
                     && gAiLogicData->speedStats[ally] * 2 > gAiLogicData->speedStats[foe])
                        crossings++;
            return min(60, crossings * 20);
        }
    }
    if (IsStatRaisingMove(move) && (plan & EC_BATTLE_PLAN_SETUP)
     // A neutral Drum score can reflect isolated incoming damage, before
     // the concrete partner's redirection or Fake Out is considered. The
     // pair trial already checks action-time HP, berry recovery and survival;
     // let that supported setup earn the existing bonus. Preserve negative
     // viability judgments and never reward an already-maximized Attack.
     && (action->score > AI_SCORE_DEFAULT
         || (effect == EFFECT_BELLY_DRUM && action->score == AI_SCORE_DEFAULT))
     && AI_CanAnyStatChange(actor, actor, move))
        return 35;
    s32 score = EC_PerishPlanScore(actor, move);
    if (effect == EFFECT_PROTECT
     && GetProtectType(GetMoveProtectMethod(move)) == PROTECT_TYPE_SINGLE)
    {
        enum Move lastMove = gLastMoves[actor];
        if (lastMove != MOVE_NONE && lastMove != MOVE_UNAVAILABLE
         && gBattleMoveEffects[GetMoveEffect(lastMove)].usesProtectCounter)
        {
            // A repeat guard must buy a meaningful advantage, not win on
            // small one-turn survival differences. Use the same 35-point
            // commitment margin as voluntary switches. This is a policy
            // cost, additional to the native success odds, not a move ban.
            // Include failed attempts: their native counter resets, but
            // that must not make repeated waiting a free default action.
            // Modern side guards remain freely repeatable; useful self
            // guards can still win through healing, partner payoff or Perish.
            score -= 35;
        }
    }
    return score;
}

static u32 PairChangeStage(u8 *stage, s32 change)
{
    u32 old = *stage;
    u32 next = min(MAX_STAT_STAGE, max(MIN_STAT_STAGE, (s32)old + change));
    u32 oldNum = old >= DEFAULT_STAT_STAGE ? 2 + old - DEFAULT_STAT_STAGE : 2;
    u32 oldDen = old >= DEFAULT_STAT_STAGE ? 2 : 2 + DEFAULT_STAT_STAGE - old;
    u32 nextNum = next >= DEFAULT_STAT_STAGE ? 2 + next - DEFAULT_STAT_STAGE : 2;
    u32 nextDen = next >= DEFAULT_STAT_STAGE ? 2 : 2 + DEFAULT_STAT_STAGE - next;
    *stage = next;
    return 100 * nextNum * oldDen / (nextDen * oldNum);
}

static void CachePairConditionalBoost(struct PairEvaluation *ev, enum BattlerId actor, u32 index, enum Move move)
{
    enum BattlerId partner = GetPartnerBattler(actor);
    enum Ability ability = gAiLogicData->abilities[actor];
    enum Ability partnerAbility = gAiLogicData->abilities[partner];
    bool32 partnerBoost = IsBattleMoveSpecial(move) && IsBattlerAlive(partner)
        && (ability == ABILITY_PLUS || ability == ABILITY_MINUS)
        && (partnerAbility == ABILITY_PLUS || partnerAbility == ABILITY_MINUS)
        && (ability != partnerAbility || B_PLUS_MINUS_INTERACTION >= GEN_5);
    bool32 itemBoost = !partnerBoost && !IsBattleMoveStatus(move)
        && (ev->itemRemovers & ~(1u << actor))
        && (gAiLogicData->holdEffects[actor] == HOLD_EFFECT_LIFE_ORB
            || (IsBattleMoveSpecial(move) && gBattleMons[actor].species == SPECIES_CLAMPERL
                && gAiLogicData->holdEffects[actor] == HOLD_EFFECT_DEEP_SEA_TOOTH));
    bool32 defeatist = ability == ABILITY_DEFEATIST && gBattleMons[actor].item == ITEM_NONE && !IsBattleMoveStatus(move)
        && GetMoveEffect(move) != EFFECT_FLAIL && GetMoveEffect(move) != EFFECT_FINAL_GAMBIT && GetMoveEffect(move) != EFFECT_ENDEAVOR;
    if (!IsBattlerAlive(actor) || (!partnerBoost && !itemBoost && !defeatist)
     || IsMoveUnusable(index, gBattleMons[actor].moves[index], gAiLogicData->moveLimitations[actor])
     || GetMoveEffect(move) == EFFECT_POWER_BASED_ON_USER_HP
     || GetMoveEffect(move) == EFFECT_PSYWAVE
     || IsMultiHitMove(move) || GetMoveStrikeCount(move) > 1 || !GetMovePower(move)
     || GetActiveGimmick(actor) == GIMMICK_DYNAMAX || GetActiveGimmick(actor) == GIMMICK_Z_MOVE)
        return;
    struct AiCalcValues calc = {.move = move, .weather = ev->weather, .terrain = gFieldTimers.terrain};
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
        if (!IsBattlerAlly(actor, target) && IsBattlerAlive(target)
         && gAiLogicData->simulatedDmg[actor][target][index].maximum)
        {
            // Native SpA rounding and the damage formula's constant mean
            // losing these boosts is not a flat ratio of final damage.
            // Two raw anchors per relevant target, never per candidate pair.
            struct SimulatedDamage boosted = defeatist ? AI_CalcDefeatistDamage(&calc, actor, target, TRUE)
                : itemBoost ? AI_CalcItemBoostDamage(&calc, actor, target, TRUE)
                : AI_CalcPartnerBoostDamage(&calc, actor, target, TRUE);
            struct SimulatedDamage unboosted = defeatist ? AI_CalcDefeatistDamage(&calc, actor, target, FALSE)
                : itemBoost ? AI_CalcItemBoostDamage(&calc, actor, target, FALSE)
                : AI_CalcPartnerBoostDamage(&calc, actor, target, FALSE);
            if (!boosted.maximum || !unboosted.maximum)
                continue;
            ev->conditionalBoosted[actor][index][target] = (struct PairDamageRange){boosted.minimum, boosted.median, boosted.maximum};
            ev->conditionalUnboosted[actor][index][target] = (struct PairDamageRange){unboosted.minimum, unboosted.median, unboosted.maximum};
            ev->conditionalBoostTargets[actor][index] |= 1u << target;
            if (itemBoost)
                ev->itemBoost[actor] |= 1u << index;
        }
}

static struct SimulatedDamage PairConditionalBoostDamage(const struct PairEvaluation *ev, enum BattlerId actor,
    enum BattlerId target, u32 index, u32 partnerChance)
{
    const struct PairDamageRange *strong = &ev->conditionalBoosted[actor][index][target];
    const struct PairDamageRange *weak = &ev->conditionalUnboosted[actor][index][target];
    struct SimulatedDamage damage = {
        .minimum = partnerChance == 100 ? strong->minimum : weak->minimum,
        .median = (strong->median * partnerChance + weak->median * (100 - partnerChance)) / 100,
        .maximum = partnerChance == 0 ? weak->maximum : strong->maximum,
    };
    // Uncertain partner survival retains both possible endpoints, not an
    // invented certain intermediate hit/KO. The mixed median is approximate.
    damage.random = damage.median;
    return damage;
}

static void CachePairHpPower(struct PairEvaluation *ev, enum BattlerId actor, u32 index, enum Move move)
{
    if (!IsBattlerAlive(actor) || GetMoveEffect(move) != EFFECT_POWER_BASED_ON_USER_HP
     || IsMoveUnusable(index, gBattleMons[actor].moves[index], gAiLogicData->moveLimitations[actor])
     || GetActiveGimmick(actor) == GIMMICK_DYNAMAX || GetActiveGimmick(actor) == GIMMICK_Z_MOVE)
        return;
    // These abilities introduce HP/power breakpoints, not a linear damage
    // curve. Keep their existing forecast until separately modeled.
    switch (gAiLogicData->abilities[actor])
    {
    case ABILITY_TECHNICIAN:
    case ABILITY_DEFEATIST:
    case ABILITY_BLAZE:
    case ABILITY_TORRENT:
    case ABILITY_OVERGROW:
    case ABILITY_SWARM:
    case ABILITY_PARENTAL_BOND:
        return;
    default:
        break;
    }
    struct AiCalcValues calc = {.move = move, .weather = ev->weather, .terrain = gFieldTimers.terrain};
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
        if (target != actor && IsBattlerAlive(target)
         && (PairSpread(move)
             ? (!IsBattlerAlly(actor, target) || GetMoveTarget(move) != TARGET_BOTH)
             : PairTargetIsLegal(actor, target, move)))
        {
            struct SimulatedDamage low = AI_CalcHpPowerDamage(&calc, actor, target, 1);
            struct SimulatedDamage high = AI_CalcHpPowerDamage(&calc, actor, target, gBattleMons[actor].maxHP);
            ev->hpPowerLow[actor][index][target] = (struct PairDamageRange){low.minimum, low.median, low.maximum};
            ev->hpPowerHigh[actor][index][target] = (struct PairDamageRange){high.minimum, high.median, high.maximum};
            ev->hpPowerTargets[actor][index] |= 1u << target;
        }
}

static u32 PairInterpolateHpDamage(u32 low, u32 high, u32 power, u32 lowPower, u32 highPower)
{
    // Interpolate native rounded endpoints rather than multiplying low-HP
    // damage upward (which also multiplies the formula's constant term).
    // Interior rounding remains approximate, like the other pair modifiers.
    return max(0, (s32)low + ((s32)high - (s32)low) * (s32)(power - lowPower)
        / (s32)max(1, highPower - lowPower));
}

static struct SimulatedDamage PairHpPowerDamage(const struct PairEvaluation *ev, enum BattlerId actor,
    enum BattlerId target, u32 index, enum Move move, u32 hp)
{
    const struct PairDamageRange *low = &ev->hpPowerLow[actor][index][target];
    const struct PairDamageRange *high = &ev->hpPowerHigh[actor][index][target];
    u32 highPower = GetMovePower(move);
    u32 lowPower = max(1, highPower / gBattleMons[actor].maxHP);
    u32 power = max(1, min(hp, gBattleMons[actor].maxHP) * highPower / gBattleMons[actor].maxHP);
    struct SimulatedDamage damage = {
        .minimum = PairInterpolateHpDamage(low->minimum, high->minimum, power, lowPower, highPower),
        .median = PairInterpolateHpDamage(low->median, high->median, power, lowPower, highPower),
        .maximum = PairInterpolateHpDamage(low->maximum, high->maximum, power, lowPower, highPower),
    };
    damage.random = damage.median;
    return damage;
}

// One guaranteed offensive drop per supported move. STAT_HP is the sentinel.
static u32 PairOffensiveDropStat(enum Move move)
{
    switch (move)
    {
    case MOVE_CHARM:
    case MOVE_LUNGE:
        return STAT_ATK;
    case MOVE_SNARL:
    case MOVE_STRUGGLE_BUG:
    case MOVE_SKITTER_SMACK:
        return STAT_SPATK;
    default:
        return STAT_HP;
    }
}

static enum Stat PairSelfDefenseStat(enum Move move)
{
    switch (move)
    {
    case MOVE_COTTON_GUARD:
    case MOVE_IRON_DEFENSE:
        return STAT_DEF;
    case MOVE_QUIVER_DANCE:
    case MOVE_CALM_MIND:
        return STAT_SPDEF;
    default:
        return STAT_HP; // Not a supported immediate defensive setup.
    }
}

static void CachePairMoveEffects(struct PairEvaluation *ev, enum BattlerId actor, u32 index)
{
    enum Move move = gBattleMons[actor].moves[index];
    enum Move executedMove = GetMoveEffect(move) == EFFECT_NATURE_POWER ? GetNaturePowerMove() : move;
    enum Stat defenseStat = PairSelfDefenseStat(move);
    if (defenseStat != STAT_HP && IsBattlerAlive(actor))
    {
        struct BattleCalcValues cv = {.battlerAtk = actor, .battlerDef = actor, .move = move, .moveEffect = GetMoveEffect(move)};
        memcpy(cv.abilities, gAiLogicData->abilities, sizeof(cv.abilities));
        memcpy(cv.holdEffects, gAiLogicData->holdEffects, sizeof(cv.holdEffects));
        struct StatChange st = {.onlyChecking = TRUE, .certain = TRUE, .stat = defenseStat};
        st.stage = GetAdjustedStatStage(GetStatStage(st.stat, GetMoveAdditionalEffectById(move, 0)), cv.abilities[actor], FALSE);
        if (CanStatChange(&cv, &st))
            ev->selfDefenseDelta[actor][index] = st.stage;
    }
    if (move == MOVE_SHELL_SMASH && IsBattlerAlive(actor))
    {
        struct BattleCalcValues cv = {.battlerAtk = actor, .battlerDef = actor, .move = move, .moveEffect = GetMoveEffect(move)};
        memcpy(cv.abilities, gAiLogicData->abilities, sizeof(cv.abilities));
        memcpy(cv.holdEffects, gAiLogicData->holdEffects, sizeof(cv.holdEffects));
        for (u32 stat = 0; stat < 2; stat++)
        {
            // Native self-lowering bypasses opposing stat-loss protection.
            // Cache eligibility outside candidate pairs; Simple/Contrary are
            // actual stage changes, not a generic fixed setup penalty.
            struct StatChange st = {.onlyChecking = TRUE, .certain = TRUE,
                .stat = stat == 0 ? STAT_DEF : STAT_SPDEF};
            st.stage = GetAdjustedStatStage(-GetStatStage(st.stat, GetMoveAdditionalEffectById(move, 0)),
                cv.abilities[actor], FALSE);
            if (CanStatChange(&cv, &st))
                ev->smashDefenseDelta[actor][stat] = st.stage;
        }
    }
    CachePairWeatherAccuracy(ev, actor, index, executedMove);
    CachePairHpPower(ev, actor, index, executedMove);
    CachePairConditionalBoost(ev, actor, index, executedMove);
    if (AI_MoveBreaksScreensBeforeDamage(executedMove))
        ev->screenBreakerMoves[actor] |= 1u << index;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
        if (target != actor && IsBattlerAlive(actor) && IsBattlerAlive(target))
        {
            ev->contactDamage[actor][index][target] = AI_GetContactDamage(actor, target, executedMove,
                gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor], gAiLogicData->holdEffects[target],
                gAiLogicData->abilities[target]);
            if (AI_CanScreenReduceDamage(actor, target, executedMove))
                ev->screenTargets[actor][index] |= 1u << target;
        }
    for (u32 effectId = 0; effectId < GetMoveAdditionalEffectCount(executedMove); effectId++)
    {
        const struct AdditionalEffect *additionalEffect = GetMoveAdditionalEffectById(executedMove, effectId);
        if (additionalEffect->moveEffect != MOVE_EFFECT_ABSORB || additionalEffect->chance
         || additionalEffect->onChargeTurnOnly || additionalEffect->onlyIfTargetRaisedStats || additionalEffect->pledgeCombo)
            continue;
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
            if (target != actor && IsBattlerAlive(target) && !DoesSubstituteBlockMove(actor, target, executedMove))
                ev->drainPercent[actor][index][target] = additionalEffect->argument.absorbPercentage;
        // Native moves can drain Substitute HP, but this bounded scorer has
        // no separate Substitute damage pool. Omit that extra credit rather
        // than using the occupant's larger HP pool as fictitious healing.
        break;
    }
    if (GetMoveEffect(move) == EFFECT_PROTECT)
    {
        enum ProtectMethod method = GetMoveProtectMethod(move);
        u32 uses = gBattleMons[actor].volatiles.consecutiveMoveUses;
        enum Move lastMove = gLastResultingMoves[actor];
        // Mirror TryResetConsecutiveUseCounter without changing native state.
        if (lastMove == MOVE_UNAVAILABLE)
            uses = 0;
        else
        {
            enum BattleMoveEffects lastEffect = GetMoveEffect(lastMove);
            if (!gBattleMoveEffects[lastEffect].usesProtectCounter
             && (GetConfig(B_ALLY_SWITCH_FAIL_CHANCE) < GEN_9 || lastEffect != EFFECT_ALLY_SWITCH))
                uses = 0;
        }
        if ((method == PROTECT_WIDE_GUARD && GetConfig(B_WIDE_GUARD) >= GEN_6)
         || (method == PROTECT_QUICK_GUARD && GetConfig(B_QUICK_GUARD) >= GEN_6)
         || method == PROTECT_CRAFTY_SHIELD)
            uses = 0;
        // Share the native capped denominator without sampling RNG.
        // Percent rounding yields 33/11/3 for modern repeated guards.
        ev->protectChance[actor][index] = 100 / GetConsecutiveMoveSuccessDenominator(uses);
        return;
    }
    if (GetMoveEffect(move) == EFFECT_NATURE_POWER)
    {
        // Damage already resolves the native submove in AI_CalcDamageInternal,
        // but the ordinary accuracy cache belongs to the always-hit wrapper.
        // Resolve accuracy once per board/target, never per candidate pair.
        if (IsBattlerAlive(actor) && !IsMoveUnusable(index, move, gAiLogicData->moveLimitations[actor]))
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (actor != target && IsBattlerAlive(target))
                    gAiLogicData->moveAccuracy[actor][target][index] = AI_GetMoveAccuracy(gAiLogicData, actor, target, GetNaturePowerMove());
        return;
    }
    bool32 thread = move == MOVE_TOXIC_THREAD;
    bool32 sleep = move == MOVE_SPORE || move == MOVE_SING || move == MOVE_SLEEP_POWDER;
    u32 attackDropStat = PairOffensiveDropStat(move);
    bool32 electricParalysis = move == MOVE_THUNDER_WAVE || move == MOVE_NUZZLE;
    bool32 primaryParalysis = move == MOVE_THUNDER_WAVE || move == MOVE_GLARE;
    bool32 paralysis = electricParalysis || move == MOVE_GLARE || move == MOVE_BODY_SLAM;
    bool32 encore = GetMoveEffect(move) == EFFECT_ENCORE;
    bool32 speedDrop = move == MOVE_ELECTROWEB || move == MOVE_ICY_WIND || move == MOVE_ROCK_TOMB || thread;
    if (move != MOVE_COACHING && move != MOVE_ACID_SPRAY && !speedDrop && !sleep && !encore && !paralysis && !attackDropStat)
        return;
    if (paralysis)
        ev->paralysisChance[actor][index] = move == MOVE_BODY_SLAM
            ? min(100, CalcSecondaryEffectChance(actor, gAiLogicData->abilities[actor], GetMoveAdditionalEffectById(move, 0)))
            : 100;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (target == actor || !IsBattlerAlive(target)
         || (move == MOVE_COACHING && target != GetPartnerBattler(actor))
         || ((speedDrop || sleep || encore || paralysis || attackDropStat) && IsBattlerAlly(actor, target)))
            continue;
        struct BattleCalcValues cv = {.battlerAtk = actor, .battlerDef = target, .move = move, .moveEffect = GetMoveEffect(move)};
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            cv.holdEffects[battler] = gAiLogicData->holdEffects[battler];
            cv.abilities[battler] = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
                gAiLogicData->abilities[battler], cv.holdEffects[battler], move);
        }
        if (move == MOVE_COACHING && cv.abilities[target] == ABILITY_GOOD_AS_GOLD)
            continue;
        if (attackDropStat)
        {
            if ((IsSoundMove(move) && cv.abilities[target] == ABILITY_SOUNDPROOF)
             || DoesSubstituteBlockMove(actor, target, move)
             || (!IsBattleMoveStatus(move)
                 && (IsSheerForceAffected(move, cv.abilities[actor])
                     || IsAdditionalEffectBlocked(actor, cv.abilities[actor], target, cv.abilities[target], move))))
                continue;
            if (move == MOVE_CHARM)
            {
                // Charm's primary drop is not blocked by Covert Cloak or
                // Shield Dust, but reflected/status-immune targets get no
                // fictitious defensive credit in this bounded forecast.
                if (cv.abilities[target] == ABILITY_GOOD_AS_GOLD
                 || cv.abilities[target] == ABILITY_MAGIC_BOUNCE || gProtectStructs[target].bounceMove
                 || (GetConfig(B_PRANKSTER_DARK_TYPES) >= GEN_7
                     && cv.abilities[actor] == ABILITY_PRANKSTER && IS_BATTLER_OF_TYPE(target, TYPE_DARK))
                 || (IsSemiInvulnerable(target, CHECK_ALL)
                     && !BreaksThroughSemiInvulnerableState(actor, target, cv.abilities[actor], cv.abilities[target],
                         move, gBattleMons[target].volatiles.semiInvulnerable)))
                    continue;
                if (AI_GetMovePriority(actor, cv.abilities[actor], move) > 0)
                {
                    bool32 blocked = IsPsychicTerrainAffected(target, cv.abilities[target], cv.holdEffects[target], gFieldTimers.terrain);
                    for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                        if (IsBattlerAlive(ally) && IsBattlerAlly(target, ally) && IsDazzlingAbility(cv.abilities[ally]))
                            blocked = TRUE;
                    if (blocked)
                        continue;
                }
            }
            const struct AdditionalEffect *additional = GetMoveAdditionalEffectById(move, 0);
            if (!MoveEffectIsGuaranteed(actor, cv.abilities[actor], additional))
                continue;
            struct StatChange st = {.onlyChecking = TRUE, .stat = attackDropStat};
            st.stage = GetAdjustedStatStage(-GetStatStage(attackDropStat, additional), cv.abilities[target], FALSE);
            if (CanStatChange(&cv, &st))
                // Keep eligibility per move: sound Snarl bypasses Substitute,
                // while Struggle Bug does not, even on the same moveset.
                ev->attackDropDelta[actor][index][target] = st.stage;
            continue;
        }
        // Primary status effects need their own immunity checks, not damage's
        // zero cache or secondary-effect Shield Dust/Covert Cloak checks.
        if ((thread || sleep || encore || primaryParalysis) && (cv.abilities[target] == ABILITY_GOOD_AS_GOLD
             || cv.abilities[target] == ABILITY_MAGIC_BOUNCE
             || gProtectStructs[target].bounceMove
             || (GetConfig(B_PRANKSTER_DARK_TYPES) >= GEN_7
                 && cv.abilities[actor] == ABILITY_PRANKSTER && IS_BATTLER_OF_TYPE(target, TYPE_DARK))
             || (IsSemiInvulnerable(target, CHECK_ALL)
                 && !BreaksThroughSemiInvulnerableState(actor, target, cv.abilities[actor], cv.abilities[target],
                     move, gBattleMons[target].volatiles.semiInvulnerable))))
            continue;
        if (paralysis)
        {
            if (!primaryParalysis
             && (IsSheerForceAffected(move, cv.abilities[actor])
                 || IsAdditionalEffectBlocked(actor, cv.abilities[actor], target, cv.abilities[target], move)
                 || !ev->paralysisChance[actor][index]))
                continue;
            s32 priority = AI_GetMovePriority(actor, cv.abilities[actor], move);
            if (priority > 0)
            {
                if (IsPsychicTerrainAffected(target, cv.abilities[target], cv.holdEffects[target], gFieldTimers.terrain))
                    continue;
                bool32 priorityBlocked = FALSE;
                for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                    if (IsBattlerAlive(ally) && IsBattlerAlly(target, ally) && IsDazzlingAbility(cv.abilities[ally]))
                        priorityBlocked = TRUE;
                if (priorityBlocked)
                    continue;
            }
            // Native status/type checks plus Thunder Wave's absorbing abilities.
            // Do not award slowdown while omitting an immediate cure, reflected
            // status, or a newly enabled offensive/defensive status benefit.
            if (DoesSubstituteBlockMove(actor, target, move)
             || !CanSetNonVolatileStatus(actor, target, cv.abilities[actor], cv.abilities[target], MOVE_EFFECT_PARALYSIS, CHECK_TRIGGER)
             || (move != MOVE_GLARE && gAiLogicData->effectiveness[actor][target][index] == UQ_4_12(0.0))
             || (move == MOVE_GLARE && B_GLARE_GHOST < GEN_4 && IS_BATTLER_OF_TYPE(target, TYPE_GHOST))
             || (electricParalysis && (cv.abilities[target] == ABILITY_VOLT_ABSORB
                 || cv.abilities[target] == ABILITY_MOTOR_DRIVE
                 || (cv.abilities[target] == ABILITY_LIGHTNING_ROD && GetConfig(B_REDIRECT_ABILITY_IMMUNITY) >= GEN_5)))
             || cv.abilities[target] == ABILITY_GUTS || cv.abilities[target] == ABILITY_MARVEL_SCALE
             || cv.abilities[target] == ABILITY_SYNCHRONIZE
             || HasMoveWithEffect(target, EFFECT_FACADE)
             || cv.holdEffects[target] == HOLD_EFFECT_CURE_PAR || cv.holdEffects[target] == HOLD_EFFECT_CURE_STATUS)
                continue;
            ev->paralysisTargets[actor][index] |= 1u << target;
            struct BattlePokemon saved = gBattleMons[target];
            gBattleMons[target].status1 = STATUS1_PARALYSIS;
            ev->paralyzedSpeed[target] = GetBattlerTotalSpeedStat(target, gAiLogicData->abilities[target], cv.holdEffects[target]);
            gBattleMons[target] = saved;
            if (electricParalysis && !IsMoveRedirectionPrevented(actor, move, cv.abilities[actor]))
                for (enum BattlerId other = 0; other < gBattlersCount; other++)
                    if (other != actor && IsBattlerAlive(other) && cv.abilities[other] == ABILITY_LIGHTNING_ROD
                     && (B_REDIRECT_ABILITY_ALLIES >= GEN_4 || !IsBattlerAlly(actor, other)))
                        ev->paralysisRedirectors[actor][index] |= 1u << other;
            continue;
        }
        if (encore)
        {
            enum Move lastMove = gLastMoves[target];
            s32 priority = AI_GetMovePriority(actor, cv.abilities[actor], move);
            if (lastMove == MOVE_NONE || lastMove == MOVE_UNAVAILABLE || IsMoveEncoreBanned(lastMove)
             || GetMoveEffect(lastMove) != EFFECT_PROTECT
             || gBattleMons[target].volatiles.encoredMove != MOVE_NONE
             || gBattleMons[target].volatiles.multipleTurns || gBattleMons[target].volatiles.rechargeTimer
             || gProtectStructs[target].noValidMoves
             || GetActiveGimmick(target) == GIMMICK_DYNAMAX || GetActiveGimmick(target) == GIMMICK_Z_MOVE
             || (B_MENTAL_HERB >= GEN_5 && cv.holdEffects[target] == HOLD_EFFECT_MENTAL_HERB)
             || (priority > 0 && IsPsychicTerrainAffected(target, cv.abilities[target], cv.holdEffects[target], gFieldTimers.terrain))
             || DoesSubstituteBlockMove(actor, target, move))
                continue;
            // These guards already have a complete block rule in this pass.
            // Do not invent contact penalties or Mat Block/Crafty Shield's
            // different status/first-turn contracts for other variants.
            enum ProtectMethod method = GetMoveProtectMethod(lastMove);
            if (method != PROTECT_NORMAL && method != PROTECT_WIDE_GUARD && method != PROTECT_QUICK_GUARD)
                continue;
            bool32 sideAbilityBlocks = FALSE;
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                if (IsBattlerAlive(ally) && IsBattlerAlly(target, ally)
                 && (cv.abilities[ally] == ABILITY_AROMA_VEIL || (priority > 0 && IsDazzlingAbility(cv.abilities[ally]))))
                    sideAbilityBlocks = TRUE;
            if (sideAbilityBlocks)
                continue;
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                if (gBattleMons[target].moves[slot] == lastMove && gBattleMons[target].pp[slot]
                 && !IsMoveUnusable(slot, lastMove, gAiLogicData->moveLimitations[target]))
                {
                    ev->encoreGuardIndex[actor][target] = slot + 1;
                    ev->encoreGuardPriority[target] = AI_GetMovePriority(target, gAiLogicData->abilities[target], lastMove);
                    break;
                }
            continue;
        }
        if (sleep)
        {
            // Conservatively omit cure-berry targets. This does not simulate
            // earlier item removal or berries temporarily blocked by Unnerve.
            if (DoesSubstituteBlockMove(actor, target, move)
             || ((move == MOVE_SPORE || move == MOVE_SLEEP_POWDER)
                 && !IsAffectedByPowderMove(target, cv.abilities[target], cv.holdEffects[target]))
             || (move == MOVE_SING && cv.abilities[target] == ABILITY_SOUNDPROOF)
             || cv.holdEffects[target] == HOLD_EFFECT_CURE_SLP
             || cv.holdEffects[target] == HOLD_EFFECT_CURE_STATUS)
                continue;
            // The native query resets this scratch flag, even when checking.
            bool32 savedClauseOverride = gBattleStruct->sleepClauseNotBlocked;
            bool32 canSleep = CanBeSlept(actor, target, cv.abilities[target], BLOCKED_BY_SLEEP_CLAUSE);
            gBattleStruct->sleepClauseNotBlocked = savedClauseOverride;
            // Do not credit denial while omitting newly activated Marvel
            // Scale's defense benefit. The native sleep opinion still applies.
            if (canSleep && cv.abilities[target] != ABILITY_MARVEL_SCALE)
            {
                u32 chance = 100;
                // Champions: counter 2 has weight 1, counter 3 has weight 2.
                // Early Bird subtracts two, so only 2/3 lose this action.
                if (cv.abilities[target] == ABILITY_EARLY_BIRD)
                    chance = GetConfig(B_SLEEP_TURNS) >= GEN_5 ? 66 : GetConfig(B_SLEEP_TURNS) >= GEN_3 ? 75 : 85;
                ev->sleepDenialChance[actor][index][target] = chance;
            }
            continue;
        }
        // This narrow forecast does not enact Thread's new poison status.
        // Do not award a "free" slowdown when that same poison would enable
        // an immediate Speed/Attack ability absent from the damage cache.
        if (thread && (cv.abilities[target] == ABILITY_QUICK_FEET
             || cv.abilities[target] == ABILITY_GUTS || cv.abilities[target] == ABILITY_TOXIC_BOOST)
         && CanBePoisoned(actor, target, cv.abilities[actor], cv.abilities[target]))
            continue;
        if ((move == MOVE_ACID_SPRAY || speedDrop)
         && (DoesSubstituteBlockMove(actor, target, move)
             || (!thread && (IsSheerForceAffected(move, cv.abilities[actor])
                 || IsAdditionalEffectBlocked(actor, cv.abilities[actor], target, cv.abilities[target], move)))))
            continue;
        const enum Stat stats[] = {STAT_ATK, STAT_DEF, STAT_SPDEF, STAT_SPEED};
        for (u32 i = 0; i < ARRAY_COUNT(stats); i++)
        {
            if (speedDrop ? i != 3 : i == 3 || ((move == MOVE_COACHING) != (i < 2)))
                continue;
            struct StatChange st = {.onlyChecking = TRUE, .stat = stats[i]};
            s32 delta = GetStatStage(stats[i], GetMoveAdditionalEffectById(move, 0));
            st.stage = GetAdjustedStatStage(move == MOVE_COACHING ? delta : -delta, cv.abilities[target], FALSE);
            // CanStatChange includes a legacy AI preference against slowing
            // Speed Boost. It is not a native immunity and must not affect
            // this read-only forecast. Speed Boost has no stat-loss protection.
            if (speedDrop && cv.abilities[target] == ABILITY_SPEED_BOOST)
                cv.abilities[target] = ABILITY_NONE;
            if (CanStatChange(&cv, &st))
            {
                if (speedDrop)
                    ev->speedDelta[actor][index][target] = st.stage;
                else
                    ev->statDelta[actor][target][i] = st.stage;
            }
        }
    }
}

static void PairChangeStat(u8 *stage, u32 *modifier, s32 delta, u32 chance, bool32 defense)
{
    u32 old = *stage;
    u32 next = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE, (s32)old + delta));
    u32 numerator = gStatStageRatios[next][0] * gStatStageRatios[old][1];
    u32 denominator = gStatStageRatios[next][1] * gStatStageRatios[old][0];
    // Defense modifies incoming damage inversely. Blend hit/survival chance,
    // rather than awarding a certain drop after an uncertain Acid Spray hit.
    u32 ratioNumerator = defense ? denominator : numerator;
    u32 ratioDenominator = defense ? numerator : denominator;
    // Keep the stage ratio rational until the final division. In particular,
    // Shell Smash's 150% incoming damage then White Herb's 2/3 must restore
    // 100%, not 99% from prematurely rounding 2/3 to 66%.
    *modifier = *modifier * ((100 - chance) * ratioDenominator + chance * ratioNumerator)
        / (100 * ratioDenominator);
    *stage = next;
}

static void PairApplyStatChanges(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target,
    enum Move move, u8 stages[MAX_BATTLERS_COUNT][4], u32 modifiers[MAX_BATTLERS_COUNT][4], u32 chance, u32 *usedItems)
{
    s32 deltas[4] = {0};
    bool32 herb = FALSE;
    for (u32 stat = 0; stat < 3; stat++)
    {
        if ((move == MOVE_COACHING) == (stat < 2))
            deltas[stat] = ev->statDelta[actor][target][stat];
        if (deltas[stat] < 0 && stages[target][stat] + deltas[stat] < DEFAULT_STAT_STAGE
         && gAiLogicData->holdEffects[target] == HOLD_EFFECT_WHITE_HERB && !(*usedItems & (1u << target)))
            herb = TRUE;
    }
    for (u32 stat = 0; stat < 4; stat++)
    {
        if (herb && stages[target][stat] + deltas[stat] < DEFAULT_STAT_STAGE)
            deltas[stat] = DEFAULT_STAT_STAGE - stages[target][stat];
        PairChangeStat(&stages[target][stat], &modifiers[target][stat], deltas[stat], chance, stat == 1 || stat == 2);
    }
    if (herb)
        *usedItems |= 1u << target;
}

static u32 PairCriticalModifier(u32 modifier, u32 old, u32 next, bool32 defense)
{
    u32 numerator = gStatStageRatios[next][0] * gStatStageRatios[old][1];
    u32 denominator = gStatStageRatios[next][1] * gStatStageRatios[old][0];
    s32 normalRatio = defense ? 100 * denominator / numerator : 100 * numerator / denominator;
    old = defense ? min(old, DEFAULT_STAT_STAGE) : max(old, DEFAULT_STAT_STAGE);
    next = defense ? min(next, DEFAULT_STAT_STAGE) : max(next, DEFAULT_STAT_STAGE);
    numerator = gStatStageRatios[next][0] * gStatStageRatios[old][1];
    denominator = gStatStageRatios[next][1] * gStatStageRatios[old][0];
    s32 criticalRatio = defense ? 100 * denominator / numerator : 100 * numerator / denominator;
    return normalRatio == 100 ? 100 : 100 + ((s32)modifier - 100) * (criticalRatio - 100) / (normalRatio - 100);
}

static u32 PairStatDamageModifier(enum BattlerId actor, enum BattlerId target, enum Move move,
    u8 stages[MAX_BATTLERS_COUNT][4], u32 modifiers[MAX_BATTLERS_COUNT][4])
{
    if (modifiers[actor][0] == 100 && modifiers[actor][1] == 100 && modifiers[actor][2] == 100
     && modifiers[actor][3] == 100 && modifiers[target][0] == 100 && modifiers[target][1] == 100 && modifiers[target][2] == 100)
        return 100;
    enum BattleMoveEffects effect = GetMoveEffect(move);
    // These use HP/level/previous damage, not the attack/defense formula.
    switch (effect)
    {
    case EFFECT_LEVEL_DAMAGE:
    case EFFECT_PSYWAVE:
    case EFFECT_FIXED_HP_DAMAGE:
    case EFFECT_FIXED_PERCENT_DAMAGE:
    case EFFECT_FINAL_GAMBIT:
    case EFFECT_REFLECT_DAMAGE:
    case EFFECT_ENDEAVOR:
    case EFFECT_OHKO:
    case EFFECT_BIDE:
        return 100;
    default:
        break;
    }
    bool32 physical = IsBattleMovePhysical(move);
    enum BattlerId attackOwner = effect == EFFECT_FOUL_PLAY ? target : actor;
    enum Ability defenderAbility = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
        gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], move);
    bool32 critical = AI_MoveAlwaysCrits(actor, target, move);
    u32 modifier = 100;
    if (defenderAbility != ABILITY_UNAWARE)
    {
        if (effect == EFFECT_BODY_PRESS)
        {
            // Body Press uses the defense stage offensively (SpDef in Wonder Room).
            u32 stat = physical && !(gFieldStatuses & STATUS_FIELD_WONDER_ROOM) ? 1 : 2;
            modifier = 10000 / max(1, modifiers[actor][stat]);
            if (critical)
                modifier = PairCriticalModifier(modifier, gBattleMons[actor].statStages[stat == 1 ? STAT_DEF : STAT_SPDEF], stages[actor][stat], FALSE);
        }
        else if (physical)
        {
            modifier = modifiers[attackOwner][0];
            if (critical)
                modifier = PairCriticalModifier(modifier, gBattleMons[attackOwner].statStages[STAT_ATK], stages[attackOwner][0], FALSE);
        }
        else
        {
            modifier = modifiers[actor][3];
            if (critical)
                modifier = PairCriticalModifier(modifier, gBattleMons[actor].statStages[STAT_SPATK], stages[actor][3], FALSE);
        }
    }
    if (gAiLogicData->abilities[actor] != ABILITY_UNAWARE && !MoveIgnoresDefenseEvasionStages(move))
    {
        bool32 usesDefense = (physical || effect == EFFECT_PSYSHOCK)
            && !(gAiLogicData->abilities[actor] == ABILITY_POWER_FISTS && IsPunchingMove(move));
        u32 stat = usesDefense ? 1 : 2;
        u32 defenseModifier = modifiers[target][stat];
        if (critical)
            defenseModifier = PairCriticalModifier(defenseModifier, gBattleMons[target].statStages[usesDefense ? STAT_DEF : STAT_SPDEF], stages[target][stat], TRUE);
        modifier = modifier * defenseModifier / 100;
    }
    return modifier;
}

static u32 PairDrainedHp(u32 damage, u32 percentage, enum HoldEffect holdEffect)
{
    if (!damage)
        return 0;
    u32 amount = damage * percentage / 100;
    if (holdEffect == HOLD_EFFECT_BIG_ROOT)
        amount = amount * 1300 / 1000;
    return max(1, amount);
}

struct PairPopulationOutcome
{
    u32 hp[2];
    u32 alive[2];
    u32 berries[2];
};

static void PairPopulationAccumulate(struct PairPopulationOutcome *out, enum BattlerId actor, enum BattlerId target,
    const u32 *hp, u32 usedItems, u32 mass)
{
    const enum BattlerId battlers[2] = {actor, target};
    for (u32 side = 0; side < 2; side++)
    {
        enum BattlerId battler = battlers[side];
        out->hp[side] += hp[battler] * mass;
        if (hp[battler])
            out->alive[side] += mass;
        if (usedItems & (1u << battler))
            out->berries[side] += mass;
    }
}

static u32 PairScreenFactor(enum Move move, u32 screens)
{
    if ((screens & SIDE_STATUS_AURORA_VEIL)
     || ((screens & SIDE_STATUS_REFLECT) && IsBattleMovePhysical(move))
     || ((screens & SIDE_STATUS_LIGHTSCREEN) && IsBattleMoveSpecial(move)))
        return IsDoubleBattle() ? UQ_4_12(0.667) : UQ_4_12(0.5);
    return UQ_4_12(1.0);
}

static u32 PairApplyScreenRatio(u32 damage, u32 numerator, u32 denominator)
{
    if (!damage || numerator == denominator)
        return damage;
    // Native final modifiers use half-down rounding. Ratios of already-rounded
    // cached anchors remain an approximation; never turn a real hit into zero.
    return max(1, (damage * numerator + (denominator - 1) / 2) / denominator);
}

static bool32 PairScreenEffectApplies(bool32 applyEffects, u32 chance, u32 *effectChance)
{
    if (!chance)
        return FALSE;
    if (chance == 100)
        return TRUE;
    if (!applyEffects)
        return FALSE;
    *effectChance = *effectChance ? min(*effectChance, chance) : chance;
    return TRUE;
}

static bool32 PairMayBeSnatched(enum BattlerId actor, const struct PairAction *actions,
    const u32 *hp, u32 acted, u32 stopped)
{
    for (enum BattlerId next = 0; next < gBattlersCount; next++)
        if (next != actor && hp[next]
         && (gProtectStructs[next].stealMove
             || ((acted & (1u << next)) && !(stopped & (1u << next))
                 && actions[next].index != PAIR_IDLE && actions[next].move == MOVE_SNATCH)))
            return TRUE;
    return FALSE;
}

static void PairApplyPopulationBomb(const struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId target,
    const struct PairAction *action, u32 accuracy, u32 guardChance, u32 actionChance, u32 boost, u32 statPower,
    u32 screenNumerator, u32 screenDenominator, u32 *hp, u32 *survival, u32 *speed, u32 *usedItems)
{
    const struct AiPopulationBombDamage *cache = &gAiLogicData->populationBomb[actor][target];
    struct PairPopulationOutcome out = {0};
    const u32 scale = 40000; // Three native damage anchors, weighted 1:2:1.
    u32 priorActorSurvival = survival[actor];
    u32 priorTargetSurvival = survival[target];
    u32 initialActorHp = hp[actor];
    u32 initialTargetHp = hp[target];
    enum Ability targetAbility = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
        gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], action->executedMove);
    for (u32 roll = 0; roll < 3; roll++)
    {
        u32 trialHp[MAX_BATTLERS_COUNT], trialSpeed[MAX_BATTLERS_COUNT];
        memcpy(trialHp, hp, sizeof(trialHp));
        memcpy(trialSpeed, speed, sizeof(trialSpeed));
        u32 trialItems = *usedItems;
        u32 weight = roll == 1 ? 2 : 1;
        u32 mass = guardChance * actionChance / 100;
        // Protect and prior actor survival are whole-move events, not ten
        // independent chances. Only native per-hit accuracy repeats below.
        PairPopulationAccumulate(&out, actor, target, trialHp, trialItems, (10000 - mass) * weight);
        for (u32 hit = 0; hit < cache->count[roll] && mass; hit++)
        {
            u32 hitAccuracy = hit == 0 || cache->repeatedAccuracy ? accuracy : 100;
            u32 miss = mass * (100 - hitAccuracy) / 100;
            PairPopulationAccumulate(&out, actor, target, trialHp, trialItems, miss * weight);
            mass -= miss;
            if (!mass)
                break;
            u32 damage = cache->strike[roll][hit] * boost / 100 * statPower / 100;
            damage = PairApplyScreenRatio(damage, screenNumerator, screenDenominator);
            if (damage >= trialHp[target])
            {
                bool32 endure = gBattleMons[target].volatiles.endured;
                if (trialHp[target] == gBattleMons[target].maxHP)
                {
                    if (GetConfig(B_STURDY) >= GEN_5 && targetAbility == ABILITY_STURDY)
                        endure = TRUE;
                    else if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_FOCUS_SASH && !(trialItems & (1u << target)))
                    {
                        endure = TRUE;
                        trialItems |= 1u << target;
                    }
                }
                if (endure)
                    damage = trialHp[target] - 1;
            }
            damage = min(damage, trialHp[target]);
            trialHp[target] -= damage;
            if (damage)
                trialHp[actor] -= min(trialHp[actor], ev->contactDamage[actor][action->index][target]);
            // Native Helmet can faint the attacker on the target's KO hit.
            // Its death may also free the target's Sitrus from Unnerve.
            PairTryHealingBerry(ev, actor, trialHp, trialSpeed, &trialItems);
            PairTryHealingBerry(ev, target, trialHp, trialSpeed, &trialItems);
            if (!trialHp[actor] || !trialHp[target] || hit + 1 == cache->count[roll])
            {
                PairPopulationAccumulate(&out, actor, target, trialHp, trialItems, mass * weight);
                mass = 0;
            }
            else if (!cache->repeatedAccuracy && cache->minimumStrikes == 4 && hit + 1 >= 4)
            {
                // Loaded Dice samples a uniform 4..10 count after one check.
                u32 stop = mass / (AI_POPULATION_BOMB_STRIKES - hit);
                PairPopulationAccumulate(&out, actor, target, trialHp, trialItems, stop * weight);
                mass -= stop;
            }
        }
    }
    // If an earlier uncertain attack already removed the target, this local
    // no-retarget forecast cannot charge its Helmet or consume our berry.
    u32 actorAlive = (out.alive[0] * priorTargetSurvival + scale * (100 - priorTargetSurvival)) / 100;
    u32 actorHp = (out.hp[0] / 100) * priorTargetSurvival + (out.hp[0] % 100) * priorTargetSurvival / 100
        + initialActorHp * (scale / 100) * (100 - priorTargetSurvival);
    hp[actor] = actorAlive ? DIV_ROUND_UP(actorHp, actorAlive) : 0;
    survival[actor] = actorAlive ? max(1, priorActorSurvival * actorAlive / scale) : 0;
    // On branches where the actor never survived to act, the target retains
    // its prior HP. Actor HP above is conditional on the actor being alive.
    u32 targetAlive = (out.alive[1] * priorActorSurvival + scale * (100 - priorActorSurvival)) / 100;
    u32 targetHp = (out.hp[1] / 100) * priorActorSurvival + (out.hp[1] % 100) * priorActorSurvival / 100
        + initialTargetHp * (scale / 100) * (100 - priorActorSurvival);
    hp[target] = targetAlive ? DIV_ROUND_UP(targetHp, targetAlive) : 0;
    survival[target] = targetAlive ? max(1, survival[target] * targetAlive / scale) : 0;
    const enum BattlerId battlers[2] = {actor, target};
    for (u32 side = 0; side < 2; side++)
    {
        enum BattlerId battler = battlers[side];
        if (out.berries[side] && (side == 0 ? priorTargetSurvival : priorActorSurvival)
         && !(*usedItems & (1u << battler)))
        {
            // A merged HP state cannot carry two item histories. Conservatively
            // prevent a second berry heal; credit Unburden only if certain.
            *usedItems |= 1u << battler;
            if (out.berries[side] == scale && (side == 0 ? priorTargetSurvival : priorActorSurvival) == 100
             && gAiLogicData->abilities[battler] == ABILITY_UNBURDEN
             && !gBattleMons[battler].volatiles.unburdenActive)
                speed[battler] *= 2;
        }
    }
}

static s32 ScoreFastPair(struct PairEvaluation *ev, bool32 applyEffects, u32 *effectChance)
{
    struct PairAction actions[MAX_BATTLERS_COUNT];
    memcpy(actions, ev->action, sizeof(actions));
    u32 hp[MAX_BATTLERS_COUNT], boost[MAX_BATTLERS_COUNT] = {100, 100, 100, 100};
    u32 survival[MAX_BATTLERS_COUNT] = {100, 100, 100, 100};
    u32 actionChance[MAX_BATTLERS_COUNT] = {10000, 10000, 10000, 10000};
    u32 newParalysisTargets = 0;
    u32 speed[MAX_BATTLERS_COUNT];
    u8 attackStage[MAX_BATTLERS_COUNT], speedStage[MAX_BATTLERS_COUNT];
    u8 statStage[MAX_BATTLERS_COUNT][4]; // Atk, Def, SpDef, SpAtk.
    u32 statModifier[MAX_BATTLERS_COUNT][4];
    u32 usedItems = 0;
    u32 acted = 0, protected = 0, stopped = 0, wideGuard = 0, quickGuard = 0;
    struct PairRetaliation received[MAX_BATTLERS_COUNT][DAMAGE_CATEGORY_STATUS] = {0};
    u8 lastReceivedCategory[MAX_BATTLERS_COUNT] = {0};
    u32 newSleepTargets = 0, newSleepSides = 0;
    u32 newWish = 0;
    u32 encoredGuards = 0;
    u8 forcedGuardChance[MAX_BATTLERS_COUNT] = {0};
    u8 forcedWideChance[NUM_BATTLE_SIDES] = {0}, forcedQuickChance[NUM_BATTLE_SIDES] = {0};
    u32 weather = ev->weather;
    bool32 trickRoom = gFieldStatuses & STATUS_FIELD_TRICK_ROOM;
    enum BattlerId forceNext = MAX_BATTLERS_COUNT;
    enum BattlerId redirect[NUM_BATTLE_SIDES] = {MAX_BATTLERS_COUNT, MAX_BATTLERS_COUNT};
    u32 screens[NUM_BATTLE_SIDES];
    for (u32 side = 0; side < NUM_BATTLE_SIDES; side++)
        screens[side] = gSideStatuses[side] & SIDE_STATUS_SCREEN_ANY;
    s32 score = ev->board.reserveValue[ev->side] - ev->board.reserveValue[ev->side ^ 1];
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        hp[actor] = gBattleMons[actor].hp;
        speed[actor] = gAiLogicData->speedStats[actor];
        speedStage[actor] = gBattleMons[actor].statStages[STAT_SPEED];
        attackStage[actor] = gBattleMons[actor].statStages[IsBattleMovePhysical(actions[actor].executedMove) ? STAT_ATK : STAT_SPATK];
        statStage[actor][0] = gBattleMons[actor].statStages[STAT_ATK];
        statStage[actor][1] = gBattleMons[actor].statStages[STAT_DEF];
        statStage[actor][2] = gBattleMons[actor].statStages[STAT_SPDEF];
        statStage[actor][3] = gBattleMons[actor].statStages[STAT_SPATK];
        for (u32 stat = 0; stat < 4; stat++)
            statModifier[actor][stat] = 100;
        if (GetBattlerSide(actor) == ev->side && actions[actor].index != PAIR_IDLE)
        {
            enum BattleMoveEffects effect = GetMoveEffect(actions[actor].move);
            enum BattleMoveEffects partnerEffect = GetMoveEffect(actions[GetPartnerBattler(actor)].move);
            if (effect == partnerEffect && (effect == EFFECT_TRICK_ROOM || effect == EFFECT_WEATHER
                || effect == EFFECT_TAILWIND || effect == EFFECT_FOLLOW_ME
                || effect == EFFECT_STEALTH_ROCK || effect == EFFECT_STICKY_WEB
                || (effect == EFFECT_SPIKES && gSideTimers[ev->side ^ 1].spikesAmount >= 2)
                || (effect == EFFECT_TOXIC_SPIKES && gSideTimers[ev->side ^ 1].toxicSpikesAmount >= 1)))
                return -10000;
        }
    }
    for (u32 turn = 0; turn < gBattlersCount; turn++)
    {
        enum BattlerId actor = MAX_BATTLERS_COUNT;
        for (enum BattlerId next = 0; next < gBattlersCount; next++)
        {
            if ((acted & (1u << next)) || !hp[next] || actions[next].index == PAIR_IDLE)
                continue;
            if (actor == MAX_BATTLERS_COUNT
             || PairActsBefore(next, &actions[next], actor, &actions[actor], speed, trickRoom))
                actor = next;
        }
        if (forceNext < MAX_BATTLERS_COUNT && hp[forceNext] && !(acted & (1u << forceNext)))
            actor = forceNext;
        forceNext = MAX_BATTLERS_COUNT;
        if (actor == MAX_BATTLERS_COUNT)
            break;
        acted |= 1u << actor;
        if (stopped & (1u << actor))
            continue;
        const struct PairAction *action = &actions[actor];
        enum BattlerId partner = GetPartnerBattler(actor);
        const struct PairAction *other = &actions[partner];
        enum Move move = action->move;
        enum BattleMoveEffects effect = GetMoveEffect(move);
        s32 sign = GetBattlerSide(actor) == ev->side ? 1 : -1;
        s32 damageOpinion = 0;
        if (((gBattleMons[actor].status1 & STATUS1_SLEEP) > (gAiLogicData->abilities[actor] == ABILITY_EARLY_BIRD ? 2 : 1)
             && !IsUsableWhileAsleepEffect(effect))
         || ((gBattleMons[actor].status1 & STATUS1_FREEZE) && !MoveThawsUser(move))
         || gBattleMons[actor].volatiles.rechargeTimer
         || (gAiLogicData->abilities[actor] == ABILITY_TRUANT && gBattleMons[actor].volatiles.truantCounter))
            continue;
        // A faster pair can spend the berry and leave insufficient HP before
        // Drum acts. A failed Drum must not retain its cached setup reward.
        u32 drumCost = 0;
        if (effect == EFFECT_BELLY_DRUM)
        {
            drumCost = max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 2);
            if (statStage[actor][0] == MAX_STAT_STAGE || hp[actor] <= drumCost)
                continue;
        }
        struct PairRetaliation *retaliation = NULL;
        if (effect == EFFECT_REFLECT_DAMAGE)
        {
            u32 categories = GetMoveReflectDamage_DamageCategories(move);
            u32 category = categories == (1u << DAMAGE_CATEGORY_PHYSICAL) ? DAMAGE_CATEGORY_PHYSICAL
                : categories == (1u << DAMAGE_CATEGORY_SPECIAL) ? DAMAGE_CATEGORY_SPECIAL : lastReceivedCategory[actor];
            retaliation = &received[actor][category];
            // The isolated opinion cannot see this trial's recipients or
            // shields. A preceding opposing attack is not enough: it must
            // actually hit this user with the required damage category.
            if (!retaliation->chance)
            {
                score -= sign * 150;
                continue;
            }
        }
        if (sign > 0)
        {
            s32 planScore = action->planScore;
            if (planScore <= -10000)
                return -10000;
            score += planScore;
            // The complete turn owns protection's value. Applying the
            // isolated scorer again can prefer a blocked attack over a shield
            // that saves HP while producing the same damage on both sides.
            if (!PairSupport(move) && effect != EFFECT_WISH && effect != EFFECT_PROTECT && effect != EFFECT_REFLECT_DAMAGE)
            {
                if (action->score == 0)
                    score -= 500;
                else
                {
                    s32 opinion = (action->score - AI_SCORE_DEFAULT) * 4;
                    if (opinion <= 0 || IsBattleMoveStatus(action->executedMove))
                        score += opinion;
                    else
                        damageOpinion = opinion;
                }
            }
        }
        if (effect == EFFECT_BELLY_DRUM)
        {
            hp[actor] -= drumCost;
            PairChangeStat(&statStage[actor][0], &statModifier[actor][0],
                GetAdjustedStatStage(STAT_CHANGE_FORCE_MAX, gAiLogicData->abilities[actor], FALSE), survival[actor], FALSE);
            PairTryHealingBerry(ev, actor, hp, speed, &usedItems);
            continue;
        }
        if (effect == EFFECT_PROTECT)
        {
            u32 chance = ev->protectChance[actor][action->index];
            bool32 hasLaterMove = FALSE;
            // Native Protect fails if no living battler will attempt a move
            // afterward. Use the trial's actions, not pending human commands.
            for (enum BattlerId next = 0; next < gBattlersCount; next++)
                if (!(acted & (1u << next)) && hp[next] && actions[next].index != PAIR_IDLE)
                    hasLaterMove = TRUE;
            if (!chance || !hasLaterMove)
                continue;
            if (encoredGuards & (1u << actor))
            {
                // Encore still replaces the attack when repeated Protect
                // fails. Keep guard odds local, not in Encore's outer branch.
                chance = chance * survival[actor] / 100;
                u32 side = GetBattlerSide(actor);
                if (move == MOVE_WIDE_GUARD)
                    forcedWideChance[side] = 100 - (100 - forcedWideChance[side]) * (100 - chance) / 100;
                else if (move == MOVE_QUICK_GUARD)
                    forcedQuickChance[side] = 100 - (100 - forcedQuickChance[side]) * (100 - chance) / 100;
                else if (GetProtectType(GetMoveProtectMethod(move)) == PROTECT_TYPE_SINGLE)
                    forcedGuardChance[actor] = chance;
                continue;
            }
            if (chance < 100 && !applyEffects)
                continue;
            if (chance < 100)
                *effectChance = *effectChance ? min(*effectChance, chance) : chance;
            if (move == MOVE_WIDE_GUARD)
                wideGuard |= 1u << GetBattlerSide(actor);
            else if (move == MOVE_QUICK_GUARD)
                quickGuard |= 1u << GetBattlerSide(actor);
            else if (GetProtectType(GetMoveProtectMethod(move)) == PROTECT_TYPE_SINGLE)
                protected |= 1u << actor;
            continue;
        }
        if (effect == EFFECT_WEATHER && !(gBattleWeather & B_WEATHER_PRIMAL_ANY))
        {
            u32 newWeather = HasWeatherEffect() ? PairWeatherMask(move) : B_WEATHER_NONE;
            for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
            {
                if (PairWeatherSpeed(gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler], weather))
                    speed[battler] /= 2;
                if (PairWeatherSpeed(gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler], newWeather))
                    speed[battler] *= 2;
            }
            weather = newWeather;
            continue;
        }
        if (effect == EFFECT_TAILWIND && !(gSideStatuses[GetBattlerSide(actor)] & SIDE_STATUS_TAILWIND))
        {
            speed[actor] *= 2;
            speed[partner] *= 2;
            continue;
        }
        if (effect == EFFECT_TRICK_ROOM)
        {
            trickRoom = !trickRoom;
            continue;
        }
        if (effect == EFFECT_AFTER_YOU)
        {
            if (hp[partner] && !(acted & (1u << partner)) && other->index != PAIR_IDLE)
                forceNext = partner;
            else
                score -= sign * 150;
            continue;
        }
        if (effect == EFFECT_INSTRUCT)
        {
            if (!hp[partner] || !(acted & (1u << partner)) || (stopped & (1u << partner))
             || other->index == PAIR_IDLE || IsBattleMoveStatus(other->move)
             || gBattleMoveEffects[GetMoveEffect(other->move)].twoTurnEffect
             || IsMoveInstructBanned(other->move)
             || MoveHasAdditionalEffectSelf(other->move, MOVE_EFFECT_RECHARGE)
             || gBattleMons[partner].volatiles.multipleTurns
             || gBattleMons[partner].pp[other->index] <= 1)
            {
                score -= sign * 150;
                continue;
            }
            actor = partner;
            action = other;
            move = action->move;
            effect = GetMoveEffect(move);
        }
        if (effect == EFFECT_HELPING_HAND)
        {
            if (hp[partner] && !(acted & (1u << partner)) && !IsBattleMoveStatus(other->executedMove)
             && other->index != PAIR_IDLE && gAiLogicData->abilities[partner] != ABILITY_GOOD_AS_GOLD)
                boost[partner] = boost[partner] * 3 / 2;
            // A wasted boost already produces no damage in this trial. An
            // extra penalty here makes Helping Hand + Protect look worse than
            // Helping Hand + an attack into a confirmed opposing Protect,
            // despite the latter losing more HP for the same zero damage.
            continue;
        }
        if (effect == EFFECT_FOLLOW_ME)
        {
            // Native move legality handles powder/priority immunity. Redirect
            // only single-target attacks; spread attacks still hit the setter.
            redirect[GetBattlerSide(actor)] = actor;
            continue;
        }
        if (effect == EFFECT_REFLECT)
        {
            u32 side = GetBattlerSide(actor);
            if (screens[side] & SIDE_STATUS_REFLECT)
            {
                // An earlier setter can invalidate a move that looked useful
                // on the original board; do not retain its positive opinion.
                if (sign > 0 && action->score > AI_SCORE_DEFAULT)
                    score -= (action->score - AI_SCORE_DEFAULT) * 4;
                continue;
            }
            if (!PairMayBeSnatched(actor, actions, hp, acted, stopped)
             && PairScreenEffectApplies(applyEffects, survival[actor] * actionChance[actor] / 10000, effectChance))
                screens[side] |= SIDE_STATUS_REFLECT;
            // Certain setup applies in both passes, not at the chance of an
            // unrelated Snarl. Uncertain setup shares the existing effect pass.
            continue;
        }
        if (effect == EFFECT_WISH)
        {
            if (gBattleStruct->wish[actor].counter || gBattleMons[actor].volatiles.healBlockTimer)
                score -= sign * 150;
            else if (ev->wishProtectOption[actor] && !PairMayBeSnatched(actor, actions, hp, acted, stopped))
                newWish |= 1u << actor;
            // No HP changes now: incoming attacks and recoil must still be
            // survived before even a deferred option can have value.
            continue;
        }
        if (effect == EFFECT_RESTORE_HP
         || (effect == EFFECT_ROOST && !IS_BATTLER_OF_TYPE(actor, TYPE_FLYING)))
        {
            // Recover/Slack Off/Heal Order use native floor-half non-Dynamax
            // max HP. Evaluate current trial HP: a faster hit can open a heal
            // even when the battler started the turn full. HP is conditional
            // on surviving to act; do not multiply by survival a second time.
            // Snatch redirection is not simulated, so withhold this credit
            // when an earlier modeled action could have armed a thief.
            // Pure-Bug Volbeat loses no type to Roost. Flying users still
            // need a separate type-aware incoming-damage forecast.
            if (!PairMayBeSnatched(actor, actions, hp, acted, stopped) && !gBattleMons[actor].volatiles.healBlockTimer)
            {
                u32 amount = max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 2);
                amount = min(amount, gBattleMons[actor].maxHP - hp[actor]);
                hp[actor] += amount * actionChance[actor] / 10000;
            }
            continue;
        }
        if (effect == EFFECT_HEAL_PULSE || effect == EFFECT_LIFE_DEW
         || (effect == EFFECT_HIT_ENEMY_HEAL_ALLY && IsBattlerAlly(actor, action->target)))
        {
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
            {
                if (!hp[target] || gBattleMons[target].volatiles.healBlockTimer
                 || (effect == EFFECT_LIFE_DEW ? !IsBattlerAlly(actor, target) : target != action->target))
                    continue;
                if (actor != target && effect != EFFECT_LIFE_DEW
                 && ((protected & (1u << target))
                     || (IsBattleMoveStatus(move) && gAiLogicData->abilities[target] == ABILITY_GOOD_AS_GOLD)
                     || DoesSubstituteBlockMove(actor, target, move)))
                    continue;
                u32 amount = gBattleMons[target].maxHP / 2;
                if (effect == EFFECT_LIFE_DEW)
                    amount /= 2;
                else if (gAiLogicData->abilities[actor] == ABILITY_MEGA_LAUNCHER && IsPulseMove(move))
                    amount = gBattleMons[target].maxHP * 3 / 4;
                else if (effect == EFFECT_HEAL_PULSE
                      && GetMoveEffectArg_MoveProperty(move) == MOVE_EFFECT_FLORAL_HEALING
                      && gFieldTimers.terrain == B_TERRAIN_GRASSY)
                    amount = gBattleMons[target].maxHP * 2 / 3;
                hp[target] = min(gBattleMons[target].maxHP, hp[target] + amount);
            }
            continue;
        }
        if (move == MOVE_COACHING)
        {
            if (hp[partner])
            {
                u32 attackStat = IsBattleMovePhysical(actions[partner].executedMove) ? 0 : 3;
                statStage[partner][attackStat] = attackStage[partner];
                PairApplyStatChanges(ev, actor, partner, move, statStage, statModifier, survival[actor], &usedItems);
                attackStage[partner] = statStage[partner][attackStat];
            }
            continue;
        }
        enum Stat selfDefenseStat = PairSelfDefenseStat(move);
        if (move == MOVE_SHELL_SMASH || selfDefenseStat != STAT_HP)
        {
            if (move != MOVE_SHELL_SMASH)
            {
                u32 setupChance = actionChance[actor] / 100;
                if ((gBattleMons[actor].status1 & STATUS1_PARALYSIS)
                 && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[actor] == ABILITY_MAGIC_GUARD))
                    setupChance = min(setupChance, ev->paralysisActionChance / 100);
                if (!ev->selfDefenseDelta[actor][action->index]
                 || PairMayBeSnatched(actor, actions, hp, acted, stopped)
                 || !PairScreenEffectApplies(applyEffects, setupChance, effectChance))
                    continue;
                u32 stat = selfDefenseStat == STAT_SPDEF ? 2 : 1;
                PairChangeStat(&statStage[actor][stat], &statModifier[actor][stat],
                    ev->selfDefenseDelta[actor][action->index], 100, TRUE);
            }
            else
                for (u32 stat = 1; stat < 3; stat++)
                    PairChangeStat(&statStage[actor][stat], &statModifier[actor][stat],
                        ev->smashDefenseDelta[actor][stat - 1], 100, TRUE);
            // HP is conditional on this actor being alive to act, so its own
            // defense changes are certain on that branch. A held White Herb
            // restores negative stages before the next battler's attack.
            if (gAiLogicData->holdEffects[actor] == HOLD_EFFECT_WHITE_HERB
             && !(usedItems & (1u << actor)))
            {
                bool32 consumed = FALSE;
                for (u32 stat = 0; stat < 4; stat++)
                    if (statStage[actor][stat] < DEFAULT_STAT_STAGE)
                    {
                        PairChangeStat(&statStage[actor][stat], &statModifier[actor][stat],
                            DEFAULT_STAT_STAGE - statStage[actor][stat], 100, stat == 1 || stat == 2);
                        consumed = TRUE;
                    }
                if (consumed)
                {
                    usedItems |= 1u << actor;
                    if (gAiLogicData->abilities[actor] == ABILITY_UNBURDEN && !gBattleMons[actor].volatiles.unburdenActive)
                        speed[actor] *= 2;
                }
            }
            // Future offense/Speed remains the native opinion. Calm Mind and
            // Quiver Dance earn immediate defense against later special attacks.
            // This addition models immediate durability, not a second turn.
            continue;
        }
        // Wrapper legality, priority, PP and Instruct restrictions above still
        // use the selected command. Only the executed damage behavior resolves
        // Nature Power; Sucker Punch below intentionally checks the command.
        move = action->executedMove;
        effect = GetMoveEffect(move);
        if (IsBattleMoveStatus(move) && !((move == MOVE_TOXIC_THREAD || move == MOVE_SPORE || move == MOVE_SING || move == MOVE_SLEEP_POWDER
            || move == MOVE_THUNDER_WAVE || move == MOVE_GLARE || move == MOVE_CHARM || effect == EFFECT_ENCORE) && applyEffects))
            continue;
        if (gBattleMoveEffects[effect].twoTurnEffect && !gBattleMons[actor].volatiles.multipleTurns
         && gAiLogicData->holdEffects[actor] != HOLD_EFFECT_POWER_HERB
         && !(effect == EFFECT_SOLAR_BEAM && (GetAttackerWeather(gAiLogicData->holdEffects[actor], gAiLogicData->abilities[actor], weather) & B_WEATHER_SUN))
         && !(move == MOVE_ELECTRO_SHOT && (weather & B_WEATHER_RAIN)))
            continue;
        // Native spread damage is calculated for every target before damage
        // is applied. Earlier actions (including Sitrus) affect this HP; an
        // effect from the first spread target must not repower the second hit.
        u32 actionHp = hp[actor];
        bool32 lifeOrb = gAiLogicData->holdEffects[actor] == HOLD_EFFECT_LIFE_ORB
            && !(usedItems & (1u << actor))
            && gAiLogicData->abilities[actor] != ABILITY_MAGIC_GUARD
            && !IsSheerForceAffected(move, gAiLogicData->abilities[actor]);
        bool32 singleContactOrb = GetMoveStrikeCount(move) <= 1 && !IsMultiHitMove(move)
            && effect != EFFECT_BEAT_UP && gAiLogicData->abilities[actor] != ABILITY_PARENTAL_BOND
            && !PairSpread(move);
        u32 orbHitChance = 0;
        enum BattlerId selectedTarget = action->target;
        if (retaliation)
        {
            selectedTarget = retaliation->source;
            if (!hp[selectedTarget])
            {
                if (GetConfig(B_COUNTER_TRY_HIT_PARTNER) < GEN_5)
                    continue;
                selectedTarget = GetPartnerBattler(selectedTarget);
            }
        }
        for (enum BattlerId original = 0; original < gBattlersCount; original++)
        {
            if (original == actor || !hp[original]
             || gBattleMons[original].volatiles.semiInvulnerable == STATE_COMMANDER)
                continue;
            if (PairSpread(move))
            {
                if (IsBattlerAlly(actor, original) && GetMoveTarget(move) == TARGET_BOTH)
                    continue;
            }
            else if (original != selectedTarget)
                continue;
            enum BattlerId target = original;
            if (effect == EFFECT_SUCKER_PUNCH
             && ((acted & (1u << target)) || actions[target].index == PAIR_IDLE
                 // Native Sucker Punch reads the original selected slot, even
                 // when Encore will replace that command at execution time.
                 || IsBattleMoveStatus(ev->action[target].move)))
                continue;
            enum BattlerId follow = redirect[GetBattlerSide(target)];
            bool32 followed = !PairSpread(move) && !IsBattlerAlly(actor, target) && follow < MAX_BATTLERS_COUNT && hp[follow]
             && !IsMoveRedirectionPrevented(actor, move, gAiLogicData->abilities[actor])
             && (!IsPowderMove(actions[follow].move)
                 || IsAffectedByPowderMove(actor, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor]));
            if (followed)
                target = follow;
            if (!MoveIgnoresProtect(move)
             && (((protected & (1u << target)) && !AI_CanContactBypassProtect(actor, target, move))
                 || (PairSpread(move) && (wideGuard & (1u << GetBattlerSide(target))))
                 || (action->priority > 0
                     && !IsBattlerAlly(actor, target)
                     && (quickGuard & (1u << GetBattlerSide(target))))))
                continue;
            u32 guardHitChance = 100;
            if (!MoveIgnoresProtect(move))
            {
                if (!AI_CanContactBypassProtect(actor, target, move))
                    guardHitChance = guardHitChance * (100 - forcedGuardChance[target]) / 100;
                if (PairSpread(move))
                    guardHitChance = guardHitChance * (100 - forcedWideChance[GetBattlerSide(target)]) / 100;
                if (action->priority > 0 && !IsBattlerAlly(actor, target))
                    guardHitChance = guardHitChance * (100 - forcedQuickChance[GetBattlerSide(target)]) / 100;
            }
            if (!guardHitChance)
                continue;
            if (move == MOVE_THUNDER_WAVE || move == MOVE_GLARE)
            {
                if (!(ev->paralysisTargets[actor][action->index] & (1u << target))
                 || ((newParalysisTargets | newSleepTargets) & (1u << target)))
                    continue;
                // Follow Me takes precedence over Lightning Rod. Otherwise
                // omit redirected TW credit, including any absorber reward.
                bool32 redirected = FALSE;
                if (!followed)
                    for (enum BattlerId otherTarget = 0; otherTarget < gBattlersCount; otherTarget++)
                        if (hp[otherTarget] && (ev->paralysisRedirectors[actor][action->index] & (1u << otherTarget)))
                            redirected = TRUE;
                if (redirected)
                    continue;
                u32 chance = min(100, gAiLogicData->moveAccuracy[actor][target][action->index])
                    * survival[actor] * guardHitChance / 10000;
                if (!chance)
                    continue;
                newParalysisTargets |= 1u << target;
                *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                // Native total Speed includes generation rules and Quick Feet.
                // Existing same-turn Speed/weather changes retain their ratio.
                speed[target] = speed[target] * ev->paralyzedSpeed[target] / max(1, gAiLogicData->speedStats[target]);
                if (!(acted & (1u << target)) && !IsBattleMoveStatus(actions[target].executedMove)
                 && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[target] == ABILITY_MAGIC_GUARD))
                    actionChance[target] = ev->paralysisActionChance;
                // Later arbitrary status actions remain the existing forecast;
                // this bounded addition discounts remaining damaging actions.
                continue;
            }
            if (effect == EFFECT_ENCORE)
            {
                u32 slot = ev->encoreGuardIndex[actor][target];
                if (!slot || (acted & (1u << target)) || actions[target].index == PAIR_IDLE
                 || GetMoveEffect(actions[target].move) == EFFECT_SHELL_TRAP)
                    continue;
                slot--;
                enum Move forcedMove = gBattleMons[target].moves[slot];
                if (actions[target].move == forcedMove)
                    continue;
                u32 chance = min(100, gAiLogicData->moveAccuracy[actor][target][action->index]) * survival[actor] * guardHitChance / 10000;
                if (!chance)
                    continue;
                // Only guard replacements are modeled: their self/side target
                // does not require Encore's native random-opponent retarget.
                // Unsupported forced moves retain the ordinary Encore opinion.
                actions[target].move = actions[target].executedMove = forcedMove;
                actions[target].index = slot;
                actions[target].target = target;
                actions[target].score = AI_SCORE_DEFAULT;
                actions[target].planScore = 0;
                encoredGuards |= 1u << target;
                if (GetConfig(B_ENCORE_PRIORITY) >= GEN_CHAMPIONS)
                    actions[target].priority = ev->encoreGuardPriority[target];
                *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                continue;
            }
            if (move == MOVE_SPORE || move == MOVE_SING || move == MOVE_SLEEP_POWDER)
            {
                u32 chance = ev->sleepDenialChance[actor][action->index][target];
                u32 side = GetBattlerSide(target);
                if (!chance || ((newSleepTargets | newParalysisTargets) & (1u << target))
                 || (ev->sleepClause && (newSleepSides & (1u << side))))
                    continue;
                // Landing sleep claims its target/clause even if that target
                // already acted or can use its chosen move while asleep.
                newSleepTargets |= 1u << target;
                newSleepSides |= 1u << side;
                if (!(acted & (1u << target)) && !IsUsableWhileAsleepEffect(GetMoveEffect(actions[target].move)))
                {
                    chance = chance * min(100, gAiLogicData->moveAccuracy[actor][target][action->index]) * survival[actor] / 10000;
                    chance = chance * guardHitChance / 100;
                    if (chance)
                    {
                        stopped |= 1u << target;
                        *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                    }
                }
                continue;
            }
            // Thread shares targeting/protection and cached Speed handling,
            // not damage. Poison remains the native single-action opinion.
            struct SimulatedDamage damage = move == MOVE_TOXIC_THREAD || move == MOVE_CHARM ? (struct SimulatedDamage){0}
                : gAiLogicData->simulatedDmg[actor][target][action->index];
            // An isolated KO/matchup bonus is not earned by attacking into
            // Protect. Credit it once, only after a real recipient survives
            // targeting and guard checks; spread moves may still hit a partner.
            if (damage.affectsTarget && damageOpinion)
            {
                score += damageOpinion * guardHitChance / 100;
                damageOpinion = 0;
            }
            bool32 hpPowerAdjusted = actionHp != gBattleMons[actor].hp
                && (ev->hpPowerTargets[actor][action->index] & (1u << target));
            if (hpPowerAdjusted)
                damage = PairHpPowerDamage(ev, actor, target, action->index, move, actionHp);
            bool32 conditionalBoostAdjusted = ev->conditionalBoostTargets[actor][action->index] & (1u << target);
            // Defeatist endpoints are native-rounded; threshold selection
            // uses conditional-alive aggregate HP. Roll/miss/healing branches
            // straddling half HP remain approximate, not an exact KO tree.
            if (conditionalBoostAdjusted)
                damage = PairConditionalBoostDamage(ev, actor, target, action->index,
                    gAiLogicData->abilities[actor] == ABILITY_DEFEATIST && gBattleMons[actor].item == ITEM_NONE
                    ? (actionHp > gBattleMons[actor].maxHP / 2 ? 100 : 0)
                    : (ev->itemBoost[actor] & (1u << action->index)) ? ((usedItems & (1u << actor)) ? 0 : 100)
                    : (hp[GetPartnerBattler(actor)] ? survival[GetPartnerBattler(actor)] : 0));
            u32 accuracy = min(100, gAiLogicData->moveAccuracy[actor][target][action->index]);
            if (weather != ev->weather)
            {
                u32 cachedAccuracy = ev->weatherAccuracy[actor][action->index][target][PairAccuracyWeatherIndex(weather)];
                if (cachedAccuracy)
                    accuracy = cachedAccuracy - 1;
            }
            u32 nativeAccuracy = accuracy;
            accuracy = accuracy * guardHitChance / 100;
            u32 affectedChance = accuracy * survival[target] / 100;
            enum Ability targetAbility = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
                gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], move);
            bool32 stealsSitrus = (move == MOVE_BUG_BITE || move == MOVE_PLUCK)
                && gBattleMons[target].item == ITEM_SITRUS_BERRY && !(usedItems & (1u << target))
                && targetAbility != ABILITY_STICKY_HOLD && !DoesSubstituteBlockMove(actor, target, move);
            // Resolve the actual redirected recipient before considering
            // healing or contact. These unmodeled correlations retain the
            // previous forecast instead of receiving fictitious Orb KOs.
            if (lifeOrb && (ev->drainPercent[actor][action->index][target]
             || (!singleContactOrb && ev->contactDamage[actor][action->index][target])
             || (stealsSitrus && !singleContactOrb)))
                lifeOrb = FALSE;
            if (lifeOrb && gAiLogicData->simulatedDmg[actor][target][action->index].affectsTarget)
            {
                // One payment if ANY recipient is affected, including allies
                // and Substitute/Disguise. Execution/paralysis is one shared
                // event and is applied once after the complete move below.
                orbHitChance = 100 - (100 - orbHitChance) * (100 - affectedChance) / 100;
            }
            if ((ev->screenBreakerMoves[actor] & (1u << action->index)) && damage.maximum)
            {
                u32 side = B_BRICK_BREAK >= GEN_4 ? GetBattlerSide(target) : GetBattlerSide(actor) ^ BIT_SIDE;
                u32 chance = accuracy * survival[actor] / 100 * actionChance[actor] / 10000;
                chance = chance * survival[target] / 100;
                // Targeting, immunity and Protect have already been checked.
                // A hit removes screens before its own damage; Substitute does
                // not block this primary effect. The native breaker cache is
                // already unscreened, even when the old board had a screen.
                if (screens[side] && PairScreenEffectApplies(applyEffects, chance, effectChance))
                    screens[side] = 0;
            }
            u32 screenNumerator = UQ_4_12(1.0), screenDenominator = UQ_4_12(1.0);
            if (ev->screenTargets[actor][action->index] & (1u << target))
            {
                u32 side = GetBattlerSide(target);
                screenNumerator = PairScreenFactor(move, screens[side]);
                screenDenominator = PairScreenFactor(move, gSideStatuses[side]);
            }
            u32 amount = damage.median * boost[actor] / 100;
            u32 minimumDamage = damage.minimum * boost[actor] / 100;
            u32 worstDamage = damage.maximum * boost[actor] / 100;
            if (weather != ev->weather)
            {
                u32 weatherPower = PairWeatherPower(actor, target, move, weather);
                u32 oldWeatherPower = PairWeatherPower(actor, target, move, ev->weather);
                amount = amount * weatherPower / oldWeatherPower;
                minimumDamage = minimumDamage * weatherPower / oldWeatherPower;
                worstDamage = worstDamage * weatherPower / oldWeatherPower;
            }
            u32 statPower = PairStatDamageModifier(actor, target, move, statStage, statModifier);
            if (statPower != 100)
            {
                amount = amount * statPower / 100;
                minimumDamage = minimumDamage * statPower / 100;
                worstDamage = worstDamage * statPower / 100;
            }
            amount = PairApplyScreenRatio(amount, screenNumerator, screenDenominator);
            minimumDamage = PairApplyScreenRatio(minimumDamage, screenNumerator, screenDenominator);
            worstDamage = PairApplyScreenRatio(worstDamage, screenNumerator, screenDenominator);
            if (retaliation)
            {
                if (retaliation->knownDamage)
                {
                    u32 percent = damage.affectsTarget ? GetMoveReflectDamage_DamagePercent(move) : 0;
                    amount = retaliation->damage.median * percent / 100;
                    minimumDamage = retaliation->damage.minimum * percent / 100;
                    worstDamage = retaliation->damage.maximum * percent / 100;
                }
                accuracy = accuracy * retaliation->chance / 100;
            }
            if (effect == EFFECT_POPULATION_BOMB && gAiLogicData->populationBomb[actor][target].valid)
            {
                if (damage.maximum && accuracy && !DoesSubstituteBlockMove(actor, target, move)
                 && (GetConfig(B_COUNTER_MIRROR_COAT_ALLY) <= GEN_4 || !IsBattlerAlly(actor, target)))
                {
                    received[target][DAMAGE_CATEGORY_PHYSICAL] = (struct PairRetaliation){
                        .source = actor, .chance = accuracy * survival[actor] / 100 * actionChance[actor] / 10000,
                    };
                    lastReceivedCategory[target] = DAMAGE_CATEGORY_PHYSICAL;
                }
                PairApplyPopulationBomb(ev, actor, target, action, nativeAccuracy, guardHitChance,
                    actionChance[actor], boost[actor], statPower, screenNumerator, screenDenominator,
                    hp, survival, speed, &usedItems);
                if (!hp[actor])
                    break;
                continue;
            }
            if (effect == EFFECT_FIXED_PERCENT_DAMAGE)
            {
                // Native fixed-percent damage ignores Helping Hand, stats and
                // weather. Preserve the cached zero for immunity/failure.
                amount = minimumDamage = worstDamage = damage.maximum ? PairFixedPercentDamage(ev, target, move, hp[target]) : 0;
            }
            if (IsBattlerAlly(actor, target) && damage.maximum == 0)
            {
                enum Type type = GetMoveType(move);
                if ((targetAbility == ABILITY_STORM_DRAIN && type == TYPE_WATER)
                 || (targetAbility == ABILITY_LIGHTNING_ROD && type == TYPE_ELECTRIC))
                {
                    if (!(acted & (1u << target)) && IsBattleMoveSpecial(actions[target].executedMove))
                        boost[target] = boost[target] * PairChangeStage(&attackStage[target], 1) / 100;
                }
                else if (targetAbility == ABILITY_MOTOR_DRIVE && type == TYPE_ELECTRIC)
                {
                    if (!(acted & (1u << target)))
                        speed[target] = speed[target] * PairChangeStage(&speedStage[target], 1) / 100;
                }
                else if ((targetAbility == ABILITY_WATER_ABSORB && type == TYPE_WATER)
                      || (targetAbility == ABILITY_DRY_SKIN && type == TYPE_WATER)
                      || (targetAbility == ABILITY_VOLT_ABSORB && type == TYPE_ELECTRIC)
                      || (targetAbility == ABILITY_EARTH_EATER && type == TYPE_GROUND))
                {
                    if (!gBattleMons[target].volatiles.healBlockTimer)
                        hp[target] = min(gBattleMons[target].maxHP, hp[target] + gBattleMons[target].maxHP / 4);
                }
            }
            bool32 singleHit = GetMoveStrikeCount(move) <= 1 && !IsMultiHitMove(move)
                && effect != EFFECT_BEAT_UP && gAiLogicData->abilities[actor] != ABILITY_PARENTAL_BOND;
            if (worstDamage >= hp[target] && singleHit
             && (((hpPowerAdjusted || conditionalBoostAdjusted) && gBattleMons[target].volatiles.endured)
                 || (hp[target] == gBattleMons[target].maxHP
                     && (gAiLogicData->holdEffects[target] == HOLD_EFFECT_FOCUS_SASH
                 || AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor], gAiLogicData->abilities[target],
                     gAiLogicData->holdEffects[target], move) == ABILITY_STURDY))))
            {
                amount = min(amount, hp[target] - 1);
                minimumDamage = min(minimumDamage, hp[target] - 1);
                worstDamage = hp[target] - 1;
            }
            u32 damageHitChance = accuracy * survival[actor] / 100;
            u32 hitChance = damageHitChance * actionChance[actor] / 10000;
            u32 medianDamage = amount;
            amount = min(amount, hp[target]) * hitChance / 100;
            if (hp[target] > worstDamage && amount
             && gAiLogicData->abilities[actor] != ABILITY_MOLD_BREAKER
             && gAiLogicData->abilities[actor] != ABILITY_TERAVOLT
             && gAiLogicData->abilities[actor] != ABILITY_TURBOBLAZE
             && gAiLogicData->abilities[target] == ABILITY_JUSTIFIED && GetMoveType(move) == TYPE_DARK
             && !(acted & (1u << target)) && IsBattleMovePhysical(actions[target].executedMove))
            {
                u32 gain = PairChangeStage(&attackStage[target], effect == EFFECT_BEAT_UP ? AI_GetBeatUpHitCount(actor) : 1);
                boost[target] = boost[target] * gain / 100;
            }
            // Hit-triggered boosts belong to either side. Hitting an opposing
            // Sturdy/Policy recipient can enable its already-committed reply.
            if (hp[target] > worstDamage && amount
             && !(acted & (1u << target)) && !IsBattleMoveStatus(actions[target].executedMove))
            {
                if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_WEAKNESS_POLICY
                 && !(usedItems & (1u << target))
                 && gAiLogicData->effectiveness[actor][target][action->index] > UQ_4_12(1.0))
                {
                    usedItems |= 1u << target;
                    u32 gain = PairChangeStage(&attackStage[target],
                        GetAdjustedStatStage(2, gAiLogicData->abilities[target], FALSE));
                    boost[target] = boost[target] * gain / 100;
                }
                if (gAiLogicData->abilities[target] == ABILITY_STEAM_ENGINE
                 && (GetMoveType(move) == TYPE_FIRE || GetMoveType(move) == TYPE_WATER)
                 && gAiLogicData->abilities[actor] != ABILITY_MOLD_BREAKER
                 && gAiLogicData->abilities[actor] != ABILITY_TERAVOLT
                 && gAiLogicData->abilities[actor] != ABILITY_TURBOBLAZE)
                {
                    // Steam Engine adds six stages; prior Speed drops are not
                    // erased. From -2 it reaches +4, not the +6 ceiling.
                    u32 gain = PairChangeStage(&speedStage[target], 6);
                    speed[target] = speed[target] * gain / 100;
                }
            }
            u32 drainLimit = hp[target];
            u32 recoilMinimum = 0, recoilMedian = 0, recoilMaximum = 0;
            enum HoldEffect actorHeld = gAiLogicData->holdEffects[actor];
            bool32 contactMayHeal = ev->contactDamage[actor][action->index][target]
                && !(usedItems & (1u << actor))
                && (actorHeld == HOLD_EFFECT_RESTORE_HP || actorHeld == HOLD_EFFECT_RESTORE_PCT_HP
                    || actorHeld == HOLD_EFFECT_CONFUSE_FLAVOR
                    || (gAiLogicData->abilities[actor] == ABILITY_CHEEK_POUCH
                        && GetItemPocket(gBattleMons[actor].item) == POCKET_BERRIES));
            if (effect == EFFECT_RECOIL && singleHit && !PairSpread(move)
             && GetActiveGimmick(actor) != GIMMICK_DYNAMAX && GetActiveGimmick(actor) != GIMMICK_Z_MOVE
             && AI_IsDamagedByRecoil(actor) && !contactMayHeal)
            {
                // Native damage-based recoil uses actual HP removed, not
                // attacker's max HP or accuracy-discounted expected damage.
                // Capture the recipient before this hit changes its HP. A
                // Substitute contributes its own capped loss; Disguise and
                // Ice Face contribute zero even though they trigger Orb.
                u32 limit = drainLimit;
                if (DoesSubstituteBlockMove(actor, target, move))
                    limit = gBattleMons[target].volatiles.substituteHP;
                else if (!gBattleMons[target].volatiles.transformed
                     && ((targetAbility == ABILITY_DISGUISE && IsMimikyuDisguised(target))
                         || (targetAbility == ABILITY_ICE_FACE && gBattleMons[target].species == SPECIES_EISCUE_ICE
                             && IsBattleMovePhysical(move))))
                    limit = 0;
                u32 percent = max(1, GetMoveRecoil(move));
                recoilMinimum = min(minimumDamage, limit);
                recoilMedian = min(medianDamage, limit);
                recoilMaximum = min(worstDamage, limit);
                if (recoilMinimum)
                    recoilMinimum = max(1, recoilMinimum * percent / 100);
                if (recoilMedian)
                    recoilMedian = max(1, recoilMedian * percent / 100);
                if (recoilMaximum)
                    recoilMaximum = max(1, recoilMaximum * percent / 100);
                // Multi-hit/spread recoil and healing between contact and
                // move recoil need correlated state not represented here.
            }
            bool32 contactPaid = FALSE;
            u32 beforeHitHp = hp[target], beforeHitSurvival = survival[target];
            if (amount && !DoesSubstituteBlockMove(actor, target, move)
             && (gBattleMons[target].volatiles.transformed
                 || !((targetAbility == ABILITY_DISGUISE && IsMimikyuDisguised(target))
                     || (targetAbility == ABILITY_ICE_FACE && gBattleMons[target].species == SPECIES_EISCUE_ICE
                         && IsBattleMovePhysical(move))))
             && (GetConfig(B_COUNTER_MIRROR_COAT_ALLY) <= GEN_4 || !IsBattlerAlly(actor, target)))
            {
                enum DamageCategory category = GetBattleMoveCategory(move);
                if (GetConfig(B_HIDDEN_POWER_COUNTER) < GEN_4 && effect == EFFECT_HIDDEN_POWER)
                    category = DAMAGE_CATEGORY_PHYSICAL;
                if (category != DAMAGE_CATEGORY_STATUS)
                {
                    struct PairRetaliation *hit = &received[target][category];
                    hit->source = actor;
                    hit->chance = hitChance;
                    // Native Counter uses the final strike, not the total.
                    // Equal fixed strikes can reuse the aggregate endpoints;
                    // variable counts, ramping power and Parental Bond cannot.
                    u32 strikes = max(1, GetMoveStrikeCount(move));
                    hit->knownDamage = singleHit || (strikes > 1 && !IsMultiHitMove(move)
                        && effect != EFFECT_TRIPLE_KICK && effect != EFFECT_POPULATION_BOMB
                        && gAiLogicData->abilities[actor] != ABILITY_PARENTAL_BOND);
                    hit->damage = (struct PairDamageRange){minimumDamage / strikes, medianDamage / strikes, worstDamage / strikes};
                    lastReceivedCategory[target] = category;
                }
            }
            PairApplyDamageWhenActing(&hp[target], &survival[target], minimumDamage, medianDamage, worstDamage,
                damageHitChance, actionChance[actor]);
            if (gAiLogicData->abilities[target] == ABILITY_STAMINA && target != actor && hp[target] && amount && singleHit
             && minimumDamage < beforeHitHp && !DoesSubstituteBlockMove(actor, target, move))
            {
                // A surviving damaging hit raises Defense before the next
                // action, including when that first hit was special. Keep
                // multi-strike damage on its existing forecast: its internal
                // hit/Defense sequence is not represented by one stage gain.
                // Miss-only survival must not invent an ability activation;
                // Stamina is also not a Mold Breaker-suppressible ability.
                u32 landedSurvival = worstDamage < beforeHitHp ? 100
                    : (beforeHitHp - minimumDamage) * 100 / (worstDamage - minimumDamage + 1);
                u32 chance = landedSurvival * hitChance * beforeHitSurvival / 10000;
                if (PairScreenEffectApplies(applyEffects, chance, effectChance))
                    PairChangeStat(&statStage[target][1], &statModifier[target][1], 1, 100, TRUE);
            }
            if (move == MOVE_FEINT && hp[target] && damage.maximum && amount
             && !DoesSubstituteBlockMove(actor, target, move))
            {
                u32 side = GetBattlerSide(target);
                bool32 maxGuard = actions[target].executedMove == MOVE_MAX_GUARD
                    || GetActiveGimmick(target) == GIMMICK_DYNAMAX;
                bool32 hasGuard = (!maxGuard && ((protected & (1u << target)) || forcedGuardChance[target]))
                    || ((wideGuard | quickGuard) & (1u << side))
                    || forcedWideChance[side] || forcedQuickChance[side];
                // Native Feint breaks protection after a landed hit, only
                // while its recipient survives. It is a primary effect:
                // Shield Dust and Covert Cloak do not prevent this payoff.
                // Keep an initially blocking Substitute conservative; its
                // destruction and a subsequent effect need their own state.
                if (hasGuard && PairScreenEffectApplies(applyEffects, hitChance * survival[target] / 100, effectChance))
                {
                    if (!maxGuard)
                    {
                        protected &= ~(1u << target);
                        forcedGuardChance[target] = 0;
                    }
                    wideGuard &= ~(1u << side);
                    quickGuard &= ~(1u << side);
                    forcedWideChance[side] = forcedQuickChance[side] = 0;
                }
            }
            if (applyEffects && (move == MOVE_NUZZLE || move == MOVE_BODY_SLAM) && hp[target] && damage.maximum && amount
             && (ev->paralysisTargets[actor][action->index] & (1u << target))
             && !((newParalysisTargets | newSleepTargets) & (1u << target)))
            {
                // Follow Me wins over ability redirection. As with Thunder
                // Wave, do not credit an unresolved Lightning Rod recipient.
                bool32 redirected = FALSE;
                if (!followed)
                    for (enum BattlerId otherTarget = 0; otherTarget < gBattlersCount; otherTarget++)
                        if (hp[otherTarget] && (ev->paralysisRedirectors[actor][action->index] & (1u << otherTarget)))
                            redirected = TRUE;
                u32 chance = hitChance * ev->paralysisChance[actor][action->index] / 100;
                if (!redirected && chance)
                {
                    newParalysisTargets |= 1u << target;
                    *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                    speed[target] = speed[target] * ev->paralyzedSpeed[target] / max(1, gAiLogicData->speedStats[target]);
                    if (!(acted & (1u << target)) && !IsBattleMoveStatus(actions[target].executedMove)
                     && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[target] == ABILITY_MAGIC_GUARD))
                        actionChance[target] = ev->paralysisActionChance;
                }
                // Native secondary status lands before Helmet/Rough Skin can
                // faint the user. Keep ordinary damage and contact payment.
            }
            u32 drainPercent = ev->drainPercent[actor][action->index][target];
            if (drainPercent && medianDamage && accuracy)
            {
                enum HoldEffect held = gAiLogicData->holdEffects[actor];
                u32 healing = PairDrainedHp(min(medianDamage, drainLimit), drainPercent, held);
                // Native absorb uses capped landed damage, floors the drain,
                // applies Big Root, then minimum one BEFORE hit weighting.
                // Actor HP is conditional on being alive; its survival already
                // scales final board value and must not dilute healing twice.
                if (targetAbility == ABILITY_LIQUID_OOZE
                 && (effect != EFFECT_DREAM_EATER || GetConfig(B_DREAM_EATER_LIQUID_OOZE) >= GEN_5))
                {
                    if (gAiLogicData->abilities[actor] != ABILITY_MAGIC_GUARD)
                    {
                        PairApplyDamageWhenActing(&hp[actor], &survival[actor],
                            PairDrainedHp(min(minimumDamage, drainLimit), drainPercent, held), healing,
                            PairDrainedHp(min(worstDamage, drainLimit), drainPercent, held), accuracy, actionChance[actor]);
                        PairTryHealingBerry(ev, actor, hp, speed, &usedItems);
                    }
                }
                else if (!gBattleMons[actor].volatiles.healBlockTimer)
                    hp[actor] += min(gBattleMons[actor].maxHP - hp[actor], healing) * accuracy * actionChance[actor] / 1000000;
                // Ooze is passive damage, not a Sash/Sturdy-protected hit.
                // Magic Guard prevents its damage; it does not grant healing.
            }
            if (amount || (stealsSitrus && affectedChance
                && gAiLogicData->simulatedDmg[actor][target][action->index].affectsTarget))
            {
                if (stealsSitrus && singleContactOrb && hp[actor])
                {
                    // A stolen berry uses its raw item effect: no held-item,
                    // half-HP or Unnerve gate. Healing precedes Orb, and both
                    // belong to the same hit event, not independent averages.
                    u32 healing = 0;
                    if (!(B_HEAL_BLOCKING >= GEN_5 && gBattleMons[actor].volatiles.healBlockTimer))
                    {
                        healing = PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP)
                            * GetItemHoldEffectParam(ITEM_SITRUS_BERRY) / 100;
                        if (gAiLogicData->abilities[actor] == ABILITY_RIPEN)
                            healing *= 2;
                        healing = max(1, healing);
                    }
                    if (gAiLogicData->abilities[actor] == ABILITY_CHEEK_POUCH
                     && !gBattleMons[actor].volatiles.healBlockTimer)
                        healing += max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 3);
                    u32 afterHit = min(gBattleMons[actor].maxHP, hp[actor] + healing);
                    // Native theft/healing happens before Iron Barbs and
                    // Rough Skin, which may coexist with the stolen Sitrus.
                    afterHit -= min(afterHit, ev->contactDamage[actor][action->index][target]);
                    contactPaid = TRUE;
                    if (lifeOrb)
                    {
                        u32 recoil = max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 10);
                        afterHit -= min(afterHit, recoil);
                        lifeOrb = FALSE;
                    }
                    if (afterHit >= hp[actor])
                        hp[actor] += (afterHit - hp[actor]) * affectedChance * actionChance[actor] / 1000000;
                    else
                    {
                        u32 loss = hp[actor] - afterHit;
                        PairApplyDamageWhenActing(&hp[actor], &survival[actor], loss, loss, loss,
                            affectedChance, actionChance[actor]);
                    }
                }
                // These native effects remove supported berries before HP-threshold
                // items run. Other theft/transfer benefits remain unmodeled.
                // Knock Off also prevents a later Life Orb payment and
                // removes the supported offensive boost before a later attack.
                if (!(usedItems & (1u << target))
                 && (gBattleMons[target].item == ITEM_SITRUS_BERRY
                     || GetItemHoldEffect(gBattleMons[target].item) == HOLD_EFFECT_CONFUSE_FLAVOR
                     || (effect == EFFECT_KNOCK_OFF && (gBattleMons[target].item == ITEM_LIFE_ORB
                         || ev->itemBoost[target])))
                 && targetAbility != ABILITY_STICKY_HOLD && !DoesSubstituteBlockMove(actor, target, move)
                 && (move == MOVE_INCINERATE || move == MOVE_BUG_BITE || move == MOVE_PLUCK
                     || (effect == EFFECT_KNOCK_OFF
                         && !(B_KNOCK_OFF_REMOVAL >= GEN_5 && IsOnPlayerSide(target) && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
                         && CanBattlerGetOrLoseItem(target, actor, gBattleMons[target].item)))
                 && (!ev->itemBoost[target] || PairScreenEffectApplies(applyEffects, hitChance, effectChance)))
                {
                    usedItems |= 1u << target;
                    if (gAiLogicData->abilities[target] == ABILITY_UNBURDEN && !gBattleMons[target].volatiles.unburdenActive)
                        speed[target] *= 2;
                }
                PairTryHealingBerry(ev, target, hp, speed, &usedItems);
            }
            // Target berries still run when Ooze faints a blocking Unnerve
            // attacker. Only then stop processing this fainted actor's move.
            if (!hp[actor])
                break;
            if (move == MOVE_ACID_SPRAY && hp[target] && damage.maximum && amount)
            {
                u32 attackStat = IsBattleMovePhysical(actions[target].executedMove) ? 0 : 3;
                statStage[target][attackStat] = attackStage[target];
                PairApplyStatChanges(ev, actor, target, move, statStage, statModifier, hitChance, &usedItems);
                attackStage[target] = statStage[target][attackStat];
            }
            u32 attackDropStat = PairOffensiveDropStat(move);
            if (applyEffects && attackDropStat && hp[target] && (move == MOVE_CHARM || (damage.maximum && amount))
             && ev->attackDropDelta[actor][action->index][target])
            {
                *effectChance = *effectChance ? min(*effectChance, hitChance) : hitChance;
                u32 chosenAttackStat = IsBattleMovePhysical(actions[target].executedMove) ? 0 : 3;
                // Earlier Weakness Policy/ability boosts retain their existing
                // damage multiplier. Start this ratio at their updated stage.
                statStage[target][chosenAttackStat] = attackStage[target];
                u8 next[4];
                memcpy(next, statStage[target], sizeof(next));
                u32 dropColumn = attackDropStat == STAT_ATK ? 0 : 3;
                next[dropColumn] = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE,
                    (s32)next[dropColumn] + ev->attackDropDelta[actor][action->index][target]));
                bool32 lowered = next[dropColumn] < statStage[target][dropColumn];
                // Native retaliation runs in the stat-drop message, before
                // move-end White Herb. Competitive can erase the negative.
                if (lowered && !IsBattlerAlly(actor, target))
                {
                    if (targetAbility == ABILITY_DEFIANT)
                        next[0] = min(MAX_STAT_STAGE, next[0] + 2);
                    else if (targetAbility == ABILITY_COMPETITIVE)
                        next[3] = min(MAX_STAT_STAGE, next[3] + 2);
                }
                bool32 herb = FALSE;
                if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_WHITE_HERB && !(usedItems & (1u << target)))
                {
                    for (u32 stat = 0; stat < ARRAY_COUNT(next); stat++)
                        if (next[stat] < DEFAULT_STAT_STAGE)
                        {
                            next[stat] = DEFAULT_STAT_STAGE;
                            herb = TRUE;
                        }
                    if (speedStage[target] < DEFAULT_STAT_STAGE)
                        herb = TRUE;
                }
                // Apply final stage differences once. A neutral Snarl+Herb
                // must remain100%, not100→66→99 from two rounded ratios.
                for (u32 stat = 0; stat < ARRAY_COUNT(next); stat++)
                    PairChangeStat(&statStage[target][stat], &statModifier[target][stat],
                        (s32)next[stat] - statStage[target][stat], 100, stat == 1 || stat == 2);
                attackStage[target] = statStage[target][chosenAttackStat];
                if (herb)
                {
                    usedItems |= 1u << target;
                    if (speedStage[target] < DEFAULT_STAT_STAGE)
                    {
                        speed[target] = speed[target] * gStatStageRatios[speedStage[target]][1]
                            / gStatStageRatios[speedStage[target]][0];
                        speedStage[target] = DEFAULT_STAT_STAGE;
                    }
                    if (gAiLogicData->abilities[target] == ABILITY_UNBURDEN && !gBattleMons[target].volatiles.unburdenActive)
                        speed[target] *= 2;
                }
            }
            if (applyEffects && hp[target]
             && (move == MOVE_TOXIC_THREAD
                 || (damage.maximum && amount && (move == MOVE_ELECTROWEB || move == MOVE_ICY_WIND || move == MOVE_ROCK_TOMB))))
            {
                s32 delta = ev->speedDelta[actor][action->index][target];
                u32 oldStage = speedStage[target];
                u32 nextStage = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE, (s32)oldStage + delta));
                if (nextStage != oldStage)
                {
                    *effectChance = *effectChance ? min(*effectChance, hitChance) : hitChance;
                    bool32 lowered = nextStage < oldStage;
                    if (lowered && nextStage < DEFAULT_STAT_STAGE
                     && gAiLogicData->holdEffects[target] == HOLD_EFFECT_WHITE_HERB && !(usedItems & (1u << target)))
                    {
                        nextStage = DEFAULT_STAT_STAGE;
                        usedItems |= 1u << target;
                        // Herb restores all negative stats before the next
                        // action, including any modeled Coaching/Acid Spray.
                        for (u32 stat = 0; stat < 4; stat++)
                            if (statStage[target][stat] < DEFAULT_STAT_STAGE)
                                PairChangeStat(&statStage[target][stat], &statModifier[target][stat],
                                    DEFAULT_STAT_STAGE - statStage[target][stat], 100, stat == 1 || stat == 2);
                        if (attackStage[target] < DEFAULT_STAT_STAGE)
                        {
                            // Both offensive stats now have their own modifier;
                            // do not restore Special Attack a second time here.
                            attackStage[target] = DEFAULT_STAT_STAGE;
                        }
                        if (gAiLogicData->abilities[target] == ABILITY_UNBURDEN && !gBattleMons[target].volatiles.unburdenActive)
                            speed[target] *= 2;
                    }
                    // Keep the stage ratio rational here: rounding -1 Speed
                    // to 66% first can invent a one-point order crossing.
                    speed[target] = speed[target] * gStatStageRatios[nextStage][0] * gStatStageRatios[oldStage][1]
                        / (gStatStageRatios[nextStage][1] * gStatStageRatios[oldStage][0]);
                    speedStage[target] = nextStage;
                    // A successful opposing drop triggers retaliation even if
                    // White Herb immediately restores the lowered Speed.
                    if (lowered && !IsBattlerAlly(actor, target) && !(acted & (1u << target))
                     && ((gAiLogicData->abilities[target] == ABILITY_DEFIANT && IsBattleMovePhysical(actions[target].executedMove))
                         || (gAiLogicData->abilities[target] == ABILITY_COMPETITIVE && IsBattleMoveSpecial(actions[target].executedMove))))
                        boost[target] = boost[target] * PairChangeStage(&attackStage[target], 2) / 100;
                }
            }
            if (move == MOVE_FAKE_OUT && amount && gBattleStruct->battlerState[actor].isFirstTurn
             && gAiLogicData->abilities[target] != ABILITY_INNER_FOCUS
             && gAiLogicData->abilities[target] != ABILITY_SHIELD_DUST
             && gAiLogicData->holdEffects[target] != HOLD_EFFECT_COVERT_CLOAK)
                stopped |= 1u << target;
            if (!contactPaid && singleHit && (ev->contactDamage[actor][action->index][target] || recoilMaximum)
             && gAiLogicData->simulatedDmg[actor][target][action->index].affectsTarget)
            {
                // Contact, damage-based recoil and Orb belong to one landed
                // hit. Apply their combined range once so a missed attack
                // cannot subsequently kill its conditional surviving user.
                // Native contact precedes move recoil, then Orb; with no
                // intervening healing, summing costs preserves final HP even
                // when an earlier cost already faints the attacker.
                u32 contact = ev->contactDamage[actor][action->index][target];
                u32 contactChance = affectedChance;
                if (lifeOrb && singleContactOrb)
                {
                    contact += max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 10);
                    contactChance = orbHitChance;
                    lifeOrb = FALSE;
                }
                PairApplyDamageWhenActing(&hp[actor], &survival[actor],
                    contact + recoilMinimum, contact + recoilMedian, contact + recoilMaximum,
                    contactChance, actionChance[actor]);
                PairTryHealingBerry(ev, actor, hp, speed, &usedItems);
                if (!hp[actor])
                    break;
            }
        }
        if (lifeOrb && orbHitChance && hp[actor])
        {
            // Native move-end ordering: after every hit/target and their
            // contact effects, never per strike and never on a fainted user.
            u32 recoil = max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 10);
            PairApplyDamageWhenActing(&hp[actor], &survival[actor], recoil, recoil, recoil,
                orbHitChance, actionChance[actor]);
        }
    }
    u32 livingSides = 0;
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if (hp[actor])
            livingSides |= 1u << GetBattlerSide(actor);
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        if (!(ev->board.activeMask & (1u << actor)))
            continue;
        // A previously cast Wish with counter 1 pays out after attacks, not
        // before incoming damage or recoil. Never revive a failed candidate.
        // Counter 2/new casts are outside this immediate healing horizon.
        if (hp[actor] && ev->dueWishHeal[actor])
            hp[actor] = min(gBattleMons[actor].maxHP, hp[actor] + ev->dueWishHeal[actor]);
        if (EC_PerishMustEscape(actor))
        {
            // Count before applying ANY countdown deaths. A lower-index foe
            // perishing does not cancel this actor's own pending deadline.
            u32 foeSide = GetBattlerSide(actor) ^ 1;
            bool32 hasFoe = (livingSides & (1u << foeSide)) || ev->board.reserveValue[foeSide] > 0;
            if (hasFoe)
                hp[actor] = 0;
        }
        s32 value = PairMonValue(&ev->board, ev->board.owner[actor], hp[actor], gBattleMons[actor].maxHP)
            * survival[actor] / 100;
        if (hp[actor] && (newWish & (1u << actor)) && !(newParalysisTargets & (1u << actor)))
        {
            u32 foeSide = GetBattlerSide(actor) ^ 1;
            if ((livingSides & (1u << foeSide)) || ev->board.reserveValue[foeSide] > 0)
            {
                u32 amount = min(gBattleMons[actor].maxHP - hp[actor],
                    max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 2));
                u32 chance = actionChance[actor];
                // Half of useful future HP value: Protect consumes next
                // turn's action and the opponent can adapt. This is explicitly
                // a horizon heuristic, never present HP or extra KO survival.
                value += amount * ev->board.healthValue[ev->board.owner[actor]]
                    / max(1, gBattleMons[actor].maxHP) * survival[actor] / 100 * chance / 20000;
            }
        }
        score += GetBattlerSide(actor) == ev->side ? value : -value;
    }
    return score;
}

static s32 ScorePairWithImmediateEffects(struct PairEvaluation *ev)
{
    s32 ordinary = ScoreFastPair(ev, FALSE, NULL);
    if (ordinary <= -10000)
        return ordinary;
    bool32 hasEffect = FALSE;
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        const struct PairAction *action = &ev->action[actor];
        if (action->index != PAIR_IDLE
         && (GetMoveEffect(action->move) == EFFECT_REFLECT
             || (PairSelfDefenseStat(action->move) != STAT_HP
                 && (gBattleMons[actor].status1 & STATUS1_PARALYSIS))
             || ((ev->screenBreakerMoves[actor] & (1u << action->index))
                 && ((gSideStatuses[0] | gSideStatuses[1]) & SIDE_STATUS_SCREEN_ANY))))
            hasEffect = TRUE;
        if (action->index != PAIR_IDLE && GetMoveEffect(action->move) == EFFECT_PROTECT
         && ev->protectChance[actor][action->index] > 0 && ev->protectChance[actor][action->index] < 100)
            hasEffect = TRUE;
        if (action->index != PAIR_IDLE && GetMoveEffect(action->move) == EFFECT_ENCORE)
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (ev->encoreGuardIndex[actor][target])
                    hasEffect = TRUE;
        if (action->index != PAIR_IDLE && ev->paralysisTargets[actor][action->index])
            hasEffect = TRUE;
        if (action->index != PAIR_IDLE && action->move == MOVE_FEINT)
            hasEffect = TRUE;
        if (action->index != PAIR_IDLE && !IsBattleMoveStatus(action->executedMove))
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (gAiLogicData->abilities[target] == ABILITY_STAMINA
                 && gAiLogicData->simulatedDmg[actor][target][action->index].maximum)
                    hasEffect = TRUE;
        if (action->index != PAIR_IDLE && GetMoveEffect(action->move) == EFFECT_KNOCK_OFF)
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (ev->itemBoost[target] && gAiLogicData->simulatedDmg[actor][target][action->index].maximum)
                    hasEffect = TRUE;
        if (action->index != PAIR_IDLE && PairOffensiveDropStat(action->move))
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (ev->attackDropDelta[actor][action->index][target]
                 && (action->move == MOVE_CHARM || gAiLogicData->simulatedDmg[actor][target][action->index].maximum))
                    hasEffect = TRUE;
        if (action->index == PAIR_IDLE || (action->move != MOVE_ELECTROWEB && action->move != MOVE_ICY_WIND && action->move != MOVE_ROCK_TOMB
            && action->move != MOVE_TOXIC_THREAD && action->move != MOVE_SPORE && action->move != MOVE_SING
            && action->move != MOVE_SLEEP_POWDER))
            continue;
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
            if (((action->move == MOVE_SPORE || action->move == MOVE_SING || action->move == MOVE_SLEEP_POWDER)
                 && ev->sleepDenialChance[actor][action->index][target])
             || (ev->speedDelta[actor][action->index][target]
                 && (action->move == MOVE_TOXIC_THREAD || gAiLogicData->simulatedDmg[actor][target][action->index].maximum)))
                hasEffect = TRUE;
    }
    if (!hasEffect)
        return ordinary;
    // Exactly one extra four-action pass, not a branching battle simulation.
    // Damage remains probability-weighted in both passes. Speed changes,
    // Sleep denial and repeated guards share one alternative at their minimum
    // hit/survival chance; independent hit or sleep-turn trees are not built.
    u32 chance = 0;
    s32 changed = ScoreFastPair(ev, TRUE, &chance);
    return ordinary + (changed - ordinary) * (s32)chance / 100;
}

static bool32 RefreshPairMoveData(u32 noActionMask, bool32 canStop)
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
            {
                if (canStop && PairDecisionBudgetExpired())
                    return FALSE;
                CalcBattlerAiMovesData(gAiLogicData, actor, target, AI_GetWeather(), gFieldTimers.terrain);
            }
    }
    return TRUE;
}


static u32 LoadCommittedSwitches(u32 noActionMask)
{
    enum BattlerId actors[2];
    u32 slots[2], count = 0;
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if (!(noActionMask & (1u << actor)) && IsBattlerActionCommitted(actor)
         && gChosenActionByBattler[actor] == B_ACTION_SWITCH
         && gBattleStruct->monToSwitchIntoId[actor] < PARTY_SIZE)
        {
            actors[count] = actor;
            slots[count++] = gBattleStruct->monToSwitchIntoId[actor];
            noActionMask |= 1u << actor;
        }
    if (count == 2)
        AI_LoadSwitchCandidatePair(actors[0], slots[0], actors[1], slots[1], FALSE);
    else if (count == 1)
        AI_LoadSwitchCandidate(actors[0], slots[0], FALSE);
    return noActionMask;
}


static s32 EvaluatePairBoard(enum BattlerId actor, u32 noActionMask, struct PairAction *chosen, struct PairEvaluation *ev, bool32 refresh, bool32 canStop)
{
    s32 best = INT_MIN;
    struct SwitchCandidateSnapshot *state = ev->board.state;
    memset(ev, 0, sizeof(*ev));
    ev->board.state = state;
    ev->side = GetBattlerSide(actor);
    AI_CaptureCandidateState(ev->board.state);
    u32 committedMask = LoadCommittedSwitches(noActionMask);
    refresh |= committedMask != noActionMask;
    noActionMask = committedMask;
    SavePairBoard(&ev->board, ev->side);
    if (refresh && !RefreshPairMoveData(noActionMask, canStop))
        goto done;
    enum BattlerId partner = GetPartnerBattler(actor);
    ev->weather = AI_GetWeather();
    ev->sleepClause = IsSleepClauseEnabled();
    ev->paralysisActionChance = GetConfig(B_PARALYSIS_CHANCE) >= GEN_CHAMPIONS ? 8750 : 7500;
    // The only newly forecast item loss is Knock Off. Avoid native anchor
    // calculations when no other living actor can use it. Do not filter by
    // initial Speed: public speed control can change this turn's order.
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (IsBattlerAlive(battler) && !(noActionMask & (1u << battler)))
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
                if (GetMoveEffect(gBattleMons[battler].moves[index]) == EFFECT_KNOCK_OFF
                 && !IsMoveUnusable(index, gBattleMons[battler].moves[index], gAiLogicData->moveLimitations[battler]))
                    ev->itemRemovers |= 1u << battler;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (canStop && PairDecisionBudgetExpired())
            goto done;
        ev->wishProtectOption[battler] = PairHasWishProtectOption(battler);
        if (gBattleStruct->wish[battler].counter == 1 && !gBattleMons[battler].volatiles.healBlockTimer)
        {
            // Decrypt the caster's party record once per board, never once
            // per candidate action pair. The recipient may have switched.
            u32 sourceHP = GetConfig(B_WISH_HP_SOURCE) >= GEN_5
                ? GetMonData(&GetBattlerParty(battler)[gBattleStruct->wish[battler].partyId], MON_DATA_MAX_HP)
                : PairNonDynamaxHP(ev, battler, gBattleMons[battler].maxHP);
            ev->dueWishHeal[battler] = max(1, sourceHP / 2);
        }
        // A switch recipient cannot use a move this turn. Still model its
        // incoming damage, ability and due Wish, but do not calculate native
        // damage anchors/status effects for four actions absent from the trial.
        if (!(noActionMask & (1u << battler)))
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
                CachePairMoveEffects(ev, battler, index);
        BuildPairActions(ev, battler, noActionMask);
        for (u32 index = 0; index < ev->count[battler]; index++)
        {
            struct PairAction *action = &ev->choices[battler][index];
            action->priority = AI_GetMovePriority(battler, gAiLogicData->abilities[battler], action->move);
            action->planScore = GetBattlerSide(battler) == ev->side ? PairPlanScore(battler, action) : 0;
        }
    }
    // Confirmed human actions have exactly one choice. Only genuinely unknown
    // actors retain the alternative-move/target forecast.
    struct PairAction forecasts[2][2];
    u32 forecastCount = ChooseJointFoeForecast(ev, actor, forecasts);
    enum BattlerId firstFoe = GetOppositeBattler(actor);
    enum BattlerId secondFoe = GetPartnerBattler(firstFoe);
    bool32 canWait = PairWaitingHasPayoff(ev, actor);
    bool32 emptySoloGuard = !canWait && !IsBattlerAlive(partner)
        && ev->board.reserveValue[ev->side] == 0;
    bool32 hasAlternative = FALSE;
    for (u32 index = 0; index < 2; index++)
    {
        enum BattlerId battler = index ? partner : actor;
        for (u32 choice = 0; choice < ev->count[battler]; choice++)
        {
            const struct PairAction *action = &ev->choices[battler][choice];
            if (action->index != PAIR_IDLE && action->score > 0 && action->planScore > -10000
             && !PairIsPassiveGuard(action))
                hasAlternative = TRUE;
        }
    }
    for (u32 left = 0; left < ev->count[actor]; left++)
    {
        for (u32 right = 0; right < ev->count[partner]; right++)
        {
            if (canStop && PairDecisionBudgetExpired())
                goto done;
            ev->action[actor] = ev->choices[actor][left];
            ev->action[partner] = ev->choices[partner][right];
            // Preserving the same board for one turn is not progress. Keep
            // guards beside an acting/pivoting partner and real waiting plans;
            // retain a legal fallback if both actors can only protect.
            if (!canWait && hasAlternative && PairIsPassiveGuard(&ev->action[actor])
             && PairIsPassiveGuard(&ev->action[partner]))
                continue;
            // The first shield also needs a payoff when nobody can act behind
            // it. Otherwise it can forfeit a one-turn opportunity like Fake Out.
            // Keep real waiting payoffs, contact effects and modern side guards.
            if (emptySoloGuard && hasAlternative && PairIsPassiveGuard(&ev->action[actor])
             && GetProtectType(GetMoveProtectMethod(ev->action[actor].move)) == PROTECT_TYPE_SINGLE)
                continue;
            s32 score = INT_MAX;
            for (u32 forecast = 0; forecast < forecastCount; forecast++)
            {
                ev->action[firstFoe] = forecasts[forecast][0];
                ev->action[secondFoe] = forecasts[forecast][1];
                s32 forecastScore = ScorePairWithImmediateEffects(ev);
                score = min(score, forecastScore);
                // A second outcome cannot improve this worst-case score.
                if (score <= best)
                    break;
            }
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
done:
    AI_RestoreCandidateState(ev->board.state);
    return best;
}


s32 AI_EvaluateDoublesPosition(enum BattlerId battler, u32 noActionMask)
{
    struct PairEvaluation *ev = AllocZeroed(sizeof(*ev));
    ev->board.state = AI_SaveCandidateState();
    s32 score = EvaluatePairBoard(battler, noActionMask, NULL, ev, TRUE, FALSE);
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

static bool32 PairNeedsSwitchSearch(enum BattlerId actor)
{
    enum Ability ability = gAiLogicData->abilities[actor];
    if (gBattleMons[actor].species == SPECIES_PALAFIN
     || (ability == ABILITY_REGENERATOR && gBattleMons[actor].hp * 3 <= gBattleMons[actor].maxHP * 2)
     || (ability == ABILITY_NATURAL_CURE && gBattleMons[actor].status1)
     || (ability == ABILITY_TRUANT && gBattleMons[actor].volatiles.truantCounter)
     || gBattleMons[actor].volatiles.perishSong || gBattleMons[actor].volatiles.yawn
     || gBattleMons[actor].statStages[STAT_ATK] <= DEFAULT_STAT_STAGE - 2
     || gBattleMons[actor].statStages[STAT_SPATK] <= DEFAULT_STAT_STAGE - 2
     || gBattleMons[actor].hp * 3 <= gBattleMons[actor].maxHP)
        return TRUE;
    u32 incoming = 0, outgoing = 0;
    bool32 hasAttack = FALSE;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (IsBattlerAlly(actor, foe) || !IsBattlerAlive(foe))
            continue;
        u32 best = 0;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            if (!IsMoveUnusable(index, gBattleMons[foe].moves[index], gAiLogicData->moveLimitations[foe]))
                best = max(best, gAiLogicData->simulatedDmg[foe][actor][index].median);
            enum Move move = gBattleMons[actor].moves[index];
            hasAttack |= move != MOVE_NONE && !IsBattleMoveStatus(move);
            if (!IsMoveUnusable(index, move, gAiLogicData->moveLimitations[actor]))
                outgoing = max(outgoing, gAiLogicData->simulatedDmg[actor][foe][index].median);
        }
        incoming += best;
    }
    // Keep productive positions. Explore every legal reserve when taking
    // serious pressure or when a lock/immunity removes all damaging options.
    return incoming * 2 >= gBattleMons[actor].hp || (hasAttack && outgoing == 0);
}

static bool32 PairCanEndAuthoredGas(enum BattlerId actor, struct Pokemon *mon)
{
    enum BattlerId partner = GetPartnerBattler(actor);
    if (!IsBattlerAlive(partner) || gAiLogicData->abilities[actor] != ABILITY_NEUTRALIZING_GAS
     || GetMonAbility(mon) == ABILITY_NEUTRALIZING_GAS
     || !EmeraldChampions_HasTacticActor(actor, EC_BATTLE_TACTIC_SUPPRESS)
     || (EmeraldChampions_GetPartnerTactics(actor, gBattleMons[actor].species, GetMonData(mon, MON_DATA_SPECIES)) & EC_BATTLE_TACTIC_SUPPRESS)
     || GetMonAbility(&GetBattlerParty(partner)[gBattlerPartyIndexes[partner]]) != ABILITY_TRUANT)
        return FALSE;
    u32 index = GetMoveIndex(partner, MOVE_ENTRAINMENT);
    if (index >= MAX_MON_MOVES || IsMoveUnusable(index, MOVE_ENTRAINMENT, gAiLogicData->moveLimitations[partner]))
        return FALSE;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (battler != actor && IsBattlerAlive(battler) && gBattleMons[battler].volatiles.neutralizingGas)
            return FALSE;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe)
         || gBattleMons[foe].ability == ABILITY_GOOD_AS_GOLD
         || DoesSubstituteBlockMove(partner, foe, MOVE_ENTRAINMENT))
            continue;
        // The existing native legality query needs the post-Gas abilities.
        // Restore these two cache entries immediately; full candidate loading
        // still determines whether a legal exit actually improves the board.
        enum Ability oldPartner = gAiLogicData->abilities[partner], oldFoe = gAiLogicData->abilities[foe];
        gAiLogicData->abilities[partner] = ABILITY_TRUANT;
        gAiLogicData->abilities[foe] = gBattleMons[foe].ability;
        bool32 canEntrain = CanEffectChangeAbility(partner, foe, MOVE_ENTRAINMENT, gAiLogicData);
        gAiLogicData->abilities[partner] = oldPartner;
        gAiLogicData->abilities[foe] = oldFoe;
        if (canEntrain)
            return TRUE;
    }
    return FALSE;
}

static bool32 PairReserveChangesPlan(enum BattlerId actor, u32 slot)
{
    struct Pokemon *mon = &GetBattlerParty(actor)[slot];
    enum Ability ability = GetMonAbility(mon);
    enum BattlerId partner = GetPartnerBattler(actor);
    if (PairCanEndAuthoredGas(actor, mon))
        return TRUE;
    if (IsBattlerAlive(partner)
     && EmeraldChampions_GetPartnerTactics(actor, GetMonData(mon, MON_DATA_SPECIES), gBattleMons[partner].species))
        return TRUE;
    // Weather/terrain restoration and Commander formation can justify a
    // healthy pivot even before damage pressure develops.
    switch (ability)
    {
    case ABILITY_DRIZZLE: return !(gBattleWeather & B_WEATHER_RAIN);
    case ABILITY_DROUGHT:
    case ABILITY_ORICHALCUM_PULSE: return !(gBattleWeather & B_WEATHER_SUN);
    case ABILITY_SAND_STREAM: return !(gBattleWeather & B_WEATHER_SANDSTORM);
    case ABILITY_SNOW_WARNING: return !(gBattleWeather & B_WEATHER_ICY_ANY);
    case ABILITY_ELECTRIC_SURGE:
    case ABILITY_HADRON_ENGINE: return gFieldTimers.terrain != B_TERRAIN_ELECTRIC;
    case ABILITY_GRASSY_SURGE: return gFieldTimers.terrain != B_TERRAIN_GRASSY;
    case ABILITY_MISTY_SURGE: return gFieldTimers.terrain != B_TERRAIN_MISTY;
    case ABILITY_PSYCHIC_SURGE: return gFieldTimers.terrain != B_TERRAIN_PSYCHIC;
    case ABILITY_COMMANDER: return gBattleMons[partner].species == SPECIES_DONDOZO;
    default: break;
    }
    return GetMonData(mon, MON_DATA_SPECIES) == SPECIES_DONDOZO
        && gAiLogicData->abilities[partner] == ABILITY_COMMANDER;
}

static u32 PairReserveTypeFactor(enum BattlerId attacker, enum BattlerId defender, enum Move move,
                               enum Species species, enum Ability attackAbility, enum Ability defendAbility,
                               enum HoldEffect defendItem)
{
    struct DamageContext ctx = {0};
    ctx.battlerAtk = attacker;
    ctx.battlerDef = defender;
    ctx.move = ctx.chosenMove = ctx.baseMove = move;
    ctx.moveType = GetBattleMoveType(move);
    ctx.abilities[attacker] = attackAbility;
    ctx.abilities[defender] = AI_GetMoldBreakerSanitizedAbility(attacker, attackAbility, defendAbility, defendItem, move);
    // These native ability queries also prepare heal/message scratch values,
    // even without a script. Ranking must not leave those writes behind.
    s32 heal = gBattleStruct->passiveHpUpdate[defender];
    u8 message = gBattleCommunication[MULTISTRING_CHOOSER];
    bool32 absorbed = CanAbilityAbsorbMove(&ctx);
    gBattleStruct->passiveHpUpdate[defender] = heal;
    gBattleCommunication[MULTISTRING_CHOOSER] = message;
    if (absorbed || (ctx.moveType == TYPE_GROUND && defendItem == HOLD_EFFECT_AIR_BALLOON
                 && !(gFieldStatuses & STATUS_FIELD_GRAVITY)))
        return 0;
    return CalcPartyMonTypeEffectivenessMultiplier(move, species, ctx.abilities[defender]);
}

static s32 PairRankReserve(enum BattlerId actor, u32 slot)
{
    struct Pokemon *mon = &GetBattlerParty(actor)[slot];
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Ability ability = GetMonAbility(mon);
    enum HoldEffect item = GetItemHoldEffect(GetMonData(mon, MON_DATA_HELD_ITEM));
    enum Type type1 = gSpeciesInfo[species].types[0], type2 = gSpeciesInfo[species].types[1];
    s32 rank = GetMonData(mon, MON_DATA_HP) * 100 / max(1, GetMonData(mon, MON_DATA_MAX_HP));
    if (PairReserveChangesPlan(actor, slot))
        rank += 40;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
            continue;
        u32 attack = 0, threat = 0;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = GetMonData(mon, MON_DATA_MOVE1 + index);
            if (move != MOVE_NONE && !IsBattleMoveStatus(move) && GetMonData(mon, MON_DATA_PP1 + index))
            {
                enum Type type = GetMoveType(move);
                u32 factor = PairReserveTypeFactor(actor, foe, move, gBattleMons[foe].species,
                    ability, gAiLogicData->abilities[foe], gAiLogicData->holdEffects[foe]);
                u32 power = max(40, GetMovePower(move));
                if (type == type1 || type == type2)
                    power = power * 3 / 2;
                attack = max(attack, power * factor / UQ_4_12(1.0));
            }
            move = gBattleMons[foe].moves[index];
            if (!IsMoveUnusable(index, move, gAiLogicData->moveLimitations[foe]) && !IsBattleMoveStatus(move))
            {
                u32 factor = PairReserveTypeFactor(foe, actor, move, species,
                    gAiLogicData->abilities[foe], ability, item);
                threat = max(threat, max(40, GetMovePower(move)) * factor / UQ_4_12(1.0));
            }
        }
        rank += (s32)attack - (s32)threat;
    }
    return rank;
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
    if (!IsDoubleBattle() || !IsBattlerAlive(actor) || !BattlerHasAi(partner)
        || gAiLogicData->aiPredictionInProgress
        || !(gAiThinkingStruct->aiFlags[actor] & AI_FLAG_SMART_MON_CHOICES)
        || (IsBattlerAlive(partner) && !(gAiThinkingStruct->aiFlags[partner] & AI_FLAG_SMART_MON_CHOICES)))
        return FALSE;
    if (gAiLogicData->battlerMovesScored & (1u << actor))
        return TRUE;
    struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
    struct PairEvaluation *ev = AllocZeroed(sizeof(*ev));
    ev->board.state = AI_SaveCandidateState();
    u8 reserves[2][PARTY_SIZE + 1] = {{PARTY_SIZE}, {PARTY_SIZE}};
    u32 count[2] = {1, 1};
    enum BattlerId actors[2] = {actor, partner};
    u32 canMega = (CanMegaEvolve(actor) ? 1u : 0u) | (IsBattlerAlive(partner) && CanMegaEvolve(partner) ? 2u : 0u);
    bool32 deadline[2] = {EC_PerishMustEscape(actor), EC_PerishMustEscape(partner)};
    bool32 earlyPivot[2] = {EC_PerishShouldPivotEarly(actor), EC_PerishShouldPivotEarly(partner)};
    for (u32 index = 0; index < 2; index++)
        if (PairCanSwitch(actors[index]))
        {
            bool32 pressured = PairNeedsSwitchSearch(actors[index]) || earlyPivot[index];
            s32 bestRank = INT_MIN;
            s32 secondRank = INT_MIN;
            for (u32 slot = 0; slot < GetAILastPartyIndex(actors[index]); slot++)
                if (PairLegalReserve(actors[index], slot) && (pressured || PairReserveChangesPlan(actors[index], slot)))
                {
                    s32 rank = PairRankReserve(actors[index], slot);
                    if (rank > bestRank)
                    {
                        secondRank = bestRank;
                        reserves[index][2] = reserves[index][1];
                        bestRank = rank;
                        reserves[index][1] = slot;
                        count[index] = 2;
                    }
                    else if (rank > secondRank)
                    {
                        secondRank = rank;
                        reserves[index][2] = slot;
                    }
                }
            // Two simultaneous deadlines need distinct recipients. Retain one
            // alternate on the right, without expanding ordinary switch search.
            if (index == 1 && deadline[0] && deadline[1] && secondRank != INT_MIN)
                count[index] = 3;
        }
    struct PairAction chosen[2], bestActions[2];
    u32 bestReserves[2] = {PARTY_SIZE, PARTY_SIZE};
    u32 bestMega = 0, bestTieCost = UINT_MAX;
    u32 activeMega = 0;
    s32 best = INT_MIN;
    for (u32 left = 0; left < count[0]; left++)
    {
        for (u32 right = 0; right < count[1]; right++)
        {
            u32 slots[2] = {reserves[0][left], reserves[1][right]};
            // Voluntary double switches spend both actions and multiply the
            // candidate space. Forced double replacements are ranked separately.
            if (slots[0] < PARTY_SIZE && slots[1] < PARTY_SIZE)
            {
                if (!(deadline[0] && deadline[1]))
                    continue;
                if (slots[0] == slots[1] && IsPartnerMonFromSameTrainer(actor))
                    continue;
            }
            for (u32 mega = 0; mega < 4; mega++)
            {
                bool32 canStop = best != INT_MIN && !deadline[0] && !deadline[1];
                if (canStop && PairDecisionBudgetExpired())
                    goto decisionReady;
                // Reuse the active pair's Mega choice for a voluntary pivot.
                // Forced replacements still evaluate every legal reserve.
                if (left || right)
                {
                    u32 retainedMega = activeMega;
                    if (slots[0] < PARTY_SIZE)
                        retainedMega &= ~1u;
                    if (slots[1] < PARTY_SIZE)
                        retainedMega &= ~2u;
                    if (mega != retainedMega)
                        continue;
                }
                if ((mega == 3 && IsPartnerMonFromSameTrainer(actor))
                    || (mega & ~canMega)
                    || ((mega & 1) && slots[0] < PARTY_SIZE)
                    || ((mega & 2) && slots[1] < PARTY_SIZE))
                    continue;
                AI_RestoreCandidateState(state);
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
                s32 score = EvaluatePairBoard(actor, noActionMask, chosen, ev, mega != 0 || noActionMask != 0, canStop);
                if (score == INT_MIN)
                    continue;
                // Demand a meaningful improvement before voluntarily giving up
                // an action; ties and tiny forecast noise must not cause cycling.
                if (noActionMask)
                    score -= 35;
                // An authored singer leaves one turn early while its partner
                // still traps the affected foes. This is a conditional phase
                // preference, never an excuse to bypass reserve legality.
                for (u32 index = 0; index < 2; index++)
                    if (earlyPivot[index] && slots[index] < PARTY_SIZE)
                        score += 70;
                if (score > best || (score == best && tieCost < bestTieCost))
                {
                    best = score;
                    bestTieCost = tieCost;
                    bestMega = mega;
                    memcpy(bestActions, chosen, sizeof(bestActions));
                    memcpy(bestReserves, slots, sizeof(bestReserves));
                    if (left == 0 && right == 0)
                        activeMega = bestMega;
                }
            }
        }
    }
decisionReady:
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
