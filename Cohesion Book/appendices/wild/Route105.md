# Route105 — wild distribution

**Decision: KEEP.** The shoreline roster offers Water, Psychic and Fighting options. Verify actual grass/island access rather than assuming Briney's passage allows landing at every listed encounter tile.

[World pathways and interactions](../../world/maps/Route105.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L17996)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SLOWPOKE|13%|4–4|
|1|SPECIES_EXEGGCUTE|12%|4–4|
|2|SPECIES_INKAY|11%|5–5|
|3|SPECIES_CORPHISH|10%|5–5|
|4|SPECIES_WINGULL|10%|4–4|
|5|SPECIES_PSYDUCK|8%|5–5|
|6|SPECIES_CRABRAWLER|8%|4–4|
|7|SPECIES_CHATOT|7%|5–5|
|8|SPECIES_BUIZEL|6%|4–4|
|9|SPECIES_CLAUNCHER|5%|4–4|
|10|SPECIES_MAREANIE|5%|3–3|
|11|SPECIES_CRAMORANT|5%|5–5|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WISHIWASHI|35%|50–55|
|1|SPECIES_WAILORD|25%|50–55|
|2|SPECIES_CLAMPERL|18%|50–55|
|3|SPECIES_LUMINEON|12%|50–55|
|4|SPECIES_MANTINE|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_REMORAID|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_SEADRA|45%|50–55|
|3|SPECIES_CLAWITZER|30%|50–55|
|4|SPECIES_DRAGALGE|25%|50–55|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_LUVDISC|30%|55–60|
|6|SPECIES_BRUXISH|25%|55–60|
|7|SPECIES_KINGLER|20%|55–60|
|8|SPECIES_SHELLDER|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

