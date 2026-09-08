# SafariZone_South — wild distribution

**Decision: REVISE.** The accessible imported-fauna section gains Galarian Meowth instead of repeated Porygon. Keep Alolan cats/Raichu, Chansey and the recognizable Safari collection mix.

[World pathways and interactions](../../world/maps/SafariZone_South.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L28687)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WOBBUFFET|13%|25–25|
|1|SPECIES_PIKACHU|12%|27–27|
|2|SPECIES_MEOWTH_ALOLA|11%|25–25|
|3|SPECIES_SMEARGLE|10%|27–27|
|4|SPECIES_GIRAFARIG|10%|25–25|
|5|SPECIES_PERSIAN_ALOLA|8%|28–28|
|6|SPECIES_RAICHU_ALOLA|8%|25–25|
|7|SPECIES_MR_MIME|7%|27–27|
|8|SPECIES_CHANSEY|6%|25–25|
|9|SPECIES_DITTO|5%|27–27|
|10|SPECIES_PORYGON|5%|27–27|
|11|SPECIES_EEVEE|5%|29–29|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WOBBUFFET|13%|25–25|
|1|SPECIES_PIKACHU|12%|27–27|
|2|SPECIES_MEOWTH_ALOLA|11%|25–25|
|3|SPECIES_SMEARGLE|10%|27–27|
|4|SPECIES_GIRAFARIG|10%|25–25|
|5|SPECIES_PERSIAN_ALOLA|8%|28–28|
|6|SPECIES_RAICHU_ALOLA|8%|25–25|
|7|SPECIES_MR_MIME|7%|27–27|
|8|SPECIES_CHANSEY|6%|25–25|
|9|SPECIES_DITTO|5%|27–27|
|10|SPECIES_MEOWTH_GALAR|5%|27–27|
|11|SPECIES_EEVEE|5%|29–29|

- **WILD-03:** The imported-fauna collection gains the missing Galarian Meowth/Perrserker line. New Mauville retains Porygon at the same Surf access tier.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

