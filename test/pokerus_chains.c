#include "global.h"
#include "story.h"
#include "test/test.h"
#include "pokemon.h"
#include "event_data.h"
#include "pokerus.h"
#include "party_menu.h"
#include "constants/party_menu.h"
#include "constants/battle.h"
#include "dexnav.h"
#include "random.h"
#include "data.h"
#include "text.h"
#include "constants/characters.h"
#include "constants/species.h"
#include "constants/flags.h"
#include "pokemon_storage_system.h"

TEST("Hunt preparation: boxed donors allow six isolated direct infections before the Pokedex")
{
    static EWRAM_DATA struct BoxPokemon originalBox[PARTY_SIZE];
    bool32 hadPokedex = FlagGet(FLAG_SYS_POKEDEX_GET);
    StoryStageBefore(STORY_STEP_GOT_POKEDEX);
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < PARTY_SIZE; i++)
        originalBox[i] = *GetBoxedMonPtr(0, i);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 14, 0, OTID_STRUCT_PLAYER_ID, 31);
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        EXPECT(!IsPokerusInParty());
        SET_RNG(RNG_POKERUS_INFECTION, 0);
        SET_RNG(RNG_POKERUS_PARTY_MEMBER, 0);
        RandomlyGivePartyPokerus();
        PartySpreadPokerus(); // The isolated donor has no eligible neighbour.
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_POKERUS), 0xFE);
        // Withdraw the next eligible member before depositing the donor;
        // the real PC forbids depositing the last usable party member.
        if (i + 1 < PARTY_SIZE)
        {
            CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_EEVEE, 14, i + 1, OTID_STRUCT_PLAYER_ID, 31);
            SetBoxMonAt(0, i, &gParties[B_TRAINER_PLAYER][0].box);
            gParties[B_TRAINER_PLAYER][0] = gParties[B_TRAINER_PLAYER][1];
        }
        ZeroMonData(&gParties[B_TRAINER_PLAYER][1]);
    }
    // Keep the last donor in the party and withdraw the five parked donors.
    for (u32 i = 0; i + 1 < PARTY_SIZE; i++)
        BoxMonToMon(GetBoxedMonPtr(0, i), &gParties[B_TRAINER_PLAYER][i + 1]);
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_POKERUS), 0xFE);
        EXPECT_EQ(GetPokerusSpreadsLeft(&gParties[B_TRAINER_PLAYER][i]), 2);
        SetBoxMonAt(0, i, &originalBox[i]);
    }
    EXPECT(IsPokerusInParty());
    ZeroPlayerPartyMons();
    if (hadPokedex)
        StoryStageAtLeast(STORY_STEP_GOT_POKEDEX);
}

TEST("Hunt rewards: exact independent shiny and Pokerus chain thresholds")
{
    u32 chain = 0, shinyRoll = 0, virusRoll = 0;
    bool32 shiny = FALSE, virus = FALSE;
    PARAMETRIZE { chain = 0; shinyRoll = 0; virusRoll = 2; shiny = TRUE; virus = TRUE; }
    PARAMETRIZE { chain = 0; shinyRoll = 65535; virusRoll = 3; }
    PARAMETRIZE { chain = 1; shinyRoll = 0; virusRoll = 1; shiny = TRUE; }
    PARAMETRIZE { chain = 1; shinyRoll = 1; virusRoll = 0; virus = TRUE; }
    PARAMETRIZE { chain = 50; shinyRoll = 49; virusRoll = 49; shiny = TRUE; virus = TRUE; }
    PARAMETRIZE { chain = 50; shinyRoll = 50; virusRoll = 50; }
    PARAMETRIZE { chain = 100; shinyRoll = 99; virusRoll = 99; shiny = TRUE; virus = TRUE; }
    PARAMETRIZE { chain = 100; shinyRoll = 99; virusRoll = 100; shiny = TRUE; }
    struct Pokemon mon;
    u32 oldChain = gSaveBlock3Ptr->dexNavChain;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    gSaveBlock3Ptr->dexNavChain = chain;
    SET_RNG(RNG_DEXNAV_SHINY, shinyRoll);
    SET_RNG(RNG_DEXNAV_POKERUS, virusRoll);
    ApplyDexNavChainRewards(&mon);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IS_SHINY), shiny);
    EXPECT_EQ(CheckMonHasHadPokerus(&mon), virus);
    EXPECT_EQ(GetPokerusSpreadsLeft(&mon), virus ? 2 : 0);
    gSaveBlock3Ptr->dexNavChain = oldChain;
}

TEST("Hunt rewards: Pokerus follows nature changes, recovery and boxed saves")
{
    struct Pokemon mon, restored;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    for (u32 stat = STAT_HP; stat < NUM_STATS; stat++)
    {
        u32 ev = 0;
        SetMonData(&mon, MON_DATA_HP_EV + stat, &ev);
    }
    GiveMonPokerus(&mon, TRUE);
    static const u8 natures[] = {NATURE_ADAMANT, NATURE_MODEST, NATURE_JOLLY, NATURE_HARDY};
    for (u32 recovered = 0; recovered < 2; recovered++)
    {
        if (recovered)
        {
            u32 status = 0xFC;
            SetMonData(&mon, MON_DATA_POKERUS, &status);
        }
        for (u32 i = 0; i < ARRAY_COUNT(natures); i++)
        {
            u32 nature = natures[i];
            SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
            CalculateMonStats(&mon);
            BoxMonToMon(&mon.box, &restored);
            EXPECT_EQ(CheckMonHasHadPokerus(&restored), TRUE);
            EXPECT_EQ(GetPokerusSpreadsLeft(&restored), recovered ? 0 : 2);
            for (u32 stat = STAT_ATK; stat < NUM_STATS; stat++)
            {
                u32 base = CalculateSpeciesStatForOwner(SPECIES_EEVEE, NATURE_HARDY, stat, 50, 0, 31, FALSE);
                u32 expected = base;
                if (gNaturesInfo[nature].statUp != gNaturesInfo[nature].statDown)
                {
                    if (stat == gNaturesInfo[nature].statUp) expected = base * 115 / 100;
                    if (stat == gNaturesInfo[nature].statDown) expected = base * 90 / 100;
                }
                EXPECT_EQ(IsPokerusNatureBoosted(&mon, stat),
                    gNaturesInfo[nature].statUp != gNaturesInfo[nature].statDown && stat == gNaturesInfo[nature].statUp);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP + stat), expected);
                EXPECT_EQ(GetMonData(&restored, MON_DATA_MAX_HP + stat), expected);
                EXPECT_EQ(GetMonData(&restored, MON_DATA_HP_EV + stat), 0);
            }
        }
    }
}

TEST("Hunt rewards: only two adjacent recipients and no secondary spread")
{
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < PARTY_SIZE; i++)
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    struct Pokemon *source = &gParties[B_TRAINER_PLAYER][2];
    GiveMonPokerus(source, TRUE);
    SET_RNG(RNG_POKERUS_SPREAD, 0);
    SET_RNG(RNG_POKERUS_SPREAD_SIDE, 0);
    PartySpreadPokerus();
    EXPECT_EQ(GetPokerusSpreadsLeft(source), 1);
    EXPECT(CheckMonHasHadPokerus(&gParties[B_TRAINER_PLAYER][1]));
    EXPECT_EQ(GetPokerusSpreadsLeft(&gParties[B_TRAINER_PLAYER][1]), 0);
    EXPECT(!CheckMonHasHadPokerus(&gParties[B_TRAINER_PLAYER][3]));
    PartySpreadPokerus();
    EXPECT_EQ(GetPokerusSpreadsLeft(source), 0);
    EXPECT(CheckMonHasHadPokerus(&gParties[B_TRAINER_PLAYER][3]));
    GiveMonPokerus(source, TRUE); // Re-infection must not replenish its budget.
    for (u32 i = 0; i < 6; i++) PartySpreadPokerus();
    EXPECT_EQ(GetPokerusSpreadsLeft(source), 0);
    EXPECT(!CheckMonHasHadPokerus(&gParties[B_TRAINER_PLAYER][0]));
    EXPECT(!CheckMonHasHadPokerus(&gParties[B_TRAINER_PLAYER][4]));
    EXPECT(!CheckMonHasHadPokerus(&gParties[B_TRAINER_PLAYER][5]));
    ZeroPlayerPartyMons();
}

TEST("Hunt rewards: failed spread and ineligible neighbors do not spend a transmission")
{
    ZeroPlayerPartyMons();
    struct Pokemon *source = &gParties[B_TRAINER_PLAYER][0];
    CreateMonWithIVs(source, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    GiveMonPokerus(source, TRUE);
    SET_RNG(RNG_POKERUS_SPREAD, 0);
    PartySpreadPokerus();
    EXPECT_EQ(GetPokerusSpreadsLeft(source), 2);
    struct Pokemon *target = &gParties[B_TRAINER_PLAYER][1];
    CreateMonWithIVs(target, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    u32 egg = TRUE;
    SetMonData(target, MON_DATA_IS_EGG, &egg);
    PartySpreadPokerus();
    EXPECT_EQ(GetPokerusSpreadsLeft(source), 2);
    EXPECT(!CheckMonHasHadPokerus(target));
    egg = FALSE;
    SetMonData(target, MON_DATA_IS_EGG, &egg);
    SET_RNG(RNG_POKERUS_SPREAD, 65535);
    PartySpreadPokerus();
    EXPECT_EQ(GetPokerusSpreadsLeft(source), 2);
    EXPECT(!CheckMonHasHadPokerus(target));
    ZeroPlayerPartyMons();
}

TEST("Hunt rewards: legacy Pokerus retains its benefit without acquiring a spread budget")
{
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    u32 oldStatus = 0xF4;
    SetMonData(&mon, MON_DATA_POKERUS, &oldStatus);
    EXPECT(CheckMonHasHadPokerus(&mon));
    EXPECT_EQ(GetPokerusSpreadsLeft(&mon), 0);
    EXPECT(!ShouldPokemonShowActivePokerus(&mon));
    EXPECT(ShouldPokemonShowCuredPokerus(&mon));
    GiveMonPokerus(&mon, TRUE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_POKERUS), oldStatus);
    EXPECT_EQ(GetPokerusSpreadsLeft(&mon), 0);
}

TEST("Hot spring Pokerus: any infected partner can prepare without changing stats or transmission budget")
{
    static const u8 statuses[PARTY_SIZE] = {0xFE, 0xFD, 0xFC, 0xF4, 0, 0xFE};
    struct Pokemon unchanged[PARTY_SIZE];
    ZeroPlayerPartyMons();
    EXPECT(!PreparePartyPokerusInHotSpring());
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        CreateMonWithIVs(mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
        u32 value = statuses[i];
        SetMonData(mon, MON_DATA_POKERUS, &value);
        value = i == PARTY_SIZE - 1;
        SetMonData(mon, MON_DATA_IS_EGG, &value);
        CalculateMonStats(mon);
        unchanged[i] = *mon;
    }
    EXPECT(PreparePartyPokerusInHotSpring());
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        EXPECT_EQ(IsMonReadyForHotSpringTreatment(mon), i < 4);
        EXPECT(!HasHotSpringPokerus(mon));
        EXPECT_EQ(GetMonData(mon, MON_DATA_ATK), GetMonData(&unchanged[i], MON_DATA_ATK));
        EXPECT_EQ(GetPokerusSpreadsLeft(mon), GetPokerusSpreadsLeft(&unchanged[i]));
        EXPECT_EQ(RecoverMonPokerusInHotSpring(mon), i < 4);
        EXPECT_EQ(HasHotSpringPokerus(mon), i < 4);
        EXPECT_EQ(GetPokerusSpreadsLeft(mon), i < 4 ? 0 : GetPokerusSpreadsLeft(&unchanged[i]));
        if (i >= 4)
            EXPECT_EQ(memcmp(mon, &unchanged[i], sizeof(*mon)), 0);
    }
    EXPECT(!PreparePartyPokerusInHotSpring());
    ZeroPlayerPartyMons();
}

TEST("Hot spring Pokerus: preview is read-only and treatment requires a prepared selected partner")
{
    ZeroPlayerPartyMons();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMonWithIVs(mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    CalculatePlayerPartyCount();
    GiveMonPokerus(mon, FALSE);
    gSpecialVar_0x8004 = 0;
    EXPECT(!RecoverMonPokerusInHotSpring(mon));
    EXPECT(PreparePartyPokerusInHotSpring());
    struct Pokemon before = *mon;
    BufferHotSpringPokerusPreview();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(memcmp(mon, &before, sizeof(*mon)), 0);
    ApplyHotSpringPokerusTreatment();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(HasHotSpringPokerus(mon));
    EXPECT(!RecoverMonPokerusInHotSpring(mon));
    ZeroPlayerPartyMons();
}

TEST("Hot spring Pokerus: nature stats gain fifteen and five percent across all natures, nature changes and boxed saves")
{
    ZeroPlayerPartyMons();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    struct Pokemon restored;
    CreateMonWithIVs(mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    for (u32 stat = STAT_HP; stat < NUM_STATS; stat++)
    {
        u32 ev = 0;
        SetMonData(mon, MON_DATA_HP_EV + stat, &ev);
    }
    u32 nature = NATURE_LONELY;
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    GiveMonPokerus(mon, TRUE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_ATK), 86);
    EXPECT_EQ(GetMonData(mon, MON_DATA_DEF), 63);
    EXPECT(PreparePartyPokerusInHotSpring());
    EXPECT(RecoverMonPokerusInHotSpring(mon));
    // Recovery recalculates immediately; it replaces the penalty from the
    // neutral baseline rather than multiplying the already-reduced stat.
    EXPECT_EQ(GetMonData(mon, MON_DATA_ATK), 86);
    EXPECT_EQ(GetMonData(mon, MON_DATA_DEF), 73);
    for (nature = 0; nature < NUM_NATURES; nature++)
    {
        SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
        CalculateMonStats(mon);
        struct BoxPokemon saved = mon->box;
        BoxMonToMon(&saved, &restored);
        EXPECT_EQ(GetBoxMonData(&saved, MON_DATA_POKERUS), 0xFB);
        EXPECT(HasHotSpringPokerus(&restored));
        EXPECT(!CheckMonPokerus(&restored));
        u32 boosts = 0;
        for (u32 stat = STAT_HP; stat < NUM_STATS; stat++)
        {
            bool32 boosted = stat != STAT_HP
                && gNaturesInfo[nature].statUp != gNaturesInfo[nature].statDown
                && (stat == gNaturesInfo[nature].statUp || stat == gNaturesInfo[nature].statDown);
            u32 base = CalculateSpeciesStatForOwner(SPECIES_EEVEE, NATURE_HARDY, stat, 50, 0, 31, FALSE);
            u32 expected = !boosted ? base : base * (stat == gNaturesInfo[nature].statUp ? 115 : 105) / 100;
            EXPECT_EQ(IsPokerusNatureBoosted(mon, stat), boosted);
            EXPECT_EQ(IsPokerusNatureBoosted(&restored, stat), boosted);
            EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP + stat), expected);
            EXPECT_EQ(GetMonData(&restored, MON_DATA_MAX_HP + stat), expected);
            EXPECT_EQ(GetMonData(&restored, MON_DATA_HP_EV + stat), 0);
            boosts += boosted;
        }
        EXPECT_EQ(boosts, nature % 6 == 0 ? 0 : 2);
    }
    ZeroPlayerPartyMons();
}

TEST("Hot spring Pokerus: trainer-owned Pokemon never gain the upgrade or enhanced nature stats")
{
    ZeroPlayerPartyMons();
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMonWithIVs(mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    u32 nature = NATURE_LONELY;
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    GiveMonPokerus(mon, TRUE);
    SetMonTrainerOwned(mon, TRUE);
    EXPECT(!PreparePartyPokerusInHotSpring());
    EXPECT_EQ(GetMonData(mon, MON_DATA_POKERUS), 0xFE);
    // A stored marker must also confer no stat benefit on a trainer opponent.
    u32 status = 0xFB;
    SetMonData(mon, MON_DATA_POKERUS, &status);
    CalculateMonStats(mon);
    for (u32 stat = STAT_ATK; stat < NUM_STATS; stat++)
    {
        EXPECT(!IsPokerusNatureBoosted(mon, stat));
        u32 expected = CalculateSpeciesStatForOwner(SPECIES_EEVEE, nature, stat, 50,
                            GetMonData(mon, MON_DATA_HP_EV + stat), 31, TRUE);
        EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP + stat), expected);
    }
    ZeroPlayerPartyMons();
}

TEST("Hunt rewards: Pokerus indicators preserve battle-status priority")
{
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 50, 0, OTID_STRUCT_PLAYER_ID, 31);
    GiveMonPokerus(&mon, TRUE);
    EXPECT_EQ(GetMonAilment(&mon), AILMENT_PKRS);
    u32 status = STATUS1_POISON;
    SetMonData(&mon, MON_DATA_STATUS, &status);
    EXPECT_EQ(GetMonAilment(&mon), AILMENT_PSN);
    EXPECT(ShouldPokemonShowActivePokerus(&mon));
    u32 hp = 0;
    SetMonData(&mon, MON_DATA_HP, &hp);
    EXPECT_EQ(GetMonAilment(&mon), AILMENT_FNT);
    status = 0;
    hp = GetMonData(&mon, MON_DATA_MAX_HP);
    SetMonData(&mon, MON_DATA_STATUS, &status);
    SetMonData(&mon, MON_DATA_HP, &hp);
    status = 0xFC;
    SetMonData(&mon, MON_DATA_POKERUS, &status);
    EXPECT_EQ(GetMonAilment(&mon), AILMENT_NONE);
    EXPECT(ShouldPokemonShowCuredPokerus(&mon));
}

extern u32 Test_SummaryAbilityDescriptionFont(const u8 *text);
TEST("Hunt rewards: every summary ability fits above the Pokerus memo")
{
    for (u32 ability = 0; ability < ABILITIES_COUNT; ability++)
    {
        const u8 *text = gAbilitiesInfo[ability].description;
        u32 font = Test_SummaryAbilityDescriptionFont(text), lines = 1;
        for (const u8 *p = text; *p != EOS; p++)
            if (*p == CHAR_NEWLINE) lines++;
        EXPECT_LE(GetStringWidth(font, text, 0), 144);
        EXPECT_LE(lines * GetFontAttribute(font, FONTATTR_MAX_LETTER_HEIGHT), 16);
        font = GetFontIdToFit(gAbilitiesInfo[ability].name, FONT_NORMAL, 0, 144);
        EXPECT_LE(GetStringWidth(font, gAbilitiesInfo[ability].name, 0), 144);
    }
    const u8 *twoLines = COMPOUND_STRING("First line of an Ability.\nSecond line stays above memo.");
    u32 font = Test_SummaryAbilityDescriptionFont(twoLines);
    EXPECT_LE(GetStringWidth(font, twoLines, 0), 144);
    EXPECT_LE(2 * GetFontAttribute(font, FONTATTR_MAX_LETTER_HEIGHT), 16);
}
