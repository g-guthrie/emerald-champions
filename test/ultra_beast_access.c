#include "global.h"
#include "pokemon.h"
#include "event_data.h"
#include "legendary_signs.h"
#include "wild_encounter.h"
#include "caps.h"
#include "constants/maps.h"
#include "random.h"
#include "test/test.h"

static const u16 sUltraBeastCaughtVars[] = {VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5};

static const struct WildPokemonInfo *FindUltraBeastLandTable(u16 map)
{
    for (u32 i = 0; gWildMonHeaders[i].mapGroup != MAP_GROUP(MAP_UNDEFINED); i++)
        if (gWildMonHeaders[i].mapGroup == MAP_GROUP(map) && gWildMonHeaders[i].mapNum == MAP_NUM(map))
            return gWildMonHeaders[i].encounterTypes[TIME_OF_DAY_DEFAULT].landMonsInfo;
    return NULL;
}

TEST("Ultra Beast access: every habitat has a 2-3% gated slot that spawns at the cap and closes on capture")
{
    static const struct { enum Species species; u16 map; } habitats[] = {
        {SPECIES_POIPOLE, MAP_SEASPRAY_CAVE_B1F},
        {SPECIES_BLACEPHALON, MAP_EMBER_PATH},
        {SPECIES_BUZZWOLE, MAP_ASHEN_WOODS},
        {SPECIES_KARTANA, MAP_PETALBURG_WOODS_3},
        {SPECIES_STAKATAKA, MAP_ROUTE111_RUINS_EXTERIOR},
        {SPECIES_PHEROMOSA, MAP_DEWFORD_MEADOW},
        {SPECIES_CELESTEELA, MAP_ROUTE120},
        {SPECIES_XURKITREE, MAP_NEW_MAUVILLE_INSIDE},
        {SPECIES_NIHILEGO, MAP_UNDERWATER_SEAFLOOR_CAVERN},
        {SPECIES_GUZZLORD, MAP_ALTERING_CAVE_B1F},
    };
    bool8 badges[NUM_BADGES];
    bool8 wattson = FlagGet(FLAG_GOT_TM24_FROM_WATTSON);
    u16 savedRepel = VarGet(VAR_REPEL_STEP_COUNT);
    VarSet(VAR_REPEL_STEP_COUNT, 0);
    ZeroPlayerPartyMons();
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        badges[badge] = FlagGet(FLAG_BADGE01_GET + badge);
    for (u32 i = 0; i < ARRAY_COUNT(habitats); i++)
    {
        enum Species species = habitats[i].species;
        for (u32 v = 0; v < ARRAY_COUNT(sUltraBeastCaughtVars); v++)
            VarSet(sUltraBeastCaughtVars[v], 0);
        EXPECT_EQ(GetRestrictedPartyClass(species), RESTRICTED_PARTY_ULTRA_BEAST);
        EXPECT(GetLegendarySignIdBySpecies(species) < LEGENDARY_SIGN_COUNT);
        const struct WildPokemonInfo *land = FindUltraBeastLandTable(habitats[i].map);
        EXPECT(land != NULL);
        u32 odds = 0;
        for (u32 slot = 0; slot < NUM_LAND_MONS_ENCOUNTER_SLOTS; slot++)
            if (land->wildPokemon[slot].species == species)
                odds += GetWildSlotOdds(land, WILD_AREA_LAND, slot);
        Test_MgbaPrintf("Ultra Beast %d on map %d: %d percent", species, habitats[i].map, odds);
        EXPECT(odds >= 2 && odds <= 3);

        // Open every gate: all badges and the one milestone flag in use.
        for (u32 badge = 0; badge < NUM_BADGES; badge++)
            FlagSet(FLAG_BADGE01_GET + badge);
        FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
        EXPECT(CanAcquireLegendarySignSpecies(species));
        u32 hits = 0;
        for (u32 seed = 0; seed < 2048 && hits == 0; seed++)
        {
            SeedRng(seed);
            EXPECT(TryGenerateWildMon(land, WILD_AREA_LAND, 0));
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
            if (GetMonData(mon, MON_DATA_SPECIES) != species)
                continue;
            hits++;
            EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
        }
        EXPECT_GT(hits, 0);

        MarkLegendarySignCaughtBySpecies(species);
        EXPECT(!CanAcquireLegendarySignSpecies(species));
        for (u32 seed = 0; seed < 512; seed++)
        {
            SeedRng(seed);
            EXPECT(TryGenerateWildMon(land, WILD_AREA_LAND, 0));
            EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), species);
        }
        for (u32 badge = 0; badge < NUM_BADGES; badge++)
            FlagClear(FLAG_BADGE01_GET + badge);
    }
    for (u32 v = 0; v < ARRAY_COUNT(sUltraBeastCaughtVars); v++)
        VarSet(sUltraBeastCaughtVars[v], 0);
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        if (badges[badge]) FlagSet(FLAG_BADGE01_GET + badge); else FlagClear(FLAG_BADGE01_GET + badge);
    if (wattson) FlagSet(FLAG_GOT_TM24_FROM_WATTSON); else FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    VarSet(VAR_REPEL_STEP_COUNT, savedRepel);
    ZeroEnemyPartyMons();
}

TEST("Ultra Beast access: badge gates keep Ultra Beast slots inert until their milestone")
{
    bool8 badges[NUM_BADGES];
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
    {
        badges[badge] = FlagGet(FLAG_BADGE01_GET + badge);
        FlagClear(FLAG_BADGE01_GET + badge);
    }
    for (u32 v = 0; v < ARRAY_COUNT(sUltraBeastCaughtVars); v++)
        VarSet(sUltraBeastCaughtVars[v], 0);
    // Poipole is the ungated first hint; Buzzwole waits for three badges.
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_POIPOLE));
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_BUZZWOLE));
    EXPECT(!IsWildSlotSpeciesAcquirable(SPECIES_BUZZWOLE));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_BUZZWOLE));
    FlagSet(FLAG_BADGE03_GET);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_BUZZWOLE));
    // Kartana also needs the fifth badge's own flag, not just a count.
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_KARTANA));
    FlagSet(FLAG_BADGE04_GET);
    FlagSet(FLAG_BADGE05_GET);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_KARTANA));
    // Paradox Pokemon have no gate row and are always acquirable.
    EXPECT(IsWildSlotSpeciesAcquirable(SPECIES_IRON_LEAVES));
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        if (badges[badge]) FlagSet(FLAG_BADGE01_GET + badge); else FlagClear(FLAG_BADGE01_GET + badge);
}

TEST("Ultra Beast access: Poipole evolves only with its obtainable Dragon Pulse")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_POIPOLE, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&mon, MOVE_PECK, 0);
    for (u32 slot = 1; slot < MAX_MON_MOVES; slot++)
        SetMonMoveSlot(&mon, MOVE_NONE, slot);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NONE);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(SPECIES_POIPOLE);
    bool32 learnsPulse = FALSE;
    for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
        learnsPulse |= learnset[i].move == MOVE_DRAGON_PULSE;
    EXPECT(learnsPulse);
    SetMonMoveSlot(&mon, MOVE_DRAGON_PULSE, 0);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NAGANADEL);
}
