# Route116 — wild distribution

**Decision: KEEP.** Nincada, Skitty and the recent babies coexist with strong unevolved choices and Dreepy. Preserve all of them; low-level late bloomers are valid player choices rather than a pacing defect.

[World pathways and interactions](../../world/maps/Route116.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L19942)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_NINCADA|13%|8–10|
|1|SPECIES_MIME_JR|12%|8–10|
|2|SPECIES_SKITTY|11%|8–10|
|3|SPECIES_NICKIT|10%|8–10|
|4|SPECIES_THROH|10%|8–10|
|5|SPECIES_PURRLOIN|8%|8–10|
|6|SPECIES_RIOLU|8%|8–10|
|7|SPECIES_MUNCHLAX|7%|8–10|
|8|SPECIES_STARLY|6%|8–10|
|9|SPECIES_JOLTIK|5%|8–10|
|10|SPECIES_ROOKIDEE|5%|8–10|
|11|SPECIES_DREEPY|5%|8–10|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_GEODUDE|35%|10–15|
|1|SPECIES_SPINARAK|25%|5–10|
|2|SPECIES_NOSEPASS|18%|15–16|
|3|SPECIES_PINECO|12%|15–16|
|4|SPECIES_SUDOWOODO|10%|15–16|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|8–10|
|SPECIES_PANCHAM|8–10|
|SPECIES_NICKIT|8–10|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

