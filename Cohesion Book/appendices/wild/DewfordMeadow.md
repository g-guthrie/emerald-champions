# DewfordMeadow — wild distribution

**Decision: KEEP.** Keep butterflies, redirection/support candidates and Pheromosa. Early power is intentional ammunition, not a reason for automatic delay; test the actual nearby battles rather than infer imbalance from rarity labels.

[World pathways and interactions](../../world/maps/DewfordMeadow.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L3449)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CUTIEFLY|13%|41–43|
|1|SPECIES_FLABEBE|12%|41–43|
|2|SPECIES_COMBEE|11%|41–43|
|3|SPECIES_BUTTERFREE|10%|41–43|
|4|SPECIES_ORICORIO|10%|41–43|
|5|SPECIES_BEAUTIFLY|8%|41–43|
|6|SPECIES_PHEROMOSA|8%|41–43|
|7|SPECIES_RIBOMBEE|7%|41–43|
|8|SPECIES_VOLBEAT|6%|41–43|
|9|SPECIES_ILLUMISE|5%|41–43|
|10|SPECIES_SWIRLIX|5%|41–43|
|11|SPECIES_SPRITZEE|5%|41–43|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|41–43|
|SPECIES_SWIRLIX|41–43|
|SPECIES_SPRITZEE|41–43|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

