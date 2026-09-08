# SandstrewnRuins_2F — wild distribution

**Decision: REVISE.** Iron Treads distinguishes this excavated floor while repeated Claydol remains nearby. Preserve the remaining relic residents and all Rock Smash entries.

[World pathways and interactions](../../world/maps/SandstrewnRuins_2F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L29556)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_YAMASK|13%|21–21|
|1|SPECIES_BRONZONG|12%|33–33|
|2|SPECIES_GOLETT|11%|20–20|
|3|SPECIES_DARUMAKA|10%|20–20|
|4|SPECIES_HONEDGE|10%|20–20|
|5|SPECIES_SIGILYPH|8%|20–20|
|6|SPECIES_KROKOROK|8%|29–29|
|7|SPECIES_ORTHWORM|7%|22–22|
|8|SPECIES_CLAYDOL|6%|36–36|
|9|SPECIES_GABITE|5%|24–24|
|10|SPECIES_SANDACONDA|5%|36–36|
|11|SPECIES_TINKATINK|5%|24–24|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_YAMASK|13%|21–21|
|1|SPECIES_BRONZONG|12%|33–33|
|2|SPECIES_GOLETT|11%|20–20|
|3|SPECIES_DARUMAKA|10%|20–20|
|4|SPECIES_HONEDGE|10%|20–20|
|5|SPECIES_SIGILYPH|8%|20–20|
|6|SPECIES_KROKOROK|8%|29–29|
|7|SPECIES_ORTHWORM|7%|22–22|
|8|SPECIES_IRON_TREADS|6%|36–36|
|9|SPECIES_GABITE|5%|24–24|
|10|SPECIES_SANDACONDA|5%|36–36|
|11|SPECIES_TINKATINK|5%|24–24|

- **WILD-14:** Add a missing mechanical excavator to a ruin excavation floor. Claydol remains on adjacent ruin floors and can be evolved from Baltoy.

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BALTOY|35%|20–30|
|1|SPECIES_ONIX|25%|10–20|
|2|SPECIES_STEELIX|18%|30–35|
|3|SPECIES_DWEBBLE|12%|5–10|
|4|SPECIES_ORTHWORM|10%|5–10|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

