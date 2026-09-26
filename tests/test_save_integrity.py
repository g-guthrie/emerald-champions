"""Execute save/reload game functions with emulated flash and stubbed UI on the host.

The whole of src/save.c, src/start_menu.c and src/reload_save.c is compiled
on the host, including the real write path; flash is a host byte array behind
the agb_flash interface. These exercise actual status transitions and
corrupt-sector decoding, not GBA flash timing, power loss, or emulator reload
callbacks.
"""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

HOST_SAVE = r'''
struct HostSave { bool8 writeFails, erasedHallOfFame; unsigned slotWrites; const u8 *lastMessage; };
extern struct HostSave gHost;
extern struct SaveSector gHostFlash[SECTORS_COUNT];
u8 HostSaveDoSaveCallback(void);
'''

# Other units: flash chip, save blocks, game stats, message box and strings.
BOUNDARY = r'''
#include "global.h"
#include "gba/flash_internal.h"
#include "agb_flash.h"
#include "battle_pyramid.h"
#include "hall_of_fame.h"
#include "load_save.h"
#include "menu.h"
#include "new_game.h"
#include "overworld.h"
#include "pokemon_storage_system.h"
#include "save.h"
#include "string_util.h"
#include "strings.h"
#include "trainer_hill.h"
''' + host_c.ASSERTS + r'''
#include "host_save.h"
struct HostSave gHost;
struct SaveSector gHostFlash[SECTORS_COUNT];
static struct SaveBlock1 sSave1;
static struct SaveBlock2 sSave2;
static struct PokemonStorage sStorage;
struct SaveBlock1 *gSaveBlock1Ptr = &sSave1;
struct SaveBlock2 *gSaveBlock2Ptr = &sSave2;
struct PokemonStorage *gPokemonStoragePtr = &sStorage;
struct SaveBlock3 gSaveblock3;
struct HallofFameTeam *gHoFSaveBuffer;
u32 *gTrainerHillVBlankCounter;
bool32 gFlashMemoryPresent;
bool8 gDifferentSaveFile;
u8 gStringVar4[0x3E8];
const u8 gText_PlayerSavedGame[] = _("success");
const u8 gText_SaveError[] = _("error");

static u16 HostProgramFlashByte(u16 sector, u32 offset, u8 data)
{
    assert(sector < SECTORS_COUNT && offset < SECTOR_SIZE);
    if (gHost.writeFails) return 1;
    ((u8 *)&gHostFlash[sector])[offset] = data;
    return 0;
}
static u16 HostEraseFlashSector(u16 sector)
{
    assert(sector < SECTORS_COUNT);
    if (sector >= SECTOR_ID_HOF_1) gHost.erasedHallOfFame = TRUE;
    memset(&gHostFlash[sector], 0xff, SECTOR_SIZE);
    return 0;
}
u16 (*ProgramFlashByte)(u16, u32, u8) = HostProgramFlashByte;
u16 (*EraseFlashSector)(u16) = HostEraseFlashSector;
u32 ProgramFlashSectorAndVerify(u16 sector, u8 *src)
{
    assert(sector < SECTORS_COUNT);
    if (sector < NUM_SECTORS_PER_SLOT * NUM_SAVE_SLOTS) gHost.slotWrites++;
    if (gHost.writeFails) return 1;
    memcpy(&gHostFlash[sector], src, SECTOR_SIZE);
    return 0;
}
void ReadFlash(u16 sector, u32 offset, u8 *dest, u32 size)
{
    assert(sector < SECTORS_COUNT && offset + size <= SECTOR_SIZE);
    memcpy(dest, (u8 *)&gHostFlash[sector] + offset, size);
}
void CopyPartyAndObjectsToSave(void) {}
u32 GetGameStat(u8 index) { return 0; }
void IncrementGameStat(u8 index) {}
void PausePyramidChallenge(void) {}
void DoSaveFailedScreen(u8 saveType) {}
u8 *StringExpandPlaceholders(u8 *dest, const u8 *src) { gHost.lastMessage = src; return dest; }
void LoadMessageBoxAndFrameGfx(u8 windowId, bool8 copyToVram) {}
void AddTextPrinterForMessage(bool8 allowSkippingDelayWithButtonPress) {}
'''

START_MENU = r'''
u8 HostSaveDoSaveCallback(void) { return SaveDoSaveCallback(); }
'''

# Save/text callbacks registered for later frames; this fixture never runs them.
UNCALLED = ('ClearStdWindowAndFrame', 'IsSEPlaying', 'IsTextPrinterActiveOnWindow', 'PlaySE', 'RemoveWindow', 'gMain')

PRELUDE = r'''
#include "reload_save.h"
#include "strings.h"
#include "host_save.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
#define flash gHostFlash
static u32 destinations[NUM_SECTORS_PER_SLOT];
static struct SaveSectorLocation locations[NUM_SECTORS_PER_SLOT];
static void DoSave(void)
{
    gHost.erasedHallOfFame = FALSE;
    gHost.slotWrites = 0;
    HostSaveDoSaveCallback();
}
// SaveBlock3 chunks copied by the loader, observed through gSaveblock3.
static unsigned SaveBlock3Sectors(void)
{
    unsigned sectors = 0;
    for (u32 id = 0; id < NUM_SECTORS_PER_SLOT; id++) sectors += SaveBlock3Size(id) != 0;
    return sectors;
}
static void ResetSaveBlock3(void)
{
    assert(SaveBlock3Sectors() != 0);
    memset(&gSaveblock3, 0xee, sizeof(gSaveblock3));
}
static unsigned SaveBlock3Copies(void)
{
    unsigned copies = 0;
    for (u32 id = 0; id < NUM_SECTORS_PER_SLOT; id++)
    {
        const u8 *chunk = (const u8 *)&gSaveblock3 + id * SAVE_BLOCK_3_CHUNK_SIZE;
        bool32 copied = SaveBlock3Size(id) != 0;
        for (u32 i = 0; i < SaveBlock3Size(id); i++) copied &= chunk[i] == 0;
        copies += copied;
    }
    return copies;
}
'''

SCENARIOS = PRELUDE + r'''
static void Reset(u16 status, bool8 different)
{
    gSaveFileStatus = status;
    gDifferentSaveFile = different;
    gFlashMemoryPresent = TRUE;
    gHost.writeFails = FALSE;
    gDamagedSaveSectors = 0;
}

static int Lifecycle(void)
{
    const u16 invalid[] = {SAVE_STATUS_EMPTY, SAVE_STATUS_CORRUPT, SAVE_STATUS_NO_FLASH};
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(*invalid); i++)
    {
        Reset(invalid[i], TRUE);
        CHECK(!CanReloadLastSave());
        gHost.writeFails = TRUE;
        DoSave();
        CHECK(gDifferentSaveFile && gSaveFileStatus == invalid[i]);
        CHECK(!CanReloadLastSave() && gHost.lastMessage == gText_SaveError);
        CHECK(gHost.erasedHallOfFame && gHost.slotWrites);
        gHost.writeFails = FALSE;
        DoSave();
        CHECK(!gDifferentSaveFile && gSaveFileStatus == SAVE_STATUS_OK);
        CHECK(CanReloadLastSave() && gHost.lastMessage == gText_PlayerSavedGame);
        CHECK(gHost.erasedHallOfFame && gHost.slotWrites);
        gHost.writeFails = TRUE;
        DoSave();
        CHECK(CanReloadLastSave() && gSaveFileStatus == SAVE_STATUS_OK);
        CHECK(!gDifferentSaveFile && !gHost.erasedHallOfFame && gHost.slotWrites);
        CHECK(gHost.lastMessage == gText_SaveError);
    }
    Reset(SAVE_STATUS_NO_FLASH, TRUE);
    gFlashMemoryPresent = FALSE;
    DoSave();
    CHECK(gDifferentSaveFile && !CanReloadLastSave());
    Reset(SAVE_STATUS_EMPTY, FALSE); // default/reset flag is not proof of a save
    CHECK(!CanReloadLastSave());
    Reset(SAVE_STATUS_ERROR, FALSE); // valid redundant slot recovered on load
    CHECK(CanReloadLastSave());
    gHost.writeFails = TRUE;
    DoSave();
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
    ResetSaveBlock3();
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

static void ValidExtendedSlot(unsigned slot)
{
    for (unsigned id = 0; id < NUM_SECTORS_PER_SLOT; id++)
    {
        struct SaveSector *s = &flash[slot * NUM_SECTORS_PER_SLOT + id];
        memset(s, 0, sizeof(*s));
        s->id = id;
        s->signature = SECTOR_SIGNATURE_EXTENDED;
        s->counter = slot;
        *(u32 *)s->data = 100 + id;
        s->checksum = CalculateChecksum(s->data, offsetof(struct SaveSector, id));
    }
}

static int ExtendedFormat(void)
{
    SeedFlash();
    ValidExtendedSlot(0);
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_OK);
    for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) CHECK(destinations[i] == 100 + i);

    // Incremental saves can legitimately update only part of a legacy slot.
    SeedFlash();
    ValidSlot(0);
    for (unsigned id = 0; id <= SECTOR_ID_SAVEBLOCK1_END; id++)
    {
        struct SaveSector *s = &flash[id];
        s->signature = SECTOR_SIGNATURE_EXTENDED;
        s->checksum = CalculateChecksum(s->data, offsetof(struct SaveSector, id));
    }
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_OK);
    for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) CHECK(destinations[i] == 100 + i);

    // Both edges of every SaveBlock3 chunk are in the extended checksum range.
    for (unsigned id = 0; id < NUM_SECTORS_PER_SLOT; id++)
        for (unsigned edge = 0; edge < 2; edge++)
        {
            SeedFlash();
            ValidExtendedSlot(0);
            flash[id].saveBlock3Chunk[edge ? SAVE_BLOCK_3_CHUNK_SIZE - 1 : 0] ^= 1;
            CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_CORRUPT);
            CHECK(destinations[id] == 0xdeadbeef);
        }

    SeedFlash();
    ValidExtendedSlot(0);
    flash[0].signature = 0x08032025;
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_CORRUPT);
    CHECK(destinations[0] == 0xdeadbeef);

    SeedFlash();
    ValidExtendedSlot(0);
    flash[1].counter++;
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_CORRUPT);
    return 0;
}

static int Corruption(void)
{
    SeedFlash();
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_EMPTY);
    CHECK(SaveBlock3Copies() == 0);
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
            CHECK(SaveBlock3Copies() == 0);
            ValidSlot(1 - slot);
            CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_ERROR);
            CHECK(gSaveCounter == 1 - slot && SaveBlock3Copies() == SaveBlock3Sectors());
            for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) CHECK(destinations[i] == 100 + i);
        }
    // A valid sector ID/signature does not make a corrupt payload trustworthy.
    // Mutate after computing the original checksum, never using the production
    // checksum helper to manufacture the corrupted sector's expected result.
    for (unsigned slot = 0; slot < NUM_SAVE_SLOTS; slot++)
        for (unsigned corruptPayload = 0; corruptPayload < 2; corruptPayload++)
        {
            SeedFlash();
            ValidSlot(slot);
            struct SaveSector *s = &flash[slot * NUM_SECTORS_PER_SLOT];
            if (corruptPayload)
                s->data[0] ^= 1;
            else
                s->checksum ^= 1;
            CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_CORRUPT);
            CHECK(destinations[0] == 0xdeadbeef);
            // The loader may copy individually valid sectors even when the
            // complete save is rejected. Only the damaged sector must not copy.
            ResetSaveBlock3();
            for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) destinations[i] = 0xdeadbeef;
            // Recover only the intact alternate slot, not any of the invalid
            // slot's otherwise well-formed sectors or its damaged payload.
            ValidSlot(1 - slot);
            CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_ERROR);
            CHECK(gSaveCounter == 1 - slot && SaveBlock3Copies() == SaveBlock3Sectors());
            for (unsigned i = 0; i < NUM_SECTORS_PER_SLOT; i++) CHECK(destinations[i] == 100 + i);
        }
    SeedFlash();
    ValidSlot(0);
    ValidSlot(1);
    CHECK(TryLoadSaveSlot(FULL_SAVE_SLOT, locations) == SAVE_STATUS_OK);
    CHECK(gSaveCounter == 1 && SaveBlock3Copies() == SaveBlock3Sectors());
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "lifecycle")) return Lifecycle();
    if (!strcmp(argv[1], "other-saves")) return OtherSaves();
    if (!strcmp(argv[1], "corruption")) return Corruption();
    if (!strcmp(argv[1], "extended-format")) return ExtendedFormat();
    return 2;
}
'''


class SaveIntegrityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory = tempfile.TemporaryDirectory(prefix='ec-save-integrity-')
        cls.addClassCleanup(cls.directory.cleanup)
        directory = Path(cls.directory.name)
        (directory / 'host_save.h').write_text(HOST_SAVE)
        cls.executable = host_c.build(directory, {
            'save.c': host_c.production('src/save.c') + SCENARIOS,
            'start_menu.c': host_c.production('src/start_menu.c') + START_MENU,
            'reload_save.c': host_c.production('src/reload_save.c'),
            'boundary.c': BOUNDARY,
        }, name='save', flags=('-iquote', str(directory)), inert=UNCALLED)

    def scenario(self, name):
        host_c.run(self.executable, name, timeout=20)

    def test_first_save_failure_success_repair_and_later_failure(self):
        self.scenario('lifecycle')

    def test_full_and_partial_save_state(self):
        self.scenario('other-saves')

    def test_erased_invalid_and_recovered_sectors(self):
        self.scenario('corruption')

    def test_legacy_extended_and_incremental_mixed_formats(self):
        self.scenario('extended-format')


if __name__ == '__main__':
    unittest.main()
