# CaveOfOrigin_UnusedRubySapphireMap1 — wild distribution

**Decision: KEEP.** Despite its legacy name, this is part of the expanded connected chamber sequence. Preserve mineral/dragon continuity and verify the actual warp route rather than excluding it by name.

[World pathways and interactions](../../world/maps/CaveOfOrigin_UnusedRubySapphireMap1.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L1636)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SABLEYE|13%|30–30|
|1|SPECIES_MAWILE|12%|31–31|
|2|SPECIES_CARBINK|11%|32–32|
|3|SPECIES_BOLDORE|10%|30–30|
|4|SPECIES_NOIVERN|10%|48–48|
|5|SPECIES_DRUDDIGON|8%|34–34|
|6|SPECIES_GLIMMET|8%|33–33|
|7|SPECIES_TINKATINK|7%|34–34|
|8|SPECIES_GABITE|6%|34–34|
|9|SPECIES_DURALUDON|5%|35–35|
|10|SPECIES_GOODRA|5%|50–50|
|11|SPECIES_DRAKLOAK|5%|50–50|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

