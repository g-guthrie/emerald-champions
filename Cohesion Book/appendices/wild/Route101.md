# Route101 — wild distribution

**Decision: KEEP.** The opening has strong Hoenn anchors and baby/unevolved alternatives. Preserve its12-species roster around the new two-starter rescue and pre-rival catching loop.

[World pathways and interactions](../../world/maps/Route101.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L17355)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_ZIGZAGOON|13%|2–2|
|1|SPECIES_POOCHYENA|12%|2–2|
|2|SPECIES_WURMPLE|11%|2–2|
|3|SPECIES_TAILLOW|10%|3–3|
|4|SPECIES_PIDGEY|10%|3–3|
|5|SPECIES_SENTRET|8%|3–3|
|6|SPECIES_LILLIPUP|8%|3–3|
|7|SPECIES_BONSLY|7%|3–3|
|8|SPECIES_NIDORAN_F|6%|2–2|
|9|SPECIES_NIDORAN_M|5%|2–2|
|10|SPECIES_EEVEE|5%|3–3|
|11|SPECIES_PAWMI|5%|3–3|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|2–3|
|SPECIES_PIKACHU|2–3|
|SPECIES_WURMPLE|2–3|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

