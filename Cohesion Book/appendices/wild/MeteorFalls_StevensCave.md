# MeteorFalls_StevensCave — wild distribution

**Decision: REVISE.** The steel/stone collection gains Iron Boulder in a redundant Duraludon slot. Retain Metagross and Iron Crown, and keep Steven's authored team as a separate encounter obligation.

[World pathways and interactions](../../world/maps/MeteorFalls_StevensCave.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L10034)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BELDUM|13%|33–33|
|1|SPECIES_METANG|12%|35–35|
|2|SPECIES_METAGROSS|11%|45–45|
|3|SPECIES_SKARMORY|10%|35–35|
|4|SPECIES_AGGRON|10%|42–42|
|5|SPECIES_CLAYDOL|8%|37–37|
|6|SPECIES_DURALUDON|8%|35–35|
|7|SPECIES_GHOLDENGO|7%|45–45|
|8|SPECIES_ARCHALUDON|6%|38–38|
|9|SPECIES_IRON_CROWN|5%|40–40|
|10|SPECIES_BAXCALIBUR|5%|54–54|
|11|SPECIES_DRAGAPULT|5%|60–60|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BELDUM|13%|33–33|
|1|SPECIES_METANG|12%|35–35|
|2|SPECIES_METAGROSS|11%|45–45|
|3|SPECIES_SKARMORY|10%|35–35|
|4|SPECIES_AGGRON|10%|42–42|
|5|SPECIES_CLAYDOL|8%|37–37|
|6|SPECIES_IRON_BOULDER|8%|35–35|
|7|SPECIES_GHOLDENGO|7%|45–45|
|8|SPECIES_ARCHALUDON|6%|38–38|
|9|SPECIES_IRON_CROWN|5%|40–40|
|10|SPECIES_BAXCALIBUR|5%|54–54|
|11|SPECIES_DRAGAPULT|5%|60–60|

- **WILD-20:** The deepest steel/stone collection gains a missing future mineral species. Duraludon retains Granite Cave, Route118 and other caves.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

