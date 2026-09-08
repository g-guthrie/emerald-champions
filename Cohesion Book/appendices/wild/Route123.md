# Route123 — wild distribution

**Decision: KEEP.** The Berry Master's countryside supports plants, insects, birds and orchard families. Preserve Karrablast/Shelmet access, their evolution methods and the separate garden-currency rewards.

[World pathways and interactions](../../world/maps/Route123.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L21448)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MIGHTYENA|13%|26–26|
|1|SPECIES_LINOONE|12%|26–26|
|2|SPECIES_GLOOM|11%|26–26|
|3|SPECIES_STANTLER|10%|28–28|
|4|SPECIES_KARRABLAST|10%|28–28|
|5|SPECIES_SHELMET|8%|26–26|
|6|SPECIES_TROPIUS|8%|28–28|
|7|SPECIES_FLAMIGO|7%|28–28|
|8|SPECIES_APPLIN|6%|26–26|
|9|SPECIES_SMOLIV|5%|27–27|
|10|SPECIES_ORANGURU|5%|28–28|
|11|SPECIES_KECLEON|5%|25–25|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BUIZEL|35%|50–55|
|1|SPECIES_SEAKING|25%|50–55|
|2|SPECIES_SWANNA|18%|50–55|
|3|SPECIES_SEISMITOAD|12%|50–55|
|4|SPECIES_CHEWTLE|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_NOCTOWL|35%|20–20|
|1|SPECIES_EXEGGCUTE|25%|5–10|
|2|SPECIES_PINECO|18%|15–20|
|3|SPECIES_BEEDRILL|12%|15–20|
|4|SPECIES_PINSIR|10%|15–20|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_GOLDEEN|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_CRAWDAUNT|45%|50–55|
|3|SPECIES_SEISMITOAD|30%|50–55|
|4|SPECIES_DREDNAW|25%|50–55|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_QUAGSIRE|30%|55–60|
|6|SPECIES_WHISCASH|25%|55–60|
|7|SPECIES_BASCULIN|20%|55–60|
|8|SPECIES_LANTURN|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|25–28|
|SPECIES_ACCELGOR|25–28|
|SPECIES_ESCAVALIER|25–28|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

