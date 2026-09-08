# SafariZone_North — wild distribution

**Decision: KEEP.** Imported regional forms, Kangaskhan, Tauros and Scyther make a strong collection destination. Preserve the roster and actual bike-area access; normal species collection remains distinct from the nursery Mega reward.

[World pathways and interactions](../../world/maps/SafariZone_North.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L28027)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_GLOOM|13%|27–27|
|1|SPECIES_WEEPINBELL|12%|27–27|
|2|SPECIES_DONPHAN|11%|29–29|
|3|SPECIES_MAROWAK_ALOLA|10%|29–29|
|4|SPECIES_HERACROSS|10%|27–27|
|5|SPECIES_KANGASKHAN|8%|29–29|
|6|SPECIES_DUGTRIO_ALOLA|8%|31–31|
|7|SPECIES_CUFANT|7%|29–29|
|8|SPECIES_EXEGGUTOR_ALOLA|6%|29–29|
|9|SPECIES_RHYHORN|5%|27–27|
|10|SPECIES_TAUROS|5%|31–31|
|11|SPECIES_SCYTHER|5%|29–29|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_GRAVELER_ALOLA|35%|25–25|
|1|SPECIES_RHYHORN|25%|5–10|
|2|SPECIES_RHYDON|18%|42–42|
|3|SPECIES_STEELIX|12%|20–25|
|4|SPECIES_CARBINK|10%|20–25|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

