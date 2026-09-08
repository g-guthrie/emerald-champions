# MtPyre_Summit — wild distribution

**Decision: KEEP.** Flutter Mane is a powerful thematic discovery among spirits and mournful residents. Preserve it; the Darkrai NPC quest and relic story state require separate clear guidance.

[World pathways and interactions](../../world/maps/MtPyre_Summit.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L13314)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_VULPIX|13%|28–28|
|1|SPECIES_MEDICHAM|12%|37–37|
|2|SPECIES_DRIFBLIM|11%|28–28|
|3|SPECIES_BRONZONG|10%|33–33|
|4|SPECIES_CHIMECHO|10%|30–30|
|5|SPECIES_GROWLITHE|8%|25–25|
|6|SPECIES_FLUTTER_MANE|8%|24–24|
|7|SPECIES_BEHEEYEM|7%|42–42|
|8|SPECIES_ZORUA_HISUI|6%|26–26|
|9|SPECIES_CORSOLA_GALAR|5%|30–30|
|10|SPECIES_ABSOL|5%|28–28|
|11|SPECIES_HOUNDSTONE|5%|30–30|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|24–30|
|SPECIES_FLUTTER_MANE|24–30|
|SPECIES_CHIMECHO|24–30|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

