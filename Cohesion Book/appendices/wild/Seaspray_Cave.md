# Seaspray_Cave — wild distribution

**Decision: KEEP.** The early side cave supplies aquatic, Ground and unusual resistance options. Keep its current broad access; do not assume Rock Smash objects block every route to the lower floor.

[World pathways and interactions](../../world/maps/Seaspray_Cave.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L31986)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_PSYDUCK|13%|10–12|
|1|SPECIES_ZUBAT|12%|10–12|
|2|SPECIES_WOOPER|11%|10–12|
|3|SPECIES_TYNAMO|10%|10–12|
|4|SPECIES_KRABBY|10%|10–12|
|5|SPECIES_CHINCHOU|8%|10–12|
|6|SPECIES_FRILLISH|8%|10–12|
|7|SPECIES_STUNFISK_GALAR|7%|10–12|
|8|SPECIES_WOOBAT|6%|10–12|
|9|SPECIES_STUNFISK|5%|10–12|
|10|SPECIES_DWEBBLE|5%|10–12|
|11|SPECIES_WISHIWASHI|5%|10–12|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BINACLE|35%|50–55|
|1|SPECIES_CLAMPERL|25%|50–55|
|2|SPECIES_SHELLOS|18%|50–55|
|3|SPECIES_WAILORD|12%|50–55|
|4|SPECIES_MANTINE|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BINACLE|35%|10–15|
|1|SPECIES_DWEBBLE|25%|5–10|
|2|SPECIES_KRABBY|18%|15–20|
|3|SPECIES_CORSOLA|12%|15–20|
|4|SPECIES_RELICANTH|10%|15–20|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_SHELLDER|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_KRABBY|45%|25–30|
|3|SPECIES_CARVANHA|30%|25–30|
|4|SPECIES_LUVDISC|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_BRUXISH|30%|55–60|
|6|SPECIES_STARYU|25%|55–60|
|7|SPECIES_QWILFISH|20%|55–60|
|8|SPECIES_CLAMPERL|15%|55–60|
|9|SPECIES_MAREANIE|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

