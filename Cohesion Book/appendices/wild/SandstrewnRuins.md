# SandstrewnRuins — wild distribution

**Decision: KEEP.** Relic Ghost/Steel Pokémon and Great Tusk make an appropriate archaeological reward. Preserve unusual power and distinguish puzzle navigation from simple map registration.

[World pathways and interactions](../../world/maps/SandstrewnRuins.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L29372)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_YAMASK|13%|21–21|
|1|SPECIES_BRONZOR|12%|21–21|
|2|SPECIES_GOLETT|11%|20–20|
|3|SPECIES_DARUMAKA|10%|20–20|
|4|SPECIES_HONEDGE|10%|20–20|
|5|SPECIES_SIGILYPH|8%|20–20|
|6|SPECIES_GREAT_TUSK|8%|22–22|
|7|SPECIES_SPIRITOMB|7%|22–22|
|8|SPECIES_UNOWN|6%|23–23|
|9|SPECIES_GABITE|5%|24–24|
|10|SPECIES_BALTOY|5%|24–24|
|11|SPECIES_KROKOROK|5%|29–29|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WOOPER|35%|50–55|
|1|SPECIES_WHISCASH|25%|50–55|
|2|SPECIES_GOLDUCK|18%|50–55|
|3|SPECIES_SWANNA|12%|50–55|
|4|SPECIES_CHEWTLE|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BALTOY|35%|20–30|
|1|SPECIES_ONIX|25%|10–20|
|2|SPECIES_STEELIX|18%|30–32|
|3|SPECIES_SHUCKLE|12%|5–10|
|4|SPECIES_CARBINK|10%|5–10|

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
|2|SPECIES_GOLDEEN|45%|25–30|
|3|SPECIES_CARVANHA|30%|25–30|
|4|SPECIES_OCTILLERY|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SEISMITOAD|30%|55–60|
|6|SPECIES_DREDNAW|25%|55–60|
|7|SPECIES_GOLDUCK|20%|55–60|
|8|SPECIES_LOMBRE|15%|55–60|
|9|SPECIES_DHELMISE|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

