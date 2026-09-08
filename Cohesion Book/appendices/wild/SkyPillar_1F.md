# SkyPillar_1F — wild distribution

**Decision: KEEP.** Ancient Ghosts, minerals and dragons support the tower's identity. Preserve the ready team options and the distinction between first story ascent and later exploration conditions.

[World pathways and interactions](../../world/maps/SkyPillar_1F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L36784)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SABLEYE|13%|33–33|
|1|SPECIES_MAWILE|12%|34–34|
|2|SPECIES_BANETTE|11%|37–37|
|3|SPECIES_DUSCLOPS|10%|37–37|
|4|SPECIES_CLAYDOL|10%|36–36|
|5|SPECIES_ALTARIA|8%|37–37|
|6|SPECIES_GOLURK|8%|43–43|
|7|SPECIES_NOIVERN|7%|48–48|
|8|SPECIES_DRUDDIGON|6%|37–37|
|9|SPECIES_MINIOR|5%|38–38|
|10|SPECIES_BAGON|5%|37–37|
|11|SPECIES_METANG|5%|38–38|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

