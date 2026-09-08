# Route115 — wild distribution

**Decision: KEEP.** The northern grass roster supports useful families but is not on the same initial path as southern Seaspray access. Preserve the species and explicitly separate those physical areas in guidance.

[World pathways and interactions](../../world/maps/Route115.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L19768)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_JIGGLYPUFF|13%|16–16|
|1|SPECIES_TAILLOW|12%|16–16|
|2|SPECIES_TANGELA|11%|16–16|
|3|SPECIES_PANCHAM|10%|16–16|
|4|SPECIES_SKWOVET|10%|16–16|
|5|SPECIES_SAWK|8%|16–16|
|6|SPECIES_FARFETCHD|8%|16–16|
|7|SPECIES_SNUBBULL|7%|16–16|
|8|SPECIES_FOMANTIS|6%|16–16|
|9|SPECIES_CLEFFA|5%|16–16|
|10|SPECIES_SPRITZEE|5%|16–16|
|11|SPECIES_MINIOR|5%|16–16|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_KRABBY|35%|50–55|
|1|SPECIES_BARBARACLE|25%|50–55|
|2|SPECIES_SHELLOS|18%|50–55|
|3|SPECIES_WAILORD|12%|50–55|
|4|SPECIES_MANTINE|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|15–20|
|1|SPECIES_SHELLDER|40%|15–20|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_STARYU|45%|25–30|
|3|SPECIES_CLAMPERL|30%|25–30|
|4|SPECIES_OCTILLERY|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_CLAWITZER|30%|55–60|
|6|SPECIES_DRAGALGE|25%|55–60|
|7|SPECIES_SHARPEDO|20%|55–60|
|8|SPECIES_LUVDISC|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|16–16|
|SPECIES_AROMATISSE|16–16|
|SPECIES_NOIBAT|16–16|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

