# Route120 — wild distribution

**Decision: KEEP.** Keep Absol, Tropius and the mushroom/pumpkin ecology. The four Pumpkaboo sizes are deliberate form access; do not call them four entirely new species or remove mechanically distinct size options.

[World pathways and interactions](../../world/maps/Route120.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L20951)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_ABSOL|13%|25–25|
|1|SPECIES_VENOMOTH|12%|31–31|
|2|SPECIES_TROPIUS|11%|27–27|
|3|SPECIES_PUMPKABOO|10%|25–25|
|4|SPECIES_WATCHOG|10%|25–25|
|5|SPECIES_SHIFTRY|8%|26–26|
|6|SPECIES_MORELULL|8%|27–27|
|7|SPECIES_GOTHITA|7%|27–27|
|8|SPECIES_PUMPKABOO_SMALL|6%|25–25|
|9|SPECIES_PUMPKABOO_LARGE|5%|27–27|
|10|SPECIES_PUMPKABOO_SUPER|5%|25–25|
|11|SPECIES_SPIRITOMB|5%|25–25|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WOOPER|35%|50–55|
|1|SPECIES_MASQUERAIN|25%|50–55|
|2|SPECIES_POLIWHIRL|18%|50–55|
|3|SPECIES_LOMBRE|12%|50–55|
|4|SPECIES_CHEWTLE|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_PINSIR|35%|10–15|
|1|SPECIES_HERACROSS|25%|5–10|
|2|SPECIES_YANMEGA|18%|15–20|
|3|SPECIES_TREVENANT|12%|15–20|
|4|SPECIES_FORRETRESS|10%|31–31|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_CORPHISH|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_SEISMITOAD|45%|50–55|
|3|SPECIES_QUAGSIRE|30%|50–55|
|4|SPECIES_SEAKING|25%|50–55|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_LANTURN|30%|55–60|
|6|SPECIES_POLIWHIRL|25%|55–60|
|7|SPECIES_DREDNAW|20%|55–60|
|8|SPECIES_SHARPEDO|15%|55–60|
|9|SPECIES_DHELMISE|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|25–27|
|SPECIES_YANMEGA|25–27|
|SPECIES_DITTO|25–27|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

