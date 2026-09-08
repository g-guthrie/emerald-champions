# SeafloorCavern_Room3 — wild distribution

**Decision: KEEP.** Dhelmise, relic Pokémon and defensive Water types fit the enclosed ruin. Preserve the palette and the nearby admin battle as separate content.

[World pathways and interactions](../../world/maps/SeafloorCavern_Room3.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L30372)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_DHELMISE|13%|30–30|
|1|SPECIES_CLAYDOL|12%|36–36|
|2|SPECIES_SABLEYE|11%|32–32|
|3|SPECIES_BRONZONG|10%|33–33|
|4|SPECIES_MALAMAR|10%|30–30|
|5|SPECIES_DRAGALGE|8%|48–48|
|6|SPECIES_TOXAPEX|8%|38–38|
|7|SPECIES_GOLISOPOD|7%|35–35|
|8|SPECIES_CROBAT|6%|34–34|
|9|SPECIES_BARBARACLE|5%|39–39|
|10|SPECIES_KINGDRA|5%|33–33|
|11|SPECIES_BASCULEGION|5%|36–36|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

