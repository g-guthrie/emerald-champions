# MeteorFalls_1F_1R — wild distribution

**Decision: KEEP.** Solrock/Lunatone, Bagon and rare mineral/dragon visitors make an excellent landmark roster. Preserve early Bagon elsewhere too; this site remains meaningful through density and its own discoveries.

[World pathways and interactions](../../world/maps/MeteorFalls_1F_1R.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L9418)

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
|2|SPECIES_ZUBAT|11%|18–18|
|3|SPECIES_NOIBAT|10%|22–22|
|4|SPECIES_CLEFAIRY|10%|14–14|
|5|SPECIES_BAGON|8%|16–16|
|6|SPECIES_FERROSEED|8%|18–18|
|7|SPECIES_DRUDDIGON|7%|14–14|
|8|SPECIES_MINIOR|6%|19–19|
|9|SPECIES_DEINO|5%|20–20|
|10|SPECIES_CARBINK|5%|19–19|
|11|SPECIES_DRAMPA|5%|20–20|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BUIZEL|35%|50–55|
|1|SPECIES_WHISCASH|25%|50–55|
|2|SPECIES_SLOWBRO|18%|50–55|
|3|SPECIES_DREDNAW|12%|50–55|
|4|SPECIES_DRATINI|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_BARBOACH|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_CRAWDAUNT|45%|35–40|
|3|SPECIES_SEAKING|30%|35–40|
|4|SPECIES_BASCULIN|25%|35–40|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_GOLDUCK|30%|55–60|
|6|SPECIES_LOMBRE|25%|55–60|
|7|SPECIES_QUAGSIRE|20%|55–60|
|8|SPECIES_LANTURN|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

