#include "global.h"
#include "save.h"
#include "agb_flash.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "gba/flash_internal.h"
#include "test/test.h"

TEST("Save recovery: damaged newest slot restores the older complete campaign state")
{
    // This runs against the native test runner's disposable flash image.
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x1234);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 olderCounter = gSaveCounter;
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x5678);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u16 newestSector = NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    EXPECT_EQ(EraseFlashSector(newestSector), 0);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveCounter, olderCounter);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x1234);
    // A new full save repairs the damaged slot and becomes loadable normally.
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x9ABC);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x9ABC);
}

TEST("Save recovery: mixed save counters cannot masquerade as a complete newest slot")
{
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x1357);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 olderCounter = gSaveCounter;
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x2468);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    // Alter only the first physical sector's footer. All payload checksums and
    // logical IDs remain valid, and the last scanned counter remains newest.
    u16 newestSector = NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    struct SaveSector *sector = &gSaveDataBuffer;
    ReadFlash(newestSector, 0, (u8 *)sector, sizeof(*sector));
    sector->counter = olderCounter;
    EXPECT_EQ(ProgramFlashSectorAndVerify(newestSector, (u8 *)sector), 0);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveCounter, olderCounter);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x1357);
    // The legitimate partial-save path must continue to use the same counter
    // as the PC sectors it leaves untouched.
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0xABCD);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 repairedCounter = gSaveCounter;
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x4321);
    EXPECT_EQ(TrySavingData(SAVE_LINK), SAVE_STATUS_OK);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(gSaveCounter, repairedCounter);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x4321);
}

TEST("Save recovery: damaged extension bytes restore the older complete save")
{
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x1111);
    gSaveBlock3Ptr->dexNavChain = 17;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 olderCounter = gSaveCounter;
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x2222);
    gSaveBlock3Ptr->dexNavChain = 29;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 extensionOffset = offsetof(struct SaveBlock3, dexNavChain);
    u16 logicalSector = extensionOffset / SAVE_BLOCK_3_CHUNK_SIZE;
    u16 physicalSector = (logicalSector + gLastWrittenSector) % NUM_SECTORS_PER_SLOT
                       + NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    struct SaveSector *sector = &gSaveDataBuffer;
    ReadFlash(physicalSector, 0, (u8 *)sector, sizeof(*sector));
    sector->saveBlock3Chunk[extensionOffset % SAVE_BLOCK_3_CHUNK_SIZE] ^= 1;
    EXPECT_EQ(ProgramFlashSectorAndVerify(physicalSector, (u8 *)sector), 0);
    gSaveBlock3Ptr->dexNavChain = 0;
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveCounter, olderCounter);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x1111);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 17);
}

static u16 LegacyPayloadChecksum(const u8 *data, u16 size)
{
    u32 sum = 0;
    for (u32 offset = 0; offset + 4 <= size; offset += 4)
        sum += data[offset] | (data[offset + 1] << 8)
             | (data[offset + 2] << 16) | ((u32)data[offset + 3] << 24);
    return (sum >> 16) + sum;
}

TEST("Save recovery: legacy slots load and partial saves upgrade every extension chunk")
{
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x1234);
    gSaveBlock3Ptr->dexNavChain = 37;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u16 slot = NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    struct SaveSector *sector = &gSaveDataBuffer;
    // Reconstruct the historical on-flash format independently of the new
    // checksum helper. Data positions and extension bytes are unchanged.
    for (u32 i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        ReadFlash(slot + i, 0, (u8 *)sector, sizeof(*sector));
        EXPECT_EQ(sector->signature, SECTOR_SIGNATURE_EXTENDED);
        EXPECT(sector->id < NUM_SECTORS_PER_SLOT);
        sector->signature = SECTOR_SIGNATURE;
        sector->checksum = LegacyPayloadChecksum(sector->data, gRamSaveSectorLocations[sector->id].size);
        EXPECT_EQ(ProgramFlashSectorAndVerify(slot + i, (u8 *)sector), 0);
    }
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    gSaveBlock3Ptr->dexNavChain = 0;
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x1234);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 37);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x5678);
    EXPECT_EQ(TrySavingData(SAVE_LINK), SAVE_STATUS_OK);
    for (u32 i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        ReadFlash(slot + i, 0, (u8 *)sector, sizeof(*sector));
        EXPECT_EQ(sector->signature, SECTOR_SIGNATURE_EXTENDED);
    }
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x5678);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 37);
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    slot = NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    for (u32 i = 0; i < NUM_SECTORS_PER_SLOT; i++)
    {
        ReadFlash(slot + i, 0, (u8 *)sector, sizeof(*sector));
        EXPECT_EQ(sector->signature, SECTOR_SIGNATURE_EXTENDED);
    }
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
}

TEST("Save recovery: extended incremental sectors commit with the original final byte")
{
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x9876);
    gSaveBlock3Ptr->dexNavChain = 61;
    EXPECT(!LinkFullSave_Init());
    for (u32 i = 0; i < NUM_SECTORS_PER_SLOT - 1; i++)
        EXPECT(!LinkFullSave_WriteSector());
    EXPECT(LinkFullSave_WriteSector());
    EXPECT(!LinkFullSave_ReplaceLastSector());
    u16 lastSector = (NUM_SECTORS_PER_SLOT - 1 + gLastWrittenSector) % NUM_SECTORS_PER_SLOT
                   + NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS);
    struct SaveSector *sector = &gSaveDataBuffer;
    // Read the footer only: SetLastSectorSignature uses the staged save buffer.
    u32 signature;
    ReadFlash(lastSector, SECTOR_SIGNATURE_OFFSET, (u8 *)&signature, sizeof(signature));
    EXPECT_EQ(signature, SECTOR_SIGNATURE_EXTENDED | 0xFF);
    EXPECT(!LinkFullSave_SetLastSectorSignature());
    ReadFlash(lastSector, 0, (u8 *)sector, sizeof(*sector));
    EXPECT_EQ(sector->signature, SECTOR_SIGNATURE_EXTENDED);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    gSaveBlock3Ptr->dexNavChain = 0;
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0x9876);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 61);
}

TEST("Save recovery: link saves persist the expanded Bag and the live PC")
{
    for (u32 incremental = 0; incremental < 2; incremental++)
    {
        ClearBag();
        struct Pokemon mon;
        CreateMonWithIVs(&mon, SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PLAYER_ID, 17);
        SetBoxMonAt(0, 0, &mon.box);
        EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
        u32 counter = gSaveCounter;
        // Link saves (contests, Frontier, record mixing) write the live PC too;
        // a stale PC beside a newer party let a reload duplicate or lose Pokemon.
        CreateMonWithIVs(&mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 19);
        SetBoxMonAt(0, 0, &mon.box);
        EXPECT(AddBagItem(ITEM_SLOWBRONITE, 1));
        if (incremental)
        {
            EXPECT(!WriteSaveBlock2());
            bool32 finished = FALSE;
            for (u32 step = 0; step < NUM_SECTORS_PER_SLOT && !finished; step++)
                finished = WriteSaveBlock1Sector();
            EXPECT(finished);
        }
        else
            EXPECT_EQ(TrySavingData(SAVE_LINK), SAVE_STATUS_OK);
        EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
        EXPECT_EQ(gSaveCounter, counter);
        EXPECT(CheckBagHasItem(ITEM_SLOWBRONITE, 1));
        EXPECT_EQ(GetBoxMonData(GetBoxedMonPtr(0, 0), MON_DATA_SPECIES), SPECIES_EEVEE);
    }
    ClearBag();
}

TEST("Save recovery: valid fallback receives the same legacy Bag migration as a clean load")
{
    ClearBag();
    gSaveBlock3Ptr->bagPocketLayoutMagic = 0;
    gSaveBlock3Ptr->bagPocketLayoutMagicInverse = 0;
    gSaveBlock1Ptr->bag.items[0].itemId = ITEM_LEFTOVERS;
    gSaveBlock1Ptr->bag.items[0].quantity = 3 ^ (u16)gSaveBlock2Ptr->encryptionKey;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    u32 older = gSaveCounter;
    gSaveBlock1Ptr->bag.items[0].quantity = 7 ^ (u16)gSaveBlock2Ptr->encryptionKey;
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT_EQ(EraseFlashSector(NUM_SECTORS_PER_SLOT * (gSaveCounter % NUM_SAVE_SLOTS)), 0);
    ClearBag();
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_ERROR);
    EXPECT_EQ(gSaveCounter, older);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LEFTOVERS), 3);
    EXPECT_NE(gSaveBlock3Ptr->bagPocketLayoutMagic, 0);
    EXPECT_EQ(gSaveBlock3Ptr->bagPocketLayoutMagicInverse, ~gSaveBlock3Ptr->bagPocketLayoutMagic);
    MigrateBagPocketsIfNeeded();
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LEFTOVERS), 3);
}
