# MirageTower_3F — wild distribution

**Decision: KEEP.** Developed relic Pokémon make the upper floor feel deeper without withholding advanced strategy earlier. Keep its distinct Golurk/Claydol/Brongzong mix and stone interactions.

[World pathways and interactions](../../world/maps/MirageTower_3F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L10241)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CLAYDOL|13%|36–36|
|1|SPECIES_YAMASK|12%|21–21|
|2|SPECIES_GOLURK|11%|43–43|
|3|SPECIES_BRONZONG|10%|33–33|
|4|SPECIES_DARUMAKA|10%|20–20|
|5|SPECIES_SIGILYPH|8%|20–20|
|6|SPECIES_KROKOROK|8%|29–29|
|7|SPECIES_DWEBBLE|7%|22–22|
|8|SPECIES_HONEDGE|6%|23–23|
|9|SPECIES_SANDACONDA|5%|36–36|
|10|SPECIES_ORTHWORM|5%|24–24|
|11|SPECIES_TINKATINK|5%|24–24|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

