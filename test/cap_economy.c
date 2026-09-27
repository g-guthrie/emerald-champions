#include "global.h"
#include "test/test.h"
#include "pokemon.h"
#include "item.h"
#include "event_data.h"
#include "emerald_champions_battle_sets.h"
#include "constants/items.h"
#include "constants/move_relearner.h"
#include "constants/vars.h"
#include "constants/flags.h"

extern void ExchangeSootForCaps(void);
extern void PayForChosenMonIVChange(void);
extern void PayForChosenMonHiddenPower(void);
extern u16 GetNatureChangerBerry(void);

TEST("Cap economy: soot trades debit spendable soot only and reject insufficient funds")
{
    ClearBag();
    VarSet(VAR_ASH_GATHER_COUNT, 1999);
    VarSet(VAR_EC_SOOT_PROGRESS, 4321);
    gSpecialVar_0x8004 = 5;
    ExchangeSootForCaps();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 1999);
    gSpecialVar_0x8004 = 9;
    ExchangeSootForCaps();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 1);
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 999);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), 4321);
    ExchangeSootForCaps();
    EXPECT_EQ(gSpecialVar_Result, 0);
}

TEST("Cap economy: Ivy charges for changes only and restores the mon when caps are missing")
{
    ClearBag();
    ZeroPlayerPartyMons();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMonWithIVs(mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = STAT_ATK;
    gSpecialVar_0x8006 = 0;
    PayForChosenMonIVChange();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(GetMonData(mon, MON_DATA_ATK_IV), 31);
    EXPECT(AddBagItem(ITEM_BOTTLE_CAP, 4));
    PayForChosenMonIVChange();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(GetMonData(mon, MON_DATA_ATK_IV), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 3);
    PayForChosenMonIVChange();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 3);
    gSpecialVar_0x8005 = 0;
    PayForChosenMonHiddenPower();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 0);
    PayForChosenMonHiddenPower();
    EXPECT_EQ(gSpecialVar_Result, 0);
}

TEST("Cap economy: iconic purchases survive boxing evolution and relearning without cross-mon access")
{
    ClearBag();
    struct Pokemon mon;
    enum Move move = MOVE_SPARKLY_SWIRL;
    enum Species evolution = SPECIES_SYLVEON;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    EXPECT(AddBagItem(ITEM_GOLD_BOTTLE_CAP, 1));
    EXPECT(!PayForIconicMove(&mon.box, move, ITEM_GOLD_BOTTLE_CAP)); // no lesson
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), 1);
    SetMonMoveSlot(&mon, move, 0);
    EXPECT(PayForIconicMove(&mon.box, move, ITEM_GOLD_BOTTLE_CAP));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), 0);
    EXPECT(IsIconicMoveUnlocked(&mon.box, move)); // receipt bit 8
    struct BoxPokemon boxed = mon.box;
    SetBoxMonData(&boxed, MON_DATA_SPECIES, &evolution);
    SetBoxMonMoveSlot(&boxed, MOVE_TACKLE, 0);
    EXPECT(IsIconicMoveUnlocked(&boxed, move));
    enum Move list[MAX_RELEARNER_MOVES];
    u32 count = GetEmeraldChampionsIconicMovesToLearn(&boxed, list);
    bool32 found = FALSE;
    for (u32 i = 0; i < count; i++)
        found |= list[i] == move;
    EXPECT(found);
    SetBoxMonMoveSlot(&boxed, move, 0);
    EXPECT(PayForIconicMove(&boxed, move, ITEM_BOTTLE_CAP)); // no caps needed
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    EXPECT(!IsIconicMoveUnlocked(&mon.box, move));
}

TEST("Cap economy: nature ingredient is stable for a saved day and covers ordinary berry ids")
{
    FlagClear(FLAG_SYS_CLOCK_SET);
    bool8 seen[ITEM_MARANGA_BERRY - FIRST_BERRY_INDEX + 1] = {FALSE};
    for (u32 seed = 0; seed < 4096; seed++)
    {
        gSaveBlock1Ptr->dailySeed = seed;
        u32 berry = GetNatureChangerBerry();
        EXPECT(berry >= FIRST_BERRY_INDEX && berry <= ITEM_MARANGA_BERRY);
        EXPECT_EQ(berry, GetNatureChangerBerry());
        seen[berry - FIRST_BERRY_INDEX] = TRUE;
    }
    for (u32 i = 0; i < ARRAY_COUNT(seen); i++)
        EXPECT(seen[i]);
}

TEST("Cap economy: failed soot delivery keeps the full balance")
{
    ClearBag();
    EXPECT(AddBagItem(ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY));
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BOTTLE_CAP)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY);
    VarSet(VAR_ASH_GATHER_COUNT, 1000);
    VarSet(VAR_EC_SOOT_PROGRESS, 250);
    gSpecialVar_0x8004 = 1;
    ExchangeSootForCaps();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 1000);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), 250);
}
