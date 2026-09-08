# RusturfTunnel — wild distribution

**Decision: KEEP.** Whismur has the top slot and is uniquely associated with this tunnel; retain broad early alternatives including Chingling, Larvitar and Bagon. The prior suggestion to sacrifice this ammunition for a narrow roster is not adopted.

[World pathways and interactions](../../world/maps/RusturfTunnel.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L27342)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WHISMUR|13%|8–10|
|1|SPECIES_GEODUDE|12%|8–10|
|2|SPECIES_DUNSPARCE|11%|8–10|
|3|SPECIES_TEDDIURSA|10%|8–10|
|4|SPECIES_ZUBAT|10%|8–10|
|5|SPECIES_MACHOP|8%|8–10|
|6|SPECIES_LARVITAR|8%|8–10|
|7|SPECIES_DRILBUR|7%|8–10|
|8|SPECIES_NOIBAT|6%|8–10|
|9|SPECIES_ROGGENROLA|5%|8–10|
|10|SPECIES_CHINGLING|5%|8–10|
|11|SPECIES_BAGON|5%|8–10|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

