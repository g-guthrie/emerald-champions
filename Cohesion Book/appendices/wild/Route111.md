# Route111 — wild distribution

**Decision: KEEP.** Desert residents and Galarian Yamask fit the arid region. Retain Gible and other strong choices; the Dynamo rope, desert goggles and permanent cursed-stone path are separate physical requirements.

[World pathways and interactions](../../world/maps/Route111.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L19113)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SANDSHREW|13%|20–20|
|1|SPECIES_CACNEA|12%|20–20|
|2|SPECIES_MARACTUS|11%|21–21|
|3|SPECIES_BRAMBLIN|10%|19–19|
|4|SPECIES_HIPPOPOTAS|10%|21–21|
|5|SPECIES_GIBLE|8%|19–19|
|6|SPECIES_MASCHIFF|8%|19–19|
|7|SPECIES_RELLOR|7%|20–20|
|8|SPECIES_SILICOBRA|6%|21–21|
|9|SPECIES_HELIOPTILE|5%|20–20|
|10|SPECIES_YAMASK_GALAR|5%|22–22|
|11|SPECIES_STONJOURNER|5%|22–22|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WOOPER|35%|50–55|
|1|SPECIES_WHISCASH|25%|50–55|
|2|SPECIES_DREDNAW|18%|50–55|
|3|SPECIES_CRAWDAUNT|12%|50–55|
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
|3|SPECIES_CARBINK|12%|15–20|
|4|SPECIES_STONJOURNER|10%|15–20|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_BARBOACH|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_GOLDEEN|45%|25–30|
|3|SPECIES_PALPITOAD|30%|25–30|
|4|SPECIES_DREDNAW|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_OCTILLERY|30%|55–60|
|6|SPECIES_GOLDUCK|25%|55–60|
|7|SPECIES_LOMBRE|20%|55–60|
|8|SPECIES_QUAGSIRE|15%|55–60|
|9|SPECIES_DHELMISE|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

