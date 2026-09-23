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
            u16 map = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
            u32 nativeCount = 0;
            for (u32 id = 0; id < LEGENDARY_SIGN_COUNT; id++)
                nativeCount += gLegendarySignDefinitions[id].mapId == map
                    && gLegendarySignDefinitions[id].source == LEGENDARY_SOURCE_NATIVE_WILD;
            EXPECT(nativeCount <= NUM_LAND_MONS_ENCOUNTER_SLOTS);
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
            EXPECT_EQ(GetMonData(&expected[0], MON_DATA_LEVEL), encounters[first].level);
            EXPECT_EQ(GetMonData(&expected[0], MON_DATA_HELD_ITEM), encounters[first].item);
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
    static const u8 weights[] = {20, 18, 10, 10, 9, 9, 5, 5, 2, 4, 4, 4};
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
