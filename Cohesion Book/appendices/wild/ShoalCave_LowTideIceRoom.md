# ShoalCave_LowTideIceRoom — wild distribution

**Decision: KEEP.** Alolan Vulpix, Hisuian Sneasel, Iron Bundle and distinct forms give the room fresh value despite earlier Ice access. Preserve its meaningful discoveries and Mega rewards.

[World pathways and interactions](../../world/maps/ShoalCave_LowTideIceRoom.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L34071)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SNORUNT|13%|26–26|
|1|SPECIES_SPHEAL|12%|26–26|
|2|SPECIES_SNEASEL|11%|28–28|
|3|SPECIES_VULPIX_ALOLA|10%|28–28|
|4|SPECIES_SNOVER|10%|30–30|
|5|SPECIES_CETODDLE|8%|30–30|
|6|SPECIES_IRON_BUNDLE|8%|32–32|
|7|SPECIES_ARCTIBAX|7%|35–35|
|8|SPECIES_EISCUE|6%|32–32|
|9|SPECIES_SNEASEL_HISUI|5%|32–32|
|10|SPECIES_MR_MIME_GALAR|5%|32–32|
|11|SPECIES_SNOM|5%|32–32|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

