# SeafloorCavern_Room8 — wild distribution

**Decision: KEEP.** The final approach retains capable marine Pokémon plus Clobbopus/Dewpider options. Preserve the breadth immediately before Archie's demanding battle.

[World pathways and interactions](../../world/maps/SeafloorCavern_Room8.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L30887)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_KINGDRA|13%|30–30|
|1|SPECIES_DRAGALGE|12%|48–48|
|2|SPECIES_DEWPIDER|11%|32–32|
|3|SPECIES_DHELMISE|10%|33–33|
|4|SPECIES_GOLISOPOD|10%|30–30|
|5|SPECIES_MALAMAR|8%|30–30|
|6|SPECIES_CLAWITZER|8%|37–37|
|7|SPECIES_TOXAPEX|7%|38–38|
|8|SPECIES_CROBAT|6%|34–34|
|9|SPECIES_BARRASKEWDA|5%|35–35|
|10|SPECIES_BASCULEGION|5%|33–33|
|11|SPECIES_CLOBBOPUS|5%|36–36|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

