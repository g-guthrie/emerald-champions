# Route113 — wild distribution

**Decision: KEEP.** Spinda, Skarmory and Spoink establish ash-route nostalgia. Preserve the new competitive choices around those anchors and avoid duplicate relocation merely for apparent rarity.

[World pathways and interactions](../../world/maps/Route113.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L19475)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SPINDA|13%|15–15|
|1|SPECIES_SKARMORY|12%|15–15|
|2|SPECIES_SPOINK|11%|15–15|
|3|SPECIES_SLUGMA|10%|14–14|
|4|SPECIES_SCRAGGY|10%|14–14|
|5|SPECIES_PAWNIARD|8%|14–14|
|6|SPECIES_KLEFKI|8%|16–16|
|7|SPECIES_MIENFOO|7%|16–16|
|8|SPECIES_FLETCHINDER|6%|17–17|
|9|SPECIES_FALINKS|5%|16–16|
|10|SPECIES_BOUFFALANT|5%|16–16|
|11|SPECIES_CHARCADET|5%|16–16|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_SPINDA|14–16|
|SPECIES_MIENFOO|14–16|
|SPECIES_COALOSSAL|34–34|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

