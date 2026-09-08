# MtPyre_3F — wild distribution

**Decision: KEEP.** Lampent and the original ghosts provide increasing local variety. Preserve the roster and the actual indoor trainer paths rather than use cosmetic floor numbers as capture gates.

[World pathways and interactions](../../world/maps/MtPyre_3F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L12949)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SHUPPET|13%|27–27|
|1|SPECIES_DUSKULL|12%|28–28|
|2|SPECIES_HAUNTER|11%|26–26|
|3|SPECIES_LITWICK|10%|25–25|
|4|SPECIES_MISDREAVUS|10%|29–29|
|5|SPECIES_MURKROW|8%|24–24|
|6|SPECIES_LAMPENT|8%|41–41|
|7|SPECIES_PHANTUMP|7%|22–22|
|8|SPECIES_GREAVARD|6%|29–29|
|9|SPECIES_SINISTEA|5%|24–24|
|10|SPECIES_BANETTE|5%|37–37|
|11|SPECIES_DUSCLOPS|5%|37–37|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

