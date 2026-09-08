# VictoryRoad_B2F — wild distribution

**Decision: KEEP.** The deepest route retains final-team power and a separate water layer. Preserve its roster and the League handoff; repeated useful families are acceptable here.

[World pathways and interactions](../../world/maps/VictoryRoad_B2F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L39465)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LAIRON|13%|40–40|
|1|SPECIES_MEDICHAM|12%|40–40|
|2|SPECIES_NOIBAT|11%|40–40|
|3|SPECIES_NOIVERN|10%|48–48|
|4|SPECIES_METANG|10%|42–42|
|5|SPECIES_PUPITAR|8%|42–42|
|6|SPECIES_KOMMO_O|8%|45–45|
|7|SPECIES_DRAGAPULT|7%|60–60|
|8|SPECIES_METAGROSS|6%|45–45|
|9|SPECIES_IRON_VALIANT|5%|42–42|
|10|SPECIES_VOLCARONA|5%|59–59|
|11|SPECIES_BAXCALIBUR|5%|54–54|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BASCULIN|35%|75–80|
|1|SPECIES_FLOATZEL|25%|75–80|
|2|SPECIES_SEAKING|18%|75–80|
|3|SPECIES_SHELLOS|12%|75–80|
|4|SPECIES_DRATINI|10%|75–80|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_CARVANHA|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_LANTURN|45%|75–80|
|3|SPECIES_POLIWHIRL|30%|75–80|
|4|SPECIES_SEISMITOAD|25%|75–80|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_DREDNAW|30%|75–80|
|6|SPECIES_OCTILLERY|25%|75–80|
|7|SPECIES_GOLDUCK|20%|75–80|
|8|SPECIES_LOMBRE|15%|75–80|
|9|SPECIES_DRATINI|10%|75–80|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

