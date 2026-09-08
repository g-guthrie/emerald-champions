# Route118 — wild distribution

**Decision: REVISE.** Linoone/Manectric preserve the familiar river corridor. Replace redundant Raticate/Porygon occurrences with Galarian Zigzagoon and Squawkabilly; their displaced families retain actual other routes.

[World pathways and interactions](../../world/maps/Route118.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L20265)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LINOONE|13%|24–24|
|1|SPECIES_MANECTRIC|12%|26–26|
|2|SPECIES_LICKITUNG|11%|26–26|
|3|SPECIES_RATICATE|10%|26–26|
|4|SPECIES_PASSIMIAN|10%|26–26|
|5|SPECIES_LIEPARD|8%|26–26|
|6|SPECIES_DEDENNE|8%|25–25|
|7|SPECIES_CARNIVINE|7%|25–25|
|8|SPECIES_ZORUA|6%|26–26|
|9|SPECIES_PORYGON|5%|26–26|
|10|SPECIES_DURALUDON|5%|27–27|
|11|SPECIES_KOMALA|5%|25–25|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LINOONE|13%|24–24|
|1|SPECIES_MANECTRIC|12%|26–26|
|2|SPECIES_LICKITUNG|11%|26–26|
|3|SPECIES_ZIGZAGOON_GALAR|10%|26–26|
|4|SPECIES_PASSIMIAN|10%|26–26|
|5|SPECIES_LIEPARD|8%|26–26|
|6|SPECIES_DEDENNE|8%|25–25|
|7|SPECIES_CARNIVINE|7%|25–25|
|8|SPECIES_ZORUA|6%|26–26|
|9|SPECIES_SQUAWKABILLY_GREEN|5%|26–26|
|10|SPECIES_DURALUDON|5%|27–27|
|11|SPECIES_KOMALA|5%|25–25|

- **WILD-01:** Add the missing Galarian raccoon line on the settled river corridor. Ordinary Rattata is available in Dewford Manor and can already evolve; preserve Linoone/Manectric as route anchors.
- **WILD-02:** Add the missing flocking parrot beside a human travel corridor. Porygon retains New Mauville, Safari and other sources; no technological family disappears.

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CARVANHA|35%|50–55|
|1|SPECIES_GOLDUCK|25%|50–55|
|2|SPECIES_BASCULIN|18%|50–55|
|3|SPECIES_SHELLOS|12%|50–55|
|4|SPECIES_TAUROS_PALDEA_AQUA|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_FEAROW|35%|20–20|
|1|SPECIES_VENONAT|25%|10–10|
|2|SPECIES_PINSIR|18%|15–20|
|3|SPECIES_HERACROSS|12%|15–20|
|4|SPECIES_SCYTHER|10%|15–20|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_CARVANHA|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_CHINCHOU|45%|25–30|
|3|SPECIES_PSYDUCK|30%|25–30|
|4|SPECIES_LOMBRE|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SEAKING|30%|55–60|
|6|SPECIES_WHISCASH|25%|55–60|
|7|SPECIES_CRAWDAUNT|20%|55–60|
|8|SPECIES_BASCULIN|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SEAKING|30%|55–60|
|6|SPECIES_WHISCASH|25%|55–60|
|7|SPECIES_CRAWDAUNT|20%|55–60|
|8|SPECIES_BASCULIN_BLUE_STRIPED|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

- **FORM-WILD-05:** Add Blue Basculin and its different ability option through Route118 Super Rod; red Basculin remains on this route by Surf and elsewhere.

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|24–27|
|SPECIES_DEDENNE|24–27|
|SPECIES_DITTO|24–27|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

