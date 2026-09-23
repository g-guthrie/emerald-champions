#include "global.h"
#include "battle.h"
#include "script.h"
#include "caps.h"
#include "constants/emerald_champions.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "event_data.h"
#include "field_specials.h"
#include "constants/field_specials.h"
#include "item.h"
#include "legendary_signs.h"
#include "pokemon.h"
#include "overworld.h"
#include "pokedex.h"
#include "constants/flags.h"
#include "save.h"
#include "test/test.h"
#include "constants/vars.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/moves.h"
#include "random.h"
#include "string_util.h"
#include "text.h"
#include "constants/characters.h"

static const u16 sUnlockedVars[] = {
    VAR_LEGENDARY_SIGNS_UNLOCKED_0, VAR_LEGENDARY_SIGNS_UNLOCKED_1,
    VAR_LEGENDARY_SIGNS_UNLOCKED_2, VAR_LEGENDARY_SIGNS_UNLOCKED_3,
    VAR_LEGENDARY_SIGNS_UNLOCKED_4, VAR_LEGENDARY_SIGNS_UNLOCKED_5,
};
static const u16 sCaughtVars[] = {
    VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static void ResetSignState(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        VarSet(sUnlockedVars[i], 0);
        VarSet(sCaughtVars[i], 0);
    }
    ZeroPlayerPartyMons();
    ClearBag();
}

TEST("Mauville Genesect prize needs eight badges and records its one-time acquisition")
{
    ResetSignState();
    FlagClear(FLAG_RECEIVED_GAME_CORNER_GENESECT);
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gSpecialVar_0x8004 = SPECIES_GENESECT;

    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_ALREADY_CAUGHT);
    EXPECT(!FlagGet(FLAG_RECEIVED_GAME_CORNER_GENESECT));

    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    EXPECT(FlagGet(FLAG_RECEIVED_GAME_CORNER_GENESECT));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_GENESECT));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_GENESECT);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), GetCurrentLevelCap());

    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_SET_FAILED);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
}

TEST("Sign state survives a native flash save and load across all six storage groups")
{
    u16 expectedUnlocked[ARRAY_COUNT(sUnlockedVars)];
    u16 expectedCaught[ARRAY_COUNT(sCaughtVars)];

    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        // Retain both active and caught entries in each populated group.
        UnlockLegendarySign(id);
        if (id % 2 == 0)
            MarkLegendarySignCaughtBySpecies(gLegendaryGates[id].species);
    }
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        expectedUnlocked[i] = VarGet(sUnlockedVars[i]);
        expectedCaught[i] = VarGet(sCaughtVars[i]);
    }
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        VarSet(sUnlockedVars[i], 0);
        VarSet(sCaughtVars[i], 0);
    }
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_COBALION));
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        EXPECT_EQ(VarGet(sUnlockedVars[i]), expectedUnlocked[i]);
        EXPECT_EQ(VarGet(sCaughtVars[i]), expectedCaught[i]);
    }
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        EXPECT(IsLegendarySignUnlocked(id));
        EXPECT_EQ(IsLegendarySignCaught(id), id % 2 == 0);
    }
}

// These exercise production selection and quest state. They do not simulate
// walking, script choreography, capture animations, or campaign traversal.
TEST("Sweet Scent reverses species totals with duplicates and ties while preserving legendary slots")
{
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS] = {
        {5, 5, SPECIES_MAGIKARP}, {5, 5, SPECIES_MAGIKARP},
        {5, 5, SPECIES_GOLDEEN}, {5, 5, SPECIES_TENTACOOL}, {5, 5, SPECIES_WINGULL},
    };
    const enum Species species[] = {SPECIES_MAGIKARP, SPECIES_GOLDEEN, SPECIES_TENTACOOL, SPECIES_WINGULL};
    u32 counts[ARRAY_COUNT(species)] = {0};
    ResetSignState();
    // Water slots are 40/30/20/10. Combining the first two yields
    // 70/20/10; reversal gives 10/20/70. Duplicate slots remain one species.
    for (u32 choice = 1; choice <= 2; choice++)
    {
        SET_RNG(RNG_WILD_MON_TARGET, choice);
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            u32 index = ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_WATER);
            EXPECT_LT(index, NUM_WATER_MONS_ENCOUNTER_SLOTS);
            for (u32 i = 0; i < ARRAY_COUNT(species); i++)
                counts[i] += mons[index].species == species[i];
        }
    }
    EXPECT_EQ(counts[0], 20);
    EXPECT_EQ(counts[1], 40);
    EXPECT_EQ(counts[2], 140);
    EXPECT_EQ(counts[3], 0);

    // Land totals 28/28/44 become 36/36/28: identical rarity gets equal odds.
    // After sorting, tied species occupy indices 1 and 2.
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = i < 4 ? species[i % 2] : species[2];
    memset(counts, 0, sizeof(counts));
    for (u32 choice = 1; choice <= 2; choice++)
    {
        SET_RNG(RNG_WILD_MON_TARGET, choice);
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            u32 index = ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_LAND);
            EXPECT_LT(index, NUM_LAND_MONS_ENCOUNTER_SLOTS);
            for (u32 i = 0; i < 3; i++)
                counts[i] += mons[index].species == species[i];
        }
    }
    EXPECT_EQ(counts[0], 72);
    EXPECT_EQ(counts[1], 72);
    EXPECT_EQ(counts[2], 56);

    // Legend slots and their five-fold Sweet Scent boost are covered in
    // test/wild_slot_odds.c.
}

static bool32 BufferContains(const u8 *haystack, const u8 *needle)
{
    u32 length = StringLength(haystack);
    u32 needleLength = StringLength(needle);
    for (u32 i = 0; i + needleLength <= length; i++)
        if (StringCompareN(haystack + i, needle, needleLength) == 0)
            return TRUE;
    return FALSE;
}

static void ExpectLinesFitSignWindow(void)
{
    u8 line[sizeof(gStringVar4)];
    u32 lineLength = 0;
    for (u32 i = 0; ; i++)
    {
        u8 c = gStringVar4[i];
        if (c == EOS || c == CHAR_NEWLINE || c == CHAR_PROMPT_SCROLL || c == CHAR_PROMPT_CLEAR)
        {
            line[lineLength] = EOS;
            EXPECT_LE(GetStringWidth(FONT_NORMAL, line, 0), 200);
            lineLength = 0;
            if (c == EOS)
                break;
        }
        else
            line[lineLength++] = c;
    }
}

TEST("Route rosters list legend slots with caught marks and the Sweet Scent hint")
{
    struct WarpData oldLocation = gSaveBlock1Ptr->location;
    u16 oldCave = VarGet(VAR_ALTERING_CAVE_WILD_SET);
    u32 checked = 0;
    ResetSignState();
    VarSet(VAR_ALTERING_CAVE_WILD_SET, 0);
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[TIME_OF_DAY_DEFAULT];
        enum Species legend = SPECIES_NONE;
        for (u32 slot = 0; types->landMonsInfo != NULL && slot < NUM_LAND_MONS_ENCOUNTER_SLOTS && legend == SPECIES_NONE; slot++)
            if (IsLegendaryEncounterSpecies(types->landMonsInfo->wildPokemon[slot].species))
                legend = types->landMonsInfo->wildPokemon[slot].species;
        for (u32 slot = 0; types->waterMonsInfo != NULL && slot < NUM_WATER_MONS_ENCOUNTER_SLOTS && legend == SPECIES_NONE; slot++)
            if (IsLegendaryEncounterSpecies(types->waterMonsInfo->wildPokemon[slot].species))
                legend = types->waterMonsInfo->wildPokemon[slot].species;
        if (legend == SPECIES_NONE || GetLegendarySignIdBySpecies(legend) >= LEGENDARY_SIGN_COUNT)
            continue;
        gSaveBlock1Ptr->location.mapGroup = gWildMonHeaders[header].mapGroup;
        gSaveBlock1Ptr->location.mapNum = gWildMonHeaders[header].mapNum;
        if (GetCurrentMapWildMonHeaderId() != header)
            continue; // Altering Cave variants share one map.
        MarkLegendarySignCaughtBySpecies(legend);
        BufferCurrentMapRouteSignSpecies();
        u8 name[64];
        StringCopy(name, GetLegendaryDisplayName(legend));
        StringAppend(name, COMPOUND_STRING(" (Caught)"));
        EXPECT_LT(StringLength(gStringVar4), sizeof(gStringVar4));
        EXPECT(BufferContains(gStringVar4, name));
        EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Legends and Ultra Beasts are rare")));
        EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("%")));
        ExpectLinesFitSignWindow();
        checked++;
    }
    Test_MgbaPrintf("Route rosters with legend slots: %d", checked);
    EXPECT_GT(checked, 0);
    // Every gate row reports its capture on research.
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        MarkLegendarySignCaughtBySpecies(gLegendaryGates[id].species);
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 4);
    }
    gSaveBlock1Ptr->location = oldLocation;
    VarSet(VAR_ALTERING_CAVE_WILD_SET, oldCave);
    ResetSignState();
}

TEST("Research reports badges, milestone, family and availability from the gate row")
{
    ResetSignState();
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    FlagClear(FLAG_RECEIVED_RED_OR_BLUE_ORB);

    // Zeraora: five badges, then Wattson's New Mauville receipt.
    gSpecialVar_0x8004 = LEGENDARY_SIGN_ZERAORA;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Gym Badges required:\n5.")));
    for (u32 badge = 0; badge < 5; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("Gym Badges required")));
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Help Wattson")));
    FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT(BufferContains(gStringVar4, COMPOUND_STRING("Available now!")));

    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_RECEIVED_RED_OR_BLUE_ORB);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_CRESSELIA;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT(BufferContains(gStringVar4, GetSpeciesName(SPECIES_DARKRAI)));
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_DARKRAI), FLAG_SET_CAUGHT);
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT(!BufferContains(gStringVar4, COMPOUND_STRING("%")));
    ExpectLinesFitSignWindow();

    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    FlagClear(FLAG_RECEIVED_RED_OR_BLUE_ORB);
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    ResetSignState();
}

TEST("Rare wild NPC discovery requires local help and survives depositing the partner")
{
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_DEWFORD_MEADOW);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_DEWFORD_MEADOW);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_MELOETTA;
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_JIGGLYPUFF, 10, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 move = 0; move < MAX_MON_MOVES; move++)
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_NONE, move);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SING, 0);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_POUND, 0);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SING, 0);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    ZeroPlayerPartyMons();
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MELOETTA));
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);

    for (u32 badge = 0; badge < 5; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_LANDORUS;
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_CASTFORM, 25, 0, OTID_STRUCT_PLAYER_ID);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_LANDORUS));
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE111_RUINS_EXTERIOR);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE111_RUINS_EXTERIOR);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_LANDORUS));
    ZeroPlayerPartyMons();
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_LANDORUS));
}

static bool32 GuideTextContains(const u8 *needle)
{
    u32 length = StringLength(gStringVar4);
    u32 needleLength = StringLength(needle);
    for (u32 i = 0; i + needleLength <= length; i++)
        if (StringCompareN(gStringVar4 + i, needle, needleLength) == 0)
            return TRUE;
    return FALSE;
}

TEST("Center local guide reflects quest progress without unlocking discoveries")
{
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gMapHeader.regionMapSectionId = MAPSEC_DEWFORD_TOWN;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("SING")));
    EXPECT(GuideTextContains(COMPOUND_STRING("Gym Badges required:\n2.")));
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    UnlockLegendarySign(LEGENDARY_SIGN_MELOETTA);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've met its requirements!")));
    MarkLegendarySignCaughtBySpecies(SPECIES_MELOETTA);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("SING")));
    gMapHeader.regionMapSectionId = MAPSEC_OLDALE_TOWN;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("SHAYMIN")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("%")));
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("find more")));
    // Regional Moltres shares the national Dex entry, but not the Ember Path scene.
    FlagClear(FLAG_EC_CAUGHT_MOLTRES);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MOLTRES_GALAR), FLAG_SET_CAUGHT);
    gMapHeader.regionMapSectionId = MAPSEC_LAVARIDGE_TOWN;
    gSpecialVar_0x8004 = 0;
    do { BufferNextCenterLegendaryLead(); } while (gSpecialVar_Result && !GuideTextContains(COMPOUND_STRING("MOLTRES")));
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("EMBER PATH")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("You've already found")));
    FlagSet(FLAG_EC_CAUGHT_MOLTRES);
    gSpecialVar_0x8004--; // Revisit the same lead after the original encounter is caught.
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    const u16 cities[] = {MAPSEC_OLDALE_TOWN, MAPSEC_PETALBURG_CITY, MAPSEC_DEWFORD_TOWN,
        MAPSEC_RUSTBORO_CITY, MAPSEC_SLATEPORT_CITY, MAPSEC_MAUVILLE_CITY,
        MAPSEC_VERDANTURF_TOWN, MAPSEC_LAVARIDGE_TOWN, MAPSEC_FALLARBOR_TOWN,
        MAPSEC_FORTREE_CITY, MAPSEC_LILYCOVE_CITY, MAPSEC_MOSSDEEP_CITY,
        MAPSEC_SOOTOPOLIS_CITY, MAPSEC_PACIFIDLOG_TOWN, MAPSEC_EVER_GRANDE_CITY};
    for (u32 city = 0; city < ARRAY_COUNT(cities); city++)
    {
        u32 pages = 0;
        gMapHeader.regionMapSectionId = cities[city];
        gSpecialVar_0x8004 = 0;
        while (TRUE)
        {
            BufferNextCenterLegendaryLead();
            if (!gSpecialVar_Result)
                break;
            EXPECT_LT(++pages, 20);
            EXPECT_LT(StringLength(gStringVar4), sizeof(gStringVar4));
            u8 line[sizeof(gStringVar4)];
            u32 length = 0;
            for (u32 i = 0; ; i++)
            {
                u8 c = gStringVar4[i];
                if (c == EOS || c == CHAR_NEWLINE || c == CHAR_PROMPT_SCROLL || c == CHAR_PROMPT_CLEAR)
                {
                    line[length] = EOS;
                    EXPECT_LE(GetStringWidth(FONT_NORMAL, line, 0), 200);
                    length = 0;
                    if (c == EOS)
                        break;
                }
                else
                    line[length++] = c;
            }
        }
        EXPECT_GT(pages, 0);
    }
}

TEST("Regigigas wakes for the three Regis' Pokedex records, not a three-Legendary party")
{
    // Route roster signs must resolve in Emerald, not only in FireRed/LeafGreen.
    const struct ObjectEventGraphicsInfo *sign = GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_SIGN);
    EXPECT(sign != NULL);
    EXPECT(sign->images != NULL);
    ResetSignState();
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    const enum Species regis[] = {SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL};
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    for (u32 i = 0; i < ARRAY_COUNT(regis); i++)
    {
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(regis[i]), FLAG_SET_CAUGHT);
        EXPECT_EQ(CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS), i + 1 == ARRAY_COUNT(regis));
    }
    // The party holds none of them: the restricted-party rule allows only one.
    EXPECT(!PlayerPartyHasSpeciesFamily(SPECIES_REGIROCK));
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    // An unlocked bit from an older save does not bypass the records.
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    UnlockLegendarySign(LEGENDARY_SIGN_REGIGIGAS);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    ResetSignState();
}

extern const u8 SealedChamber_InnerRoom_EventScript_CheckRegigigasResult[];
extern const u8 Common_EventScript_VisibleLegendaryCaught[];
extern const u8 ShoalCave_LowTideIceRoom_EventScript_CheckArticunoResult[];
extern const u8 NewMauville_Inside_EventScript_CheckZapdosResult[];
extern const u8 AlteringCave_B1F_EventScript_CheckMewtwoResult[];
extern const u8 Common_EventScript_LegendaryResting[];
extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];

extern const u8 SealedChamber_InnerRoom_EventScript_Regigigas[];
extern const u8 ShoalCave_LowTideIceRoom_EventScript_Articuno[];
extern const u8 NewMauville_Inside_EventScript_Zapdos[];
extern const u8 AlteringCave_B1F_EventScript_Mewtwo[];

TEST("Visible legendary events: uncaught outcomes retreat and capture uses the map hide flag")
{
    const struct {u16 map; enum Species species; const u8 *gate, *entry;} cases[] = {
        {MAP_SEALED_CHAMBER_INNER_ROOM, SPECIES_REGIGIGAS, SealedChamber_InnerRoom_EventScript_CheckRegigigasResult, SealedChamber_InnerRoom_EventScript_Regigigas},
        {MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM, SPECIES_ARTICUNO, ShoalCave_LowTideIceRoom_EventScript_CheckArticunoResult, ShoalCave_LowTideIceRoom_EventScript_Articuno},
        {MAP_NEW_MAUVILLE_INSIDE, SPECIES_ZAPDOS, NewMauville_Inside_EventScript_CheckZapdosResult, NewMauville_Inside_EventScript_Zapdos},
        {MAP_ALTERING_CAVE_B1F, SPECIES_MEWTWO, AlteringCave_B1F_EventScript_CheckMewtwoResult, AlteringCave_B1F_EventScript_Mewtwo},
    };
    const u8 outcomes[] = {B_OUTCOME_WON, B_OUTCOME_RAN, B_OUTCOME_PLAYER_TELEPORTED, B_OUTCOME_CAUGHT};
    for (u32 encounter = 0; encounter < ARRAY_COUNT(cases); encounter++)
    {
        for (u32 i = 0; i < ARRAY_COUNT(outcomes); i++)
        {
            struct ScriptContext ctx;
            InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
            SetupBytecodeScript(&ctx, cases[encounter].gate);
            gBattleOutcome = outcomes[i];
            FlagSet(FLAG_SYS_CTRL_OBJ_DELETE);
            for (u32 step = 0; step < 6
              && ctx.scriptPtr != Common_EventScript_LegendaryResting
              && ctx.scriptPtr != Common_EventScript_VisibleLegendaryCaught; step++)
            {
                u8 command = *ctx.scriptPtr++;
                EXPECT(!ctx.cmdTable[command](&ctx));
            }
            EXPECT_EQ(ctx.scriptPtr, outcomes[i] == B_OUTCOME_CAUGHT
                ? Common_EventScript_VisibleLegendaryCaught
                : Common_EventScript_LegendaryResting);
            EXPECT(!FlagGet(FLAG_SYS_CTRL_OBJ_DELETE));
        }
        ResetSignState();
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(
            MAP_GROUP(cases[encounter].map), MAP_NUM(cases[encounter].map));
        // Resolve the real encounter object through its script, independent of artwork.
        const struct ObjectEventTemplate *object = NULL;
        for (u32 i = 0; i < map->events->objectEventCount; i++)
            if (map->events->objectEvents[i].script == cases[encounter].entry)
                object = &map->events->objectEvents[i];
        EXPECT(object != NULL);
        FlagClear(object->flagId);
        MarkLegendarySignCaughtBySpecies(cases[encounter].species);
        EXPECT(FlagGet(object->flagId));
        EXPECT(IsLegendarySignCaught(GetLegendarySignIdBySpecies(cases[encounter].species)));
    }
}

extern void Test_CollectAsh(void);

TEST("Soot collection: spendable ash and lifetime discovery progress remain independent")
{
    ResetSignState();
    VarSet(VAR_ASH_GATHER_COUNT, 0);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED);
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 0);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED);
    EXPECT(AddBagItem(ITEM_SOOT_SACK, 1));
    for (u32 step = 0; step < EC_SOOT_MARSHADOW_TARGET; step++)
        Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 250);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 250);
    // Buying a Blue Flute spends the old balance, never the lifetime total.
    VarSet(VAR_ASH_GATHER_COUNT, VarGet(VAR_ASH_GATHER_COUNT) - 250);
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 1);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 251);
    VarSet(VAR_ASH_GATHER_COUNT, 9998);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 9998);
    Test_CollectAsh();
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 9999);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 9999);
    VarSet(VAR_ASH_GATHER_COUNT, 0);
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 1);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 9999);
}

TEST("Marshadow discovery: glassmaker requires lifetime soot and unlocks once without spending it")
{
    ResetSignState();
    for (u32 badge = 0; badge < 3; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    VarSet(VAR_ASH_GATHER_COUNT, 0);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 250);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_MARSHADOW;
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113_GLASS_WORKSHOP);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113_GLASS_WORKSHOP);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 249);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 250);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_MARSHADOW));
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 0);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 250);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    MarkLegendarySignCaughtBySpecies(SPECIES_MARSHADOW);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    for (u32 badge = 0; badge < 3; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
}

extern bool32 Test_PokedexAreaHasSection(enum Species species, u16 section);
extern u8 gAreaTimeOfDay;

TEST("Rare habitat: the Pokedex reflects unlocked and uncaught wild residents")
{
    ResetSignState();
    u16 oldSection = gMapHeader.regionMapSectionId;
    u8 oldTime = gAreaTimeOfDay;
    struct WarpData oldLocation = gSaveBlock1Ptr->location;
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_113;
    gAreaTimeOfDay = TIME_OF_DAY_DEFAULT;
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113);
    for (u32 badge = 0; badge < 3; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    UnlockLegendarySign(LEGENDARY_SIGN_MARSHADOW);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    gMapHeader.regionMapSectionId = MAPSEC_PALLET_TOWN;
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_113;
    MarkLegendarySignCaughtBySpecies(SPECIES_MARSHADOW);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    for (u32 badge = 0; badge < 3; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gMapHeader.regionMapSectionId = oldSection;
    gAreaTimeOfDay = oldTime;
    gSaveBlock1Ptr->location = oldLocation;
    ResetSignState();
}

TEST("Rare habitat: native meadow residents disappear independently after capture")
{
    ResetSignState();
    u16 oldSection = gMapHeader.regionMapSectionId;
    u8 oldTime = gAreaTimeOfDay;
    gMapHeader.regionMapSectionId = MAPSEC_VERDANTURF_MEADOW;
    gAreaTimeOfDay = TIME_OF_DAY_DEFAULT;
    // Gated residents stay off the map until their milestones.
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    for (u32 badge = 0; badge < 6; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
    EXPECT(Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    MarkLegendarySignCaughtBySpecies(SPECIES_ENAMORUS);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
    gMapHeader.regionMapSectionId = oldSection;
    gAreaTimeOfDay = oldTime;
    ResetSignState();
}

static bool32 IsEmeraldWildHeader(u32 header)
{
    const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(gWildMonHeaders[header].mapGroup, gWildMonHeaders[header].mapNum);
    return map->mapLayout == NULL || !map->mapLayout->isFrlg;
}

static bool32 WildTableHasSpecies(const struct WildPokemonInfo *info, u32 slots, enum Species species)
{
    for (u32 slot = 0; info != NULL && slot < slots; slot++)
        if (info->wildPokemon[slot].species == species)
            return TRUE;
    return FALSE;
}

TEST("Gate table: every wild or quest row has a compiled slot and every wild legend has a row")
{
    u32 missing = 0;
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendaryGate *gate = &gLegendaryGates[id];
        EXPECT(IsLegendaryEncounterSpecies(gate->species));
        EXPECT_EQ(GetLegendarySignIdBySpecies(gate->species), id);
        if (gate->kind != LEGENDARY_KIND_WILD && gate->kind != LEGENDARY_KIND_QUEST)
            continue;
        bool32 found = FALSE;
        for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED) && !found; header++)
        {
            const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[TIME_OF_DAY_DEFAULT];
            found = IsEmeraldWildHeader(header)
                && (WildTableHasSpecies(types->landMonsInfo, NUM_LAND_MONS_ENCOUNTER_SLOTS, gate->species)
                 || WildTableHasSpecies(types->waterMonsInfo, NUM_WATER_MONS_ENCOUNTER_SLOTS, gate->species));
        }
        if (!found)
        {
            Test_MgbaPrintf("Wild gate row without a compiled slot: sign %d species %d", id, gate->species);
            missing++;
        }
    }
    // A Legendary or Ultra Beast in a wild table without a row would be
    // ungated and uncatchable-once only by accident.
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        if (!IsEmeraldWildHeader(header))
            continue;
        const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[TIME_OF_DAY_DEFAULT];
        const struct WildPokemonInfo *infos[] = {types->landMonsInfo, types->waterMonsInfo};
        const u32 slots[] = {NUM_LAND_MONS_ENCOUNTER_SLOTS, NUM_WATER_MONS_ENCOUNTER_SLOTS};
        for (u32 method = 0; method < ARRAY_COUNT(infos); method++)
        for (u32 slot = 0; infos[method] != NULL && slot < slots[method]; slot++)
        {
            enum Species species = infos[method]->wildPokemon[slot].species;
            if (IsLegendaryEncounterSpecies(species) && GetLegendarySignIdBySpecies(species) >= LEGENDARY_SIGN_COUNT)
            {
                Test_MgbaPrintf("Wild legend without a gate row: map %d.%d species %d",
                    gWildMonHeaders[header].mapGroup, gWildMonHeaders[header].mapNum, species);
                missing++;
            }
        }
    }
    EXPECT_EQ(missing, 0);
}
