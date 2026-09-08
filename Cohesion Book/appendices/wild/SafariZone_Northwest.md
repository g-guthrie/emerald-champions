# SafariZone_Northwest — wild distribution

**Decision: REVISE.** The more developed Safari population complements the northern section and includes Ditto. Preserve Kangaskhan/Scyther access and all water-method distinctions; a redundant Ditto occurrence yields space for the Yellow Squawkabilly ability group.

[World pathways and interactions](../../world/maps/SafariZone_Northwest.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L28225)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_XATU|13%|27–27|
|1|SPECIES_DODRIO|12%|31–31|
|2|SPECIES_RHYHORN|11%|29–29|
|3|SPECIES_PINSIR|10%|29–29|
|4|SPECIES_RHYDON|10%|42–42|
|5|SPECIES_CHANSEY|8%|29–29|
|6|SPECIES_RATICATE_ALOLA|8%|31–31|
|7|SPECIES_EXEGGUTOR_ALOLA|7%|29–29|
|8|SPECIES_KANGASKHAN|6%|29–29|
|9|SPECIES_SCYTHER|5%|27–27|
|10|SPECIES_TAUROS|5%|31–31|
|11|SPECIES_DITTO|5%|29–29|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_XATU|13%|27–27|
|1|SPECIES_DODRIO|12%|31–31|
|2|SPECIES_RHYHORN|11%|29–29|
|3|SPECIES_PINSIR|10%|29–29|
|4|SPECIES_RHYDON|10%|42–42|
|5|SPECIES_CHANSEY|8%|29–29|
|6|SPECIES_RATICATE_ALOLA|8%|31–31|
|7|SPECIES_EXEGGUTOR_ALOLA|7%|29–29|
|8|SPECIES_KANGASKHAN|6%|29–29|
|9|SPECIES_SCYTHER|5%|27–27|
|10|SPECIES_TAUROS|5%|31–31|
|11|SPECIES_SQUAWKABILLY_YELLOW|5%|29–29|

- **FORM-WILD-06:** Add the Yellow Squawkabilly ability group to the imported-fauna destination. Ditto remains accessible much earlier on Route117 and other Safari sections.

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_BUIZEL|35%|50–55|
|1|SPECIES_BASCULIN|25%|50–55|
|2|SPECIES_QUAGSIRE|18%|50–55|
|3|SPECIES_GOLDUCK|12%|50–55|
|4|SPECIES_DRATINI|10%|50–55|

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
|2|SPECIES_LANTURN|45%|50–55|
|3|SPECIES_SHARPEDO|30%|50–55|
|4|SPECIES_OCTILLERY|25%|50–55|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_SEISMITOAD|30%|55–60|
|6|SPECIES_DREDNAW|25%|55–60|
|7|SPECIES_GOLDUCK|20%|55–60|
|8|SPECIES_LOMBRE|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

