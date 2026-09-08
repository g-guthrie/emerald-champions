# Route121 — wild distribution

**Decision: REVISE.** The city/haunted-country transition gains Alolan Grimer in a repeated evolved Hypno slot. Early Drowzee preserves Hypno access; Komala also yields space for a directly obtainable rare Maushold form while remaining on Route118.

[World pathways and interactions](../../world/maps/Route121.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L21155)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SKUNTANK|13%|34–34|
|1|SPECIES_MABOSSTIFF|12%|30–30|
|2|SPECIES_HYPNO|11%|26–26|
|3|SPECIES_ELGYEM|10%|28–28|
|4|SPECIES_FURFROU|10%|28–28|
|5|SPECIES_KOMALA|8%|26–26|
|6|SPECIES_PANGORO|8%|32–32|
|7|SPECIES_TOEDSCRUEL|7%|30–30|
|8|SPECIES_ZOROARK|6%|30–30|
|9|SPECIES_EKANS|5%|27–27|
|10|SPECIES_MIMIKYU|5%|28–28|
|11|SPECIES_POLTCHAGEIST|5%|25–25|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SKUNTANK|13%|34–34|
|1|SPECIES_MABOSSTIFF|12%|30–30|
|2|SPECIES_GRIMER_ALOLA|11%|26–26|
|3|SPECIES_ELGYEM|10%|28–28|
|4|SPECIES_FURFROU|10%|28–28|
|5|SPECIES_MAUSHOLD_THREE|8%|26–26|
|6|SPECIES_PANGORO|8%|32–32|
|7|SPECIES_TOEDSCRUEL|7%|30–30|
|8|SPECIES_ZOROARK|6%|30–30|
|9|SPECIES_EKANS|5%|27–27|
|10|SPECIES_MIMIKYU|5%|28–28|
|11|SPECIES_POLTCHAGEIST|5%|25–25|

- **WILD-06:** The Lilycove approach gains an imported urban scavenger and Alolan Muk path. Drowzee remains in early Dewford Manor, preserving Hypno through evolution.
- **FORM-WILD-07:** Provide the rare three-member Maushold directly at8% without requiring repeated PID hunts. Komala remains on Route118.

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_FRILLISH|35%|55–60|
|1|SPECIES_SWANNA|25%|55–60|
|2|SPECIES_MASQUERAIN|18%|55–60|
|3|SPECIES_AZUMARILL|12%|55–60|
|4|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_ARBOK|35%|22–22|
|1|SPECIES_ARIADOS|25%|22–22|
|2|SPECIES_PINSIR|18%|15–20|
|3|SPECIES_VENOMOTH|12%|31–31|
|4|SPECIES_FORRETRESS|10%|31–31|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_BASCULIN|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_LANTURN|45%|55–60|
|3|SPECIES_WHISCASH|30%|55–60|
|4|SPECIES_CRAWDAUNT|25%|55–60|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SHARPEDO|30%|55–60|
|6|SPECIES_OCTILLERY|25%|55–60|
|7|SPECIES_GOLDUCK|20%|55–60|
|8|SPECIES_LOMBRE|15%|55–60|
|9|SPECIES_DHELMISE|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|25–28|
|SPECIES_DUSCLOPS|37–37|
|SPECIES_BANETTE|37–37|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

