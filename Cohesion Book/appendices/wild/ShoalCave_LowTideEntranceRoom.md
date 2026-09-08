# ShoalCave_LowTideEntranceRoom — wild distribution

**Decision: KEEP.** Spheal/Snorunt and familiar cave residents establish the cold habitat. Keep its water and fishing layer and explain tide access through actual map state.

[World pathways and interactions](../../world/maps/ShoalCave_LowTideEntranceRoom.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L33917)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_ZUBAT|13%|26–26|
|1|SPECIES_SPHEAL|12%|26–26|
|2|SPECIES_SEEL|11%|28–28|
|3|SPECIES_SWINUB|10%|28–28|
|4|SPECIES_SNEASEL|10%|30–30|
|5|SPECIES_SNORUNT|8%|30–30|
|6|SPECIES_BERGMITE|8%|32–32|
|7|SPECIES_SNOVER|7%|32–32|
|8|SPECIES_CUBCHOO|6%|32–32|
|9|SPECIES_DELIBIRD|5%|32–32|
|10|SPECIES_CRYOGONAL|5%|32–32|
|11|SPECIES_EISCUE|5%|32–32|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SPHEAL|35%|55–60|
|1|SPECIES_DEWGONG|25%|55–60|
|2|SPECIES_AVALUGG|18%|55–60|
|3|SPECIES_SNOM|12%|55–60|
|4|SPECIES_CETODDLE|10%|55–60|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_SHELLDER|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_LANTURN|45%|55–60|
|3|SPECIES_OCTILLERY|30%|55–60|
|4|SPECIES_LUVDISC|25%|55–60|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_WALREIN|30%|55–60|
|6|SPECIES_DEWGONG|25%|55–60|
|7|SPECIES_QWILFISH_HISUI|20%|55–60|
|8|SPECIES_CLAMPERL|15%|55–60|
|9|SPECIES_CETODDLE|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

