#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script_pokemon_util.h"
#include "test/test.h"
#include "constants/items.h"

// src/pokemon.c SetPlayerMonBaselineEVs: 252 HP, 52 Attack/Defense/Sp. Atk/
// Sp. Def, 50 Speed, on every path by which a Pokemon joins the player.
extern void ScriptHatchMon(void);
extern void CreateInGameTradePokemon(void);

static const u8 sBaseline[NUM_STATS] = {252, 52, 52, 50, 52, 52};

static void ExpectBaselineEVs(struct Pokemon *mon)
{
    u32 total = 0;
    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP_EV + stat), sBaseline[stat]);
        total += GetMonData(mon, MON_DATA_HP_EV + stat);
    }
    EXPECT_EQ(total, MAX_TOTAL_EVS);
}

static void ExpectNoEVs(struct Pokemon *mon)
{
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP_EV + stat), 0);
}

TEST("Baseline EVs: catches, gifts and starters arrive with the full spread")
{
    struct Pokemon mon;
    u32 authored = MAX_PER_STAT_EVS;

    ZeroPlayerPartyMons();
    // A caught Pokemon's wild or legendary set EVs give way to the baseline.
    CreateMon(&mon, SPECIES_ZIGZAGOON, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_SPEED_EV, &authored);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    ExpectBaselineEVs(&gParties[B_TRAINER_PLAYER][0]);
    // The baseline counts in the Pokemon's real stats on arrival.
    CreateMon(&mon, SPECIES_ZIGZAGOON, 10, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP), GetMonData(&mon, MON_DATA_MAX_HP));

    // Scripted gifts: to the first free slot, and to a fixed slot (starters).
    EXPECT_EQ(ScriptGiveMon(SPECIES_WINGULL, 10, ITEM_NONE), MON_GIVEN_TO_PARTY);
    ExpectBaselineEVs(&gParties[B_TRAINER_PLAYER][1]);
    CreateMon(&mon, SPECIES_TREECKO, 5, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, 2), MON_GIVEN_TO_PARTY);
    ExpectBaselineEVs(&gParties[B_TRAINER_PLAYER][2]);

    // A full party sends the next one to the PC, still with the baseline.
    for (u32 slot = 3; slot < PARTY_SIZE; slot++)
    {
        CreateMon(&mon, SPECIES_POOCHYENA, 10, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    }
    CreateMon(&mon, SPECIES_TAILLOW, 10, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PC);
    struct BoxPokemon *boxed = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    EXPECT_EQ(GetBoxMonData(boxed, MON_DATA_HP_EV), MAX_PER_STAT_EVS);
    EXPECT_EQ(GetBoxMonData(boxed, MON_DATA_SPEED_EV), 50);
    ZeroBoxMonData(boxed);
    ZeroPlayerPartyMons();
}

TEST("Baseline EVs: an Egg has none until it hatches")
{
    ZeroPlayerPartyMons();
    EXPECT_EQ(ScriptGiveEgg(SPECIES_AZURILL), MON_GIVEN_TO_PARTY);
    ExpectNoEVs(&gParties[B_TRAINER_PLAYER][0]);
    gSpecialVar_0x8004 = 0;
    ScriptHatchMon();
    EXPECT(!GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG));
    ExpectBaselineEVs(&gParties[B_TRAINER_PLAYER][0]);
    ZeroPlayerPartyMons();
}

TEST("Baseline EVs: an in-game trade partner's Pokemon arrives with the spread")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_RALTS, 10, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = 0; // The first in-game trade.
    CreateInGameTradePokemon();
    ExpectBaselineEVs(&gParties[B_TRAINER_OPPONENT_A][0]);
    ZeroMonData(&gParties[B_TRAINER_OPPONENT_A][0]);
    ZeroPlayerPartyMons();
}

TEST("Baseline EVs: storage never resets a spread the player chose")
{
    struct Pokemon mon;
    u32 zero = 0, max = MAX_PER_STAT_EVS;

    ZeroPlayerPartyMons();
    CreateMon(&mon, SPECIES_RALTS, 10, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    struct Pokemon *owned = &gParties[B_TRAINER_PLAYER][0];
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(owned, MON_DATA_HP_EV + stat, &zero);
    SetMonData(owned, MON_DATA_SPATK_EV, &max);
    EXPECT_EQ(CopyMonToPC(owned), MON_GIVEN_TO_PC);
    struct BoxPokemon *boxed = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    EXPECT_EQ(GetBoxMonData(boxed, MON_DATA_HP_EV), 0);
    EXPECT_EQ(GetBoxMonData(boxed, MON_DATA_SPATK_EV), MAX_PER_STAT_EVS);
    BoxMonToMon(boxed, &mon);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_EV), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK_EV), MAX_PER_STAT_EVS);
    ZeroBoxMonData(boxed);
    ZeroPlayerPartyMons();
}
