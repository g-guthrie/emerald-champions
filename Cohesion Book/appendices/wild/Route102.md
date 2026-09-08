# Route102 — wild distribution

**Decision: KEEP.** Lotad/Seedot/Ralts/Surskit remain recognizable and Happiny/Togepi offer support choices. Preserve the roster so the first expert doubles battles have multiple available answers.

[World pathways and interactions](../../world/maps/Route102.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L17444)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LOTAD|13%|3–3|
|1|SPECIES_SEEDOT|12%|3–3|
|2|SPECIES_BIDOOF|11%|4–4|
|3|SPECIES_MARILL|10%|4–4|
|4|SPECIES_RALTS|10%|3–3|
|5|SPECIES_SURSKIT|8%|4–4|
|6|SPECIES_KRICKETOT|8%|3–3|
|7|SPECIES_HAPPINY|7%|3–3|
|8|SPECIES_NYMBLE|6%|4–4|
|9|SPECIES_LECHONK|5%|4–4|
|10|SPECIES_NATU|5%|4–4|
|11|SPECIES_TOGEPI|5%|3–3|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_POLIWAG|35%|50–55|
|1|SPECIES_MASQUERAIN|25%|50–55|
|2|SPECIES_LOMBRE|18%|50–55|
|3|SPECIES_AZUMARILL|12%|50–55|
|4|SPECIES_CHEWTLE|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|15–20|
|1|SPECIES_GOLDEEN|40%|15–20|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_CORPHISH|45%|25–30|
|3|SPECIES_OCTILLERY|30%|25–30|
|4|SPECIES_PSYDUCK|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_DREDNAW|30%|55–60|
|6|SPECIES_SHARPEDO|25%|55–60|
|7|SPECIES_LOMBRE|20%|55–60|
|8|SPECIES_QUAGSIRE|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|3–4|
|SPECIES_BIDOOF|3–4|
|SPECIES_GOTHITA|3–4|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

