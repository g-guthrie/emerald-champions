# GraniteCave_B2F — wild distribution

**Decision: KEEP.** Duraludon, Deino and Tinkatink reward deeper exploration. Keep the ordinary table separate from Cobalion's landmark and Rock Smash access; neither is established by seeing a species in another method.

[World pathways and interactions](../../world/maps/GraniteCave_B2F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L8490)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SABLEYE|13%|13–15|
|1|SPECIES_MAWILE|12%|13–15|
|2|SPECIES_ARON|11%|13–15|
|3|SPECIES_ONIX|10%|13–15|
|4|SPECIES_CARBINK|10%|13–15|
|5|SPECIES_BRONZOR|8%|13–15|
|6|SPECIES_CUBONE|8%|13–15|
|7|SPECIES_NOSEPASS|7%|13–15|
|8|SPECIES_TINKATINK|6%|13–15|
|9|SPECIES_ROLYCOLY|5%|13–15|
|10|SPECIES_DURALUDON|5%|13–15|
|11|SPECIES_DEINO|5%|13–15|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_NOSEPASS|35%|22–25|
|1|SPECIES_DWEBBLE|25%|22–25|
|2|SPECIES_SHUCKLE|18%|22–25|
|3|SPECIES_BINACLE|12%|22–25|
|4|SPECIES_CARBINK|10%|22–25|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

