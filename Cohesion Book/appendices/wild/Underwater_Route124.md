# Underwater_Route124 — wild distribution

**Decision: KEEP.** Preserve Dondozo and the deep, developed marine population. This is a real Dive reward rather than a reason to move useful surface species out of the game.

[World pathways and interactions](../../world/maps/Underwater_Route124.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L38409)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LANTURN|13%|32–36|
|1|SPECIES_JELLICENT|12%|40–44|
|2|SPECIES_STARMIE|11%|34–38|
|3|SPECIES_DONDOZO|10%|40–44|
|4|SPECIES_TOXAPEX|10%|38–42|
|5|SPECIES_BARRASKEWDA|8%|34–38|
|6|SPECIES_RELICANTH|8%|36–40|
|7|SPECIES_OCTILLERY|7%|36–40|
|8|SPECIES_DRAGALGE|6%|48–50|
|9|SPECIES_GOLISOPOD|5%|38–42|
|10|SPECIES_KINGDRA|5%|40–44|
|11|SPECIES_DHELMISE|5%|40–44|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

