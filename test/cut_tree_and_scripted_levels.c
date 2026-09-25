#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "event_data.h"
#include "field_specials.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "tv.h"
#include "wild_encounter.h"
#include "constants/flags.h"
#include "constants/map_event_ids.h"
#include "constants/maps.h"
#include "constants/opponents.h"
#include "constants/region_map_sections.h"
#include "constants/rtc.h"
#include "constants/vars.h"
#include "test/overworld_script.h"
#include "test/test.h"

// Native Cut-tree habitat (src/wild_encounter.c).
extern u32 GetCutTreeSlotCount(void);
extern enum Species GetCutTreeSlotSpecies(u32 slot);
extern u32 GetCutTreeSlotOdds(u32 slot);
extern u32 ChooseCutTreeSlotFromRoll(u32 roll);
extern u8 GetCutTreeEncounterLevelFromRoll(u32 roll);
extern void CutTreeWildEncounter(void);
extern void GetLevelCapForScriptedGift(void);
extern const u8 GabbyAndTy_EventScript_UpdateLocation[];

static const u16 sCapFlags[] = {
    FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT,
    FLAG_BADGE07_GET, FLAG_BADGE08_GET, FLAG_IS_CHAMPION,
};

static void SaveAndClearCapFlags(bool8 *saved)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCapFlags); i++)
    {
        saved[i] = FlagGet(sCapFlags[i]);
        FlagClear(sCapFlags[i]);
    }
}

static void RestoreCapFlags(const bool8 *saved)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCapFlags); i++)
    {
        if (saved[i])
            FlagSet(sCapFlags[i]);
        else
            FlagClear(sCapFlags[i]);
    }
}

static bool32 IsCutTreeSpecies(enum Species species)
{
    for (u32 slot = 0; slot < GetCutTreeSlotCount(); slot++)
        if (GetCutTreeSlotSpecies(slot) == species)
            return TRUE;
    return FALSE;
}

TEST("Cut trees: one shared habitat with the owner's roster and odds")
{
    static const struct { enum Species species; u8 odds; } expected[] = {
        {SPECIES_SKWOVET, 40}, {SPECIES_PINECO, 30}, {SPECIES_AIPOM, 15},
        {SPECIES_BURMY, 8}, {SPECIES_APPLIN, 5}, {SPECIES_PHANTUMP, 2},
    };
    u32 rolled[ARRAY_COUNT(expected)] = {0};

    EXPECT_EQ(GetCutTreeSlotCount(), ARRAY_COUNT(expected));
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetCutTreeSlotSpecies(slot), expected[slot].species);
        EXPECT_EQ(GetCutTreeSlotOdds(slot), expected[slot].odds);
    }
    // Every percent of the roll lands on exactly its slot's share.
    for (u32 roll = 0; roll < 100; roll++)
        rolled[ChooseCutTreeSlotFromRoll(roll)]++;
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
        EXPECT_EQ(rolled[slot], expected[slot].odds);
}

TEST("Cut trees: levels sit four to eight under the live cap at every milestone")
{
    bool8 saved[ARRAY_COUNT(sCapFlags)];
    SaveAndClearCapFlags(saved);
    for (u32 milestone = 0; milestone <= ARRAY_COUNT(sCapFlags); milestone++)
    {
        if (milestone > 0)
            FlagSet(sCapFlags[milestone - 1]);
        u32 cap = GetCurrentLevelCap();
        bool32 seen[5] = {FALSE};
        for (u32 roll = 0; roll < 50; roll++)
        {
            u32 level = GetCutTreeEncounterLevelFromRoll(roll);
            EXPECT_GE(level, cap - 8);
            EXPECT_LE(level, cap - 4);
            seen[level - (cap - 8)] = TRUE;
        }
        for (u32 i = 0; i < ARRAY_COUNT(seen); i++)
            EXPECT(seen[i]);
    }
    RestoreCapFlags(saved);
}

TEST("Cut trees: their six species live in no map table")
{
    u32 checked = 0;
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        for (u32 time = 0; time < TIMES_OF_DAY_COUNT; time++)
        {
            const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[time];
            const struct { const struct WildPokemonInfo *info; u32 slots; } tables[] = {
                {types->landMonsInfo, NUM_LAND_MONS_ENCOUNTER_SLOTS},
                {types->waterMonsInfo, NUM_WATER_MONS_ENCOUNTER_SLOTS},
                {types->rockSmashMonsInfo, NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS},
                {types->fishingMonsInfo, NUM_FISHING_MONS_ENCOUNTER_SLOTS},
                {types->hiddenMonsInfo, NUM_HIDDEN_MONS_ENCOUNTER_SLOTS},
                {types->honeyMonsInfo, NUM_HONEY_MONS_ENCOUNTER_SLOTS},
            };
            for (u32 t = 0; t < ARRAY_COUNT(tables); t++)
            {
                if (tables[t].info == NULL)
                    continue;
                for (u32 slot = 0; slot < tables[t].slots; slot++)
                {
                    EXPECT(!IsCutTreeSpecies(tables[t].info->wildPokemon[slot].species));
                    checked++;
                }
            }
        }
    }
    EXPECT_GT(checked, 0);
}

extern bool32 Test_PokedexAreaHasSection(enum Species species, u16 section);

TEST("Cut trees: the Pokedex area page marks the tree habitat only where a Cut tree grows")
{
    // Route 116 has Cut trees; Route 101 has none.
    EXPECT(MapHeaderHasCutTrees(Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ROUTE116), MAP_NUM(MAP_ROUTE116))));
    EXPECT(!MapHeaderHasCutTrees(Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101))));
    for (u32 slot = 0; slot < GetCutTreeSlotCount(); slot++)
    {
        enum Species species = GetCutTreeSlotSpecies(slot);
        EXPECT(IsCutTreeHabitatSpecies(species));
        EXPECT(Test_PokedexAreaHasSection(species, MAPSEC_ROUTE_116));
        EXPECT(!Test_PokedexAreaHasSection(species, MAPSEC_ROUTE_101));
    }
    EXPECT(!IsCutTreeHabitatSpecies(SPECIES_ZIGZAGOON));
}

TEST("Cut trees: a map without wild Pokemon never rolls an encounter")
{
    s8 savedGroup = gSaveBlock1Ptr->location.mapGroup;
    s8 savedMap = gSaveBlock1Ptr->location.mapNum;

    // The Trick House puzzle room is a Cut puzzle with no wild table.
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE110_TRICK_HOUSE_PUZZLE1);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE110_TRICK_HOUSE_PUZZLE1);
    EXPECT_EQ(GetCurrentMapWildMonHeaderId(), HEADER_NONE);
    for (u32 seed = 0; seed < 64; seed++)
    {
        SeedRng(seed);
        rng_value_t before = gRngValue;
        gSpecialVar_Result = 0xFF;
        CutTreeWildEncounter();
        EXPECT_EQ(gSpecialVar_Result, FALSE);
        EXPECT_EQ(memcmp(&before, &gRngValue, sizeof(before)), 0);
    }
    gSaveBlock1Ptr->location.mapGroup = savedGroup;
    gSaveBlock1Ptr->location.mapNum = savedMap;
}

TEST("Scripted levels: every gift and static arrives at the live cap")
{
    bool8 saved[ARRAY_COUNT(sCapFlags)];
    SaveAndClearCapFlags(saved);
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 milestone = 0; milestone <= ARRAY_COUNT(sCapFlags); milestone++)
    {
        if (milestone > 0)
            FlagSet(sCapFlags[milestone - 1]);
        // The party's best (Lv 5 or the cap) never changes the answer.
        for (u32 best = 0; best < 2; best++)
        {
            SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL, &(u8){best ? GetCurrentLevelCap() : 5});
            gSpecialVar_0x800A = 0;
            GetStaticEncounterLevel();
            EXPECT_EQ(gSpecialVar_0x800A, GetCurrentLevelCap());
            gSpecialVar_0x800A = 0;
            GetLevelCapForScriptedGift();
            EXPECT_EQ(gSpecialVar_0x800A, GetCurrentLevelCap());
        }
    }
    RestoreCapFlags(saved);
    ZeroPlayerPartyMons();
}

TEST("Gabby and Ty: each stop shows one pair and the retired pair keeps its trainer flag")
{
    static const struct { u8 battleNum; u16 shown; } stops[] = {
        {0, FLAG_HIDE_ROUTE_111_GABBY_AND_TY_1},
        {1, FLAG_HIDE_ROUTE_118_GABBY_AND_TY_1},
        {4, FLAG_HIDE_ROUTE_118_GABBY_AND_TY_2},
        {5, FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2},
        {6, FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2},
        {7, FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2}, // legacy endless-cycle save
        {8, FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2}, // legacy endless-cycle save
    };
    static const u16 allStops[] = {
        FLAG_HIDE_ROUTE_111_GABBY_AND_TY_1, FLAG_HIDE_ROUTE_111_GABBY_AND_TY_2,
        FLAG_HIDE_ROUTE_111_GABBY_AND_TY_3, FLAG_HIDE_ROUTE_118_GABBY_AND_TY_1,
        FLAG_HIDE_ROUTE_118_GABBY_AND_TY_2, FLAG_HIDE_ROUTE_118_GABBY_AND_TY_3,
        FLAG_HIDE_ROUTE_120_GABBY_AND_TY_1, FLAG_HIDE_ROUTE_120_GABBY_AND_TY_2,
    };
    for (u32 i = 0; i < ARRAY_COUNT(stops); i++)
    {
        ResetGabbyAndTy();
        ClearTrainerFlag(TRAINER_GABBY_AND_TY_6);
        for (u32 f = 0; f < ARRAY_COUNT(allStops); f++)
            FlagClear(allStops[f]);
        gSaveBlock1Ptr->gabbyAndTyData.battleNum = stops[i].battleNum;
        RUN_OVERWORLD_SCRIPT(call GabbyAndTy_EventScript_UpdateLocation;);
        for (u32 f = 0; f < ARRAY_COUNT(allStops); f++)
            EXPECT_EQ(FlagGet(allStops[f]), allStops[f] != stops[i].shown);
        // Only the retired pair (party 6 beaten) has its battle closed.
        EXPECT_EQ(HasTrainerBeenFought(TRAINER_GABBY_AND_TY_6), stops[i].battleNum >= 6);
        EXPECT_EQ(GabbyAndTyGetBattleNum(), min(stops[i].battleNum, 6));
    }
    ResetGabbyAndTy();
    ClearTrainerFlag(TRAINER_GABBY_AND_TY_6);
}
