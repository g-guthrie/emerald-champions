#include "global.h"
#include "pokemon.h"
#include "trade.h"
#include "event_data.h"
#include "legendary_signs.h"
#include "constants/party_menu.h"
#include "constants/trade.h"
#include "constants/vars.h"
#include "test/test.h"

extern void Test_FinishInGameTrade(void);
extern void CreateInGameTradePokemon(void);

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

TEST("NPC trade handoff: Type Null shares the special slot across categories")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_SHAYMIN; }
    PARAMETRIZE { species = SPECIES_KARTANA; }
    PARAMETRIZE { species = SPECIES_FLUTTER_MANE; }
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], species, 14, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_SKITTY, 14, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8005 = INGAME_TRADE_TYPE_NULL;
    gSpecialVar_0x8004 = 1;
    EXPECT(!CanReceiveInGameTradePokemon());
    gSpecialVar_0x8004 = PC_MON_CHOSEN;
    EXPECT(CanReceiveInGameTradePokemon());
    gSpecialVar_0x8004 = 0;
    EXPECT(CanReceiveInGameTradePokemon()); // Swapping the current special Pokemon is legal.
    ZeroPlayerPartyMons();
}

TEST("NPC trade handoff: a traded Type: Null counts as caught")
{
    static const u16 caughtVars[] = {
        VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
    };
    for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
        VarSet(caughtVars[i], 0);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SKITTY, 30, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = INGAME_TRADE_TYPE_NULL;
    CreateInGameTradePokemon();
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_TYPE_NULL));
    Test_FinishInGameTrade();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_TYPE_NULL);
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_TYPE_NULL));
    for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
        VarSet(caughtVars[i], 0);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
}

TEST("NPC trade handoff: link trades distinguish the shared-slot refusal from other failures")
{
    extern u8 Test_CheckValidityOfTradeMons(u8 playerSlot, u8 partnerSlot);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_KARTANA, 20, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_FLUTTER_MANE, 20, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(Test_CheckValidityOfTradeMons(1, 0), TRADE_RESTRICTED_PARTY);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
}
