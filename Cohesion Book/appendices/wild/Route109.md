# Route109 — wild distribution

**Decision: KEEP.** The beach's Rock Smash pool includes Sandygast and Pincurchin, making a method-specific discovery worthwhile. Keep Surf and fishing availability separate from early beach trainer access.

[World pathways and interactions](../../world/maps/Route109.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L18512)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SHELLOS|35%|50–55|
|1|SPECIES_KINGLER|25%|50–55|
|2|SPECIES_LUMINEON|18%|50–55|
|3|SPECIES_JELLICENT|12%|50–55|
|4|SPECIES_WIGLETT|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SANDYGAST|35%|14–16|
|1|SPECIES_KRABBY|25%|14–16|
|2|SPECIES_CORSOLA|18%|14–16|
|3|SPECIES_PINCURCHIN|12%|14–16|
|4|SPECIES_WIMPOD|10%|14–16|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_LUVDISC|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_OCTILLERY|45%|25–30|
|3|SPECIES_CLAUNCHER|30%|25–30|
|4|SPECIES_SKRELP|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_BRUXISH|30%|55–60|
|6|SPECIES_KINGLER|25%|55–60|
|7|SPECIES_SHELLDER|20%|55–60|
|8|SPECIES_STARYU|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

