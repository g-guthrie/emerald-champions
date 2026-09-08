# Route110 — wild distribution

**Decision: KEEP.** Electrike, Gulpin, Plusle/Minun and urban newcomers establish the electrical corridor. Preserve the broad roster, including Pachirisu and Gimmighoul; resolve legendary clues without moving their gates.

[World pathways and interactions](../../world/maps/Route110.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L18939)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_ELECTRIKE|13%|12–12|
|1|SPECIES_GULPIN|12%|12–12|
|2|SPECIES_PLUSLE|11%|12–12|
|3|SPECIES_MINUN|10%|13–13|
|4|SPECIES_VAROOM|10%|13–13|
|5|SPECIES_SHROODLE|8%|13–13|
|6|SPECIES_PACHIRISU|8%|13–13|
|7|SPECIES_TRUBBISH|7%|13–13|
|8|SPECIES_STUNKY|6%|12–12|
|9|SPECIES_TADBULB|5%|12–12|
|10|SPECIES_MORPEKO|5%|12–12|
|11|SPECIES_GIMMIGHOUL_ROAMING|5%|13–13|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_FINNEON|35%|50–55|
|1|SPECIES_PELIPPER|25%|50–55|
|2|SPECIES_JELLICENT|18%|50–55|
|3|SPECIES_BARRASKEWDA|12%|50–55|
|4|SPECIES_CORSOLA|10%|50–55|

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
|2|SPECIES_OCTILLERY|45%|25–30|
|3|SPECIES_HORSEA|30%|25–30|
|4|SPECIES_WISHIWASHI|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_CLAWITZER|30%|55–60|
|6|SPECIES_DRAGALGE|25%|55–60|
|7|SPECIES_SHARPEDO|20%|55–60|
|8|SPECIES_LUVDISC|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|12–13|
|SPECIES_TRUBBISH|12–13|
|SPECIES_DODUO|12–13|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

