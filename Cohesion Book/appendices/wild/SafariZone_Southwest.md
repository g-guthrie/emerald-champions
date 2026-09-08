# SafariZone_Southwest — wild distribution

**Decision: KEEP.** Swamp insects, Croagunk and Goomy families provide a coherent alternative Safari ecosystem. Keep their evolved catches and independent fishing choices.

[World pathways and interactions](../../world/maps/SafariZone_Southwest.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L28910)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_KARRABLAST|13%|25–25|
|1|SPECIES_SHELMET|12%|27–27|
|2|SPECIES_PALPITOAD|11%|25–25|
|3|SPECIES_CROAGUNK|10%|27–27|
|4|SPECIES_GOOMY|10%|25–25|
|5|SPECIES_ARBOK|8%|27–27|
|6|SPECIES_ACCELGOR|8%|25–25|
|7|SPECIES_ESCAVALIER|7%|27–27|
|8|SPECIES_SLIGGOO|6%|40–40|
|9|SPECIES_TOXICROAK|5%|37–37|
|10|SPECIES_GOODRA|5%|50–50|
|11|SPECIES_SEISMITOAD|5%|36–36|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SLOWPOKE|35%|50–55|
|1|SPECIES_GOLDUCK|25%|50–55|
|2|SPECIES_FLOATZEL|18%|50–55|
|3|SPECIES_SEISMITOAD|12%|50–55|
|4|SPECIES_FEEBAS|10%|50–55|

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
|2|SPECIES_BASCULIN|45%|50–55|
|3|SPECIES_LOMBRE|30%|50–55|
|4|SPECIES_QUAGSIRE|25%|50–55|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_OCTILLERY|30%|55–60|
|6|SPECIES_GOLDUCK|25%|55–60|
|7|SPECIES_SEAKING|20%|55–60|
|8|SPECIES_WHISCASH|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

