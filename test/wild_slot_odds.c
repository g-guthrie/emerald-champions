#include "global.h"
#include "caps.h"
#include "legendary_signs.h"
#include "string_util.h"
#include "constants/maps.h"
#include "constants/characters.h"
#include "pokemon.h"
#include "mass_outbreak.h"
#include "roamer.h"
#include "overworld.h"
#include "constants/region_map_sections.h"
#include "event_data.h"
#include "field_specials.h"
#include "fieldmap.h"
#include "item.h"
#include "metatile_behavior.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "wild_encounter.h"
#include "constants/item.h"
#include "test/test.h"

TEST("Wild slots: land water and Rock Smash preserve authored odds and Lure draws")
{
    static const u8 land[] = {18, 18, 10, 10, 9, 9, 5, 5, 4, 4, 4, 4};
    static const u8 water[] = {40, 30, 20, 10};
    static const u8 rocks[] = {60, 30, 5, 5};
    for (u32 method = 0; method < 3; method++)
    for (u32 lure = 0; lure < 2; lure++)
    {
        const u8 *weights = method == 2 ? rocks : method == 1 ? water : land;
        u32 count = method ? ARRAY_COUNT(water) : ARRAY_COUNT(land);
        VarSet(VAR_REPEL_STEP_COUNT, lure ? REPEL_LURE_MASK | 100 : 0);
        for (u32 seed = 0; seed < 512; seed++)
        {
            SeedRng(seed);
            u32 roll = Random() % 100;
            u32 expected = 0;
            while (roll >= weights[expected])
                roll -= weights[expected++];
            if (lure && Random() % 10 < 2)
                expected = count - 1 - expected;
            rng_value_t after = gRngValue;
            SeedRng(seed);
            u32 actual = method == 2 ? ChooseWildMonIndex_Rocks(NULL)
                : method == 1 ? ChooseWildMonIndex_Water(NULL) : ChooseWildMonIndex_Land(NULL);
            EXPECT_EQ(actual, expected);
            EXPECT_EQ(memcmp(&gRngValue, &after, sizeof(after)), 0);
        }
    }
    VarSet(VAR_REPEL_STEP_COUNT, 0);
}

static const u16 sWildCapFlags[] = {
    FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET, FLAG_BADGE08_GET,
    FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, FLAG_IS_CHAMPION,
};

static void SaveAndClearWildCapFlags(bool8 *saved)
{
    for (u32 i = 0; i < ARRAY_COUNT(sWildCapFlags); i++)
    {
        saved[i] = FlagGet(sWildCapFlags[i]);
        FlagClear(sWildCapFlags[i]);
    }
}

static void RestoreWildCapFlags(const bool8 *saved)
{
    for (u32 i = 0; i < ARRAY_COUNT(sWildCapFlags); i++)
        if (saved[i])
            FlagSet(sWildCapFlags[i]);
        else
            FlagClear(sWildCapFlags[i]);
}

TEST("Wild levels: creation respects the current cap without raising low-level encounters")
{
    bool8 saved[ARRAY_COUNT(sWildCapFlags)];
    SaveAndClearWildCapFlags(saved);
    EXPECT_EQ(GetCurrentLevelCap(), 14);
    static const u8 levels[] = {2, 14, 50, 100};
    for (u32 i = 0; i < ARRAY_COUNT(levels); i++)
    {
        CreateWildMon(SPECIES_EEVEE, levels[i]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), min(levels[i], 14));
    }
    FlagSet(FLAG_BADGE01_GET);
    CreateWildMon(SPECIES_EEVEE, 50);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 20);
    FlagSet(FLAG_IS_CHAMPION);
    CreateWildMon(SPECIES_EEVEE, 100);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 100);
    RestoreWildCapFlags(saved);
    ZeroEnemyPartyMons();
}

TEST("Wild levels: stale tables are floored to the cap and evolved Pokemon to their evolution level")
{
    bool8 saved[ARRAY_COUNT(sWildCapFlags)];
    SaveAndClearWildCapFlags(saved);
    // Cap 14: an early route's own levels stand.
    EXPECT_EQ(ApplyWildLevelFloor(SPECIES_ZIGZAGOON, 3), 3);
    // Cap 45: a table authored for level 20 rises to within 12 of the cap.
    FlagSet(FLAG_BADGE01_GET); FlagSet(FLAG_BADGE02_GET); FlagSet(FLAG_BADGE03_GET); FlagSet(FLAG_BADGE04_GET);
    EXPECT_EQ(GetCurrentLevelCap(), 45);
    for (u32 i = 0; i < 20; i++)
    {
        u32 level = ApplyWildLevelFloor(SPECIES_SANDSHREW, 20);
        EXPECT(level >= 33 && level <= 36);
    }
    // In-range levels are untouched.
    EXPECT_EQ(ApplyWildLevelFloor(SPECIES_SANDSHREW, 40), 40);
    // Zweilous evolves from Deino at 50: never met below it, but never above the cap.
    EXPECT_EQ(ApplyWildLevelFloor(SPECIES_ZWEILOUS, 40), 45);
    FlagSet(FLAG_BADGE05_GET); FlagSet(FLAG_BADGE06_GET); FlagSet(FLAG_BADGE07_GET);
    EXPECT_EQ(ApplyWildLevelFloor(SPECIES_ZWEILOUS, 60), 60);
    u32 floored = ApplyWildLevelFloor(SPECIES_ZWEILOUS, 49); // cap 70: floor 58-61
    EXPECT(floored >= 58 && floored <= 61);
    RestoreWildCapFlags(saved);
}

TEST("Wild levels: the generated evolution floors match each species' level evolution")
{
    static const struct { enum Species species; u8 level; } cases[] = {
        {SPECIES_GRAVELER, 25}, {SPECIES_ZWEILOUS, 50}, {SPECIES_CLAYDOL, 36},
        {SPECIES_MANECTRIC, 26}, {SPECIES_ZIGZAGOON, 1},
    };
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        enum Species pre = GetSpeciesPreEvolution(cases[i].species);
        u32 expected = 1;
        const struct Evolution *evolutions = pre != SPECIES_NONE ? GetSpeciesEvolutions(pre) : NULL;
        for (u32 j = 0; evolutions != NULL && evolutions[j].method != EVOLUTIONS_END; j++)
            if (evolutions[j].targetSpecies == cases[i].species && evolutions[j].method == EVO_LEVEL && evolutions[j].param != 0)
                expected = evolutions[j].param;
        EXPECT_EQ(expected, cases[i].level);
    }
}

TEST("Wild levels: outbreak Repel checks the capped level before creating an encounter")
{
    bool8 saved[ARRAY_COUNT(sWildCapFlags)];
    SaveAndClearWildCapFlags(saved);
    u16 oldRepel = VarGet(VAR_REPEL_STEP_COUNT);
    enum Species oldSpecies = gSaveBlock1Ptr->outbreakPokemonSpecies;
    u8 oldLevel = gSaveBlock1Ptr->outbreakPokemonLevel;
    u16 oldMoves[MAX_MON_MOVES];
    memcpy(oldMoves, gSaveBlock1Ptr->outbreakPokemonMoves, sizeof(oldMoves));
    gSaveBlock1Ptr->outbreakPokemonSpecies = SPECIES_SEEDOT;
    gSaveBlock1Ptr->outbreakPokemonLevel = 80;
    memset(gSaveBlock1Ptr->outbreakPokemonMoves, 0, sizeof(oldMoves));
    gSaveBlock1Ptr->outbreakPokemonMoves[0] = MOVE_TACKLE;
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 15, 0, OTID_STRUCT_PLAYER_ID);
    VarSet(VAR_REPEL_STEP_COUNT, 100);
    EXPECT(!SetUpMassOutbreakEncounter(WILD_CHECK_REPEL));
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(SetUpMassOutbreakEncounter(WILD_CHECK_REPEL));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 14);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MOVE1), MOVE_TACKLE);
    gSaveBlock1Ptr->outbreakPokemonSpecies = oldSpecies;
    gSaveBlock1Ptr->outbreakPokemonLevel = oldLevel;
    memcpy(gSaveBlock1Ptr->outbreakPokemonMoves, oldMoves, sizeof(oldMoves));
    VarSet(VAR_REPEL_STEP_COUNT, oldRepel);
    RestoreWildCapFlags(saved);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
}

TEST("Wild outbreaks: applying authored moves preserves the visible Pokemon identity")
{
    struct Pokemon mon;
    u16 oldMoves[MAX_MON_MOVES];
    memcpy(oldMoves, gSaveBlock1Ptr->outbreakPokemonMoves, sizeof(oldMoves));
    static const u16 moves[] = {MOVE_TACKLE, MOVE_GROWTH, MOVE_LEECH_SEED, MOVE_NONE};
    memcpy(gSaveBlock1Ptr->outbreakPokemonMoves, moves, sizeof(moves));
    CreateMon(&mon, SPECIES_SEEDOT, 14, 123456, OTID_STRUCT_PLAYER_ID);
    u32 shiny = TRUE;
    SetMonData(&mon, MON_DATA_IS_SHINY, &shiny);
    u32 personality = GetMonData(&mon, MON_DATA_PERSONALITY);
    u32 otId = GetMonData(&mon, MON_DATA_OT_ID);
    u32 hp = GetMonData(&mon, MON_DATA_HP);
    rng_value_t rng = gRngValue;
    ApplyMassOutbreakMoves(&mon);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PERSONALITY), personality);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_OT_ID), otId);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_SEEDOT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 14);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), hp);
    EXPECT(GetMonData(&mon, MON_DATA_IS_SHINY));
    EXPECT_EQ(memcmp(&gRngValue, &rng, sizeof(rng)), 0);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + slot), moves[slot]);
    memcpy(gSaveBlock1Ptr->outbreakPokemonMoves, oldMoves, sizeof(oldMoves));
}

TEST("Route rosters: every encounter map fits the dialog buffer with caught labels")
{
    static const u16 caughtVars[] = {
        VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
    };
    u16 savedCaught[ARRAY_COUNT(caughtVars)];
    s8 savedGroup = gSaveBlock1Ptr->location.mapGroup;
    s8 savedMap = gSaveBlock1Ptr->location.mapNum;
    u16 savedCave = VarGet(VAR_ALTERING_CAVE_WILD_SET);
    for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
        savedCaught[i] = VarGet(caughtVars[i]);
    VarSet(VAR_ALTERING_CAVE_WILD_SET, 0);
    u32 maximum = 0, checked = 0;
    for (u32 caught = 0; caught < 2; caught++)
    {
        for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
            VarSet(caughtVars[i], caught ? 0xFFFF : 0);
        for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
        {
            gSaveBlock1Ptr->location.mapGroup = gWildMonHeaders[header].mapGroup;
            gSaveBlock1Ptr->location.mapNum = gWildMonHeaders[header].mapNum;
            rng_value_t before = gRngValue;
            BufferCurrentMapRouteSignSpecies();
            u32 length = 0;
            while (length < sizeof(gStringVar4) && gStringVar4[length] != EOS)
                length++;
            EXPECT_LT(length, sizeof(gStringVar4));
            EXPECT_EQ(memcmp(&before, &gRngValue, sizeof(before)), 0);
            maximum = max(maximum, length);
            checked++;
        }
    }
    Test_MgbaPrintf("Route roster checks=%d maximum bytes=%d capacity=%d", checked, maximum, sizeof(gStringVar4));
    for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
        VarSet(caughtVars[i], savedCaught[i]);
    VarSet(VAR_ALTERING_CAVE_WILD_SET, savedCave);
    gSaveBlock1Ptr->location.mapGroup = savedGroup;
    gSaveBlock1Ptr->location.mapNum = savedMap;
}

TEST("Local wild species: optional habitat output preserves selection and RNG")
{
    static const u16 maps[] = {MAP_ROUTE101, MAP_ROUTE103, MAP_ROUTE125, MAP_UNDEFINED};
    s8 savedGroup = gSaveBlock1Ptr->location.mapGroup;
    s8 savedMap = gSaveBlock1Ptr->location.mapNum;
    u32 checked = 0;
    for (u32 map = 0; map < ARRAY_COUNT(maps); map++)
    {
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(maps[map]);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(maps[map]);
        for (u32 seed = 0; seed < 128; seed++)
        {
            bool8 water = TRUE;
            SeedRng(seed);
            enum Species expected = GetLocalWildMon(&water);
            rng_value_t after = gRngValue;
            if (maps[map] == MAP_UNDEFINED)
            {
                EXPECT_EQ(expected, SPECIES_NONE);
                EXPECT_EQ(water, FALSE);
                EXPECT(!DoesCurrentMapHaveFishingMons());
            }
            SeedRng(seed);
            EXPECT_EQ(GetLocalWildMon(NULL), expected);
            EXPECT_EQ(memcmp(&after, &gRngValue, sizeof(after)), 0);
            checked++;
        }
    }
    EXPECT_EQ(checked, 512);
    gSaveBlock1Ptr->location.mapGroup = savedGroup;
    gSaveBlock1Ptr->location.mapNum = savedMap;
}

TEST("Scripted wild creation: doubles preserve single-mon identity items moves and RNG order")
{
    const struct {enum Species species; u8 level; enum Item item;} encounters[] = {
        {SPECIES_REGIGIGAS, 50, ITEM_NORMAL_GEM},
        {SPECIES_LATIAS, 70, ITEM_SOUL_DEW},
        {SPECIES_UNOWN, 5, ITEM_NONE},
        {SPECIES_PIKACHU, 25, ITEM_SITRUS_BERRY},
    };
    for (u32 lead = 0; lead < 2; lead++)
    {
        ZeroPlayerPartyMons();
        if (lead)
            CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ABRA, 20, 0, OTID_STRUCT_PLAYER_ID);
        CalculatePlayerPartyCount();
        for (u32 seed = 0; seed < 32; seed++)
        {
            u32 first = seed % ARRAY_COUNT(encounters);
            u32 second = (first + 1) % ARRAY_COUNT(encounters);
            struct Pokemon expected[2];
            SeedRng(seed);
            CreateScriptedWildMon(encounters[first].species, encounters[first].level, encounters[first].item);
            memcpy(&expected[0], &gParties[B_TRAINER_OPPONENT_A][0], sizeof(expected[0]));
            CreateScriptedWildMon(encounters[second].species, encounters[second].level, encounters[second].item);
            memcpy(&expected[1], &gParties[B_TRAINER_OPPONENT_A][0], sizeof(expected[1]));
            rng_value_t after = gRngValue;
            // A new double must clear stale opponents beyond its two members.
            gParties[B_TRAINER_OPPONENT_A][5] = expected[0];
            SeedRng(seed);
            CreateScriptedDoubleWildMon(encounters[first].species, encounters[first].level, encounters[first].item,
                encounters[second].species, encounters[second].level, encounters[second].item);
            if (memcmp(expected, gParties[B_TRAINER_OPPONENT_A], sizeof(expected)))
                Test_MgbaPrintf("Scripted parity lead=%d seed=%d p0=%d/%d p1=%d/%d", lead, seed,
                    GetMonData(&expected[0], MON_DATA_PERSONALITY), GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_PERSONALITY),
                    GetMonData(&expected[1], MON_DATA_PERSONALITY), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_PERSONALITY));
            EXPECT_EQ(GetMonData(&expected[0], MON_DATA_SPECIES), encounters[first].species);
            // Legendary-class species ignore the script level and take the
            // cap plus their set; everything else keeps the script's values.
            if (IsLegendaryEncounterSpecies(encounters[first].species))
            {
                EXPECT_EQ(GetMonData(&expected[0], MON_DATA_LEVEL), GetCurrentLevelCap());
                EXPECT_NE(GetMonData(&expected[0], MON_DATA_HELD_ITEM), ITEM_NONE);
            }
            else
            {
                EXPECT_EQ(GetMonData(&expected[0], MON_DATA_LEVEL), encounters[first].level);
                EXPECT_EQ(GetMonData(&expected[0], MON_DATA_HELD_ITEM), encounters[first].item);
            }
            EXPECT_EQ(memcmp(expected, gParties[B_TRAINER_OPPONENT_A], sizeof(expected)), 0);
            EXPECT_EQ(memcmp(&after, &gRngValue, sizeof(after)), 0);
            for (u32 slot = 0; slot < 2; slot++)
            {
                u8 nickname[POKEMON_NAME_LENGTH + 1];
                enum Species species = GetMonData(&expected[slot], MON_DATA_SPECIES);
                GetMonData(&expected[slot], MON_DATA_NICKNAME, nickname);
                EXPECT_EQ(StringCompare(nickname, GetSpeciesName(species)), 0);
                for (u32 character = StringLength(nickname); character < POKEMON_NAME_LENGTH; character++)
                    EXPECT_EQ(nickname[character], EOS);
            }
            for (u32 slot = 2; slot < PARTY_SIZE; slot++)
                EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES), SPECIES_NONE);
        }
    }
    ZeroPlayerPartyMons();
    CalculatePlayerPartyCount();
    ZeroEnemyPartyMons();
}

TEST("Pokemon creation: facility nicknames have initialized fixed-width tails")
{
    struct BattleTowerPokemon source = {0};
    struct Pokemon mon;
    source.species = SPECIES_EEVEE;
    source.level = 50;
    StringCopy(source.nickname, COMPOUND_STRING("ACE"));
    for (u32 adjusted = 0; adjusted < 2; adjusted++)
    {
        if (adjusted)
            CreateBattleTowerMon_HandleLevel(&mon, &source, TRUE);
        else
            CreateBattleTowerMon(&mon, &source);
        u8 nickname[POKEMON_NAME_LENGTH + 1];
        GetMonData(&mon, MON_DATA_NICKNAME, nickname);
        EXPECT_EQ(StringCompare(nickname, source.nickname), 0);
        for (u32 i = 3; i < POKEMON_NAME_LENGTH; i++)
            EXPECT_EQ(nickname[i], EOS);
    }
}

TEST("Wild outbreaks: TV selection stays within the five authored outbreaks")
{
    u32 seen = 0;
    SeedRng(0xABCDEF01);
    for (u32 draw = 0; draw < 512; draw++)
    {
        TVShow show = {0};
        // Function tests otherwise force RNG_NONE to zero. Exercise the real
        // inclusive RNG range, restoring the harness before any assertion.
        const struct Test *savedTest = gTestRunnerState.test;
        struct Test localTest = *savedTest;
        struct TestRunner localRunner = *savedTest->runner;
        localRunner.randomUniform = NULL;
        localTest.runner = &localRunner;
        gTestRunnerState.test = &localTest;
        PrepareTvShowForRandomOutbreak(&show);
        gTestRunnerState.test = savedTest;
        EXPECT(show.massOutbreak.outbreakIndex >= 1);
        EXPECT(show.massOutbreak.outbreakIndex <= OUTBREAK_COUNT);
        u32 index = show.massOutbreak.outbreakIndex - 1;
        EXPECT_EQ(show.massOutbreak.species, GetStaticOutbreakSpecies(index));
        seen |= 1 << index;
    }
    EXPECT_EQ(seen, (1 << OUTBREAK_COUNT) - 1);
}

TEST("Wild outbreaks: invalid saved indices preserve the current outbreak")
{
    static const u16 invalid[] = {OUTBREAK_COUNT, 255, 65535};
    StartStaticMassOutbreak(OUTBREAK_ID_ROUTE102);
    for (u32 i = 0; i < ARRAY_COUNT(invalid); i++)
    {
        StartStaticMassOutbreak(invalid[i]);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonSpecies, SPECIES_SEEDOT);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakLocationMapGroup, MAP_GROUP(MAP_ROUTE102));
        EXPECT_EQ(gSaveBlock1Ptr->outbreakLocationMapNum, MAP_NUM(MAP_ROUTE102));
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonLevel, 3);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonProbability, 100);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakDaysLeft, 1);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonMoves[0], MOVE_BIDE);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonMoves[1], MOVE_HARDEN);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonMoves[2], MOVE_LEECH_SEED);
        EXPECT_EQ(gSaveBlock1Ptr->outbreakPokemonMoves[3], MOVE_NONE);
    }
    UpdateMassOutbreakDaysLeft(65535);
    EXPECT(!IsMassOutbreakActive());
}

TEST("Wild roamers: reencounters preserve identity damage status and authored moves")
{
    static const enum Species species[] = {SPECIES_LATIAS, SPECIES_LATIOS};
    static const u32 statuses[] = {STATUS1_PARALYSIS, STATUS1_FROSTBITE};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        DeactivateAllRoamers();
        EXPECT(TryAddRoamer(species[i], 40));
        EXPECT(!TryAddRoamer(SPECIES_EEVEE, 5)); // The campaign has one roaming slot.
        struct Roamer saved = gSaveBlock1Ptr->roamer[0];
        CreateRoamerMonInstance(0);
        struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), species[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 40);
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP), saved.hp);
        EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP), saved.hp);
        enum Move moves[MAX_MON_MOVES];
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            moves[slot] = GetMonData(mon, MON_DATA_MOVE1 + slot);
        u32 hp = saved.hp / 2;
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_STATUS, &statuses[i]);
        gEncounteredRoamerIndex = 0;
        UpdateRoamerHPStatus(mon);
        CreateRoamerMonInstance(0);
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP), hp);
        EXPECT_EQ(GetMonData(mon, MON_DATA_STATUS), statuses[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_IVS), saved.ivs);
        EXPECT_EQ(GetMonData(mon, MON_DATA_PERSONALITY), saved.personality);
        EXPECT_EQ(GetMonData(mon, MON_DATA_IS_SHINY), saved.shiny);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1 + slot), moves[slot]);
        u8 group, map;
        GetRoamerLocation(0, &group, &map);
        for (u32 move = 0; move < 64; move++)
        {
            u8 oldMap = map;
            RoamerMoveToOtherLocationSet(0);
            GetRoamerLocation(0, &group, &map);
            EXPECT_EQ(group, MAP_GROUP(MAP_ROUTE110));
            EXPECT_NE(map, oldMap);
            EXPECT_NE(map, MAP_NUM(MAP_UNDEFINED));
            EXPECT(IsRoamerAt(0, group, map));
        }
        // Defeat permits a fresh attempt with full HP and no carried status.
        hp = 0;
        SetMonData(mon, MON_DATA_HP, &hp);
        UpdateRoamerHPStatus(mon);
        EXPECT(gSaveBlock1Ptr->roamer[0].active);
        CreateRoamerMonInstance(0);
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP), saved.hp);
        EXPECT_EQ(GetMonData(mon, MON_DATA_STATUS), 0);
        EXPECT_EQ(GetMonData(mon, MON_DATA_PERSONALITY), saved.personality);
        GetRoamerLocation(0, &group, &map);
        SetRoamerInactive(0);
        EXPECT(!IsRoamerAt(0, group, map));
    }
    ZeroEnemyPartyMons();
}

extern bool32 Test_PokedexAreaHasSection(enum Species species, u16 section);
extern u8 gAreaTimeOfDay;

TEST("Wild habitat: active Altering Cave floors are not discarded as legacy encounter variants")
{
    u16 oldSection = gMapHeader.regionMapSectionId;
    u8 oldTime = gAreaTimeOfDay;
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_103;
    gAreaTimeOfDay = TIME_OF_DAY_DEFAULT;
    FlagSet(FLAG_LANDMARK_ALTERING_CAVE);
    for (u32 variant = 0; variant <= NUM_ALTERING_CAVE_TABLES; variant++)
    {
        VarSet(VAR_ALTERING_CAVE_WILD_SET, variant);
        EXPECT(Test_PokedexAreaHasSection(SPECIES_NOIVERN, MAPSEC_ALTERING_CAVE));
        EXPECT(Test_PokedexAreaHasSection(SPECIES_BASCULEGION, MAPSEC_ALTERING_CAVE));
        EXPECT_EQ(Test_PokedexAreaHasSection(SPECIES_ZORUA, MAPSEC_ALTERING_CAVE),
                  variant == 0 || variant == NUM_ALTERING_CAVE_TABLES);
    }
    FlagClear(FLAG_LANDMARK_ALTERING_CAVE);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_NOIVERN, MAPSEC_ALTERING_CAVE));
    VarSet(VAR_ALTERING_CAVE_WILD_SET, 0);
    gMapHeader.regionMapSectionId = oldSection;
    gAreaTimeOfDay = oldTime;
    FlagClear(FLAG_LANDMARK_ALTERING_CAVE);
}

extern bool32 Test_PokedexMapHasSpecies(const struct WildEncounterTypes *info, enum Species species);

TEST("Honey habitats: six-slot selection uses its own table and the Pokédex shows it")
{
    EXPECT_EQ(GetItemPocket(ITEM_HONEY), POCKET_ITEMS);
    static const u8 weights[] = {50, 15, 15, 10, 5, 5};
    static const struct WildPokemon mons[] = {
        {10, 10, SPECIES_EEVEE}, {10, 10, SPECIES_SEEDOT},
        {10, 10, SPECIES_ZIGZAGOON}, {10, 10, SPECIES_WURMPLE},
        {10, 10, SPECIES_POOCHYENA}, {10, 10, SPECIES_LOTAD},
    };
    const struct WildPokemonInfo honey = {.encounterRate = 20, .wildPokemon = mons};
    const struct WildEncounterTypes types = {.honeyMonsInfo = &honey};

    EXPECT(Test_PokedexMapHasSpecies(&types, SPECIES_LOTAD));
    EXPECT(!Test_PokedexMapHasSpecies(&types, SPECIES_MAGIKARP));
    for (u32 seed = 0; seed < 128; seed++)
    {
        SeedRng(seed);
        u32 roll = Random() % 100;
        u32 slot = 0;
        while (roll >= weights[slot])
            roll -= weights[slot++];
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&honey, WILD_AREA_HONEY, 0));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), mons[slot].species);
    }
    ZeroEnemyPartyMons();
}

TEST("Honey habitats: every listed map layout has a usable encounter tile")
{
    struct MapHeader saved = gMapHeader;
    u32 checked = 0;
    for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
    {
        const struct WildPokemonHeader *wild = &gWildMonHeaders[i];
        if (wild->encounterTypes[TIME_OF_DAY_DEFAULT].honeyMonsInfo == NULL)
            continue;
        gMapHeader = *Overworld_GetMapHeaderByGroupAndId(wild->mapGroup, wild->mapNum);
        const struct MapLayout *layout = gMapHeader.mapLayout;
        bool32 found = FALSE;
        for (u32 y = 0; y < layout->height && !found; y++)
        for (u32 x = 0; x < layout->width && !found; x++)
        {
            u16 cell = layout->map[y * layout->width + x];
            if (!UNPACK_COLLISION(cell)
             && MetatileBehavior_IsLandWildEncounter(GetAttributeByMetatileIdAndMapLayout(
                    UNPACK_METATILE(cell), METATILE_ATTRIBUTE_BEHAVIOR, layout->isFrlg)))
                found = TRUE;
        }
        if (!found)
            Test_MgbaPrintf("Honey map has no usable encounter tile: %d/%d", wild->mapGroup, wild->mapNum);
        EXPECT(found);
        checked++;
    }
    EXPECT_EQ(checked, 29);
    gMapHeader = saved;
}

TEST("Paradox habitats: every enabled species has a usable land encounter tile")
{
    struct MapHeader saved = gMapHeader;
    u32 checked = 0;

    for (enum Species species = 1; species < NUM_SPECIES; species++)
    {
        if (!gSpeciesInfo[species].isParadox)
            continue;
        bool32 found = FALSE;
        for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED) && !found; i++)
        {
            const struct WildPokemonHeader *header = &gWildMonHeaders[i];
            const struct WildEncounterTypes *types = &header->encounterTypes[TIME_OF_DAY_DEFAULT];
            const struct WildPokemonInfo *tables[] = {types->landMonsInfo};
            const u32 counts[] = {NUM_LAND_MONS_ENCOUNTER_SLOTS};
            bool32 listed = FALSE;

            for (u32 method = 0; method < ARRAY_COUNT(tables); method++)
                if (tables[method] != NULL)
                    for (u32 slot = 0; slot < counts[method]; slot++)
                        if (tables[method]->wildPokemon[slot].species == species)
                            listed = TRUE;
            if (!listed)
                continue;
            gMapHeader = *Overworld_GetMapHeaderByGroupAndId(header->mapGroup, header->mapNum);
            const struct MapLayout *layout = gMapHeader.mapLayout;
            for (u32 y = 0; y < layout->height && !found; y++)
            for (u32 x = 0; x < layout->width && !found; x++)
            {
                u16 cell = layout->map[y * layout->width + x];
                if (!UNPACK_COLLISION(cell)
                 && MetatileBehavior_IsLandWildEncounter(GetAttributeByMetatileIdAndMapLayout(
                        UNPACK_METATILE(cell), METATILE_ATTRIBUTE_BEHAVIOR, layout->isFrlg)))
                    found = TRUE;
            }
        }
        if (!found)
            Test_MgbaPrintf("Paradox species has no usable source tile: %d", species);
        EXPECT(found);
        checked++;
    }
    EXPECT_EQ(checked, 20);
    gMapHeader = saved;
}

TEST("Disabled DexNav: no wild source depends on hidden habitats")
{
    u32 checked = 0;

    for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
    {
        const struct WildEncounterTypes *types = &gWildMonHeaders[i].encounterTypes[TIME_OF_DAY_DEFAULT];
        const struct WildPokemonInfo *hidden = types->hiddenMonsInfo;

        if (hidden == NULL)
            continue;
        checked++;
    }
    EXPECT_EQ(checked, 0);
}

TEST("Wild habitat: fishing scans stop before neighboring table data")
{
    struct WildPokemon mons[NUM_FISHING_MONS_ENCOUNTER_SLOTS + 2] = {0};
    for (u32 i = 0; i < NUM_FISHING_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = SPECIES_MAGIKARP;
    mons[NUM_FISHING_MONS_ENCOUNTER_SLOTS].species = SPECIES_MEW;
    mons[NUM_FISHING_MONS_ENCOUNTER_SLOTS + 1].species = SPECIES_CELEBI;
    const struct WildPokemonInfo fishing = {.encounterRate = 30, .wildPokemon = mons};
    const struct WildEncounterTypes info = {.fishingMonsInfo = &fishing};
    EXPECT(Test_PokedexMapHasSpecies(&info, SPECIES_MAGIKARP));
    EXPECT(!Test_PokedexMapHasSpecies(&info, SPECIES_MEW));
    EXPECT(!Test_PokedexMapHasSpecies(&info, SPECIES_CELEBI));
    const struct WildEncounterTypes empty = {0};
    EXPECT(!Test_PokedexMapHasSpecies(&empty, SPECIES_MAGIKARP));
}

TEST("Wild habitats: compiled water tables retain regional identities after reconciliation")
{
    static const struct {
        u8 group, map;
        enum Species first, second;
    } habitats[] = {
        {MAP_GROUP(MAP_ROUTE108), MAP_NUM(MAP_ROUTE108), SPECIES_LAPRAS, SPECIES_DHELMISE},
        {MAP_GROUP(MAP_ROUTE120), MAP_NUM(MAP_ROUTE120), SPECIES_STUNFISK, SPECIES_CHEWTLE},
        {MAP_GROUP(MAP_SAFARI_ZONE_NORTHWEST), MAP_NUM(MAP_SAFARI_ZONE_NORTHWEST), SPECIES_GRIMER_ALOLA, SPECIES_MUK_ALOLA},
        {MAP_GROUP(MAP_SEASPRAY_CAVE), MAP_NUM(MAP_SEASPRAY_CAVE), SPECIES_TYNAMO, SPECIES_CLAMPERL},
        {MAP_GROUP(MAP_ABANDONED_SHIP_ROOMS_B1F), MAP_NUM(MAP_ABANDONED_SHIP_ROOMS_B1F), SPECIES_GOLISOPOD, SPECIES_BARBARACLE},
        {MAP_GROUP(MAP_SOOTOPOLIS_CITY), MAP_NUM(MAP_SOOTOPOLIS_CITY), SPECIES_MAGIKARP, SPECIES_GYARADOS},
    };
    for (u32 i = 0; i < ARRAY_COUNT(habitats); i++)
    {
        const struct WildPokemonHeader *header = gWildMonHeaders;
        while (header->mapGroup != MAP_GROUP(MAP_UNDEFINED)
            && (header->mapGroup != habitats[i].group || header->mapNum != habitats[i].map))
            header++;
        EXPECT_NE(header->mapGroup, MAP_GROUP(MAP_UNDEFINED));
        const struct WildPokemonInfo *water = header->encounterTypes[TIME_OF_DAY_DEFAULT].waterMonsInfo;
        EXPECT(water != NULL);
        EXPECT_NE(water->encounterRate, 0);
        bool32 first = FALSE, second = FALSE;
        for (u32 slot = 0; slot < NUM_WATER_MONS_ENCOUNTER_SLOTS; slot++)
        {
            first |= water->wildPokemon[slot].species == habitats[i].first;
            second |= water->wildPokemon[slot].species == habitats[i].second;
        }
        EXPECT(first);
        EXPECT(second);
    }
}

TEST("Wild rarity: per-habitat odds and every rod preserve Lure RNG behavior")
{
    static const struct {
        u16 map;
        u8 rod; // 255 selects Surf.
        u8 count;
        u8 weights[5];
    } cases[] = {
        {MAP_ROUTE108, 255, 4, {50, 30, 2, 18}},
        {MAP_ROUTE111, 255, 4, {50, 30, 18, 2}},
        {MAP_ROUTE119, OLD_ROD, 2, {60, 40}},
        {MAP_ROUTE119, GOOD_ROD, 3, {60, 20, 20}},
        {MAP_ROUTE119, SUPER_ROD, 5, {40, 30, 18, 10, 2}},
        {MAP_PETALBURG_CITY, OLD_ROD, 2, {60, 40}},
        {MAP_PETALBURG_CITY, GOOD_ROD, 3, {60, 20, 20}},
        {MAP_PETALBURG_CITY, SUPER_ROD, 5, {40, 30, 15, 10, 5}},
    };
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        const struct WildPokemonHeader *header = gWildMonHeaders;
        while (header->mapGroup != MAP_GROUP(MAP_UNDEFINED)
            && (header->mapGroup != MAP_GROUP(cases[i].map) || header->mapNum != MAP_NUM(cases[i].map)))
            header++;
        EXPECT_NE(header->mapGroup, MAP_GROUP(MAP_UNDEFINED));
        const struct WildEncounterTypes *types = &header->encounterTypes[TIME_OF_DAY_DEFAULT];
        const struct WildPokemonInfo *info = cases[i].rod == 255 ? types->waterMonsInfo : types->fishingMonsInfo;
        EXPECT(info != NULL);
        for (u32 lure = 0; lure < 2; lure++)
        {
            u32 seen = 0;
            VarSet(VAR_REPEL_STEP_COUNT, lure ? REPEL_LURE_MASK | 100 : 0);
            for (u32 seed = 0; seed < 512; seed++)
            {
                SeedRng(seed);
                u32 roll = Random() % 100;
                u32 expected = 0;
                while (roll >= cases[i].weights[expected])
                    roll -= cases[i].weights[expected++];
                if (lure && Random() % 10 < 2)
                    expected = cases[i].count - 1 - expected;
                rng_value_t after = gRngValue;
                SeedRng(seed);
                u32 actual;
                if (cases[i].rod == 255)
                    actual = ChooseWildMonIndex_Water(info);
                else
                {
                    static const u8 starts[] = {0, 2, 5};
                    actual = ChooseWildMonIndex_Fishing(info, cases[i].rod) - starts[cases[i].rod];
                }
                EXPECT_EQ(actual, expected);
                EXPECT_EQ(memcmp(&gRngValue, &after, sizeof(after)), 0);
                seen |= 1 << actual;
            }
            EXPECT_EQ(seen, (1 << cases[i].count) - 1);
        }
    }
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
}

TEST("Wild rarity: Sweet Scent honors local weights instead of global defaults")
{
    const struct WildPokemon mons[] = {
        {30, 30, SPECIES_FRILLISH}, {30, 30, SPECIES_DRAGALGE},
        {30, 30, SPECIES_LAPRAS}, {30, 30, SPECIES_DHELMISE},
    };
    static const u8 bounds[] = {50, 80, 82, 100};
    const struct WildPokemonInfo info = {.wildPokemon = mons, .encounterBounds = bounds};
    u32 counts[4] = {0};
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        u32 index = ChooseSweetScentWildMonIndex(&info, WILD_AREA_WATER);
        EXPECT_LT(index, 4);
        counts[index]++;
    }
    EXPECT_EQ(counts[0], 2);
    EXPECT_EQ(counts[1], 18);
    EXPECT_EQ(counts[2], 50);
    EXPECT_EQ(counts[3], 30);
}

TEST("Wild rarity: early Dreepy uses local land odds and Sweet Scent reverses them")
{
    const struct WildPokemonHeader *header = gWildMonHeaders;
    while (header->mapGroup != MAP_GROUP(MAP_UNDEFINED)
        && (header->mapGroup != MAP_GROUP(MAP_ROUTE116) || header->mapNum != MAP_NUM(MAP_ROUTE116)))
        header++;
    EXPECT_NE(header->mapGroup, MAP_GROUP(MAP_UNDEFINED));
    const struct WildPokemonInfo *info = header->encounterTypes[TIME_OF_DAY_DEFAULT].landMonsInfo;
    EXPECT(info != NULL);
    u8 weights[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    for (u32 slot = 0; slot < NUM_LAND_MONS_ENCOUNTER_SLOTS; slot++)
        weights[slot] = GetWildSlotOdds(info, WILD_AREA_LAND, slot);
    EXPECT_EQ(weights[0], 20); // The most common resident sets Dreepy's reversed share.
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    for (u32 seed = 0; seed < 512; seed++)
    {
        SeedRng(seed);
        u32 roll = Random() % 100;
        u32 expected = 0;
        while (roll >= weights[expected])
            roll -= weights[expected++];
        SeedRng(seed);
        EXPECT_EQ(ChooseWildMonIndex_Land(info), expected);
    }
    u32 dreepy = 0;
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        u32 index = ChooseSweetScentWildMonIndex(info, WILD_AREA_LAND);
        dreepy += info->wildPokemon[index].species == SPECIES_DREEPY;
    }
    // The rarest resident gets the most common species'20% under Sweet Scent.
    EXPECT_EQ(dreepy, 20);
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
}

extern bool32 Test_TryGenerateSecondWildMon(const struct WildPokemonInfo *info, enum WildPokemonArea area, u8 flags);

TEST("Wild doubles: rejected second encounter preserves the first without duplicating it")
{
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    for (u32 i = 0; i < ARRAY_COUNT(mons); i++)
        mons[i] = (struct WildPokemon){2, 2, SPECIES_MAGIKARP};
    const struct WildPokemonInfo info = {.encounterRate = 20, .wildPokemon = mons};
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    u8 savedGroup = gSaveBlock1Ptr->location.mapGroup;
    u8 savedMap = gSaveBlock1Ptr->location.mapNum;
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_LITTLEROOT_TOWN);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_LITTLEROOT_TOWN);
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    VarSet(VAR_REPEL_STEP_COUNT, 100);
    static const enum WildPokemonArea areas[] = {WILD_AREA_LAND, WILD_AREA_WATER, WILD_AREA_ROCKS};
    for (u32 i = 0; i < ARRAY_COUNT(areas); i++)
    {
        ZeroEnemyPartyMons();
        CreateWildMon(SPECIES_PIKACHU, 5);
        struct Pokemon first = gParties[B_TRAINER_OPPONENT_A][0];
        EXPECT(!Test_TryGenerateSecondWildMon(&info, areas[i], WILD_CHECK_REPEL));
        EXPECT_EQ(memcmp(&first, &gParties[B_TRAINER_OPPONENT_A][0], sizeof(first)), 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_SPECIES), SPECIES_NONE);
        // On success keep the established order: newly generated first, original second.
        EXPECT(Test_TryGenerateSecondWildMon(&info, areas[i], 0));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_MAGIKARP);
        EXPECT_EQ(memcmp(&first, &gParties[B_TRAINER_OPPONENT_A][1], sizeof(first)), 0);
    }
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
    gSaveBlock1Ptr->location.mapGroup = savedGroup;
    gSaveBlock1Ptr->location.mapNum = savedMap;
    ZeroEnemyPartyMons();
    ZeroPlayerPartyMons();
    CalculatePlayerPartyCount();
}

static const u16 sLegendCaughtVars[] = {
    VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static void ClearLegendCaughtBits(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sLegendCaughtVars); i++)
        VarSet(sLegendCaughtVars[i], 0);
}

TEST("Legendary wild slots: gated and caught slots reroll; live slots spawn at the cap with a set")
{
    bool8 saved[ARRAY_COUNT(sWildCapFlags)];
    SaveAndClearWildCapFlags(saved);
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    ZeroPlayerPartyMons();
    ClearLegendCaughtBits();
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS];
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i] = (struct WildPokemon){5, 5, SPECIES_ZIGZAGOON};
    mons[NUM_LAND_MONS_ENCOUNTER_SLOTS - 1].species = SPECIES_RAIKOU; // Badge 3 gate.
    const struct WildPokemonInfo land = {.encounterRate = 20, .wildPokemon = mons};

    // Gate closed: the Raikou slot is inert and passes to the next slot.
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_RAIKOU));
    for (u32 seed = 0; seed < 512; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&land, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    }

    // Gate open: the slot spawns at the cap with a competitive set, while
    // ordinary slots keep the wild level floor.
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    FlagSet(FLAG_BADGE03_GET);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_RAIKOU));
    u32 raikou = 0;
    for (u32 seed = 0; seed < 512; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&land, WILD_AREA_LAND, 0));
        struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
        if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_RAIKOU)
        {
            raikou++;
            EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
            if (GetEmeraldChampionsRawBattleSetCount(SPECIES_RAIKOU) != 0)
                EXPECT_EQ(GetMonData(mon, MON_DATA_SPEED_IV), MAX_PER_STAT_IVS);
        }
        else
        {
            // A level-5 table at cap 40 is floored to within 12 of the cap.
            u32 level = GetMonData(mon, MON_DATA_LEVEL);
            EXPECT(level >= GetCurrentLevelCap() - 12 && level <= GetCurrentLevelCap() - 9);
        }
    }
    // A default 4% slot, boosted x5 (capped +20 points): about a fifth.
    EXPECT_GT(raikou, 64);
    EXPECT_LT(raikou, 160);

    // Caught: the slot is inert again.
    MarkLegendarySignCaughtBySpecies(SPECIES_RAIKOU);
    for (u32 seed = 0; seed < 512; seed++)
    {
        SeedRng(seed);
        EXPECT(TryGenerateWildMon(&land, WILD_AREA_LAND, 0));
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    }
    // A table made only of inert legends yields no encounter.
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = SPECIES_RAIKOU;
    SeedRng(1);
    EXPECT(!TryGenerateWildMon(&land, WILD_AREA_LAND, 0));

    // Paradox slots are not legend slots: no gate, and the ordinary wild
    // level floor (not the cap) applies to their table level.
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i] = (struct WildPokemon){9, 9, SPECIES_IRON_LEAVES};
    SeedRng(2);
    EXPECT(TryGenerateWildMon(&land, WILD_AREA_LAND, 0));
    u32 paradoxLevel = GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL);
    EXPECT(paradoxLevel >= GetCurrentLevelCap() - 12 && paradoxLevel <= GetCurrentLevelCap() - 9);

    ClearLegendCaughtBits();
    RestoreWildCapFlags(saved);
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
    ZeroEnemyPartyMons();
}

static u32 CountSweetScentSlot(const struct WildPokemonInfo *info, u32 slot)
{
    u32 hits = 0;
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        hits += ChooseSweetScentWildMonIndex(info, WILD_AREA_WATER) == slot;
    }
    return hits;
}

TEST("Sweet Scent: live legend slots get a storm's share, Ultra Beasts five times, capped at half")
{
    bool8 saved[ARRAY_COUNT(sWildCapFlags)];
    SaveAndClearWildCapFlags(saved);
    ClearLegendCaughtBits();
    struct WildPokemon mons[NUM_WATER_MONS_ENCOUNTER_SLOTS] = {
        {5, 5, SPECIES_MAGIKARP}, {5, 5, SPECIES_GOLDEEN}, {5, 5, SPECIES_TENTACOOL}, {5, 5, SPECIES_SHAYMIN},
    };
    static const u8 onePercent[] = {60, 90, 99, 100};
    static const u8 threePercent[] = {60, 87, 97, 100};
    static const u8 heavy[] = {40, 60, 80, 100};
    struct WildPokemonInfo info = {.wildPokemon = mons, .encounterBounds = onePercent};

    // A 1% Legendary slot becomes 25%, a storm's share.
    EXPECT_EQ(CountSweetScentSlot(&info, 3), 25);
    // Caught: inert, and the ordinary reversal takes every outcome.
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    EXPECT_EQ(CountSweetScentSlot(&info, 3), 0);
    // Gated: Cobalion waits for Badge 2, so its slot is inert too.
    mons[3].species = SPECIES_COBALION;
    EXPECT_EQ(CountSweetScentSlot(&info, 3), 0);
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    EXPECT_EQ(CountSweetScentSlot(&info, 3), 25);

    // A 3% Ultra Beast slot becomes 15%.
    mons[3].species = SPECIES_POIPOLE;
    info.encounterBounds = threePercent;
    EXPECT_EQ(CountSweetScentSlot(&info, 3), 15);

    // 60% of legend odds would boost to 300%: scaled down to 50%, shared
    // by table odds, and the other half stays with the ordinary species.
    ClearLegendCaughtBits();
    mons[1].species = SPECIES_SHAYMIN;
    mons[2].species = SPECIES_MELTAN;
    info.encounterBounds = heavy;
    u32 legends = 0;
    for (u32 slot = 1; slot < NUM_WATER_MONS_ENCOUNTER_SLOTS; slot++)
    {
        u32 hits = CountSweetScentSlot(&info, slot);
        EXPECT_GT(hits, 0);
        legends += hits;
    }
    EXPECT_EQ(legends, 50);
    EXPECT_EQ(CountSweetScentSlot(&info, 0), 50);

    ClearLegendCaughtBits();
    RestoreWildCapFlags(saved);
}

TEST("Scripted legendary encounters take the cap and their authored or random set")
{
    ZeroPlayerPartyMons();
    // Authored set: Moltres ignores the script level and item.
    CreateScriptedWildMon(SPECIES_MOLTRES, 5, ITEM_LEFTOVERS);
    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
    EXPECT_EQ(GetMonAbility(mon), ABILITY_FLAME_BODY);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1), MOVE_FLAMETHROWER);

    // Unauthored: a random non-Mega set at the cap.
    CreateScriptedWildMon(SPECIES_COBALION, 5, ITEM_NONE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        EXPECT_NE(GetMonData(mon, MON_DATA_MOVE1 + slot), MOVE_NONE);

    // A script item survives when no set supplies one.
    enum Species presetless = SPECIES_NONE;
    static const enum Species candidates[] = {SPECIES_COSMOG, SPECIES_MELTAN, SPECIES_KUBFU, SPECIES_TYPE_NULL, SPECIES_POIPOLE};
    for (u32 i = 0; i < ARRAY_COUNT(candidates) && presetless == SPECIES_NONE; i++)
        if (GetEmeraldChampionsRawBattleSetCount(candidates[i]) == 0)
            presetless = candidates[i];
    if (presetless != SPECIES_NONE)
    {
        CreateScriptedWildMon(presetless, 5, ITEM_ORAN_BERRY);
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
        EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_ORAN_BERRY);
    }

    // Ordinary species keep the script's level and item.
    CreateScriptedWildMon(SPECIES_PIKACHU, 7, ITEM_ORAN_BERRY);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 7);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_ORAN_BERRY);

    // Island events use CreateEventLegalEnemyMon with the same rule.
    gSpecialVar_0x8004 = SPECIES_MEW;
    gSpecialVar_0x8005 = 30;
    gSpecialVar_0x8006 = ITEM_NONE;
    CreateEventLegalEnemyMon();
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_MEW);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(gSpecialVar_0x8005, 30);
    gSpecialVar_0x8004 = SPECIES_PIKACHU;
    gSpecialVar_0x8005 = 7;
    gSpecialVar_0x8006 = ITEM_ORAN_BERRY;
    CreateEventLegalEnemyMon();
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 7);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_ORAN_BERRY);
    ZeroEnemyPartyMons();
}

TEST("Wild roamers: the roaming Lati starts at the current cap with its authored set")
{
    bool8 saved[ARRAY_COUNT(sWildCapFlags)];
    SaveAndClearWildCapFlags(saved);
    FlagSet(FLAG_IS_CHAMPION);
    DeactivateAllRoamers();
    gSpecialVar_0x8004 = 0;
    InitRoamer();
    EXPECT(gSaveBlock1Ptr->roamer[0].active);
    EXPECT_EQ(gSaveBlock1Ptr->roamer[0].species, SPECIES_LATIAS);
    EXPECT_EQ(gSaveBlock1Ptr->roamer[0].level, GetCurrentLevelCap());
    CreateRoamerMonInstance(0);
    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_SOUL_DEW);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP));
    DeactivateAllRoamers();
    RestoreWildCapFlags(saved);
    ZeroEnemyPartyMons();
}

// Data contract for src/data/wild_encounters.json (Emerald maps only):
// Legendary-class slots are exactly 1%, Ultra Beast and Paradox slots 2-3%,
// no ordinary slot is below 2%, a table holds at most two Legendary-class
// slots, and each method (each rod) totals 100.
TEST("Wild tables: Legendary, Ultra Beast and Paradox slot odds follow the rarity ladder")
{
    static const struct { enum WildPokemonArea area; u8 count; } methods[] = {
        {WILD_AREA_LAND, NUM_LAND_MONS_ENCOUNTER_SLOTS},
        {WILD_AREA_WATER, NUM_WATER_MONS_ENCOUNTER_SLOTS},
        {WILD_AREA_ROCKS, NUM_ROCK_SMASH_MONS_ENCOUNTER_SLOTS},
        {WILD_AREA_FISHING, NUM_FISHING_MONS_ENCOUNTER_SLOTS},
        {WILD_AREA_HONEY, NUM_HONEY_MONS_ENCOUNTER_SLOTS},
    };
    u32 tables = 0, legendSlots = 0, failures = 0;
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        const struct WildPokemonHeader *wild = &gWildMonHeaders[header];
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(wild->mapGroup, wild->mapNum);
        if (map->mapLayout != NULL && map->mapLayout->isFrlg)
            continue;
        for (u32 time = 0; time < TIMES_OF_DAY_COUNT; time++)
        {
            const struct WildEncounterTypes *types = &wild->encounterTypes[time];
            const struct WildPokemonInfo *infos[] = {types->landMonsInfo, types->waterMonsInfo,
                types->rockSmashMonsInfo, types->fishingMonsInfo, types->honeyMonsInfo};
            for (u32 method = 0; method < ARRAY_COUNT(methods); method++)
            {
                const struct WildPokemonInfo *info = infos[method];
                if (info == NULL)
                    continue;
                tables++;
                u32 legends = 0, totals[3] = {0};
                for (u32 slot = 0; slot < methods[method].count; slot++)
                {
                    enum Species species = info->wildPokemon[slot].species;
                    u32 odds = GetWildSlotOdds(info, methods[method].area, slot);
                    bool32 ok;
                    // Each rod is its own method.
                    totals[methods[method].area != WILD_AREA_FISHING ? 0 : slot < 2 ? 0 : slot < 5 ? 1 : 2] += odds;
                    switch (GetRestrictedPartyClass(species))
                    {
                    case RESTRICTED_PARTY_LEGENDARY:
                        legends++;
                        legendSlots++;
                        ok = odds == 1;
                        break;
                    case RESTRICTED_PARTY_ULTRA_BEAST:
                    case RESTRICTED_PARTY_PARADOX:
                        ok = odds >= 2 && odds <= 3;
                        break;
                    default:
                        ok = odds >= 2;
                        break;
                    }
                    if (!ok)
                    {
                        Test_MgbaPrintf("Slot odds: map %d.%d method %d slot %d species %d odds %d",
                            wild->mapGroup, wild->mapNum, method, slot, species, odds);
                        failures++;
                    }
                }
                bool32 fishing = methods[method].area == WILD_AREA_FISHING;
                if (legends > 2 || totals[0] != 100
                 || (fishing && (totals[1] != 100 || totals[2] != 100)))
                {
                    Test_MgbaPrintf("Table: map %d.%d method %d legends %d totals %d/%d/%d",
                        wild->mapGroup, wild->mapNum, method, legends, totals[0], totals[1], totals[2]);
                    failures++;
                }
            }
        }
    }
    Test_MgbaPrintf("Wild tables checked=%d legend slots=%d failures=%d", tables, legendSlots, failures);
    EXPECT_GT(tables, 0);
    EXPECT_GT(legendSlots, 0);
    EXPECT_EQ(failures, 0);
}
