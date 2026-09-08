# Route114 — wild distribution

**Decision: KEEP.** Swablu, Lombre/Nuzleaf and Zangoose/Seviper retain identity with varied visiting families. Keep the mixed roster and the separate river/rock methods that reward return exploration.

[World pathways and interactions](../../world/maps/Route114.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L19564)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SWABLU|13%|16–16|
|1|SPECIES_LOMBRE|12%|16–16|
|2|SPECIES_NUZLEAF|11%|17–17|
|3|SPECIES_ZANGOOSE|10%|15–15|
|4|SPECIES_SEVIPER|10%|15–15|
|5|SPECIES_PHANPY|8%|16–16|
|6|SPECIES_SKORUPI|8%|16–16|
|7|SPECIES_DUCKLETT|7%|18–18|
|8|SPECIES_SKIDDO|6%|17–17|
|9|SPECIES_STUFFUL|5%|15–15|
|10|SPECIES_GOOMY|5%|17–17|
|11|SPECIES_JANGMO_O|5%|15–15|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BUIZEL|35%|50–55|
|1|SPECIES_BASCULIN|25%|50–55|
|2|SPECIES_SHELLOS|18%|50–55|
|3|SPECIES_POLIWHIRL|12%|50–55|
|4|SPECIES_DRATINI|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_GEODUDE|35%|10–15|
|1|SPECIES_GRAVELER|25%|25–25|
|2|SPECIES_NOSEPASS|18%|15–20|
|3|SPECIES_DONPHAN|12%|25–25|
|4|SPECIES_PROBOPASS|10%|15–20|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_CORPHISH|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_WHISCASH|45%|35–40|
|3|SPECIES_GOLDUCK|30%|35–40|
|4|SPECIES_LOMBRE|25%|35–40|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SEAKING|30%|55–60|
|6|SPECIES_BASCULIN|25%|55–60|
|7|SPECIES_LANTURN|20%|55–60|
|8|SPECIES_POLIWHIRL|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|15–18|
|SPECIES_QUAGSIRE|20–20|
|SPECIES_DONPHAN|25–25|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

