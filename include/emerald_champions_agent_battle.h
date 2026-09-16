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
#define EC_AGENT_BATTLE_PREV_BASE    472
#define EC_AGENT_BATTLE_PREV_SIZE    4
#define EC_AGENT_BATTLE_MSG_CHARS    ((EC_AGENT_BATTLE_MSG_SIZE - 1) * 4)

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

bool32 EmeraldChampionsAgentBattleActive(void);
void EmeraldChampionsAgentBattlePoll(void);
void EmeraldChampionsAgentBattleBegin(u32 levelCap, u32 difficulty);
void EmeraldChampionsAgentBattleText(const u8 *text);
// Player-side controller entry points. Each parks the battler on a serving
// function that answers only from the mailbox; no menus and no player-side AI.
void EmeraldChampionsAgentBattleChooseAction(enum BattlerId battler);
void EmeraldChampionsAgentBattleChooseMove(enum BattlerId battler);
void EmeraldChampionsAgentBattleChoosePokemon(enum BattlerId battler);

#else

static inline void EmeraldChampionsAgentBattlePoll(void) {}
static inline void EmeraldChampionsAgentBattleText(const u8 *text) { (void)text; }

#endif

#endif // GUARD_EMERALD_CHAMPIONS_AGENT_BATTLE_H
