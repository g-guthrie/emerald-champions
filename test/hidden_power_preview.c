#include "global.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/characters.h"
#include "constants/items.h"

extern void PayForChosenMonHiddenPower(void);

TEST("Hidden Power preview: all IV changes, including low Speed, are disclosed without mutation or payment")
{
    ClearBag();
    ZeroPlayerPartyMons();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMonWithIVs(mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    u32 speed = 0;
    SetMonData(mon, MON_DATA_SPEED_IV, &speed);
    CalculateMonStats(mon);
    EXPECT(AddBagItem(ITEM_BOTTLE_CAP, 3));
    struct Pokemon before = *mon;
    static const u8 fighting[] = _("HP{CLEAR_TO 58}31{CLEAR_TO 76}{RIGHT_ARROW}{CLEAR_TO 94}31\n"
                                  "Attack{CLEAR_TO 58}31{CLEAR_TO 76}{RIGHT_ARROW}{CLEAR_TO 94}0\n"
                                  "Defense{CLEAR_TO 58}31{CLEAR_TO 76}{RIGHT_ARROW}{CLEAR_TO 94}30\n"
                                  "Speed{CLEAR_TO 58}0{CLEAR_TO 76}{RIGHT_ARROW}{CLEAR_TO 94}30\n"
                                  "Sp. Atk{CLEAR_TO 58}31{CLEAR_TO 76}{RIGHT_ARROW}{CLEAR_TO 94}30\n"
                                  "Sp. Def{CLEAR_TO 58}31{CLEAR_TO 76}{RIGHT_ARROW}{CLEAR_TO 94}30");
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = 0;
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(StringCompare(gStringVar1, COMPOUND_STRING("Fighting")), 0);
    EXPECT_EQ(StringCompare(gStringVar4, fighting), 0);
    EXPECT_EQ(memcmp(mon, &before, sizeof(before)), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 3);
    // Apply is a separate, explicitly confirmed script action.
    PayForChosenMonHiddenPower();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPEED_IV), 30);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 0);
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    PayForChosenMonHiddenPower();
    EXPECT_EQ(gSpecialVar_Result, 0); // unchanged remains free
}

TEST("Hidden Power preview: invalid slots, eggs and invalid types never offer a change")
{
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = 16;
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(gStringVar1[0], EOS);
    EXPECT_EQ(gStringVar4[0], EOS);
    gSpecialVar_0x8005 = 0;
    gSpecialVar_0x8004 = PARTY_SIZE;
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    gSpecialVar_0x8004 = 1; // empty slot
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    gSpecialVar_0x8004 = 0;
    u32 isEgg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &isEgg);
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    isEgg = FALSE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &isEgg);
    isEgg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SANITY_IS_BAD_EGG, &isEgg);
    BufferChosenMonHiddenPowerPreview();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
}
