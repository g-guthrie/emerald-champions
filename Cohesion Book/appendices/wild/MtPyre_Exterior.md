# MtPyre_Exterior — wild distribution

**Decision: KEEP.** Vulpix, Meditite, Chimecho and unusual regional spirits preserve the familiar exterior identity. Keep Duskull here and distinguish exterior grass from internal ghost floors.

[World pathways and interactions](../../world/maps/MtPyre_Exterior.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L13225)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_VULPIX|13%|27–27|
|1|SPECIES_MEDITITE|12%|27–27|
|2|SPECIES_DRIFLOON|11%|28–28|
|3|SPECIES_CHIMECHO|10%|29–29|
|4|SPECIES_GROWLITHE|10%|29–29|
|5|SPECIES_BRONZOR|8%|27–27|
|6|SPECIES_ZORUA_HISUI|8%|29–29|
|7|SPECIES_CORSOLA_GALAR|7%|25–25|
|8|SPECIES_BEHEEYEM|6%|42–42|
|9|SPECIES_BRONZONG|5%|33–33|
|10|SPECIES_DUSKULL|5%|26–26|
|11|SPECIES_ABSOL|5%|28–28|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|25–29|
|SPECIES_CHINGLING|25–29|
|SPECIES_CORSOLA_GALAR|25–29|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

