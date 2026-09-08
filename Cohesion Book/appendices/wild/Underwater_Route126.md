# Underwater_Route126 — wild distribution

**Decision: KEEP.** Clamperl's branches, Relicanth, Milotic and other deep residents make this underwater pocket distinct. Keep its variation and actual grass/terrain encounter access.

[World pathways and interactions](../../world/maps/Underwater_Route126.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L38478)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CLAMPERL|13%|34–38|
|1|SPECIES_CORSOLA|12%|34–38|
|2|SPECIES_HUNTAIL|11%|38–42|
|3|SPECIES_GOREBYSS|10%|38–42|
|4|SPECIES_FEEBAS|10%|34–38|
|5|SPECIES_CARBINK|8%|34–38|
|6|SPECIES_ALOMOMOLA|8%|38–42|
|7|SPECIES_BRUXISH|7%|38–42|
|8|SPECIES_RELICANTH|6%|38–42|
|9|SPECIES_CURSOLA|5%|38–42|
|10|SPECIES_MILOTIC|5%|40–44|
|11|SPECIES_PALAFIN|5%|40–44|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

