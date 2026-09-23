#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script_pokemon_util.h"
#include "test/test.h"
#include "constants/items.h"

TEST("Restricted party: Legendary, Ultra Beast, and Paradox categories are independent")
{
    ZeroPlayerPartyMons();
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_SHAYMIN), RESTRICTED_PARTY_LEGENDARY);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_MEWTWO), RESTRICTED_PARTY_LEGENDARY);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_KARTANA), RESTRICTED_PARTY_ULTRA_BEAST);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_NAGANADEL), RESTRICTED_PARTY_ULTRA_BEAST);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_FLUTTER_MANE), RESTRICTED_PARTY_PARADOX);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_ROARING_MOON), RESTRICTED_PARTY_PARADOX);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_URSALUNA_BLOODMOON), RESTRICTED_PARTY_NONE);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_GARCHOMP), RESTRICTED_PARTY_NONE);

    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SHAYMIN, 14, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_KARTANA, 14, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_FLUTTER_MANE, 14, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT(PlayerPartyWithinRestrictedLimit());
    // The League door uses the same one/one/one rule as the party.
    EXPECT(PlayerPartyLeagueEligible());
    EXPECT(!CanAddRestrictedMonToParty(SPECIES_MEWTWO, PARTY_SIZE));
    EXPECT(!CanAddRestrictedMonToParty(SPECIES_NAGANADEL, PARTY_SIZE));
    EXPECT(!CanAddRestrictedMonToParty(SPECIES_ROARING_MOON, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_URSALUNA_BLOODMOON, PARTY_SIZE));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_MEWTWO, 0));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_NAGANADEL, 1));
    EXPECT(CanAddRestrictedMonToParty(SPECIES_ROARING_MOON, 2));
    // A second member of any one category closes the door.
    static const enum Species extras[] = {SPECIES_MEWTWO, SPECIES_NAGANADEL, SPECIES_ROARING_MOON};
    for (u32 i = 0; i < ARRAY_COUNT(extras); i++)
    {
        CreateMon(&gParties[B_TRAINER_PLAYER][3], extras[i], 14, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT(!PlayerPartyWithinRestrictedLimit());
        EXPECT(!PlayerPartyLeagueEligible());
    }
    ZeroMonData(&gParties[B_TRAINER_PLAYER][3]);
    EXPECT(PlayerPartyLeagueEligible());
    ZeroMonData(&gParties[B_TRAINER_PLAYER][0]);
    EXPECT(PlayerPartyLeagueEligible());
    ZeroPlayerPartyMons();
    EXPECT(PlayerPartyLeagueEligible());
}

TEST("Restricted party: extra captures and gifts go to storage without losing any category")
{
    struct Pokemon mon;

    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    CreateMon(&mon, SPECIES_SHAYMIN, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    CreateMon(&mon, SPECIES_MEWTWO, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PC);
    CreateMon(&mon, SPECIES_KARTANA, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, PARTY_SIZE), MON_GIVEN_TO_PARTY);
    CreateMon(&mon, SPECIES_NAGANADEL, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, 2), MON_GIVEN_TO_PC);
    CreateMon(&mon, SPECIES_FLUTTER_MANE, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    CreateMon(&mon, SPECIES_ROARING_MOON, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PC);
    EXPECT_EQ(CalculatePlayerPartyCount(), 3);
    EXPECT(PlayerPartyWithinRestrictedLimit());
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_SPECIES), SPECIES_MEWTWO);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][1], MON_DATA_SPECIES), SPECIES_NAGANADEL);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][2], MON_DATA_SPECIES), SPECIES_ROARING_MOON);
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
}

TEST("Restricted party: a boxed Legendary gift can replace the party Legendary safely")
{
    struct Pokemon gift;
    u16 oldItem = ITEM_LEFTOVERS;
    u16 giftItem = ITEM_FOCUS_SASH;

    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SHAYMIN, 14, 111, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &oldItem);
    CreateMon(&gift, SPECIES_MEWTWO, 14, 222, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gift, MON_DATA_HELD_ITEM, &giftItem);

    EXPECT_EQ(GiveScriptedMonToPlayer(&gift, PARTY_SIZE), MON_GIVEN_TO_PC);
    struct BoxPokemon *box = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_SHAYMIN);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_SPECIES), SPECIES_MEWTWO);

    u16 mail = ITEM_ORANGE_MAIL;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &mail);
    EXPECT_EQ(GetBoxedLegendaryGiftSwapStatus(), 2);
    EXPECT(!SwapBoxedLegendaryGiftWithParty());
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_SPECIES), SPECIES_MEWTWO);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &oldItem);
    EXPECT_EQ(GetBoxedLegendaryGiftSwapStatus(), 1);
    EXPECT(SwapBoxedLegendaryGiftWithParty());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_MEWTWO);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), giftItem);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_SPECIES), SPECIES_SHAYMIN);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_HELD_ITEM), oldItem);
    EXPECT(PlayerPartyWithinRestrictedLimit());

    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
}

TEST("Restricted party: a scripted Meltan gift offers the Legendary swap")
{
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SHAYMIN, 14, 333, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ScriptGiveMon(SPECIES_MELTAN, 5, ITEM_NONE), MON_GIVEN_TO_PC);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_MELTAN), RESTRICTED_PARTY_LEGENDARY);
    EXPECT_EQ(GetBoxedLegendaryGiftSwapStatus(), 1);
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
}
