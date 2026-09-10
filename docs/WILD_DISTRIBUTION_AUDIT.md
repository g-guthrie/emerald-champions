# Wild distribution audit — September 8, 2026

**Historical baseline before the approved habitat revision.** The counts,
rosters and generator findings below describe the pre-revision source. The user
subsequently authorized habitat changes and stronger early Fighting choices.
See [the implemented revision](WILD_HABITAT_REVISION_2026_09_08.md) for current
rosters, preserved early discoveries and verification. Do not regenerate the
historical populations from this audit or the Cohesion Book.

This audit evaluates the current game sources, not the Cohesion Book as a design authority. The current distribution is exceptionally broad and its declared probabilities are consistent. Its weaknesses are uneven habitat identity, misleading access/timing evidence, and a lack of battle evidence showing that all those choices serve the campaign. A blanket redistribution would not repair the reported battle AI or freezes.

This audit removed an unused false clue: the orphaned Petalburg Woods 2 ranger script claimed that Virizion required a fully grown Breloom, while the current Sign definition requires the Knuckle Badge and no species. No current map object or script called the ranger, so this is stale-source cleanup, not a claim of fixing dialogue a player could reach. The water generator's introductory documentation was also corrected, and its unused obsolete probability constant removed. No encounter species, probabilities, levels, progression requirements, or rewards were rebalanced.

## Evidence and scope

Inputs: `src/data/wild_encounters.json`, registered map JSON and scripts, `src/wild_encounter.c`, the native header generator, species/evolution data, `src/caps.c`, current legendary definitions, and water authoring. Ordinary encounters are the four fields actually declared in the main encounter group: land, water, rocks, fishing. The three rods are separate methods.

The Makefile generates `src/data/wild_encounters.h` from the JSON; `src/wild_encounter.c` includes that header. The book is not a build input. This is a static source audit; it does not claim an inspected fresh ROM, a completed map traversal, capture rates measured in an emulator, or battle difficulty results.

Measured inventory:

| Quantity | Result |
| --- | ---: |
| Registered Hoenn maps with encounter rows | 138 |
| Main-group Hoenn rows | 146 |
| Native method tables, including selector alternatives | 264 |
| Separately counted methods, splitting rods | 380 |
| Tables with Altering Cave at its default selector | 256 |
| Methods with Altering Cave at its default selector | 372 |
| Distinct direct species/form identifiers | 640 |
| Hoenn rows retaining inactive Hidden data | 29 |
| Species present in Hidden but absent from ordinary slots | 5 |

The eight extra rows are alternatives to the default Altering Cave population, not eight extra places. All nine alternatives together add **zero** species to the regionwide default-selector union. “640” counts direct species/form identifiers, not 640 evolution families, not all obtainable Pokémon, and not proof of physical access.

The existing distribution check passes: **264 active encounter tables have valid slots, levels and species probabilities**. All 58 active Surf tables and 58 fishing tables agree with their current authored generator output. An in-memory execution of the generator's land placement, pinned restoration, evolution-floor, deduplication and seafloor passes changed **zero** current rows. The book's final ordinary slot catalogue also differs from the current source at **zero** compared slots. That verifies implementation agreement, not design quality.

## Findings requiring design decisions

### 1. More choices is not automatically more strategy

Seven core opening land maps — Routes 101, 102, 103, 104 and 116, Petalburg Woods, and Rusturf Tunnel — already contain **83 distinct direct species** across 84 slots. That excludes the deeper forest, Dewford, water, gifts and starters.

This is not a lack-of-options opening. Adding more species is unlikely to fix the first battles. The important questions are which options provide useful actions at the opening cap, how clearly preparation explains them, and whether the opponents reliably execute their own strategy. Those require native battle tests against the available player tools.

The opening has useful fast evolutions and babies, alongside long investments:

| Available choice | Current ordinary route / normal slot chance | Practical distinction |
| --- | --- | --- |
| Happiny | Route 102, 7% | Early access is present; its evolution conditions still matter. |
| Munchlax | Route 116, 7% | Friendship evolution, not a late-game wild Snorlax substitute. |
| Mime Jr. | Route 116, 12%; Dewford Manor also | Evolution needs Mimic; finding it is not identical to having Mr. Mime. |
| Chingling | Rusturf Tunnel, 5% | Friendship/night requirement. |
| Mantyke | Dewford Old Rod, 40% | Early fishing is the actual method; evolving requires Remoraid. |
| Caterpie / Wurmple | Petalburg Woods, 10% / 11% | Their level-10 final forms fit the opening cap. |
| Mienfoo | Route 104, 11% | Useful unevolved option; Mienshao is level 50. |
| Ferroseed | Petalburg Woods, 5% | Useful unevolved defensive option; Ferrothorn is level 40. |
| Dreepy | Route 116, 5% | Drakloak is level 50 and Dragapult level 60. |
| Bagon | Rusturf Tunnel, 5% | Shelgon is level 30 and Salamence level 50. |
| Beldum | Granite Cave | Metang is level 20 and Metagross level 45; not part of the seven-map opening count. |

Do not remove Dreepy, Bagon or Beldum solely because they evolve late. Conversely, do not count their final forms as early strategic coverage. The free preparation system can change their practical usefulness, so normal main-series learnsets alone are not adequate balance evidence.

### 2. Universal twelve-species land tables flatten identity

With the default Altering Cave selector, **112 of 113 land tables contain exactly 12 distinct species**. Altering Cave alone contains one. The historical generator explicitly fills duplicates from a generic list.

The two Artisan Cave floors have exactly the same twelve-species roster: Smeargle, Woobat, Gligar, Sableye, Mawile, Aron, Carbink, Doduo, Girafarig, Wobbuffet, Pinsir and Heracross. This closely follows the generic filler list. It preserves access but provides no floor-to-floor discovery and weakens Smeargle's signature role.

That is an editorial weakness, not a broken table. The prior user direction explicitly preserved broad Rusturf/Artisan/Mirage choice. A future pass should choose anchor species and meaningful floor distinctions while preserving displaced families elsewhere, rather than mechanically restoring monotypic caves or deleting options to satisfy a quota.

### 3. Water has regional structure but considerable repetition

Magikarp occurs in **57 of the 58 active fishing maps**; Route 117's Paldean Wooper override is the exception. Petalburg City's Old Rod is two Magikarp slots, therefore **100% Magikarp**, not 60% plus another discovery.

Most geographically widespread ordinary species include Octillery (32 maps), Sharpedo (29), Dragalge (28), Bruxish (26), Dhelmise (24), Luvdisc and Lanturn (23 each). These counts aggregate methods within a map. Magikarp's repetition supports a familiar baseline; repeating Dhelmise across 24 maps makes it less location-specific.

The authored hunt fields are preferences, not guaranteed last-slot species: duplicate avoidance can replace them. Active examples include Meteor Falls B1F 1R Surf (authored Dratini, final Chewtle), Route 107 Surf (Corsola, final Wiglett), Route 111 Surf (Chewtle, final Dratini), Routes 129/134 Super Rod (Relicanth, final Basculegion), Seafloor Room 7 Super Rod (Hisuian Qwilfish, final Basculegion), and Shoal entrance Super Rod (Hisuian Qwilfish, final Cetoddle). Their final tables agree with current book authoring. Changing this generator behavior would change agreed populations; it was not silently “fixed.”

A future water pass should assess discoveries across adjacent routes and across rods, not merely ensure every table differs. Reducing repetition can increase distinctiveness but can also make useful tools less convenient.

### 4. Runtime levels do not enforce evolution timing

`TryGenerateWildMon` and fishing clamp a selected Pokémon's **level** to the live cap. They do not replace its **species** with a pre-evolution. A numeric source-level audit therefore cannot establish usable form timing.

Concrete example: Dewford Meadow's land slots are authored at levels 41–43, but first-visit cap-20 encounters become level 20. This includes Ribombee, although Cutiefly normally evolves at level 25. The meadow also supplies Pheromosa at 8%; the legendary ordinary-wild definition has no badge/species prerequisite and uses the current cap. Petalburg Woods 3 supplies Kartana at 8% with the same absence of a special progression requirement. Their physical map access still matters.

This is not an instruction to postpone either Ultra Beast: the current direction deliberately allowed exciting early strength. It is evidence that the existing “evolution-floor” and timing tools cannot prove balance. Decide explicitly whether under-level evolved wild forms are acceptable and play the affected early battles before changing these encounters.

## Habitat appraisal

| Habitat | What the actual roster supports | Main concern / next decision |
| --- | --- | --- |
| Opening meadows / woods | Babies, quick bugs, Grass, birds and flexible unevolved options | Choice volume is already high; prioritize clear useful roles over more additions. |
| Rusturf / Granite | Whismur, rock/digging families, bats, Abra/Timburr and early long-term projects | Broad choice is intentional. Do not treat all high-evolution families as ready final forms. |
| Deep Petalburg forest | Plants, insects, nocturnal/haunted species, Kartana | Physical depth and its early high-power outlier need battle context. |
| Dewford Meadow / Manor | Pollinators, fairies, ghosts and Pheromosa | Source levels substantially overstate first-visit encounter levels. |
| Route 110 / New Mauville | Industrial species and the power-station Magnemite anchor | Keep power-station identity; Surf/key access must be checked separately from city access. |
| Route 111 / Mirage / Sandstrewn | Sand, burrowing species, relics, constructs and Paradox anchors | Strong shared theme, but successive floors repeat many of the same families. |
| Fiery Path / Ember / Ashen | Fire/industrial insects, volcanic residents and differentiated strong discoveries | Assess readily usable weather tools alongside actual trainer strategies. |
| Meteor Falls | Moon/sun stones, dragons, minerals and Steven's metallic final chamber | Good identity; entrance vs Waterfall/deeper access cannot share one timing label. |
| Mt. Pyre | Duskull line, Ghost families, antique/artisan forms and ghostly regional forms | Strong identity; early floors overlap substantially but later evolved/rare forms distinguish them. |
| Shoal | Ice families and regional forms, cold-water residents | High tide uses low-tide map IDs; do not invent missing headers from layout names. |
| Safari | Distinct regional forms, Kangaskhan, Chansey, trade-evolution choices | Six active Hoenn subareas; inherited Kanto Safari tables must not inflate coverage. |
| Dive | Reef/tank/parasite forms, Dondozo, Tatsugiri and deeper exceptional finds | Three separate submerged populations are meaningful; “land” here means underwater walking. |
| Seafloor Cavern | Dark/deep-water pressure with a different small cave resident per room | Many shared evolved water families; varied single slots alone do not guarantee exploration interest. |
| Victory Road | High-stage dragons/steel and powerful final discoveries | Distinct climax, but proximity to the end reduces time to use new investments. |
| Artisan Cave | Smeargle plus broad utility roster | Both floors are identical and generic; improve identity only with explicit availability tradeoffs. |

Magnemite's unevolved ordinary encounters remain New Mauville; the Duskull family's Mt. Pyre identity is intact, with some evolved-family distribution elsewhere. Dondozo is underwater-only in the ordinary tables: Route 124 and the submerged Seafloor entrance. All three Tatsugiri forms occur in the latter. Their ordinary land-table chances are Dondozo 10%, curly Tatsugiri 10%, droopy 8% and stretchy 6%. A prior document's shorthand about “two 10% slots” is incomplete for the expanded form roster.

## Access, acquisition gaps and Mega timing

There is no evidence here of a broken ordinary-table family route that justifies arbitrary replacement. The five Hidden-only direct forms are Hisuian Arcanine, Aromatisse, Hitmonchan, Hitmonlee and Slaking. Ordinary ancestors are available (Hisuian Growlithe, Spritzee, Tyrogue and Slakoth), so they are **not** five proven acquisition gaps. Their evolution conditions still require separate review.

`DEXNAV_ENABLED` is false and Hidden is not among the main native group's declared fields. Hidden entries cannot close an ordinary roster gap. The route-sign function already suppresses that method when disabled.

Method access comes before table arithmetic:

- Old Rod is the Dewford fisherman gift. Good Rod has attached fisherman gifts on Routes 114 and 118 in the current checkout. Super Rod is Mossdeep House 3. A gift's map name alone does not establish the earliest physical route to it.
- Surf, Dive, Waterfall, Rock Smash, story objects and map-specific obstacles can delay a method within an otherwise reachable map.
- Boat travel past a route does not establish access to its grass. Route 115's northern grass and the southern cave entrance are not one access case.
- A graph of only warp events and map connections omits scripted movement, Dive links and conditional entrances. This audit rejected that graph's apparent “unreachable” reports instead of calling working maps broken.
- The Ruby/Sapphire-named Cave of Origin rooms are actually connected through the current 1F warp to Diancie's room. Their names do not establish that they are unused.

`scripts/audit/mega_stone_timing.py` has been corrected to be a **candidate-evidence inventory, not an exact progression audit**. The former output incorrectly assigned rods, Surf and disabled Hidden to a generic map cap and compared stones with an ancestor's presence as if the evolved user were ready. It now excludes Hidden and non-default selector alternatives, splits all three rod methods and reports their gift-script references, and reads Surf/Rock Smash/Dive badge and received-HM-license prerequisites from the current native field-move definitions. These gates are separate from authored map caps; broad cap fallbacks are removed. Physical access, evolution readiness and usable Mega timing remain explicitly **UNKNOWN**. Gifts/trades/Signs are labelled limited script or definition references, not proven reachable acquisitions. Only the actual starter table supplies starter evidence, not unrelated evolved-species helpers in the same file.

Verification ran the corrected tool and checked its live evidence in memory: Dewford Old Rod includes Mantyke and requires Old Rod; Good/Super Rod retain their distinct item gates; Dewford Surf requires badge 5, its HM license and an eligible field-move user; Beldum is a Metagross-family candidate, not a ready Mega Metagross. The ordinary evidence reproduces 138 maps, 372 methods and 640 species/form identifiers. The stone scan finds 73 overworld objects and zero sparkle/item mismatches. No encounter, stone or evolution data changed. This tool still does not prove complete acquisition routes, all possible stone sources, exact evolution conditions or a usable-Mega date, and its output must not justify moving stones by itself.

Usable Mega timing is the latest of the Ring, the stone's physical access, and the evolved user's availability. Steven's script requires the Knuckle Badge before giving the Ring. Therefore the Butterfrenite and Lopunnite pickups in Petalburg Woods are advance discoveries, not opening Mega access. Beldum does not make Mega Metagross ready: Metagross needs level 45 unless separately caught evolved. Early Bagon does not make Mega Salamence ready: its ordinary evolution requires level 50. These are source-backed prerequisites, not a complete timing certification of all stone routes.

## Probability and engine checks

Current slot weights are:

| Method | Slot percentages |
| --- | --- |
| Land | 13, 12, 11, 10, 10, 8, 8, 7, 6, 5, 5, 5 |
| Surf / Rock Smash | 35, 25, 18, 12, 10 |
| Old Rod | 60, 40 |
| Good Rod | 45, 30, 25 |
| Super Rod | 30, 25, 20, 15, 10 |

All ordinary species aggregates meet the 5% minimum under both normal and reversed slot order. Sweet Scent reverses selection; active Lures sometimes reverse selection. Ability attraction, encounter frequency, Repel, caught/gated Sign rejection, fishing success and Feebas's separate Route 119 mechanism are additional conditions. Thus these are baseline slot chances, not per-step or guaranteed capture rates.

Ordinary doubles chance is zero. The scripted opening doubles does not imply double wild captures. Sweet Scent's ordinary land and water branches already check failed generation before starting battle and its wrapper restores the inversion flag. The audit did not reproduce or newly fix a stale-party Sweet Scent battle.

The water generator previously advertised obsolete 1% hunts and stronger level guarantees than runtime provides. Its top-level explanation now describes the current weights and evidence limits. No generator behavior was changed.

## What still needs actual play

1. Native early-battle trials using realistic opening choices, including Pheromosa/Kartana where physically accessible, compared with less extreme viable teams.
2. First-visit captures at method/obstacle boundaries: Dewford meadow/rod, Route 115 grass, deeper forest, New Mauville, Meteor Falls, Shoal tides and Dive.
3. A stone-by-stone physical timing pass joined to actual evolution conditions and the Ring; do not reuse generic map caps as proof.
4. Editorial decisions on generic filler floors and water repetition with preserved acquisition routes.

The appendix enumerates every registered Hoenn encounter map and method from current source. Counts show unique direct species within a method, not catchability, species quality or progression certification. Altering Cave uses the default row; the eight selector alternatives are accounted for above. No new regression framework or design quota was added.

## Complete source method inventory

| Map | Land | Surf | Rocks | Old / Good / Super |
| --- | ---: | ---: | ---: | --- |
| ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS | — | 5 | — | 2 / 3 / 5 |
| ABANDONED_SHIP_ROOMS_B1F | — | 5 | — | 2 / 3 / 5 |
| ALTERING_CAVE | 1 | — | — | — |
| ALTERING_CAVE_1F | 12 | — | 5 | — |
| ALTERING_CAVE_B1F | 12 | 5 | 5 | 2 / 3 / 5 |
| ARTISAN_CAVE_1F | 12 | — | — | — |
| ARTISAN_CAVE_B1F | 12 | — | — | — |
| ASHEN_WOODS | 12 | — | — | — |
| CAVE_OF_ORIGIN_1F | 12 | — | — | — |
| CAVE_OF_ORIGIN_DIANCIES_ROOM | 12 | — | — | — |
| CAVE_OF_ORIGIN_ENTRANCE | 12 | — | — | — |
| CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1 | 12 | — | — | — |
| CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP2 | 12 | — | — | — |
| CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3 | 12 | — | — | — |
| DESERT_UNDERPASS | 12 | — | — | — |
| DEWFORD_MANOR_1F | 12 | — | — | — |
| DEWFORD_MEADOW | 12 | — | — | — |
| DEWFORD_TOWN | — | 5 | 5 | 2 / 3 / 5 |
| EMBER_PATH | 12 | — | — | — |
| EVER_GRANDE_CITY | — | 5 | — | 2 / 3 / 5 |
| FIERY_PATH | 12 | — | — | — |
| GRANITE_CAVE_1F | 12 | — | — | — |
| GRANITE_CAVE_B1F | 12 | — | — | — |
| GRANITE_CAVE_B2F | 12 | — | 5 | — |
| GRANITE_CAVE_STEVENS_ROOM | 12 | — | — | — |
| JAGGED_PASS | 12 | — | — | — |
| LILYCOVE_CITY | — | 5 | 5 | 2 / 3 / 5 |
| MAGMA_HIDEOUT_1F | 12 | — | — | — |
| MAGMA_HIDEOUT_2F_1R | 12 | — | — | — |
| MAGMA_HIDEOUT_2F_2R | 12 | — | — | — |
| MAGMA_HIDEOUT_2F_3R | 12 | — | — | — |
| MAGMA_HIDEOUT_3F_1R | 12 | — | — | — |
| MAGMA_HIDEOUT_3F_2R | 12 | — | — | — |
| MAGMA_HIDEOUT_3F_3R | 12 | — | — | — |
| MAGMA_HIDEOUT_4F | 12 | — | — | — |
| METEOR_FALLS_1F_1R | 12 | 5 | — | 2 / 3 / 5 |
| METEOR_FALLS_1F_2R | 12 | 5 | — | 2 / 3 / 5 |
| METEOR_FALLS_B1F_1R | 12 | 5 | — | 2 / 3 / 5 |
| METEOR_FALLS_B1F_2R | 12 | 5 | — | 2 / 3 / 5 |
| METEOR_FALLS_STEVENS_CAVE | 12 | — | — | — |
| MIRAGE_TOWER_1F | 12 | — | — | — |
| MIRAGE_TOWER_2F | 12 | — | — | — |
| MIRAGE_TOWER_3F | 12 | — | — | — |
| MIRAGE_TOWER_4F | 12 | — | — | — |
| MIRAGE_TOWER_B1F | 12 | — | — | — |
| MOSSDEEP_CITY | — | 5 | — | 2 / 3 / 5 |
| MT_PYRE_1F | 12 | — | — | — |
| MT_PYRE_2F | 12 | — | — | — |
| MT_PYRE_3F | 12 | — | — | — |
| MT_PYRE_4F | 12 | — | — | — |
| MT_PYRE_5F | 12 | — | — | — |
| MT_PYRE_6F | 12 | — | — | — |
| MT_PYRE_EXTERIOR | 12 | — | — | — |
| MT_PYRE_SUMMIT | 12 | — | — | — |
| NEW_MAUVILLE_ENTRANCE | 12 | — | — | — |
| NEW_MAUVILLE_INSIDE | 12 | — | — | — |
| PACIFIDLOG_TOWN | — | 5 | — | 2 / 3 / 5 |
| PETALBURG_CITY | — | 5 | — | 1 / 3 / 5 |
| PETALBURG_WOODS | 12 | — | 5 | — |
| PETALBURG_WOODS_2 | 12 | — | 5 | — |
| PETALBURG_WOODS_3 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE101 | 12 | — | — | — |
| ROUTE102 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE103 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE104 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE105 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE106 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE107 | — | 5 | — | 2 / 3 / 5 |
| ROUTE108 | — | 5 | — | 2 / 3 / 5 |
| ROUTE109 | — | 5 | 5 | 2 / 3 / 5 |
| ROUTE110 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE111 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE111_RUINS_EXTERIOR | 12 | — | — | — |
| ROUTE112 | 12 | — | — | — |
| ROUTE113 | 12 | — | — | — |
| ROUTE114 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE115 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE116 | 12 | — | 5 | — |
| ROUTE117 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE118 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE119 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE120 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE121 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE122 | — | 5 | — | 2 / 3 / 5 |
| ROUTE123 | 12 | 5 | 5 | 2 / 3 / 5 |
| ROUTE124 | — | 5 | — | 2 / 3 / 5 |
| ROUTE125 | — | 5 | — | 2 / 3 / 5 |
| ROUTE126 | — | 5 | — | 2 / 3 / 5 |
| ROUTE127 | — | 5 | — | 2 / 3 / 5 |
| ROUTE128 | — | 5 | — | 2 / 3 / 5 |
| ROUTE129 | — | 5 | — | 2 / 3 / 5 |
| ROUTE130 | 12 | 5 | — | 2 / 3 / 5 |
| ROUTE131 | — | 5 | — | 2 / 3 / 5 |
| ROUTE132 | — | 5 | — | 2 / 3 / 5 |
| ROUTE133 | — | 5 | — | 2 / 3 / 5 |
| ROUTE134 | — | 5 | — | 2 / 3 / 5 |
| RUSTURF_TUNNEL | 12 | — | — | — |
| SAFARI_ZONE_NORTH | 12 | — | 5 | — |
| SAFARI_ZONE_NORTHEAST | 12 | — | 5 | — |
| SAFARI_ZONE_NORTHWEST | 12 | 5 | — | 2 / 3 / 5 |
| SAFARI_ZONE_SOUTH | 12 | — | — | — |
| SAFARI_ZONE_SOUTHEAST | 12 | 5 | — | 2 / 3 / 5 |
| SAFARI_ZONE_SOUTHWEST | 12 | 5 | — | 2 / 3 / 5 |
| SANDSTREWN_RUINS | 12 | 5 | 5 | 2 / 3 / 5 |
| SANDSTREWN_RUINS_2F | 12 | — | 5 | — |
| SANDSTREWN_RUINS_3F | 12 | — | 5 | — |
| SANDSTREWN_RUINS_B1F | 12 | — | 5 | — |
| SCORCHED_SLAB_B1F | 12 | 5 | — | 2 / 3 / 5 |
| SCORCHED_SLAB_B2F | 12 | — | — | — |
| SCORCHED_SLAB_HEATRANS_ROOM | 12 | — | — | — |
| SEAFLOOR_CAVERN_ENTRANCE | — | 5 | — | 2 / 3 / 5 |
| SEAFLOOR_CAVERN_ROOM1 | 12 | — | — | — |
| SEAFLOOR_CAVERN_ROOM2 | 12 | — | — | — |
| SEAFLOOR_CAVERN_ROOM3 | 12 | — | — | — |
| SEAFLOOR_CAVERN_ROOM4 | 12 | — | — | — |
| SEAFLOOR_CAVERN_ROOM5 | 12 | — | — | — |
| SEAFLOOR_CAVERN_ROOM6 | 12 | 5 | — | 2 / 3 / 5 |
| SEAFLOOR_CAVERN_ROOM7 | 12 | 5 | — | 2 / 3 / 5 |
| SEAFLOOR_CAVERN_ROOM8 | 12 | — | — | — |
| SEASPRAY_CAVE | 12 | 5 | 5 | 2 / 3 / 5 |
| SEASPRAY_CAVE_B1F | 12 | — | — | — |
| SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM | 12 | 5 | — | 2 / 3 / 5 |
| SHOAL_CAVE_LOW_TIDE_ICE_ROOM | 12 | — | — | — |
| SHOAL_CAVE_LOW_TIDE_INNER_ROOM | 12 | 5 | — | 2 / 3 / 5 |
| SHOAL_CAVE_LOW_TIDE_LOWER_ROOM | 12 | — | — | — |
| SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM | 12 | — | — | — |
| SKY_PILLAR_1F | 12 | — | — | — |
| SKY_PILLAR_3F | 12 | — | — | — |
| SKY_PILLAR_5F | 12 | — | — | — |
| SLATEPORT_CITY | — | 5 | — | 2 / 3 / 5 |
| SOOTOPOLIS_CITY | — | 5 | — | 2 / 3 / 5 |
| UNDERWATER_ROUTE124 | 12 | — | — | — |
| UNDERWATER_ROUTE126 | 12 | — | — | — |
| UNDERWATER_SEAFLOOR_CAVERN | 12 | — | — | — |
| VERDANTURF_MEADOW | 12 | — | — | — |
| VICTORY_ROAD_1F | 12 | — | — | — |
| VICTORY_ROAD_B1F | 12 | — | 5 | — |
| VICTORY_ROAD_B2F | 12 | 5 | — | 2 / 3 / 5 |
