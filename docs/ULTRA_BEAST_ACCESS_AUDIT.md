# Ultra Beast acquisition audit — September 19, 2026

Native source coverage for all ten catchable Ultra Beast acquisition entries; Naganadel is obtained by evolution. No species, odds, teams or AI changed in this pass.

| Species | Authored location | Source rule |
|---|---|---|
| BLACEPHALON | EMBER_PATH | Native rare resident, 3% while eligible; one capture |
| BUZZWOLE | ASHEN_WOODS | Native rare resident, 3% while eligible; one capture |
| CELESTEELA | ROUTE120 | Native rare resident, 3% while eligible; one capture |
| GUZZLORD | ALTERING_CAVE_B1F | Native rare resident, 3% while eligible; one capture |
| KARTANA | PETALBURG_WOODS_3 | Native rare resident, 3% while eligible; one capture |
| NIHILEGO | UNDERWATER_SEAFLOOR_CAVERN | Native rare resident, 3% while eligible; one capture |
| PHEROMOSA | DEWFORD_MEADOW | Native rare resident, 3% while eligible; one capture |
| POIPOLE | ALTERING_CAVE_B1F | Native rare resident, 3% while eligible; one capture |
| STAKATAKA | ROUTE111_RUINS_EXTERIOR | Native rare resident, 3% while eligible; one capture |
| XURKITREE | NEW_MAUVILLE_INSIDE | Native rare resident, 3% while eligible; one capture |

## Verified

- Every destination has a main-table land encounter roster. Underwater Seafloor Cavern uses its twelve-slot seabed roster through UsesLandEncounterTable, so Nihilego matches the native selector habitat.
- Native test exhausts all 100 ordinary probability rolls for each species and checks exactly three appearances; after marking capture, all 100 rolls exclude that species.
- Naganadel evolution rejects Poipole without Dragon Pulse and accepts it with Dragon Pulse. The move is present in Poipole's level-one learnset; existing tutor/Leveler interfaces supply the preparation route.
- Map definitions provide incoming entrances for every destination. This establishes graph connectivity only, not traversability without the appropriate keys/HMs or story flags.
- New Mauville entrance explicitly requires ITEM_BASEMENT_KEY before changing its closed-door tiles and setting VAR_NEW_MAUVILLE_STATE. Route111 desert entry checks Go-Goggles. Other full gate chains remain to trace.

## Evidence and remaining work

test/ultra_beast_access.c and work/ultra-access-20260919/tests.log pass two native groups. Source inventory: work/shoal-sighting-20260919/ultra-beast-sources.json. No production code changed, so a new normal ROM build is unnecessary for this test-only pass.

Still required: start-to-destination gate traversal and earliest acquisition milestones; suitability of these powerful species for the authored campaign at that milestone; encounter level behavior, reward value, local dialogue and discoverability; rare-wild prepared sets versus native wild learnsets; complete battle-mechanics audit. This report does not claim the full distribution or game is finished.
