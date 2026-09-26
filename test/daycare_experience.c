#include "global.h"
#include "daycare.h"
#include "event_data.h"
#include "caps.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"

TEST("Daycare experience: the board raises no levels however far the player walks")
{
    u32 steps;
    PARAMETRIZE { steps = 0; }
    PARAMETRIZE { steps = 100; }
    PARAMETRIZE { steps = 1000000; }
    PARAMETRIZE { steps = UINT_MAX; }
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    u32 before = GetMonData(mon, MON_DATA_EXP);
    StorePokemonInDaycare(mon, &gSaveBlock1Ptr->daycare.mons[0]);
    gSaveBlock1Ptr->daycare.mons[0].steps = steps;
    gSpecialVar_0x8004 = 0;
    EXPECT_EQ(GetNumLevelsGainedFromDaycare(), 0);
    gSpecialVar_0x8005 = 9999;
    GetDaycareCostAndPrepareString();
    EXPECT_EQ(gSpecialVar_0x8005, 0);
    EXPECT_EQ(TakePokemonFromDaycare(), SPECIES_EEVEE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_EXP), before);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 5);
    ZeroPlayerPartyMons();
}

TEST("Daycare experience: Hoopa comes back Confined at the level it was left")
{
    ZeroPlayerPartyMons();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_HOOPA_UNBOUND, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    StorePokemonInDaycare(mon, &gSaveBlock1Ptr->daycare.mons[0]);
    gSaveBlock1Ptr->daycare.mons[0].steps = 1000000;
    struct BoxPokemon original = gSaveBlock1Ptr->daycare.mons[0].mon;
    gSpecialVar_0x8004 = 0;
    EXPECT_EQ(GetNumLevelsGainedFromDaycare(), 0);
    EXPECT_EQ(memcmp(&original, &gSaveBlock1Ptr->daycare.mons[0].mon, sizeof(original)), 0);
    EXPECT_EQ(TakePokemonFromDaycare(), SPECIES_HOOPA_CONFINED);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 5);
    ZeroPlayerPartyMons();
}
