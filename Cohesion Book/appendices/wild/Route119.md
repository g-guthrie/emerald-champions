# Route119 — wild distribution

**Decision: REVISE.** Tropius, Goomy and rainforest residents stay with Raging Bolt. Hisuian Voltorb replaces a repeated final-evolution Kommo-o encounter; its base family remains earlier and elsewhere.

[World pathways and interactions](../../world/maps/Route119.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L20469)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `15`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_TROPIUS|13%|25–25|
|1|SPECIES_GOOMY|12%|25–25|
|2|SPECIES_ORANGURU|11%|27–27|
|3|SPECIES_COMFEY|10%|25–25|
|4|SPECIES_KECLEON|10%|27–27|
|5|SPECIES_AMOONGUSS|8%|39–39|
|6|SPECIES_RAGING_BOLT|8%|27–27|
|7|SPECIES_CRAMORANT|7%|24–24|
|8|SPECIES_DREEPY|6%|25–25|
|9|SPECIES_TOEDSCOOL|5%|26–26|
|10|SPECIES_TOUCANNON|5%|28–28|
|11|SPECIES_KOMMO_O|5%|45–45|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_TROPIUS|13%|25–25|
|1|SPECIES_GOOMY|12%|25–25|
|2|SPECIES_ORANGURU|11%|27–27|
|3|SPECIES_COMFEY|10%|25–25|
|4|SPECIES_KECLEON|10%|27–27|
|5|SPECIES_AMOONGUSS|8%|39–39|
|6|SPECIES_RAGING_BOLT|8%|27–27|
|7|SPECIES_CRAMORANT|7%|24–24|
|8|SPECIES_DREEPY|6%|25–25|
|9|SPECIES_TOEDSCOOL|5%|26–26|
|10|SPECIES_TOUCANNON|5%|28–28|
|11|SPECIES_VOLTORB_HISUI|5%|45–45|

- **WILD-09:** The wet forest gains a wooden, Grass/Electric regional form. Jangmo-o remains on Route114/JaggedPass and can evolve before this cap; direct Kommo-o remains elsewhere.

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_POLIWAG|35%|50–55|
|1|SPECIES_SLOWBRO|25%|50–55|
|2|SPECIES_LOMBRE|18%|50–55|
|3|SPECIES_MASQUERAIN|12%|50–55|
|4|SPECIES_FEEBAS|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|25–30|
|1|SPECIES_TYMPOLE|40%|25–30|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_BASCULIN|45%|50–55|
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
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|24–27|
|SPECIES_AMOONGUSS|39–39|
|SPECIES_SLAKING|36–36|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

