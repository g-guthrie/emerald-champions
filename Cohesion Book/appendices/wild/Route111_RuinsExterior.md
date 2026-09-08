# Route111_RuinsExterior — wild distribution

**Decision: REVISE.** The plateau gains Sandy Shocks in a repeated Claydol slot. Preserve Rockruff's distinct form, Girafarig and other useful visitors, and use actual ruins access for guide text.

[World pathways and interactions](../../world/maps/Route111_RuinsExterior.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L19297)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_XATU|13%|25–25|
|1|SPECIES_HELIOPTILE|12%|21–21|
|2|SPECIES_ROCKRUFF|11%|20–20|
|3|SPECIES_ROCKRUFF_OWN_TEMPO|10%|20–20|
|4|SPECIES_GIRAFARIG|10%|20–20|
|5|SPECIES_MEDITITE|8%|20–20|
|6|SPECIES_HAWLUCHA|8%|22–22|
|7|SPECIES_SKIPLOOM|7%|22–22|
|8|SPECIES_FARIGIRAF|6%|23–23|
|9|SPECIES_MINIOR|5%|23–23|
|10|SPECIES_JUMPLUFF|5%|27–27|
|11|SPECIES_CLAYDOL|5%|36–36|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_XATU|13%|25–25|
|1|SPECIES_HELIOPTILE|12%|21–21|
|2|SPECIES_ROCKRUFF|11%|20–20|
|3|SPECIES_ROCKRUFF_OWN_TEMPO|10%|20–20|
|4|SPECIES_GIRAFARIG|10%|20–20|
|5|SPECIES_MEDITITE|8%|20–20|
|6|SPECIES_HAWLUCHA|8%|22–22|
|7|SPECIES_SKIPLOOM|7%|22–22|
|8|SPECIES_FARIGIRAF|6%|23–23|
|9|SPECIES_MINIOR|5%|23–23|
|10|SPECIES_JUMPLUFF|5%|27–27|
|11|SPECIES_SANDY_SHOCKS|5%|36–36|

- **WILD-13:** The desert ruins gain the missing ancient magnet. Baltoy/Claydol remain in the desert and ruin interiors; no early player family is removed.

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_JUMPLUFF|27–27|
|SPECIES_FARIGIRAF|20–24|
|SPECIES_MEDICHAM|37–37|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

