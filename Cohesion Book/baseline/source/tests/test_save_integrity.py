"""Execute save/reload game functions with mocked flash and UI on the host.

These exercise actual status transitions and corrupt-sector decoding, not GBA
flash timing, power loss, or emulator reload callbacks.
"""
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

from test_restart_integrity import declaration

ROOT = Path(__file__).resolve().parents[1]


def fixture_source():
    save = (ROOT / 'src/save.c').read_text()
    header = (ROOT / 'include/save.h').read_text()
    constants = header[header.index('// Each 4 KiB'):header.index('extern u16')]
    signatures = [
        'static u16 CalculateChecksum(void *data, u16 size)',
        'static u8 CopySaveSlotData(u16 sectorId, struct SaveSectorLocation *locations)',
        'static u8 GetSaveValidStatus(const struct SaveSectorLocation *locations)',
        'static u8 TryLoadSaveSlot(u16 sectorId, struct SaveSectorLocation *locations)',
        'u8 TrySavingData(u8 saveType)',
    ]
    return PREAMBLE + constants + ENVIRONMENT + ''.join(declaration(save, s) for s in signatures) + declaration(
        (ROOT / 'src/start_menu.c').read_text(), 'static u8 SaveDoSaveCallback(void)'
    ) + declaration((ROOT / 'src/reload_save.c').read_text(), 'bool32 CanReloadLastSave(void)') + SCENARIOS


PREAMBLE = r'''
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef u8 bool8;
typedef u32 bool32;
#define TRUE 1
#define FALSE 0
#define SAVE_IN_PROGRESS 0
#define GAME_STAT_SAVED_GAME 0
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
'''

ENVIRONMENT = r'''
static u16 gSaveFileStatus, gSaveAttemptStatus, gLastWrittenSector;
static u32 gSaveCounter, gDamagedSaveSectors;
static bool8 gDifferentSaveFile, gFlashMemoryPresent;
static struct SaveSector gSaveDataBuffer, flash[NUM_SECTORS_PER_SLOT * NUM_SAVE_SLOTS];
static struct SaveSector *gReadWriteSector = &gSaveDataBuffer;
static u32 destinations[NUM_SECTORS_PER_SLOT];
static struct SaveSectorLocation locations[NUM_SECTORS_PER_SLOT];
static unsigned extraCopies;
static u8 requestedSaveType;
static bool8 writeFails;
static const u8 gText_PlayerSavedGame[] = "success", gText_SaveError[] = "error";
static const u8 *lastMessage;
static void IncrementGameStat(unsigned stat) { (void)stat; }
static void PausePyramidChallenge(void) {}
static void SaveStartTimer(void) {}
static u8 SaveSuccessCallback(void) { return 0; }
static u8 SaveErrorCallback(void) { return 0; }
static void ShowSaveMessage(const u8 *message, u8 (*callback)(void)) { lastMessage = message; (void)callback; }
static void HandleSavingData(u8 type) { requestedSaveType = type; gDamagedSaveSectors = writeFails; }
static void DoSaveFailedScreen(u8 type) { (void)type; }
static bool8 ReadFlashSector(u8 sector, struct SaveSector *out) { *out = flash[sector]; return TRUE; }
static void CopyToSaveBlock3(u16 id, struct SaveSector *sector) { (void)id; (void)sector; extraCopies++; }
'''

SCENARIOS = r'''
static void Reset(u16 status, bool8 different)
{
    gSaveFileStatus = status;
    gDifferentSaveFile = different;
    gFlashMemoryPresent = TRUE;
    writeFails = FALSE;
    gDamagedSaveSectors = 0;
}

static int Lifecycle(void)
{
    const u16 invalid[] = {SAVE_STATUS_EMPTY, SAVE_STATUS_CORRUPT, SAVE_STATUS_NO_FLASH};
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(*invalid); i++)
    {
        Reset(invalid[i], TRUE);
        CHECK(!CanReloadLastSave());
        writeFails = TRUE;
        SaveDoSaveCallback();
        CHECK(gDifferentSaveFile && gSaveFileStatus == invalid[i]);
        CHECK(!CanReloadLastSave() && lastMessage == gText_SaveError);
        CHECK(requestedSaveType == SAVE_OVERWRITE_DIFFERENT_FILE);
        writeFails = FALSE;
        SaveDoSaveCallback();
        CHECK(!gDifferentSaveFile && gSaveFileStatus == SAVE_STATUS_OK);
        CHECK(CanReloadLastSave() && lastMessage == gText_PlayerSavedGame);
        CHECK(requestedSaveType == SAVE_OVERWRITE_DIFFERENT_FILE);
        writeFails = TRUE;
        SaveDoSaveCallback();
        CHECK(CanReloadLastSave() && gSaveFileStatus == SAVE_STATUS_OK);
        CHECK(!gDifferentSaveFile && requestedSaveType == SAVE_NORMAL);
        CHECK(lastMessage == gText_SaveError);
    }
    Reset(SAVE_STATUS_NO_FLASH, TRUE);
    gFlashMemoryPresent = FALSE;
    SaveDoSaveCallback();
    CHECK(gDifferentSaveFile && !CanReloadLastSave());
    Reset(SAVE_STATUS_EMPTY, FALSE); // default/reset flag is not proof of a save
    CHECK(!CanReloadLastSave());
    Reset(SAVE_STATUS_ERROR, FALSE); // valid redundant slot recovered on load
    CHECK(CanReloadLastSave());
    writeFails = TRUE;
    SaveDoSaveCallback();
    CHECK(CanReloadLastSave() && gSaveFileStatus == SAVE_STATUS_ERROR);
    return 0;
}

static int OtherSaves(void)
{
    const u8 complete[] = {SAVE_NORMAL, SAVE_HALL_OF_FAME, SAVE_OVERWRITE_DIFFERENT_FILE, SAVE_HALL_OF_FAME_ERASE_BEFORE};
    for (unsigned i = 0; i < sizeof(complete); i++)
    {
        Reset(SAVE_STATUS_CORRUPT, TRUE);
        CHECK(TrySavingData(complete[i]) == SAVE_STATUS_OK);
        CHECK(CanReloadLastSave() && !gDifferentSaveFile);
    }
    const u8 partial[] = {SAVE_LINK, SAVE_EREADER};
    for (unsigned i = 0; i < sizeof(partial); i++)
    {
        Reset(SAVE_STATUS_EMPTY, TRUE);
        CHECK(TrySavingData(partial[i]) == SAVE_STATUS_OK);
        CHECK(!CanReloadLastSave() && gDifferentSaveFile);
    }
    return 0;
}

static void SeedFlash(void)
{
    memset(flash, 0xff, sizeof(flash));
    for (unsigned id = 0; id < NUM_SECTORS_PER_SLOT; id++)
    {
        destinations[id] = 0xdeadbeef;
        locations[id] = (struct SaveSectorLocation){&destinations[id], sizeof(u32)};
    }
    gSaveCounter = 0;
    extraCopies = 0;
}

static void ValidSlot(unsigned slot)
{
    for (unsigned id = 0; id < NUM_SECTORS_PER_SLOT; id++)
    {
        struct SaveSector *s = &flash[slot * NUM_SECTORS_PER_SLOT + id];
        memset(s, 0, sizeof(*s));
        s->id = id;
        s->signature = SECTOR_SIGNATURE;
        s->counter = slot;
        *(u32 *)s->data = 100 + id;
        s->checksum = CalculateChecksum(s->data, sizeof(u32));
    }
}

static int Corruption(void)
{
    SeedFlash();
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_EMPTY);
    CHECK(extraCopies == 0);
    for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) CHECK(destinations[i] == 0xdeadbeef);
    const u16 badIds[] = {NUM_SECTORS_PER_SLOT, 31, 32, 0xffff};
    for (unsigned bad = 0; bad < sizeof(badIds) / sizeof(*badIds); bad++)
        for (unsigned slot = 0; slot < NUM_SAVE_SLOTS; slot++)
        {
            SeedFlash();
            struct SaveSector *s = &flash[slot * NUM_SECTORS_PER_SLOT];
            s->signature = SECTOR_SIGNATURE;
            s->id = badIds[bad];
            CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_CORRUPT);
            CHECK(extraCopies == 0);
            ValidSlot(1 - slot);
            CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_ERROR);
            CHECK(gSaveCounter == 1 - slot && extraCopies == NUM_SECTORS_PER_SLOT);
            for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) CHECK(destinations[i] == 100 + i);
        }
    SeedFlash();
    ValidSlot(0);
    ValidSlot(1);
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_OK);
    CHECK(gSaveCounter == 1 && extraCopies == NUM_SECTORS_PER_SLOT);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "lifecycle")) return Lifecycle();
    if (!strcmp(argv[1], "other-saves")) return OtherSaves();
    if (!strcmp(argv[1], "corruption")) return Corruption();
    return 2;
}
'''


class SaveIntegrityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        compiler = shutil.which('cc')
        if compiler is None:
            raise AssertionError('host C compiler required')
        cls.directory = tempfile.TemporaryDirectory(prefix='ec-save-integrity-')
        cls.addClassCleanup(cls.directory.cleanup)
        directory = Path(cls.directory.name)
        source = directory / 'save.c'
        source.write_text(fixture_source())
        cls.executable = directory / 'save'
        result = subprocess.run([compiler, '-std=gnu11', '-Wall', '-Wextra',
                                 '-Wno-unused-parameter', '-Wno-sign-compare',
                                 '-fsanitize=undefined', '-fno-sanitize-recover=all',
                                 str(source), '-o', str(cls.executable)], capture_output=True, text=True)
        if result.returncode:
            raise AssertionError(result.stdout + result.stderr)

    def scenario(self, name):
        result = subprocess.run([str(self.executable), name], capture_output=True, text=True, timeout=20)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_first_save_failure_success_repair_and_later_failure(self):
        self.scenario('lifecycle')

    def test_full_and_partial_save_state(self):
        self.scenario('other-saves')

    def test_erased_invalid_and_recovered_sectors(self):
        self.scenario('corruption')


if __name__ == '__main__':
    unittest.main()
