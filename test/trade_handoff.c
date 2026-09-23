#include "global.h"
#include "pokemon.h"
#include "trade.h"
#include "event_data.h"
#include "constants/party_menu.h"
#include "test/test.h"

extern void Test_FinishInGameTrade(void);

TEST("NPC trade handoff: selected party slot is independent of the authored trade ID")
{
    u32 slot;
    PARAMETRIZE { slot = 0; }
    PARAMETRIZE { slot = 2; }
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    for (u32 i = 0; i < 3; i++)
    {
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 10, i, OTID_STRUCT_PLAYER_ID);
        CalculateMonStats(&gParties[B_TRAINER_PLAYER][i]);
    }
    gPartiesCount[B_TRAINER_PLAYER] = 3;
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_CYCLIZAR, 10, 0, OTID_STRUCT_PRESET(42));
    CalculateMonStats(&gParties[B_TRAINER_OPPONENT_A][0]);
    enum Item held = ITEM_LEFTOVERS;
    SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, &held);
    gSpecialVar_0x8004 = slot;
    gSpecialVar_0x8005 = 1;
    Test_FinishInGameTrade();
    for (u32 i = 0; i < 3; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES), i == slot ? SPECIES_CYCLIZAR : SPECIES_EEVEE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_EEVEE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_FRIENDSHIP), 70);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
}

TEST("NPC trade handoff: Type Null cannot add a second Legendary to the party")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SHAYMIN, 14, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_SKITTY, 14, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8005 = INGAME_TRADE_TYPE_NULL;
    gSpecialVar_0x8004 = 1;
    EXPECT(!CanReceiveInGameTradePokemon());
    gSpecialVar_0x8004 = PC_MON_CHOSEN;
    EXPECT(CanReceiveInGameTradePokemon());
    gSpecialVar_0x8004 = 0;
    EXPECT(CanReceiveInGameTradePokemon()); // Swapping the current Legendary is legal.
    ZeroPlayerPartyMons();
}
