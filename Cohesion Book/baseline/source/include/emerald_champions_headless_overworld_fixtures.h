// Reviewed camera positions for every physical one-off overworld Pokémon.
//
// This is deliberately a macro table: the test-only ROM consumes the map and
// camera coordinates, while the host renderer consumes the stable row/species
// names. The native UI gate independently derives the authoritative object set
// from live map.json files and requires an exact one-to-one match.

EC_HEADLESS_OVERWORLD_FIXTURE(1, MAP_ALTERING_CAVE_B1F,                   SPECIES_MEWTWO,    7, 15)
EC_HEADLESS_OVERWORLD_FIXTURE(2, MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM,        SPECIES_DIANCIE,    9, 11)
EC_HEADLESS_OVERWORLD_FIXTURE(3, MAP_EMBER_PATH,                          SPECIES_MOLTRES,    21, 17)
EC_HEADLESS_OVERWORLD_FIXTURE(4, MAP_METEOR_FALLS_JIRACHIS_ROOM,          SPECIES_JIRACHI,     7,  9)
EC_HEADLESS_OVERWORLD_FIXTURE(5, MAP_NEW_MAUVILLE_INSIDE,                 SPECIES_ZAPDOS,      33, 18)
EC_HEADLESS_OVERWORLD_FIXTURE(6, MAP_SCORCHED_SLAB_HEATRANS_ROOM,         SPECIES_HEATRAN,     10, 15)
EC_HEADLESS_OVERWORLD_FIXTURE(7, MAP_SEALED_CHAMBER_INNER_ROOM,           SPECIES_REGIGIGAS,   10, 16)
EC_HEADLESS_OVERWORLD_FIXTURE(8, MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM,        SPECIES_ARTICUNO,     8, 11)
