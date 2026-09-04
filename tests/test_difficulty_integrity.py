"""Execute production C difficulty/party resolution and title migration paths.

This host harness uses real function bodies and constants with controlled trainer
rows and small save/graphics stubs. It does not pin an authored encounter's layout,
validate generated trainer data, or replace either game pipeline.
"""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def function(path, name):
    source = (ROOT / path).read_text()
    match = re.search(r"(?m)^[\w\s*]+\b" + name + r"\s*\([^;{}]*\)\s*\{", source)
    if not match:
        raise AssertionError(f"cannot locate {path}:{name}")
    depth = 1
    end = match.end()
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[match.start():end]


class DifficultyIntegrity(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = shutil.which("cc")
        if not compiler:
            raise RuntimeError("A host C compiler is required for production-branch checks")
        cls.temp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.temp.cleanup)
        cls.executable = Path(cls.temp.name) / "difficulty_integrity"
        fixture = Path(cls.temp.name) / "difficulty_integrity.c"
        chunks = [r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define TESTING 0
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef bool bool32;
typedef bool bool8;
#include "constants/difficulty.h"
#include "constants/vars.h"
#include "constants/flags.h"
#include "constants/pokemon.h"
#include "constants/battle.h"
#define B_VAR_DIFFICULTY VAR_EMERALD_CHAMPIONS_DIFFICULTY
#define B_MULTI_HALF_TEAMS FALSE
#define assertf(condition, ...) if (!(condition))
static u16 vars[65536];
static bool flags[65536];
static u16 VarGet(u16 key) { return vars[key]; }
static void VarSet(u16 key, u16 value) { vars[key] = value; }
static bool FlagGet(u16 key) { return flags[key]; }
static void FlagSet(u16 key) { flags[key] = true; }
static void FlagClear(u16 key) { flags[key] = false; }
enum { MULTI_TEAM_SIZE_FULL, MULTI_TEAM_SIZE_HALF };
struct Trainer { const void *party; u8 multiTeamSize; };
static struct Trainer gTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT];
static struct Trainer gBattlePartners[DIFFICULTY_COUNT][PARTNER_COUNT];
static bool gIsDebugBattle;
static const struct Trainer *GetDebugAiTrainer(void) { abort(); }
static struct { u16 opponentA, opponentB; } trainerParams;
#define TRAINER_BATTLE_PARAM trainerParams
static u32 gBattleTypeFlags;
static u16 gSpecialVar_Result;
''']
        chunks.append(function("include/data.h", "SanitizeTrainerId"))
        for name in ("GetCurrentDifficultyLevel", "SetCurrentDifficultyLevel", "GetTrainerLevelReduction",
                     "GetBattlePartnerDifficultyLevel", "GetTrainerDifficultyLevel"):
            chunks.append(function("src/difficulty.c", name))
        for name in ("IsPartnerTrainerId", "IsSpecialTrainer",
                     "GetPartnerIdFromTrainerId", "GetTrainerStructFromId"):
            chunks.append(function("include/data.h", name))
        chunks.append(function("src/battle_util.c", "AreMultiPartiesFullTeams"))
        chunks.append(r'''
static struct SaveBlock2 {
    u8 optionsTextSpeed, optionsBattleSceneOff, optionsBattleStyle;
    u8 optionsSound, optionsButtonMode, optionsWindowFrameType;
} save2, *gSaveBlock2Ptr = &save2;
static struct { void (*savedCallback)(void); } gMain;
static void CB2_ReinitMainMenu(void) {}
static void InGameCallback(void) {}
static u16 gSaveFileStatus;
static void ResetEmeraldChampionsCollidingFlags(void) {
    FlagClear(FLAG_UNUSED_0x91E);
    FlagClear(FLAG_UNUSED_0x91F);
    FlagClear(FLAG_EC_CAUGHT_SHAYMIN);
}
static void ResetEmeraldChampionsSignState(void) {}
static void ResetEmeraldChampionsRepurposedTrainers(void) {}
static void SetEmeraldChampionsPhysicalSignFlags(void) {}
static unsigned relicInitializations;
static void InitializeLegendaryRelicDeliveryState(void) {
    relicInitializations++;
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0,0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1,0);
}
''')
        # Use the game's actual status values without importing the save hardware API.
        save_header = (ROOT / "include/save.h").read_text()
        for name in ("SAVE_STATUS_OK", "SAVE_STATUS_ERROR", "SAVE_STATUS_EMPTY",
                     "SAVE_STATUS_CORRUPT", "SAVE_STATUS_NO_FLASH"):
            chunks.append(re.search(r"(?m)^#define " + name + r"\s+[^\n]+", save_header).group())
        for name in ("MigrateEmeraldChampions81eState", "ResetAmbiguousEmeraldChampionsState",
                     "MigrateEmeraldChampionsCoreState"):
            chunks.append(function("src/overworld.c", name))
        chunks.append(function("src/option_menu.c", "PrepareDifficultyForOptionMenu"))
        chunks.append(r'''
static struct { int data[16]; void (*func)(u8); } gTasks[1];
#define tDifficulty data[1]
#define tBattleSceneOff data[2]
#define tSound data[3]
#define tButtonMode data[4]
#define tWindowFrameType data[5]
#define OPTIONS_BATTLE_STYLE_SET 1
#define PALETTES_ALL 0xffffffff
#define RGB_BLACK 0
static void BeginNormalPaletteFade(u32 a, int b, int c, int d, int e) {}
static void Task_OptionMenuFadeOut(u8 taskId) {}
''')
        chunks.append(function("src/option_menu.c", "Task_OptionMenuSave"))
        # Test sparse-row fallback independently of changing authored teams or a
        # potentially stale generated trainers.h. Nonnull party pointers only
        # establish that a controlled row exists; no party data is dereferenced.
        chunks.append(r'''
enum { TEST_HALF_TRAINER = 1, TEST_FULL_TRAINER = 2 };
static void InitTrainerMetadata(void) {
    gTrainers[DIFFICULTY_NORMAL][TEST_HALF_TRAINER] = (struct Trainer){(void *)1, MULTI_TEAM_SIZE_HALF};
    gTrainers[DIFFICULTY_NORMAL][TEST_FULL_TRAINER] = (struct Trainer){(void *)1, MULTI_TEAM_SIZE_FULL};
}
''')
        chunks.append(r'''
static void LegacySave(void) {
    memset(vars, 0, sizeof(vars));
    memset(flags, 0, sizeof(flags));
    FlagSet(FLAG_UNUSED_0x91E);
    FlagSet(FLAG_UNUSED_0x91F);
    FlagSet(FLAG_EC_CAUGHT_SHAYMIN);
    VarSet(0x40F7, 0xa554); // Legacy unlocked bits, NOT a difficulty enum.
    VarSet(0x40FB, 0x1420); // Legacy caught bits.
    VarSet(0x40FF, 37);
    save2.optionsTextSpeed = 2; // Legacy Easy.
}
int main(int argc, char **argv) {
    assert(argc == 2);
    InitTrainerMetadata();
    int scenario = atoi(argv[1]);
    if (scenario == 0) {
        trainerParams.opponentA = TEST_HALF_TRAINER;
        trainerParams.opponentB = TEST_HALF_TRAINER;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_MULTI | BATTLE_TYPE_TWO_OPPONENTS;
        for (u16 d = DIFFICULTY_EASY; d <= DIFFICULTY_HARD; d++) {
            SetCurrentDifficultyLevel(d);
            assert(GetTrainerStructFromId(trainerParams.opponentA)->multiTeamSize == MULTI_TEAM_SIZE_HALF);
            assert(!AreMultiPartiesFullTeams());
            assert(!gSpecialVar_Result);
        }
    } else if (scenario == 1) {
        trainerParams.opponentA = TEST_FULL_TRAINER;
        trainerParams.opponentB = TEST_HALF_TRAINER; // Stale, absent second opponent.
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        for (u16 d = DIFFICULTY_EASY; d <= DIFFICULTY_HARD; d++) {
            SetCurrentDifficultyLevel(d);
            assert(AreMultiPartiesFullTeams());
            gBattleTypeFlags |= BATTLE_TYPE_TWO_OPPONENTS;
            assert(!AreMultiPartiesFullTeams());
            gBattleTypeFlags &= ~BATTLE_TYPE_TWO_OPPONENTS;
        }
    } else if (scenario == 2) {
        const u16 ids[] = {TRAINER_NONE, TRAINER_SECRET_BASE, TRAINER_UNION_ROOM, 0xffff};
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_TWO_OPPONENTS;
        SetCurrentDifficultyLevel(DIFFICULTY_HARD);
        for (u32 i = 0; i < ARRAY_COUNT(ids); i++) {
            trainerParams.opponentA = ids[i];
            trainerParams.opponentB = ids[i];
            assert(AreMultiPartiesFullTeams());
        }
        trainerParams.opponentA = TRAINER_LINK_OPPONENT;
        assert(!AreMultiPartiesFullTeams());
        trainerParams.opponentA = TEST_HALF_TRAINER;
        gBattleTypeFlags = BATTLE_TYPE_TOWER_LINK_MULTI;
        assert(!AreMultiPartiesFullTeams());
        gBattleTypeFlags = BATTLE_TYPE_BATTLE_TOWER;
        assert(AreMultiPartiesFullTeams());
        gBattleTypeFlags = BATTLE_TYPE_LINK;
        assert(AreMultiPartiesFullTeams());
        gBattleTypeFlags = BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_DOUBLE;
        assert(AreMultiPartiesFullTeams()); // Wild partner battle with stale HALF IDs.
    } else if (scenario == 3) {
        const u16 invalid[] = {DIFFICULTY_COUNT, 0xffff};
        trainerParams.opponentA = TEST_HALF_TRAINER;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER;
        for (u32 i = 0; i < ARRAY_COUNT(invalid); i++) {
            VarSet(B_VAR_DIFFICULTY, invalid[i]);
            assert(GetCurrentDifficultyLevel() == DIFFICULTY_HARD);
            assert(GetTrainerLevelReduction() == 0);
            assert(!AreMultiPartiesFullTeams());
        }
        SetCurrentDifficultyLevel(DIFFICULTY_EASY);
        assert(GetTrainerLevelReduction() == 4);
        SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
        assert(GetTrainerLevelReduction() == 2);
    } else if (scenario == 4) {
        const u16 statuses[] = {SAVE_STATUS_OK, SAVE_STATUS_ERROR};
        for (u32 i = 0; i < ARRAY_COUNT(statuses); i++) {
            LegacySave();
            gSaveFileStatus = statuses[i];
            gMain.savedCallback = CB2_ReinitMainMenu;
            PrepareDifficultyForOptionMenu();
            gTasks[0].tDifficulty = GetCurrentDifficultyLevel();
            Task_OptionMenuSave(0); // Opening and exiting without changes.
            MigrateEmeraldChampionsCoreState(); // Continue does not reload the save.
            assert(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_0) == 0xa554);
            assert(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0) == 0x1420);
            assert(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS) == 37);
            assert(GetCurrentDifficultyLevel() == DIFFICULTY_EASY);
            assert(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION) == EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
            gTasks[0].tDifficulty = DIFFICULTY_NORMAL;
            Task_OptionMenuSave(0);
            MigrateEmeraldChampionsCoreState();
            assert(GetCurrentDifficultyLevel() == DIFFICULTY_NORMAL);
            assert(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_0) == 0xa554);
        }
    } else if (scenario == 5) {
        const u16 statuses[] = {SAVE_STATUS_EMPTY, SAVE_STATUS_CORRUPT, SAVE_STATUS_NO_FLASH};
        for (u32 i = 0; i < ARRAY_COUNT(statuses); i++) {
            LegacySave();
            gSaveFileStatus = statuses[i];
            gMain.savedCallback = CB2_ReinitMainMenu;
            PrepareDifficultyForOptionMenu();
            assert(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION) == 0);
            assert(VarGet(0x40F7) == 0xa554);
        }
        gSaveFileStatus = SAVE_STATUS_OK;
        gMain.savedCallback = InGameCallback;
        PrepareDifficultyForOptionMenu();
        assert(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION) == 0);
    } else if (scenario == 7) {
        VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, 1);
        VarSet(VAR_LEGENDARY_SIGNS_UNLOCKED_0, 0x2356);
        VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 37);
        VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, 0xFFFF);
        SetCurrentDifficultyLevel(DIFFICULTY_EASY);
        MigrateEmeraldChampionsCoreState();
        assert(relicInitializations==1);
        assert(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0)==0);
        assert(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_0)==0x2356);
        assert(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS)==37);
        assert(GetCurrentDifficultyLevel()==DIFFICULTY_EASY);
        assert(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION)==2);
        VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0,0x1234);
        VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1,0x2100);
        gSaveFileStatus=SAVE_STATUS_OK;
        gMain.savedCallback=CB2_ReinitMainMenu;
        PrepareDifficultyForOptionMenu();
        MigrateEmeraldChampionsCoreState();
        assert(relicInitializations==1);
        assert(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0)==0x1234);
        assert(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1)==0x2100);
    } else if (scenario == 6) {
        VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
        VarSet(VAR_LEGENDARY_SIGNS_UNLOCKED_0, 0x2356);
        SetCurrentDifficultyLevel(DIFFICULTY_EASY);
        gSaveFileStatus = SAVE_STATUS_OK;
        gMain.savedCallback = CB2_ReinitMainMenu;
        PrepareDifficultyForOptionMenu();
        assert(GetCurrentDifficultyLevel() == DIFFICULTY_EASY);
        assert(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_0) == 0x2356);
    } else { abort(); }
    return 0;
}
''')
        fixture.write_text("\n".join(chunks))
        result = subprocess.run(
            [compiler, "-std=c11", "-fsanitize=undefined,bounds", "-fno-sanitize-recover=all", "-g", "-I", str(ROOT / "include"),
             str(fixture), "-o", str(cls.executable)], capture_output=True, text=True,
        )
        if result.returncode:
            raise AssertionError(result.stderr)

    def check_scenario(self, number):
        result = subprocess.run([str(self.executable), str(number)], capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_half_party_metadata_matches_party_generation_on_all_modes(self):
        self.check_scenario(0)

    def test_absent_opponent_does_not_read_stale_half_party_metadata(self):
        self.check_scenario(1)

    def test_sentinel_and_facility_ids_do_not_index_campaign_trainers(self):
        self.check_scenario(2)

    def test_invalid_loaded_difficulty_cannot_index_past_tables(self):
        self.check_scenario(3)

    def test_legacy_title_options_preserve_sign_bits_then_continue(self):
        self.check_scenario(4)

    def test_empty_and_ingame_options_do_not_run_migration(self):
        self.check_scenario(5)

    def test_current_title_options_preserve_difficulty_and_signs(self):
        self.check_scenario(6)

    def test_v1_upgrade_and_current_options_preserve_pending_state(self):
        self.check_scenario(7)

    def test_options_source_orders_migration_before_difficulty_read(self):
        # This is a limited wiring check, not execution of the Options callback.
        # Whitespace and brace placement are not requirements.
        init = function("src/option_menu.c", "CB2_InitOptionMenu")
        code = re.sub(r"/\*.*?\*/|//[^\n]*", "", init, flags=re.S)
        migration = re.search(r"\bPrepareDifficultyForOptionMenu\s*\(\s*\)\s*;", code)
        read = re.search(r"\bGetCurrentDifficultyLevel\s*\(\s*\)", code)
        self.assertIsNotNone(migration)
        self.assertIsNotNone(read)
        self.assertLess(migration.start(), read.start())


if __name__ == "__main__":
    unittest.main()
