#include "global.h"
#include "emerald_champions_agent_battle.h"

#if EC_HEADLESS_FIXTURES

#include "battle.h"
#include "battle_controllers.h"
#include "battle_gimmick.h"
#include "battle_script_commands.h"
#include "battle_setup.h"
#include "data.h"
#include "battle_util.h"
#include "item.h"
#include "battle_partner.h"
#include "overworld.h"
#include "party_menu.h"
#include "script.h"
#include "caps.h" // GetCurrentLevelCap for the observation view
#include "difficulty.h"
#include "event_data.h"
#include "main.h"
#include "load_save.h"
#include "pokemon.h"
#include "random.h"
#include "constants/battle.h"
#include "constants/battle_partner.h"
#include "constants/battle_util.h"
#include "constants/opponents.h"
#include "constants/party_menu.h"
#include "constants/characters.h"

// Headless agent battle bridge. A host process starts one authored trainer
// battle, then answers every player decision point from this mailbox. Opponent
// battlers keep the ordinary AI, its ordinary turn-start timing and its ordinary
// party knowledge; nothing here is available in the release ROM.

EWRAM_DATA volatile u32 gEcAgentBattleCommand = 0;
EWRAM_DATA volatile u32 gEcAgentBattleResult = EC_AGENT_BATTLE_PENDING;
EWRAM_DATA volatile u32 gEcAgentBattleTrainerA = TRAINER_NONE;
EWRAM_DATA volatile u32 gEcAgentBattleTrainerB = TRAINER_NONE;
EWRAM_DATA volatile u32 gEcAgentBattlePartner = PARTNER_NONE;
EWRAM_DATA volatile u32 gEcAgentBattlePhase = EC_AGENT_BATTLE_PHASE_IDLE;
EWRAM_DATA volatile u32 gEcAgentBattleSerial = 0;
EWRAM_DATA volatile u32 gEcAgentBattleSeed = 0;
// One word the host can stop on: the ROM is parked and waiting for the host.
EWRAM_DATA volatile u32 gEcAgentBattleHalted = 0;
EWRAM_DATA volatile u32 gEcAgentBattleNeedMask = 0;
EWRAM_DATA volatile u32 gEcAgentBattleAction[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA volatile u32 gEcAgentBattleMoveIndex[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA volatile u32 gEcAgentBattleTarget[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA volatile u32 gEcAgentBattleMega[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA volatile u32 gEcAgentBattleSwitchSlot[MAX_BATTLERS_COUNT] = {0};
EWRAM_DATA volatile u32 gEcAgentBattleView[EC_AGENT_BATTLE_VIEW_WORDS] = {0};

static EWRAM_DATA bool8 sBridgeArmed = FALSE;
static EWRAM_DATA bool8 sBattleSeen = FALSE;
static EWRAM_DATA u8 sLastAction[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u8 sLastMovePos[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u8 sLastTarget[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u16 sLastMove[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u8 sLastSelection[MAX_BATTLERS_COUNT] = {0};
// The turn that just finished, kept readable at the next decision point.
static EWRAM_DATA u8 sPrevAction[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u8 sPrevMovePos[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u8 sPrevTarget[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u16 sPrevMove[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u32 sDecisionStart = 0;
static EWRAM_DATA u16 sLastTurn = 0;
static EWRAM_DATA u32 sAiDecisionFrames = 0;
static EWRAM_DATA u32 sAiSetupFrames = 0;
static EWRAM_DATA bool8 sAiDecisionLatched = FALSE;
static EWRAM_DATA u16 sRevealed[MAX_BATTLE_TRAINERS] = {0};
static EWRAM_DATA u32 sMessageSerial = 0;
static EWRAM_DATA u8 sPendingSwitchSlot[MAX_BATTLERS_COUNT] = {0};
static EWRAM_DATA u8 sLevelCap = 0;
// The engine's own refusal of the last submitted switch, per battler.
static EWRAM_DATA u8 sSwitchRefused[MAX_BATTLERS_COUNT] = {0};
// Latched while the battle is live; the parties are restored once it ends.
static EWRAM_DATA u8 sPlayerFaints = 0;
static EWRAM_DATA u8 sOpponentFaints = 0;
static EWRAM_DATA u8 sFinalOutcome = 0;
// The field the battle actually opened with, before any move could change it.
static EWRAM_DATA u8 sOpeningTerrain = 0;
static EWRAM_DATA u16 sOpeningWeather = 0;

#define NO_PENDING_SWITCH 0xFF

bool32 EmeraldChampionsAgentBattleActive(void)
{
    return sBridgeArmed;
}

// A clean headless boot asks for the campaign cap and difficulty that the
// authored level offsets are read against. The cap comes from the one milestone
// table in caps.c; this never introduces a second level formula.
void EmeraldChampionsAgentBattleBegin(u32 levelCap, u32 difficulty)
{
    sBridgeArmed = TRUE;
    sBattleSeen = FALSE;
    sLastTurn = 0xFFFF;
    sAiDecisionFrames = 0;
    sAiSetupFrames = 0;
    sAiDecisionLatched = FALSE;
    sMessageSerial = 0;
    sLevelCap = levelCap;
    sPlayerFaints = 0;
    sOpponentFaints = 0;
    sFinalOutcome = 0;
    sOpeningTerrain = 0;
    sOpeningWeather = 0;
    gEcAgentBattlePhase = EC_AGENT_BATTLE_PHASE_IDLE;
    gEcAgentBattleSerial = 0;
    gEcAgentBattleNeedMask = 0;
    for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gEcAgentBattleAction[i] = EC_AGENT_BATTLE_ACTION_NONE;
        sPendingSwitchSlot[i] = NO_PENDING_SWITCH;
        sLastAction[i] = B_ACTION_NONE;
        sLastSelection[i] = 0;
        sSwitchRefused[i] = EC_AGENT_SWITCH_ALLOWED;
    }
    for (u32 i = 0; i < MAX_BATTLE_TRAINERS; i++)
        sRevealed[i] = 0;
    if (difficulty < DIFFICULTY_COUNT)
        SetCurrentDifficultyLevel(difficulty);
    // The campaign cap is milestone flags, and the one milestone table lives in
    // caps.c. The host reads that table and sets the flags through the existing
    // Studio flag command before preparing the party, so this bridge neither
    // copies the table nor edits a file the trainer side owns.
    SeedRng(gEcAgentBattleSeed);
    SeedRng2(gEcAgentBattleSeed ^ 0x9E3779B9);
}

static void ClearCommand(enum BattlerId battler)
{
    gEcAgentBattleAction[battler] = EC_AGENT_BATTLE_ACTION_NONE;
    gEcAgentBattleNeedMask &= ~(1u << battler);
}

static void RequestCommand(enum BattlerId battler, u32 phase)
{
    if (!(gEcAgentBattleNeedMask & (1u << battler)))
    {
        gEcAgentBattleNeedMask |= 1u << battler;
        gEcAgentBattleSerial++;
    }
    gEcAgentBattlePhase = phase;
}

// ---------------------------------------------------------------- controllers

static void ServeChooseAction(enum BattlerId battler)
{
    u32 action = gEcAgentBattleAction[battler];

    if (action == EC_AGENT_BATTLE_ACTION_NONE)
    {
        RequestCommand(battler, EC_AGENT_BATTLE_PHASE_AWAIT_ACTION);
        return;
    }
    gEcAgentBattleNeedMask &= ~(1u << battler);
    if (action == EC_AGENT_BATTLE_ACTION_SWITCH)
    {
        sPendingSwitchSlot[battler] = gEcAgentBattleSwitchSlot[battler];
        ClearCommand(battler);
        BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_SWITCH, 0);
    }
    else
    {
        BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_USE_MOVE, 0);
    }
    BtlController_Complete(battler);
}

static void ServeChooseMove(enum BattlerId battler)
{
    u32 moveIndex = gEcAgentBattleMoveIndex[battler];
    u32 target = gEcAgentBattleTarget[battler];
    bool32 mega = gEcAgentBattleMega[battler] != 0;
    enum Gimmick usable = gBattleStruct->gimmick.usableGimmick[battler];

    if (gEcAgentBattleAction[battler] != EC_AGENT_BATTLE_ACTION_MOVE)
    {
        RequestCommand(battler, EC_AGENT_BATTLE_PHASE_AWAIT_ACTION);
        return;
    }
    if (moveIndex >= MAX_MON_MOVES)
        moveIndex = 0;
    if (target >= MAX_BATTLERS_COUNT)
        target = battler;
    if (mega && (usable == GIMMICK_NONE || HasTrainerUsedGimmick(battler, usable)))
        mega = FALSE;
    ClearCommand(battler);
    gBattlerTarget = target;
    BtlController_EmitTwoReturnValues(battler, B_COMM_TO_ENGINE, B_ACTION_EXEC_SCRIPT,
                                      moveIndex | (mega ? RET_GIMMICK : 0) | (target << 8));
    BtlController_Complete(battler);
}

static void ServeChoosePokemon(enum BattlerId battler)
{
    u32 caseId = gBattleResources->bufferA[battler][1];
    u32 slot;

    // The engine refuses the switch here exactly as it does to the native menu,
    // which answers PARTY_SIZE and is sent back to action selection. Drop the
    // command so the bridge halts for a fresh one instead of resubmitting it.
    if (caseId == PARTY_ACTION_CANT_SWITCH || caseId == PARTY_ACTION_ABILITY_PREVENTS)
    {
        sSwitchRefused[battler] = (caseId == PARTY_ACTION_ABILITY_PREVENTS)
            ? EC_AGENT_SWITCH_BLOCKED_ABILITY : EC_AGENT_SWITCH_BLOCKED_TRAPPED;
        sPendingSwitchSlot[battler] = NO_PENDING_SWITCH;
        ClearCommand(battler);
        BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, PARTY_SIZE, NULL);
        BtlController_Complete(battler);
        return;
    }
    sSwitchRefused[battler] = EC_AGENT_SWITCH_ALLOWED;

    if (sPendingSwitchSlot[battler] != NO_PENDING_SWITCH)
    {
        slot = sPendingSwitchSlot[battler];
        sPendingSwitchSlot[battler] = NO_PENDING_SWITCH;
    }
    else if (gEcAgentBattleAction[battler] == EC_AGENT_BATTLE_ACTION_SWITCH)
    {
        slot = gEcAgentBattleSwitchSlot[battler];
        ClearCommand(battler);
    }
    else
    {
        RequestCommand(battler, EC_AGENT_BATTLE_PHASE_AWAIT_SWITCH);
        return;
    }
    if (slot >= PARTY_SIZE)
        slot = 0;
    gSelectedMonPartyId = slot;
    gBattleStruct->monToSwitchIntoId[battler] = slot;
    BtlController_EmitChosenMonReturnValue(battler, B_COMM_TO_ENGINE, slot, NULL);
    BtlController_Complete(battler);
}

void EmeraldChampionsAgentBattleChooseAction(enum BattlerId battler)
{
    gBattlerControllerFuncs[battler] = ServeChooseAction;
}

void EmeraldChampionsAgentBattleChooseMove(enum BattlerId battler)
{
    gBattlerControllerFuncs[battler] = ServeChooseMove;
}

void EmeraldChampionsAgentBattleChoosePokemon(enum BattlerId battler)
{
    // Keep the engine's party-order bookkeeping identical to the menu path.
    for (u32 i = 0; i < ARRAY_COUNT(gBattlePartyCurrentOrder); i++)
        gBattlePartyCurrentOrder[i] = gBattleResources->bufferA[battler][4 + i];
    memcpy(gBattleStruct->battlerPartyOrders[battler], gBattlePartyCurrentOrder,
           sizeof(gBattlePartyCurrentOrder));
    gBattleStruct->battlerPreventingSwitchout = gBattleResources->bufferA[battler][8];
    gBattleStruct->prevSelectedPartySlot = gBattleResources->bufferA[battler][2];
    gBattleStruct->abilityPreventingSwitchout =
        (gBattleResources->bufferA[battler][3] & 0xFF) | (gBattleResources->bufferA[battler][7] << 8);
    gBattlerControllerFuncs[battler] = ServeChoosePokemon;
}

// ---------------------------------------------------------------- message log

void EmeraldChampionsAgentBattleText(const u8 *text)
{
    if (!sBridgeArmed || text == NULL || !gMain.inBattle)
        return;

    u32 entry = sMessageSerial % EC_AGENT_BATTLE_MSG_COUNT;
    u32 base = EC_AGENT_BATTLE_MSG_BASE + entry * EC_AGENT_BATTLE_MSG_SIZE;
    u8 buffer[EC_AGENT_BATTLE_MSG_CHARS] = {0};
    u32 length = 0;

    while (length < EC_AGENT_BATTLE_MSG_CHARS && text[length] != EOS)
    {
        buffer[length] = text[length];
        length++;
    }
    if (length == 0)
        return;
    gEcAgentBattleView[base] = length | (sMessageSerial << 8);
    for (u32 i = 0; i < EC_AGENT_BATTLE_MSG_SIZE - 1; i++)
    {
        gEcAgentBattleView[base + 1 + i] = buffer[i * 4] | (buffer[i * 4 + 1] << 8)
                                         | (buffer[i * 4 + 2] << 16) | (buffer[i * 4 + 3] << 24);
    }
    sMessageSerial++;
}

// ------------------------------------------------------------------- the view

// The engine clears an owner's party during the end-of-battle teardown while
// gMain.inBattle is still set, which would zero a finished battle's faint count.
static bool32 PartyPopulated(enum BattleTrainer trainer)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[trainer][i], MON_DATA_SPECIES) != SPECIES_NONE)
            return TRUE;
    }
    return FALSE;
}

static u32 CountFainted(enum BattleTrainer trainer, u32 count)
{
    u32 fainted = 0;

    for (u32 i = 0; i < count; i++)
    {
        if (GetMonData(&gParties[trainer][i], MON_DATA_SPECIES) != SPECIES_NONE
         && GetMonData(&gParties[trainer][i], MON_DATA_HP) == 0)
            fainted++;
    }
    return fainted;
}

// The player commands their own battlers. An in-game partner (the Steven multi)
// keeps its ordinary partner AI, exactly as in play.
static bool32 IsAgentControlled(enum BattlerId battler)
{
    if (battler >= gBattlersCount || GetBattlerSide(battler) != B_SIDE_PLAYER)
        return FALSE;
    if (gBattleTypeFlags & BATTLE_TYPE_INGAME_PARTNER
     && GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT)
        return FALSE;
    return TRUE;
}

// The one switch gate, shared by the observation view and the serving
// controller. It calls the same engine functions as the B_ACTION_SWITCH case in
// HandleTurnActionSelectionState rather than restating their logic, so
// Shadow Tag, Arena Trap, Magnet Pull, Mean Look, Block, Spider Web, the
// Bind-class volatiles, Ingrain, No Retreat, Octolock, Jaw Lock and Fairy Lock
// all reach the mailbox exactly as they reach the native menu.
static u32 AgentSwitchBlocker(enum BattlerId battler)
{
    if (!IsBattlerAlive(battler))
        return EC_AGENT_SWITCH_ALLOWED; // a fainted battler owes a replacement
    if (gBattleTypeFlags & BATTLE_TYPE_ARENA)
        return EC_AGENT_SWITCH_BLOCKED_ARENA;
    if (gBattleStruct->battlerState[battler].commanderSpecies != SPECIES_NONE)
        return EC_AGENT_SWITCH_BLOCKED_COMMANDER;
    if (!CanBattlerEscape(battler) && GetBattlerHoldEffect(battler) != HOLD_EFFECT_SHED_SHELL)
        return EC_AGENT_SWITCH_BLOCKED_TRAPPED;
    if (GetItemHoldEffect(gBattleMons[battler].item) != HOLD_EFFECT_SHED_SHELL
     && IsAbilityPreventingEscape(battler))
        return EC_AGENT_SWITCH_BLOCKED_ABILITY;
    return EC_AGENT_SWITCH_ALLOWED;
}

static void WriteBattlers(void)
{
    for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        u32 base = EC_AGENT_BATTLE_BATTLER_BASE + battler * EC_AGENT_BATTLE_BATTLER_SIZE;
        bool32 live = battler < gBattlersCount;
        struct BattlePokemon *mon = &gBattleMons[battler];

        if (!live)
        {
            for (u32 i = 0; i < EC_AGENT_BATTLE_BATTLER_SIZE; i++)
                gEcAgentBattleView[base + i] = 0;
            continue;
        }
        gEcAgentBattleView[base + 0] = mon->species;
        gEcAgentBattleView[base + 1] = mon->level;
        gEcAgentBattleView[base + 2] = mon->hp;
        gEcAgentBattleView[base + 3] = mon->maxHP;
        gEcAgentBattleView[base + 4] = mon->status1;
        gEcAgentBattleView[base + 5] = mon->item;
        gEcAgentBattleView[base + 6] = mon->ability;
        gEcAgentBattleView[base + 7] = gBattlerPartyIndexes[battler];
        gEcAgentBattleView[base + 8] = (IsBattlerAlive(battler) ? 1 : 0)
                                     | ((gAbsentBattlerFlags & (1u << battler)) ? 2 : 0)
                                     | (GetBattlerSide(battler) == B_SIDE_PLAYER ? 4 : 0)
                                     | (IsAgentControlled(battler) ? 8 : 0);
        for (u32 i = 0; i < NUM_BATTLE_STATS; i++)
            gEcAgentBattleView[base + 9 + i] = (u32)(s32)mon->statStages[i];
        for (u32 i = 0; i < MAX_MON_MOVES; i++)
        {
            // A real player sees their own full set, but only the opposing
            // moves that have actually been used this battle.
            enum Move move = mon->moves[i];
            if (GetBattlerSide(battler) != B_SIDE_PLAYER)
                move = (gBattleHistory != NULL) ? gBattleHistory->usedMoves[battler][i] : MOVE_NONE;
            gEcAgentBattleView[base + 17 + i] = move;
            gEcAgentBattleView[base + 21 + i] = mon->pp[i];
        }
        gEcAgentBattleView[base + 25] = GetActiveGimmick(battler)
                                      | (gBattleStruct->gimmick.usableGimmick[battler] << 8)
                                      | (HasTrainerUsedGimmick(battler, GIMMICK_MEGA) ? 0x10000 : 0);
        gEcAgentBattleView[base + 26] = sLastAction[battler] | (sLastMovePos[battler] << 8)
                                      | (sLastTarget[battler] << 16);
        gEcAgentBattleView[base + 27] = mon->types[0] | (mon->types[1] << 8) | (mon->types[2] << 16);
    }
}

static void WritePlayerParty(void)
{
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        u32 base = EC_AGENT_BATTLE_PARTY_BASE + slot * EC_AGENT_BATTLE_PARTY_SIZE;
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
        u32 species = GetMonData(mon, MON_DATA_SPECIES);

        gEcAgentBattleView[base + 0] = species;
        gEcAgentBattleView[base + 1] = GetMonData(mon, MON_DATA_LEVEL);
        gEcAgentBattleView[base + 2] = GetMonData(mon, MON_DATA_HP);
        gEcAgentBattleView[base + 3] = GetMonData(mon, MON_DATA_MAX_HP);
        gEcAgentBattleView[base + 4] = GetMonData(mon, MON_DATA_STATUS);
        gEcAgentBattleView[base + 5] = GetMonData(mon, MON_DATA_HELD_ITEM);
        gEcAgentBattleView[base + 6] = species ? GetMonAbility(mon) : 0;
        u32 pp = 0;
        for (u32 i = 0; i < MAX_MON_MOVES; i++)
        {
            gEcAgentBattleView[base + 7 + i] = GetMonData(mon, MON_DATA_MOVE1 + i);
            pp |= (GetMonData(mon, MON_DATA_PP1 + i) & 0xFF) << (i * 8);
        }
        gEcAgentBattleView[base + 11] = pp;
    }
}

static void WriteRevealedOpponents(void)
{
    static const enum BattleTrainer sOwners[2] = {B_TRAINER_OPPONENT_A, B_TRAINER_OPPONENT_B};

    for (u32 owner = 0; owner < 2; owner++)
    {
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            u32 base = EC_AGENT_BATTLE_FOE_BASE + (owner * PARTY_SIZE + slot) * EC_AGENT_BATTLE_FOE_SIZE;
            struct Pokemon *mon = &gParties[sOwners[owner]][slot];
            bool32 revealed = (sRevealed[sOwners[owner]] & (1u << slot)) != 0;
            u32 species = GetMonData(mon, MON_DATA_SPECIES);

            gEcAgentBattleView[base + 0] = (revealed && species) ? species : SPECIES_NONE;
            gEcAgentBattleView[base + 1] = revealed ? GetMonData(mon, MON_DATA_LEVEL) : 0;
            gEcAgentBattleView[base + 2] = (species ? 1 : 0) | (revealed ? 2 : 0)
                                         | ((species && GetMonData(mon, MON_DATA_HP) == 0) ? 4 : 0);
        }
    }
}

static void WriteLegality(void)
{
    for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        u32 base = EC_AGENT_BATTLE_LEGAL_BASE + battler * EC_AGENT_BATTLE_LEGAL_SIZE;

        for (u32 i = 0; i < EC_AGENT_BATTLE_LEGAL_SIZE; i++)
            gEcAgentBattleView[base + i] = 0;
        if (!IsAgentControlled(battler))
            continue;

        // CheckMoveLimitations answers "which move slots", not "why", so its
        // result is a slot mask and decoding it as a reason mask made an
        // unusable fourth slot read as MOVE_LIMITATION_TORMENTED. Ask once per
        // limitation as well, so each slot can name what actually blocked it.
        // Only while parked for a command: that is the only time legality is
        // read, and this is 17 passes per battler.
        u32 reasons[MAX_MON_MOVES] = {0};
        gEcAgentBattleView[base + 0] = IsBattlerAlive(battler)
            ? CheckMoveLimitations(battler, 0, MOVE_LIMITATIONS_ALL) : 0xF;
        if (IsBattlerAlive(battler) && gEcAgentBattleNeedMask != 0)
        {
            for (u32 reason = 0; reason < 16; reason++)
            {
                u32 slots = CheckMoveLimitations(battler, 0, 1u << reason);
                for (u32 i = 0; i < MAX_MON_MOVES; i++)
                {
                    if (slots & (1u << i))
                        reasons[i] |= 1u << reason;
                }
            }
        }
        for (u32 i = 0; IsBattlerAlive(battler) && i < MAX_MON_MOVES; i++)
        {
            enum Move move = gBattleMons[battler].moves[i];
            u32 mask = 0;

            if (move != MOVE_NONE)
            {
                for (u32 target = 0; target < gBattlersCount; target++)
                {
                    if (CanTargetBattler(battler, target, move) && IsBattlerAlive(target))
                        mask |= 1u << target;
                }
                mask |= (GetBattlerMoveSelectionTargetType(battler, move) << 8);
            }
            gEcAgentBattleView[base + 1 + i] = mask | (reasons[i] << 16);
        }

        u32 blocker = AgentSwitchBlocker(battler);
        u32 switchable = (blocker == EC_AGENT_SWITCH_ALLOWED && CanBattlerSwitch(battler))
                       ? 0x100 : 0;
        switchable |= blocker << 16;
        switchable |= sSwitchRefused[battler] << 24;
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
            bool32 onField = FALSE;

            for (u32 other = 0; other < gBattlersCount; other++)
            {
                // In a multi battle the partner indexes its own party, so its
                // party index must not shadow one of the player's slots.
                if (GetBattlerTrainer(other) == B_TRAINER_PLAYER
                 && gBattlerPartyIndexes[other] == slot)
                    onField = TRUE;
            }
            if (!onField && IsValidForBattle(mon))
                switchable |= 1u << slot;
        }
        gEcAgentBattleView[base + 5] = switchable;
    }
}

// What the battle setup actually asked for. The engine's only setup-side channel
// for a field effect is the trainer's authored startingStatus, which battle_main
// ORs into gStartingStatuses before the first turn; there is no map or Gym field
// table. Reporting it lets a caller tell "the room authored nothing" apart from
// "the driver dropped it".
static u32 AuthoredFieldMask(u32 trainerId)
{
    if (trainerId == TRAINER_NONE || trainerId >= TRAINERS_COUNT)
        return 0;

    struct StartingStatuses authored = GetTrainerStartingStatusFromId(trainerId);
    u32 mask = 0;

    if (authored.electricTerrain)          mask |= 1u << 0;
    if (authored.electricTerrainTemporary) mask |= 1u << 1;
    if (authored.mistyTerrain)             mask |= 1u << 2;
    if (authored.mistyTerrainTemporary)    mask |= 1u << 3;
    if (authored.grassyTerrain)            mask |= 1u << 4;
    if (authored.grassyTerrainTemporary)   mask |= 1u << 5;
    if (authored.psychicTerrain)           mask |= 1u << 6;
    if (authored.psychicTerrainTemporary)  mask |= 1u << 7;
    if (authored.trickRoom)                mask |= 1u << 8;
    if (authored.magicRoom)                mask |= 1u << 9;
    if (authored.wonderRoom)               mask |= 1u << 10;
    if (authored.weatherSun)               mask |= 1u << 16;
    if (authored.weatherSunTemporary)      mask |= 1u << 17;
    if (authored.weatherRain)              mask |= 1u << 18;
    if (authored.weatherRainTemporary)     mask |= 1u << 19;
    if (authored.weatherSandstorm)         mask |= 1u << 20;
    if (authored.weatherHail)              mask |= 1u << 21;
    if (authored.weatherSnow)              mask |= 1u << 22;
    if (authored.weatherFog)               mask |= 1u << 23;
    return mask;
}

static void WriteView(void)
{
    u32 liveFoes = 0;

    gEcAgentBattleView[0] = 1; // schema
    gEcAgentBattleView[1] = gEcAgentBattlePhase;
    gEcAgentBattleView[2] = gEcAgentBattleSerial;
    gEcAgentBattleView[3] = gBattleTurnCounter;
    gEcAgentBattleView[4] = gBattleTypeFlags;
    gEcAgentBattleView[5] = gBattleWeather;
    gEcAgentBattleView[6] = gFieldStatuses;
    gEcAgentBattleView[7] = gMain.inBattle ? gBattleOutcome : sFinalOutcome;
    gEcAgentBattleView[8] = gEcAgentBattleNeedMask;
    gEcAgentBattleView[9] = sAiDecisionFrames;
    gEcAgentBattleView[10] = sAiSetupFrames;
    gEcAgentBattleView[11] = (gBattleStruct != NULL) ? (u32)gBattleStruct->aiDelayFrames : 0;
    gEcAgentBattleView[12] = gAbsentBattlerFlags;
    gEcAgentBattleView[13] = gBattlersCount;
    if (gMain.inBattle)
    {
        bool32 twoOwners = (gBattleTypeFlags & BATTLE_TYPE_TWO_OPPONENTS) != 0;
        if (PartyPopulated(B_TRAINER_PLAYER))
            sPlayerFaints = CountFainted(B_TRAINER_PLAYER, PARTY_SIZE);
        // Recounted rather than accumulated, so Revival Blessing lowers it.
        if (PartyPopulated(B_TRAINER_OPPONENT_A)
         || (twoOwners && PartyPopulated(B_TRAINER_OPPONENT_B)))
        {
            sOpponentFaints = CountFainted(B_TRAINER_OPPONENT_A, PARTY_SIZE)
                            + (twoOwners ? CountFainted(B_TRAINER_OPPONENT_B, PARTY_SIZE) : 0);
        }
        if (gBattleOutcome != 0)
            sFinalOutcome = gBattleOutcome;
    }
    gEcAgentBattleView[14] = sPlayerFaints;
    gEcAgentBattleView[15] = sOpponentFaints;
    gEcAgentBattleView[16] = sMessageSerial;
    gEcAgentBattleView[17] = gSideStatuses[B_SIDE_PLAYER];
    gEcAgentBattleView[18] = gSideStatuses[B_SIDE_OPPONENT];
    gEcAgentBattleView[19] = gEcAgentBattleTrainerA;
    gEcAgentBattleView[20] = gEcAgentBattleTrainerB;
    gEcAgentBattleView[21] = gEcAgentBattlePartner;
    gEcAgentBattleView[22] = GetCurrentLevelCap();
    gEcAgentBattleView[23] = GetCurrentDifficultyLevel();
    gEcAgentBattleView[24] = gMain.inBattle;
    for (u32 battler = 0; battler < gBattlersCount; battler++)
    {
        if (GetBattlerSide(battler) != B_SIDE_PLAYER && IsBattlerAlive(battler))
            liveFoes |= 1u << battler;
    }
    gEcAgentBattleView[25] = liveFoes;
    gEcAgentBattleView[26] = (gAiLogicData != NULL) ? gAiLogicData->battlerMovesScored : 0;
    gEcAgentBattleView[27] = gMain.vblankCounter1;
    gEcAgentBattleView[28] = sLevelCap;
    gEcAgentBattleView[EC_AGENT_BATTLE_FIELD_BASE + 0] = AuthoredFieldMask(gEcAgentBattleTrainerA);
    gEcAgentBattleView[EC_AGENT_BATTLE_FIELD_BASE + 1] = AuthoredFieldMask(gEcAgentBattleTrainerB);
    if (gMain.inBattle && gBattleTurnCounter == 0)
    {
        sOpeningTerrain = gFieldTimers.terrain;
        sOpeningWeather = gBattleWeather;
    }
    gEcAgentBattleView[EC_AGENT_BATTLE_FIELD_BASE + 2] = sOpeningTerrain;
    gEcAgentBattleView[EC_AGENT_BATTLE_FIELD_BASE + 3] = sOpeningWeather;
    gEcAgentBattleView[29] = gFieldTimers.terrain;
    gEcAgentBattleView[30] = gFieldTimers.terrainTimer;
    // Per-battler action-selection state, so the host knows which of its
    // battlers still owe a command this turn.
    gEcAgentBattleView[31] = gBattleCommunication[0] | (gBattleCommunication[1] << 8)
                           | (gBattleCommunication[2] << 16) | (gBattleCommunication[3] << 24);

    for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
    {
        u32 base = EC_AGENT_BATTLE_PREV_BASE + battler * EC_AGENT_BATTLE_PREV_SIZE;
        gEcAgentBattleView[base + 0] = sPrevAction[battler];
        gEcAgentBattleView[base + 1] = sPrevMovePos[battler];
        gEcAgentBattleView[base + 2] = sPrevTarget[battler];
        gEcAgentBattleView[base + 3] = sPrevMove[battler];
    }
    WriteBattlers();
    WritePlayerParty();
    WriteRevealedOpponents();
    WriteLegality();
}

// ------------------------------------------------------------------- the poll

static void StartRequestedBattle(void)
{
    u32 trainerA = gEcAgentBattleTrainerA;
    u32 trainerB = gEcAgentBattleTrainerB;
    u32 partner = gEcAgentBattlePartner;

    if (gMain.inBattle || gMain.callback2 != CB2_Overworld
     || ArePlayerFieldControlsLocked() || ScriptContext_IsEnabled())
    {
        gEcAgentBattleResult = EC_AGENT_BATTLE_NOT_READY;
        gEcAgentBattleCommand = 0;
        return;
    }
    if (trainerA == TRAINER_NONE || trainerA >= TRAINERS_COUNT
     || GetTrainerStructFromId(trainerA)->partySize == 0
     || (trainerB != TRAINER_NONE
         && (trainerB >= TRAINERS_COUNT || GetTrainerStructFromId(trainerB)->partySize == 0)))
    {
        gEcAgentBattleResult = EC_AGENT_BATTLE_BAD_TRAINER;
        gEcAgentBattleCommand = 0;
        return;
    }
    if (CalculatePlayerPartyCount() < 2)
    {
        gEcAgentBattleResult = EC_AGENT_BATTLE_BAD_PARTY;
        gEcAgentBattleCommand = 0;
        return;
    }

    // Mirrors the native debug trainer lifecycle (DebugAction_Trainers_TryBattle
    // and the Studio sandbox), extended to the authored two-owner pairs.
    memset(&gTrainerBattleParameter, 0, sizeof(gTrainerBattleParameter));
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    TRAINER_BATTLE_PARAM.opponentA = trainerA;
    TRAINER_BATTLE_PARAM.opponentB = 0xFFFF;
    TRAINER_BATTLE_PARAM.isDoubleBattle = TRUE;
    CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_A], GetTrainerStructFromId(trainerA));
    if (trainerB != TRAINER_NONE)
    {
        TRAINER_BATTLE_PARAM.opponentB = trainerB;
        CreateNPCTrainerPartyFromTrainer(gParties[B_TRAINER_OPPONENT_B], GetTrainerStructFromId(trainerB));
        gBattleTypeFlags |= BATTLE_TYPE_TWO_OPPONENTS;
    }
    if (partner != PARTNER_NONE)
    {
        SavePlayerParty();
        gPartnerTrainerId = TRAINER_PARTNER(partner);
        gBattleTypeFlags |= BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER;
        for (u32 i = 0; i < MAX_FRONTIER_PARTY_SIZE; i++)
        {
            gSelectedOrderFromParty[i] = i + 1;
            gSaveBlock2Ptr->frontier.selectedPartyMons[i] = gSelectedOrderFromParty[i];
        }
        FillPartnerParty(gPartnerTrainerId);
    }
    gBattleEnvironment = BattleSetup_GetEnvironmentId();
    CalculateEnemyPartyCount();
    BattleSetup_StartTrainerBattle_Debug();

    sBattleSeen = FALSE;
    gEcAgentBattlePhase = EC_AGENT_BATTLE_PHASE_STARTING;
    gEcAgentBattleResult = EC_AGENT_BATTLE_OK;
    gEcAgentBattleCommand = 0;
}

void EmeraldChampionsAgentBattlePoll(void)
{
    if (!sBridgeArmed)
        return;

    if (gEcAgentBattleCommand == 1)
        StartRequestedBattle();
    else if (gEcAgentBattleCommand != 0)
    {
        gEcAgentBattleResult = EC_AGENT_BATTLE_BAD_COMMAND;
        gEcAgentBattleCommand = 0;
    }

    if (!gMain.inBattle || gBattleStruct == NULL)
    {
        if (sBattleSeen)
            gEcAgentBattlePhase = EC_AGENT_BATTLE_PHASE_ENDED;
        gEcAgentBattleHalted = (gEcAgentBattlePhase == EC_AGENT_BATTLE_PHASE_ENDED);
        WriteView();
        return;
    }
    sBattleSeen = TRUE;
    if (gEcAgentBattlePhase == EC_AGENT_BATTLE_PHASE_STARTING)
        gEcAgentBattlePhase = EC_AGENT_BATTLE_PHASE_RUNNING;
    if (gEcAgentBattleNeedMask == 0 && gEcAgentBattlePhase != EC_AGENT_BATTLE_PHASE_ENDED)
        gEcAgentBattlePhase = EC_AGENT_BATTLE_PHASE_RUNNING;

    if (gBattleTurnCounter != sLastTurn)
    {
        sLastTurn = gBattleTurnCounter;
        for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            sPrevAction[i] = sLastAction[i];
            sPrevMovePos[i] = sLastMovePos[i];
            sPrevTarget[i] = sLastTarget[i];
            sPrevMove[i] = sLastMove[i];
            sLastAction[i] = B_ACTION_NONE;
            sLastMovePos[i] = 0;
            sLastTarget[i] = 0;
            sLastMove[i] = MOVE_NONE;
            sLastSelection[i] = 0;
        }
    }

    // Same accounting the blessed timing helper uses: the complete opposing
    // decision, setup included, in native VBlank frames.
    if (gAiLogicData != NULL && gAiLogicData->decisionStartFrame != sDecisionStart)
    {
        sDecisionStart = gAiLogicData->decisionStartFrame;
        sAiDecisionLatched = FALSE;
    }
    if (!sAiDecisionLatched && gAiLogicData != NULL)
    {
        u32 mask = 0;
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            if (GetBattlerSide(battler) != B_SIDE_PLAYER && IsBattlerAlive(battler))
                mask |= 1u << battler;
        }
        if (mask != 0 && (gAiLogicData->battlerMovesScored & mask) == mask)
        {
            sAiDecisionFrames = gMain.vblankCounter1 - gAiLogicData->decisionStartFrame;
            sAiSetupFrames = gAiLogicData->decisionSetupFrames;
            sAiDecisionLatched = TRUE;
        }
    }

    for (u32 battler = 0; battler < gBattlersCount; battler++)
    {
        // Latch the turn's choice exactly once, as the action is confirmed.
        // moveTarget and chosenMovePositions are working fields the engine
        // rewrites while the turn executes, so sampling them every frame
        // recorded whatever was last written rather than what was chosen: an
        // ally-targeted Heal Pulse came back pointing at a player battler, and
        // a later switch could be paired with the earlier move's name.
        // 4 is STATE_WAIT_ACTION_CONFIRMED_STANDBY, the first confirmed state
        // in the action-selection enum local to battle_main.c; battle_main sets
        // all four fields immediately before advancing into it.
        u32 selection = gBattleCommunication[battler];
        if (selection >= 4 && sLastSelection[battler] < 4
         && gChosenActionByBattler[battler] != B_ACTION_NONE)
        {
            sLastAction[battler] = gChosenActionByBattler[battler];
            sLastMovePos[battler] = gBattleStruct->chosenMovePositions[battler];
            sLastTarget[battler] = gBattleStruct->moveTarget[battler];
            sLastMove[battler] = (gChosenActionByBattler[battler] == B_ACTION_USE_MOVE)
                               ? gChosenMoveByBattler[battler] : MOVE_NONE;
        }
        sLastSelection[battler] = selection;
        if (GetBattlerSide(battler) != B_SIDE_PLAYER)
            sRevealed[GetBattlerTrainer(battler)] |= 1u << gBattlerPartyIndexes[battler];
    }
    gEcAgentBattleHalted = (gEcAgentBattleNeedMask != 0);
    WriteView();
}

#endif
