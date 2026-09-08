# FieryPath — wild distribution

**Decision: REVISE.** Numel-country heat, Torkoal, gas and Fire types remain recognizable. Replace only a redundant Larvesta occurrence with Slither Wing, retaining its earlier forest acquisition.

[World pathways and interactions](../../world/maps/FieryPath.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L3953)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SLUGMA|13%|15–15|
|1|SPECIES_DIGLETT|12%|15–15|
|2|SPECIES_TORKOAL|11%|16–16|
|3|SPECIES_GRIMER|10%|15–15|
|4|SPECIES_KOFFING|10%|15–15|
|5|SPECIES_MAGBY|8%|15–15|
|6|SPECIES_CHARCADET|8%|16–16|
|7|SPECIES_HOUNDOUR|7%|16–16|
|8|SPECIES_LARVESTA|6%|14–14|
|9|SPECIES_DURANT|5%|16–16|
|10|SPECIES_SIZZLIPEDE|5%|14–14|
|11|SPECIES_HEATMOR|5%|14–14|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SLUGMA|13%|15–15|
|1|SPECIES_DIGLETT|12%|15–15|
|2|SPECIES_TORKOAL|11%|16–16|
|3|SPECIES_GRIMER|10%|15–15|
|4|SPECIES_KOFFING|10%|15–15|
|5|SPECIES_MAGBY|8%|15–15|
|6|SPECIES_CHARCADET|8%|16–16|
|7|SPECIES_HOUNDOUR|7%|16–16|
|8|SPECIES_SLITHER_WING|6%|14–14|
|9|SPECIES_DURANT|5%|16–16|
|10|SPECIES_SIZZLIPEDE|5%|14–14|
|11|SPECIES_HEATMOR|5%|14–14|

- **WILD-12:** A sun-associated ancient insect gives the hot path a distinct find. Larvesta remains in the deeper Petalburg forest, Ashen Woods and volcanic interiors.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

