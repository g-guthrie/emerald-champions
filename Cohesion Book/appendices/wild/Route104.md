# Route104 — wild distribution

**Decision: KEEP.** Mienfoo, babies, birds and plants give varied low-cap team-building. Preserve the eleven-percent Mienfoo and the fishing/Surf return layer independently of the initial woodland route.

[World pathways and interactions](../../world/maps/Route104.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L17822)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_AZURILL|13%|5–7|
|1|SPECIES_PIKIPEK|12%|5–7|
|2|SPECIES_MIENFOO|11%|5–7|
|3|SPECIES_LEDYBA|10%|5–7|
|4|SPECIES_BUDEW|10%|5–7|
|5|SPECIES_PIDOVE|8%|5–7|
|6|SPECIES_HOPPIP|8%|5–7|
|7|SPECIES_BUNNELBY|7%|5–7|
|8|SPECIES_YUNGOOS|6%|5–7|
|9|SPECIES_LITLEO|5%|5–7|
|10|SPECIES_SMOLIV|5%|5–7|
|11|SPECIES_FLETCHLING|5%|5–7|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_WINGULL|35%|50–55|
|1|SPECIES_TENTACRUEL|25%|50–55|
|2|SPECIES_CORSOLA|18%|50–55|
|3|SPECIES_MANTINE|12%|50–55|
|4|SPECIES_WIGLETT|10%|50–55|

**Final: KEEP the complete current slots above.**

### old_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MAGIKARP|60%|15–20|
|1|SPECIES_SKRELP|40%|15–20|

**Final: KEEP the complete current slots above.**

### good_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|2|SPECIES_CLAUNCHER|45%|25–30|
|3|SPECIES_OCTILLERY|30%|25–30|
|4|SPECIES_HORSEA|25%|25–30|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_WISHIWASHI|30%|55–60|
|6|SPECIES_SHARPEDO|25%|55–60|
|7|SPECIES_LUVDISC|20%|55–60|
|8|SPECIES_BRUXISH|15%|55–60|
|9|SPECIES_FEEBAS|10%|55–60|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|5–7|
|SPECIES_AIPOM|5–7|
|SPECIES_PICHU|5–7|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

