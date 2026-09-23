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
            MarkLegendarySignCaughtBySpecies(gLegendarySignDefinitions[id].species);
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

    // An ordinary-table Ultra Beast keeps its 40% slot, outside reversal.
    // Ordinary totals 30/20/10 reverse to 10/20/30.
    mons[0].species = SPECIES_PHEROMOSA;
    for (u32 i = 1; i < NUM_WATER_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = species[i - 1];
    u32 legendaryCount = 0;
    memset(counts, 0, sizeof(counts));
    for (u32 choice = 1; choice <= 2; choice++)
    {
        SET_RNG(RNG_WILD_MON_TARGET, choice);
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            u32 index = ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_WATER);
            EXPECT_LT(index, NUM_WATER_MONS_ENCOUNTER_SLOTS);
            legendaryCount += index == 0;
            for (u32 i = 0; i < ARRAY_COUNT(species); i++)
                counts[i] += mons[index].species == species[i];
        }
    }
    EXPECT_EQ(legendaryCount, 80);
    EXPECT_EQ(counts[0], 20);
    EXPECT_EQ(counts[1], 40);
    EXPECT_EQ(counts[2], 60);
    EXPECT_EQ(counts[3], 0);
    MarkLegendarySignCaughtBySpecies(SPECIES_PHEROMOSA);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        EXPECT_NE(ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_WATER), 0);
    }
    // A habitat with a single ordinary species remains usable after capture.
    for (u32 i = 1; i < NUM_WATER_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = SPECIES_MAGIKARP;
    EXPECT_EQ(mons[ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_WATER)].species, SPECIES_MAGIKARP);
}

TEST("Wild residents leave encounter pools after capture and signs record completion")
{
    const enum Species expected[] = {SPECIES_RAIKOU, SPECIES_TAPU_KOKO, SPECIES_THUNDURUS};
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE110);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE110);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_RAIKOU));
    for (u32 unlocked = 0; unlocked < 2; unlocked++)
    {
        if (unlocked)
        {
            for (u32 badge = 0; badge < 5; badge++)
                FlagSet(FLAG_BADGE01_GET + badge);
            FlagSet(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
        }
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE),
                roll == 0 ? SPECIES_RAIKOU : roll == 1 ? SPECIES_TAPU_KOKO
                : unlocked && roll == 2 ? SPECIES_THUNDURUS : SPECIES_NONE);
            for (u32 choice = 0; choice < 2 + unlocked; choice++)
            {
                SET_RNG(RNG_WILD_MON_TARGET, choice);
                EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE),
                    roll < 25 ? expected[choice] : SPECIES_NONE);
            }
        }
    }
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_WATER, TRUE), SPECIES_NONE);
    MarkLegendarySignCaughtBySpecies(SPECIES_RAIKOU);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_RAIKOU));
    // The remaining native and quest each keep a 1% standard slot.
    // Sweet Scent redistributes its total 25% over the two remaining species.
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE),
            roll == 0 ? SPECIES_TAPU_KOKO : roll == 1 ? SPECIES_THUNDURUS : SPECIES_NONE);
        for (u32 choice = 0; choice < 2; choice++)
        {
            SET_RNG(RNG_WILD_MON_TARGET, choice);
            EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE),
                roll < 25 ? expected[choice + 1] : SPECIES_NONE);
        }
    }
    MarkLegendarySignCaughtBySpecies(SPECIES_THUNDURUS);
    SET_RNG(RNG_NONE, 0);
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE), SPECIES_TAPU_KOKO);
    MarkLegendarySignCaughtBySpecies(SPECIES_TAPU_KOKO);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE), SPECIES_NONE);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE), SPECIES_NONE);
    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        if (sign->source != LEGENDARY_SOURCE_NATIVE_WILD)
            continue;
        EXPECT(CanAcquireLegendarySignSpecies(sign->species));
        MarkLegendarySignCaughtBySpecies(sign->species);
        EXPECT(!CanAcquireLegendarySignSpecies(sign->species));
        gSaveBlock1Ptr->location.mapGroup = sign->mapId >> 8;
        gSaveBlock1Ptr->location.mapNum = sign->mapId & 255;
        BufferCurrentMapRouteSignSpecies();
        u32 length = StringLength(gStringVar4);
        u8 name[64];
        StringCopy(name, GetLegendaryDisplayName(sign->species));
        StringAppend(name, COMPOUND_STRING(" (Caught)"));
        u32 nameLength = StringLength(name);
        bool32 found = FALSE;
        EXPECT_LT(length, sizeof(gStringVar4));
        for (u32 i = 0; i + nameLength <= length; i++)
            found |= StringCompareN(gStringVar4 + i, name, nameLength) == 0;
        EXPECT(found);
        // Long regional names plus the marker must still fit the sign window.
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
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 4);
    }
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
    EXPECT(GuideTextContains(COMPOUND_STRING("1%")));
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("1%")));
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

TEST("Legendary scene retries require the Regis")
{
    // Route roster signs must resolve in Emerald, not only in FireRed/LeafGreen.
    const struct ObjectEventGraphicsInfo *sign = GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_SIGN);
    EXPECT(sign != NULL);
    EXPECT(sign->images != NULL);
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    UnlockLegendarySign(LEGENDARY_SIGN_REGIGIGAS);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    const enum Species regis[] = {SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL};
    for (u32 slot = 0; slot < ARRAY_COUNT(regis); slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], regis[slot], 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    ZeroMonData(&gParties[B_TRAINER_PLAYER][2]);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    // Southern Island's Latis keep Inclement's own encounter (party level,
    // Soul Dew) through CreateEventLegalEnemyMon in their map script.
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

TEST("Verdanturf Meadow: each uncaught resident keeps its odds and capture removes only that resident")
{
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_VERDANTURF_MEADOW);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_VERDANTURF_MEADOW);
    const enum Species residents[] = {SPECIES_ENAMORUS, SPECIES_FEZANDIPITI};
    for (u32 caught = 0; caught <= ARRAY_COUNT(residents); caught++)
    {
        u32 counts[3] = {0};
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            enum Species species = ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE);
            EXPECT(species == SPECIES_NONE || species == residents[0] || species == residents[1]);
            counts[species == residents[0] ? 0 : species == residents[1] ? 1 : 2]++;
        }
        EXPECT_EQ(counts[0], caught == 0 ? 1 : 0);
        EXPECT_EQ(counts[1], caught < 2 ? 1 : 0);
        EXPECT_EQ(counts[2], 98 + caught);
        EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_WATER, FALSE), SPECIES_NONE);
        EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_ROCKS, FALSE), SPECIES_NONE);
        for (u32 choice = 0; choice < ARRAY_COUNT(residents) - caught; choice++)
        {
            SET_RNG(RNG_WILD_MON_TARGET, choice);
            for (u32 roll = 0; roll < 100; roll++)
            {
                SET_RNG(RNG_NONE, roll);
                EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE),
                    roll < 25 ? residents[caught + choice] : SPECIES_NONE);
            }
        }
        if (caught < ARRAY_COUNT(residents))
            MarkLegendarySignCaughtBySpecies(residents[caught]);
    }
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE), SPECIES_NONE);
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
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE),
            roll == 0 ? SPECIES_MARSHADOW : SPECIES_NONE);
    }
    MarkLegendarySignCaughtBySpecies(SPECIES_MARSHADOW);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
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
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    UnlockLegendarySign(LEGENDARY_SIGN_MARSHADOW);
    SET_RNG(RNG_NONE, 0);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE), SPECIES_MARSHADOW);
    EXPECT(Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    gMapHeader.regionMapSectionId = MAPSEC_PALLET_TOWN;
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_113;
    MarkLegendarySignCaughtBySpecies(SPECIES_MARSHADOW);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE), SPECIES_NONE);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
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
    EXPECT(Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    MarkLegendarySignCaughtBySpecies(SPECIES_ENAMORUS);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    gMapHeader.regionMapSectionId = oldSection;
    gAreaTimeOfDay = oldTime;
    ResetSignState();
}

TEST("Rare distribution: every wild legend has a runtime table and one-percent standard odds")
{
    u32 id = 0;
    for (u32 candidate = 0; candidate < LEGENDARY_SIGN_COUNT; candidate++)
        if (gLegendarySignDefinitions[candidate].source == LEGENDARY_SOURCE_RARE_WILD
            || gLegendarySignDefinitions[candidate].source == LEGENDARY_SOURCE_NATIVE_WILD)
            PARAMETRIZE { id = candidate; }

    struct WarpData oldLocation = gSaveBlock1Ptr->location;
    u32 oldBadges = 0;
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
    {
        oldBadges |= FlagGet(FLAG_BADGE01_GET + badge) << badge;
        FlagSet(FLAG_BADGE01_GET + badge);
    }
    ResetSignState();
    const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
    bool32 oldRequirement = sign->requiredFlag && FlagGet(sign->requiredFlag);
    if (sign->requiredFlag) FlagSet(sign->requiredFlag);
    UnlockLegendarySign(id);
    EXPECT(CanAcquireLegendarySignSpecies(sign->species));
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(sign->mapId);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(sign->mapId);
    u16 header = GetCurrentMapWildMonHeaderId();
    EXPECT_NE(header, HEADER_NONE);
    enum WildPokemonArea habitat = sign->mapId == MAP_ROUTE125 || sign->mapId == MAP_ROUTE126
                                || sign->mapId == MAP_ROUTE127 ? WILD_AREA_WATER : WILD_AREA_LAND;
    enum TimeOfDay encounterTime = GetTimeOfDayForEncounters(header, habitat);
    const struct WildEncounterTypes *tables = &gWildMonHeaders[header].encounterTypes[encounterTime];
    const struct WildPokemonInfo *table = habitat == WILD_AREA_WATER ? tables->waterMonsInfo : tables->landMonsInfo;
    EXPECT(table != NULL);
    EXPECT(table->encounterRate > 0);
    u32 hits = 0;
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        if (ChooseRareWildLegendarySpecies(habitat, FALSE) == sign->species)
            hits++;
    }
    EXPECT_EQ(hits, 1);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(habitat == WILD_AREA_WATER ? WILD_AREA_LAND : WILD_AREA_WATER, FALSE), SPECIES_NONE);
    MarkLegendarySignCaughtBySpecies(sign->species);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        EXPECT_NE(ChooseRareWildLegendarySpecies(habitat, FALSE), sign->species);
    }
    if (sign->requiredFlag && !oldRequirement) FlagClear(sign->requiredFlag);
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        if (!(oldBadges & (1u << badge))) FlagClear(FLAG_BADGE01_GET + badge);
    gSaveBlock1Ptr->location = oldLocation;
    ResetSignState();
}
