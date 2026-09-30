#include "global.h"
#include "test/test.h"
#include "field_control_avatar.h"
#include "pokemon.h"
#include "pokerus.h"
#include "constants/maps.h"
#include "constants/metatile_behaviors.h"

static void PrepareHotSpringCarrier(void)
{
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    CalculatePlayerPartyCount();
    GiveMonPokerus(&gParties[B_TRAINER_PLAYER][0], TRUE);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_LAVARIDGE_TOWN);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_LAVARIDGE_TOWN);
    ResetHotSpringPokerusSteps();
}

TEST("Hot spring steps: preparation happens on step 25 without applying treatment")
{
    u8 oldGroup = gSaveBlock1Ptr->location.mapGroup;
    u8 oldNum = gSaveBlock1Ptr->location.mapNum;
    PrepareHotSpringCarrier();
    for (u32 step = 0; step < 24; step++)
    {
        EXPECT(!UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
        EXPECT(CheckMonPokerus(&gParties[B_TRAINER_PLAYER][0]));
        EXPECT(!HasHotSpringPokerus(&gParties[B_TRAINER_PLAYER][0]));
    }
    EXPECT(UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
    EXPECT(CheckMonPokerus(&gParties[B_TRAINER_PLAYER][0]));
    EXPECT(IsMonReadyForHotSpringTreatment(&gParties[B_TRAINER_PLAYER][0]));
    EXPECT(!HasHotSpringPokerus(&gParties[B_TRAINER_PLAYER][0]));
    for (u32 step = 0; step < 50; step++)
        EXPECT(!UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
    ZeroPlayerPartyMons();
    ResetHotSpringPokerusSteps();
    gSaveBlock1Ptr->location.mapGroup = oldGroup;
    gSaveBlock1Ptr->location.mapNum = oldNum;
}

TEST("Hot spring steps: dry ground, another map and map reload reset an incomplete soak")
{
    u32 interruption;
    PARAMETRIZE { interruption = 0; }
    PARAMETRIZE { interruption = 1; }
    PARAMETRIZE { interruption = 2; }
    u8 oldGroup = gSaveBlock1Ptr->location.mapGroup;
    u8 oldNum = gSaveBlock1Ptr->location.mapNum;
    PrepareHotSpringCarrier();
    for (u32 step = 0; step < 24; step++)
        EXPECT(!UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
    if (interruption == 0)
        EXPECT(!UpdateHotSpringPokerusSteps(MB_NORMAL));
    else if (interruption == 1)
    {
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_LITTLEROOT_TOWN);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_LITTLEROOT_TOWN);
        EXPECT(!UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_LAVARIDGE_TOWN);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_LAVARIDGE_TOWN);
    }
    else
        ResetHotSpringPokerusSteps();
    for (u32 step = 0; step < 24; step++)
        EXPECT(!UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
    EXPECT(UpdateHotSpringPokerusSteps(MB_HOT_SPRINGS));
    ZeroPlayerPartyMons();
    ResetHotSpringPokerusSteps();
    gSaveBlock1Ptr->location.mapGroup = oldGroup;
    gSaveBlock1Ptr->location.mapNum = oldNum;
}
