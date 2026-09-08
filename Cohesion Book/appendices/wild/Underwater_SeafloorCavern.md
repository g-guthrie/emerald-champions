# Underwater_SeafloorCavern — wild distribution

**Decision: REVISE.** Dondozo/Tatsugiri remain the signature discovery. Add Droopy and Stretchy variants in redundant Kingdra/Wishiwashi slots while retaining each form at least5% and the submarine-only association.

[World pathways and interactions](../../world/maps/Underwater_SeafloorCavern.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L38547)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `15`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_DRAGALGE|13%|48–50|
|1|SPECIES_DHELMISE|12%|38–42|
|2|SPECIES_RELICANTH|11%|38–42|
|3|SPECIES_DONDOZO|10%|40–44|
|4|SPECIES_TATSUGIRI|10%|40–44|
|5|SPECIES_KINGDRA|8%|40–44|
|6|SPECIES_GOLISOPOD|8%|40–44|
|7|SPECIES_VELUZA|7%|40–44|
|8|SPECIES_WISHIWASHI|6%|40–44|
|9|SPECIES_NIHILEGO|5%|48–50|
|10|SPECIES_IRON_BUNDLE|5%|46–50|
|11|SPECIES_PALAFIN|5%|44–48|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_DRAGALGE|13%|48–50|
|1|SPECIES_DHELMISE|12%|38–42|
|2|SPECIES_RELICANTH|11%|38–42|
|3|SPECIES_DONDOZO|10%|40–44|
|4|SPECIES_TATSUGIRI|10%|40–44|
|5|SPECIES_TATSUGIRI_DROOPY|8%|40–44|
|6|SPECIES_GOLISOPOD|8%|40–44|
|7|SPECIES_VELUZA|7%|40–44|
|8|SPECIES_TATSUGIRI_STRETCHY|6%|40–44|
|9|SPECIES_NIHILEGO|5%|48–50|
|10|SPECIES_IRON_BUNDLE|5%|46–50|
|11|SPECIES_PALAFIN|5%|44–48|

- **FORM-WILD-03:** Make Droopy Tatsugiri independently obtainable at8% beside Dondozo; Kingdra remains in other underwater and cavern tables.
- **FORM-WILD-04:** Make Stretchy Tatsugiri independently obtainable at6% beside Dondozo; Wishiwashi remains in many earlier sea/fishing tables.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

