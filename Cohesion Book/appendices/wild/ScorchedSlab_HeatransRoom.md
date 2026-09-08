# ScorchedSlab_HeatransRoom — wild distribution

**Decision: KEEP.** The furnace roster and Heatran interaction fit the room. Keep physical-reveal and capture states distinct; the misleading name of a flag alone is not a defect.

[World pathways and interactions](../../world/maps/ScorchedSlab_HeatransRoom.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L30076)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGCARGO|13%|41–41|
|1|SPECIES_MAGMAR|12%|42–42|
|2|SPECIES_DUGTRIO|11%|43–43|
|3|SPECIES_BOLDORE|10%|41–41|
|4|SPECIES_TURTONATOR|10%|42–42|
|5|SPECIES_HEATMOR|8%|43–43|
|6|SPECIES_ZWEILOUS|8%|50–50|
|7|SPECIES_COALOSSAL|7%|42–42|
|8|SPECIES_DARMANITAN|6%|43–43|
|9|SPECIES_DRUDDIGON|5%|41–41|
|10|SPECIES_SLUGMA|5%|42–42|
|11|SPECIES_LARVESTA|5%|43–43|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

