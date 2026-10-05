#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "legendary_signs.h"
#include "caps.h"
#include "item.h"
#include "pokedex.h"
#include "overworld.h"
#include "rtc.h"
#include "fake_rtc.h"
#include "test/test.h"

extern void Test_CommitEvolution(struct Pokemon *mon, enum Species before, enum Species after);

TEST("Restricted evolution: blocked Gimmighoul stays pending at cap and retries at the same level")
{
    enum Species base;
    PARAMETRIZE { base = SPECIES_GIMMIGHOUL; }
    PARAMETRIZE { base = SPECIES_GIMMIGHOUL_ROAMING; }
    static const u16 flags[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
        FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET, FLAG_IS_CHAMPION,
        FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT};
    bool8 saved[ARRAY_COUNT(flags)];
    for (u32 i = 0; i < ARRAY_COUNT(flags); i++)
    {
        saved[i] = FlagGet(flags[i]);
        FlagClear(flags[i]);
        if (i < 4)
            FlagSet(flags[i]);
    }
    u8 savedSeen[sizeof(gSaveBlock1Ptr->dexSeen)], savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedSeen, gSaveBlock1Ptr->dexSeen, sizeof(savedSeen));
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    EXPECT_EQ(GetCurrentLevelCap(), 45);
    ZeroPlayerPartyMons();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, base, 45, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_ENTEI, 45, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT_EQ(GetEvolutionTargetSpecies(mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_GHOLDENGO);
    EXPECT(IsMonEligibleForLeveler(mon));
    EXPECT(!CanEvolveMonWithinRestrictedLimit(mon, SPECIES_GHOLDENGO));
    Test_CommitEvolution(mon, base, SPECIES_GHOLDENGO);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), base);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 45);
    EXPECT(IsMonEligibleForLeveler(mon));
    ZeroMonData(&gParties[B_TRAINER_PLAYER][1]);
    CalculatePlayerPartyCount();
    EXPECT(CanEvolveMonWithinRestrictedLimit(mon, SPECIES_GHOLDENGO));
    EXPECT_EQ(GetEvolutionTargetSpecies(mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_GHOLDENGO);
    Test_CommitEvolution(mon, base, SPECIES_GHOLDENGO);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_GHOLDENGO);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 45);
    EXPECT(PlayerPartyWithinRestrictedLimit());
    ZeroPlayerPartyMons();
    memcpy(gSaveBlock1Ptr->dexSeen, savedSeen, sizeof(savedSeen));
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    for (u32 i = 0; i < ARRAY_COUNT(flags); i++)
        if (saved[i]) FlagSet(flags[i]); else FlagClear(flags[i]);
}

TEST("Restricted evolution: Ursaluna item use preserves Peat Block while another restricted mon is present")
{
    struct Time savedOffset = gSaveBlock2Ptr->localTimeOffset;
    struct SiiRtcInfo savedRtc = *FakeRtc_GetCurrentTime();
    RtcCalcLocalTime();
    RtcCalcLocalTimeOffset(gLocalTime.days, 21, 0, 0);
    EXPECT_EQ(GetTimeOfDay(), TIME_NIGHT);
    ZeroPlayerPartyMons();
    ClearBag();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_URSARING, 45, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GHOLDENGO, 45, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT(AddBagItem(ITEM_PEAT_BLOCK, 1));
    EXPECT_EQ(GetEvolutionTargetSpecies(mon, EVO_MODE_ITEM_CHECK, ITEM_PEAT_BLOCK, NULL, NULL, CHECK_EVO), SPECIES_URSALUNA);
    EXPECT(!CanEvolveMonWithinRestrictedLimit(mon, SPECIES_URSALUNA));
    // The direct item-effect entry cannot debit or open a blocked evolution.
    EXPECT(ExecuteTableBasedItemEffect(mon, ITEM_PEAT_BLOCK, 0, 0));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_PEAT_BLOCK), 1);
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_URSARING);
    EXPECT(CanEvolveMonWithinRestrictedLimit(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_URSALUNA));
    *FakeRtc_GetCurrentTime() = savedRtc;
    gSaveBlock2Ptr->localTimeOffset = savedOffset;
    RtcCalcLocalTime();
    GetTimeOfDay();
    ZeroPlayerPartyMons();
    ClearBag();
}

TEST("Restricted wild: regular Ursaluna's Pokedex record does not close Bloodmoon; its own record does")
{
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    bool8 saved = FlagGet(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON);
    FlagClear(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON);
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_URSALUNA, 40, 0, OTID_STRUCT_PLAYER_ID);
    HandleSetPokedexFlagFromMon(&mon, FLAG_SET_CAUGHT);
    EXPECT(IsWildSlotSpeciesAcquirable(SPECIES_URSALUNA_BLOODMOON));
    CreateMon(&mon, SPECIES_URSALUNA_BLOODMOON, 40, 0, OTID_STRUCT_PLAYER_ID);
    HandleSetPokedexFlagFromMon(&mon, FLAG_SET_CAUGHT);
    EXPECT(!IsWildSlotSpeciesAcquirable(SPECIES_URSALUNA_BLOODMOON));
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    if (!saved) FlagClear(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON);
}

TEST("Restricted party: named final forms consume the shared slot and their pre-evolutions do not")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ENTEI, 40, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(!CanAddRestrictedMonToParty(SPECIES_GHOLDENGO, PARTY_SIZE));
    EXPECT(!CanAddRestrictedMonToParty(SPECIES_URSALUNA, PARTY_SIZE));
    EXPECT(!CanAddRestrictedMonToParty(SPECIES_URSALUNA_BLOODMOON, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_GIMMIGHOUL, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_GIMMIGHOUL_ROAMING, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_TEDDIURSA, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_URSARING, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_URSALUNA_BLOODMOON, 0));
    ZeroPlayerPartyMons();
}

TEST("Restricted wild: the rules lesson remembers an older save's boxed Bloodmoon")
{
    bool8 hadCaught = FlagGet(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON);
    struct BoxPokemon *boxmon = GetBoxedMonPtr(0, 0);
    struct BoxPokemon saved = *boxmon;
    ZeroPlayerPartyMons();
    FlagClear(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON);
    CreateBoxMon(boxmon, SPECIES_URSALUNA_BLOODMOON, 40, 0, OTID_STRUCT_PLAYER_ID);
    RecordOwnedBloodmoon();
    EXPECT(FlagGet(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON));
    EXPECT(!IsWildSlotSpeciesAcquirable(SPECIES_URSALUNA_BLOODMOON));
    *boxmon = saved;
    if (!hadCaught) FlagClear(FLAG_EC_CAUGHT_URSALUNA_BLOODMOON);
}
