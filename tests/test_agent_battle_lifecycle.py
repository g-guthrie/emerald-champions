"""Production terminal observation preserves battle evidence after allocation free."""
import sys
from pathlib import Path
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c


class TerminalViewTests(unittest.TestCase):
    def test_factory_audit_reuses_effective_nature_and_canonical_stat_row(self):
        source = host_c.production('src/emerald_champions_agent_battle.c')
        boundary = r'''
#include "constants/emerald_champions.h"
struct Pokemon gParties[MAX_BATTLE_TRAINERS][PARTY_SIZE];
static struct SaveBlock2 save;
struct SaveBlock2 *gSaveBlock2Ptr = &save;
u8 CalculatePlayerPartyCount(void) { return 2; }
u16 GetEmeraldChampionsSecondStarterIndex(void) { return 0; }
u16 VarGet(u16 id)
{
    if (id == VAR_STARTER_GEN) return 4;
    if (id == VAR_STARTER_MON) return 2;
    if (id == VAR_EC_OPENING_STATE) return EC_OPENING_PAIR_GRANTED;
    assert(0); return 0;
}
enum Ability GetMonAbility(struct Pokemon *mon) { return ABILITY_TORRENT; }
u32 GetMonData2(struct Pokemon *mon, s32 field)
{
    if (field == MON_DATA_SPECIES) return SPECIES_PIPLUP;
    if (field == MON_DATA_LEVEL) return 5;
    if (field == MON_DATA_HIDDEN_NATURE) return NATURE_BOLD;
    if (field == MON_DATA_FRIENDSHIP) return 50;
    if (field >= MON_DATA_HP_EV && field <= MON_DATA_SPDEF_EV) return field;
    if (field >= MON_DATA_HP_IV && field <= MON_DATA_SPDEF_IV) return 31;
    return 0;
}
int main(void)
{
    save.playerGender = FEMALE;
    WriteRescuePlayerFactory();
    assert(gEcAgentBattlePlayerFactory[0] == 1);
    assert(gEcAgentBattlePlayerFactory[1] == 2);
    assert(gEcAgentBattlePlayerFactory[2] == 4);
    assert(gEcAgentBattlePlayerFactory[3] == 2);
    assert(gEcAgentBattlePlayerFactory[4] == 0);
    assert(gEcAgentBattlePlayerFactory[5] == FEMALE);
    u32 base = EC_AGENT_BATTLE_FACTORY_HEADER;
    assert(gEcAgentBattlePlayerFactory[base + 1] == 5);
    assert(gEcAgentBattlePlayerFactory[base + 4] == NATURE_BOLD);
    assert(gEcAgentBattlePlayerFactory[base + 9] == MON_DATA_SPATK_EV);
    assert(gEcAgentBattlePlayerFactory[base + 10] == MON_DATA_SPDEF_EV);
    assert(gEcAgentBattlePlayerFactory[base + 11] == MON_DATA_SPEED_EV);
    assert(gEcAgentBattlePlayerFactory[base + 17] == 31);
    assert(gEcAgentBattlePlayerFactory[EC_AGENT_BATTLE_FACTORY_WORDS - 1] == 0);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as scratch:
            executable = host_c.build(Path(scratch), {'bridge.c': source + boundary},
                defines={'EC_HEADLESS_FIXTURES': '1'}, optimize='-O3')
            host_c.run(executable)

    def test_scripted_wild_owns_a_paused_release_end_resume_script(self):
        source = host_c.production('src/emerald_champions_agent_battle.c')
        boundary = r'''
static const u8 *ownedScript;
static u32 calls;
void ScriptContext_SetupScript(const u8 *ptr)
{
    assert(calls == 0);
    ownedScript = ptr;
    calls++;
}
void ScriptContext_Stop(void)
{
    assert(calls == 1);
    calls++;
}
int main(void)
{
    PrepareScriptedBattleResume();
    assert(calls == 2);
    assert(ownedScript != NULL);
    assert(ownedScript[0] == SCR_OP_RELEASEALL && ownedScript[1] == SCR_OP_END);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as scratch:
            executable = host_c.build(Path(scratch), {'bridge.c': source + boundary},
                defines={'EC_HEADLESS_FIXTURES': '1'}, optimize='-O3')
            host_c.run(executable)

    def test_null_battle_keeps_last_live_board_and_faints(self):
        source = host_c.production('src/emerald_champions_agent_battle.c')
        # Permit constant propagation of this terminal-only call. The complete
        # production unit is compiled; no branch or helper implementation is
        # copied. Unsafe/live helper dependencies remain deliberately unstubbed.
        source = source.replace('static void WriteView(void)',
            'static inline __attribute__((always_inline)) void WriteView(void)')
        boundary = r'''
struct Main gMain;
struct BattleStruct *gBattleStruct;
struct AiLogicData *gAiLogicData;
u16 gBattleTurnCounter;
u32 gBattleTypeFlags;
u16 gBattleWeather;
u32 gFieldStatuses;
u8 gBattleOutcome;
u8 gAbsentBattlerFlags;
u8 gBattlersCount;
u32 gSideStatuses[NUM_BATTLE_SIDES];
struct FieldTimer gFieldTimers;
u8 gBattleCommunication[BATTLE_COMMUNICATION_ENTRIES_COUNT];
u8 gBattleEnvironment;
u32 GetCurrentLevelCap(void) { return 14; }
enum DifficultyLevel GetCurrentDifficultyLevel(void) { return DIFFICULTY_EASY; }
bool8 gIsDebugBattle;
const struct Trainer gTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT] = {0};
const struct Trainer gBattlePartners[DIFFICULTY_COUNT][PARTNER_COUNT] = {0};
const struct Trainer *GetDebugAiTrainer(void) { return &gTrainers[0][0]; }
enum DifficultyLevel GetTrainerDifficultyLevel(u16 trainer) { return DIFFICULTY_EASY; }
enum DifficultyLevel GetBattlePartnerDifficultyLevel(u16 trainer) { return DIFFICULTY_EASY; }
int main(void)
{
    gMain.inBattle = TRUE;
    gBattleStruct = NULL;
    gAiLogicData = NULL;
    gBattlersCount = MAX_BATTLERS_COUNT;
    sBattleSeen = TRUE;
    sFinalOutcome = B_OUTCOME_WON;
    sPlayerFaints = 5;
    sOpponentFaints = 4;
    gEcAgentBattleView[EC_AGENT_BATTLE_BATTLER_BASE + 2] = 7;
    gEcAgentBattleView[EC_AGENT_BATTLE_PARTY_BASE + 2] = 48;
    gEcAgentBattleView[EC_AGENT_BATTLE_LEGAL_BASE] = 0xFFFFFFFF;
    WriteView();
    assert(gEcAgentBattleView[7] == B_OUTCOME_WON);
    assert(gEcAgentBattleView[14] == 5 && gEcAgentBattleView[15] == 4);
    assert(gEcAgentBattleView[EC_AGENT_BATTLE_BATTLER_BASE + 2] == 7);
    assert(gEcAgentBattleView[EC_AGENT_BATTLE_PARTY_BASE + 2] == 48);
    assert(gEcAgentBattleView[EC_AGENT_BATTLE_LEGAL_BASE] == 0);
    assert(gEcAgentBattleView[EC_AGENT_BATTLE_LAST_LIVE_WORD] == 1);
    gMain.inBattle = FALSE;
    gBattleOutcome = 0;
    WriteView();
    assert(gEcAgentBattleView[7] == B_OUTCOME_WON);
    assert(gEcAgentBattleView[EC_AGENT_BATTLE_PARTY_BASE + 2] == 48);
    assert(gEcAgentBattleView[14] == 5 && gEcAgentBattleView[15] == 4);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as scratch:
            executable = host_c.build(Path(scratch), {'bridge.c': source + boundary},
                defines={'EC_HEADLESS_FIXTURES': '1'}, optimize='-O3')
            host_c.run(executable)


if __name__ == '__main__':
    unittest.main()
