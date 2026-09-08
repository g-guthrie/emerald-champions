# MagmaHideout_4F — wild distribution

**Decision: REVISE.** The climax floor gains Gouging Fire by replacing one redundant Magmar occurrence. Magby/Magmar remain available elsewhere; Camerupt and the volcanic identity stay intact.

[World pathways and interactions](../../world/maps/MagmaHideout_4F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L9349)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CAMERUPT|13%|33–33|
|1|SPECIES_MAGCARGO|12%|38–38|
|2|SPECIES_EXCADRILL|11%|31–31|
|3|SPECIES_WEEZING|10%|35–35|
|4|SPECIES_HOUNDOOM|10%|29–29|
|5|SPECIES_TURTONATOR|8%|30–30|
|6|SPECIES_MAGMAR|8%|30–30|
|7|SPECIES_COALOSSAL|7%|34–34|
|8|SPECIES_HEATMOR|6%|30–30|
|9|SPECIES_DARMANITAN|5%|31–31|
|10|SPECIES_VOLCARONA|5%|59–59|
|11|SPECIES_CHANDELURE|5%|33–33|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CAMERUPT|13%|33–33|
|1|SPECIES_MAGCARGO|12%|38–38|
|2|SPECIES_EXCADRILL|11%|31–31|
|3|SPECIES_WEEZING|10%|35–35|
|4|SPECIES_HOUNDOOM|10%|29–29|
|5|SPECIES_TURTONATOR|8%|30–30|
|6|SPECIES_GOUGING_FIRE|8%|30–30|
|7|SPECIES_COALOSSAL|7%|34–34|
|8|SPECIES_HEATMOR|6%|30–30|
|9|SPECIES_DARMANITAN|5%|31–31|
|10|SPECIES_VOLCARONA|5%|59–59|
|11|SPECIES_CHANDELURE|5%|33–33|

- **WILD-19:** Add a missing ancient fire species near the volcanic climax. Magby and Magmar remain in multiple earlier fire habitats.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

