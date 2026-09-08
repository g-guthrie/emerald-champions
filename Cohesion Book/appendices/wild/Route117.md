# Route117 — wild distribution

**Decision: REVISE.** The Daycare meadow stays broad with Ditto and varied mammals/plants. The Old Rod gains Paldean Wooper while retaining Tympole, adding a missing functional line without another scarce hunt.

[World pathways and interactions](../../world/maps/Route117.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L20061)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_TANDEMAUS|13%|13–13|
|1|SPECIES_AUDINO|12%|13–13|
|2|SPECIES_IGGLYBUFF|11%|14–14|
|3|SPECIES_PETILIL|10%|14–14|
|4|SPECIES_GOSSIFLEUR|10%|13–13|
|5|SPECIES_MEOWTH|8%|13–13|
|6|SPECIES_EXEGGCUTE|8%|13–13|
|7|SPECIES_DITTO|7%|13–13|
|8|SPECIES_GLAMEOW|6%|14–14|
|9|SPECIES_MINCCINO|5%|14–14|
|10|SPECIES_FARFETCHD_GALAR|5%|13–13|
|11|SPECIES_WOOLOO|5%|13–13|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LOTAD|35%|50–55|
|1|SPECIES_SWANNA|25%|50–55|
|2|SPECIES_CRAWDAUNT|18%|50–55|
|3|SPECIES_SEAKING|12%|50–55|
|4|SPECIES_CHEWTLE|10%|50–55|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_FLOETTE|35%|10–15|
|1|SPECIES_SUNKERN|25%|5–10|
|2|SPECIES_SUNFLORA|18%|15–20|
|3|SPECIES_COMBEE|12%|15–20|
|4|SPECIES_HERACROSS|10%|15–20|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_TYMPOLE|40%|25–30|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WOOPER_PALDEA|60%|25–30|
|1|SPECIES_TYMPOLE|40%|25–30|

- **WILD-04:** The Daycare pond gains a mud-dwelling regional form and immediate Clodsire option. Keep Tympole as the other Old Rod species. Magikarp remains widely available from the earlier Dewford Old Rod.

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
|5|SPECIES_LOMBRE|30%|55–60|
|6|SPECIES_QUAGSIRE|25%|55–60|
|7|SPECIES_WHISCASH|20%|55–60|
|8|SPECIES_CRAWDAUNT|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|13–14|
|SPECIES_KARRABLAST|13–14|
|SPECIES_SHELMET|13–14|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

