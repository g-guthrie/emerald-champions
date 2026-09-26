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
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/items.h"
#include "constants/battle_script_commands.h"

// Four moves, each with at most four legal targets. Switch recipients and
// Mega forms are board alternatives outside this list. Voluntary reserves use
// a bounded, ability-aware shortlist; forced replacements retain every slot.
#define PAIR_ACTIONS (MAX_MON_MOVES * MAX_BATTLERS_COUNT)
#define PAIR_IDLE MAX_MON_MOVES
#define PAIR_SOAK_ATTACKER 1
#define PAIR_SOAK_TARGET 2
#define PAIR_CHARGED 4
#define PAIR_SOAK_CHARGE_STATES 8
#define PAIR_DANCE_NO_ITEM 8
#define PAIR_DANCE_TARGET_NO_ITEM 16
#define PAIR_DANCE_STATES 32

// Opponent model. The pair search cannot see the human's pending commands, so
// every trial is scored against a small weighted set of joint foe forecasts
// instead of a single assumed-worst pattern: the strongest damage pattern, a
// credible alternative (redirection, or the same foes focusing the other
// target), and a passive turn that damages neither of us (spread that misses
// our slots, setup, support). Weights are percentages; whatever the available
// patterns do not use falls back to the primary one.
#define PAIR_FORECASTS 3
#define PAIR_FORECAST_ALTERNATE 30
#define PAIR_FORECAST_PASSIVE 15
// A lead holding Fake Out on an opening spends it about a third of the time,
// split across our bodies by the threat each poses to its side.
#define PAIR_FORECAST_FAKE_OUT 35
// A foe that has raised Wide Guard in its last few moves raises it again while
// our side keeps spread moves in hand: the guard never fails on repeat, so the
// player loses nothing by holding it up. Maxie's Torkoal and Camerupt fed Heat
// Wave and Eruption into a visible Pelipper's Wide Guard on six of seven turns.
#define PAIR_FORECAST_WIDE_GUARD 50
// A redirector drawing a hit it is immune to or absorbs gives up nothing by
// drawing it again, so it is the likeliest pattern, not an alternative.
#define PAIR_FORECAST_REDIRECT_SAFE 60
// Expected value drives the choice. A small pessimism share keeps a rare
// catastrophe visible without restoring assume-the-worst guarding.
#define PAIR_RISK_AVERSION 25
// Scoring every candidate pair against every forecast does not fit the 1.2s
// decision budget. Rank the pairs once against the primary forecast, then
// settle the shortlist on expected value. A shield still reaches the
// shortlist: it scores well against the pattern it is meant to answer.
#define PAIR_SHORTLIST 4
// A multi shares one decision budget between three AI actors. Halving the
// shortlist keeps the settle stage bounded without changing how a pair is
// valued, which must stay identical across every board of one decision.
#define PAIR_SHORTLIST_CROWDED 2
// One decision searches the same number of pairs on every arm. The allowance
// is fixed once per decision from the board in front of the AI, so a switch
// candidate is never compared against a stay board that was searched deeper.
#define PAIR_WORK_MIN 24
#define PAIR_WORK_MAX 96

// Guard policy. A shield's simulated HP saving is not all permanent: with no
// payoff the same threat simply returns next turn and the foes can focus the
// unprotected ally instead. Bank only the share a payoff makes real, and pay a
// flat tempo cost so an empty shield loses ties to an action that does something.
#define PAIR_GUARD_TEMPO 10
#define PAIR_GUARD_BANKED_BASE 45
#define PAIR_GUARD_BANKED_WAIT 40
#define PAIR_GUARD_BANKED_PARTNER 25
// The Conservative trait is a stated preference for the safe line, and the
// banked share is exactly where safety is valued. Without this the trait had
// no contact with the guard model at all, which is why a team whose four
// members all carried Protect never used it.
#define PAIR_GUARD_BANKED_CONSERVATIVE 20
// A guard that denies nothing at all bought nothing at all. The banked share
// prices what a shield keeps, so it says nothing about a shield with no denial
// to keep - and a turn-one guard from full health was still winning the safest
// setup or status turn its side would get.
#define PAIR_GUARD_EMPTY_COST 30
// Two guards on the same turn are one decision about the whole turn, not two
// independent ones. Each can look right alone while together they pass the
// turn and halve both guards next turn, so the pair pays unless the payoff is
// a whole-turn one: a weather, Tailwind or Trick Room clock running out.
#define PAIR_GUARD_CORRELATED_COST 45

// An authored signature interaction (ACTIVATE / INSTRUCT / AFTER_YOU) is the
// point of the trainer, not an accident of the damage arithmetic. Reward it on
// the same bounded scale as the other authored plans, and only while the
// recipient is still alive to use what the trigger gives it.
#define PAIR_TACTIC_REWARD 80
// A Mega's worth is rarely this turn's damage: Parental Bond, a Speed or bulk
// jump and the ability that comes with the form all pay over the turns after
// it. Scored only on the turn's damage, a Kangaskhan holds its stone, takes
// Fake Out and Sucker Punch and faints in base form with Parental Bond never
// existing. Evolving is the default once the holder acts; the ordinary board
// comparison still overrules it when the base form has something the Mega
// loses, and the existing tie cost still prefers not evolving on a true tie.
#define PAIR_MEGA_HORIZON 50
// A support move that reaches a partner which cannot use it spent the turn for
// nothing; the trial already gives it no benefit, and this makes it lose to an
// action that does something rather than to a tie.
#define PAIR_SUPPORT_WASTED_COST 40
// A move that fails on the board a visible foe's Mega leaves. Trainers Mega
// on the first turn they can, so that board is the forecast; this is the
// failure priced as likely rather than the certain-failure veto.
#define PAIR_MEGA_FORECAST_FAIL_COST 150
// A hit on a foe the end of the turn finishes after its own last action. Not
// a veto: a switch out of that slot would meet the hit instead.
#define PAIR_DOOMED_TARGET_COST 100

// A single turn cannot see what a boost, a sleep or a stat drop is worth,
// because all of their value arrives on the turns after this one. Without an
// explicit horizon they price at zero and lose to any direct attack, which is
// how three separate teams went a whole battle without using Tailwind and a
// Quiver Dance user never danced. These are bounded next-turn values, paid only
// when the effect actually landed and survives the turn - the same shape the
// Wish horizon already uses - never a quota.
#define PAIR_SETUP_HORIZON 40
#define PAIR_SLEEP_HORIZON 35
#define PAIR_STATUS_HORIZON 20
#define PAIR_STAT_DROP_HORIZON 12
// A burn halves physical damage as well as ticking, so which body it goes on
// matters as much as landing it: four turns of an unused Will-O-Wisp in front
// of a Life Orb physical attacker is the case this exists for.
#define PAIR_BURN_PHYSICAL_HORIZON 20
// A speed drop that puts one of ours in front of the target is speed control
// rather than chip, which is the turn a slow partner needs bought for it.
#define PAIR_SPEED_CROSSING_HORIZON 25
// A switch preserves board value while an attack spends HP, so a one-turn
// board makes withdrawing a healthy lead look nearly free. Charge it for the
// turn it actually gives up - the damage the outgoing battler was about to
// deal - on top of the flat commitment cost, capped so a real escape stays
// affordable. Turn one, with nothing revealed, costs a little more again.
#define PAIR_SWITCH_COMMITMENT 35
#define PAIR_SWITCH_TEMPO_CAP 70
#define PAIR_SWITCH_BLIND_COST 60
// More than a whole member, so an exit that loses the body it brings in can
// never win on a margin.
#define PAIR_SWITCH_INTO_DEATH 220
// A foe move the body leaving does not resist: not a hit its switch dodges.
// See PairOutgoingLoss.
#define PAIR_NOT_RELIEVED 0xFF
// A Choice lock into a move that every foe is immune to or resists is not one
// wasted turn but every turn until it leaves, which a one-turn board cannot
// see. Credit the exit with the turns it recovers: more than the commitment,
// so a reasonable reserve wins, and far less than a lethal entry.
#define PAIR_USELESS_LOCK_ESCAPE 70
// The first move a Choice holder uses is the move it is held to next turn, so
// a lock is worth what that move still does to the foes left standing. A
// sure knockout of one foe beside a move the other foe walls leaves a lock
// that buys nothing, which is where the escape above begins.
#define PAIR_CHOICE_LOCK_HORIZON 40

static const enum Move sCopiedDances[] = {MOVE_PETAL_DANCE, MOVE_FIERY_DANCE, MOVE_REVELATION_DANCE, MOVE_AQUA_STEP, MOVE_FEATHER_DANCE};

// Stop optional comparisons after one second, leaving room under the 1.2s
// complete-decision limit for the current native calculation and restoration.
// Always establish a legal fallback; urgent Perish exits retain their search.
// AI battlers that still have to be scored out of this turn's shared budget.
static u32 PairPendingDecisionActors(void)
{
    u32 actors = 0;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (IsBattlerAlive(battler) && BattlerHasAi(battler)
         && !(gAiLogicData->battlerMovesScored & (1u << battler)))
            actors++;
    return actors;
}

static u32 PairShortlistSize(void)
{
    return PairPendingDecisionActors() > 2 ? PAIR_SHORTLIST_CROWDED : PAIR_SHORTLIST;
}

// One turn's opposing decision can involve more than one group of AI actors:
// a two-owner multi scores both opponents together and then the in-game
// partner separately, and every one of them spends the same shared budget.
// Give each group an equal slice of it, measured from the shared start, so the
// complete decision still lands inside the limit however many groups there are.
static u32 PairDecisionBudgetShare(void)
{
    u32 groups = 0, pending = 0;
    for (u32 side = 0; side < NUM_BATTLE_SIDES; side++)
    {
        bool32 present = FALSE, unscored = FALSE;
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            if (!IsBattlerAlive(battler) || !BattlerHasAi(battler) || GetBattlerSide(battler) != side)
                continue;
            present = TRUE;
            if (!(gAiLogicData->battlerMovesScored & (1u << battler)))
                unscored = TRUE;
        }
        if (present)
            groups++;
        if (unscored)
            pending++;
    }
    if (groups <= 1)
        return 60;
    return 60 * (groups - pending + 1) / groups;
}

#if TESTING
// Test hook: the next joint decision starts with its budget already spent, as
// a shared multi clock can leave it. Consumed by that one decision.
EWRAM_DATA bool8 gTestPairBudgetSpent = FALSE;
static EWRAM_DATA bool8 sTestPairBudgetSpentNow = FALSE;
// Test hook: the next opposing joint decision's clock runs out once it has
// scored this many pairs, as the shared multi clock did after seven of the
// Mossdeep board's forty-nine. Consumed by that one decision.
EWRAM_DATA u8 gTestPairBudgetPairs = 0;
static EWRAM_DATA u8 sTestPairBudgetPairsNow = 0;
static EWRAM_DATA u16 sTestPairsScored = 0;
#endif

static bool32 PairDecisionBudgetExpired(void)
{
#if TESTING
    if (sTestPairBudgetSpentNow)
        return TRUE;
    if (sTestPairBudgetPairsNow && sTestPairsScored >= sTestPairBudgetPairsNow)
        return TRUE;
#endif
    return (u32)(gMain.vblankCounter1 - gAiLogicData->decisionStartFrame) >= PairDecisionBudgetShare();
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
    s16 tacticScore;
    s8 priority;
    enum Move executedMove; // Nature Power's called move; move/index remain the command.
    u8 danceCopy; // Copied-dance index + 1; zero is an ordinary selected action.
};

struct PairBoard
{
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

struct PairRageCache
{
    struct SimulatedDamage damage[MAX_BATTLERS_COUNT][4][7];
    u8 states[MAX_BATTLERS_COUNT];
};

struct PairDefenderItemCache
{
    u8 mask, count;
    u8 stateIndex[PAIR_SOAK_CHARGE_STATES];
    u8 moves;
    struct SimulatedDamage damage[]; // Move slot, then compact field state.
};

struct PairDancerDamage
{
    struct SimulatedDamage damage;
    uq4_12_t effectiveness;
};

struct PairDancerMoveCache
{
    u8 mask, count;
    u8 stateIndex[PAIR_DANCE_STATES];
    u8 accuracy[4][MAX_BATTLERS_COUNT];
    u16 contact[4][MAX_BATTLERS_COUNT];
    s8 targetDropDelta[2][MAX_BATTLERS_COUNT];
    u8 screened[2];
    enum Type type[2];
    struct PairDancerDamage damage[];
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
    s8 targetDropDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    s8 smashDefenseDelta[MAX_BATTLERS_COUNT][2];
    s8 selfDefenseDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES]; // Native immediate Def/SpDef changes; see PairSelfDefenseStat.
    s8 speedDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 sleepDenialChance[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 paralysisTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 burnTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 quashTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 tauntTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
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
    // Native damage anchors for type changes and Charge, only populated on
    // boards with those actual possibilities. No native calls inside pair trials.
    struct SimulatedDamage soakChargeDamage[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT][PAIR_SOAK_CHARGE_STATES];
    u8 soakChargeStates[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    u8 soakTargets[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 soakRedirectors[MAX_BATTLERS_COUNT];
    u8 soakable;
    u8 chargeable;
    u8 electricMoves[MAX_BATTLERS_COUNT][2]; // Original / Soaked attacker; native dynamic move typing.
    // Whether this move would start a Choice lock, the small tie-break value
    // the foes' forecast gives it, and what it takes from each foe's maximum
    // HP, as a percentage.
    bool8 firstChoice[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 firstChoiceValue[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    u8 firstChoiceShare[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    struct PairDefenderItemCache *defenderItem[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT];
    struct PairRageCache *rage[MAX_BATTLERS_COUNT];
    u8 rageActors;
    u8 rageHitCount[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT];
    s8 danceDelta[MAX_BATTLERS_COUNT][MAX_MON_MOVES][MAX_BATTLERS_COUNT][5]; // Atk/Def/SpDef/SpAtk/Speed.
    struct PairDancerMoveCache *dancer[MAX_BATTLERS_COUNT][ARRAY_COUNT(sCopiedDances)];
    enum Type revelationType[MAX_BATTLERS_COUNT][2];
    u8 protectChance[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    bool8 waitingPayoff; // Evaluating side has a concrete reason to spend a turn waiting.
    bool8 wholeTurnPayoff; // ...and it is a field clock, which one turn of waiting spends for both slots.
    u8 encoreGuardIndex[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT]; // Move slot + 1; zero is ineligible.
    u8 entering; // Our bodies arriving by switch on this board.
    s8 encoreGuardPriority[MAX_BATTLERS_COUNT];
    bool8 sleepClause;
    // What a held healing Berry will restore when it is eaten, this turn or a
    // later one. See the board value in ScoreFastPair.
    u16 berryHeal[MAX_BATTLERS_COUNT];
    // End-of-turn damage each battler is certain to take (AI_GetCertainResidualDamage).
    u16 residual[MAX_BATTLERS_COUNT];
};

static u32 PairCopiedDance(enum Move move)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCopiedDances); i++)
        if (sCopiedDances[i] == move)
            return i;
    return ARRAY_COUNT(sCopiedDances);
}

static bool32 PairHasCopiedDance(const struct PairEvaluation *ev, enum BattlerId source, enum Move move)
{
    u32 dance = PairCopiedDance(move);
    if (dance >= ARRAY_COUNT(sCopiedDances))
        return FALSE;
    for (enum BattlerId dancer = 0; dancer < gBattlersCount; dancer++)
        if (dancer != source && ev->dancer[dancer][dance] != NULL)
            return TRUE;
    return FALSE;
}

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

static bool32 PairSpread(enum Move move)
{
    enum MoveTarget target = GetMoveTarget(move);
    return target == TARGET_BOTH || target == TARGET_FOES_AND_ALLY || target == TARGET_ALL_BATTLERS;
}

static void BuildPairActions(struct PairEvaluation *ev, enum BattlerId actor, u32 noActionMask)
{
    ev->count[actor] = 0;
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
                 && !PairHasCopiedDance(ev, actor, executedMove)
                 // An authored ACTIVATE is a deliberate hit on your own
                 // partner, so the isolated opinion of hitting an ally is the
                 // wrong gate for it: the trigger would never be enumerated and
                 // the reward that judges it would never be reached. Legality,
                 // lethality and usefulness are still decided by PairTacticScore.
                 && !EmeraldChampions_GetTacticKind(actor, target, move)
                 && (score < AI_SCORE_DEFAULT || GetBattlerSide(actor) != ev->side))
                    continue;
                // Enabling actions are retained even when their independent
                // heuristic cannot yet see a concrete partner action.
                ev->choices[actor][ev->count[actor]++] = (struct PairAction){move, score, index, target, .executedMove = executedMove};
            }
        }
    }
    {
        // Known single-target failures are not enabling actions on a foe.
        // This runs for the opposing enumeration too: those actions are
        // damage forecasts, and one that cannot affect its target contributes
        // no damage and no target pattern, so carrying it only makes the joint
        // forecast's comparison larger.
        // Preserve allied activation and multi-recipient/retargeting moves:
        // other recipients may be vulnerable. If every option fails, retain the actual
        // legal moves rather than inventing Struggle or an unavailable action.
        u32 useful = 0;
        for (u32 index = 0; index < ev->count[actor]; index++)
        {
            const struct PairAction *action = &ev->choices[actor][index];
            enum MoveTarget targetType = GetMoveTarget(action->executedMove);
            // A move the engine is certain to refuse on this board - its
            // effect already up, or the target immune by type, ability, item,
            // Substitute or field - is not an option for either side: the
            // shared check the standard AI zeroes it with.
            if (AI_IsMoveCertainToFail(actor, action->target, action->executedMove))
                continue;
            // A spread attack every foe is visibly immune to reaches nobody
            // but the partner; it stays only when that hit is the point.
            if (AI_IsSpreadMoveWasted(actor, action->executedMove))
                continue;
            if (!IsBattlerAlly(actor, action->target)
             && targetType != TARGET_BOTH && targetType != TARGET_FOES_AND_ALLY
             && targetType != TARGET_ALL_BATTLERS && targetType != TARGET_RANDOM
             && targetType != TARGET_SMART)
            {
                if (!IsBattleMoveStatus(action->executedMove)
                 && !(ev->soakable & ((1u << actor) | (1u << action->target)))
                 && gAiLogicData->effectiveness[actor][action->target][action->index] == UQ_4_12(0.0))
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

// Helping Hand multiplies an attack's power. A move whose damage is a fixed
// number, a fraction of the target's HP, or a reflection of damage taken
// ignores that multiplier, so the turn spent boosting it buys nothing.
bool32 IsFixedDamageMove(enum Move move)
{
    switch (GetMoveEffect(move))
    {
    case EFFECT_FIXED_HP_DAMAGE:
    case EFFECT_FIXED_PERCENT_DAMAGE:
    case EFFECT_LEVEL_DAMAGE:
    case EFFECT_PSYWAVE:
    case EFFECT_ENDEAVOR:
    case EFFECT_FINAL_GAMBIT:
    case EFFECT_REFLECT_DAMAGE:
    case EFFECT_OHKO:
    case EFFECT_BIDE:
        return TRUE;
    default:
        return FALSE;
    }
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

static u32 PairWeatherPower(enum BattlerId actor, enum BattlerId target, enum Move move, enum Type type, u32 weather)
{
    // Match native GetWeatherDamageModifier: the defender's Umbrella blocks
    // ordinary rain/sun damage modifiers. The attacker's Umbrella does not
    // remove the global weather's outgoing damage modifier.
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

// The HP a held healing Berry restores whenever it is eaten, whatever the
// holder's HP now. Zero for anything else, a disliked flavor or Heal Block.
static u32 PairHeldBerryHeal(const struct PairEvaluation *ev, enum BattlerId battler)
{
    enum Item item = gBattleMons[battler].item;
    enum HoldEffect effect = gAiLogicData->holdEffects[battler];
    u32 amount;
    if (!IsBattlerAlive(battler)
     || (B_HEAL_BLOCKING >= GEN_5 && gBattleMons[battler].volatiles.healBlockTimer))
        return 0;
    if (effect == HOLD_EFFECT_RESTORE_PCT_HP)
        amount = PairNonDynamaxHP(ev, battler, gBattleMons[battler].maxHP) * GetItemHoldEffectParam(item) / 100;
    else if (effect == HOLD_EFFECT_RESTORE_HP)
        amount = GetItemHoldEffectParam(item);
    else if (effect == HOLD_EFFECT_CONFUSE_FLAVOR
          && GetFlavorRelationByPersonality(gBattleMons[battler].personality, GetItemSecondaryId(item)) >= 0)
        amount = PairNonDynamaxHP(ev, battler, gBattleMons[battler].maxHP) / max(1, GetItemHoldEffectParam(item));
    else
        return 0;
    if (gAiLogicData->abilities[battler] == ABILITY_RIPEN && GetItemPocket(item) == POCKET_BERRIES)
        amount *= 2;
    return max(1, amount);
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
     && ((gAiLogicData->holdEffects[target] == HOLD_EFFECT_FOCUS_SASH && !IsFocusSashBypassed(move, gAiLogicData->abilities[target]))
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

static bool32 PairSameForecast(const struct PairAction left[2], const struct PairAction right[2]);

static u32 ChooseJointFoeForecast(struct PairEvaluation *ev, enum BattlerId actor,
    struct PairAction forecasts[PAIR_FORECASTS][2], u8 weights[PAIR_FORECASTS])
{
    enum BattlerId first = GetOppositeBattler(actor);
    enum BattlerId second = GetPartnerBattler(first);
    enum BattlerId partner = GetPartnerBattler(actor);
    weights[0] = 100;
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
    u32 count = 1;
    // A revealed redirector is a credible alternative to the strongest damage
    // forecast. Otherwise repeated Follow Me can keep feeding an immune
    // recipient while every trial assumes the foe attacks instead. Choices
    // already exclude exhausted, Taunted and otherwise unavailable moves.
    // Retain the primary damage forecast: last turn's move is not a command.
    for (u32 foe = 0; foe < ARRAY_COUNT(foes) && count == 1; foe++)
    {
        enum BattlerId redirector = foes[foe];
        enum Move previous = gAiLogicData->lastUsedMove[redirector];
        if (!HasPartner(redirector)
         || (previous != MOVE_FOLLOW_ME && previous != MOVE_RAGE_POWDER)
         || IsBattlerIncapacitated(redirector, gAiLogicData->abilities[redirector]))
            continue;
        // A redirector that takes nothing from what it draws - a Volt Absorb
        // body drawing our Thunderbolt - loses nothing by drawing it again, and
        // every one of those hits heals it: the Route 103 rival's Pikachu
        // Thunderbolted into Follow Me from a Volt Absorb Pachirisu twice.
        u32 weight = PAIR_FORECAST_ALTERNATE;
        for (u32 body = 0; body < 2 && weight == PAIR_FORECAST_ALTERNATE; body++)
            for (u32 choice = 0; choice < ev->count[targets[body]]; choice++)
            {
                const struct PairAction *mine = &ev->choices[targets[body]][choice];
                if (mine->index != PAIR_IDLE && !IsBattleMoveStatus(mine->executedMove) && !PairSpread(mine->executedMove)
                 && mine->target == foes[foe ^ 1]
                 && !IsMoveRedirectionPrevented(targets[body], mine->executedMove, gAiLogicData->abilities[targets[body]])
                 && gAiLogicData->simulatedDmg[targets[body]][foes[foe ^ 1]][mine->index].maximum
                 && !gAiLogicData->simulatedDmg[targets[body]][redirector][mine->index].maximum)
                {
                    weight = PAIR_FORECAST_REDIRECT_SAFE;
                    break;
                }
            }
        for (u32 choice = 0; choice < ev->count[redirector]; choice++)
        {
            const struct PairAction *action = &ev->choices[redirector][choice];
            if (action->index == PAIR_IDLE || action->move != previous)
                continue;
            forecasts[1][0] = forecasts[0][0];
            forecasts[1][1] = forecasts[0][1];
            forecasts[1][foe] = *action;
            weights[1] = weight;
            count = 2;
            break;
        }
    }
    // A Wide Guard the foe keeps raising is the pattern our spread moves meet
    // next. It is judged on public history - the foe's own last moves - and
    // only while our pair holds a spread attack for it to stop.
    bool32 ourSpread = FALSE;
    for (u32 body = 0; body < 2 && !ourSpread; body++)
        for (u32 choice = 0; choice < ev->count[targets[body]]; choice++)
        {
            const struct PairAction *mine = &ev->choices[targets[body]][choice];
            if (mine->index != PAIR_IDLE && !IsBattleMoveStatus(mine->executedMove) && PairSpread(mine->executedMove))
                ourSpread = TRUE;
        }
    for (u32 foe = 0; foe < ARRAY_COUNT(foes) && ourSpread && count < PAIR_FORECASTS; foe++)
    {
        enum BattlerId guard = foes[foe];
        bool32 raised = FALSE;
        for (u32 index = 0; index < AI_MOVE_HISTORY_COUNT; index++)
            if (gBattleHistory->moveHistory[guard][index] == MOVE_WIDE_GUARD)
                raised = TRUE;
        if (!raised || !IsBattlerAlive(guard) || IsBattlerIncapacitated(guard, gAiLogicData->abilities[guard]))
            continue;
        for (u32 choice = 0; choice < ev->count[guard]; choice++)
        {
            const struct PairAction *action = &ev->choices[guard][choice];
            if (action->index == PAIR_IDLE || action->move != MOVE_WIDE_GUARD)
                continue;
            struct PairAction guarded[2] = {forecasts[0][0], forecasts[0][1]};
            guarded[foe] = *action;
            if (!PairSameForecast(guarded, forecasts[0]))
            {
                forecasts[count][0] = guarded[0];
                forecasts[count][1] = guarded[1];
                weights[count] = PAIR_FORECAST_WIDE_GUARD;
                count++;
            }
            break;
        }
    }
    // A foe that can still Fake Out is a credible pattern of its own. The
    // joint forecast ranks patterns by damage, so a turn-one Fake Out never
    // appeared, and every board assumed the body it would stop gets to act:
    // Drake's Reshiram chose its Earth Power on the Double-Edge a Fake Out
    // then took away from Salamence. The Fake Out is forecast into each of our
    // bodies in proportion to the damage that body would do to the foes this
    // turn, and the foe's partner keeps its primary action. Which of our
    // bodies the player actually picks is never read. It takes the place of
    // the other-target and passive patterns on an opening - every body on the
    // field just arrived - where a lead Fake Out is the standard line; a fresh
    // body beside a partner already in play is judged on its damage.
    bool32 opening = TRUE;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (IsBattlerAlive(battler) && !gBattleStruct->battlerState[battler].isFirstTurn)
            opening = FALSE;
    if (count < PAIR_FORECASTS && opening)
    {
        bool32 found = FALSE;
        u32 threat[2] = {0, 0};
        for (u32 target = 0; target < 2; target++)
        {
            enum BattlerId body = targets[target];
            if (!IsBattlerAlive(body) || (ev->entering & (1u << body))
             || gAiLogicData->abilities[body] == ABILITY_INNER_FOCUS
             || gAiLogicData->abilities[body] == ABILITY_SHIELD_DUST
             || gAiLogicData->holdEffects[body] == HOLD_EFFECT_COVERT_CLOAK)
                continue;
            for (u32 foe = 0; foe < 2; foe++)
            {
                if (!IsBattlerAlive(foes[foe]))
                    continue;
                u32 best = 0;
                for (u32 index = 0; index < MAX_MON_MOVES; index++)
                    if (!IsMoveUnusable(index, gBattleMons[body].moves[index], gAiLogicData->moveLimitations[body]))
                        best = max(best, min(gBattleMons[foes[foe]].hp, gAiLogicData->simulatedDmg[body][foes[foe]][index].median));
                threat[target] += best * 100 / max(1, gBattleMons[foes[foe]].maxHP);
            }
        }
        // Aimed at either body in proportion to what each threatens: the
        // stronger hitter is the likelier target, never the certain one.
        u32 totalThreat = threat[0] + threat[1];
        for (u32 foe = 0; foe < 2 && !found && totalThreat; foe++)
        {
            // A foe whose strongest line already knocks one of our bodies out
            // takes the knockout, not the Fake Out: Juan's Manaphy is not
            // spared Iron Hands' Wild Charge for a Tail Glow to land.
            const struct PairAction *primary = &forecasts[0][foe];
            if (!gBattleStruct->battlerState[foes[foe]].isFirstTurn
             || (primary->index != PAIR_IDLE && !IsBattleMoveStatus(primary->executedMove)
                 && primary->target < gBattlersCount && IsBattlerAlive(primary->target)
                 && !IsBattlerAlly(foes[foe], primary->target)
                 && gAiLogicData->simulatedDmg[foes[foe]][primary->target][primary->index].minimum
                    >= gBattleMons[primary->target].hp))
                continue;
            for (u32 target = 0; target < 2; target++)
            {
                u32 weight = PAIR_FORECAST_FAKE_OUT * threat[target] / totalThreat;
                if (!weight || count >= PAIR_FORECASTS)
                    continue;
                for (u32 choice = 0; choice < ev->count[foes[foe]]; choice++)
                {
                    const struct PairAction *action = &ev->choices[foes[foe]][choice];
                    if (action->index == PAIR_IDLE || GetMoveEffect(action->move) != EFFECT_FIRST_TURN_ONLY
                     || action->target != targets[target]
                     || !gAiLogicData->simulatedDmg[foes[foe]][targets[target]][action->index].maximum)
                        continue;
                    struct PairAction fakeOut[2] = {forecasts[0][0], forecasts[0][1]};
                    fakeOut[foe] = *action;
                    found = TRUE;
                    if (!PairSameForecast(fakeOut, forecasts[0]))
                    {
                        forecasts[count][0] = fakeOut[0];
                        forecasts[count][1] = fakeOut[1];
                        weights[count] = weight;
                        count++;
                    }
                    break;
                }
            }
        }
    }
    // A fixed target forecast can make Protect look like it shields both
    // allies: the opponent could simply focus the unprotected partner. Keep
    // the strongest different damage-target pattern from the same enumeration.
    if (count == 1 && bestMask != 0)
    {
        u32 alternate = 0;
        for (u32 mask = 1; mask < ARRAY_COUNT(maskScores); mask++)
            if (mask != bestMask && maskScores[mask] > 0
             && (alternate == 0 || maskScores[mask] > maskScores[alternate]))
                alternate = mask;
        if (alternate != 0)
        {
            forecasts[1][0] = maskChoices[alternate][0];
            forecasts[1][1] = maskChoices[alternate][1];
            weights[1] = PAIR_FORECAST_ALTERNATE;
            count = 2;
        }
    }
    // The opponent does not have to attack us at all. A spread that our slots
    // resist, a setup turn or a support turn is the pattern that punishes a
    // reflexive shield, so it carries real weight instead of being discarded
    // by an assume-the-worst comparison.
    if (bestMask != 0 && maskScores[0] != INT_MIN && count < PAIR_FORECASTS)
    {
        forecasts[count][0] = maskChoices[0][0];
        forecasts[count][1] = maskChoices[0][1];
        weights[count] = PAIR_FORECAST_PASSIVE;
        count++;
    }
    // The primary pattern always keeps a share of its own.
    u32 others = 0;
    for (u32 index = 1; index < count; index++)
        others += weights[index];
    if (others > 90)
        for (u32 index = 1; index < count; index++)
            weights[index] = weights[index] * 90 / others;
    weights[0] = 100;
    for (u32 index = 1; index < count; index++)
        weights[0] -= weights[index];
    // The pairs are ranked against the first pattern before the mixture
    // settles the shortlist, so the likeliest pattern goes first. A repeat
    // Wide Guard outweighing the damage pattern left the shortlist full of
    // Heat Wave pairs that the guard stops, and the single-target moves that
    // play around it were never settled.
    for (u32 index = 1; index < count; index++)
    {
        if (weights[index] <= weights[0])
            continue;
        struct PairAction swap[2] = {forecasts[0][0], forecasts[0][1]};
        u8 weight = weights[0];
        forecasts[0][0] = forecasts[index][0];
        forecasts[0][1] = forecasts[index][1];
        weights[0] = weights[index];
        forecasts[index][0] = swap[0];
        forecasts[index][1] = swap[1];
        weights[index] = weight;
    }
    return count;
}

// A partner turn the shield genuinely buys: an authored field or setup reward
// (Trick Room, Tailwind, weather, a stat boost), a Fake Out turn, or a
// knockout the partner is certain to land while the guard absorbs the reply.
// The trial says whether that turn happened: partnerPaid is false when the
// partner fell before it acted, or when its self-boost dies with it or can
// never be cashed (PairBoostOutrun). Juan's Altaria shielded beside a
// Manaphy whose Tail Glow Iron Hands' Wild Charge was about to end, and the
// guard was paid for the boost anyway.
static bool32 PairGuardPartnerPayoff(enum BattlerId user, const struct PairAction *actions, bool32 partnerPaid)
{
    enum BattlerId partner = GetPartnerBattler(user);
    if (!partnerPaid || !IsBattlerAlive(partner) || actions[partner].index == PAIR_IDLE)
        return FALSE;
    const struct PairAction *action = &actions[partner];
    if (GetMoveEffect(action->move) == EFFECT_PROTECT)
        return FALSE;
    if (action->planScore > 0 || action->move == MOVE_FAKE_OUT)
        return TRUE;
    if (IsBattleMoveStatus(action->executedMove))
        return FALSE;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        if (IsBattlerAlive(foe) && !IsBattlerAlly(user, foe)
         && gAiLogicData->simulatedDmg[partner][foe][action->index].minimum >= gBattleMons[foe].hp
         && gAiLogicData->moveAccuracy[partner][foe][action->index] >= 100)
            return TRUE;
    return FALSE;
}

// The share of the denied damage that waiting actually banks. Without a payoff
// the threat returns next turn, so most of it is only postponed.
// Half the maximum back is permanent; what a shield saves is only the part a
// payoff makes permanent. A guard that denies less than the user could simply
// have healed is the worse way to spend the same turn, so it banks nothing
// beyond the base: the choice is then settled by the HP the trial actually
// ends with, which is what watching a Naclstack guard twice and die showed.
static bool32 PairHealsMoreThanGuardDenies(enum BattlerId user, s32 denied)
{
    u32 missing = gBattleMons[user].maxHP - gBattleMons[user].hp;
    if (!missing || gBattleMons[user].volatiles.healBlockTimer)
        return FALSE;
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
    {
        enum Move move = gBattleMons[user].moves[index];
        enum BattleMoveEffects effect = GetMoveEffect(move);
        if (move == MOVE_NONE || IsMoveUnusable(index, move, gAiLogicData->moveLimitations[user]))
            continue;
        if (effect != EFFECT_RESTORE_HP
         && !(effect == EFFECT_ROOST && !IS_BATTLER_OF_TYPE(user, TYPE_FLYING)))
            continue;
        u32 heal = min(missing, max(1, gBattleMons[user].maxHP / 2));
        if ((s32)(heal * 180 / max(1, gBattleMons[user].maxHP)) > denied)
            return TRUE;
    }
    return FALSE;
}

static u32 PairGuardBankedShare(const struct PairEvaluation *ev, enum BattlerId user,
    const struct PairAction *actions, s32 denied, bool32 partnerPaid)
{
    if (PairHealsMoreThanGuardDenies(user, denied))
        return PAIR_GUARD_BANKED_BASE;
    u32 banked = PAIR_GUARD_BANKED_BASE
        + (ev->waitingPayoff ? PAIR_GUARD_BANKED_WAIT : 0)
        + (PairGuardPartnerPayoff(user, actions, partnerPaid) ? PAIR_GUARD_BANKED_PARTNER : 0)
        + ((gAiThinkingStruct->aiFlags[user] & AI_FLAG_CONSERVATIVE) ? PAIR_GUARD_BANKED_CONSERVATIVE : 0);
    return min(100, banked);
}

// Whether anything on the other side can still hit this body on the given
// side of the damage split. A Defense boost against a purely special board
// buys the turn nothing, exactly as an Attack boost does on a special set.
static bool32 PairFacesCategory(enum BattlerId battler, enum DamageCategory category)
{
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(battler, foe))
            continue;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = gBattleMons[foe].moves[index];
            if (move == MOVE_NONE || move == MOVE_UNAVAILABLE || IsBattleMoveStatus(move))
                continue;
            if (GetBattleMoveCategory(move) == category)
                return TRUE;
        }
    }
    return FALSE;
}

// Whether a burn on this body takes anything off its offence.
static bool32 PairAttacksPhysically(enum BattlerId battler)
{
    u32 physical = 0, special = 0;
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
    {
        enum Move move = gBattleMons[battler].moves[index];
        if (move == MOVE_NONE || IsBattleMoveStatus(move)
         || IsMoveUnusable(index, move, gAiLogicData->moveLimitations[battler]))
            continue;
        if (IsBattleMovePhysical(move))
            physical = max(physical, GetMovePower(move));
        else
            special = max(special, GetMovePower(move));
    }
    return physical != 0 && physical >= special;
}

// Two joint forecasts that are the same four actions score the same.
static bool32 PairSameForecast(const struct PairAction left[2], const struct PairAction right[2])
{
    for (u32 index = 0; index < 2; index++)
        if (left[index].index != right[index].index || left[index].move != right[index].move
         || left[index].target != right[index].target)
            return FALSE;
    return TRUE;
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

// A clock that one turn of waiting runs out. Unlike a per-slot payoff, this
// justifies the whole side spending the turn, both slots included.
static bool32 PairFieldClockExpiring(enum BattlerId actor)
{
    u32 foeSide = GetBattlerSide(actor) ^ 1;
    return ((gSideStatuses[foeSide] & SIDE_STATUS_TAILWIND) && gSideTimers[foeSide].tailwindTimer == 1)
        || ((gFieldStatuses & STATUS_FIELD_TRICK_ROOM) && ShouldClearFieldStatus(actor, STATUS_FIELD_TRICK_ROOM))
        || (gBattleWeather && gBattleStruct->weatherDuration == 1);
}

// A Sucker Punch is worth waiting out only where it would land something that
// matters: at least half of what one of our bodies has left. Waiting spends
// none of it, so a Sucker Punch that barely scratches is no reason to keep
// shielding: Wattson's Mega Manectric shielded seven turns running against a
// lone -1 Attack Shedinja whose Sucker Punch could not dent it.
static bool32 PairSuckerPunchThreat(enum BattlerId foe, enum BattlerId actor)
{
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        enum Move move = gBattleMons[foe].moves[slot];
        if (move == MOVE_NONE || GetMoveEffect(move) != EFFECT_SUCKER_PUNCH
         || IsMoveUnusable(slot, move, gAiLogicData->moveLimitations[foe]))
            continue;
        for (enum BattlerId ours = 0; ours < gBattlersCount; ours++)
            if (IsBattlerAlive(ours) && IsBattlerAlly(actor, ours)
             && gAiLogicData->simulatedDmg[foe][ours][slot].median * 2 >= gBattleMons[ours].hp)
                return TRUE;
    }
    return FALSE;
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
            // Public, flag-filtered knowledge only: an expiring resource on
            // the foe that one waiting turn removes. A pending command is not
            // knowledge, so an available Sucker Punch or one-PP attack counts
            // exactly like an available Fake Out: a turn worth waiting out.
            if (GetBattlerSecondaryDamage(battler)
             || ((gBattleMons[battler].status1 & STATUS1_BURN) && ability != ABILITY_MAGIC_GUARD)
             || gBattleMons[battler].volatiles.yawn
             || (ability == ABILITY_TRUANT && !gBattleMons[battler].volatiles.truantCounter)
             || (gBattleStruct->battlerState[battler].isFirstTurn && HasMove(battler, MOVE_FAKE_OUT))
             || PairSuckerPunchThreat(battler, actor)
             || (gBattleMons[battler].volatiles.semiInvulnerable
                 && gBattleMons[battler].volatiles.semiInvulnerable != STATE_COMMANDER))
                return TRUE;
            // A last-PP attack is spent by one more waiting turn.
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
            {
                enum Move move = gBattleMons[battler].moves[index];
                if (move != MOVE_NONE && gBattleMons[battler].pp[index] == 1
                 && !IsBattleMoveStatus(move)
                 && !IsMoveUnusable(index, move, gAiLogicData->moveLimitations[battler]))
                    return TRUE;
            }
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
    return PairFieldClockExpiring(actor);
}

// The work allowance for one decision. It is taken from the first board the
// AI looks at this turn and reused by every candidate board afterwards, so the
// comparison between staying and switching is made at one depth. It is a count
// of pairs, never a length of time: the same position must always produce the
// same answer.
bool8 gAiPairBudgetTruncated;
// Why the joint search did not run for this decision, if it did not.
u32 gAiPairSkipReason;
// Whether the clock cut this decision, as opposed to some earlier one.
bool8 gAiPairDecisionTruncated;
// Per battler: what the joint search decided about Mega Evolution, so a driven
// battle can say whether the search ran, whether the candidate was legal, and
// what it chose - none of which is visible from the outside otherwise.
u32 gAiPairMegaTrace[MAX_BATTLERS_COUNT];
// Per battler: how a voluntary switch came to be emitted, so a driven battle
// can tell the joint search's own reserve enumeration from the per-battler
// path, and say whether the decision clock had already run out when it did.
u32 gAiSwitchTrace[MAX_BATTLERS_COUNT];
// Per battler: whether the chosen action was a guard, and by how much the
// winning board beat the best scored pair in which that battler attacked
// instead. An empty guard that wins by a hair is a different story from one
// that wins by a mile, and a receipt cannot tell them apart. The margin now
// comes from every pair the final scoring reached, on every candidate board;
// reading it off the winning pair alone made a board whose best pair shielded
// report the 4095 sentinel even though attacking pairs had been scored and
// lost, which is a completely different defect. 4095 now means what it says:
// no pair in which this body attacked reached the final scoring at all.
u32 gAiGuardTrace[MAX_BATTLERS_COUNT];
// Per battler, per move slot: the largest simulated damage that move has
// against anything on the other side, as the decision saw it. Enough to tell a
// mispriced move from a correctly priced one that lost for another reason.
u32 gAiMoveDamageTrace[MAX_BATTLERS_COUNT][MAX_MON_MOVES];
static u32 sPairWorkAllowance;
static u32 sPairWorkDecision;

static u32 PairWorkAllowance(u32 pairs)
{
    // Fixed by the first board of the decision - the one the AI is standing
    // on - and reused unchanged by every candidate board compared against it.
    if (sPairWorkDecision != gAiLogicData->decisionStartFrame || sPairWorkAllowance == 0)
    {
        sPairWorkDecision = gAiLogicData->decisionStartFrame;
        sPairWorkAllowance = pairs < PAIR_WORK_MIN ? PAIR_WORK_MIN
            : pairs > PAIR_WORK_MAX ? PAIR_WORK_MAX : pairs;
    }
    return sPairWorkAllowance;
}

static s32 PairPlanScoreInner(enum BattlerId actor, const struct PairAction *action);

// Two copies of one effect in the same turn where only the first can land:
// the second meets the state the first has just set - the same "already in
// place" failure the shared check sees on the start-of-turn board, reached a
// few frames later. The standard AI prices this from the partner's chosen
// move; the pair chooses both at once and has to see it here instead.
static bool32 PairSecondCopyFails(enum BattleSide side, const struct PairAction *mine, const struct PairAction *theirs)
{
    if (mine->index == PAIR_IDLE || theirs->index == PAIR_IDLE)
        return FALSE;
    enum BattleMoveEffects effect = GetMoveEffect(mine->executedMove);
    enum BattleMoveEffects other = GetMoveEffect(theirs->executedMove);
    switch (effect)
    {
    case EFFECT_TRICK_ROOM:
    case EFFECT_MAGIC_ROOM:
    case EFFECT_WONDER_ROOM:
    case EFFECT_WEATHER:
    case EFFECT_TAILWIND:
    case EFFECT_FOLLOW_ME:
    case EFFECT_STEALTH_ROCK:
    case EFFECT_STICKY_WEB:
    case EFFECT_SAFEGUARD:
    case EFFECT_MIST:
    case EFFECT_LUCKY_CHANT:
    case EFFECT_REFLECT:
    case EFFECT_LIGHT_SCREEN:
    case EFFECT_AURORA_VEIL:
    case EFFECT_GRAVITY:
    case EFFECT_FAIRY_LOCK:
    case EFFECT_PERISH_SONG:
    case EFFECT_HAZE:
        return effect == other;
    case EFFECT_TERRAIN:
        return other == EFFECT_TERRAIN && GetMoveTerrainType(mine->executedMove) == GetMoveTerrainType(theirs->executedMove);
    case EFFECT_SPIKES:
        return other == effect && gSideTimers[side ^ BIT_SIDE].spikesAmount >= 2;
    case EFFECT_TOXIC_SPIKES:
        return other == effect && gSideTimers[side ^ BIT_SIDE].toxicSpikesAmount >= 1;
    // One non-volatile status per body: a second inducer, or a Yawn beside
    // one, meets a body that is already statused.
    case EFFECT_NON_VOLATILE_STATUS:
    case EFFECT_YAWN:
        return mine->target == theirs->target
            && (other == EFFECT_NON_VOLATILE_STATUS || other == EFFECT_YAWN);
    case EFFECT_LEECH_SEED:
    case EFFECT_ENCORE:
    case EFFECT_DISABLE:
    case EFFECT_TORMENT:
    case EFFECT_HEAL_BLOCK:
    case EFFECT_CONFUSE:
    case EFFECT_TAUNT:
    case EFFECT_ATTRACT:
    case EFFECT_EMBARGO:
    case EFFECT_NIGHTMARE:
        return effect == other && mine->target == theirs->target;
    default:
        return FALSE;
    }
}

// Helping Hand multiplies the partner's attack this turn. The pair picks both
// actions, so a partner spending the turn on a status move - a screen, a
// guard, a boost - or not acting at all leaves the boost nothing to multiply:
// Blake's Meowstic helped its partner's Reflect.
static bool32 PairHelpsNothing(const struct PairAction *mine, const struct PairAction *theirs)
{
    return GetMoveEffect(mine->executedMove) == EFFECT_HELPING_HAND
        && (theirs->index == PAIR_IDLE || IsBattleMoveStatus(theirs->executedMove));
}

// What a foe's own history with this body says about a Counter or Mirror
// Coat: the move it last landed on the body while both have stayed in. It is
// public and it is about this body, where the side's last moves may have been
// aimed at the partner. 1 when that hit is the category the move returns, -1
// when it is the other one, 0 when the foe has not hit the body with damage.
static s32 PairReflectEvidence(enum BattlerId actor, enum BattlerId foe, enum Move move)
{
    enum Move taken = gBattleStruct->lastTakenMoveFrom[actor][foe];
    if (taken == MOVE_NONE || taken == MOVE_UNAVAILABLE || IsBattleMoveStatus(taken))
        return 0;
    u32 category = IsBattleMovePhysical(taken) ? DAMAGE_CATEGORY_PHYSICAL : DAMAGE_CATEGORY_SPECIAL;
    return (GetMoveReflectDamage_DamageCategories(move) & (1u << category)) ? 1 : -1;
}

// Whether a status move takes its user out of a Choice lock on its own: it
// leaves the field, gives the item away or faints the user.
static bool32 PairStatusEndsChoiceLock(enum BattleMoveEffects effect)
{
    switch (effect)
    {
    case EFFECT_TRICK:
    case EFFECT_BATON_PASS:
    case EFFECT_TELEPORT:
    case EFFECT_PARTING_SHOT:
    case EFFECT_SHED_TAIL:
    case EFFECT_HEALING_WISH:
    case EFFECT_LUNAR_DANCE:
    case EFFECT_MEMENTO:
    case EFFECT_TRANSFORM:
        return TRUE;
    default:
        return FALSE;
    }
}

// A plain hit on a foe whose last action this turn is already behind it: the
// end of the turn is certain to finish it (AI_WillFaintFromResidual) and it
// visibly moves before this attack does. Nothing the hit does - damage, a
// secondary on the target - outlives the residual, so while another foe
// stands, this one is a spent turn. Drains, self effects and
// anything that is not a plain hit keep their own value and are not judged.
static bool32 PairHitsOnlyAfterLastAction(enum BattlerId actor, const struct PairAction *action)
{
    enum BattlerId target = action->target;
    enum Move move = action->executedMove;
    if (action->index == PAIR_IDLE || IsBattleMoveStatus(move)
     || AI_GetBattlerMoveTargetType(actor, move) != TARGET_SELECTED
     || GetMoveEffect(move) != EFFECT_HIT || target >= gBattlersCount || !IsBattlerAlive(target)
     || IsBattlerAlly(actor, target) || !AI_WillFaintFromResidual(target))
        return FALSE;
    for (u32 effectIndex = 0; effectIndex < GetMoveAdditionalEffectCount(move); effectIndex++)
    {
        const struct AdditionalEffect *additional = GetMoveAdditionalEffectById(move, effectIndex);
        if (additional->self || additional->moveEffect == MOVE_EFFECT_ABSORB)
            return FALSE;
    }
    if (AI_GetMovePriority(actor, gAiLogicData->abilities[actor], move) > 0)
        return FALSE;
    u32 mine = gAiLogicData->speedStats[actor], theirs = gAiLogicData->speedStats[target];
    if (gFieldStatuses & STATUS_FIELD_TRICK_ROOM ? mine <= theirs : mine >= theirs)
        return FALSE;
    // Another foe that stays is where this turn belongs, by this move or any
    // other the user holds.
    for (enum BattlerId other = 0; other < gBattlersCount; other++)
        if (other != target && IsBattlerAlive(other) && !IsBattlerAlly(actor, other)
         && !AI_WillFaintFromResidual(other))
            return TRUE;
    return FALSE;
}

// A charge move that has to spend this turn charging - Solar Beam outside the
// sun, a Sky Attack without a Power Herb - does nothing on the board this
// turn, and the trial rightly gives it no damage. It is still not a free
// option: it holds the user to a hit the foes watch coming, a Solar Beam
// fired from rain lands at half power, and the user can be removed before it
// fires. Beside a usable attack that hits now it was winning ties by noise:
// Maxie's Torkoal charged Solar Beam into Pelipper's rain with Heat Wave and
// Earth Power in hand. A charge turn that does something itself (Meteor Beam's
// boost) keeps its value.
static bool32 PairChargeTurnWasted(enum BattlerId actor, const struct PairAction *action)
{
    enum Move move = action->executedMove;
    if (action->index == PAIR_IDLE || gBattleMons[actor].volatiles.multipleTurns
     || !IsTwoTurnNotSemiInvulnerableMove(actor, move))
        return FALSE;
    for (u32 effectIndex = 0; effectIndex < GetMoveAdditionalEffectCount(move); effectIndex++)
        if (GetMoveAdditionalEffectById(move, effectIndex)->onChargeTurnOnly)
            return FALSE;
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        enum Move other = gBattleMons[actor].moves[slot];
        if (other != MOVE_NONE && other != move && !IsBattleMoveStatus(other)
         && !IsMoveUnusable(slot, other, gAiLogicData->moveLimitations[actor])
         && !IsTwoTurnNotSemiInvulnerableMove(actor, other)
         && gBattleMoveEffects[GetMoveEffect(other)].twoTurnEffect == FALSE)
            return TRUE;
    }
    return FALSE;
}

static s32 PairPlanScore(enum BattlerId actor, const struct PairAction *action)
{
    s32 score = PairPlanScoreInner(actor, action);
    // A Choice item locks its holder into the first move it uses, and a status
    // move is a lock with nothing in it: every later turn is that move again
    // or a switch. The switch search already prices leaving such a lock at
    // PAIR_USELESS_LOCK_ESCAPE; walking into it costs the same, or a Scarf
    // Imposter Ditto opens with Protect and repeats it until it faints.
    if (action->index != PAIR_IDLE && score > -10000
     && IsBattleMoveStatus(action->executedMove) && HasChoiceEffect(actor)
     && (gBattleStruct->choicedMove[actor] == MOVE_NONE || gBattleStruct->choicedMove[actor] == MOVE_UNAVAILABLE)
     && !PairStatusEndsChoiceLock(GetMoveEffect(action->executedMove)))
        score -= PAIR_USELESS_LOCK_ESCAPE;
    return score;
}

static s32 PairPlanScoreInner(enum BattlerId actor, const struct PairAction *action)
{
    if (action->index == PAIR_IDLE)
        return 0;
    enum Move move = action->move;
    enum BattleMoveEffects effect = GetMoveEffect(move);
    u32 plan = EmeraldChampions_GetBattlePlan(actor);
    // A move the engine is certain to refuse on the board we can see is a
    // failed move, whatever the isolated opinion or any plan reward says: the
    // same shared check the standard AI zeroes it with.
    if (AI_IsMoveCertainToFail(actor, action->target, action->executedMove)
     || AI_IsSpreadMoveWasted(actor, action->executedMove))
        return -10000;
    // The board a visible Mega leaves is the one this turn's moves most
    // likely meet. A move that fails there is priced as the failure it will
    // almost always be, short of the certain-failure veto.
    if (AI_IsMoveLikelyToFailAfterFoeMega(actor, action->target, action->executedMove))
        return -PAIR_MEGA_FORECAST_FAIL_COST;
    // A foe the end of the turn will finish is not worth a hit it takes after
    // its own last action, when another target stands: Norman's Weezing
    // Sludge Bombed a 10 HP poisoned Blaziken that had already moved.
    if (PairHitsOnlyAfterLastAction(actor, action))
        return -PAIR_DOOMED_TARGET_COST;
    if (PairChargeTurnWasted(actor, action))
        return -PAIR_SUPPORT_WASTED_COST;
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
        if (!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM))
        {
            // The order crossings it actually buys are the value, whether or
            // not the trainer carries the authored flag: a team that is outsped
            // gains exactly as much from speed control either way. The flag
            // still adds its own weight on top.
            u32 crossings = 0;
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                    if (IsBattlerAlive(ally) && IsBattlerAlly(actor, ally) && IsBattlerAlive(foe)
                     && !IsBattlerAlly(actor, foe) && gAiLogicData->speedStats[ally] <= gAiLogicData->speedStats[foe]
                     && gAiLogicData->speedStats[ally] * 2 > gAiLogicData->speedStats[foe])
                        crossings++;
            if (crossings)
                return min(60, crossings * 20) + ((plan & EC_BATTLE_PLAN_TAILWIND) ? 20 : 0);
        }
    }
    if (IsStatRaisingMove(move)
     // A neutral Drum score can reflect isolated incoming damage, before
     // the concrete partner's redirection or Fake Out is considered. The
     // pair trial already checks action-time HP, berry recovery and survival;
     // let that supported setup earn the bonus. Preserve negative viability
     // judgments and never reward an already-maximized stat.
     && (action->score >= AI_SCORE_DEFAULT
         || ((plan & EC_BATTLE_PLAN_SETUP) && effect == EFFECT_BELLY_DRUM))
     && AI_CanAnyStatChange(actor, actor, move))
        return (plan & EC_BATTLE_PLAN_SETUP) ? 35 : 0;
    // Last turn's move is legal knowledge on every difficulty, and these four
    // classes live or die on it.
    if (effect == EFFECT_SUCKER_PUNCH && IsBattlerAlive(action->target))
    {
        // A redirector that just used Follow Me is the likeliest thing on the
        // board to use a status move again, and Sucker Punch fails into one.
        enum Move last = gAiLogicData->lastUsedMove[action->target];
        if (last != MOVE_NONE && last != MOVE_UNAVAILABLE && IsBattleMoveStatus(last))
            return -25;
    }
    if (effect == EFFECT_REFLECT_DAMAGE)
    {
        // Counter and Mirror Coat reflect one damage category. If nothing the
        // opposing side has actually used belongs to it, this is a guess, not
        // a read.
        u32 categories = GetMoveReflectDamage_DamageCategories(move);
        bool32 seen = FALSE, matched = FALSE, able = FALSE, read = FALSE, contradicted = FALSE;
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
                continue;
            // The returned hit carries the move's own type: Counter cannot
            // touch a Ghost and Mirror Coat cannot touch a Dark body, so a
            // hit from one of those is nothing to reflect.
            if (gAiLogicData->effectiveness[actor][foe][action->index] == UQ_4_12(0.0))
                continue;
            s32 evidence = PairReflectEvidence(actor, foe, move);
            if (evidence > 0)
                read = TRUE;
            else if (evidence < 0)
                contradicted = TRUE;
            // The reflection moves last, so the hit it returns must also leave
            // the user standing: a Dragon Rage's fixed 40 into a 28 HP Feebas
            // ends it before Mirror Coat can answer.
            for (u32 slot = 0; slot < MAX_MON_MOVES && !able; slot++)
            {
                enum Move known = gBattleMons[foe].moves[slot];
                if (known != MOVE_NONE && known != MOVE_UNAVAILABLE && !IsBattleMoveStatus(known)
                 && (categories & (1u << (IsBattleMovePhysical(known) ? DAMAGE_CATEGORY_PHYSICAL : DAMAGE_CATEGORY_SPECIAL)))
                 && !IsMoveUnusable(slot, known, gAiLogicData->moveLimitations[foe])
                 && gAiLogicData->simulatedDmg[foe][actor][slot].maximum
                 && (gAiLogicData->simulatedDmg[foe][actor][slot].minimum < gBattleMons[actor].hp
                  || CanEndureHit(foe, actor, known)))
                    able = TRUE;
            }
            // Only a hit aimed at this user is a read on what it will take:
            // a special move thrown at the partner says nothing about the
            // Tackles that keep landing here.
            enum Move last = gAiLogicData->lastUsedMove[foe];
            if (last == MOVE_NONE || last == MOVE_UNAVAILABLE || IsBattleMoveStatus(last)
             || (!PairSpread(last) && gBattleStruct->battlerState[foe].lastMoveTarget != actor))
                continue;
            seen = TRUE;
            if (categories & (1u << (IsBattleMovePhysical(last) ? DAMAGE_CATEGORY_PHYSICAL : DAMAGE_CATEGORY_SPECIAL)))
                matched = TRUE;
        }
        // Nothing the reflection could reach is able to hit this user in the
        // right category at all: the engine is certain to refuse it.
        if (!able)
            return -10000;
        // The read is a foe that has hit this body in the returned category.
        // A side that has only hit specially is where Mirror Coat is a read
        // rather than a guess - but only when those hits landed here. Dragon
        // Rages aimed at the partner read nothing about this body: the
        // fisherman's Feebas took them as a read for eight failed Mirror Coats
        // while every hit it actually took was a Tackle.
        if (read)
            return 25;
        // A foe that has answered this body in the other category, or a side
        // whose every hit was the other category: a guess, not a read.
        if (contradicted || (seen && !matched))
            return -40;
    }
    if (!IsBattleMoveStatus(action->executedMove) && IsBattlerAlive(action->target)
     && !IsBattlerAlly(actor, action->target)
     && AI_GetMovePriority(actor, gAiLogicData->abilities[actor], move) > 0
     // Only a move built for priority trades power for it. Gale Wings lends a
     // full Brave Bird the step for free, and charging that as redundant sent
     // Jeff's faster Talonflame's resisted Flare Blitz into Glimmora over a
     // neutral Brave Bird into Incineroar.
     && GetMovePriority(action->executedMove) > 0
     // Fake Out's worth is the flinch it buys, not the step in the order, so
     // it is not redundant on a user that is already faster.
     && action->executedMove != MOVE_FAKE_OUT && action->executedMove != MOVE_FIRST_IMPRESSION)
    {
        // Against ordinary-priority attacks, an already faster user gains
        // no order advantage from priority. In that case the comparison
        // belongs to the damage - which is
        // how a Huge Power body took the small priority attack twice over the
        // move that would have ended the same target.
        bool32 slowerTarget = gFieldStatuses & STATUS_FIELD_TRICK_ROOM
            ? gAiLogicData->speedStats[actor] < gAiLogicData->speedStats[action->target]
            : gAiLogicData->speedStats[actor] > gAiLogicData->speedStats[action->target];
        if (slowerTarget)
        {
            // Speed alone does not make priority redundant: it can outrun
            // the target's own priority. Use available moves, never its
            // selected command. Keep the existing penalty otherwise.
            s32 priority = AI_GetMovePriority(actor, gAiLogicData->abilities[actor], move);
            bool32 gainsOrder = FALSE;
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                enum Move response = gBattleMons[action->target].moves[slot];
                if (!HasMove(action->target, response)
                 || IsMoveUnusable(slot, response, gAiLogicData->moveLimitations[action->target])
                 || IsBattleMoveStatus(response)
                 || !gAiLogicData->simulatedDmg[action->target][actor][slot].maximum)
                    continue;
                s32 responsePriority = AI_GetMovePriority(action->target,
                    gAiLogicData->abilities[action->target], response);
                if (responsePriority > 0 && responsePriority <= priority)
                {
                    gainsOrder = TRUE;
                    break;
                }
            }
            if (!gainsOrder)
                return -45;
        }
    }
    if (move == MOVE_WIDE_GUARD || move == MOVE_QUICK_GUARD)
    {
        // A spread move seen from a living foe last turn is the reason these
        // exist, and a guard whose denial has not happened yet cannot earn it
        // from the trial alone.
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            enum Move last = gAiLogicData->lastUsedMove[foe];
            if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe)
             || last == MOVE_NONE || last == MOVE_UNAVAILABLE)
                continue;
            if (move == MOVE_WIDE_GUARD ? PairSpread(last)
                : AI_GetMovePriority(foe, gAiLogicData->abilities[foe], last) > 0)
                return 30;
        }
    }
    if (move == MOVE_FEINT)
    {
        // Feint's point is breaking a shield. Nothing on the field has guarded
        // and nothing is known to be about to, so it is a weak attack.
        bool32 guarding = FALSE;
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            enum Move last = gAiLogicData->lastUsedMove[foe];
            if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
                continue;
            if (gProtectStructs[foe].protected != PROTECT_NONE
             || (last != MOVE_NONE && last != MOVE_UNAVAILABLE && GetMoveEffect(last) == EFFECT_PROTECT))
                guarding = TRUE;
        }
        if (!guarding)
            return -20;
    }
    if (effect == EFFECT_ENCORE && IsBattlerAlive(action->target)
     && !IsBattlerAlly(actor, action->target))
    {
        // Locking a foe into the move it just used is worth a turn, and worth
        // more when that move is one it cannot hurt anybody with.
        enum Move last = gAiLogicData->lastUsedMove[action->target];
        // Prankster's priority is what makes a Dark body immune to the move,
        // so the lock cannot happen at all and the turn buys nothing. The
        // trial already refuses to model it; the plan score has to agree, or
        // it pays for an interaction that never occurs.
        bool32 pranksterBlocked = GetConfig(B_PRANKSTER_DARK_TYPES) >= GEN_7
            && gAiLogicData->abilities[actor] == ABILITY_PRANKSTER
            && IS_BATTLER_OF_TYPE(action->target, TYPE_DARK);
        // Native Encore fails on a move that cannot be encored or that has
        // run out of PP, so neither is a lock worth paying for.
        u32 lastSlot = last != MOVE_NONE && last != MOVE_UNAVAILABLE ? GetMoveIndex(action->target, last) : MAX_MON_MOVES;
        if (lastSlot < MAX_MON_MOVES && !pranksterBlocked && !IsMoveEncoreBanned(last)
         && gBattleMons[action->target].pp[lastSlot] != 0
         && !gBattleMons[action->target].volatiles.encoreTimer)
        {
            s32 value = IsBattleMoveStatus(last) ? 35 : 20;
            // A Choice-locked repeater is already committed to that move, so
            // the lock costs it nothing it had - but it keeps it there after
            // the item would have let it switch out of the lock.
            if (IsHoldEffectChoice(gAiLogicData->holdEffects[action->target])
             && IsBattlerItemEnabled(action->target))
                value += 15;
            return value;
        }
    }
    if ((effect == EFFECT_TRICK || effect == EFFECT_BESTOW) && IsBattlerAlive(action->target)
     && !IsBattlerAlly(actor, action->target)
     && gAiLogicData->abilities[action->target] != ABILITY_STICKY_HOLD
     && !gBattleMons[action->target].volatiles.substitute)
    {
        // A swap is worth the difference between the two items, not a flat
        // trade. Handing over something the user cannot use anyway - a Choice
        // item on a status body, anything at all under Klutz - while taking a
        // working one is the whole reason the move is on the set.
        // Read the item itself, not the suppressed hold effect: a Klutz body's
        // Flame Orb does nothing in its hands and everything in theirs.
        enum HoldEffect mine = GetItemHoldEffect(gAiLogicData->items[actor]);
        enum HoldEffect theirs = gAiLogicData->holdEffects[action->target];
        bool32 junkToThem = gAiLogicData->holdEffects[actor] == HOLD_EFFECT_NONE
                         || (IsHoldEffectChoice(mine) && !IsBattlerItemEnabled(actor));
        bool32 theirsWorthTaking = theirs != HOLD_EFFECT_NONE;
        // Sticking a Choice item on a foe locks it into whatever it picks -
        // a real cost to it even when the item itself is nothing to us.
        if (IsHoldEffectChoice(mine) && IsBattlerItemEnabled(action->target))
            return 30;
        if (junkToThem && theirsWorthTaking)
            return 45;
        if (theirsWorthTaking && mine != theirs)
            return 15;
        // Handing a working item to an empty hand is a pure gift. The gifts
        // nobody wants - orbs, a Barb, an Iron Ball - are the exception, and
        // they are already priced by the per-battler scorer, which knows the
        // recipient's ability and immunities. Leave that judgment where it is.
        if (!theirsWorthTaking && !junkToThem
         && mine != HOLD_EFFECT_FLAME_ORB && mine != HOLD_EFFECT_TOXIC_ORB
         && mine != HOLD_EFFECT_STICKY_BARB && mine != HOLD_EFFECT_IRON_BALL)
            return -25;
        return 0;
    }
    if (move == MOVE_DESTINY_BOND && !gBattleMons[actor].volatiles.destinyBond)
    {
        // Trading a body that is already lost for the thing that is killing it
        // is a fair trade, but only when the death is certain and lands first.
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
                continue;
            if (!AI_IsSlower(actor, foe, MOVE_DESTINY_BOND, MOVE_NONE, DONT_CONSIDER_PRIORITY))
                continue;
            if (CanTargetFaintAi(foe, actor))
                return 55;
        }
        return -20;
    }
    // The mirror image: a move whose whole effect is to take choices away from
    // whoever it lands on must never land on our own side. Encore aimed at a
    // partner locks it into whatever it just did for three turns and costs the
    // foes nothing; the trial sees no cost because the lock only bites later.
    if (action->target != actor && IsBattlerAlive(action->target) && IsBattlerAlly(actor, action->target))
    {
        switch (effect)
        {
        case EFFECT_ENCORE:
        case EFFECT_TAUNT:
        case EFFECT_DISABLE:
        case EFFECT_TORMENT:
        case EFFECT_HEAL_BLOCK:
            return -10000;
        default:
            break;
        }
    }
    // A move whose whole effect is to help whoever it lands on must never
    // land on the other side. Heal Pulse aimed at a foe heals the foe; the
    // engine allows it and no board makes it worth a turn. Pollen Puff is not
    // in this list because it damages a foe and only heals an ally.
    if (IsBattlerAlive(action->target) && !IsBattlerAlly(actor, action->target))
    {
        switch (effect)
        {
        case EFFECT_HEAL_PULSE:
        case EFFECT_HELPING_HAND:
        case EFFECT_AFTER_YOU:
        case EFFECT_INSTRUCT:
            return -10000;
        default:
            break;
        }
        if (move == MOVE_COACHING || move == MOVE_DECORATE || move == MOVE_AROMATIC_MIST
         || move == MOVE_FLORAL_HEALING)
            return -10000;
    }
    // A partner that absorbs the move's type takes it instead of the foe, so
    // the attack is not an attack at all - it is a gift of a Special Attack
    // stage to our own side and a wasted turn. Spread moves are exempt: they
    // still reach the foes.
    if (!IsBattleMoveStatus(move) && !PairSpread(move)
     && IsBattlerAlive(action->target) && !IsBattlerAlly(actor, action->target)
     && !IsMoveRedirectionPrevented(actor, move, gAiLogicData->abilities[actor]))
    {
        enum Ability absorbs = GetMoveType(move) == TYPE_WATER ? ABILITY_STORM_DRAIN
            : GetMoveType(move) == TYPE_ELECTRIC ? ABILITY_LIGHTNING_ROD : ABILITY_NONE;
        if (absorbs != ABILITY_NONE && B_REDIRECT_ABILITY_ALLIES >= GEN_4)
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                if (ally != actor && IsBattlerAlive(ally) && IsBattlerAlly(actor, ally)
                 && gAiLogicData->abilities[ally] == absorbs)
                    return -10000;
    }
    if (effect == EFFECT_TAUNT)
    {
        // Taunt spends the whole turn to take a move away. If the body it is
        // aimed at holds no status move - or has already been taunted - there
        // is nothing to take from it, whatever its partner holds: Wattson's
        // Electrode taunted a Marowak whose only status move was Protect
        // while the Trick Room setter stood beside it.
        bool32 worthTaking = FALSE;
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe)
             || gBattleMons[foe].volatiles.tauntTimer
             || (IsBattlerAlive(action->target) && !IsBattlerAlly(actor, action->target) && foe != action->target))
                continue;
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
            {
                enum Move known = gBattleMons[foe].moves[index];
                // A shield is not what Taunt is for. Denying a guard the body
                // may not even want costs the whole turn, and the receipts
                // that produced this rule were bodies holding Protect alone.
                if (known != MOVE_NONE && known != MOVE_UNAVAILABLE && IsBattleMoveStatus(known)
                 && GetMoveEffect(known) != EFFECT_PROTECT)
                    worthTaking = TRUE;
            }
        }
        if (!worthTaking)
            return -80;
    }
    if (effect == EFFECT_LEECH_SEED && IsBattlerAlive(action->target)
     && !IsBattlerAlly(actor, action->target)
     && !gBattleMons[action->target].volatiles.leechSeed
     && !IS_BATTLER_OF_TYPE(action->target, TYPE_GRASS))
        return 20;
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

// The authored trigger must actually reach the authored recipient this turn:
// a single-target activation aimed elsewhere is not the interaction.
static s32 PairTacticScore(enum BattlerId actor, const struct PairAction *action)
{
    enum BattlerId partner = GetPartnerBattler(actor);
    if (action->index == PAIR_IDLE || !IsBattlerAlive(actor) || !IsBattlerAlive(partner))
        return 0;
    if (!(EmeraldChampions_GetTacticKind(actor, partner, action->move)
          & (EC_BATTLE_TACTIC_ACTIVATE | EC_BATTLE_TACTIC_AFTER_YOU | EC_BATTLE_TACTIC_INSTRUCT)))
        return 0;
    // A trigger that can be aimed at a single foe has to be aimed at the
    // authored recipient. A side or field trigger - Darius's Tailwind into
    // Wind Power - reaches its own side by construction.
    switch (GetMoveTarget(action->executedMove))
    {
    case TARGET_USER:
    case TARGET_USER_AND_ALLY:
    case TARGET_USER_OR_ALLY:
    case TARGET_ALLY:
    case TARGET_FIELD:
    case TARGET_ALL_BATTLERS:
    case TARGET_BOTH:
    case TARGET_FOES_AND_ALLY:
        break;
    default:
        if (action->target != partner)
            return 0;
        break;
    }
    // The authored rule is explicit: friendly fire that kills the recipient is
    // not the interaction. A super-effective trigger is a cost however the
    // roll lands - Beat Up is Dark, which feeds a Steel recipient's Justified
    // and kills a Ghost one - and the worst roll must leave it standing.
    if (!IsBattleMoveStatus(action->executedMove))
    {
        u32 cost = gAiLogicData->simulatedDmg[actor][partner][action->index].maximum;
        // The cached figure is one strike; the authored count is what lands.
        if (GetMoveEffect(action->executedMove) == EFFECT_BEAT_UP)
            cost *= AI_GetBeatUpHitCount(actor);
        else if (GetMoveStrikeCount(action->executedMove) > 1)
            cost *= GetMoveStrikeCount(action->executedMove);
        // A Weakness Policy recipient is armed by a super-effective hit and
        // nothing else, so that is the one case where effectiveness above
        // neutral is the point rather than a cost. Lethality still decides.
        if ((gAiLogicData->effectiveness[actor][partner][action->index] > UQ_4_12(1.0)
             && gAiLogicData->holdEffects[partner] != HOLD_EFFECT_WEAKNESS_POLICY)
         || cost >= gBattleMons[partner].hp)
            return 0;
    }
    return PAIR_TACTIC_REWARD;
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

static bool32 PairCanSoak(enum BattlerId actor, enum BattlerId target)
{
    if (actor == target || !IsBattlerAlive(target)
     || DoesSubstituteBlockMove(actor, target, MOVE_SOAK))
        return FALSE;
    enum Type types[3];
    GetBattlerTypes(target, types);
    if (types[0] == TYPE_WATER && types[1] == TYPE_WATER)
        return FALSE;
    enum Ability ability = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
        gAiLogicData->abilities[target], gAiLogicData->holdEffects[target], MOVE_SOAK);
    if (ability == ABILITY_MULTITYPE || ability == ABILITY_RKS_SYSTEM
     || ability == ABILITY_GOOD_AS_GOLD || ability == ABILITY_MAGIC_BOUNCE
     || ability == ABILITY_WATER_ABSORB || ability == ABILITY_DRY_SKIN
     || (ability == ABILITY_STORM_DRAIN && GetConfig(B_REDIRECT_ABILITY_IMMUNITY) >= GEN_5)
     || gProtectStructs[target].bounceMove
     || (GetConfig(B_PRANKSTER_DARK_TYPES) >= GEN_7 && gAiLogicData->abilities[actor] == ABILITY_PRANKSTER
         && IS_BATTLER_OF_TYPE(target, TYPE_DARK)))
        return FALSE;
    if (IsSemiInvulnerable(target, CHECK_ALL)
     && !BreaksThroughSemiInvulnerableState(actor, target, gAiLogicData->abilities[actor], ability,
         MOVE_SOAK, gBattleMons[target].volatiles.semiInvulnerable))
        return FALSE;
    if (AI_GetMovePriority(actor, gAiLogicData->abilities[actor], MOVE_SOAK) > 0 && !IsBattlerAlly(actor, target))
    {
        if (IsPsychicTerrainAffected(target, ability, gAiLogicData->holdEffects[target], gFieldTimers.terrain))
            return FALSE;
        for (enum BattlerId other = 0; other < gBattlersCount; other++)
            if (IsBattlerAlive(other) && IsBattlerAlly(target, other) && IsDazzlingAbility(gAiLogicData->abilities[other]))
                return FALSE;
    }
    return TRUE;
}

static void CachePairSoakChargeDamage(struct PairEvaluation *ev, enum BattlerId actor, u32 index, enum Move move)
{
    if (!IsBattlerAlive(actor) || IsBattleMoveStatus(move)
     || IsMoveUnusable(index, gBattleMons[actor].moves[index], gAiLogicData->moveLimitations[actor]))
        return;
    if (ev->chargeable & (1u << actor))
    {
        struct BattlePokemon saved = gBattleMons[actor];
        for (u32 soaked = 0; soaked < 2; soaked++)
        {
            if (soaked)
                SET_BATTLER_TYPE(actor, TYPE_WATER);
            if (GetDynamicMoveType(GetBattlerMon(actor), move, actor, gAiLogicData->abilities[actor],
                    gAiLogicData->holdEffects[actor], MON_IN_BATTLE) == TYPE_ELECTRIC)
                ev->electricMoves[actor][soaked] |= 1u << index;
        }
        gBattleMons[actor] = saved;
    }
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (actor == target || !IsBattlerAlive(target))
            continue;
        u32 possible = (ev->soakable & (1u << actor) ? PAIR_SOAK_ATTACKER : 0)
            | (ev->soakable & (1u << target) ? PAIR_SOAK_TARGET : 0)
            | (ev->chargeable & (1u << actor) ? PAIR_CHARGED : 0);
        if (!possible)
            continue;
        for (u32 state = 0; state < PAIR_SOAK_CHARGE_STATES; state++)
        {
            if (state & ~possible)
                continue;
            struct AiCalcValues calc = {.move = move, .weather = ev->weather, .terrain = gFieldTimers.terrain};
            ev->soakChargeDamage[actor][index][target][state] = AI_CalcSoakChargeDamage(&calc, actor, target,
                state & PAIR_SOAK_ATTACKER, state & PAIR_SOAK_TARGET, state & PAIR_CHARGED);
            ev->soakChargeStates[actor][index][target] |= 1u << state;
        }
    }
}

static void CachePairRageDamage(struct PairEvaluation *ev, enum BattlerId actor, u32 index, enum Move move)
{
    if (GetMoveEffect(move) != EFFECT_RAGE_FIST || !IsBattlerAlive(actor)
     || IsMoveUnusable(index, gBattleMons[actor].moves[index], gAiLogicData->moveLimitations[actor]))
        return;
    u32 current = min(6, GetBattlerPartyState(actor)->timesGotHit);
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (actor == target || !IsBattlerAlive(target))
            continue;
        u32 possible = (ev->soakable & (1u << actor) ? PAIR_SOAK_ATTACKER : 0)
            | (ev->soakable & (1u << target) ? PAIR_SOAK_TARGET : 0);
        for (u32 state = 0; state < 4; state++)
        {
            if (state & ~possible)
                continue;
            for (u32 hits = current; hits <= 6; hits++)
            {
                struct AiCalcValues calc = {.move = move, .weather = ev->weather, .terrain = gFieldTimers.terrain};
                ev->rage[actor]->damage[target][state][hits] = AI_CalcRageFistDamage(&calc, actor, target, hits,
                    state & PAIR_SOAK_ATTACKER, state & PAIR_SOAK_TARGET);
            }
            ev->rage[actor]->states[target] |= 1u << state;
        }
    }
}

// Native itemless anchors are needed only for defensive items that can be
// consumed or removed on this board. Ordinary follow-up attacks share the same
// item event state as copied attacks; no native query runs inside pair trials.
static void CachePairDefenderItems(struct PairEvaluation *ev, u32 noActionMask)
{
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
        {
            struct PairDefenderItemCache *cache = ev->defenderItem[actor][target];
            enum HoldEffect held = gAiLogicData->holdEffects[target];
            bool32 removableDefense = (ev->itemRemovers & ~(1u << target))
                && (held == HOLD_EFFECT_EVIOLITE || held == HOLD_EFFECT_ASSAULT_VEST
                    || held == HOLD_EFFECT_DEEP_SEA_SCALE || held == HOLD_EFFECT_METAL_POWDER);
            // A full-HP Focus Sash spent by one hit leaves the next hit of the
            // same turn to land in full.
            bool32 sash = held == HOLD_EFFECT_FOCUS_SASH && gBattleMons[target].hp == gBattleMons[target].maxHP;
            if (actor == target || !IsBattlerAlive(actor) || !IsBattlerAlive(target)
             || (noActionMask & (1u << actor)) || (held != HOLD_EFFECT_RESIST_BERRY && !removableDefense && !sash))
            {
                if (cache != NULL)
                    Free(cache);
                ev->defenderItem[actor][target] = NULL;
                continue;
            }
            u32 mask = (ev->soakable & (1u << actor) ? PAIR_SOAK_ATTACKER : 0)
                | (ev->soakable & (1u << target) ? PAIR_SOAK_TARGET : 0)
                | (ev->chargeable & (1u << actor) ? PAIR_CHARGED : 0);
            u32 count = 1;
            for (u32 bit = 1; bit <= PAIR_CHARGED; bit <<= 1)
                if (mask & bit)
                    count *= 2;
            if (cache == NULL || cache->mask != mask)
            {
                if (cache != NULL)
                    Free(cache);
                cache = AllocZeroed(sizeof(*cache) + MAX_MON_MOVES * count * sizeof(cache->damage[0]));
                ev->defenderItem[actor][target] = cache;
            }
            else
                memset(cache, 0, sizeof(*cache) + MAX_MON_MOVES * count * sizeof(cache->damage[0]));
            if (cache == NULL)
                continue;
            cache->mask = mask;
            cache->count = count;
            u32 offset = 0;
            for (u32 state = 0; state < PAIR_SOAK_CHARGE_STATES; state++)
                if (!(state & ~mask))
                    cache->stateIndex[state] = offset++;
            enum Item actualItem = gBattleMons[target].item;
            enum Item knownItem = gAiLogicData->items[target];
            gBattleMons[target].item = ITEM_NONE;
            gAiLogicData->items[target] = ITEM_NONE;
            gAiLogicData->holdEffects[target] = HOLD_EFFECT_NONE;
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
            {
                enum Move move = gBattleMons[actor].moves[index];
                if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[actor]))
                    continue;
                if (GetMoveEffect(move) == EFFECT_NATURE_POWER)
                    move = GetNaturePowerMove();
                if (IsBattleMoveStatus(move))
                    continue;
                cache->moves |= 1u << index;
                for (u32 state = 0; state < PAIR_SOAK_CHARGE_STATES; state++)
                    if (!(state & ~mask))
                    {
                        struct AiCalcValues calc = {.move = move, .weather = ev->weather, .terrain = gFieldTimers.terrain};
                        cache->damage[index * count + cache->stateIndex[state]] = AI_CalcSoakChargeDamage(&calc,
                            actor, target, state & PAIR_SOAK_ATTACKER, state & PAIR_SOAK_TARGET, state & PAIR_CHARGED);
                    }
            }
            gBattleMons[target].item = actualItem;
            gAiLogicData->items[target] = knownItem;
            gAiLogicData->holdEffects[target] = held;
        }
}

static u32 PairTargetDropStat(enum Move move);

static s8 PairCacheTargetDrop(enum BattlerId actor, enum BattlerId target, enum Move move)
{
    u32 targetDropStat = PairTargetDropStat(move);
    if (!targetDropStat)
        return 0;
    struct BattleCalcValues cv = {.battlerAtk = actor, .battlerDef = target, .move = move, .moveEffect = GetMoveEffect(move)};
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        cv.holdEffects[battler] = gAiLogicData->holdEffects[battler];
        cv.abilities[battler] = AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor],
            gAiLogicData->abilities[battler], cv.holdEffects[battler], move);
    }
    if ((IsSoundMove(move) && cv.abilities[target] == ABILITY_SOUNDPROOF)
     || DoesSubstituteBlockMove(actor, target, move)
     || (!IsBattleMoveStatus(move)
         && (IsSheerForceAffected(move, cv.abilities[actor])
             || IsAdditionalEffectBlocked(actor, cv.abilities[actor], target, cv.abilities[target], move))))
        return 0;
    if (IsBattleMoveStatus(move))
    {
        // Primary drops are not blocked by Covert Cloak or
        // Shield Dust, but reflected/status-immune targets get no
        // fictitious defensive credit in this bounded forecast.
        if (cv.abilities[target] == ABILITY_GOOD_AS_GOLD
         || cv.abilities[target] == ABILITY_MAGIC_BOUNCE || gProtectStructs[target].bounceMove
         || (GetConfig(B_PRANKSTER_DARK_TYPES) >= GEN_7
             && cv.abilities[actor] == ABILITY_PRANKSTER && IS_BATTLER_OF_TYPE(target, TYPE_DARK))
         || (IsSemiInvulnerable(target, CHECK_ALL)
             && !BreaksThroughSemiInvulnerableState(actor, target, cv.abilities[actor], cv.abilities[target],
                 move, gBattleMons[target].volatiles.semiInvulnerable)))
            return 0;
        if (AI_GetMovePriority(actor, cv.abilities[actor], move) > 0)
        {
            bool32 blocked = IsPsychicTerrainAffected(target, cv.abilities[target], cv.holdEffects[target], gFieldTimers.terrain);
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                if (IsBattlerAlive(ally) && IsBattlerAlly(target, ally) && IsDazzlingAbility(cv.abilities[ally]))
                    blocked = TRUE;
            if (blocked)
                return 0;
        }
    }
    const struct AdditionalEffect *additional = GetMoveAdditionalEffectById(move, 0);
    if (!MoveEffectIsGuaranteed(actor, cv.abilities[actor], move, additional))
        return 0;
    struct StatChange st = {.onlyChecking = TRUE, .stat = targetDropStat};
    st.stage = GetAdjustedStatStage(-GetStatStage(targetDropStat, additional), cv.abilities[target], FALSE);
    if (CanStatChange(&cv, &st))
        // Keep eligibility per move: sound Snarl bypasses Substitute,
        // while Struggle Bug does not, even on the same moveset.
        return st.stage;
    return 0;
}

static void CachePairCopiedDancers(struct PairEvaluation *ev, u32 noActionMask)
{
    u8 sources[MAX_BATTLERS_COUNT] = {0};
    for (enum BattlerId source = 0; source < gBattlersCount; source++)
        if (IsBattlerAlive(source) && !(noActionMask & (1u << source)))
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                enum Move move = gBattleMons[source].moves[slot];
                u32 dance = PairCopiedDance(move);
                if (dance < ARRAY_COUNT(sCopiedDances) && !IsMoveUnusable(slot, move, gAiLogicData->moveLimitations[source]))
                    sources[source] |= 1u << dance;
            }
    for (enum BattlerId dancer = 0; dancer < gBattlersCount; dancer++)
    {
        u32 needed = 0;
        if (IsBattlerAlive(dancer) && gAiLogicData->abilities[dancer] == ABILITY_DANCER)
            for (enum BattlerId source = 0; source < gBattlersCount; source++)
                if (source != dancer)
                    needed |= sources[source];
        for (u32 dance = 0; dance < ARRAY_COUNT(sCopiedDances); dance++)
        {
            struct PairDancerMoveCache *cache = ev->dancer[dancer][dance];
            if (!(needed & (1u << dance)))
            {
                if (cache != NULL)
                    Free(cache);
                ev->dancer[dancer][dance] = NULL;
                continue;
            }
            bool32 targetItem = FALSE;
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (target != dancer && IsBattlerAlive(target) && gBattleMons[target].item != ITEM_NONE)
                    targetItem = TRUE;
            u32 mask = (ev->soakable & (1u << dancer) ? PAIR_SOAK_ATTACKER : 0)
                | (ev->soakable & ~(1u << dancer) ? PAIR_SOAK_TARGET : 0)
                | (ev->chargeable & (1u << dancer) ? PAIR_CHARGED : 0)
                | (gBattleMons[dancer].item != ITEM_NONE ? PAIR_DANCE_NO_ITEM : 0)
                | (targetItem ? PAIR_DANCE_TARGET_NO_ITEM : 0);
            if (IsBattleMoveStatus(sCopiedDances[dance]))
                mask = 0;
            u32 count = 1;
            for (u32 bit = 1; bit <= PAIR_DANCE_TARGET_NO_ITEM; bit <<= 1)
                if (mask & bit)
                    count *= 2;
            if (cache == NULL || cache->mask != mask)
            {
                if (cache != NULL)
                    Free(cache);
                cache = AllocZeroed(sizeof(*cache) + MAX_BATTLERS_COUNT * count * sizeof(cache->damage[0]));
                ev->dancer[dancer][dance] = cache;
            }
            else
                memset(cache, 0, sizeof(*cache) + MAX_BATTLERS_COUNT * count * sizeof(cache->damage[0]));
            if (cache == NULL)
                continue; // Optional forecast must not crash a crowded hypothetical board.
            cache->mask = mask;
            cache->count = count;
            u32 offset = 0;
            for (u32 state = 0; state < PAIR_DANCE_STATES; state++)
                if (!(state & ~mask))
                    cache->stateIndex[state] = offset++;
            enum Move move = sCopiedDances[dance];
            struct BattlePokemon saved = gBattleMons[dancer];
            enum Item savedKnown = gAiLogicData->items[dancer];
            enum HoldEffect savedHeld = gAiLogicData->holdEffects[dancer];
            enum BattlerId savedItemBattler = gPotentialItemEffectBattler;
            for (u32 soaked = 0; soaked < 2; soaked++)
            {
                if (soaked)
                    SET_BATTLER_TYPE(dancer, TYPE_WATER);
                cache->type[soaked] = GetDynamicMoveType(GetBattlerMon(dancer), move, dancer,
                    gAiLogicData->abilities[dancer], savedHeld, MON_IN_BATTLE);
            }
            gBattleMons[dancer] = saved;
            for (u32 removed = 0; removed < 4; removed++)
            {
                gBattleMons[dancer].item = removed & 1 ? ITEM_NONE : saved.item;
                gAiLogicData->items[dancer] = removed & 1 ? ITEM_NONE : savedKnown;
                gAiLogicData->holdEffects[dancer] = removed & 1 ? HOLD_EFFECT_NONE : savedHeld;
                for (enum BattlerId target = 0; target < gBattlersCount; target++)
                    if (target != dancer && IsBattlerAlive(target))
                    {
                        enum Item targetItem = gBattleMons[target].item;
                        enum Item targetKnown = gAiLogicData->items[target];
                        enum HoldEffect targetHeld = gAiLogicData->holdEffects[target];
                        if (removed & 2)
                        {
                            gBattleMons[target].item = ITEM_NONE;
                            gAiLogicData->items[target] = ITEM_NONE;
                            gAiLogicData->holdEffects[target] = HOLD_EFFECT_NONE;
                        }
                        cache->targetDropDelta[!!(removed & 2)][target] = PairCacheTargetDrop(dancer, target, move);
                        cache->accuracy[removed][target] = AI_GetMoveAccuracy(gAiLogicData, dancer, target, move);
                        cache->contact[removed][target] = AI_GetContactDamage(dancer, target, move,
                            gAiLogicData->abilities[dancer], gAiLogicData->holdEffects[dancer],
                            gAiLogicData->holdEffects[target], gAiLogicData->abilities[target]);
                        if (AI_CanScreenReduceDamage(dancer, target, move))
                            cache->screened[removed & 1] |= 1u << target;
                        gBattleMons[target].item = targetItem;
                        gAiLogicData->items[target] = targetKnown;
                        gAiLogicData->holdEffects[target] = targetHeld;
                    }
            }
            gBattleMons[dancer] = saved;
            gAiLogicData->items[dancer] = savedKnown;
            gAiLogicData->holdEffects[dancer] = savedHeld;
            gPotentialItemEffectBattler = savedItemBattler;
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (target != dancer && IsBattlerAlive(target))
                    for (u32 state = 0; state < PAIR_DANCE_STATES; state++)
                    {
                        if (state & ~mask)
                            continue;
                        struct AiCalcValues calc = {.move = move, .weather = ev->weather, .terrain = gFieldTimers.terrain};
                        struct PairDancerDamage *entry = &cache->damage[target * count + cache->stateIndex[state]];
                        entry->damage = AI_CalcDancerDamage(&calc, dancer, target,
                            state & PAIR_SOAK_ATTACKER, state & PAIR_SOAK_TARGET, state & PAIR_CHARGED,
                            state & PAIR_DANCE_NO_ITEM, state & PAIR_DANCE_TARGET_NO_ITEM);
                        entry->effectiveness = calc.typeEffectiveness;
                    }
        }
    }
}

static const struct PairDancerDamage *PairCopiedDamage(const struct PairDancerMoveCache *cache,
    enum BattlerId actor, enum BattlerId target, u32 soaked, u32 charged, u32 usedItems)
{
    u32 state = ((soaked & (1u << actor) ? PAIR_SOAK_ATTACKER : 0)
        | (soaked & (1u << target) ? PAIR_SOAK_TARGET : 0)
        | (charged & (1u << actor) ? PAIR_CHARGED : 0)
        | (usedItems & (1u << actor) ? PAIR_DANCE_NO_ITEM : 0)
        | (usedItems & (1u << target) ? PAIR_DANCE_TARGET_NO_ITEM : 0)) & cache->mask;
    return &cache->damage[target * cache->count + cache->stateIndex[state]];
}

static struct SimulatedDamage PairConditionalBoostDamage(const struct PairEvaluation *ev, enum BattlerId actor,
    enum BattlerId target, u32 index, u32 partnerChance)
{
    const struct PairDamageRange *strong = &ev->conditionalBoosted[actor][index][target];
    const struct PairDamageRange *weak = &ev->conditionalUnboosted[actor][index][target];
    struct SimulatedDamage damage = {
        .affectsTarget = gAiLogicData->simulatedDmg[actor][target][index].affectsTarget,
        .consumedItem = gAiLogicData->simulatedDmg[actor][target][index].consumedItem,
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
        .affectsTarget = gAiLogicData->simulatedDmg[actor][target][index].affectsTarget,
        .consumedItem = gAiLogicData->simulatedDmg[actor][target][index].consumedItem,
        .minimum = PairInterpolateHpDamage(low->minimum, high->minimum, power, lowPower, highPower),
        .median = PairInterpolateHpDamage(low->median, high->median, power, lowPower, highPower),
        .maximum = PairInterpolateHpDamage(low->maximum, high->maximum, power, lowPower, highPower),
    };
    damage.random = damage.median;
    return damage;
}

// One guaranteed target-stat drop per supported move. STAT_HP is the sentinel.
static u32 PairTargetDropStat(enum Move move)
{
    switch (move)
    {
    case MOVE_CHARM:
    case MOVE_FEATHER_DANCE:
    case MOVE_LUNGE:
        return STAT_ATK;
    case MOVE_FAKE_TEARS:
    case MOVE_METAL_SOUND:
    case MOVE_ACID_SPRAY:
    case MOVE_APPLE_ACID:
    case MOVE_LUMINA_CRASH:
        return STAT_SPDEF;
    case MOVE_SNARL:
    case MOVE_EERIE_IMPULSE:
    case MOVE_STRUGGLE_BUG:
    case MOVE_SKITTER_SMACK:
        return STAT_SPATK;
    default:
        return STAT_HP;
    }
}

static bool32 PairPrimarySpeedDrop(enum Move move)
{
    return move == MOVE_TOXIC_THREAD || move == MOVE_STRING_SHOT
        || move == MOVE_SCARY_FACE || move == MOVE_COTTON_SPORE;
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

static bool32 PairSetupDance(enum Move move)
{
    return IsDanceMove(move) && (GetMoveEffect(move) == EFFECT_STAT_CHANGE || GetMoveEffect(move) == EFFECT_CLANGOROUS_SOUL)
        && GetMoveTarget(move) == TARGET_USER && IsStatRaisingMove(move);
}

static void CachePairDance(struct PairEvaluation *ev, enum BattlerId source, u32 index, enum Move move)
{
    if (!PairSetupDance(move))
        return;
    const enum Stat stats[] = {STAT_ATK, STAT_DEF, STAT_SPDEF, STAT_SPATK, STAT_SPEED};
    for (enum BattlerId receiver = 0; receiver < gBattlersCount; receiver++)
    {
        if (!IsBattlerAlive(receiver) || (receiver != source && gAiLogicData->abilities[receiver] != ABILITY_DANCER))
            continue;
        struct BattleCalcValues cv = {.battlerAtk = receiver, .battlerDef = receiver,
            .move = move, .moveEffect = GetMoveEffect(move)};
        memcpy(cv.abilities, gAiLogicData->abilities, sizeof(cv.abilities));
        memcpy(cv.holdEffects, gAiLogicData->holdEffects, sizeof(cv.holdEffects));
        for (u32 stat = 0; stat < ARRAY_COUNT(stats); stat++)
        {
            struct StatChange change = {.onlyChecking = TRUE, .certain = TRUE, .stat = stats[stat]};
            change.stage = GetAdjustedStatStage(GetStatStage(stats[stat], GetMoveAdditionalEffectById(move, 0)),
                cv.abilities[receiver], FALSE);
            // Cache native permission, not the starting cap. Earlier actions
            // can lower a maxed stat before this dance is performed.
            u8 savedStage = gBattleMons[receiver].statStages[stats[stat]];
            gBattleMons[receiver].statStages[stats[stat]] = DEFAULT_STAT_STAGE;
            if (change.stage && CanStatChange(&cv, &change))
                ev->danceDelta[source][index][receiver][stat] = change.stage;
            gBattleMons[receiver].statStages[stats[stat]] = savedStage;
        }
    }
}

static void CachePairMoveEffects(struct PairEvaluation *ev, enum BattlerId actor, u32 index)
{
    enum Move move = gBattleMons[actor].moves[index];
    enum Move executedMove = GetMoveEffect(move) == EFFECT_NATURE_POWER ? GetNaturePowerMove() : move;
    CachePairDance(ev, actor, index, executedMove);
    if (GetMoveEffect(executedMove) == EFFECT_REVELATION_DANCE)
    {
        struct BattlePokemon saved = gBattleMons[actor];
        for (u32 changed = 0; changed < 2; changed++)
        {
            if (changed)
                SET_BATTLER_TYPE(actor, TYPE_WATER);
            ev->revelationType[actor][changed] = GetDynamicMoveType(GetBattlerMon(actor), executedMove, actor,
                gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor], MON_IN_BATTLE);
        }
        gBattleMons[actor] = saved;
    }
    if (HasChoiceEffect(actor) && (gBattleStruct->choicedMove[actor] == MOVE_NONE
        || gBattleStruct->choicedMove[actor] == MOVE_UNAVAILABLE)
     && gBattleMons[actor].pp[index] > 1 && !IsBattleMoveStatus(executedMove)
     && GetMoveEffect(executedMove) != EFFECT_HIT_ESCAPE
     && executedMove != MOVE_FAKE_OUT && executedMove != MOVE_FIRST_IMPRESSION)
    {
        enum BattlerId partner = GetPartnerBattler(actor);
        bool32 harmsPartner = PairSpread(executedMove) && GetMoveTarget(executedMove) == TARGET_FOES_AND_ALLY
            && IsBattlerAlive(partner) && gAiLogicData->simulatedDmg[actor][partner][index].maximum;
        if (!harmsPartner)
        {
            ev->firstChoice[actor][index] = TRUE;
            for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                if (IsBattlerAlive(foe) && !IsBattlerAlly(actor, foe))
                {
                    u32 damage = min(gBattleMons[foe].maxHP, gAiLogicData->simulatedDmg[actor][foe][index].median);
                    ev->firstChoiceValue[actor][index] += 8 * damage / max(1, gBattleMons[foe].maxHP);
                    ev->firstChoiceShare[actor][index][foe] = 100 * damage / max(1, gBattleMons[foe].maxHP);
                }
        }
    }
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
    CachePairSoakChargeDamage(ev, actor, index, executedMove);
    CachePairRageDamage(ev, actor, index, executedMove);
    if (AI_MoveBreaksScreensBeforeDamage(executedMove))
        ev->screenBreakerMoves[actor] |= 1u << index;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
        if (target != actor && IsBattlerAlive(actor) && IsBattlerAlive(target))
        {
            ev->contactDamage[actor][index][target] = AI_GetContactDamage(actor, target, executedMove,
                gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor], gAiLogicData->holdEffects[target],
                gAiLogicData->abilities[target]);
            if ((ev->rageActors & (1u << target)) && !IsBattleMoveStatus(executedMove))
                ev->rageHitCount[actor][index][target] = AI_GetMinimumRageHits(actor, target, executedMove);
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
    bool32 burn = move == MOVE_WILL_O_WISP;
    bool32 quash = move == MOVE_QUASH;
    bool32 taunt = GetMoveEffect(move) == EFFECT_TAUNT;
    u32 targetDropStat = PairTargetDropStat(move);
    bool32 electricParalysis = move == MOVE_THUNDER_WAVE || move == MOVE_NUZZLE;
    bool32 primaryParalysis = move == MOVE_THUNDER_WAVE || move == MOVE_GLARE;
    bool32 paralysis = electricParalysis || move == MOVE_GLARE || move == MOVE_BODY_SLAM;
    bool32 encore = GetMoveEffect(move) == EFFECT_ENCORE;
    bool32 primarySpeedDrop = PairPrimarySpeedDrop(move);
    bool32 speedDrop = move == MOVE_ELECTROWEB || move == MOVE_ICY_WIND || move == MOVE_ROCK_TOMB || primarySpeedDrop;
    if (move != MOVE_COACHING && !speedDrop && !sleep && !burn && !quash && !taunt && !encore && !paralysis && !targetDropStat)
        return;
    if (paralysis)
        ev->paralysisChance[actor][index] = move == MOVE_BODY_SLAM
            ? min(100, CalcSecondaryEffectChance(actor, gAiLogicData->abilities[actor], move, GetMoveAdditionalEffectById(move, 0)))
            : 100;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (target == actor || !IsBattlerAlive(target)
         || (move == MOVE_COACHING && target != GetPartnerBattler(actor))
         // Sleep can legally target an ally. Cache its effect as well so the
         // trial charges the lost action/status instead of treating it as free.
         || ((speedDrop || burn || quash || taunt || encore || paralysis || targetDropStat) && IsBattlerAlly(actor, target)))
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
        if (targetDropStat)
        {
            ev->targetDropDelta[actor][index][target] = PairCacheTargetDrop(actor, target, move);
            continue;
        }
        // Primary status effects need their own immunity checks, not damage's
        // zero cache or secondary-effect Shield Dust/Covert Cloak checks.
        if ((primarySpeedDrop || sleep || burn || quash || taunt || encore || primaryParalysis) && (cv.abilities[target] == ABILITY_GOOD_AS_GOLD
             || cv.abilities[target] == ABILITY_MAGIC_BOUNCE
             || gProtectStructs[target].bounceMove
             || (GetConfig(B_PRANKSTER_DARK_TYPES) >= GEN_7
                 && cv.abilities[actor] == ABILITY_PRANKSTER && IS_BATTLER_OF_TYPE(target, TYPE_DARK))
             || (IsSemiInvulnerable(target, CHECK_ALL)
                 && !BreaksThroughSemiInvulnerableState(actor, target, cv.abilities[actor], cv.abilities[target],
                     move, gBattleMons[target].volatiles.semiInvulnerable))))
            continue;
        if (primarySpeedDrop)
        {
            if (IsPowderMove(move) && !IsAffectedByPowderMove(target, cv.abilities[target], cv.holdEffects[target]))
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
        if (taunt)
        {
            if (gBattleMons[target].volatiles.tauntTimer
             || (GetConfig(B_OBLIVIOUS_TAUNT) >= GEN_6 && cv.abilities[target] == ABILITY_OBLIVIOUS)
             || DoesSubstituteBlockMove(actor, target, move))
                continue;
            s32 priority = AI_GetMovePriority(actor, cv.abilities[actor], move);
            bool32 blocked = priority > 0
                && IsPsychicTerrainAffected(target, cv.abilities[target], cv.holdEffects[target], gFieldTimers.terrain);
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                if (IsBattlerAlive(ally) && IsBattlerAlly(target, ally)
                 && (cv.abilities[ally] == ABILITY_AROMA_VEIL || (priority > 0 && IsDazzlingAbility(cv.abilities[ally]))))
                    blocked = TRUE;
            if (!blocked)
                ev->tauntTargets[actor][index] |= 1u << target;
            continue;
        }
        if (burn || quash)
        {
            // Only ordinary uncured burn is modeled here. Status-benefit and
            // reflection/cure interactions retain their existing opinion,
            // never a fictitious physical-damage reduction.
            if (DoesSubstituteBlockMove(actor, target, move))
                continue;
            if (burn && (!CanSetNonVolatileStatus(actor, target, cv.abilities[actor], cv.abilities[target], MOVE_EFFECT_BURN, CHECK_TRIGGER)
             || cv.abilities[target] == ABILITY_GUTS || cv.abilities[target] == ABILITY_FLARE_BOOST
             || cv.abilities[target] == ABILITY_QUICK_FEET || cv.abilities[target] == ABILITY_MARVEL_SCALE
             || cv.abilities[target] == ABILITY_FLASH_FIRE || cv.abilities[target] == ABILITY_WELL_BAKED_BODY
             || cv.abilities[target] == ABILITY_SYNCHRONIZE
             || cv.holdEffects[target] == HOLD_EFFECT_CURE_BRN || cv.holdEffects[target] == HOLD_EFFECT_CURE_STATUS))
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
            if (burn)
                ev->burnTargets[actor][index] |= 1u << target;
            else
                ev->quashTargets[actor][index] |= 1u << target;
            continue;
        }
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
        if (speedDrop
         && (DoesSubstituteBlockMove(actor, target, move)
             || (!primarySpeedDrop && (IsSheerForceAffected(move, cv.abilities[actor])
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

static bool32 PairApplyDance(struct PairEvaluation *ev, enum BattlerId source, u32 index, enum BattlerId receiver,
    u8 stages[MAX_BATTLERS_COUNT][4], u32 modifiers[MAX_BATTLERS_COUNT][4], u8 *speedStage,
    u32 *speed, const struct PairAction *actions, u32 *usedItems, u32 *hp)
{
    enum Move move = gBattleMons[source].moves[index];
    u32 cost = GetMoveEffect(move) == EFFECT_CLANGOROUS_SOUL
        ? max(1, PairNonDynamaxHP(ev, receiver, gBattleMons[receiver].maxHP) / 3) : 0;
    if (hp[receiver] <= cost)
        return FALSE;
    bool32 changed = FALSE;
    for (u32 stat = 0; stat < 4; stat++)
    {
        u8 old = stages[receiver][stat];
        PairChangeStat(&stages[receiver][stat], &modifiers[receiver][stat],
            ev->danceDelta[source][index][receiver][stat], 100, stat == 1 || stat == 2);
        changed |= old != stages[receiver][stat];
    }
    u32 oldSpeed = speedStage[receiver];
    u32 nextSpeed = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE,
        (s32)oldSpeed + ev->danceDelta[source][index][receiver][4]));
    changed |= nextSpeed != oldSpeed;
    bool32 herb = FALSE;
    if (changed && gAiLogicData->holdEffects[receiver] == HOLD_EFFECT_WHITE_HERB
     && !(*usedItems & (1u << receiver)))
    {
        for (u32 stat = 0; stat < 4; stat++)
            if (stages[receiver][stat] < DEFAULT_STAT_STAGE)
            {
                PairChangeStat(&stages[receiver][stat], &modifiers[receiver][stat],
                    DEFAULT_STAT_STAGE - stages[receiver][stat], 100, stat == 1 || stat == 2);
                herb = TRUE;
            }
        if (nextSpeed < DEFAULT_STAT_STAGE)
        {
            nextSpeed = DEFAULT_STAT_STAGE;
            herb = TRUE;
        }
    }
    speed[receiver] = speed[receiver] * gStatStageRatios[nextSpeed][0] * gStatStageRatios[oldSpeed][1]
        / (gStatStageRatios[nextSpeed][1] * gStatStageRatios[oldSpeed][0]);
    speedStage[receiver] = nextSpeed;
    if (herb)
    {
        *usedItems |= 1u << receiver;
        if (gAiLogicData->abilities[receiver] == ABILITY_UNBURDEN && !gBattleMons[receiver].volatiles.unburdenActive)
            speed[receiver] *= 2;
    }
    if (changed && cost)
    {
        hp[receiver] -= cost;
        PairTryHealingBerry(ev, receiver, hp, speed, usedItems);
    }
    return changed;
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

static u32 PairScreenMask(enum Move move)
{
    switch (GetMoveEffect(move))
    {
    case EFFECT_REFLECT:
        return SIDE_STATUS_REFLECT;
    case EFFECT_LIGHT_SCREEN:
        return SIDE_STATUS_LIGHTSCREEN;
    case EFFECT_AURORA_VEIL:
        return SIDE_STATUS_AURORA_VEIL;
    default:
        return 0;
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

static bool32 PairDancerCanAct(enum BattlerId actor, enum Move move, const u32 *hp,
    u32 stopped, u32 newSleep, u32 newTaunt)
{
    return hp[actor] && !(stopped & (1u << actor)) && !(newSleep & (1u << actor))
        && (gBattleMons[actor].status1 & STATUS1_SLEEP) <= 1
        && (!(gBattleMons[actor].status1 & STATUS1_FREEZE) || MoveThawsUser(move))
        && !gBattleMons[actor].volatiles.rechargeTimer && !gBattleMons[actor].volatiles.flinched
        && gBattleMons[actor].volatiles.disabledMove != move
        && !IsSemiInvulnerable(actor, CHECK_ALL) && !GetImprisonedMovesCount(actor, move)
        && (!IsBattleMoveStatus(move) || (!(newTaunt & (1u << actor)) && !gBattleMons[actor].volatiles.tauntTimer))
        && (!IsSoundMove(move) || !gBattleMons[actor].volatiles.throatChopTimer);
}

static void PairCopyTargetWeights(const struct PairDancerMoveCache *cache, enum BattlerId actor,
    enum BattlerId source, enum Move move, const u8 *sourceWeights, const u32 *hp, const u32 *speed,
    const struct PairAction *actions, const enum BattlerId *redirect, bool32 trickRoom,
    u32 soaked, u32 usedItems, u8 *weights)
{
    enum BattleSide foeSide = GetBattlerSide(actor) ^ 1;
    enum Type type = cache->type[!!(soaked & (1u << actor))];
    u32 total = 0, foes = 0;
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        total += sourceWeights[target];
        if (hp[target] && !IsBattlerAlly(actor, target))
            foes++;
    }
    for (enum BattlerId original = 0; original < gBattlersCount; original++)
    {
        enum BattlerId target = original;
        u32 chance = sourceWeights[original];
        if (GetMoveTarget(move) == TARGET_RANDOM)
        {
            if (!hp[target] || IsBattlerAlly(actor, target) || !foes)
                continue;
            chance = total / foes;
        }
        else
        {
            if (!chance)
                continue;
            if (IsBattlerAlly(actor, target))
                target = source;
            enum BattlerId follow = redirect[foeSide];
            bool32 powder = follow < gBattlersCount
                && (gSideTimers[foeSide].followmePowder || IsPowderMove(actions[follow].move));
            enum HoldEffect held = usedItems & (1u << actor) ? HOLD_EFFECT_NONE : gAiLogicData->holdEffects[actor];
            bool32 powderApplies = soaked & (1u << actor)
                ? held != HOLD_EFFECT_SAFETY_GOGGLES
                : IsAffectedByPowderMove(actor, gAiLogicData->abilities[actor], held);
            if (!IsBattlerAlly(actor, target) && follow < gBattlersCount && hp[follow]
             && (!powder || powderApplies))
                target = follow;
        }
        // Native Follow Me suppresses ability redirection. Otherwise the
        // earliest eligible Rod/Drain owner receives the copied Water/Electric move.
        if (redirect[foeSide] >= gBattlersCount && !gSideTimers[foeSide].followmeTimer)
        {
            enum Ability redirects = type == TYPE_ELECTRIC ? ABILITY_LIGHTNING_ROD
                : type == TYPE_WATER ? ABILITY_STORM_DRAIN : ABILITY_NONE;
            enum BattlerId found = MAX_BATTLERS_COUNT;
            if (redirects != ABILITY_NONE && gAiLogicData->abilities[target] != redirects)
                for (enum BattlerId other = 0; other < gBattlersCount; other++)
                    if (other != actor && hp[other] && gAiLogicData->abilities[other] == redirects
                     && (B_REDIRECT_ABILITY_ALLIES >= GEN_4 || !IsBattlerAlly(actor, other))
                     && (found == MAX_BATTLERS_COUNT || PairActsBefore(other, &actions[other], found, &actions[found], speed, trickRoom)))
                        found = other;
            if (found < gBattlersCount)
                target = found;
        }
        if (!hp[target] && !IsBattlerAlly(actor, target))
            target = GetPartnerBattler(target);
        if (hp[target])
            weights[target] = min(100, weights[target] + chance);
    }
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

// What a self-boost raises: a mask of the columns the trial tracks (bit 0
// Attack, 1 Defense, 2 Sp. Def, 3 Sp. Atk) and its Speed stages, after the
// user's own Simple. A Contrary body's "boost" is a drop and Acupressure's is
// a lottery; neither is read here.
static u32 PairSelfBoostRaises(enum BattlerId actor, enum Move move, s32 *speedStages)
{
    *speedStages = 0;
    if (move == MOVE_NONE || !IsStatRaisingMove(move) || GetMoveEffect(move) == EFFECT_ACUPRESSURE
     || AI_GetBattlerMoveTargetType(actor, move) != TARGET_USER
     || gAiLogicData->abilities[actor] == ABILITY_CONTRARY)
        return 0;
    u32 raised = 0;
    for (u32 effectIndex = 0; effectIndex < GetMoveAdditionalEffectCount(move); effectIndex++)
    {
        const struct AdditionalEffect *additional = GetMoveAdditionalEffectById(move, effectIndex);
        if (additional->moveEffect != STAT_CHANGE_EFFECT_PLUS)
            continue;
        if (GetStatStage(STAT_ATK, additional))
            raised |= 1u << 0;
        if (GetStatStage(STAT_DEF, additional))
            raised |= 1u << 1;
        if (GetStatStage(STAT_SPDEF, additional))
            raised |= 1u << 2;
        if (GetStatStage(STAT_SPATK, additional))
            raised |= 1u << 3;
        if (GetStatStage(STAT_SPEED, additional))
            *speedStages += GetAdjustedStatStage(GetStatStage(STAT_SPEED, additional), gAiLogicData->abilities[actor], FALSE);
    }
    return raised;
}

// The share of a self-boost's full reward that its raise is still worth from
// the stages the user already holds: the damage multiplier the raise adds,
// relative to the same raise from neutral. +2 Defense from +3 multiplies it by
// 1.4 where the first +2 doubled it, so it is worth 40 percent. Speed is order,
// not a multiplier, and keeps its full worth. Wallace's burned Zamazenta took
// Iron Defense from +3 to +5 in front of a Great Tusk its partner was about to
// remove, on the full reward of a first boost.
static u32 PairBoostWorth(enum BattlerId actor, enum Move move)
{
    u32 best = 0;
    bool32 any = FALSE;
    for (u32 effectIndex = 0; effectIndex < GetMoveAdditionalEffectCount(move); effectIndex++)
    {
        const struct AdditionalEffect *additional = GetMoveAdditionalEffectById(move, effectIndex);
        if (additional->moveEffect != STAT_CHANGE_EFFECT_PLUS)
            continue;
        static const u8 stats[] = {STAT_ATK, STAT_DEF, STAT_SPATK, STAT_SPDEF, STAT_SPEED};
        for (u32 index = 0; index < ARRAY_COUNT(stats); index++)
        {
            s32 delta = GetAdjustedStatStage(GetStatStage(stats[index], additional), gAiLogicData->abilities[actor], FALSE);
            if (delta <= 0)
                continue;
            any = TRUE;
            if (stats[index] == STAT_SPEED)
                return 100;
            s32 old = (s32)gBattleMons[actor].statStages[stats[index]] - DEFAULT_STAT_STAGE;
            s32 next = min(MAX_STAT_STAGE - DEFAULT_STAT_STAGE, old + delta);
            if (next <= old)
                continue;
            u32 oldNum = old >= 0 ? 2 + old : 2, oldDen = old >= 0 ? 2 : 2 - old;
            u32 newNum = next >= 0 ? 2 + next : 2, newDen = next >= 0 ? 2 : 2 - next;
            s32 gain = (s32)(newNum * oldDen * 100 / (newDen * oldNum)) - 100;
            best = max(best, min(100, (u32)max(0, gain) * 100 / (u32)(delta * 50)));
        }
    }
    return any ? best : 100;
}

// A boost that only raises Defense or Sp. Def pays through the foes it is
// raised against: those still standing after this turn with a hit of that
// category worth trimming (an eighth of the user's HP), or, for Defense, a foe
// the user's Body Press can reach. The share is the likeliest of them to be
// there. Wallace's Zamazenta raised Defense into a Great Tusk that Zapdos'
// Hurricane was about to remove, beside a Sableye whose Knock Off barely
// scratches it.
static u32 PairDefenseBoostShare(enum BattlerId actor, enum Move move, const u32 *hp, const u32 *survival)
{
    s32 speedStages;
    u32 raised = PairSelfBoostRaises(actor, move, &speedStages);
    if (speedStages || (raised & ((1u << 0) | (1u << 3))) || !(raised & ((1u << 1) | (1u << 2))))
        return 100;
    bool32 bodyPress = FALSE;
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        if (GetMoveEffect(gBattleMons[actor].moves[slot]) == EFFECT_BODY_PRESS
         && !IsMoveUnusable(slot, gBattleMons[actor].moves[slot], gAiLogicData->moveLimitations[actor]))
            bodyPress = TRUE;
    // Through Body Press a Defense boost is offense, and offense bought with
    // this turn's Press only breaks even on the next one: +2 then a doubled
    // Press is two Presses. It gains from the second boosted Press, so the
    // user has to outlast two more rounds of the hits the boost does not
    // blunt. Winona's Mega Skarmory raised Defense in front of a lone Lanturn
    // whose special Thunderbolt took five sixths of it.
    if (bodyPress && (raised & (1u << 1)))
    {
        u32 incoming = 0;
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (!hp[foe] || !IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
                continue;
            u32 best = 0;
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                enum Move known = gBattleMons[foe].moves[slot];
                if (known == MOVE_NONE || known == MOVE_UNAVAILABLE || IsBattleMoveStatus(known)
                 || IsMoveUnusable(slot, known, gAiLogicData->moveLimitations[foe]))
                    continue;
                bool32 physical = IsBattleMovePhysical(known) || GetMoveEffect(known) == EFFECT_PSYSHOCK;
                if (raised & (physical ? (1u << 1) : (1u << 2)))
                    continue;
                best = max(best, gAiLogicData->simulatedDmg[foe][actor][slot].median);
            }
            incoming += best;
        }
        if (incoming * 2 >= hp[actor])
            bodyPress = FALSE;
    }
    u32 threshold = max(1, gBattleMons[actor].maxHP / 8), share = 0;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!hp[foe] || !IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
            continue;
        bool32 pays = FALSE;
        for (u32 slot = 0; slot < MAX_MON_MOVES && !pays; slot++)
        {
            enum Move known = gBattleMons[foe].moves[slot];
            if (known == MOVE_NONE || known == MOVE_UNAVAILABLE || IsBattleMoveStatus(known)
             || gAiLogicData->simulatedDmg[foe][actor][slot].maximum < threshold)
                continue;
            bool32 physical = IsBattleMovePhysical(known) || GetMoveEffect(known) == EFFECT_PSYSHOCK;
            if (raised & (physical ? (1u << 1) : (1u << 2)))
                pays = TRUE;
        }
        if (!pays && bodyPress && (raised & (1u << 1)))
            for (u32 slot = 0; slot < MAX_MON_MOVES && !pays; slot++)
                if (GetMoveEffect(gBattleMons[actor].moves[slot]) == EFFECT_BODY_PRESS
                 && gAiLogicData->simulatedDmg[actor][foe][slot].maximum)
                    pays = TRUE;
        if (pays)
            share = max(share, survival[foe]);
    }
    return share;
}

// A self-boost pays through its user's next action, so it pays nothing when
// a visible foe still standing after this turn will move before that action
// and is certain to knock the user out from the HP it keeps: the foe's
// weakest roll, with a hit that cannot miss. Declining the knockout this turn
// only moves it to the next one. Braxton's Garchomp Swords Danced in front of
// a faster Miraidon whose Draco Meteor removes it from full. What the boost
// itself buys still counts: a Speed boost that puts the user in front, or a
// defensive boost on the side the hit comes from. A Focus Sash or Sturdy on a
// full-HP user keeps it standing, and a Fake Out is spent by next turn.
static bool32 PairBoostOutrun(enum BattlerId actor, const struct PairAction *action, const u32 *hp, const u32 *speed, const u8 *speedStage, bool32 trickRoom)
{
    s32 speedStages;
    u32 raised = PairSelfBoostRaises(actor, action->executedMove, &speedStages);
    if (action->index == PAIR_IDLE || !hp[actor] || (!raised && !speedStages))
        return FALSE;
    u32 mySpeed = speed[actor];
    // The trial applies a dance's Speed itself; any other boost's Speed is
    // what the user carries into next turn.
    if (speedStages && !PairSetupDance(action->executedMove))
    {
        u32 old = speedStage[actor];
        u32 next = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE, (s32)old + speedStages));
        mySpeed = mySpeed * gStatStageRatios[next][0] * gStatStageRatios[old][1]
            / (gStatStageRatios[next][1] * gStatStageRatios[old][0]);
    }
    s32 myPriority = 0;
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        enum Move known = gBattleMons[actor].moves[slot];
        if (known != MOVE_NONE && !IsBattleMoveStatus(known) && GetMoveEffect(known) != EFFECT_FIRST_TURN_ONLY
         && !IsMoveUnusable(slot, known, gAiLogicData->moveLimitations[actor]))
            myPriority = max(myPriority, AI_GetMovePriority(actor, gAiLogicData->abilities[actor], known));
    }
    bool32 enduresFull = hp[actor] >= gBattleMons[actor].maxHP
        && (gAiLogicData->holdEffects[actor] == HOLD_EFFECT_FOCUS_SASH || gAiLogicData->abilities[actor] == ABILITY_STURDY);
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!hp[foe] || !IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
            continue;
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        {
            enum Move known = gBattleMons[foe].moves[slot];
            if (known == MOVE_NONE || known == MOVE_UNAVAILABLE || IsBattleMoveStatus(known)
             || GetMoveEffect(known) == EFFECT_FIRST_TURN_ONLY
             || IsMoveUnusable(slot, known, gAiLogicData->moveLimitations[foe])
             || gAiLogicData->moveAccuracy[foe][actor][slot] < 100
             || gAiLogicData->simulatedDmg[foe][actor][slot].minimum < hp[actor])
                continue;
            bool32 physical = IsBattleMovePhysical(known) || GetMoveEffect(known) == EFFECT_PSYSHOCK;
            if ((raised & (physical ? (1u << 1) : (1u << 2))) || enduresFull)
                continue;
            s32 priority = AI_GetMovePriority(foe, gAiLogicData->abilities[foe], known);
            if (priority > myPriority
             || (priority == myPriority && (trickRoom ? speed[foe] < mySpeed : speed[foe] > mySpeed)))
                return TRUE;
        }
    }
    return FALSE;
}

static s32 ScoreFastPair(struct PairEvaluation *ev, bool32 applyEffects, u32 *effectChance)
{
    struct PairAction actions[MAX_BATTLERS_COUNT];
    memcpy(actions, ev->action, sizeof(actions));
    // A plan veto holds whether or not this trial lets the move run. The
    // per-action check below is reached only by an action that executes, so
    // a body forecast to fall first, or a Mirror Coat whose trial found no hit
    // to return, made its certain failure a free choice: the fisherman's
    // Feebas picked Mirror Coat at 28 HP with Dragon Rage its only read.
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (GetBattlerSide(battler) == ev->side && actions[battler].index != PAIR_IDLE
         && actions[battler].planScore <= -10000)
            return -10000;
    u32 hp[MAX_BATTLERS_COUNT], boost[MAX_BATTLERS_COUNT] = {100, 100, 100, 100};
    u32 statusBoost[MAX_BATTLERS_COUNT] = {100, 100, 100, 100};
    u32 survival[MAX_BATTLERS_COUNT] = {100, 100, 100, 100};
    u32 actionChance[MAX_BATTLERS_COUNT] = {10000, 10000, 10000, 10000};
    s32 coachingValue[MAX_BATTLERS_COUNT] = {0};
    u32 newParalysisTargets = 0, newBurnTargets = 0;
    u32 newTauntTargets = 0;
    u32 speed[MAX_BATTLERS_COUNT];
    u8 speedStage[MAX_BATTLERS_COUNT];
    u8 statStage[MAX_BATTLERS_COUNT][4]; // Atk, Def, SpDef, SpAtk.
    u32 statModifier[MAX_BATTLERS_COUNT][4];
    u32 usedItems = 0;
    u32 soaked = 0, charged = 0;
    u32 choiceStarted = 0;
    u8 rageHits[MAX_BATTLERS_COUNT];
    u32 acted = 0, protected = 0, stopped = 0, wideGuard = 0, quickGuard = 0;
    u32 executed = 0; // Selected actions that reached the board this trial.
    u32 sashBroken = 0; // Focus Sashes this trial's hits spent.
    struct PairRetaliation received[MAX_BATTLERS_COUNT][DAMAGE_CATEGORY_STATUS] = {0};
    u8 lastReceivedCategory[MAX_BATTLERS_COUNT] = {0};
    u32 newSleepTargets = 0, newSleepSides = 0;
    u32 newWish = 0;
    u32 encoredGuards = 0;
    u8 forcedGuardChance[MAX_BATTLERS_COUNT] = {0};
    u8 forcedWideChance[NUM_BATTLE_SIDES] = {0}, forcedQuickChance[NUM_BATTLE_SIDES] = {0};
    // Health value this turn's guards actually denied, credited to the battler
    // that chose the guard. Measured inside the trial, so turn order, misses,
    // redirection and an ally that removes the attacker first all apply.
    s32 guardDenied[MAX_BATTLERS_COUNT] = {0};
    u32 guardUsed = 0;
    s32 tacticReward[MAX_BATTLERS_COUNT] = {0};
    s32 setupReward[MAX_BATTLERS_COUNT] = {0};
    u32 friendlyLoss[MAX_BATTLERS_COUNT] = {0}; // Expected HP our spread moves took from our own side.
    u8 sideGuardUser[NUM_BATTLE_SIDES][2];
    memset(sideGuardUser, MAX_BATTLERS_COUNT, sizeof(sideGuardUser));
    u32 weather = ev->weather;
    bool32 trickRoom = gFieldStatuses & STATUS_FIELD_TRICK_ROOM;
    enum BattlerId forceNext = MAX_BATTLERS_COUNT;
    enum BattlerId redirect[NUM_BATTLE_SIDES] = {MAX_BATTLERS_COUNT, MAX_BATTLERS_COUNT};
    u32 pendingDancers = 0;
    enum BattlerId danceSource = MAX_BATTLERS_COUNT;
    enum Move queuedDance = MOVE_NONE;
    u8 queuedTargets[MAX_BATTLERS_COUNT] = {0};
    u32 screens[NUM_BATTLE_SIDES];
    for (u32 side = 0; side < NUM_BATTLE_SIDES; side++)
    {
        screens[side] = gSideStatuses[side] & SIDE_STATUS_SCREEN_ANY;
        if (gSideTimers[side].followmeTimer && IsBattlerAlive(gSideTimers[side].followmeTarget))
            redirect[side] = gSideTimers[side].followmeTarget;
    }
    s32 score = ev->board.reserveValue[ev->side] - ev->board.reserveValue[ev->side ^ 1];
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        hp[actor] = gBattleMons[actor].hp;
        rageHits[actor] = min(6, GetBattlerPartyState(actor)->timesGotHit);
        if (gBattleMons[actor].volatiles.chargeTimer)
            charged |= 1u << actor;
        speed[actor] = gAiLogicData->speedStats[actor];
        speedStage[actor] = gBattleMons[actor].statStages[STAT_SPEED];
        statStage[actor][0] = gBattleMons[actor].statStages[STAT_ATK];
        statStage[actor][1] = gBattleMons[actor].statStages[STAT_DEF];
        statStage[actor][2] = gBattleMons[actor].statStages[STAT_SPDEF];
        statStage[actor][3] = gBattleMons[actor].statStages[STAT_SPATK];
        for (u32 stat = 0; stat < 4; stat++)
            statModifier[actor][stat] = 100;
        if (GetBattlerSide(actor) == ev->side && actions[actor].index != PAIR_IDLE
         && (PairSecondCopyFails(ev->side, &actions[actor], &actions[GetPartnerBattler(actor)])
          || PairHelpsNothing(&actions[actor], &actions[GetPartnerBattler(actor)])))
            return -10000;
    }
    for (u32 turn = 0; turn < gBattlersCount || pendingDancers;)
    {
        enum BattlerId actor = MAX_BATTLERS_COUNT;
        bool32 copied = pendingDancers != 0;
        struct PairAction copyAction = {0};
        u8 copyWeights[MAX_BATTLERS_COUNT] = {0};
        if (copied)
        {
            for (enum BattlerId next = 0; next < gBattlersCount; next++)
                if (pendingDancers & (1u << next))
                {
                    bool32 earlier = actor == MAX_BATTLERS_COUNT;
                    if (!earlier)
                        earlier = GetConfig(B_DANCER_ORDER) < GEN_8
                            ? gBattleMons[next].speed < gBattleMons[actor].speed : speed[next] > speed[actor];
                    if (earlier)
                        actor = next;
                }
            pendingDancers &= ~(1u << actor);
            u32 dance = PairCopiedDance(queuedDance);
            if (dance >= ARRAY_COUNT(sCopiedDances) || ev->dancer[actor][dance] == NULL
             || !PairDancerCanAct(actor, queuedDance, hp, stopped, newSleepTargets, newTauntTargets))
                continue;
            copyAction = (struct PairAction){.move = queuedDance, .executedMove = queuedDance,
                .score = AI_SCORE_DEFAULT, .target = danceSource, .danceCopy = dance + 1};
            PairCopyTargetWeights(ev->dancer[actor][dance], actor, danceSource, queuedDance, queuedTargets,
                hp, speed, actions, redirect, trickRoom, soaked, usedItems, copyWeights);
            if ((gBattleMons[actor].status1 & STATUS1_PARALYSIS) && actionChance[actor] == 10000)
                for (u32 target = 0; target < gBattlersCount; target++)
                    copyWeights[target] = copyWeights[target] * ev->paralysisActionChance / 10000;
        }
        else
        {
            turn++;
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
        }
        if (stopped & (1u << actor))
            continue;
        const struct PairAction *action = copied ? &copyAction : &actions[actor];
        const struct PairDancerMoveCache *copy = copied ? ev->dancer[actor][action->danceCopy - 1] : NULL;
        enum BattlerId partner = GetPartnerBattler(actor);
        const struct PairAction *other = &actions[partner];
        enum Move move = action->move;
        enum BattleMoveEffects effect = GetMoveEffect(move);
        s32 sign = GetBattlerSide(actor) == ev->side ? 1 : -1;
        s32 damageOpinion = 0;
        s32 coachingOpinion = 0;
        if (((gBattleMons[actor].status1 & STATUS1_SLEEP) > (gAiLogicData->abilities[actor] == ABILITY_EARLY_BIRD ? 2 : 1)
             && !IsUsableWhileAsleepEffect(effect))
         || ((gBattleMons[actor].status1 & STATUS1_FREEZE) && !MoveThawsUser(move))
         || gBattleMons[actor].volatiles.rechargeTimer
         || (gAiLogicData->abilities[actor] == ABILITY_TRUANT && gBattleMons[actor].volatiles.truantCounter))
            continue;
        // Taunt cancels the selected status command, including Nature Power's
        // wrapper. A failed setup earns neither its effect nor its plan reward.
        if (((newTauntTargets & (1u << actor)) || gBattleMons[actor].volatiles.tauntTimer)
         && IsBattleMoveStatus(move))
            continue;
        if (!copied)
            executed |= 1u << actor;
        if (!copy && ev->firstChoice[actor][action->index])
            choiceStarted |= 1u << actor;
        // Conditional HP includes branches where an earlier lethal move
        // missed. A surviving setter must not supply certain weather/order
        // changes (or the full authored setup reward) on those rare branches.
        if (effect == EFFECT_WEATHER || effect == EFFECT_TAILWIND || effect == EFFECT_TRICK_ROOM)
        {
            u32 chance = survival[actor] * actionChance[actor] / 10000;
            if ((gBattleMons[actor].status1 & STATUS1_PARALYSIS)
             && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[actor] == ABILITY_MAGIC_GUARD))
                chance = chance * ev->paralysisActionChance / 10000;
            if ((MoveCanBeSnatched(move) && PairMayBeSnatched(actor, actions, hp, acted, stopped))
             || !PairScreenEffectApplies(applyEffects, chance, effectChance))
                continue;
        }
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
            // Nor is a hit from a body the returned damage cannot affect.
            if (!retaliation->chance
             || (retaliation->source < gBattlersCount && hp[retaliation->source]
                 && gAiLogicData->effectiveness[actor][retaliation->source][action->index] == UQ_4_12(0.0)))
            {
                score -= sign * 150;
                continue;
            }
        }
        if (sign > 0 && !copy)
        {
            s32 planScore = action->planScore;
            // A status move's reward is paid only where its payoff lands: a
            // self-boost in the branches where its user is still standing at
            // the end of the turn, anything else in those where the user is
            // alive to act now. The trial already gates the effect itself;
            // the plan and opinion rewards rode on a user that the forecast
            // had knocked out: Juan's Manaphy Tail Glowed into the Wild Charge
            // that ended it.
            s32 statusReward = 0;
            bool32 statusPayoff = IsBattleMoveStatus(action->executedMove) && effect != EFFECT_PROTECT;
            if (planScore <= -10000)
                return -10000;
            if (move == MOVE_COACHING && planScore > 0)
                coachingOpinion += planScore;
            else if (statusPayoff && planScore > 0)
                statusReward += planScore;
            else
                score += planScore;
            // Deferred: an activation that kills its own recipient is not the
            // authored interaction, so the reward is only paid if the recipient
            // is still standing when the turn ends.
            if (action->tacticScore)
            {
                // The recipient's own shield blocks the activation this pair
                // just chose. Three runs of an authored friendly critical hit
                // never firing were this: Frost Breath aimed at a Tauros that
                // was spending the same turn behind Protect. The pair picks
                // both actions together, so it has no excuse for emitting a
                // trigger its own partner will block.
                enum BattlerId recipient = action->target;
                if (recipient < gBattlersCount && recipient != actor
                 && IsBattlerAlly(actor, recipient)
                 && GetMoveEffect(actions[recipient].move) == EFFECT_PROTECT
                 && actions[recipient].index != PAIR_IDLE)
                    tacticReward[actor] = 0;
                else
                    tacticReward[actor] = action->tacticScore;
            }
            // The complete turn owns protection's value. Applying the
            // isolated scorer again can prefer a blocked attack over a shield
            // that saves HP while producing the same damage on both sides.
            // An authored activation is judged by its own reward and by what
            // the trial does with it. The isolated opinion of hitting your own
            // partner is the wrong gate for it, exactly as it is for the
            // enumeration filter that would have dropped it.
            if (!action->tacticScore
             && !PairSupport(move) && effect != EFFECT_WISH && effect != EFFECT_PROTECT && effect != EFFECT_REFLECT_DAMAGE
             && !PairHasCopiedDance(ev, actor, action->executedMove)
             // The old target/type opinion is stale after Soak. Native damage
             // on this trial's changed board owns the attack's value instead.
             && !(soaked && !IsBattleMoveStatus(action->executedMove)))
            {
                if (action->score == 0)
                    score -= 500;
                else
                {
                    s32 opinion = (action->score - AI_SCORE_DEFAULT) * 4;
                    if (move == MOVE_COACHING && opinion > 0)
                        coachingOpinion += opinion;
                    else if (statusPayoff && opinion > 0)
                        statusReward += opinion;
                    else if (opinion <= 0 || IsBattleMoveStatus(action->executedMove))
                        score += opinion;
                    else
                        damageOpinion = opinion;
                }
            }
            if (statusReward && IsStatRaisingMove(move) && AI_GetBattlerMoveTargetType(actor, move) == TARGET_USER)
                setupReward[actor] += statusReward;
            else if (statusReward)
                score += statusReward * (s32)(survival[actor] * actionChance[actor] / 10000) / 100;
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
            // Spending the action is a cost even when the shield then fails.
            if (sign > 0 && !copy)
                score -= PAIR_GUARD_TEMPO;
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
            guardUsed |= 1u << actor;
            if (move == MOVE_WIDE_GUARD)
            {
                wideGuard |= 1u << GetBattlerSide(actor);
                sideGuardUser[GetBattlerSide(actor)][0] = actor;
            }
            else if (move == MOVE_QUICK_GUARD)
            {
                quickGuard |= 1u << GetBattlerSide(actor);
                sideGuardUser[GetBattlerSide(actor)][1] = actor;
            }
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
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                if (hp[ally] && IsBattlerAlly(actor, ally) && gAiLogicData->abilities[ally] == ABILITY_WIND_POWER)
                    charged |= 1u << ally;
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
            if (hp[partner] && !(acted & (1u << partner))
             && other->index != PAIR_IDLE && gAiLogicData->abilities[partner] != ABILITY_GOOD_AS_GOLD
             && !IsFixedDamageMove(other->executedMove)
             // A partner spending its own turn behind a shield, or on any
             // other status move, has no damage to multiply, and this pair is
             // choosing both actions together, so it knows that before it
             // commits: Blake's Meowstic helped a Reflect.
             && !IsBattleMoveStatus(other->executedMove))
                boost[partner] = boost[partner] * 3 / 2;
            else if (sign > 0 && !copy)
                // Nothing to boost: the partner is leaving, has already acted,
                // is guarding, or is holding damage the multiplier cannot
                // touch. Watching a
                // Helping Hand announce itself and fail into a partner that was
                // switching out is the turn this costs.
                score -= PAIR_SUPPORT_WASTED_COST;
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
        u32 newScreen = PairScreenMask(move);
        if (newScreen)
        {
            u32 side = GetBattlerSide(actor);
            if ((screens[side] & newScreen)
             || (newScreen == SIDE_STATUS_AURORA_VEIL && !(weather & B_WEATHER_ICY_ANY)))
            {
                // An earlier setter can invalidate a move that looked useful
                // on the original board; do not retain its positive opinion.
                if (sign > 0 && action->score > AI_SCORE_DEFAULT)
                    score -= (action->score - AI_SCORE_DEFAULT) * 4;
                continue;
            }
            if (!PairMayBeSnatched(actor, actions, hp, acted, stopped)
             && PairScreenEffectApplies(applyEffects, survival[actor] * actionChance[actor] / 10000, effectChance))
                screens[side] |= newScreen;
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
                PairApplyStatChanges(ev, actor, partner, move, statStage, statModifier, survival[actor], &usedItems);
                // This turn's damage/survival already values immediate stat
                // effects. The separate setup preference is future value,
                // earned only if the recipient survives the complete exchange.
                coachingValue[partner] += coachingOpinion * survival[actor] / 100 * actionChance[actor] / 10000;
            }
            continue;
        }
        if (PairSetupDance(move))
        {
            u32 sourceChance = actionChance[actor] / 100;
            if (gBattleMons[actor].status1 & STATUS1_PARALYSIS)
                sourceChance = min(sourceChance, ev->paralysisActionChance / 100);
            if (PairMayBeSnatched(actor, actions, hp, acted, stopped)
             || !PairScreenEffectApplies(applyEffects, sourceChance, effectChance))
                continue;
            bool32 succeeded = PairApplyDance(ev, actor, action->index, actor, statStage, statModifier,
                speedStage, speed, actions, &usedItems, hp);
            if (!succeeded)
            {
                if (sign > 0)
                    score -= max(0, action->planScore) + max(0, (action->score - AI_SCORE_DEFAULT) * 4);
                continue;
            }
            for (enum BattlerId dancer = 0; dancer < gBattlersCount; dancer++)
            {
                if (dancer == actor || gAiLogicData->abilities[dancer] != ABILITY_DANCER
                 || !PairDancerCanAct(dancer, move, hp, stopped, newSleepTargets, newTauntTargets))
                    continue;
                u32 copyChance = min(sourceChance, actionChance[dancer] / 100) * survival[actor] / 100;
                if (gBattleMons[dancer].status1 & STATUS1_PARALYSIS)
                    copyChance = copyChance * ev->paralysisActionChance / 10000;
                if (PairScreenEffectApplies(applyEffects, copyChance, effectChance))
                    // Dancer's extra move does not consume its chosen action.
                    // Opposing dancers copy too; do not award a free ally-only buff.
                    PairApplyDance(ev, actor, action->index, dancer, statStage, statModifier,
                        speedStage, speed, actions, &usedItems, hp);
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
        enum Type actualType = copy ? copy->type[!!(soaked & (1u << actor))]
            : effect == EFFECT_REVELATION_DANCE ? ev->revelationType[actor][!!(soaked & (1u << actor))] : GetMoveType(move);
        u32 actionBoost = boost[actor] * (copy
            ? ((newBurnTargets & (1u << actor)) && IsBattleMovePhysical(move) ? 50 : 100)
            : statusBoost[actor]) / 100;
        if (IsBattleMoveStatus(move) && !((move == MOVE_SOAK || PairPrimarySpeedDrop(move) || PairTargetDropStat(move) || move == MOVE_SPORE || move == MOVE_SING || move == MOVE_SLEEP_POWDER
            || move == MOVE_THUNDER_WAVE || move == MOVE_GLARE || move == MOVE_CHARM || move == MOVE_WILL_O_WISP || move == MOVE_QUASH || effect == EFFECT_TAUNT || effect == EFFECT_ENCORE) && applyEffects))
            continue;
        if (gBattleMoveEffects[effect].twoTurnEffect && !gBattleMons[actor].volatiles.multipleTurns
         && gAiLogicData->holdEffects[actor] != HOLD_EFFECT_POWER_HERB
         && !(effect == EFFECT_SOLAR_BEAM && (GetAttackerSunMoveWeather(gAiLogicData->holdEffects[actor], gAiLogicData->abilities[actor], weather) & B_WEATHER_SUN))
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
        u8 danceTargets[MAX_BATTLERS_COUNT] = {0};
        bool32 randomTarget = GetMoveTarget(move) == TARGET_RANDOM;
        u32 randomTargets = 0;
        if (randomTarget)
            for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                if (hp[foe] && !IsBattlerAlly(actor, foe))
                    randomTargets++;
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
        // Native Follow Me resolves before fainted-target fallback. Resolve it
        // once so the original and fallback paths cannot both hit the redirector.
        enum BattlerId follow = redirect[GetBattlerSide(selectedTarget)];
        bool32 followed = !copy && !randomTarget && !PairSpread(move) && !IsBattlerAlly(actor, selectedTarget)
            && follow < MAX_BATTLERS_COUNT && hp[follow]
            && !IsMoveRedirectionPrevented(actor, move, gAiLogicData->abilities[actor])
            && (!IsPowderMove(actions[follow].move)
                || IsAffectedByPowderMove(actor, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor]));
        if (followed)
            selectedTarget = follow;
        // A crash move whose targets an earlier action on this side already
        // removed does not simply do nothing: it takes half the user's maximum
        // off. Watching a High Jump Kick land on a body its partner's Surf had
        // removed, twice, is what this is: the retarget below cannot save it
        // when there is nothing left on that side to retarget to.
        if (!copy && GetMoveEffect(move) == EFFECT_RECOIL_IF_MISS && !PairSpread(move)
         && !IsBattlerAlly(actor, selectedTarget))
        {
            bool32 anyTarget = FALSE;
            for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                if (hp[foe] && !IsBattlerAlly(actor, foe)
                 && gBattleMons[foe].volatiles.semiInvulnerable != STATE_COMMANDER)
                    anyTarget = TRUE;
            if (!anyTarget)
            {
                u32 crash = max(1, PairNonDynamaxHP(ev, actor, gBattleMons[actor].maxHP) / 2);
                PairApplyDamageWhenActing(&hp[actor], &survival[actor], crash, crash, crash,
                    100, actionChance[actor]);
                continue;
            }
        }
        enum BattlerId fallback = GetPartnerBattler(selectedTarget);
        // HP is conditional on survival. Keep the original recipient's live
        // branch and send the already-fainted mass to its partner, as native
        // CancelerSetTargets does. Freeze this before applying the current hit:
        // this move cannot knock out one foe and then hit the other as well.
        u32 fallbackChance = !copy && !randomTarget && !retaliation && !followed && !PairSpread(move)
            && !IsBattlerAlly(actor, selectedTarget)
            && GetMoveTarget(move) != TARGET_OPPONENTS_FIELD
            ? (hp[selectedTarget] ? 100 - survival[selectedTarget] : 100) : 0;
        // The positive opinion was formed against the selected target. The
        // share of this hit that falls to its partner - because the selected
        // target is already gone - earns the partner's opinion of the same
        // move, not a knockout bonus on a body that was never there to hit:
        // Heatran's Magma Storm kept the 1 HP Metagross's knockout credit
        // while the hit landed, resisted, on Mega Tyranitar.
        s32 fallbackOpinion = 0;
        if (damageOpinion && fallbackChance && fallback < gBattlersCount)
            for (u32 choice = 0; choice < ev->count[actor]; choice++)
                if (ev->choices[actor][choice].index == action->index
                 && ev->choices[actor][choice].target == fallback)
                {
                    fallbackOpinion = max(0, (ev->choices[actor][choice].score - AI_SCORE_DEFAULT) * 4);
                    break;
                }
        for (enum BattlerId original = 0; original < gBattlersCount; original++)
        {
            if (original == actor || !hp[original]
             || gBattleMons[original].volatiles.semiInvulnerable == STATE_COMMANDER)
                continue;
            if (copy)
            {
                if (!copyWeights[original])
                    continue;
            }
            else if (randomTarget)
            {
                if (IsBattlerAlly(actor, original) || !randomTargets)
                    continue;
            }
            else if (PairSpread(move))
            {
                if (IsBattlerAlly(actor, original) && GetMoveTarget(move) == TARGET_BOTH)
                    continue;
            }
            else if (original != selectedTarget && (original != fallback || !fallbackChance))
                continue;
            enum BattlerId target = original;
            if (effect == EFFECT_SUCKER_PUNCH
             && ((acted & (1u << target)) || actions[target].index == PAIR_IDLE
                 // Native Sucker Punch reads the original selected slot, even
                 // when Encore will replace that command at execution time.
                 || IsBattleMoveStatus(ev->action[target].move)))
                continue;
            if (!MoveIgnoresProtect(move))
            {
                enum BattlerId guardUser = MAX_BATTLERS_COUNT;
                if ((protected & (1u << target)) && !AI_CanContactBypassProtect(actor, target, move))
                    guardUser = target;
                else if (PairSpread(move) && (wideGuard & (1u << GetBattlerSide(target))))
                    guardUser = sideGuardUser[GetBattlerSide(target)][0];
                else if (action->priority > 0 && !IsBattlerAlly(actor, target)
                      && (quickGuard & (1u << GetBattlerSide(target))))
                    guardUser = sideGuardUser[GetBattlerSide(target)][1];
                if (guardUser < MAX_BATTLERS_COUNT)
                {
                    if (guardUser < gBattlersCount && GetBattlerSide(guardUser) == ev->side
                     && !IsBattlerAlly(actor, target) && !IsBattleMoveStatus(action->executedMove))
                    {
                        const struct SimulatedDamage *blocked = &gAiLogicData->simulatedDmg[actor][target][action->index];
                        u32 accuracy = min(100, gAiLogicData->moveAccuracy[actor][target][action->index]);
                        u32 median = PairForecastDamage(ev, actor, target, action, hp[target], blocked->median);
                        guardDenied[guardUser] += (s32)(median * 180 / max(1, gBattleMons[target].maxHP)
                            * accuracy / 100 * survival[actor] / 100);
                        if (PairForecastDamage(ev, actor, target, action, hp[target], blocked->minimum) >= hp[target])
                            guardDenied[guardUser] += 80 * (s32)survival[actor] / 100;
                    }
                    continue;
                }
            }
            u32 guardHitChance = copy ? copyWeights[original] : randomTarget ? 100 / randomTargets
                : !PairSpread(move) && original != selectedTarget ? fallbackChance : 100;
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
            if (move == MOVE_SOAK)
            {
                if (!(ev->soakTargets[actor][action->index] & (1u << target)) || (soaked & (1u << target)))
                    continue;
                bool32 redirected = FALSE;
                if (!followed)
                    for (enum BattlerId other = 0; other < gBattlersCount; other++)
                        if (hp[other] && (ev->soakRedirectors[actor] & (1u << other)))
                            redirected = TRUE;
                if (redirected)
                    continue;
                u32 chance = min(100, gAiLogicData->moveAccuracy[actor][target][action->index])
                    * survival[actor] * guardHitChance / 10000 * actionChance[actor] / 10000;
                if (PairScreenEffectApplies(applyEffects, chance, effectChance))
                {
                    // Unlike Tailwind, Soak is absent from the ordinary pass;
                    // even a certain conversion must weight the changed pass.
                    *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                    soaked |= 1u << target;
                }
                continue;
            }
            if (effect == EFFECT_TAUNT)
            {
                if (!(ev->tauntTargets[actor][action->index] & (1u << target)))
                    continue;
                u32 chance = min(100, gAiLogicData->moveAccuracy[actor][target][action->index])
                    * survival[actor] * guardHitChance / 10000 * actionChance[actor] / 10000;
                if (!chance)
                    continue;
                *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                // An available Mental Herb cures before the pending move.
                // Track consumption so an earlier removal or second Taunt is
                // not incorrectly protected by the same item twice.
                if (B_MENTAL_HERB >= GEN_5 && gAiLogicData->holdEffects[target] == HOLD_EFFECT_MENTAL_HERB
                 && !(usedItems & (1u << target)))
                {
                    usedItems |= 1u << target;
                    if (gAiLogicData->abilities[target] == ABILITY_UNBURDEN && !gBattleMons[target].volatiles.unburdenActive)
                        speed[target] *= 2;
                    continue;
                }
                newTauntTargets |= 1u << target;
                continue;
            }
            if (move == MOVE_QUASH)
            {
                if (!(ev->quashTargets[actor][action->index] & (1u << target))
                 || (acted & (1u << target)))
                    continue;
                u32 chance = min(100, gAiLogicData->moveAccuracy[actor][target][action->index])
                    * survival[actor] * guardHitChance / 10000 * actionChance[actor] / 10000;
                if (!chance)
                    continue;
                *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                // Native GetMovePriorityInternal uses -8 for Quash. Apply only
                // after legal targeting/protection, never to an earlier action.
                actions[target].priority = -8;
                continue;
            }
            if (move == MOVE_WILL_O_WISP)
            {
                if (!(ev->burnTargets[actor][action->index] & (1u << target))
                 || ((newBurnTargets | newParalysisTargets | newSleepTargets) & (1u << target)))
                    continue;
                u32 chance = min(100, gAiLogicData->moveAccuracy[actor][target][action->index])
                    * survival[actor] * guardHitChance / 10000 * actionChance[actor] / 10000;
                if (!chance)
                    continue;
                newBurnTargets |= 1u << target;
                *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                // Apply at execution time, after actual targeting/Protect.
                // An attack already performed cannot be weakened retroactively.
                // Facade doubles its own power after this status lands. In
                // Gen 6+ it also ignores burn's reduction. Knowing Facade
                // never exempts the target's other physical attacks.
                if (!(acted & (1u << target)) && IsBattleMovePhysical(actions[target].executedMove))
                {
                    if (GetMoveEffect(actions[target].executedMove) == EFFECT_FACADE)
                    {
                        if (GetConfig(B_BURN_FACADE_DMG) >= GEN_6)
                            statusBoost[target] *= 2;
                    }
                    else
                        statusBoost[target] /= 2;
                }
                continue;
            }
            if (move == MOVE_THUNDER_WAVE || move == MOVE_GLARE)
            {
                if (!(ev->paralysisTargets[actor][action->index] & (1u << target))
                 || ((newBurnTargets | newParalysisTargets | newSleepTargets) & (1u << target)))
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
                // Status boosts a remaining Facade, not every move in its set.
                if (!(acted & (1u << target)) && GetMoveEffect(actions[target].executedMove) == EFFECT_FACADE)
                    statusBoost[target] *= 2;
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
                if (!chance || ((newBurnTargets | newSleepTargets | newParalysisTargets) & (1u << target))
                 || (ev->sleepClause && (newSleepSides & (1u << side))))
                    continue;
                // Landing sleep claims its target/clause even if that target
                // already acted or can use its chosen move while asleep.
                newSleepTargets |= 1u << target;
                newSleepSides |= 1u << side;
                chance = chance * min(100, gAiLogicData->moveAccuracy[actor][target][action->index]) * survival[actor] / 10000;
                chance = chance * guardHitChance / 100;
                if (chance)
                {
                    // Sleep still affects later turns if the target already
                    // acted (or used Sleep Talk). Keep that state's value even
                    // when there is no current action left to deny.
                    *effectChance = *effectChance ? min(*effectChance, chance) : chance;
                    if (!(acted & (1u << target)) && !IsUsableWhileAsleepEffect(GetMoveEffect(actions[target].move)))
                        stopped |= 1u << target;
                }
                continue;
            }
            // Thread shares targeting/protection and cached Speed handling,
            // not damage. Poison remains the native single-action opinion.
            u32 itemGone = !!(usedItems & (1u << actor));
            const struct PairDancerDamage *copyDamage = copy ? PairCopiedDamage(copy, actor, target, soaked, charged, usedItems) : NULL;
            u32 contactDamage = copy ? copy->contact[itemGone | (usedItems & (1u << target) ? 2 : 0)][target] : ev->contactDamage[actor][action->index][target];
            uq4_12_t effectiveness = copyDamage ? copyDamage->effectiveness : gAiLogicData->effectiveness[actor][target][action->index];
            struct SimulatedDamage damage = copyDamage ? copyDamage->damage
                : IsBattleMoveStatus(move) ? (struct SimulatedDamage){0} : gAiLogicData->simulatedDmg[actor][target][action->index];
            // An isolated KO/matchup bonus is not earned by attacking into
            // Protect. Credit it once, only after a real recipient survives
            // targeting and guard checks; spread moves may still hit a partner.
            if (damage.affectsTarget && damageOpinion)
            {
                if (!fallbackChance)
                {
                    score += damageOpinion * guardHitChance / 100;
                    damageOpinion = 0;
                }
                else if (target == selectedTarget)
                    score += damageOpinion * guardHitChance / 100 * (100 - fallbackChance) / 100;
                else
                    score += fallbackOpinion * guardHitChance / 100;
            }
            bool32 hpPowerAdjusted = !copy && actionHp != gBattleMons[actor].hp
                && (ev->hpPowerTargets[actor][action->index] & (1u << target));
            if (hpPowerAdjusted)
                damage = PairHpPowerDamage(ev, actor, target, action->index, move, actionHp);
            bool32 conditionalBoostAdjusted = !copy && (ev->conditionalBoostTargets[actor][action->index] & (1u << target));
            // Defeatist endpoints are native-rounded; threshold selection
            // uses conditional-alive aggregate HP. Roll/miss/healing branches
            // straddling half HP remain approximate, not an exact KO tree.
            if (conditionalBoostAdjusted)
                damage = PairConditionalBoostDamage(ev, actor, target, action->index,
                    gAiLogicData->abilities[actor] == ABILITY_DEFEATIST && gBattleMons[actor].item == ITEM_NONE
                    ? (actionHp > gBattleMons[actor].maxHP / 2 ? 100 : 0)
                    : (ev->itemBoost[actor] & (1u << action->index)) ? ((usedItems & (1u << actor)) ? 0 : 100)
                    : (hp[GetPartnerBattler(actor)] ? survival[GetPartnerBattler(actor)] : 0));
            u32 fieldState = (soaked & (1u << actor) ? PAIR_SOAK_ATTACKER : 0)
                | (soaked & (1u << target) ? PAIR_SOAK_TARGET : 0)
                | (charged & (1u << actor) ? PAIR_CHARGED : 0);
            u32 originalFieldState = gBattleMons[actor].volatiles.chargeTimer ? PAIR_CHARGED : 0;
            bool32 fieldAdjusted = !copy && fieldState != originalFieldState
                && (ev->soakChargeStates[actor][action->index][target] & (1u << fieldState));
            u32 rageState = fieldState & (PAIR_SOAK_ATTACKER | PAIR_SOAK_TARGET);
            u32 originalHits = min(6, GetBattlerPartyState(actor)->timesGotHit);
            bool32 rageAdjusted = effect == EFFECT_RAGE_FIST && ev->rage[actor] && rageHits[actor] != originalHits
                && (ev->rage[actor]->states[target] & (1u << rageState));
            if (fieldAdjusted || rageAdjusted)
            {
                struct SimulatedDamage changed = rageAdjusted ? ev->rage[actor]->damage[target][rageState][rageHits[actor]]
                    : ev->soakChargeDamage[actor][action->index][target][fieldState];
                if (hpPowerAdjusted || conditionalBoostAdjusted)
                {
                    // Compose the already-modeled HP/item/partner change with
                    // this native type/charge anchor. The ratio remains the
                    // bounded forecast, not an exact joint rounding tree.
                    const struct SimulatedDamage *base = rageAdjusted ? &ev->rage[actor]->damage[target][0][originalHits]
                        : &ev->soakChargeDamage[actor][action->index][target][originalFieldState];
                    if (base->minimum && base->median && base->maximum)
                    {
                        changed.minimum = min(65535u, (u32)changed.minimum * damage.minimum / base->minimum);
                        changed.median = min(65535u, (u32)changed.median * damage.median / base->median);
                        changed.maximum = min(65535u, (u32)changed.maximum * damage.maximum / base->maximum);
                    }
                }
                damage = changed;
            }
            const struct PairDefenderItemCache *itemCache = ev->defenderItem[actor][target];
            bool32 defenderItemAdjusted = !copy && (usedItems & (1u << target)) && itemCache != NULL
                && (itemCache->moves & (1u << action->index));
            if (defenderItemAdjusted)
            {
                struct SimulatedDamage changed = itemCache->damage[action->index * itemCache->count
                    + itemCache->stateIndex[fieldState & itemCache->mask]];
                if (hpPowerAdjusted || conditionalBoostAdjusted || rageAdjusted)
                {
                    const struct SimulatedDamage *base = ev->soakChargeStates[actor][action->index][target] & (1u << fieldState)
                        ? &ev->soakChargeDamage[actor][action->index][target][fieldState]
                        : &gAiLogicData->simulatedDmg[actor][target][action->index];
                    // Existing HP/partner/Rage combinations retain their bounded
                    // native-anchor ratio; item loss itself uses native rounding.
                    if (base->minimum && base->median && base->maximum)
                    {
                        changed.minimum = min(65535u, (u32)changed.minimum * damage.minimum / base->minimum);
                        changed.median = min(65535u, (u32)changed.median * damage.median / base->median);
                        changed.maximum = min(65535u, (u32)changed.maximum * damage.maximum / base->maximum);
                    }
                }
                damage = changed;
            }
            u32 accuracy = copy ? copy->accuracy[itemGone | (usedItems & (1u << target) ? 2 : 0)][target] : min(100, gAiLogicData->moveAccuracy[actor][target][action->index]);
            if (!copy && weather != ev->weather)
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
            if (lifeOrb && ((!copy && ev->drainPercent[actor][action->index][target])
             || (!singleContactOrb && contactDamage)
             || (stealsSitrus && !singleContactOrb)))
                lifeOrb = FALSE;
            if (lifeOrb && damage.affectsTarget)
            {
                // One payment if ANY recipient is affected, including allies
                // and Substitute/Disguise. Execution/paralysis is one shared
                // event and is applied once after the complete move below.
                orbHitChance = PairSpread(move)
                    ? 100 - (100 - orbHitChance) * (100 - affectedChance) / 100
                    : min(100, orbHitChance + affectedChance); // Fallback recipients are exclusive.
            }
            if (!copy && (ev->screenBreakerMoves[actor] & (1u << action->index)) && damage.maximum)
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
            if ((copy ? copy->screened[itemGone] : ev->screenTargets[actor][action->index]) & (1u << target))
            {
                u32 side = GetBattlerSide(target);
                screenNumerator = PairScreenFactor(move, screens[side]);
                screenDenominator = PairScreenFactor(move, gSideStatuses[side]);
            }
            u32 amount = damage.median * actionBoost / 100;
            u32 minimumDamage = damage.minimum * actionBoost / 100;
            u32 worstDamage = damage.maximum * actionBoost / 100;
            if (weather != ev->weather)
            {
                u32 weatherPower = PairWeatherPower(actor, target, move, actualType, weather);
                u32 oldWeatherPower = PairWeatherPower(actor, target, move, actualType, ev->weather);
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
                // The forecast fed the reflection with a hit this foe has
                // visibly not been answering this body with: its last hit on
                // it was the other category. That hit is the alternative,
                // not the expected pattern, and is priced as one.
                if (!copy && retaliation->source < gBattlersCount
                 && PairReflectEvidence(actor, retaliation->source, move) < 0)
                    accuracy = accuracy * PAIR_FORECAST_ALTERNATE / 100;
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
                    actionChance[actor], actionBoost, statPower, screenNumerator, screenDenominator,
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
            // A foe's absorbing ability turns the hit into its own gain. A
            // spread move still reaches the other recipient, so it is not a
            // failed move - but Discharge healing a Volt Absorb body and
            // Eruption lighting a Flash Fire were being scored as free.
            bool32 foeAbsorbs = !copy && !IsBattlerAlly(actor, target) && !IsBattleMoveStatus(move);
            if ((copy || IsBattlerAlly(actor, target) || foeAbsorbs) && damage.maximum == 0)
            {
                enum Type type = actualType;
                bool32 boosted = FALSE;
                if ((targetAbility == ABILITY_STORM_DRAIN && type == TYPE_WATER)
                 || (targetAbility == ABILITY_LIGHTNING_ROD && type == TYPE_ELECTRIC))
                {
                    PairChangeStat(&statStage[target][3], &statModifier[target][3], 1, 100, FALSE);
                    boosted = TRUE;
                }
                else if (targetAbility == ABILITY_MOTOR_DRIVE && type == TYPE_ELECTRIC)
                {
                    if (!(acted & (1u << target)))
                        speed[target] = speed[target] * PairChangeStage(&speedStage[target], 1) / 100;
                    boosted = TRUE;
                }
                else if ((targetAbility == ABILITY_WATER_ABSORB && type == TYPE_WATER)
                      || (targetAbility == ABILITY_DRY_SKIN && type == TYPE_WATER)
                      || (targetAbility == ABILITY_VOLT_ABSORB && type == TYPE_ELECTRIC)
                      || (targetAbility == ABILITY_EARTH_EATER && type == TYPE_GROUND))
                {
                    if (!gBattleMons[target].volatiles.healBlockTimer)
                        hp[target] = min(gBattleMons[target].maxHP, hp[target] + gBattleMons[target].maxHP / 4);
                }
                else if ((targetAbility == ABILITY_SAP_SIPPER && type == TYPE_GRASS)
                      || (targetAbility == ABILITY_FLASH_FIRE && type == TYPE_FIRE)
                      || (targetAbility == ABILITY_WELL_BAKED_BODY && type == TYPE_FIRE)
                      || (targetAbility == ABILITY_WIND_RIDER && IsWindMove(move)))
                {
                    boosted = TRUE;
                }
                // The heal is already in the foe's HP. A boost is the same
                // next-turn value a setup move buys, handed to the other side.
                if (foeAbsorbs && boosted)
                    score -= sign * PAIR_SETUP_HORIZON * (s32)(accuracy * survival[actor] / 100) / 100;
            }
            bool32 singleHit = GetMoveStrikeCount(move) <= 1 && !IsMultiHitMove(move)
                && effect != EFFECT_BEAT_UP && gAiLogicData->abilities[actor] != ABILITY_PARENTAL_BOND;
            if (worstDamage >= hp[target] && singleHit
             && (((copy || hpPowerAdjusted || conditionalBoostAdjusted || fieldAdjusted || rageAdjusted || defenderItemAdjusted) && gBattleMons[target].volatiles.endured)
                 || (hp[target] == gBattleMons[target].maxHP
                     && ((gAiLogicData->holdEffects[target] == HOLD_EFFECT_FOCUS_SASH && !(usedItems & (1u << target))
                          && !IsFocusSashBypassed(move, gAiLogicData->abilities[target]))
                 || AI_GetMoldBreakerSanitizedAbility(actor, gAiLogicData->abilities[actor], gAiLogicData->abilities[target],
                     gAiLogicData->holdEffects[target], move) == ABILITY_STURDY))))
            {
                amount = min(amount, hp[target] - 1);
                minimumDamage = min(minimumDamage, hp[target] - 1);
                worstDamage = hp[target] - 1;
            }
            u32 damageHitChance = accuracy * survival[actor] / 100;
            u32 hitChance = damageHitChance * actionChance[actor] / 10000;
            if (!copy && PairCopiedDance(move) < ARRAY_COUNT(sCopiedDances) && damage.affectsTarget)
                danceTargets[target] = min(100, danceTargets[target] + hitChance * survival[target] / 100);
            if ((ev->rageActors & (1u << target)) && damage.affectsTarget && !IsBattleMoveStatus(move)
             && !DoesSubstituteBlockMove(actor, target, move)
             && PairScreenEffectApplies(applyEffects, hitChance, effectChance))
            {
                u32 hits = copy ? 1 : ev->rageHitCount[actor][action->index][target];
                if (effect == EFFECT_BEAT_UP)
                    for (enum BattlerId member = 0; member < gBattlersCount; member++)
                        if (ev->board.owner[member] == ev->board.owner[actor] && gBattleMons[member].hp
                         && !gBattleMons[member].status1 && (!hp[member]
                             || ((newBurnTargets | newParalysisTargets | newSleepTargets) & (1u << member))))
                            hits = hits ? hits - 1 : 0;
                u32 contact = contactDamage;
                if (contact)
                    hits = min(hits, max(1, (hp[actor] + contact - 1) / contact));
                rageHits[target] = min(6, rageHits[target] + hits);
            }
            if (hp[target] > worstDamage && damage.affectsTarget
             && (gAiLogicData->abilities[target] == ABILITY_ELECTROMORPHOSIS
                 || (IsWindMove(move) && gAiLogicData->abilities[target] == ABILITY_WIND_POWER))
             && !DoesSubstituteBlockMove(actor, target, move)
             && PairScreenEffectApplies(applyEffects, hitChance, effectChance))
                charged |= 1u << target;
            // A Focus Sash that stops a knockout is spent by it: a second hit
            // this turn meets no Sash (the defender-item anchors), and what
            // it kept standing is one hit from falling (sashBroken below).
            if (!(usedItems & (1u << target)) && damage.affectsTarget
             && gAiLogicData->holdEffects[target] == HOLD_EFFECT_FOCUS_SASH
             && hp[target] == gBattleMons[target].maxHP
             && (damage.consumedItem & AI_ITEM_CONSUMED_MEDIAN)
             && !DoesSubstituteBlockMove(actor, target, move)
             && PairScreenEffectApplies(applyEffects, hitChance, effectChance))
            {
                usedItems |= 1u << target;
                sashBroken |= 1u << target;
            }
            if (!(usedItems & (1u << target)) && damage.affectsTarget
             && gAiLogicData->holdEffects[target] == HOLD_EFFECT_RESIST_BERRY
             && (damage.consumedItem & AI_ITEM_CONSUMED_MEDIAN)
             && !DoesSubstituteBlockMove(actor, target, move)
             && PairScreenEffectApplies(applyEffects, hitChance, effectChance))
            {
                usedItems |= 1u << target;
                if (gAiLogicData->abilities[target] == ABILITY_UNBURDEN && !gBattleMons[target].volatiles.unburdenActive)
                    speed[target] *= 2;
            }
            u32 medianDamage = amount;
            amount = min(amount, hp[target]) * hitChance / 100;
            if (hp[target] > worstDamage && amount
             && gAiLogicData->abilities[actor] != ABILITY_MOLD_BREAKER
             && gAiLogicData->abilities[actor] != ABILITY_TERAVOLT
             && gAiLogicData->abilities[actor] != ABILITY_TURBOBLAZE
             && gAiLogicData->abilities[target] == ABILITY_JUSTIFIED && actualType == TYPE_DARK)
            {
                PairChangeStat(&statStage[target][0], &statModifier[target][0],
                    effect == EFFECT_BEAT_UP ? AI_GetBeatUpHitCount(actor) : 1, 100, FALSE);
            }
            // Hit-triggered boosts belong to either side. Hitting an opposing
            // Sturdy/Policy recipient can enable its reply inside this trial.
            if (hp[target] > worstDamage && amount)
            {
                // Anger Point turns a critical hit into maximum Attack before
                // the body's own action. An always-critical move is certain
                // to trigger it, so a slower reply in this same trial lands
                // at +6. Without this the trial priced Aisha's Storm Throw
                // into Tauros as pure self-damage, and the knockout it sets
                // up that very turn lost to throwing the foe instead.
                if (gAiLogicData->abilities[target] == ABILITY_ANGER_POINT && target != actor
                 && statStage[target][0] < MAX_STAT_STAGE
                 && !DoesSubstituteBlockMove(actor, target, move)
                 && AI_MoveAlwaysCrits(actor, target, move))
                    PairChangeStat(&statStage[target][0], &statModifier[target][0],
                        MAX_STAT_STAGE - statStage[target][0], hitChance, FALSE);
                if (gAiLogicData->holdEffects[target] == HOLD_EFFECT_WEAKNESS_POLICY
                 && !(usedItems & (1u << target))
                 && effectiveness > UQ_4_12(1.0))
                {
                    usedItems |= 1u << target;
                    s32 gain = GetAdjustedStatStage(2, gAiLogicData->abilities[target], FALSE);
                    PairChangeStat(&statStage[target][0], &statModifier[target][0], gain, 100, FALSE);
                    PairChangeStat(&statStage[target][3], &statModifier[target][3], gain, 100, FALSE);
                }
                if (gAiLogicData->abilities[target] == ABILITY_STEAM_ENGINE
                 && (actualType == TYPE_FIRE || actualType == TYPE_WATER)
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
            bool32 contactMayHeal = contactDamage
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
            if (!copy && target != actor && IsBattlerAlly(actor, target) && GetBattlerSide(actor) == ev->side
             && PairSpread(move) && beforeHitHp * beforeHitSurvival > hp[target] * survival[target])
                friendlyLoss[target] += (beforeHitHp * beforeHitSurvival - hp[target] * survival[target]) / 100;
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
             && !((newBurnTargets | newParalysisTargets | newSleepTargets) & (1u << target)))
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
                    if (!(acted & (1u << target)) && GetMoveEffect(actions[target].executedMove) == EFFECT_FACADE)
                        statusBoost[target] *= 2;
                    speed[target] = speed[target] * ev->paralyzedSpeed[target] / max(1, gAiLogicData->speedStats[target]);
                    if (!(acted & (1u << target)) && !IsBattleMoveStatus(actions[target].executedMove)
                     && !(B_MAGIC_GUARD == GEN_4 && gAiLogicData->abilities[target] == ABILITY_MAGIC_GUARD))
                        actionChance[target] = ev->paralysisActionChance;
                }
                // Native secondary status lands before Helmet/Rough Skin can
                // faint the user. Keep ordinary damage and contact payment.
            }
            u32 drainPercent = copy ? 0 : ev->drainPercent[actor][action->index][target];
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
                && damage.affectsTarget))
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
                    afterHit -= min(afterHit, contactDamage);
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
                     || (effect == EFFECT_KNOCK_OFF && gBattleMons[target].item != ITEM_NONE))
                 && targetAbility != ABILITY_STICKY_HOLD && !DoesSubstituteBlockMove(actor, target, move)
                 && (move == MOVE_INCINERATE || move == MOVE_BUG_BITE || move == MOVE_PLUCK
                     || (effect == EFFECT_KNOCK_OFF
                         && !(B_KNOCK_OFF_REMOVAL >= GEN_5 && IsOnPlayerSide(target) && !(gBattleTypeFlags & BATTLE_TYPE_TRAINER))
                         && CanBattlerGetOrLoseItem(target, actor, gBattleMons[target].item)))
                 && PairScreenEffectApplies(applyEffects, hitChance, effectChance))
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
            if ((move == MOVE_FIERY_DANCE || move == MOVE_AQUA_STEP) && damage.affectsTarget
             && !IsSheerForceAffected(move, gAiLogicData->abilities[actor]))
            {
                const struct AdditionalEffect *additional = GetMoveAdditionalEffectById(move, 0);
                u32 chance = hitChance * CalcSecondaryEffectChance(actor, gAiLogicData->abilities[actor], move, additional) / 100;
                if (PairScreenEffectApplies(applyEffects, chance, effectChance))
                {
                    enum Stat stat = move == MOVE_FIERY_DANCE ? STAT_SPATK : STAT_SPEED;
                    s32 delta = GetAdjustedStatStage(GetStatStage(stat, additional), gAiLogicData->abilities[actor], FALSE);
                    u32 old = stat == STAT_SPEED ? speedStage[actor] : statStage[actor][3];
                    u32 next = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE, (s32)old + delta));
                    if (next < DEFAULT_STAT_STAGE && gAiLogicData->holdEffects[actor] == HOLD_EFFECT_WHITE_HERB
                     && !(usedItems & (1u << actor)))
                    {
                        next = DEFAULT_STAT_STAGE;
                        usedItems |= 1u << actor;
                    }
                    if (stat == STAT_SPEED)
                    {
                        speed[actor] = speed[actor] * gStatStageRatios[next][0] * gStatStageRatios[old][1]
                            / (gStatStageRatios[next][1] * gStatStageRatios[old][0]);
                        speedStage[actor] = next;
                    }
                    else
                        PairChangeStat(&statStage[actor][3], &statModifier[actor][3], (s32)next - old, 100, FALSE);
                }
            }
            u32 targetDropStat = PairTargetDropStat(move);
            s32 targetDropDelta = copy ? copy->targetDropDelta[!!(usedItems & (1u << target))][target] : ev->targetDropDelta[actor][action->index][target];
            if (applyEffects && targetDropStat && hp[target] && (IsBattleMoveStatus(move) || (damage.maximum && amount))
             && targetDropDelta)
            {
                *effectChance = *effectChance ? min(*effectChance, hitChance) : hitChance;
                u8 next[4];
                memcpy(next, statStage[target], sizeof(next));
                u32 dropColumn = targetDropStat == STAT_ATK ? 0 : targetDropStat == STAT_SPDEF ? 2 : 3;
                next[dropColumn] = max(MIN_STAT_STAGE, min(MAX_STAT_STAGE,
                    (s32)next[dropColumn] + targetDropDelta));
                if (!copy && move == MOVE_FEATHER_DANCE && next[dropColumn] != statStage[target][dropColumn])
                    danceTargets[target] = min(100, danceTargets[target] + hitChance * survival[target] / 100);
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
             && (PairPrimarySpeedDrop(move)
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
                    if (lowered && !IsBattlerAlly(actor, target))
                    {
                        if (gAiLogicData->abilities[target] == ABILITY_DEFIANT)
                            PairChangeStat(&statStage[target][0], &statModifier[target][0], 2, 100, FALSE);
                        else if (gAiLogicData->abilities[target] == ABILITY_COMPETITIVE)
                            PairChangeStat(&statStage[target][3], &statModifier[target][3], 2, 100, FALSE);
                    }
                }
            }
            if (move == MOVE_FAKE_OUT && amount && gBattleStruct->battlerState[actor].isFirstTurn
             && gAiLogicData->abilities[target] != ABILITY_INNER_FOCUS
             && gAiLogicData->abilities[target] != ABILITY_SHIELD_DUST
             && gAiLogicData->holdEffects[target] != HOLD_EFFECT_COVERT_CLOAK)
                stopped |= 1u << target;
            if (!contactPaid && singleHit && (contactDamage || recoilMaximum) && damage.affectsTarget)
            {
                // Contact, damage-based recoil and Orb belong to one landed
                // hit. Apply their combined range once so a missed attack
                // cannot subsequently kill its conditional surviving user.
                // Native contact precedes move recoil, then Orb; with no
                // intervening healing, summing costs preserves final HP even
                // when an earlier cost already faints the attacker.
                u32 contact = contactDamage;
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
        if (B_CHARGE >= GEN_9 && (copy ? actualType == TYPE_ELECTRIC
            : (ev->electricMoves[actor][!!(soaked & (1u << actor))] & (1u << action->index))))
            charged &= ~(1u << actor);
        if (!copy)
        {
            u32 dance = PairCopiedDance(move);
            u32 chance = 0;
            for (u32 target = 0; target < gBattlersCount; target++)
                chance += danceTargets[target];
            if (dance < ARRAY_COUNT(sCopiedDances) && chance)
            {
                danceSource = actor;
                queuedDance = move;
                memcpy(queuedTargets, danceTargets, sizeof(queuedTargets));
                for (enum BattlerId receiver = 0; receiver < gBattlersCount; receiver++)
                    if (receiver != actor && hp[receiver] && ev->dancer[receiver][dance] != NULL)
                        pendingDancers |= 1u << receiver;
            }
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
        // The end of the turn is the board the next one starts from: a body
        // its residual damage finishes is gone, whatever it kept until then.
        // Phoebe's Mega Banette shielded at 9 HP in the sand that ended it.
        if (hp[actor] && ev->residual[actor] >= hp[actor])
            hp[actor] = 0;
        if (EC_PerishMustEscape(actor))
        {
            // Count before applying ANY countdown deaths. A lower-index foe
            // perishing does not cancel this actor's own pending deadline.
            u32 foeSide = GetBattlerSide(actor) ^ 1;
            bool32 hasFoe = (livingSides & (1u << foeSide)) || ev->board.reserveValue[foeSide] > 0;
            if (hasFoe)
                hp[actor] = 0;
        }
        // A healing Berry still in hand is HP its holder has yet to use. The
        // one-turn board saw only the heal of the hit that triggered it, so a
        // hit pushing a Sitrus holder under half looked a quarter smaller
        // than the same hit on anything else: Winona's Zapdos answered a
        // Sitrus Incineroar with a non-STAB Heat Wave, and Sidney's
        // Incineroar kept its Flare Blitz off a fresh Sitrus Buzzwole.
        u32 standingHp = hp[actor];
        if (standingHp && ev->berryHeal[actor] && !(usedItems & (1u << actor)))
            standingHp += ev->berryHeal[actor];
        s32 value = PairMonValue(&ev->board, ev->board.owner[actor], standingHp, gBattleMons[actor].maxHP)
            * survival[actor] / 100;
        if (hp[actor])
            score += coachingValue[actor] * survival[actor] / 100;
        if (hp[actor] && (choiceStarted & (1u << actor))
         && ((livingSides >> (GetBattlerSide(actor) ^ 1)) & 1u)
         && (!(usedItems & (1u << actor)) || gAiLogicData->abilities[actor] == ABILITY_GORILLA_TACTICS))
        {
            // Our own lock is worth what it still threatens next turn: the
            // move's hit on the foe it hurts most among those this trial
            // leaves standing. Not damage through Protect or a fictitious KO -
            // a first lock is not settled by every current target guarding -
            // and not a foe this turn removes: Tate & Liza's Specs Tapu Lele
            // took Moonblast for the sure knockout on Incineroar with Focus
            // Blast, super effective on both, in hand, and sat two turns in a
            // lock Heatran walled. The foes' forecast keeps the small
            // tie-break value: it asks what they do this turn, and the lock
            // they walk into is theirs to escape.
            u32 lockValue = ev->firstChoiceValue[actor][actions[actor].index];
            if (GetBattlerSide(actor) == ev->side)
            {
                u32 threat = 0;
                for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                    if (hp[foe] && !IsBattlerAlly(actor, foe))
                        threat = max(threat, ev->firstChoiceShare[actor][actions[actor].index][foe] * survival[foe] / 100);
                lockValue = PAIR_CHOICE_LOCK_HORIZON * threat / 100;
            }
            // Signed before it meets the unsigned survival: a foe's value is
            // subtracted, and letting the -1 wrap made every board a
            // Choice-holding foe could reach worth ~42 million, which the
            // forecast weighting then overflowed below the -10000 vetoes.
            score += (GetBattlerSide(actor) == ev->side ? 1 : -1)
                * (s32)(lockValue * survival[actor] / 100);
        }
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
    // Our own spread hit on our partner is a real cost at the full HP scale,
    // whatever happens to the partner afterwards. The end-of-turn value only
    // counts it while the partner is still standing, and an authored pressure
    // plan counts HP at a quarter - meant for trading chip with the foes, not
    // with each other. Roxanne's Tyrantrum Earthquaked its 57% Carbink with a
    // Fire Fang that hit both foes super effectively in hand, because Seed
    // Flare was forecast to finish the Carbink anyway. What the end state
    // already charges for the same HP is not charged twice; an immune,
    // shielded or absorbing partner lost nothing and pays nothing.
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        if (!friendlyLoss[target] || GetBattlerSide(target) != ev->side)
            continue;
        u32 credited = hp[target] ? ev->board.healthValue[ev->board.owner[target]] * survival[target] / 100 : 0;
        score -= (s32)(friendlyLoss[target] * (100 - min(100, credited)) / max(1, gBattleMons[target].maxHP));
    }
    // A spent Focus Sash is next-turn value the one-turn board cannot see: the
    // HP its holder kept only because of the Sash is gone at the next hit. A
    // body the Sash left at 1 HP has already paid that in HP; a 1 HP Shedinja
    // has paid nothing. Without this, breaking a Shedinja's Sash scored as a
    // wasted turn, and Wattson's Mega Manectric shielded seven turns running
    // against a lone Shedinja rather than Flamethrower it twice.
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
        if ((sashBroken & (1u << target)) && hp[target])
            score += (GetBattlerSide(target) == ev->side ? -1 : 1)
                * (s32)(ev->board.healthValue[ev->board.owner[target]] * hp[target] / max(1, gBattleMons[target].maxHP)
                    * survival[target] / 100);
    // Whose self-boost this trial leaves nothing to cash: see PairBoostOutrun.
    u32 outrun = 0;
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if (hp[actor] && PairBoostOutrun(actor, &actions[actor], hp, speed, speedStage, trickRoom))
            outrun |= 1u << actor;
    // A shield's saving is only partly permanent. Without a payoff the same
    // threat returns next turn and the foes can focus the unprotected ally, so
    // bank only the share a payoff makes real.
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        if (GetBattlerSide(actor) != ev->side)
            continue;
        enum BattlerId partner = GetPartnerBattler(actor);
        s32 partnerBoostStages;
        bool32 partnerPaid = (executed & (1u << partner))
            && !((PairSelfBoostRaises(partner, actions[partner].executedMove, &partnerBoostStages) || partnerBoostStages)
                 && (!hp[partner] || (outrun & (1u << partner))));
        // A shield on a body the end of the turn finishes saved nothing.
        if (guardDenied[actor] && !hp[actor] && ev->residual[actor])
            guardDenied[actor] = 0;
        if (guardDenied[actor])
            score -= guardDenied[actor] * (s32)(100 - PairGuardBankedShare(ev, actor, actions, guardDenied[actor], partnerPaid)) / 100;
        else if ((guardUsed & (1u << actor)) && !ev->waitingPayoff
              && !PairGuardPartnerPayoff(actor, actions, partnerPaid))
            score -= PAIR_GUARD_EMPTY_COST;
    }
    if (!ev->wholeTurnPayoff)
    {
        for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        {
            enum BattlerId partner = GetPartnerBattler(actor);
            if (GetBattlerSide(actor) != ev->side || !(guardUsed & (1u << actor))
             || !IsBattlerAlive(partner) || !(guardUsed & (1u << partner)))
                continue;
            score -= PAIR_GUARD_CORRELATED_COST;
            break;
        }
    }
    // A self-boost's plan and opinion reward, in the branches where its user
    // lives to use the boost, at what the raise still adds (PairBoostWorth)
    // and, for a purely defensive raise, as far as its threats stay standing.
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
        if (setupReward[actor] && hp[actor] && !(outrun & (1u << actor)))
            score += setupReward[actor] * (s32)survival[actor] / 100
                * (s32)PairBoostWorth(actor, actions[actor].executedMove) / 100
                * (s32)PairDefenseBoostShare(actor, actions[actor].executedMove, hp, survival) / 100;
    // Next-turn value that a one-turn board cannot see. Each term is paid only
    // when the effect actually landed in this trial and its owner or victim is
    // still standing at the end of it, so a boost that gets its user killed and
    // a sleep on something that faints anyway are both worth nothing.
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        const struct PairAction *action = &actions[actor];
        s32 sign = GetBattlerSide(actor) == ev->side ? 1 : -1;
        if (action->index == PAIR_IDLE || !hp[actor] || !IsStatRaisingMove(action->move)
         || (outrun & (1u << actor)))
            continue;
        bool32 raised = speedStage[actor] > gBattleMons[actor].statStages[STAT_SPEED];
        static const u8 stats[4] = {STAT_ATK, STAT_DEF, STAT_SPDEF, STAT_SPATK};
        for (u32 stat = 0; stat < ARRAY_COUNT(stats); stat++)
            if (statStage[actor][stat] > gBattleMons[actor].statStages[stats[stat]])
                raised = TRUE;
        // An offensive boost with nothing to cash it is not a boost. A Howl on
        // a body whose only attack scales off the other stat, or off neither,
        // buys the turn nothing at all.
        if (raised && statStage[actor][0] > gBattleMons[actor].statStages[STAT_ATK]
         && !PairAttacksPhysically(actor))
            raised = speedStage[actor] > gBattleMons[actor].statStages[STAT_SPEED]
                || statStage[actor][1] > gBattleMons[actor].statStages[STAT_DEF]
                || statStage[actor][2] > gBattleMons[actor].statStages[STAT_SPDEF]
                || statStage[actor][3] > gBattleMons[actor].statStages[STAT_SPATK];
        // The same rule on the defensive half: a Defense boost with nothing
        // physical on the other side, or a Special Defense boost with nothing
        // special, mitigates damage that is not coming. A Cotton Guard against
        // a purely special board was still collecting the full horizon.
        if (raised && !(speedStage[actor] > gBattleMons[actor].statStages[STAT_SPEED])
         && !(statStage[actor][0] > gBattleMons[actor].statStages[STAT_ATK])
         && !(statStage[actor][3] > gBattleMons[actor].statStages[STAT_SPATK]))
        {
            bool32 useful = FALSE;
            if (statStage[actor][1] > gBattleMons[actor].statStages[STAT_DEF]
             && PairFacesCategory(actor, DAMAGE_CATEGORY_PHYSICAL))
                useful = TRUE;
            if (statStage[actor][2] > gBattleMons[actor].statStages[STAT_SPDEF]
             && PairFacesCategory(actor, DAMAGE_CATEGORY_SPECIAL))
                useful = TRUE;
            raised = useful;
        }
        if (raised)
            score += sign * PAIR_SETUP_HORIZON * (s32)survival[actor] / 100
                * (s32)PairBoostWorth(actor, action->executedMove) / 100
                * (s32)PairDefenseBoostShare(actor, action->executedMove, hp, survival) / 100;
    }
    for (enum BattlerId target = 0; target < gBattlersCount; target++)
    {
        // Credited to the side that did not receive it.
        s32 sign = GetBattlerSide(target) == ev->side ? -1 : 1;
        if (!hp[target])
            continue;
        if (newSleepTargets & (1u << target))
            score += sign * PAIR_SLEEP_HORIZON;
        if (newParalysisTargets & (1u << target))
            score += sign * PAIR_STATUS_HORIZON;
        if (newBurnTargets & (1u << target))
        {
            score += sign * PAIR_STATUS_HORIZON;
            if (PairAttacksPhysically(target))
                score += sign * PAIR_BURN_PHYSICAL_HORIZON;
        }
        if (newTauntTargets & (1u << target))
            score += sign * PAIR_STATUS_HORIZON;
        static const u8 stats[4] = {STAT_ATK, STAT_DEF, STAT_SPDEF, STAT_SPATK};
        for (u32 stat = 0; stat < ARRAY_COUNT(stats); stat++)
            if (statStage[target][stat] < gBattleMons[target].statStages[stats[stat]])
                score += sign * PAIR_STAT_DROP_HORIZON
                    * (s32)(gBattleMons[target].statStages[stats[stat]] - statStage[target][stat]);
        if (speedStage[target] < gBattleMons[target].statStages[STAT_SPEED])
        {
            score += sign * PAIR_STAT_DROP_HORIZON
                * (s32)(gBattleMons[target].statStages[STAT_SPEED] - speedStage[target]);
            // A drop that puts one of ours in front of it is speed control,
            // not chip: that is the whole point of a slow partner's turn.
            for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
                if (hp[ally] && GetBattlerSide(ally) != GetBattlerSide(target)
                 && gAiLogicData->speedStats[ally] <= gAiLogicData->speedStats[target]
                 && speed[ally] > speed[target])
                {
                    score += sign * PAIR_SPEED_CROSSING_HORIZON;
                    break;
                }
        }
    }
    for (enum BattlerId actor = 0; actor < gBattlersCount; actor++)
    {
        enum BattlerId recipient = GetPartnerBattler(actor);
        if (!tacticReward[actor] || !hp[recipient])
            continue;
        // An activation that changes nothing is a spent turn. If the recipient
        // already knocks its target out unboosted, the trigger is not needed.
        const struct PairAction *reply = &actions[recipient];
        if (reply->index != PAIR_IDLE && !IsBattleMoveStatus(reply->executedMove)
         && !IsBattlerAlly(recipient, reply->target) && IsBattlerAlive(reply->target)
         && gAiLogicData->simulatedDmg[recipient][reply->target][reply->index].minimum
            >= gBattleMons[reply->target].hp)
            continue;
        score += tacticReward[actor];
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
         && (PairScreenMask(action->move)
             || GetMoveEffect(action->move) == EFFECT_WEATHER
             || GetMoveEffect(action->move) == EFFECT_TAILWIND
             || GetMoveEffect(action->move) == EFFECT_TRICK_ROOM
             || action->move == MOVE_SOAK
             || PairSetupDance(action->move)
             || PairHasCopiedDance(ev, actor, action->executedMove)
             || action->executedMove == MOVE_FIERY_DANCE || action->executedMove == MOVE_AQUA_STEP
             || (IsWindMove(action->executedMove) && ev->chargeable)
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
        if (action->index != PAIR_IDLE && (ev->burnTargets[actor][action->index]
            || ev->quashTargets[actor][action->index] || ev->tauntTargets[actor][action->index]))
            hasEffect = TRUE;
        if (action->index != PAIR_IDLE && action->move == MOVE_FEINT)
            hasEffect = TRUE;
        if (action->index != PAIR_IDLE && !IsBattleMoveStatus(action->executedMove))
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if ((ev->defenderItem[actor][target] != NULL
                     || (ev->rageActors & (1u << target)) || gAiLogicData->abilities[target] == ABILITY_STAMINA
                     || gAiLogicData->abilities[target] == ABILITY_ELECTROMORPHOSIS)
                 && gAiLogicData->simulatedDmg[actor][target][action->index].maximum)
                    hasEffect = TRUE;
        if (action->index != PAIR_IDLE && GetMoveEffect(action->move) == EFFECT_KNOCK_OFF)
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (ev->itemBoost[target] && gAiLogicData->simulatedDmg[actor][target][action->index].maximum)
                    hasEffect = TRUE;
        if (action->index != PAIR_IDLE && PairTargetDropStat(action->move))
            for (enum BattlerId target = 0; target < gBattlersCount; target++)
                if (ev->targetDropDelta[actor][action->index][target]
                 && (IsBattleMoveStatus(action->move) || gAiLogicData->simulatedDmg[actor][target][action->index].maximum))
                    hasEffect = TRUE;
        if (action->index == PAIR_IDLE || (action->move != MOVE_ELECTROWEB && action->move != MOVE_ICY_WIND && action->move != MOVE_ROCK_TOMB
            && !PairPrimarySpeedDrop(action->move) && action->move != MOVE_SPORE && action->move != MOVE_SING
            && action->move != MOVE_SLEEP_POWDER))
            continue;
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
            if (((action->move == MOVE_SPORE || action->move == MOVE_SING || action->move == MOVE_SLEEP_POWDER)
                 && ev->sleepDenialChance[actor][action->index][target])
             || (ev->speedDelta[actor][action->index][target]
                 && (PairPrimarySpeedDrop(action->move) || gAiLogicData->simulatedDmg[actor][target][action->index].maximum)))
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

// A body switching in meets whatever the foes' visible Megas hit it with.
// Trainers see the stone, and a foe that can Mega almost always does so at
// once (AI_IsMoveLikelyToFailAfterFoeMega), yet the board prices the foes in
// their current form: Lilycove Brendan's Salamence came in on the Moonblast
// of a Gardevoir about to become Mega Gardevoir, and its Pixilate Hyper Voice
// removed the ace before it moved. Each foe's hit on the arriving body is the
// harder of the two forms; the foe's command is still never read.
static void PairPriceFoeMegaOnEntry(u32 noActionMask)
{
    for (enum BattlerId entering = 0; entering < gBattlersCount; entering++)
    {
        if (!(noActionMask & (1u << entering)) || !IsBattlerAlive(entering))
            continue;
        for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        {
            if (!IsBattlerAlive(foe) || IsBattlerAlly(entering, foe) || !CanMegaEvolve(foe))
                continue;
            enum Move *moves = GetMovesArray(foe);
            struct AiCalcValues calc = {.gimmickAtk = GIMMICK_MEGA, .gimmickDef = GIMMICK_NONE,
                .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
            {
                if (moves[index] == MOVE_NONE || IsBattleMoveStatus(moves[index])
                 || IsMoveUnusable(index, moves[index], gAiLogicData->moveLimitations[foe]))
                    continue;
                calc.move = moves[index];
                calc.typeEffectiveness = Q_4_12(0.0);
                calc.populationBomb = NULL;
                struct SimulatedDamage mega = AI_CalcDamage(&calc, foe, entering);
                if (mega.median > gAiLogicData->simulatedDmg[foe][entering][index].median)
                {
                    gAiLogicData->simulatedDmg[foe][entering][index] = mega;
                    gAiLogicData->effectiveness[foe][entering][index] = calc.typeEffectiveness;
                }
            }
        }
    }
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
    PairPriceFoeMegaOnEntry(noActionMask);
    return TRUE;
}

enum
{
    PAIR_VISIBLE_FORECAST_NONE,
    PAIR_VISIBLE_FORECAST_CAPTURE,
    PAIR_VISIBLE_FORECAST_USE,
};

// The foe forecast of the board the opponent is actually looking at, kept for
// the switch candidates of the same decision. See EvaluatePairBoard.
static EWRAM_DATA struct
{
    u8 mode;
    bool8 valid;
    u8 count;
    u8 weights[PAIR_FORECASTS];
    struct PairAction forecasts[PAIR_FORECASTS][2];
} sPairVisibleForecast = {0};

// Expected value of our pair over a weighted foe forecast, with the bounded
// pessimism share. The primary pattern's score is passed in when the caller
// already has it, INT_MIN otherwise.
static s32 PairForecastMixture(struct PairEvaluation *ev, enum BattlerId firstFoe, enum BattlerId secondFoe,
    const struct PairAction forecasts[PAIR_FORECASTS][2], const u8 *weights, u32 count, s32 primary)
{
    if (primary == INT_MIN)
    {
        ev->action[firstFoe] = forecasts[0][0];
        ev->action[secondFoe] = forecasts[0][1];
        primary = ScorePairWithImmediateEffects(ev);
    }
    s32 total = primary * 100, worst = primary;
    if (count > 1)
    {
        total = primary * weights[0];
        for (u32 forecast = 1; forecast < count; forecast++)
        {
            // The alternative patterns can coincide with the primary one,
            // and scoring the same four actions again cannot change the
            // answer. Exact reuse, not an approximation.
            if (PairSameForecast(forecasts[forecast], forecasts[0]))
            {
                total += primary * weights[forecast];
                continue;
            }
            ev->action[firstFoe] = forecasts[forecast][0];
            ev->action[secondFoe] = forecasts[forecast][1];
            s32 forecastScore = ScorePairWithImmediateEffects(ev);
            total += forecastScore * weights[forecast];
            worst = min(worst, forecastScore);
        }
    }
    return (total / 100 * (100 - PAIR_RISK_AVERSION) + worst * PAIR_RISK_AVERSION) / 100;
}

// What a pair of actions does to the board by itself: the same trial with
// both foes standing still. Only consulted to separate pairs the forecast
// scores identically, so it never changes how a scored difference is read.
static s32 PairStandaloneScore(struct PairEvaluation *ev, enum BattlerId actor, enum BattlerId partner,
    enum BattlerId firstFoe, enum BattlerId secondFoe, const struct PairAction *mine, const struct PairAction *theirs)
{
    struct PairAction saved[4] = {ev->action[actor], ev->action[partner], ev->action[firstFoe], ev->action[secondFoe]};
    ev->action[actor] = *mine;
    ev->action[partner] = *theirs;
    ev->action[firstFoe] = (struct PairAction){MOVE_NONE, AI_SCORE_DEFAULT, PAIR_IDLE, firstFoe};
    ev->action[secondFoe] = (struct PairAction){MOVE_NONE, AI_SCORE_DEFAULT, PAIR_IDLE, secondFoe};
    s32 score = ScorePairWithImmediateEffects(ev);
    ev->action[actor] = saved[0];
    ev->action[partner] = saved[1];
    ev->action[firstFoe] = saved[2];
    ev->action[secondFoe] = saved[3];
    return score;
}

// Whether both of our actions are single-target attacks aimed at the same foe.
// When one of them removes that foe, the other lands on the foe's partner by
// the native retarget, so the model scores the stacked pair and the split pair
// alike; the stacked pair still loses both hits to that foe's Protect, Follow
// Me or switch where the split one keeps one. An exact tie goes to the split.
static bool32 PairStacksTarget(enum BattlerId actor, const struct PairAction *mine, const struct PairAction *theirs)
{
    return mine->index != PAIR_IDLE && theirs->index != PAIR_IDLE
        && mine->target == theirs->target && mine->target < gBattlersCount && !IsBattlerAlly(actor, mine->target)
        && !IsBattleMoveStatus(mine->executedMove) && !IsBattleMoveStatus(theirs->executedMove)
        && !PairSpread(mine->executedMove) && !PairSpread(theirs->executedMove)
        && GetMoveTarget(mine->executedMove) == TARGET_SELECTED
        && GetMoveTarget(theirs->executedMove) == TARGET_SELECTED;
}

// nonGuard, when supplied, comes back holding the best final score this board
// reached with each of the two bodies doing something other than shielding -
// the comparison the guard trace's margin is asking for. It is INT_MIN when no
// such pair reached the final scoring, which is itself the answer to a
// different question and is reported separately rather than folded in.
static s32 EvaluatePairBoard(enum BattlerId actor, u32 noActionMask, struct PairAction *chosen, struct PairEvaluation *ev, bool32 refresh, bool32 canStop, bool32 mustFinish, s32 *nonGuard)
{
    if (nonGuard != NULL)
        nonGuard[0] = nonGuard[1] = INT_MIN;
    s32 best = INT_MIN;
    struct PairDefenderItemCache *defenderItem[MAX_BATTLERS_COUNT][MAX_BATTLERS_COUNT];
    struct PairRageCache *rage[MAX_BATTLERS_COUNT];
    struct PairDancerMoveCache *dancer[MAX_BATTLERS_COUNT][ARRAY_COUNT(sCopiedDances)];
    memcpy(defenderItem, ev->defenderItem, sizeof(defenderItem));
    memcpy(rage, ev->rage, sizeof(rage));
    memcpy(dancer, ev->dancer, sizeof(dancer));
    memset(ev, 0, sizeof(*ev));
    memcpy(ev->defenderItem, defenderItem, sizeof(defenderItem));
    memcpy(ev->rage, rage, sizeof(rage));
    memcpy(ev->dancer, dancer, sizeof(dancer));
    ev->side = GetBattlerSide(actor);
    ev->entering = noActionMask;
    SavePairBoard(&ev->board, ev->side);
    if (refresh && !RefreshPairMoveData(noActionMask, canStop))
        goto done;
    enum BattlerId partner = GetPartnerBattler(actor);
    ev->weather = AI_GetWeather();
    ev->sleepClause = IsSleepClauseEnabled();
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        ev->berryHeal[battler] = PairHeldBerryHeal(ev, battler);
        ev->residual[battler] = min(0xFFFF, AI_GetCertainResidualDamage(battler));
    }
    ev->paralysisActionChance = GetConfig(B_PARALYSIS_CHANCE) >= GEN_CHAMPIONS ? 8750 : 7500;
    // The only newly forecast item loss is Knock Off. Avoid native anchor
    // calculations when no other living actor can use it. Do not filter by
    // initial Speed: public speed control can change this turn's order.
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (IsBattlerAlive(battler) && !(noActionMask & (1u << battler)))
            for (u32 index = 0; index < MAX_MON_MOVES; index++)
            {
                enum Move move = gBattleMons[battler].moves[index];
                if (IsMoveUnusable(index, move, gAiLogicData->moveLimitations[battler]))
                    continue;
                if (GetMoveEffect(move) == EFFECT_KNOCK_OFF)
                    ev->itemRemovers |= 1u << battler;
                if (GetMoveEffect(move) == EFFECT_RAGE_FIST)
                    ev->rageActors |= 1u << battler;
                if (move == MOVE_SOAK)
                    for (enum BattlerId target = 0; target < gBattlersCount; target++)
                    {
                        if (PairCanSoak(battler, target))
                        {
                            ev->soakTargets[battler][index] |= 1u << target;
                            ev->soakable |= 1u << target;
                        }
                        if (target != battler && IsBattlerAlive(target)
                         && gAiLogicData->abilities[target] == ABILITY_STORM_DRAIN
                         && !IsMoveRedirectionPrevented(battler, move, gAiLogicData->abilities[battler])
                         && (B_REDIRECT_ABILITY_ALLIES >= GEN_4 || !IsBattlerAlly(battler, target)))
                            ev->soakRedirectors[battler] |= 1u << target;
                    }
                if (move == MOVE_TAILWIND || IsWindMove(move))
                    for (enum BattlerId target = 0; target < gBattlersCount; target++)
                        if (IsBattlerAlive(target) && gAiLogicData->abilities[target] == ABILITY_WIND_POWER
                         && (move == MOVE_TAILWIND ? IsBattlerAlly(battler, target) : target != battler))
                            ev->chargeable |= 1u << target;
                if (!IsBattleMoveStatus(move))
                    for (enum BattlerId target = 0; target < gBattlersCount; target++)
                        if (target != battler && IsBattlerAlive(target)
                         && gAiLogicData->abilities[target] == ABILITY_ELECTROMORPHOSIS)
                            ev->chargeable |= 1u << target;
            }
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (IsBattlerAlive(battler) && gBattleMons[battler].volatiles.chargeTimer)
            ev->chargeable |= 1u << battler;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        if (ev->rageActors & (1u << battler))
        {
            // Most boards have no Rage Fist; allocate only for an actual user,
            // and reuse its buffer across switch/form candidates.
            if (ev->rage[battler] == NULL)
                ev->rage[battler] = AllocZeroed(sizeof(*ev->rage[battler]));
            else
                memset(ev->rage[battler], 0, sizeof(*ev->rage[battler]));
        }
    CachePairCopiedDancers(ev, noActionMask);
    CachePairDefenderItems(ev, noActionMask);
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
            action->tacticScore = GetBattlerSide(battler) == ev->side ? PairTacticScore(battler, action) : 0;
        }
        // Our own options are searched strongest-first by their standalone
        // opinion. A search the clock cuts short keeps what it reached first,
        // and in move-slot order that was slot zero into the first legal
        // target: the crowded Mossdeep multi, at its budget every turn, sent
        // Heatran's Magma Storm into a resisting Tyranitar while an Earth
        // Power knockout on a quarter-health Metagross was never scored.
        // Stable, so equal opinions keep their slot order.
        if (GetBattlerSide(battler) == ev->side)
        {
            for (u32 index = 1; index < ev->count[battler]; index++)
            {
                struct PairAction option = ev->choices[battler][index];
                s32 key = (option.score - AI_SCORE_DEFAULT) * 4 + option.planScore + option.tacticScore;
                u32 slot = index;
                while (slot > 0)
                {
                    const struct PairAction *before = &ev->choices[battler][slot - 1];
                    if ((before->score - AI_SCORE_DEFAULT) * 4 + before->planScore + before->tacticScore >= key)
                        break;
                    ev->choices[battler][slot] = *before;
                    slot--;
                }
                ev->choices[battler][slot] = option;
            }
        }
    }
    // Confirmed human actions have exactly one choice. Only genuinely unknown
    // actors retain the alternative-move/target forecast.
    struct PairAction forecasts[PAIR_FORECASTS][2];
    u8 forecastWeights[PAIR_FORECASTS] = {0};
    u32 forecastCount = ChooseJointFoeForecast(ev, actor, forecasts, forecastWeights);
    // The foes commit their targets against the board they can see, and our
    // switch lands after that choice. A candidate board's own forecast lets
    // them re-aim at a body that has not arrived yet, which can flatter the
    // switch as easily as punish it: the Magikarp fisherman's Feebas was
    // credited with Mirror Coating Dragon Rages re-aimed at it, and switched
    // a Magikarp out every turn for it. A switch is therefore credited with
    // the worse of the two readings - the foes' choice on the board they saw,
    // and a foe that saw the switch coming - and never the better one.
    bool32 visibleForecast = FALSE;
    if (sPairVisibleForecast.mode == PAIR_VISIBLE_FORECAST_CAPTURE)
    {
        sPairVisibleForecast.count = forecastCount;
        memcpy(sPairVisibleForecast.forecasts, forecasts, sizeof(forecasts));
        memcpy(sPairVisibleForecast.weights, forecastWeights, sizeof(forecastWeights));
        sPairVisibleForecast.valid = TRUE;
    }
    else if (sPairVisibleForecast.mode == PAIR_VISIBLE_FORECAST_USE && sPairVisibleForecast.valid)
    {
        visibleForecast = sPairVisibleForecast.count != forecastCount;
        for (u32 forecast = 0; forecast < forecastCount && !visibleForecast; forecast++)
            visibleForecast = !PairSameForecast(forecasts[forecast], sPairVisibleForecast.forecasts[forecast])
                || forecastWeights[forecast] != sPairVisibleForecast.weights[forecast];
    }
    enum BattlerId firstFoe = GetOppositeBattler(actor);
    enum BattlerId secondFoe = GetPartnerBattler(firstFoe);
    bool32 canWait = PairWaitingHasPayoff(ev, actor);
    ev->waitingPayoff = canWait;
    ev->wholeTurnPayoff = PairFieldClockExpiring(actor);
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
    struct PairShortlistEntry
    {
        s32 score;
        s32 standalone; // INT_MIN until a tie asks for it.
        u8 left;
        u8 right;
    } shortlist[PAIR_SHORTLIST];
    u32 shortCount = 0, shortLimit = PairShortlistSize(), examined = 0;
    u32 allowance = PairWorkAllowance(ev->count[actor] * ev->count[partner]);
    // A stop must never leave a battler with nothing but guards scored. The
    // enumeration can reach its allowance while every pair examined so far has
    // this body shielding, and then the guard wins by default against nothing
    // - which is exactly what an empty guard looks like from the outside. A
    // body that has an attacking action gets one of them scored first.
    bool32 scoredAttack[2] = {TRUE, TRUE};
    const enum BattlerId pairActors[2] = {actor, partner};
    for (u32 side = 0; side < 2; side++)
        for (u32 choice = 0; choice < ev->count[pairActors[side]]; choice++)
        {
            const struct PairAction *option = &ev->choices[pairActors[side]][choice];
            if (option->index != PAIR_IDLE && GetMoveEffect(option->move) != EFFECT_PROTECT)
                scoredAttack[side] = FALSE;
        }
    bool32 mixed = FALSE;
    // The order the pairs are visited in decides what a search cut short by
    // its allowance or the clock has seen. Row by row, a stop saw the actor's
    // first-ranked action beside every partner action and nothing else: the
    // crowded Mossdeep multi scored seven of forty-nine pairs, all of them
    // Heatran's Magma Storm into a 1 HP Metagross that Raging Bolt's
    // Thunderclap was already removing, and Earth Power into the Mega
    // Tyranitar it hits super effectively was never looked at. Visit best
    // responses first instead: every actor action beside the partner's
    // strongest one, then every partner action beside the actor's best reply,
    // then every actor action beside that, and only then the rest by rank.
    // A complete search scores the same pairs in any order.
    u32 countLeft = ev->count[actor], countRight = ev->count[partner];
    u32 totalPairs = countLeft * countRight;
    bool8 visited[PAIR_ACTIONS][PAIR_ACTIONS];
    memset(visited, 0, sizeof(visited));
    u32 phase = 0, cursor = 0, diagonal = 0, visits = 0;
    u32 sweepLeft = 0, sweepRight = 0, bestLeft = 0, bestRight = 0;
    s32 sweepBest = INT_MIN;
    while (visits < totalPairs)
    {
        u32 left = 0, right = 0;
        {
            bool32 found = FALSE;
            while (!found)
            {
                if (phase == 0 || phase == 2)
                {
                    if (cursor < countLeft)
                    {
                        left = cursor++;
                        right = phase == 0 ? 0 : sweepRight;
                        found = !visited[left][right];
                        continue;
                    }
                }
                else if (phase == 1)
                {
                    if (cursor < countRight)
                    {
                        left = sweepLeft;
                        right = cursor++;
                        found = !visited[left][right];
                        continue;
                    }
                }
                else
                {
                    // Remaining pairs by combined rank, strongest first.
                    while (diagonal <= countLeft + countRight - 2 && !found)
                    {
                        if (cursor > diagonal || cursor >= countLeft)
                        {
                            diagonal++;
                            cursor = 0;
                            continue;
                        }
                        left = cursor++;
                        if (diagonal - left < countRight)
                        {
                            right = diagonal - left;
                            found = !visited[left][right];
                        }
                    }
                    break;
                }
                // The phase is exhausted: the next sweep starts from the best
                // pair found so far.
                if (phase == 0)
                    sweepLeft = bestLeft;
                else if (phase == 1)
                    sweepRight = bestRight;
                phase++;
                cursor = 0;
            }
            if (!found)
                break;
        }
        visited[left][right] = TRUE;
        visits++;
        {
            // Deterministic first: every arm of one decision searches the same
            // number of pairs, so a candidate board and the stay board are
            // always compared at equal depth.
            if (!mustFinish && scoredAttack[0] && scoredAttack[1]
             && shortCount != 0 && examined >= allowance)
                goto settle;
            // The clock is only a safety stop now. For the board the AI is
            // standing on it settles with what it has; for a candidate board
            // it abandons the comparison outright, because half a candidate
            // scored against a whole stay board is the bug this replaced.
            if (!mustFinish && PairDecisionBudgetExpired())
            {
                gAiPairBudgetTruncated = TRUE;
                gAiPairDecisionTruncated = TRUE;
                // A turn's first board has nothing behind it: abandoning it
                // left the placeholder - slot zero aimed at the user - as the
                // decision, and the AI attacked its own battler whenever a
                // shared multi clock ran out first. It settles with what it
                // has once each body has a real scored action. A candidate
                // query (no chosen actions) still abandons the comparison.
                if (!canStop && chosen == NULL)
                    goto done;
                if (shortCount != 0 && scoredAttack[0] && scoredAttack[1])
                    goto settle;
            }
            examined++;
#if TESTING
            sTestPairsScored++;
#endif
            ev->action[actor] = ev->choices[actor][left];
            ev->action[partner] = ev->choices[partner][right];
            if (ev->action[actor].index != PAIR_IDLE
             && GetMoveEffect(ev->action[actor].move) != EFFECT_PROTECT)
                scoredAttack[0] = TRUE;
            if (ev->action[partner].index != PAIR_IDLE
             && GetMoveEffect(ev->action[partner].move) != EFFECT_PROTECT)
                scoredAttack[1] = TRUE;
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
            ev->action[firstFoe] = forecasts[0][0];
            ev->action[secondFoe] = forecasts[0][1];
            s32 primary = ScorePairWithImmediateEffects(ev);
            // An exact tie on the forecast is settled by what the two
            // actions do on their own, never by enumeration order. A body
            // forecast to fall before it moves scores every one of its
            // actions the same, and slot zero into the first legal target
            // used to win by default: a resisted Collision Course over a sun
            // Flare Blitz, a Specs Moonblast into Zacian over Mystical Fire,
            // and both partners aimed at the one foe the faster hit removes.
            s32 standalone = INT_MIN;
            u32 slot = shortCount;
            while (slot > 0 && shortlist[slot - 1].score <= primary)
            {
                if (shortlist[slot - 1].score == primary)
                {
                    if (standalone == INT_MIN)
                        standalone = PairStandaloneScore(ev, actor, partner, firstFoe, secondFoe,
                            &ev->choices[actor][left], &ev->choices[partner][right]);
                    if (shortlist[slot - 1].standalone == INT_MIN)
                        shortlist[slot - 1].standalone = PairStandaloneScore(ev, actor, partner, firstFoe, secondFoe,
                            &ev->choices[actor][shortlist[slot - 1].left], &ev->choices[partner][shortlist[slot - 1].right]);
                    if (shortlist[slot - 1].standalone > standalone
                     || (shortlist[slot - 1].standalone == standalone
                         && (PairStacksTarget(actor, &ev->choices[actor][left], &ev->choices[partner][right])
                             || !PairStacksTarget(actor, &ev->choices[actor][shortlist[slot - 1].left],
                                                  &ev->choices[partner][shortlist[slot - 1].right]))))
                        break;
                }
                if (slot < shortLimit)
                    shortlist[slot] = shortlist[slot - 1];
                slot--;
            }
            if (slot < shortLimit)
            {
                shortlist[slot] = (struct PairShortlistEntry){primary, standalone, left, right};
                if (shortCount < shortLimit)
                    shortCount++;
            }
            if (phase < 3 && primary > sweepBest)
            {
                sweepBest = primary;
                bestLeft = left;
                bestRight = right;
            }
        }
    }
settle:
    // Expected value over the weighted opponent model, with a bounded
    // pessimism share. Taking the minimum instead made every credible
    // knockout pattern demand a shield.
    // The scoring rule has to be the same for every board in one decision,
    // or a switch candidate evaluated after the budget expired is compared
    // against a stay board that was scored on the full mixture. The budget
    // controls how many pairs are searched, never how a pair is valued.
    mixed = forecastCount > 1;
    bool32 bestStacked = FALSE;
    for (u32 entry = 0; entry < shortCount; entry++)
    {
        ev->action[actor] = ev->choices[actor][shortlist[entry].left];
        ev->action[partner] = ev->choices[partner][shortlist[entry].right];
        s32 score = PairForecastMixture(ev, firstFoe, secondFoe, forecasts, forecastWeights,
            mixed ? forecastCount : 1, shortlist[entry].score);
        if (visibleForecast)
            score = min(score, PairForecastMixture(ev, firstFoe, secondFoe, sPairVisibleForecast.forecasts,
                sPairVisibleForecast.weights, sPairVisibleForecast.count, INT_MIN));
        if (nonGuard != NULL)
        {
            // Every scored pair, not just the one that won. The old margin was
            // fed only the winning pair of each outer candidate board, so a
            // board whose best pair shielded reported "nothing attacked" even
            // when attacking pairs had been scored and lost.
            const struct PairAction *entry[2] = {&ev->action[actor], &ev->action[partner]};
            for (u32 index = 0; index < 2; index++)
                if (entry[index]->index != PAIR_IDLE
                 && GetMoveEffect(entry[index]->move) != EFFECT_PROTECT
                 && score > nonGuard[index])
                    nonGuard[index] = score;
        }
        bool32 stacked = PairStacksTarget(actor, &ev->action[actor], &ev->action[partner]);
        if (score > best || (score == best && bestStacked && !stacked))
        {
            best = score;
            bestStacked = stacked;
            if (chosen != NULL)
            {
                chosen[0] = ev->action[actor];
                chosen[1] = ev->action[partner];
            }
        }
    }
done:
    return best;
}


static void FreePairEvaluation(struct PairEvaluation *ev)
{
    for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        if (ev->rage[battler] != NULL)
            Free(ev->rage[battler]);
        for (enum BattlerId target = 0; target < MAX_BATTLERS_COUNT; target++)
            if (ev->defenderItem[battler][target] != NULL)
                Free(ev->defenderItem[battler][target]);
        for (u32 dance = 0; dance < ARRAY_COUNT(sCopiedDances); dance++)
            if (ev->dancer[battler][dance] != NULL)
                Free(ev->dancer[battler][dance]);
    }
    Free(ev);
}

s32 AI_EvaluateDoublesCandidate(enum BattlerId battler, u32 noActionMask)
{
    // The clock is the safety stop, and it stops the comparison rather than
    // shortening one side of it. Once it has run out, every board - including
    // the one the AI is standing on - scores INT_MIN, no reserve can beat the
    // position, and the turn is played from where it already is. A truncated
    // comparison is recorded so a receipt can say so; it is never silent.
    if (PairDecisionBudgetExpired())
    {
        gAiPairBudgetTruncated = TRUE;
        return INT_MIN;
    }
    struct PairEvaluation *ev = AllocZeroed(sizeof(*ev));
    s32 score = EvaluatePairBoard(battler, noActionMask, NULL, ev, TRUE, FALSE, FALSE, NULL);
    FreePairEvaluation(ev);
    return score;
}

s32 AI_EvaluateDoublesPosition(enum BattlerId battler, u32 noActionMask)
{
    struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
    // Scoring a position on request is not part of a turn's decision, so it
    // gets its own clock and leaves the turn's untouched. The same position
    // asked twice has to answer the same thing.
    u32 decisionStart = gAiLogicData->decisionStartFrame;
    u32 allowance = sPairWorkAllowance, allowanceDecision = sPairWorkDecision;
    gAiLogicData->decisionStartFrame = gMain.vblankCounter1;
    s32 score = AI_EvaluateDoublesCandidate(battler, noActionMask);
    gAiLogicData->decisionStartFrame = decisionStart;
    sPairWorkAllowance = allowance;
    sPairWorkDecision = allowanceDecision;
    AI_RestoreCandidateState(state);
    AI_FreeCandidateState(state);
    return score;
}

// Board value of the best attack the outgoing battler is giving up this turn.
static s32 PairForfeitedAttackValue(enum BattlerId actor)
{
    s32 best = 0;
    if (!IsBattlerAlive(actor))
        return 0;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
            continue;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            if (IsMoveUnusable(index, gBattleMons[actor].moves[index], gAiLogicData->moveLimitations[actor]))
                continue;
            u32 damage = min(gBattleMons[foe].hp, gAiLogicData->simulatedDmg[actor][foe][index].median);
            u32 accuracy = min(100, gAiLogicData->moveAccuracy[actor][foe][index]);
            s32 value = (s32)(damage * 180 / max(1, gBattleMons[foe].maxHP) * accuracy / 100);
            best = max(best, value);
        }
    }
    return min(PAIR_SWITCH_TEMPO_CAP, best);
}

// What a voluntary switch-in is walking into. A move the other side holds that
// would remove the arriving body before it acts costs the whole switch penalty.
// A move that takes half of the newcomer or more is a trade the wrong way
// round when the newcomer loses more than the switch saves, costed at the
// share of a body by which it does. What the switch saves from a move the
// outgoing body resists or is immune to is exactly what that move would have
// taken from it (PairOutgoingLoss): Cynthia's full Togekiss, immune to Primal
// Groudon's Precipice Blades, left for a Garchomp that took it neutrally and
// arrived at 42%. From a super-effective hit on the newcomer the switch saved
// at most the share the outgoing body had left: the forecast is a guess at
// where the foes aim, and the switch was the answer to their best hit on this
// slot. Lea & Jed's Ursaluna gave two thirds to Iron Hands' Drain Punch to
// save a Miltank at a third; Wally's Roselia fell to Heatran's Heat Wave to
// save a Gardevoir at a ninth. A full Tapu Fini leaving a Leaf Blade it cannot
// survive for a Milotic that can is not charged, and neither is a Regidrago
// leaving the Play Rough that removes it for the Multiscale Dragonite that
// lives through it. The AI reads exactly the loadout its flags show it
// (GetMovesArray): campaign trainers see sets, so a Gardevoir that had not yet
// attacked still visibly held the Moonblast that ended Lilycove Brendan's
// Salamence on arrival. The damage is the candidate board's, which prices a
// foe's visible Mega. Dead weight - a body held in a useless lock, or with no
// attack to use - is costed as before, by the moves the foes have shown
// knocking the newcomer out: the foes' strongest hit is aimed at it wherever
// it stands.
static s32 PairEntryCost(enum BattlerId entering, bool32 deadWeight, u32 savedShare,
    const u8 outgoingLoss[MAX_BATTLERS_COUNT][MAX_MON_MOVES])
{
    if (!IsBattlerAlive(entering))
        return 0;
    u32 hp = gBattleMons[entering].hp, maxHp = max(1, gBattleMons[entering].maxHP);
    s32 excess = 0;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(entering, foe))
            continue;
        enum Move *moves = GetMovesArray(foe);
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = moves[index];
            if (move == MOVE_NONE || move == MOVE_UNAVAILABLE || IsBattleMoveStatus(move)
             || IsMoveUnusable(index, move, gAiLogicData->moveLimitations[foe]))
                continue;
            if (deadWeight)
            {
                bool32 revealed = gAiLogicData->lastUsedMove[foe] == move;
                for (u32 seen = 0; seen < MAX_MON_MOVES && !revealed; seen++)
                    revealed = gBattleHistory->usedMoves[foe][seen] == move;
                if (!revealed)
                    continue;
            }
            const struct SimulatedDamage *damage = &gAiLogicData->simulatedDmg[foe][entering][index];
            if (damage->minimum >= hp && damage->minimum != 0)
                return PAIR_SWITCH_INTO_DEATH;
            if (deadWeight)
                continue;
            u32 share = min(100, damage->median * 100 / max(1, hp));
            if (share * 2 < 100)
                continue;
            s32 loss = (s32)(min(hp, damage->median) * 100 / maxHp);
            if (outgoingLoss[foe][index] != PAIR_NOT_RELIEVED)
                excess = max(excess, loss - (s32)outgoingLoss[foe][index]);
            if (gAiLogicData->effectiveness[foe][entering][index] >= UQ_4_12(2.0))
                excess = max(excess, (s32)share - (s32)savedShare);
        }
    }
    return PAIR_SWITCH_INTO_DEATH * min(100, excess) / 100;
}

// What each foe's visible attacks take from a body that may leave, as a
// percentage of its maximum HP, priced like a newcomer: every usable attack,
// at the harder of a foe's two forms while it can still Mega Evolve. Only a
// move the body resists or is immune to is recorded - the hits a switch out of
// it dodges. The rest are PAIR_NOT_RELIEVED. Read before any candidate loads.
static void PairOutgoingLoss(enum BattlerId battler, u8 loss[MAX_BATTLERS_COUNT][MAX_MON_MOVES])
{
    memset(loss, PAIR_NOT_RELIEVED, sizeof(u8) * MAX_BATTLERS_COUNT * MAX_MON_MOVES);
    if (!IsBattlerAlive(battler))
        return;
    u32 hp = gBattleMons[battler].hp, maxHp = max(1, gBattleMons[battler].maxHP);
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(battler, foe))
            continue;
        enum Move *moves = GetMovesArray(foe);
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = moves[index];
            if (move == MOVE_NONE || move == MOVE_UNAVAILABLE || IsBattleMoveStatus(move)
             || IsMoveUnusable(index, move, gAiLogicData->moveLimitations[foe]))
                continue;
            u32 median = gAiLogicData->simulatedDmg[foe][battler][index].median;
            uq4_12_t effectiveness = gAiLogicData->effectiveness[foe][battler][index];
            if (CanMegaEvolve(foe))
            {
                struct AiCalcValues calc = {.move = move, .gimmickAtk = GIMMICK_MEGA, .gimmickDef = GIMMICK_NONE,
                    .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
                struct SimulatedDamage mega = AI_CalcDamage(&calc, foe, battler);
                if (mega.median > median)
                {
                    median = mega.median;
                    effectiveness = calc.typeEffectiveness;
                }
            }
            if (effectiveness <= UQ_4_12(0.5))
                loss[foe][index] = min(hp, median) * 100 / maxHp;
        }
    }
}

// Whether the body now loaded into this slot is expected to go down before it
// acts: some foe's best usable attack - a spread move included, and priced in
// the Mega form that foe can still take - does at least its HP on the median
// roll, and no Sash or Sturdy holds it at one. Loadouts are visible, the foes'
// committed moves are not, so every usable attack counts.
static bool32 PairEntryExpectedKo(enum BattlerId entering)
{
    if (!IsBattlerAlive(entering))
        return TRUE;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(entering, foe))
            continue;
        u32 forms = CanMegaEvolve(foe) ? 2 : 1;
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = gBattleMons[foe].moves[index];
            if (move == MOVE_NONE || IsBattleMoveStatus(move)
             || IsMoveUnusable(index, move, gAiLogicData->moveLimitations[foe])
             || CanEndureHit(foe, entering, move))
                continue;
            for (u32 form = 0; form < forms; form++)
            {
                struct AiCalcValues calc = {.move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain,
                    .gimmickAtk = form ? GIMMICK_MEGA : GIMMICK_NONE};
                if (AI_CalcDamage(&calc, foe, entering).median >= gBattleMons[entering].hp)
                    return TRUE;
            }
        }
    }
    return FALSE;
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

// The share of its power a held attack still carries at the stage its user
// fires it from, against this foe: the stat that powers it (Defense for Body
// Press; Foul Play borrows the target's), and nothing for a foe whose Unaware
// ignores the stage.
static u32 PairLockStageShare(enum BattlerId actor, enum BattlerId foe, enum Move move)
{
    enum BattleMoveEffects effect = GetMoveEffect(move);
    if (effect == EFFECT_FOUL_PLAY || gAiLogicData->abilities[foe] == ABILITY_UNAWARE)
        return 100;
    enum Stat stat = effect == EFFECT_BODY_PRESS ? STAT_DEF
        : IsBattleMovePhysical(move) ? STAT_ATK : STAT_SPATK;
    u32 stage = gBattleMons[actor].statStages[stat];
    return gStatStageRatios[stage][0] * 100 / gStatStageRatios[stage][1];
}

// Whether a move the battler is held to buys nothing: a status move, a move it
// cannot select at all (so the turn is a Struggle), or an attack that no living
// foe takes more than half damage from. The pressure search below only asks
// whether anything connects, so a quarter-damage Psychic into Metagross beside
// an immune Incineroar kept the lock "productive" for seven turns. The move is
// judged at the power it has now, not at a fresh stage: Glacia's
// Specs Kyurem fired Draco Meteor at -2, -4 and -6, and Juan's Specs Kingdra
// at -4 and -6 beside a full Mega Gyarados. A -2 Draco Meteor into a neutral
// foe is the resisted hit this already calls useless.
static bool32 PairLockedMoveUseless(enum BattlerId actor, enum Move locked)
{
    u32 index = GetMoveIndex(actor, locked);
    if (index >= MAX_MON_MOVES)
        return FALSE;
    if (IsBattleMoveStatus(locked) || IsMoveUnusable(index, locked, gAiLogicData->moveLimitations[actor]))
        return TRUE;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
        if (IsBattlerAlive(foe) && !IsBattlerAlly(actor, foe)
         && gAiLogicData->effectiveness[actor][foe][index] * PairLockStageShare(actor, foe, locked) / 100 > UQ_4_12(0.5))
            return FALSE;
    return TRUE;
}

static bool32 PairUselessChoiceLock(enum BattlerId actor)
{
    enum Move locked = gBattleStruct->choicedMove[actor];
    if (!IsBattlerAlive(actor) || !HasChoiceEffect(actor)
     || locked == MOVE_NONE || locked == MOVE_UNAVAILABLE)
        return FALSE;
    return PairLockedMoveUseless(actor, locked);
}

// The same for an Encore. Wally's Ludicolo, Encored into a Fake Out it could
// never use again, Struggled twice while his healthy Gallade was the one that
// left; Wallace's Kyogre sat in an Encored Protect that could only fail. A lock
// that ends with this turn costs only this turn, which the board already sees.
static bool32 PairUselessEncore(enum BattlerId actor, bool32 turnsAfterThis)
{
    enum Move locked = gBattleMons[actor].volatiles.encoredMove;
    if (!IsBattlerAlive(actor) || !gBattleMons[actor].volatiles.encoreTimer
     || locked == MOVE_NONE || locked == MOVE_UNAVAILABLE
     || (turnsAfterThis && gBattleMons[actor].volatiles.encoreTimer < 2))
        return FALSE;
    return PairLockedMoveUseless(actor, locked);
}

static bool32 PairUselessLock(enum BattlerId actor)
{
    return PairUselessChoiceLock(actor) || PairUselessEncore(actor, TRUE);
}

static bool32 PairNeedsSwitchSearch(enum BattlerId actor)
{
    if (PairUselessChoiceLock(actor) || PairUselessEncore(actor, FALSE))
        return TRUE;
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

// The type a Normal move becomes under an -ate ability, or TYPE_NONE.
static enum Type PairAteType(enum Ability ability)
{
    switch (ability)
    {
    case ABILITY_PIXILATE: return TYPE_FAIRY;
    case ABILITY_AERILATE: return TYPE_FLYING;
    case ABILITY_REFRIGERATE: return TYPE_ICE;
    case ABILITY_GALVANIZE: return TYPE_ELECTRIC;
    default: return TYPE_NONE;
    }
}

// A standard-formula estimate of one hit, as a percentage of the defender's
// current HP. Only the terms that separate one reserve from another: level,
// power, the two stats, STAB, type and the spread reduction. No rolls, no
// field; this only orders candidates, and the board then prices the winners.
static u32 PairEstimatePercent(u32 level, u32 power, u32 attack, u32 defense, bool32 stab,
                               uq4_12_t factor, bool32 spread, u32 hp)
{
    if (!factor || !power || !hp)
        return 0;
    u32 damage = ((2 * level / 5 + 2) * power * attack / max(1, defense)) / 50 + 2;
    if (stab)
        damage = damage * 3 / 2;
    damage = damage * factor / UQ_4_12(1.0);
    if (spread)
        damage = damage * 3 / 4;
    return min(300, damage * 100 / hp);
}

static u32 PairStagedStat(enum BattlerId battler, enum Stat stat, u32 value)
{
    u32 stage = gBattleMons[battler].statStages[stat];
    return value * gStatStageRatios[stage][0] / gStatStageRatios[stage][1];
}

// How much of this reserve the foes' visible sets take on arrival, and how
// much of the foes it takes back. Each foe's best usable attack is priced
// against the reserve's own bulk and typing - spread moves included, since
// they reach whatever comes in - and a foe that can still Mega Evolve is
// priced in whichever form hits harder: Mega Gardevoir's Pixilate Hyper
// Voice is on the board the moment the stone is. Loadouts are known to both
// sides; the foes' committed moves are not, so every usable attack counts.
static s32 PairRankReserve(enum BattlerId actor, u32 slot)
{
    struct Pokemon *mon = &GetBattlerParty(actor)[slot];
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    enum Ability ability = GetMonAbility(mon);
    enum HoldEffect item = GetItemHoldEffect(GetMonData(mon, MON_DATA_HELD_ITEM));
    enum Type type1 = gSpeciesInfo[species].types[0], type2 = gSpeciesInfo[species].types[1];
    u32 hp = GetMonData(mon, MON_DATA_HP), maxHp = max(1, GetMonData(mon, MON_DATA_MAX_HP));
    u32 level = GetMonData(mon, MON_DATA_LEVEL);
    u32 defense = GetMonData(mon, MON_DATA_DEF), spDefense = GetMonData(mon, MON_DATA_SPDEF);
    if (item == HOLD_EFFECT_ASSAULT_VEST)
        spDefense = spDefense * 3 / 2;
    s32 rank = hp * 100 / maxHp;
    if (PairReserveChangesPlan(actor, slot))
        rank += 40;
    u32 threatTotal = 0;
    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
            continue;
        u32 attack = 0, threat = 0;
        enum Ability foeAbility = gAiLogicData->abilities[foe];
        enum Species foeSpecies = gBattleMons[foe].species;
        // The visible Mega: its ability and typing, at the base form's stats.
        enum Species foeMega = SPECIES_NONE;
        if (CanMegaEvolve(foe))
        {
            foeMega = GetBattleFormChangeTargetSpecies(foe, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM, foeAbility);
            if (foeMega == foeSpecies)
                foeMega = SPECIES_NONE;
        }
        for (u32 index = 0; index < MAX_MON_MOVES; index++)
        {
            enum Move move = GetMonData(mon, MON_DATA_MOVE1 + index);
            if (move != MOVE_NONE && !IsBattleMoveStatus(move) && GetMonData(mon, MON_DATA_PP1 + index))
            {
                enum Type type = GetMoveType(move);
                u32 factor = PairReserveTypeFactor(actor, foe, move, foeSpecies,
                    ability, foeAbility, gAiLogicData->holdEffects[foe]);
                bool32 physical = GetBattleMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL;
                u32 percent = PairEstimatePercent(level, max(40, GetMovePower(move)),
                    GetMonData(mon, physical ? MON_DATA_ATK : MON_DATA_SPATK),
                    PairStagedStat(foe, physical ? STAT_DEF : STAT_SPDEF,
                        physical ? gBattleMons[foe].defense : gBattleMons[foe].spDefense),
                    type == type1 || type == type2, factor, PairSpread(move), gBattleMons[foe].hp);
                attack = max(attack, percent);
            }
            move = gBattleMons[foe].moves[index];
            if (move == MOVE_NONE || IsBattleMoveStatus(move)
             || IsMoveUnusable(index, move, gAiLogicData->moveLimitations[foe]))
                continue;
            bool32 physical = GetBattleMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL;
            u32 foeAttack = physical ? PairStagedStat(foe, STAT_ATK, gBattleMons[foe].attack)
                                     : PairStagedStat(foe, STAT_SPATK, gBattleMons[foe].spAttack);
            for (u32 form = 0; form < (foeMega != SPECIES_NONE ? 2 : 1); form++)
            {
                enum Species attacker = form ? foeMega : foeSpecies;
                enum Ability attackerAbility = form ? GetBattlerSpeciesAbility(foe, foeMega, 0) : foeAbility;
                u32 factor = PairReserveTypeFactor(foe, actor, move, species, attackerAbility, ability, item);
                enum Type type = GetMoveType(move);
                u32 power = max(40, GetMovePower(move));
                enum Type ate = PairAteType(attackerAbility);
                if (type == TYPE_NORMAL && ate != TYPE_NONE && factor)
                {
                    type = ate;
                    power = power * 6 / 5;
                    factor = GetTypeModifier(type, type1);
                    if (type2 != type1)
                        factor = uq4_12_multiply(factor, GetTypeModifier(type, type2));
                }
                bool32 stab = type == gSpeciesInfo[attacker].types[0] || type == gSpeciesInfo[attacker].types[1];
                threat = max(threat, PairEstimatePercent(gBattleMons[foe].level, power, foeAttack,
                    physical ? defense : spDefense, stab, factor, PairSpread(move), max(1, hp)));
            }
        }
        rank += (s32)min(100, attack) / 2 - (s32)min(150, threat);
        threatTotal += threat;
    }
    // A body the visible sets knock out before it acts has not been saved,
    // only spent; rank it below anything that survives.
    if (threatTotal >= 100)
        rank -= 100;
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

// An already preferred switch can sometimes depart through a reliable hit.
// Limit this shortcut to passive boards: HP-triggered abilities/items, damage
// retaliation and field-changing commands need a full pivot-event forecast.
static bool32 PairPassivePivotAbility(enum Ability ability)
{
    switch (ability)
    {
    case ABILITY_NONE:
    case ABILITY_REGENERATOR:
    case ABILITY_NATURAL_CURE:
    case ABILITY_GUTS:
    case ABILITY_THICK_FAT:
    case ABILITY_DRY_SKIN:
    case ABILITY_WATER_ABSORB:
    case ABILITY_VOLT_ABSORB:
    case ABILITY_LEVITATE:
    case ABILITY_VITAL_SPIRIT:
    case ABILITY_INSOMNIA:
    case ABILITY_OWN_TEMPO:
    case ABILITY_INNER_FOCUS:
    case ABILITY_IMMUNITY:
    case ABILITY_LIMBER:
    case ABILITY_PRANKSTER:
    case ABILITY_SERENE_GRACE:
    case ABILITY_PIXILATE:
    case ABILITY_AERILATE:
    case ABILITY_REFRIGERATE:
    case ABILITY_GALVANIZE:
    case ABILITY_NORMALIZE:
    case ABILITY_TECHNICIAN:
    case ABILITY_ADAPTABILITY:
    case ABILITY_HUGE_POWER:
    case ABILITY_PURE_POWER:
    case ABILITY_IRON_FIST:
    case ABILITY_TOUGH_CLAWS:
    case ABILITY_ROCK_HEAD:
    case ABILITY_SCRAPPY:
    case ABILITY_MOLD_BREAKER:
    case ABILITY_BATTLE_ARMOR:
    case ABILITY_SHELL_ARMOR:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 PairPassivePivotItem(enum HoldEffect held)
{
    switch (held)
    {
    case HOLD_EFFECT_NONE:
    case HOLD_EFFECT_EVIOLITE:
    case HOLD_EFFECT_ASSAULT_VEST:
    case HOLD_EFFECT_CHOICE_BAND:
    case HOLD_EFFECT_CHOICE_SCARF:
    case HOLD_EFFECT_CHOICE_SPECS:
    case HOLD_EFFECT_COVERT_CLOAK:
    case HOLD_EFFECT_EXPERT_BELT:
    case HOLD_EFFECT_MUSCLE_BAND:
    case HOLD_EFFECT_WISE_GLASSES:
    case HOLD_EFFECT_FLAME_ORB:
    case HOLD_EFFECT_TOXIC_ORB:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 PairPivotSensitiveMove(enum Move move)
{
    switch (GetMoveEffect(move))
    {
    case EFFECT_REFLECT_DAMAGE:
    case EFFECT_BIDE:
    case EFFECT_RAGE_FIST:
    case EFFECT_REVENGE:
    case EFFECT_FLAIL:
    case EFFECT_ENDEAVOR:
    case EFFECT_PAYBACK:
    case EFFECT_PURSUIT:
    case EFFECT_BEAK_BLAST:
    case EFFECT_POWER_BASED_ON_TARGET_HP:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 PairPivotStrictlyBefore(enum BattlerId actor, enum Move move,
    enum BattlerId other, enum Move otherMove, const u32 *speed)
{
    s32 priority = AI_GetMovePriority(actor, gAiLogicData->abilities[actor], move);
    s32 otherPriority = AI_GetMovePriority(other, gAiLogicData->abilities[other], otherMove);
    if (priority != otherPriority)
        return priority > otherPriority;
    return gFieldStatuses & STATUS_FIELD_TRICK_ROOM ? speed[actor] < speed[other] : speed[actor] > speed[other];
}

static bool32 PairTryAttackBeforeSwitch(enum BattlerId actor, u32 reserve,
    const struct PairAction *partnerAction, struct PairAction *action)
{
    enum BattlerId partner = GetPartnerBattler(actor);
    u32 speed[MAX_BATTLERS_COUNT];
    if (!IsBattlerAlive(actor) || reserve >= PARTY_SIZE
     || !PairPassivePivotAbility(GetMonAbility(&GetBattlerParty(actor)[reserve]))
     || !PairPassivePivotItem(GetItemHoldEffect(GetMonData(&GetBattlerParty(actor)[reserve], MON_DATA_HELD_ITEM)))
     || (gBattleMons[actor].status1 & (STATUS1_SLEEP | STATUS1_FREEZE | STATUS1_PARALYSIS))
     || gBattleMons[actor].volatiles.confusionTimer || gBattleMons[actor].volatiles.infatuation
     || gBattleMons[actor].volatiles.flinched || gBattleMons[actor].volatiles.rechargeTimer
     || IsSemiInvulnerable(actor, CHECK_ALL))
        return FALSE;
    for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
    {
        if (!IsBattlerAlive(battler))
            continue;
        if (!PairPassivePivotAbility(gAiLogicData->abilities[battler])
         || !PairPassivePivotItem(gAiLogicData->holdEffects[battler])
         || GetActiveGimmick(battler) != GIMMICK_NONE)
            return FALSE;
        for (enum Gimmick gimmick = GIMMICK_MEGA; gimmick < GIMMICKS_COUNT; gimmick++)
            if (IsGimmickSelected(battler, gimmick))
                return FALSE;
        speed[battler] = GetBattlerTotalSpeedStat(battler, gAiLogicData->abilities[battler], gAiLogicData->holdEffects[battler]);
        // A free attack before switching is only free when the opposing set
        // is actually known. An unrevealed move could be Sucker Punch.
        if (!IsBattlerAlly(actor, battler) && !IsAiBattlerAware(battler))
            return FALSE;
    }
    if (IsBattlerAlive(partner) && partnerAction->index != PAIR_IDLE && PairPivotSensitiveMove(partnerAction->executedMove))
        return FALSE;
    u32 limitations = CheckMoveLimitations(actor, 0, MOVE_LIMITATIONS_ALL);
    u32 bestDamage = 0;
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        enum Move move = gBattleMons[actor].moves[slot];
        if ((move != MOVE_U_TURN && move != MOVE_VOLT_SWITCH && move != MOVE_FLIP_TURN)
         || IsMoveUnusable(slot, move, limitations))
            continue;
        // An earlier partner attack could remove the pivot's target. Retain
        // immediate switching for those cases rather than invent a new action.
        if (IsBattlerAlive(partner) && partnerAction->index != PAIR_IDLE
         && GetMoveEffect(partnerAction->executedMove) != EFFECT_PROTECT
         && partnerAction->executedMove != MOVE_CELEBRATE
         && !PairPivotStrictlyBefore(actor, move, partner, partnerAction->executedMove, speed))
            continue;
        for (enum BattlerId target = 0; target < gBattlersCount; target++)
        {
            if (!IsBattlerAlive(target) || IsBattlerAlly(actor, target)
             || gBattleMons[target].volatiles.substitute || gBattleMons[target].volatiles.rage
             || IsSemiInvulnerable(target, CHECK_ALL))
                continue;
            // The foes' commands are unknown, so every move they could still
            // use is the worst case: the pivot must strictly precede all of
            // them, and any usable pivot-sensitive move cancels the attempt.
            bool32 safe = TRUE;
            for (enum BattlerId foe = 0; foe < gBattlersCount && safe; foe++)
            {
                if (!IsBattlerAlive(foe) || IsBattlerAlly(actor, foe))
                    continue;
                u32 foeLimitations = CheckMoveLimitations(foe, 0, MOVE_LIMITATIONS_ALL);
                for (u32 foeSlot = 0; foeSlot < MAX_MON_MOVES; foeSlot++)
                {
                    enum Move incoming = gBattleMons[foe].moves[foeSlot];
                    if (incoming == MOVE_NONE || IsMoveUnusable(foeSlot, incoming, foeLimitations))
                        continue;
                    if (PairPivotSensitiveMove(incoming))
                        safe = FALSE;
                    else if (IsBattleMoveStatus(incoming))
                    {
                        // A support command cannot punish the pivot's damage.
                        // A shield or a redirector can waste it outright.
                        if (GetMoveEffect(incoming) == EFFECT_PROTECT
                         || GetMoveEffect(incoming) == EFFECT_FOLLOW_ME)
                            safe = FALSE;
                    }
                    else if (!PairPivotStrictlyBefore(actor, move, foe, incoming, speed))
                        safe = FALSE;
                }
            }
            if (!safe || AI_GetMoveAccuracy(gAiLogicData, actor, target, move) < 100)
                continue;
            enum Stat attack = IsBattleMovePhysical(move) ? STAT_ATK : STAT_SPATK;
            enum Stat defense = IsBattleMovePhysical(move) ? STAT_DEF : STAT_SPDEF;
            if (gBattleMons[actor].statStages[attack] < DEFAULT_STAT_STAGE
             || gBattleMons[target].statStages[defense] > DEFAULT_STAT_STAGE)
                continue;
            struct AiCalcValues calc = {.move = move, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
            struct SimulatedDamage damage = AI_CalcDamage(&calc, actor, target);
            // The nonlethal margin includes the maximum ordinary critical
            // multiplier. Avoid faint-triggered effects and displaced KO credit.
            if (!damage.affectsTarget || !damage.minimum || 3u * damage.maximum >= gBattleMons[target].hp
             || AI_GetContactDamage(actor, target, move, gAiLogicData->abilities[actor], gAiLogicData->holdEffects[actor],
                 gAiLogicData->holdEffects[target], gAiLogicData->abilities[target]))
                continue;
            if (damage.minimum > bestDamage)
            {
                bestDamage = damage.minimum;
                *action = (struct PairAction){.move = move, .executedMove = move, .index = slot,
                    .target = target, .score = AI_SCORE_DEFAULT,
                    .priority = AI_GetMovePriority(actor, gAiLogicData->abilities[actor], move)};
            }
        }
    }
    return bestDamage != 0;
}

// Whether evolving this turn gives up an end-of-turn Speed Boost the base form
// would still collect. The battler choosing now is on the field from the turn's
// start, so the boost fires at its end; the Mega keeps a stage it already has.
// Only a healthy holder defers - one likely to fall this turn evolves now.
static bool32 PairMegaForfeitsSpeedBoost(enum BattlerId battler)
{
    // One banked boost is the plan; a second base-form turn only postpones
    // the Mega. Thomas's Scolipede waited two turns, guarding the second, and
    // evolved on the third with the Mega's turns spent in base form.
    if (!IsBattlerAlive(battler) || gAiLogicData->abilities[battler] != ABILITY_SPEED_BOOST
     || gBattleMons[battler].statStages[STAT_SPEED] > DEFAULT_STAT_STAGE
     || gBattleMons[battler].hp * 2 <= gBattleMons[battler].maxHP)
        return FALSE;
    enum Species mega = GetBattleFormChangeTargetSpecies(battler, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM, ABILITY_SPEED_BOOST);
    if (mega == gBattleMons[battler].species)
        mega = GetBattleFormChangeTargetSpecies(battler, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE, ABILITY_SPEED_BOOST);
    if (mega == gBattleMons[battler].species)
        return FALSE;
    for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        if (GetBattlerSpeciesAbility(battler, mega, slot) == ABILITY_SPEED_BOOST)
            return FALSE;
    return TRUE;
}

// Whether this battler's Mega brings back the weather its trainer's plan is
// built on. The trial prices this turn inside the new weather, but the five
// turns after it - Slush Rush, a sure Blizzard, the foe's sun gone - are the
// reason the form exists, and a one-turn board never sees them.
static bool32 PairMegaRestoresPlanWeather(enum BattlerId battler)
{
    if (!IsBattlerAlive(battler) || (gBattleWeather & B_WEATHER_PRIMAL_ANY))
        return FALSE;
    enum Ability ability = gAiLogicData->abilities[battler];
    enum Species mega = GetBattleFormChangeTargetSpecies(battler, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM, ability);
    if (mega == gBattleMons[battler].species)
        mega = GetBattleFormChangeTargetSpecies(battler, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE, ability);
    if (mega == gBattleMons[battler].species)
        return FALSE;
    u32 plan = EmeraldChampions_GetBattlePlan(battler), weather;
    switch (GetBattlerSpeciesAbility(battler, mega, 0))
    {
    case ABILITY_DROUGHT: plan &= EC_BATTLE_PLAN_SUN; weather = B_WEATHER_SUN; break;
    case ABILITY_DRIZZLE: plan &= EC_BATTLE_PLAN_RAIN; weather = B_WEATHER_RAIN; break;
    case ABILITY_SAND_STREAM: plan &= EC_BATTLE_PLAN_SAND; weather = B_WEATHER_SANDSTORM; break;
    case ABILITY_SNOW_WARNING: plan &= EC_BATTLE_PLAN_SNOW; weather = B_WEATHER_ICY_ANY; break;
    default: return FALSE;
    }
    return plan != 0 && !(gBattleWeather & weather);
}

bool32 AI_ComputeDoublesDecisions(enum BattlerId actor)
{
    enum BattlerId partner = GetPartnerBattler(actor);
    u32 skip = 0;
    gAiPairDecisionTruncated = FALSE;
    if (!IsDoubleBattle())
        skip |= AI_PAIR_SKIP_NOT_DOUBLE;
    if (!BattlerHasAi(partner))
        skip |= AI_PAIR_SKIP_NO_PARTNER_AI;
    if (!(gAiThinkingStruct->aiFlags[actor] & AI_FLAG_SMART_MON_CHOICES))
        skip |= AI_PAIR_SKIP_ACTOR_FLAGS;
    if (IsBattlerAlive(partner) && !(gAiThinkingStruct->aiFlags[partner] & AI_FLAG_SMART_MON_CHOICES))
        skip |= AI_PAIR_SKIP_PARTNER_FLAGS;
    if (gAiLogicData->aiPredictionInProgress)
        skip |= AI_PAIR_SKIP_PREDICTION;
    // Two owners on one side each load their own flag word. A difference
    // between them disables the joint search for both, which is invisible
    // from outside; record it so a receipt can say so.
    if ((skip & (AI_PAIR_SKIP_ACTOR_FLAGS | AI_PAIR_SKIP_PARTNER_FLAGS))
     && (skip & (AI_PAIR_SKIP_ACTOR_FLAGS | AI_PAIR_SKIP_PARTNER_FLAGS))
        != (AI_PAIR_SKIP_ACTOR_FLAGS | AI_PAIR_SKIP_PARTNER_FLAGS))
        skip |= AI_PAIR_SKIP_OWNER_MISMATCH;
    gAiPairSkipReason = skip;
    if (!IsBattlerAlive(actor) || skip != 0)
    {
        // The joint search is the only thing in doubles that elects a Mega.
        // When it does not run, the per-battler path has to keep that job
        // rather than silently leaving a usable Mega unused.
        if (IsBattlerAlive(actor) && CanMegaEvolve(actor))
            SetAIUsingGimmick(actor, USE_GIMMICK);
        return FALSE;
    }
    if (gAiLogicData->battlerMovesScored & (1u << actor))
        return TRUE;
#if TESTING
    sTestPairBudgetSpentNow = gTestPairBudgetSpent;
    gTestPairBudgetSpent = FALSE;
    sTestPairBudgetPairsNow = 0;
    if (gTestPairBudgetPairs && GetBattlerSide(actor) == B_SIDE_OPPONENT)
    {
        sTestPairBudgetPairsNow = gTestPairBudgetPairs;
        gTestPairBudgetPairs = 0;
        sTestPairsScored = 0;
    }
#endif
    struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
    struct PairEvaluation *ev = AllocZeroed(sizeof(*ev));
    // This owner restores before every candidate and at decisionReady. A
    // second full snapshot inside each board wastes more than8 KiB of heap.
    u8 reserves[2][PARTY_SIZE + 1] = {{PARTY_SIZE}, {PARTY_SIZE}};
    u32 count[2] = {1, 1};
    enum BattlerId actors[2] = {actor, partner};
    u32 canMega = (CanMegaEvolve(actor) ? 1u : 0u) | (IsBattlerAlive(partner) && CanMegaEvolve(partner) ? 2u : 0u);
    gAiPairMegaTrace[actor] = AI_PAIR_MEGA_RAN | (canMega << 4);
    bool32 deadline[2] = {EC_PerishMustEscape(actor), EC_PerishMustEscape(partner)};
    bool32 earlyPivot[2] = {EC_PerishShouldPivotEarly(actor), EC_PerishShouldPivotEarly(partner)};
    // A position that needs to change is not paying for the turn it gives up:
    // the extra costs below are for leaving a healthy, unpressured board.
    bool32 pressured[2] = {PairNeedsSwitchSearch(actor), PairNeedsSwitchSearch(partner)};
    bool32 uselessLock[2] = {PairUselessLock(actor), PairUselessLock(partner)};
    // What each body leaving would relieve: its share of HP, and whether it
    // is dead weight - held in a useless lock, or with no attack to use.
    u32 outgoingShare[2];
    u8 outgoingLoss[2][MAX_BATTLERS_COUNT][MAX_MON_MOVES];
    bool32 deadWeight[2];
    for (u32 index = 0; index < 2; index++)
    {
        PairOutgoingLoss(actors[index], outgoingLoss[index]);
        outgoingShare[index] = IsBattlerAlive(actors[index])
            ? gBattleMons[actors[index]].hp * 100 / max(1, gBattleMons[actors[index]].maxHP) : 0;
        deadWeight[index] = uselessLock[index];
        if (IsBattlerAlive(actors[index]) && !deadWeight[index])
        {
            deadWeight[index] = TRUE;
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            {
                enum Move move = gBattleMons[actors[index]].moves[slot];
                if (move != MOVE_NONE && !IsBattleMoveStatus(move)
                 && !IsMoveUnusable(slot, move, gAiLogicData->moveLimitations[actors[index]]))
                    deadWeight[index] = FALSE;
            }
        }
    }
    u32 revealMega = 0;
    for (u32 index = 0; index < 2; index++)
        if ((canMega & (1u << index)) && !deadline[index]
         && IsBattlersFirstTurn(actors[index])
         && (EmeraldChampions_GetBattlePlan(actors[index]) & EC_BATTLE_PLAN_MEGA_REVEAL))
        {
            revealMega = 1u << index;
            break;
        }
    bool32 bothMegaLegal = canMega == 3
        && !(IsPartnerMonFromSameTrainer(actor) && GetRemainingMegaEvolutions(actor) < 2);
    bool32 boostBeforeMega[2] = {FALSE, FALSE}, megaWeather[2] = {FALSE, FALSE};
    for (u32 index = 0; index < 2; index++)
        if (canMega & (1u << index))
        {
            boostBeforeMega[index] = PairMegaForfeitsSpeedBoost(actors[index]);
            megaWeather[index] = PairMegaRestoresPlanWeather(actors[index]);
        }
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
            // Under pressure the ranking only shortlists: the second reserve
            // is priced on the board too, so a ranking error costs a board
            // rather than a body walking into the hit it was meant to dodge.
            if (pressured && secondRank != INT_MIN)
                count[index] = 3;
        }
    // A board that runs out of budget before its first pair leaves nothing
    // behind, so start from a defined legal action rather than stack contents.
    struct PairAction chosen[2], bestActions[2];
    for (u32 index = 0; index < 2; index++)
        bestActions[index] = (struct PairAction){MOVE_NONE, AI_SCORE_DEFAULT, PAIR_IDLE, actors[index]};
    u32 bestReserves[2] = {PARTY_SIZE, PARTY_SIZE};
    sPairVisibleForecast.valid = FALSE;
    u32 bestMega = 0, bestTieCost = UINT_MAX;
    s32 bestStay = INT_MIN; // Best board that keeps both bodies, for the trace.
    // Whether that board spends each body on a repeated shield, which fails
    // more often than it holds.
    bool32 stayRepeatGuard[2] = {FALSE, FALSE};
    s32 bestNonGuard[2] = {INT_MIN, INT_MIN}; // Best board where each attacked.
    u32 activeMega = 0;
    s32 best = INT_MIN;
    // Every first-choice reserve is priced before any second choice, so the
    // clock never spends the actor's best exit on the partner's alternate.
    static const u8 sPairBoardOrder[][2] = {{0, 0}, {0, 1}, {1, 0}, {0, 2}, {2, 0}, {1, 1}, {1, 2}, {2, 1}, {2, 2}};
    for (u32 order = 0; order < ARRAY_COUNT(sPairBoardOrder); order++)
    {
        u32 left = sPairBoardOrder[order][0], right = sPairBoardOrder[order][1];
        if (left >= count[0] || right >= count[1])
            continue;
        {
            u32 slots[2] = {reserves[0][left], reserves[1][right]};
            u32 aceFalls = 2; // Unknown until the board is first loaded.
            // Voluntary double switches spend both actions and multiply the
            // candidate space. Forced double replacements are ranked separately.
            if (slots[0] < PARTY_SIZE && slots[1] < PARTY_SIZE)
            {
                if (!(deadline[0] && deadline[1]))
                    continue;
                if (slots[0] == slots[1] && IsPartnerMonFromSameTrainer(actor))
                    continue;
            }
            for (u32 megaChoice = 0; megaChoice < 4; megaChoice++)
            {
                // Evaluate an authored first-entry reveal before the optional
                // base form, so the decision budget cannot silently skip it.
                // Once a valid reveal board exists, compare its move/target
                // choices and partner pivots, not hiding the story's reveal.
                // If native form application fails, ordinary boards remain
                // available; this directive never grants Mega eligibility.
                // Evaluate the board where every usable Mega evolves first.
                // Ordinarily both orders are compared, but a decision that
                // runs out of budget keeps whichever board it saw, and in a
                // multi - three actors sharing one clock - the stop landed
                // immediately after the plain board every time, so an
                // available Mega was never once considered. Singles elect a
                // usable Mega by default; this makes doubles agree when the
                // comparison cannot be afforded.
                // The reveal board was ordered first on its own, which in a
                // two-owner multi put Courtney's reveal ahead of the board
                // where Maxie's Camerupt evolves too, and the shared clock
                // stopped between them every time. The board where every
                // usable Mega evolves already contains the reveal, so it
                // leads whenever it is legal; the reveal alone leads only
                // when one owner cannot afford both.
                u32 mega = megaChoice ^ (bothMegaLegal || !revealMega ? canMega : revealMega);
                if (revealMega && best != INT_MIN && (bestMega & revealMega)
                 && !(mega & revealMega))
                    continue;
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
                if ((mega == 3 && IsPartnerMonFromSameTrainer(actor)
                     && GetRemainingMegaEvolutions(actor) < 2)
                    || (mega & ~canMega)
                    || ((mega & 1) && slots[0] < PARTY_SIZE)
                    || ((mega & 2) && slots[1] < PARTY_SIZE))
                    continue;
                if (aceFalls == 1)
                    continue;
                AI_RestoreCandidateState(state);
                u32 noActionMask = 0;
                // Evolving is free. Counting it as a tie-break cost made a
                // lead Mega decline on any board where the Mega and base
                // boards scored the same - a Mega that opens with Protect,
                // for instance, stayed in base form and evolved a turn later
                // for nothing. Switching still costs a tie.
                u32 tieCost = (slots[0] < PARTY_SIZE) + (slots[1] < PARTY_SIZE);
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
                // An ace that leaves the bench to end a useless lock has to
                // live to act. If the foes' visible attacks are expected to
                // remove it on arrival, the exit only trades the ace for the
                // one turn the lock would have cost, however the rest of the
                // board prices that trade. Planned exits are not voluntary.
                if (aceFalls == 2)
                {
                    aceFalls = 0;
                    for (u32 index = 0; index < 2; index++)
                        if (slots[index] < PARTY_SIZE && uselessLock[index]
                         && !deadline[index] && !earlyPivot[index]
                         && IsAceMon(actors[index], slots[index])
                         && PairEntryExpectedKo(actors[index]))
                            aceFalls = 1;
                    if (aceFalls)
                        continue;
                }
                bool32 valid = TRUE;
                for (u32 index = 0; index < 2; index++)
                    if ((mega & (1u << index)) && !AI_ApplyMegaCandidate(actors[index], FALSE))
                    {
                        valid = FALSE;
                        gAiPairMegaTrace[actor] |= AI_PAIR_MEGA_APPLY_FAILED;
                    }
                if (!valid)
                    continue;
                // A Mega board needs its damage caches rebuilt around the new
                // form, and that refresh is abandoned when the clock has run
                // out - which in a multi is always, because three actors share
                // one budget and the Mega masks come after the plain board. So
                // every Mega board scored INT_MIN and no Mega ever evolved in
                // a two-owner multi. The refresh a form change requires is not
                // optional work: it is what makes the comparison meaningful.
                // A Mega board has to rebuild the damage caches around the new
                // form, and both of this search's stops would otherwise throw
                // that board away: the clock abandons the refresh, and a board
                // the caller may not stop on returns nothing at all. In an
                // ordinary double the clock still has room when the Mega masks
                // come up; in a two-owner multi three actors share one budget
                // and it never does, so every Mega board scored INT_MIN and no
                // Mega ever evolved. A form change is not optional work, so
                // the Mega boards finish the way a countdown exit does.
                s32 boardNonGuard[2];
                // The unchanged board is the one the foes choose against.
                sPairVisibleForecast.mode = slots[0] < PARTY_SIZE || slots[1] < PARTY_SIZE ? PAIR_VISIBLE_FORECAST_USE
                    : mega == 0 ? PAIR_VISIBLE_FORECAST_CAPTURE : PAIR_VISIBLE_FORECAST_NONE;
                s32 score = EvaluatePairBoard(actor, noActionMask, chosen, ev,
                    mega != 0 || noActionMask != 0, canStop,
                    deadline[0] || deadline[1] || mega != 0, boardNonGuard);
                if (score == INT_MIN)
                    continue;
                // The adjustments below belong to the whole board, so they
                // apply equally to its attacking pairs. Carry them across so
                // the margin compares two numbers on one scale.
                s32 boardRaw = score;
                if (mega != 0)
                {
                    // Each evolving body earns its own horizon: a board where
                    // both owners evolve is not worth the same as one Mega.
                    // A Speed Boost holder whose Mega loses the ability earns
                    // +1 Speed by ending this turn in base form, and the Mega
                    // keeps the stage. Evolving now spends that boost and
                    // only brings the horizon forward a turn, so its board
                    // has to win on this turn alone, by the boost it forfeits.
                    // A Mega that brings back its plan's weather earns the
                    // turns of that weather the board cannot see.
                    for (u32 index = 0; index < 2; index++)
                        if (mega & (1u << index))
                            score += (boostBeforeMega[index] ? -PAIR_SETUP_HORIZON : PAIR_MEGA_HORIZON)
                                + (megaWeather[index] ? PAIR_SETUP_HORIZON : 0);
                    gAiPairMegaTrace[actor] |= AI_PAIR_MEGA_SCORED;
                }
                // Demand a meaningful improvement before voluntarily giving up
                // an action; ties and tiny forecast noise must not cause cycling.
                // A countdown exit is not voluntary, and neither is the
                // authored early pivot that leaves while the partner still
                // holds the trap: both are the plan, not a change of mind.
                // Everything else must still earn its lost action.
                bool32 forcedExit = FALSE, plannedExit = FALSE;
                for (u32 index = 0; index < 2; index++)
                {
                    if (slots[index] >= PARTY_SIZE)
                        continue;
                    if (deadline[index])
                        forcedExit = TRUE;
                    if (deadline[index] || earlyPivot[index])
                        plannedExit = TRUE;
                }
                if (noActionMask && !plannedExit)
                {
                    for (u32 index = 0; index < 2; index++)
                    {
                        if (slots[index] >= PARTY_SIZE)
                            continue;
                        // The body coming in is the one that eats the turn. A
                        // reserve that a visible move removes on arrival has
                        // not improved the position; it has spent a member to
                        // change the sprite. Live play produced this over and
                        // over, and the exits were winning by 14 and 38 points
                        // - well inside a body. See PairEntryCost.
                        score -= PairEntryCost(actors[index], deadWeight[index], outgoingShare[index], outgoingLoss[index]);
                        // The commitment is the attack a pivot gives up. A body
                        // whose best stay is a repeated shield gives up none: it
                        // spends the turn either way, and the shield fails two
                        // times in three. Wallace's Tapu Fini shielded a second
                        // time into a faster Kartana's Leaf Blade, which the
                        // failed shield let through, while the switch was the
                        // better board before this cost.
                        if (!stayRepeatGuard[index])
                            score -= PAIR_SWITCH_COMMITMENT;
                        if (pressured[index])
                            continue;
                        score -= PairForfeitedAttackValue(actors[index]);
                        // Turn one from full health, with nothing revealed, is
                        // the worst moment to hand over a free turn for a
                        // matchup guess.
                        if (IsBattlersFirstTurn(actors[index])
                         && gBattleMons[actors[index]].hp == gBattleMons[actors[index]].maxHP)
                            score -= PAIR_SWITCH_BLIND_COST;
                    }
                }
                if (forcedExit)
                    score += 200;
                for (u32 index = 0; index < 2; index++)
                    if (uselessLock[index] && slots[index] < PARTY_SIZE)
                        score += PAIR_USELESS_LOCK_ESCAPE;
                // An authored singer leaves one turn early while its partner
                // still traps the affected foes. This is a conditional phase
                // preference, never an excuse to bypass reserve legality.
                for (u32 index = 0; index < 2; index++)
                    if (earlyPivot[index] && slots[index] < PARTY_SIZE)
                        score += 70;
                if (slots[0] >= PARTY_SIZE && slots[1] >= PARTY_SIZE && score > bestStay)
                {
                    bestStay = score;
                    for (u32 index = 0; index < 2; index++)
                        stayRepeatGuard[index] = PairIsPassiveGuard(&chosen[index])
                            && ev->protectChance[actors[index]][chosen[index].index] < 100;
                }
                for (u32 index = 0; index < 2; index++)
                    if (boardNonGuard[index] != INT_MIN
                     && boardNonGuard[index] + (score - boardRaw) > bestNonGuard[index])
                        bestNonGuard[index] = boardNonGuard[index] + (score - boardRaw);
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
#if TESTING
    sTestPairBudgetSpentNow = FALSE;
    sTestPairBudgetPairsNow = 0;
#endif
    sPairVisibleForecast.mode = PAIR_VISIBLE_FORECAST_NONE;
    AI_RestoreCandidateState(state);
    bool32 attackPivot[2] = {FALSE, FALSE};
    if (!bestMega)
        for (u32 index = 0; index < 2; index++)
            if (bestReserves[index] < PARTY_SIZE && bestReserves[index ^ 1] == PARTY_SIZE)
                attackPivot[index] = PairTryAttackBeforeSwitch(actors[index], bestReserves[index],
                    &bestActions[index ^ 1], &bestActions[index]);
    // Queries must not leak scratch data or RNG into the real turn.
    AI_RestoreCandidateState(state);
    AI_FreeCandidateState(state);
    FreePairEvaluation(ev);
    // No board produced a scored pair, so the actions above are still the
    // placeholder. Hand the turn to the per-battler scorer rather than the
    // placeholder's self-aimed slot zero.
    if (best == INT_MIN)
    {
        if (CanMegaEvolve(actor))
            SetAIUsingGimmick(actor, USE_GIMMICK);
        return FALSE;
    }
    for (u32 index = 0; index < 2; index++)
    {
        enum BattlerId battler = actors[index];
        gBattleStruct->prevTurnSpecies[battler] = gBattleMons[battler].species;
        gAiLogicData->shouldSwitch &= ~(1u << battler);
        gAiLogicData->mostSuitableMonId[battler] = bestReserves[index];
        gAiLogicData->monToSwitchInId[battler] = bestReserves[index];
        gBattleStruct->AI_monToSwitchIntoId[battler] = bestReserves[index];
        if (bestReserves[index] < PARTY_SIZE && !attackPivot[index])
        {
            gAiLogicData->shouldSwitch |= 1u << battler;
            // How much the exit beat the best board that keeps both bodies.
            // A switch that wins by a hair is a different story from one that
            // wins by a mile, and the receipts cannot tell them apart.
            s32 margin = bestStay == INT_MIN ? 999 : best - bestStay;
            gAiSwitchTrace[battler] = AI_SWITCH_FROM_PAIR
                | (PairDecisionBudgetExpired() ? AI_SWITCH_BUDGET_GONE : 0)
                | (gAiPairDecisionTruncated ? AI_SWITCH_TRUNCATED : 0)
                | (bestStay != INT_MIN ? AI_SWITCH_HAD_STAY : 0)
                | ((bestReserves[index] & 7) << AI_SWITCH_SLOT_SHIFT)
                | ((u32)(margin < 0 ? 0 : margin > 4095 ? 4095 : margin) << 16);
        }
        // Never hand the controller a move it cannot select: the selection
        // script would reject it and ask again, and the answer never changes.
        // With every move unusable, slot zero is the native Struggle path.
        u32 chosenIndex = bestActions[index].index == PAIR_IDLE ? 0 : bestActions[index].index;
        u32 limitations = IsBattlerAlive(battler)
            ? CheckMoveLimitations(battler, 0, MOVE_LIMITATIONS_ALL) : 0;
        if (limitations != (1u << MAX_MON_MOVES) - 1 && (limitations & (1u << chosenIndex)))
        {
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                if (!(limitations & (1u << slot)))
                {
                    chosenIndex = slot;
                    break;
                }
        }
        if (bestActions[index].index != PAIR_IDLE
         && GetMoveEffect(bestActions[index].move) == EFFECT_PROTECT)
        {
            s32 margin = bestNonGuard[index] == INT_MIN ? 4095 : best - bestNonGuard[index];
            gAiGuardTrace[battler] = 1
                | ((u32)(margin < 0 ? 0 : margin > 4095 ? 4095 : margin) << 16);
        }
        else
        {
            gAiGuardTrace[battler] = 0;
        }
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        {
            u32 best = 0;
            for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                if (IsBattlerAlive(foe) && !IsBattlerAlly(battler, foe)
                 && gAiLogicData->simulatedDmg[battler][foe][slot].median > best)
                    best = gAiLogicData->simulatedDmg[battler][foe][slot].median;
            gAiMoveDamageTrace[battler][slot] = best;
        }
        gAiBattleData->chosenMoveIndex[battler] = chosenIndex;
        // The fallback action this search starts from carries the actor as its
        // own target, which is a placeholder and not a decision. If nothing
        // ever replaced it - a second owner's battler in a multi whose actions
        // were never enumerated - the engine was handed an ordinary attack
        // aimed at the body using it, and the turn did nothing at all.
        enum BattlerId chosenTarget = bestActions[index].target;
        if (chosenTarget == battler && bestActions[index].index != PAIR_IDLE
         && !IsBattleMoveStatus(gBattleMons[battler].moves[chosenIndex])
         && AI_GetBattlerMoveTargetType(battler, gBattleMons[battler].moves[chosenIndex]) == TARGET_SELECTED)
        {
            for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
                if (IsBattlerAlive(foe) && !IsBattlerAlly(battler, foe))
                {
                    chosenTarget = foe;
                    break;
                }
        }
        gAiBattleData->chosenTarget[battler] = chosenTarget;
        gAiPairMegaTrace[actor] = (gAiPairMegaTrace[actor] & ~AI_PAIR_MEGA_BEST_MASK)
            | ((bestMega & 3) << 6);
        SetAIUsingGimmick(battler, bestMega & (1u << index) ? USE_GIMMICK : NO_GIMMICK);
        gAiLogicData->battlerMovesScored |= 1u << battler;
    }
    return TRUE;
}
