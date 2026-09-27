#ifndef GUARD_CONFIG_DEXNAV_H
#define GUARD_CONFIG_DEXNAV_H

#define DEXNAV_ENABLED                TRUE   // Whether or not DexNav is enabled. If TRUE, flags/vars below must all be non-zero
#define USE_DEXNAV_SEARCH_LEVELS      FALSE  /* WARNING: POSSIBLY EXCEEDS SAVEBLOCK SPACE! REQUIRES 1 BYTE PER SPECIES */

// Flag/var defines
#define DN_FLAG_SEARCHING             FLAG_DEXNAV_SEARCHING     // Searching for mon
#define DN_FLAG_DEXNAV_GET            FLAG_RECEIVED_DEXNAV      // DexNav shows in start menu
#define DN_FLAG_DETECTOR_MODE         FLAG_DEXNAV_DETECTOR_MODE // Allow player to find hidden mons
#define DN_VAR_SPECIES                VAR_DEXNAV_SPECIES        // Registered DexNav species
#define DN_VAR_STEP_COUNTER           VAR_DEXNAV_STEP_COUNTER   // Steps for finding hidden Pokémon

// Search parameters
#define SNEAKING_PROXIMITY              4   // Tile amount
#define CREEPING_PROXIMITY              2
#define MAX_PROXIMITY                   20

#define DEXNAV_CHAIN_MAX                100 // maximum chain value

#endif // GUARD_CONFIG_DEXNAV_H
