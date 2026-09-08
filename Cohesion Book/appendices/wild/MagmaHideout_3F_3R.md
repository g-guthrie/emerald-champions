# MagmaHideout_3F_3R — wild distribution

**Decision: KEEP.** This chamber retains unevolved Larvesta alongside developed Fire species. Keep the choice of immediate readiness or a preferred family; the Leveler removes level-grind pressure.

[World pathways and interactions](../../world/maps/MagmaHideout_3F_3R.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L9280)

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
|4|SPECIES_NOIBAT|10%|29–29|
|5|SPECIES_HOUNDOOM|8%|30–30|
|6|SPECIES_MAGMAR|8%|30–30|
|7|SPECIES_TURTONATOR|7%|30–30|
|8|SPECIES_SALAZZLE|6%|33–33|
|9|SPECIES_HEATMOR|5%|31–31|
|10|SPECIES_LARVESTA|5%|32–32|
|11|SPECIES_CHANDELURE|5%|33–33|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

