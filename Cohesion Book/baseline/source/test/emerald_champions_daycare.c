#include "global.h"
#include "daycare.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "constants/region_map_sections.h"
#include "random.h"
#include "test/overworld_script.h"
#include "test/test.h"

static void ResetNursery(void)
{
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    FlagClear(FLAG_PENDING_DAYCARE_EGG);
    FlagClear(FLAG_EC_HATCHED_DAYCARE_EGG);
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
    EXPECT(!FlagGet(FLAG_EC_HATCHED_DAYCARE_EGG)); // Receiving an egg is not a hatch.
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
    EXPECT(!FlagGet(FLAG_EC_HATCHED_DAYCARE_EGG));
    ResetNursery();
}
