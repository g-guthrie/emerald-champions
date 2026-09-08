# MirageTower_4F — wild distribution

**Decision: KEEP.** The evolved ruin roster suits the fossil summit. Preserve it and ensure the permanent Sandstrewn/Underpass route still supports later collection after the tower event.

[World pathways and interactions](../../world/maps/MirageTower_4F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L10310)

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
|1|SPECIES_GOLURK|12%|43–43|
|2|SPECIES_BRONZONG|11%|33–33|
|3|SPECIES_SIGILYPH|10%|20–20|
|4|SPECIES_COFAGRIGUS|10%|34–34|
|5|SPECIES_DARMANITAN|8%|20–20|
|6|SPECIES_HONEDGE|8%|22–22|
|7|SPECIES_SANDACONDA|7%|36–36|
|8|SPECIES_KROOKODILE|6%|40–40|
|9|SPECIES_SPIRITOMB|5%|23–23|
|10|SPECIES_ORTHWORM|5%|24–24|
|11|SPECIES_TINKATINK|5%|24–24|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

