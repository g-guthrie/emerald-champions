# DesertUnderpass — wild distribution

**Decision: REVISE.** Ditto, burrowers and reconstructed fossils make a useful secret route. Add the three-segment Dudunsparce in a redundant Espathra slot, with Flittle retaining that evolution. Preserve this unusual collection and its connection from Sandstrewn Ruins; the fossil-maniac entrance gate alone does not define earliest access.

[World pathways and interactions](../../world/maps/DesertUnderpass.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L3311)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_DITTO|13%|38–38|
|1|SPECIES_DIGLETT|12%|35–35|
|2|SPECIES_DRILBUR|11%|40–40|
|3|SPECIES_FLITTLE|10%|40–40|
|4|SPECIES_BALTOY|10%|41–41|
|5|SPECIES_SANDILE|8%|36–36|
|6|SPECIES_DRACOZOLT|8%|38–38|
|7|SPECIES_ARCTOZOLT|7%|42–42|
|8|SPECIES_DRACOVISH|6%|38–38|
|9|SPECIES_ARCTOVISH|5%|43–43|
|10|SPECIES_EXCADRILL|5%|44–44|
|11|SPECIES_ESPATHRA|5%|45–45|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_DITTO|13%|38–38|
|1|SPECIES_DIGLETT|12%|35–35|
|2|SPECIES_DRILBUR|11%|40–40|
|3|SPECIES_FLITTLE|10%|40–40|
|4|SPECIES_BALTOY|10%|41–41|
|5|SPECIES_SANDILE|8%|36–36|
|6|SPECIES_DRACOZOLT|8%|38–38|
|7|SPECIES_ARCTOZOLT|7%|42–42|
|8|SPECIES_DRACOVISH|6%|38–38|
|9|SPECIES_ARCTOVISH|5%|43–43|
|10|SPECIES_EXCADRILL|5%|44–44|
|11|SPECIES_DUDUNSPARCE_THREE_SEGMENT|5%|45–45|

- **FORM-WILD-08:** Provide three-segment Dudunsparce directly at5% in the burrowing habitat. Flittle remains here and preserves Espathra through evolution.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

