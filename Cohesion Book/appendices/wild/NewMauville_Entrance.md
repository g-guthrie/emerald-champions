# NewMauville_Entrance — wild distribution

**Decision: REVISE.** Magnemite/Voltorb/Klink establish the station. Iron Thorns replaces a redundant evolved Eelektross slot while Tynamo and its evolution path remain available locally.

[World pathways and interactions](../../world/maps/NewMauville_Entrance.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L13403)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGNEMITE|13%|24–24|
|1|SPECIES_VOLTORB|12%|24–24|
|2|SPECIES_KLINK|11%|25–25|
|3|SPECIES_ELEKID|10%|25–25|
|4|SPECIES_PORYGON|10%|23–23|
|5|SPECIES_TYNAMO|8%|23–23|
|6|SPECIES_TOGEDEMARU|8%|26–26|
|7|SPECIES_ELECTABUZZ|7%|30–30|
|8|SPECIES_KLANG|6%|38–38|
|9|SPECIES_PORYGON2|5%|22–22|
|10|SPECIES_MAGNEZONE|5%|22–22|
|11|SPECIES_EELEKTROSS|5%|22–22|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGNEMITE|13%|24–24|
|1|SPECIES_VOLTORB|12%|24–24|
|2|SPECIES_KLINK|11%|25–25|
|3|SPECIES_ELEKID|10%|25–25|
|4|SPECIES_PORYGON|10%|23–23|
|5|SPECIES_TYNAMO|8%|23–23|
|6|SPECIES_TOGEDEMARU|8%|26–26|
|7|SPECIES_ELECTABUZZ|7%|30–30|
|8|SPECIES_KLANG|6%|38–38|
|9|SPECIES_PORYGON2|5%|22–22|
|10|SPECIES_MAGNEZONE|5%|22–22|
|11|SPECIES_IRON_THORNS|5%|22–22|

- **WILD-17:** The power station gains a missing mechanical Electric species. Tynamo stays in the station and evolves through the free item archive; Magnemite/Voltorb/Klink identity remains.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

