# SkyPillar_3F — wild distribution

**Decision: KEEP.** Mature dragons and Metagross make the tower a valuable preparation destination. Preserve variety and verify actual bike/transition access before assigning an earliest capture cap.

[World pathways and interactions](../../world/maps/SkyPillar_3F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L36853)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BANETTE|13%|37–37|
|1|SPECIES_DUSCLOPS|12%|37–37|
|2|SPECIES_CLAYDOL|11%|36–36|
|3|SPECIES_ALTARIA|10%|35–35|
|4|SPECIES_GOLURK|10%|43–43|
|5|SPECIES_NOIVERN|8%|48–48|
|6|SPECIES_DRAGONAIR|8%|38–38|
|7|SPECIES_DRUDDIGON|7%|36–36|
|8|SPECIES_MINIOR|6%|37–37|
|9|SPECIES_GABITE|5%|38–38|
|10|SPECIES_SALAMENCE|5%|50–50|
|11|SPECIES_METAGROSS|5%|45–45|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

