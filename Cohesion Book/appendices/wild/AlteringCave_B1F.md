# AlteringCave_B1F — wild distribution

**Decision: KEEP.** Keep Guzzlord as the exceptional deep-cave find and distinguish its Sign capture limit from repeatable ordinary species. Fishing adds routes that are not implied by the land roster.

[World pathways and interactions](../../world/maps/AlteringCave_B1F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L998)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_RHYDON|13%|42–42|
|1|SPECIES_SWOOBAT|12%|42–42|
|2|SPECIES_DRUDDIGON|11%|43–43|
|3|SPECIES_LUCARIO|10%|41–41|
|4|SPECIES_PARASECT|10%|42–42|
|5|SPECIES_WOBBUFFET|8%|43–43|
|6|SPECIES_GUZZLORD|8%|41–41|
|7|SPECIES_GIGALITH|7%|42–42|
|8|SPECIES_EELEKTROSS|6%|43–43|
|9|SPECIES_STEELIX|5%|41–41|
|10|SPECIES_BLISSEY|5%|42–42|
|11|SPECIES_SPIRITOMB|5%|43–43|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BASCULIN|35%|95–100|
|1|SPECIES_SLOWBRO|25%|95–100|
|2|SPECIES_GOLDUCK|18%|95–100|
|3|SPECIES_SWANNA|12%|95–100|
|4|SPECIES_DRATINI|10%|95–100|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_STEELIX|35%|41–41|
|1|SPECIES_RHYDON|25%|42–42|
|2|SPECIES_CRUSTLE|18%|43–43|
|3|SPECIES_SHUCKLE|12%|41–41|
|4|SPECIES_GARGANACL|10%|42–42|

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
|2|SPECIES_SHARPEDO|45%|95–100|
|3|SPECIES_OCTILLERY|30%|95–100|
|4|SPECIES_GOLDUCK|25%|95–100|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_CRAWDAUNT|30%|95–100|
|6|SPECIES_BASCULIN|25%|95–100|
|7|SPECIES_POLIWHIRL|20%|95–100|
|8|SPECIES_SEISMITOAD|15%|95–100|
|9|SPECIES_FEEBAS|10%|95–100|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_BLISSEY|41–43|
|SPECIES_DITTO|41–43|
|SPECIES_PORYGON2|41–43|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

