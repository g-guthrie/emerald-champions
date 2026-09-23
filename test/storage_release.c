#include "global.h"
#include "item.h"
#include "malloc.h"
#include "event_data.h"
#include "constants/party_menu.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "test/test.h"

extern bool32 Test_StorageHasEnoughMonsToRelease(bool8 moving);

TEST("Storage release: Eggs do not replace the second doubles Pokemon")
{
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            ZeroBoxMonAt(box, slot);
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_PIKACHU, 10, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_TOGEPI, 1, 0, OTID_STRUCT_PLAYER_ID);
    bool8 egg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_IS_EGG, &egg);
    EXPECT(!Test_StorageHasEnoughMonsToRelease(FALSE));
    // Moving the Egg into storage must not change the answer.
    gPokemonStoragePtr->boxes[0][0] = gParties[B_TRAINER_PLAYER][2].box;
    ZeroMonData(&gParties[B_TRAINER_PLAYER][2]);
    EXPECT(!Test_StorageHasEnoughMonsToRelease(FALSE));
    egg = FALSE;
    SetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_IS_EGG, &egg);
    EXPECT(Test_StorageHasEnoughMonsToRelease(FALSE));
    // Picking up that third non-Egg temporarily removes its occupied slot.
    ZeroBoxMonData(&gPokemonStoragePtr->boxes[0][0]);
    EXPECT(Test_StorageHasEnoughMonsToRelease(TRUE));
    ZeroMonData(&gParties[B_TRAINER_PLAYER][1]);
    EXPECT(!Test_StorageHasEnoughMonsToRelease(TRUE));
    ZeroPlayerPartyMons();

}

extern bool32 Test_StorageWindowFailureExit(u8 previousDestination);

TEST("Storage initialization: unavailable window always selects the exit destination")
{
    u32 destination;
    PARAMETRIZE { destination = 0; }
    PARAMETRIZE { destination = 1; }
    PARAMETRIZE { destination = 2; }
    PARAMETRIZE { destination = 3; }
    PARAMETRIZE { destination = 255; }
    EXPECT(Test_StorageWindowFailureExit(destination));
}

extern bool32 Test_RecoverStorageCursor(struct Pokemon *mon, u16 item, u8 box, u8 slot);

TEST("Storage recovery: displaced Pokemon never overwrites an occupied origin")
{
    bool32 partyHole;
    PARAMETRIZE { partyHole = TRUE; }
    PARAMETRIZE { partyHole = FALSE; }
    struct Pokemon held, resident;
    CreateMonWithIVs(&held, SPECIES_PIKACHU, 5, 0, OTID_STRUCT_PLAYER_ID, 31);
    CreateMonWithIVs(&resident, SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID, 31);
    for (u32 i = 0; i < PARTY_SIZE; i++)
        gParties[B_TRAINER_PLAYER][i] = resident;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            gPokemonStoragePtr->boxes[box][slot] = resident.box;
    if (partyHole)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1]);
    else
        ZeroBoxMonAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1);
    EXPECT(Test_RecoverStorageCursor(&held, ITEM_NONE, 0, 0));
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_EEVEE);
    if (partyHole)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1], MON_DATA_SPECIES), SPECIES_PIKACHU);
    else
        EXPECT_EQ(GetBoxMonDataAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, MON_DATA_SPECIES), SPECIES_PIKACHU);
    u32 count = 0;
    for (u32 i = 0; i < PARTY_SIZE; i++)
        count += GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES) == SPECIES_PIKACHU;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            count += GetBoxMonDataAt(box, slot, MON_DATA_SPECIES) == SPECIES_PIKACHU;
    EXPECT_EQ(count, 1);
}

TEST("Storage recovery: carried item uses Bag, then PC, then an empty holder")
{
    u32 sink;
    PARAMETRIZE { sink = 0; }
    PARAMETRIZE { sink = 1; }
    PARAMETRIZE { sink = 2; }
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID, 31);
    if (sink > 0)
    {
        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_LEFTOVERS)];
        for (u32 i = 0; i < pocket->capacity; i++)
            BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_CHOICE_BAND, MAX_BAG_ITEM_CAPACITY);
    }
    if (sink > 1)
        for (u32 i = 0; i < PC_ITEMS_COUNT; i++)
            gSaveBlock1Ptr->pcItems[i] = (struct ItemSlot){ITEM_POTION, MAX_PC_ITEM_CAPACITY};
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    bool32 recovered = Test_RecoverStorageCursor(NULL, ITEM_LEFTOVERS, 0, 0);
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    EXPECT(recovered);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LEFTOVERS), sink == 0);
    EXPECT_EQ((bool32)CheckPCHasItem(ITEM_LEFTOVERS, 1), sink == 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), sink == 2 ? ITEM_LEFTOVERS : ITEM_NONE);
}

extern bool32 Test_StorageFailedSelection(void);

TEST("Storage recovery: failed scripted selection clears a stale success")
{
    gSpecialVar_Result = TRUE;
    gSpecialVar_0x8004 = 0;
    EXPECT(Test_StorageFailedSelection());
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(gSpecialVar_0x8004, PARTY_NOTHING_CHOSEN);
}
