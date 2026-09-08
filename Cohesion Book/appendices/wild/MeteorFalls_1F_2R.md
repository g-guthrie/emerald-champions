# MeteorFalls_1F_2R — wild distribution

**Decision: REVISE.** Scream Tail replaces a repeated Clefairy occurrence while lunar anchors and Bagon stay. Rear-chamber physical access must not be conflated with first entry to the front cave.

[World pathways and interactions](../../world/maps/MeteorFalls_1F_2R.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L9572)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LUNATONE|13%|16–16|
|1|SPECIES_SOLROCK|12%|17–17|
|2|SPECIES_NOIBAT|11%|18–18|
|3|SPECIES_CLEFAIRY|10%|15–15|
|4|SPECIES_SWABLU|10%|14–14|
|5|SPECIES_FERROTHORN|8%|40–40|
|6|SPECIES_DRUDDIGON|8%|18–18|
|7|SPECIES_CARBINK|7%|14–14|
|8|SPECIES_MINIOR|6%|19–19|
|9|SPECIES_ALTARIA|5%|35–35|
|10|SPECIES_DEINO|5%|19–19|
|11|SPECIES_BAGON|5%|20–20|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LUNATONE|13%|16–16|
|1|SPECIES_SOLROCK|12%|17–17|
|2|SPECIES_NOIBAT|11%|18–18|
|3|SPECIES_SCREAM_TAIL|10%|15–15|
|4|SPECIES_SWABLU|10%|14–14|
|5|SPECIES_FERROTHORN|8%|40–40|
|6|SPECIES_DRUDDIGON|8%|18–18|
|7|SPECIES_CARBINK|7%|14–14|
|8|SPECIES_MINIOR|6%|19–19|
|9|SPECIES_ALTARIA|5%|35–35|
|10|SPECIES_DEINO|5%|19–19|
|11|SPECIES_BAGON|5%|20–20|

- **WILD-10:** Add the missing ancient lunar Fairy while retaining Clefairy in the front chamber and other Falls floors. Existing back-room physical access is retained.

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BASCULIN|35%|50–55|
|1|SPECIES_POLIWHIRL|25%|50–55|
|2|SPECIES_WHISCASH|18%|50–55|
|3|SPECIES_CRAWDAUNT|12%|50–55|
|4|SPECIES_DRATINI|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_CHINCHOU|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_WHISCASH|45%|35–40|
|3|SPECIES_CRAWDAUNT|30%|35–40|
|4|SPECIES_BASCULIN|25%|35–40|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_QUAGSIRE|30%|55–60|
|6|SPECIES_SEAKING|25%|55–60|
|7|SPECIES_POLIWHIRL|20%|55–60|
|8|SPECIES_SEISMITOAD|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

