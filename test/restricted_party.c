#include "global.h"
#include "event_data.h"
#include "item.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "script_pokemon_util.h"
#include "test/test.h"
#include "constants/items.h"

TEST("Restricted party: special categories share one slot, including forms and legendary-like Paradox")
{
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_SHAYMIN), RESTRICTED_PARTY_LEGENDARY);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_MEWTWO), RESTRICTED_PARTY_LEGENDARY);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_KARTANA), RESTRICTED_PARTY_ULTRA_BEAST);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_NAGANADEL), RESTRICTED_PARTY_ULTRA_BEAST);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_FLUTTER_MANE), RESTRICTED_PARTY_PARADOX);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_ROARING_MOON), RESTRICTED_PARTY_PARADOX);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_URSALUNA_BLOODMOON), RESTRICTED_PARTY_NONE);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_URSALUNA), RESTRICTED_PARTY_NONE);
    EXPECT_EQ(GetRestrictedPartyClass(SPECIES_GARCHOMP), RESTRICTED_PARTY_NONE);
    static const enum Species special[] = {
        SPECIES_SHAYMIN_SKY, SPECIES_MEWTWO, SPECIES_NAGANADEL, SPECIES_FLUTTER_MANE,
        SPECIES_WALKING_WAKE, SPECIES_IRON_LEAVES, SPECIES_RAGING_BOLT,
        SPECIES_GOUGING_FIRE, SPECIES_IRON_BOULDER, SPECIES_IRON_CROWN,
    };

    ZeroPlayerPartyMons();
    EXPECT(PlayerPartyLeagueEligible());
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_GARCHOMP, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(PlayerPartyWithinRestrictedLimit());
    EXPECT(PlayerPartyLeagueEligible());
    for (u32 i = 0; i < ARRAY_COUNT(special); i++)
    {
        EXPECT_NE(GetRestrictedPartyClass(special[i]), RESTRICTED_PARTY_NONE);
        CreateMon(&gParties[B_TRAINER_PLAYER][0], special[i], 14, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT(PlayerPartyWithinRestrictedLimit());
        EXPECT(PlayerPartyLeagueEligible());
        EXPECT(CanAddRestrictedMonToParty(SPECIES_URSALUNA_BLOODMOON, PARTY_SIZE));
        for (u32 j = 0; j < ARRAY_COUNT(special); j++)
        {
            EXPECT(!CanAddRestrictedMonToParty(special[j], PARTY_SIZE));
            EXPECT(!CanAddRestrictedMonToParty(special[j], 1));
            EXPECT(CanAddRestrictedMonToParty(special[j], 0));
            CreateMon(&gParties[B_TRAINER_PLAYER][1], special[j], 14, 0, OTID_STRUCT_PLAYER_ID);
            EXPECT(!PlayerPartyWithinRestrictedLimit());
            EXPECT(!PlayerPartyLeagueEligible());
            EXPECT(!CanAddRestrictedMonToParty(special[j], 0));
            CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GARCHOMP, 14, 0, OTID_STRUCT_PLAYER_ID);
        }
    }
    ZeroPlayerPartyMons();
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
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, PARTY_SIZE), MON_GIVEN_TO_PC);
    CreateMon(&mon, SPECIES_NAGANADEL, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, 2), MON_GIVEN_TO_PC);
    CreateMon(&mon, SPECIES_FLUTTER_MANE, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PC);
    CreateMon(&mon, SPECIES_ROARING_MOON, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PC);
    EXPECT_EQ(CalculatePlayerPartyCount(), 1);
    EXPECT(PlayerPartyWithinRestrictedLimit());
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_SPECIES), SPECIES_MEWTWO);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][1], MON_DATA_SPECIES), SPECIES_KARTANA);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][2], MON_DATA_SPECIES), SPECIES_NAGANADEL);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][3], MON_DATA_SPECIES), SPECIES_FLUTTER_MANE);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][4], MON_DATA_SPECIES), SPECIES_ROARING_MOON);
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
}

TEST("Restricted party: a boxed special gift safely replaces any special category")
{
    enum Species oldSpecies, giftSpecies;
    PARAMETRIZE { oldSpecies = SPECIES_SHAYMIN; giftSpecies = SPECIES_MEWTWO; }
    PARAMETRIZE { oldSpecies = SPECIES_KARTANA; giftSpecies = SPECIES_MEWTWO; }
    PARAMETRIZE { oldSpecies = SPECIES_FLUTTER_MANE; giftSpecies = SPECIES_MEWTWO; }
    PARAMETRIZE { oldSpecies = SPECIES_SHAYMIN; giftSpecies = SPECIES_KARTANA; }
    PARAMETRIZE { oldSpecies = SPECIES_SHAYMIN; giftSpecies = SPECIES_FLUTTER_MANE; }
    struct Pokemon gift;
    u16 oldItem = ITEM_LEFTOVERS;
    u16 giftItem = ITEM_FOCUS_SASH;

    ClearBag();
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    CreateMon(&gParties[B_TRAINER_PLAYER][0], oldSpecies, 14, 111, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &oldItem);
    CreateMon(&gift, giftSpecies, 14, 222, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gift, MON_DATA_HELD_ITEM, &giftItem);

    EXPECT_EQ(GiveScriptedMonToPlayer(&gift, PARTY_SIZE), MON_GIVEN_TO_PC);
    struct BoxPokemon *box = GetBoxedMonPtr(gSpecialVar_MonBoxId, gSpecialVar_MonBoxPos);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), oldSpecies);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_SPECIES), giftSpecies);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(giftItem), 1);

    u16 mail = ITEM_ORANGE_MAIL;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &mail);
    EXPECT_EQ(GetBoxedLegendaryGiftSwapStatus(), 2);
    EXPECT(!SwapBoxedLegendaryGiftWithParty());
    EXPECT_EQ(CountTotalItemQuantityInBag(oldItem), 0);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_SPECIES), giftSpecies);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &oldItem);
    EXPECT_EQ(GetBoxedLegendaryGiftSwapStatus(), 1);
    EXPECT(SwapBoxedLegendaryGiftWithParty());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), giftSpecies);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_SPECIES), oldSpecies);
    EXPECT_EQ(GetBoxMonData(box, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(oldItem), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(giftItem), 1);
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
