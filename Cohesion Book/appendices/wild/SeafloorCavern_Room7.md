# SeafloorCavern_Room7 — wild distribution

**Decision: KEEP.** The defensive marine mix complements the adjacent water pool. Keep Toxapex, Golisopod and useful specialists rather than reduce the cave to repeated Zubat.

[World pathways and interactions](../../world/maps/SeafloorCavern_Room7.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L30733)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_TOXAPEX|13%|38–38|
|1|SPECIES_GOLISOPOD|12%|31–31|
|2|SPECIES_MAWILE|11%|32–32|
|3|SPECIES_MALAMAR|10%|33–33|
|4|SPECIES_DHELMISE|10%|28–28|
|5|SPECIES_DRAGALGE|8%|48–48|
|6|SPECIES_BASCULEGION|8%|34–34|
|7|SPECIES_GRAPPLOCT|7%|35–35|
|8|SPECIES_CROBAT|6%|34–34|
|9|SPECIES_CLAWITZER|5%|37–37|
|10|SPECIES_KINGDRA|5%|33–33|
|11|SPECIES_BARRASKEWDA|5%|36–36|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_CLAMPERL|35%|65–70|
|1|SPECIES_WISHIWASHI|25%|65–70|
|2|SPECIES_PYUKUMUKU|18%|65–70|
|3|SPECIES_DEWGONG|12%|65–70|
|4|SPECIES_FINIZEN|10%|65–70|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_RELICANTH|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_LANTURN|45%|65–70|
|3|SPECIES_LUVDISC|30%|65–70|
|4|SPECIES_KINGLER|25%|65–70|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SHELLDER|30%|65–70|
|6|SPECIES_BARRASKEWDA|25%|65–70|
|7|SPECIES_BRUXISH|20%|65–70|
|8|SPECIES_CLAMPERL|15%|65–70|
|9|SPECIES_BASCULEGION|10%|65–70|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

