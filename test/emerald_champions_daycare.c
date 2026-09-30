#include "global.h"
#include "daycare.h"
#include "event_data.h"
#include "item.h"
#include "malloc.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "constants/party_menu.h"
#include "constants/region_map_sections.h"
#include "random.h"
#include "test/overworld_script.h"
#include "test/test.h"

static void ResetNursery(void)
{
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    FlagClear(FLAG_PENDING_DAYCARE_EGG);
}

TEST("Nursery deposit: party and PC transfers preserve Pokemon without free heap space")
{
    bool32 fromPc;
    PARAMETRIZE { fromPc = FALSE; }
    PARAMETRIZE { fromPc = TRUE; }
    ResetNursery();
    ResetPokemonStorageSystem();
    struct Pokemon original;
    CreateMonWithIVs(&original, SPECIES_EEVEE, 5, 12345, OTID_STRUCT_PLAYER_ID, 7);
    SetMonMoveSlot(&original, MOVE_QUICK_ATTACK, 0);
    u32 pp = 3, ev = 200, item = ITEM_LEFTOVERS;
    SetMonData(&original, MON_DATA_PP1, &pp);
    SetMonData(&original, MON_DATA_SPEED_EV, &ev);
    SetMonData(&original, MON_DATA_HELD_ITEM, &item);
    CalculateMonStats(&original);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_WINGULL, 5, 0, OTID_STRUCT_PLAYER_ID, 31);
    struct Pokemon companion = gParties[B_TRAINER_PLAYER][0];
    if (fromPc)
    {
        SetBoxMonAt(0, 0, &original.box);
        gSpecialVar_MonBoxId = gSpecialVar_MonBoxPos = 0;
        gSpecialVar_0x8004 = PC_MON_CHOSEN;
    }
    else
    {
        gParties[B_TRAINER_PLAYER][1] = original;
        gSpecialVar_0x8004 = 1;
    }
    CalculatePlayerPartyCount();
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
    StoreSelectedPokemonInDaycare();
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    EXPECT_EQ(memcmp(&gSaveBlock1Ptr->daycare.mons[0].mon, &original.box, sizeof(original.box)), 0);
    EXPECT_EQ(memcmp(&gParties[B_TRAINER_PLAYER][0], &companion, sizeof(companion)), 0);
    EXPECT_EQ(CalculatePlayerPartyCount(), 1);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(CountPokemonInDaycare(&gSaveBlock1Ptr->daycare), 1);
    ResetNursery();
    ResetPokemonStorageSystem();
}

TEST("Nursery egg names: gift and bred eggs store only the padded nickname")
{
    bool32 bred;
    PARAMETRIZE { bred = FALSE; }
    PARAMETRIZE { bred = TRUE; }
    static const u8 expectedName[POKEMON_NAME_BUFFER_SIZE] = _("タマゴ");
    struct Pokemon egg, expected;
    ResetNursery();
    if (bred)
    {
        CreateMonWithIVs(&egg, SPECIES_BULBASAUR, 5, 0, OTID_STRUCT_PLAYER_ID, 31);
        gSaveBlock1Ptr->daycare.mons[0].mon = egg.box;
        CreateMonWithIVs(&egg, SPECIES_DITTO, 5, 0, OTID_STRUCT_PLAYER_ID, 31);
        gSaveBlock1Ptr->daycare.mons[1].mon = egg.box;
        TriggerPendingDaycareEgg();
        EXPECT(GiveEggFromDaycare());
        egg = gParties[B_TRAINER_PLAYER][0];
    }
    else
        CreateEgg(&egg, SPECIES_BULBASAUR, TRUE);
    expected = egg;
    SetMonData(&expected, MON_DATA_NICKNAME, expectedName);
    EXPECT_EQ(memcmp(&egg, &expected, sizeof(egg)), 0);
    EXPECT(GetMonData(&egg, MON_DATA_IS_EGG));
    ResetNursery();
}

TEST("Nursery deposit: a full Day Care leaves both the selection and its occupants untouched")
{
    bool32 fromPc;
    PARAMETRIZE { fromPc = FALSE; }
    PARAMETRIZE { fromPc = TRUE; }
    ResetNursery();
    ResetPokemonStorageSystem();
    struct Pokemon original;
    CreateMonWithIVs(&original, SPECIES_EEVEE, 5, 12345, OTID_STRUCT_PLAYER_ID, 7);
    for (u32 i = 0; i < DAYCARE_MON_COUNT; i++)
    {
        gSaveBlock1Ptr->daycare.mons[i].mon = original.box;
        gSaveBlock1Ptr->daycare.mons[i].steps = 100 + i;
    }
    struct DayCare saved = gSaveBlock1Ptr->daycare;
    gSpecialVar_MonBoxId = gSpecialVar_MonBoxPos = 0;
    if (fromPc)
        SetBoxMonAt(0, 0, &original.box);
    else
        gParties[B_TRAINER_PLAYER][0] = original;
    CalculatePlayerPartyCount();
    gSpecialVar_0x8004 = fromPc ? PC_MON_CHOSEN : 0;
    StoreSelectedPokemonInDaycare();
    const struct BoxPokemon *selected = fromPc ? GetBoxedMonPtr(0, 0) : &gParties[B_TRAINER_PLAYER][0].box;
    EXPECT_EQ(memcmp(selected, &original.box, sizeof(original.box)), 0);
    EXPECT_EQ(memcmp(&gSaveBlock1Ptr->daycare, &saved, sizeof(saved)), 0);
    EXPECT_EQ(CalculatePlayerPartyCount(), fromPc ? 0 : 1);
    ResetNursery();
    ResetPokemonStorageSystem();
}

TEST("Nursery parents produce the baby species without Incense")
{
    enum Species parent, baby;
    PARAMETRIZE { parent = SPECIES_SNORLAX; baby = SPECIES_MUNCHLAX; }
    PARAMETRIZE { parent = SPECIES_MR_MIME; baby = SPECIES_MIME_JR; }
    PARAMETRIZE { parent = SPECIES_CHIMECHO; baby = SPECIES_CHINGLING; }
    PARAMETRIZE { parent = SPECIES_CHANSEY; baby = SPECIES_HAPPINY; }
    PARAMETRIZE { parent = SPECIES_MANTINE; baby = SPECIES_MANTYKE; }
    ResetNursery();
    VarSet(VAR_TEMP_C, parent);
    RUN_OVERWORLD_SCRIPT(givemon VAR_TEMP_C, 25, item=ITEM_NONE; givemon SPECIES_DITTO, 25, item=ITEM_NONE;);
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[0]);
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[1]);
    EXPECT_NE(GetDaycareCompatibilityScore(&gSaveBlock1Ptr->daycare), 0);
    TriggerPendingDaycareEgg();
    GiveEggFromDaycare();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), baby);
    EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MET_LOCATION), METLOC_DAYCARE_EGG);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_FRIENDSHIP), 5);
    ResetNursery();
}

TEST("Nursery gifted and existing eggs hatch within 768 steps")
{
    bool32 existing;
    bool32 readyToHatch = FALSE;
    PARAMETRIZE { existing = FALSE; }
    PARAMETRIZE { existing = TRUE; }
    ResetNursery();
    CreateEgg(&gParties[B_TRAINER_PLAYER][0], SPECIES_TOGEPI, FALSE);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    if (existing)
    {
        u8 cycles = 40;
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_FRIENDSHIP, &cycles);
    }
    gSpecialVar_0x8004 = PARTY_SIZE;
    for (u32 steps = 0; steps < 768 && !readyToHatch; steps++)
        readyToHatch = ShouldEggHatch();
    EXPECT(readyToHatch);
    EXPECT_EQ(gSpecialVar_0x8004, 0);
    ResetNursery();
}

TEST("Nursery frequent egg checks preserve incompatible pair restrictions")
{
    ResetNursery();
    ClearBag();
    EXPECT(AddBagItem(ITEM_OVAL_CHARM, 1));
    RUN_OVERWORLD_SCRIPT(givemon SPECIES_DITTO, 25; givemon SPECIES_TOGEPI, 25;);
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[0]);
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[1]);
    EXPECT_EQ(GetDaycareCompatibilityScore(&gSaveBlock1Ptr->daycare), 0);
    for (u32 steps = 0; steps < 1024; steps++)
        IncrementDaycareSteps();
    EXPECT(!FlagGet(FLAG_PENDING_DAYCARE_EGG));
    ResetNursery();
    ClearBag();
}

TEST("Nursery checks compatible parents after 63 steps and keeps one pending egg")
{
    ResetNursery();
    RUN_OVERWORLD_SCRIPT(givemon SPECIES_SNORLAX, 25; givemon SPECIES_DITTO, 25;);
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[0]);
    StorePokemonInDaycare(&gParties[B_TRAINER_PLAYER][0], &gSaveBlock1Ptr->daycare.mons[1]);
    SET_RNG(RNG_DAYCARE_MAKE_EGG, TRUE);
    for (u32 steps = 1; steps < 63; steps++)
        IncrementDaycareSteps();
    EXPECT(!FlagGet(FLAG_PENDING_DAYCARE_EGG));
    IncrementDaycareSteps();
    EXPECT(FlagGet(FLAG_PENDING_DAYCARE_EGG));
    for (u32 steps = 0; steps < 64; steps++)
        IncrementDaycareSteps();
    EXPECT(FlagGet(FLAG_PENDING_DAYCARE_EGG));
    ResetNursery();
}

TEST("Nursery gift eggs never carry the Daycare breeding origin")
{
    bool32 hotSprings;
    PARAMETRIZE { hotSprings = FALSE; }
    PARAMETRIZE { hotSprings = TRUE; }
    ResetNursery();
    CreateEgg(&gParties[B_TRAINER_PLAYER][0], SPECIES_TOGEPI, hotSprings);
    EXPECT_NE(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MET_LOCATION), METLOC_DAYCARE_EGG);
    ResetNursery();
}

TEST("Nursery Nidoran and Volbeat lines breed both species")
{
    // The offspring gender roll splits Nidoran♂/♀ and Volbeat/Illumise eggs.
    enum Species parent, male, female;
    PARAMETRIZE { parent = SPECIES_NIDORAN_M; male = SPECIES_NIDORAN_M; female = SPECIES_NIDORAN_F; }
    PARAMETRIZE { parent = SPECIES_VOLBEAT; male = SPECIES_VOLBEAT; female = SPECIES_ILLUMISE; }
    ResetNursery();
    struct Pokemon mon;
    CreateMon(&mon, parent, 25, 0, OTID_STRUCT_PLAYER_ID);
    StorePokemonInDaycare(&mon, &gSaveBlock1Ptr->daycare.mons[0]);
    CreateMon(&mon, SPECIES_DITTO, 25, 0, OTID_STRUCT_PLAYER_ID);
    StorePokemonInDaycare(&mon, &gSaveBlock1Ptr->daycare.mons[1]);
    bool32 sawMale = FALSE, sawFemale = FALSE;
    for (u32 i = 0; i < 32 && !(sawMale && sawFemale); i++)
    {
        ZeroPlayerPartyMons();
        TriggerPendingDaycareEgg();
        GiveEggFromDaycare();
        enum Species egg = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES);
        sawMale |= egg == male;
        sawFemale |= egg == female;
    }
    EXPECT(sawMale);
    EXPECT(sawFemale);
    ResetNursery();
}

TEST("Nursery withdrawal obeys the party rule like a PC withdrawal")
{
    struct Pokemon mon;

    ResetNursery();
    ZeroPlayerPartyMons();
    CreateMon(&mon, SPECIES_KYOGRE, 50, 0, OTID_STRUCT_PLAYER_ID);
    gSaveBlock1Ptr->daycare.mons[0].mon = mon.box;
    gSpecialVar_0x8004 = 0;
    // No Legendary in the party: the board can hand Kyogre back.
    EXPECT(CanTakeDaycareMonWithinPartyRule());
    // A Legendary already in the party: refused, exactly as the PC refuses.
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_GROUDON, 50, 0, OTID_STRUCT_PLAYER_ID);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    EXPECT(!CanTakeDaycareMonWithinPartyRule());
    // An Ultra Beast shares the same slot and is also refused.
    CreateMon(&mon, SPECIES_NIHILEGO, 50, 0, OTID_STRUCT_PLAYER_ID);
    gSaveBlock1Ptr->daycare.mons[0].mon = mon.box;
    EXPECT(!CanTakeDaycareMonWithinPartyRule());
    // An out-of-range slot never passes.
    gSpecialVar_0x8004 = 2;
    EXPECT(!CanTakeDaycareMonWithinPartyRule());
    gSpecialVar_0x8004 = 0;
    ZeroPlayerPartyMons();
    ResetNursery();
}

TEST("Nursery Phione egg waits safely while the shared special slot is occupied")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_SHAYMIN; }
    PARAMETRIZE { species = SPECIES_KARTANA; }
    PARAMETRIZE { species = SPECIES_FLUTTER_MANE; }
    struct Pokemon mon;
    ResetNursery();
    CreateMon(&mon, SPECIES_MANAPHY, 25, 0, OTID_STRUCT_PLAYER_ID);
    gSaveBlock1Ptr->daycare.mons[0].mon = mon.box;
    CreateMon(&mon, SPECIES_DITTO, 25, 0, OTID_STRUCT_PLAYER_ID);
    gSaveBlock1Ptr->daycare.mons[1].mon = mon.box;
    TriggerPendingDaycareEgg();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], species, 25, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(!GiveEggFromDaycare());
    EXPECT(FlagGet(FLAG_PENDING_DAYCARE_EGG));
    EXPECT_EQ(CalculatePlayerPartyCount(), 1);
    EXPECT(PlayerPartyWithinRestrictedLimit());
    ZeroPlayerPartyMons();
    EXPECT(GiveEggFromDaycare());
    EXPECT(!FlagGet(FLAG_PENDING_DAYCARE_EGG));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_PHIONE);
    EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG));
    EXPECT(!CanAddRestrictedMonToParty(species, PARTY_SIZE));
    ResetNursery();
}
