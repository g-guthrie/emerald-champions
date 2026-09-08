# Route103 — wild distribution

**Decision: KEEP.** The Electric-rich roster is an intentional broad early resource after the user's clarification. Keep those options; improve the riverside identity through world context rather than an automatic availability nerf.

[World pathways and interactions](../../world/maps/Route103.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L17618)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_PATRAT|13%|2–2|
|1|SPECIES_SHELLOS|12%|3–3|
|2|SPECIES_SHINX|11%|3–3|
|3|SPECIES_MAREEP|10%|4–4|
|4|SPECIES_GROWLITHE|10%|2–2|
|5|SPECIES_ELECTRIKE|8%|3–3|
|6|SPECIES_HOOTHOOT|8%|3–3|
|7|SPECIES_BLITZLE|7%|4–4|
|8|SPECIES_YAMPER|6%|3–3|
|9|SPECIES_TOXEL|5%|3–3|
|10|SPECIES_GRUBBIN|5%|2–2|
|11|SPECIES_ROCKRUFF|5%|4–4|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WINGULL|35%|50–55|
|1|SPECIES_SHELLOS|25%|50–55|
|2|SPECIES_STARYU|18%|50–55|
|3|SPECIES_KINGLER|12%|50–55|
|4|SPECIES_WIGLETT|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SHELLDER|35%|10–15|
|1|SPECIES_CORSOLA|25%|5–10|
|2|SPECIES_DWEBBLE|18%|15–16|
|3|SPECIES_BINACLE|12%|15–16|
|4|SPECIES_PYUKUMUKU|10%|15–16|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|15–20|
|1|SPECIES_REMORAID|40%|15–20|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_STARYU|45%|25–30|
|3|SPECIES_QWILFISH|30%|25–30|
|4|SPECIES_CLAMPERL|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_KINGLER|30%|55–60|
|6|SPECIES_SHELLDER|25%|55–60|
|7|SPECIES_SEADRA|20%|55–60|
|8|SPECIES_WISHIWASHI|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|2–4|
|SPECIES_KRICKETOT|2–4|
|SPECIES_GRUBBIN|2–4|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

