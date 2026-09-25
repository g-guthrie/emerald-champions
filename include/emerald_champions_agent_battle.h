#ifndef GUARD_EMERALD_CHAMPIONS_AGENT_BATTLE_H
#define GUARD_EMERALD_CHAMPIONS_AGENT_BATTLE_H

#include "global.h"

#if EC_HEADLESS_FIXTURES

#include "constants/battle.h"

// One fixed-size observation buffer the host reads word by word. Keep the total
// inside the runner's 512 --read requests. See scripts/playthrough/battle_driver.py.
#define EC_AGENT_BATTLE_VIEW_WORDS   500
#define EC_AGENT_BATTLE_BATTLER_BASE 32
#define EC_AGENT_BATTLE_BATTLER_SIZE 28
#define EC_AGENT_BATTLE_PARTY_BASE   144
#define EC_AGENT_BATTLE_PARTY_SIZE   12
#define EC_AGENT_BATTLE_FOE_BASE     216
#define EC_AGENT_BATTLE_FOE_SIZE     3
#define EC_AGENT_BATTLE_LEGAL_BASE   252
#define EC_AGENT_BATTLE_LEGAL_SIZE   6
#define EC_AGENT_BATTLE_MSG_BASE     276
#define EC_AGENT_BATTLE_MSG_SIZE     14
#define EC_AGENT_BATTLE_MSG_COUNT    14
#define EC_AGENT_BATTLE_FIELD_BASE   488
#define EC_AGENT_BATTLE_PREV_BASE    472
#define EC_AGENT_BATTLE_PREV_SIZE    4
#define EC_AGENT_BATTLE_MSG_CHARS    ((EC_AGENT_BATTLE_MSG_SIZE - 1) * 4)
// Words after the field block. The event log itself lives outside the view.
#define EC_AGENT_BATTLE_LOG_HEAD_WORD 492 // bytes ever written to the event log
#define EC_AGENT_BATTLE_LOG_SIZE_WORD 493 // the log's capacity in bytes
#define EC_AGENT_BATTLE_MAP_FIELD_WORD 494 // applied map weather/environment

// The complete battle event log: every battle message, every move use and
// every HP change, as a byte ring the host reads by range after each decision.
// The 14-entry message ring in the view is kept only for older hosts.
// Record: [kind][payload length][payload...].
#define EC_AGENT_BATTLE_LOG_BYTES    8192
#define EC_AGENT_BATTLE_LOG_TEXT_MAX 255

enum EmeraldChampionsAgentBattleLogKind
{
    EC_AGENT_LOG_NONE,
    EC_AGENT_LOG_TEXT, // game-charset text, no terminator
    EC_AGENT_LOG_MOVE, // attacker, target, move (u16)
    EC_AGENT_LOG_HP,   // battler, attacker, old HP (u16), new HP (u16), move (u16), action
    EC_AGENT_LOG_POPUP, // battler, 0 ability / 1 item, id (u16): shown without any text
};

enum EmeraldChampionsAgentBattlePhase
{
    EC_AGENT_BATTLE_PHASE_IDLE,
    EC_AGENT_BATTLE_PHASE_STARTING,
    EC_AGENT_BATTLE_PHASE_RUNNING,
    EC_AGENT_BATTLE_PHASE_AWAIT_ACTION,
    EC_AGENT_BATTLE_PHASE_AWAIT_SWITCH,
    EC_AGENT_BATTLE_PHASE_ENDED,
};

enum EmeraldChampionsAgentBattleResult
{
    EC_AGENT_BATTLE_PENDING,
    EC_AGENT_BATTLE_OK,
    EC_AGENT_BATTLE_BAD_COMMAND,
    EC_AGENT_BATTLE_BAD_TRAINER,
    EC_AGENT_BATTLE_BAD_PARTY,
    EC_AGENT_BATTLE_NOT_READY,
};

// Why the engine will refuse a switch for this battler right now. These mirror
// the branches of HandleTurnActionSelectionState's B_ACTION_SWITCH gate.
enum EmeraldChampionsAgentSwitchBlock
{
    EC_AGENT_SWITCH_ALLOWED,
    EC_AGENT_SWITCH_BLOCKED_ARENA,
    EC_AGENT_SWITCH_BLOCKED_COMMANDER,
    EC_AGENT_SWITCH_BLOCKED_TRAPPED,
    EC_AGENT_SWITCH_BLOCKED_ABILITY,
};

// Per-battler command kinds written by the host into the mailbox.
enum EmeraldChampionsAgentBattleAction
{
    EC_AGENT_BATTLE_ACTION_NONE,
    EC_AGENT_BATTLE_ACTION_MOVE,
    EC_AGENT_BATTLE_ACTION_SWITCH,
};

extern volatile u32 gEcAgentBattleCommand;
extern volatile u32 gEcAgentBattleResult;
extern volatile u32 gEcAgentBattleTrainerA;
extern volatile u32 gEcAgentBattleTrainerB;
extern volatile u32 gEcAgentBattlePartner;
extern volatile u32 gEcAgentBattlePhase;
extern volatile u32 gEcAgentBattleSerial;
extern volatile u32 gEcAgentBattleSeed;
extern volatile u32 gEcAgentBattleHalted;
extern volatile u32 gEcAgentBattleNeedMask;
extern volatile u32 gEcAgentBattleAction[MAX_BATTLERS_COUNT];
extern volatile u32 gEcAgentBattleMoveIndex[MAX_BATTLERS_COUNT];
extern volatile u32 gEcAgentBattleTarget[MAX_BATTLERS_COUNT];
extern volatile u32 gEcAgentBattleMega[MAX_BATTLERS_COUNT];
extern volatile u32 gEcAgentBattleSwitchSlot[MAX_BATTLERS_COUNT];
extern volatile u32 gEcAgentBattleView[EC_AGENT_BATTLE_VIEW_WORDS];
extern volatile u8 gEcAgentBattleLog[EC_AGENT_BATTLE_LOG_BYTES];
// Written by the host before the start command. 0 keeps the headless room's
// own value; otherwise the value is the OVERWORLD weather / environment + 1.
extern volatile u32 gEcAgentBattleMapWeather;
extern volatile u32 gEcAgentBattleEnvironment;

bool32 EmeraldChampionsAgentBattleActive(void);
void EmeraldChampionsAgentBattlePoll(void);
void EmeraldChampionsAgentBattleBegin(u32 levelCap, u32 difficulty);
void EmeraldChampionsAgentBattleText(const u8 *text);
void EmeraldChampionsAgentBattleMoveUsed(u32 attacker, u32 target, u32 move);
void EmeraldChampionsAgentBattleHp(u32 battler, u32 oldHp, u32 newHp);
void EmeraldChampionsAgentBattlePopUp(u32 battler, bool32 isItem, u32 id);
// Player-side controller entry points. Each parks the battler on a serving
// function that answers only from the mailbox; no menus and no player-side AI.
void EmeraldChampionsAgentBattleChooseAction(enum BattlerId battler);
void EmeraldChampionsAgentBattleChooseMove(enum BattlerId battler);
void EmeraldChampionsAgentBattleChoosePokemon(enum BattlerId battler);

#else

static inline void EmeraldChampionsAgentBattlePoll(void) {}
static inline void EmeraldChampionsAgentBattleText(const u8 *text) { (void)text; }
static inline void EmeraldChampionsAgentBattleMoveUsed(u32 attacker, u32 target, u32 move) { (void)attacker; (void)target; (void)move; }
static inline void EmeraldChampionsAgentBattleHp(u32 battler, u32 oldHp, u32 newHp) { (void)battler; (void)oldHp; (void)newHp; }
static inline void EmeraldChampionsAgentBattlePopUp(u32 battler, bool32 isItem, u32 id) { (void)battler; (void)isItem; (void)id; }

#endif

#endif // GUARD_EMERALD_CHAMPIONS_AGENT_BATTLE_H
