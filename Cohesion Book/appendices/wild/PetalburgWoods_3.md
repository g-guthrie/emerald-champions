# PetalburgWoods_3 — wild distribution

**Decision: KEEP.** Keep the diverse deep forest and early Kartana option. Its maze, optional water and Rock Smash methods make geography meaningful; dormant Hidden rows are not an extra native encounter method.

[World pathways and interactions](../../world/maps/PetalburgWoods_3.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L14989)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_ODDISH|13%|13–15|
|1|SPECIES_BELLSPROUT|12%|13–15|
|2|SPECIES_YANMA|11%|13–15|
|3|SPECIES_CROAGUNK|10%|13–15|
|4|SPECIES_MISDREAVUS|10%|13–15|
|5|SPECIES_MURKROW|8%|13–15|
|6|SPECIES_KARTANA|8%|13–15|
|7|SPECIES_DEWPIDER|7%|13–15|
|8|SPECIES_EMOLGA|6%|13–15|
|9|SPECIES_PHANTUMP|5%|13–15|
|10|SPECIES_GOOMY|5%|13–15|
|11|SPECIES_IMPIDIMP|5%|13–15|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LOTAD|35%|50–55|
|1|SPECIES_MASQUERAIN|25%|50–55|
|2|SPECIES_SHELLOS|18%|50–55|
|3|SPECIES_POLIWHIRL|12%|50–55|
|4|SPECIES_CHEWTLE|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BEEDRILL|35%|10–15|
|1|SPECIES_TREVENANT|25%|5–10|
|2|SPECIES_PINSIR|18%|15–16|
|3|SPECIES_HERACROSS|12%|15–16|
|4|SPECIES_PINECO|10%|15–16|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|15–20|
|1|SPECIES_POLIWAG|40%|15–20|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_GOLDEEN|45%|25–30|
|3|SPECIES_DREDNAW|30%|25–30|
|4|SPECIES_CARVANHA|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_LANTURN|30%|55–60|
|6|SPECIES_SEISMITOAD|25%|55–60|
|7|SPECIES_OCTILLERY|20%|55–60|
|8|SPECIES_GOLDUCK|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|13–15|
|SPECIES_EMOLGA|13–15|
|SPECIES_GOOMY|13–15|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

