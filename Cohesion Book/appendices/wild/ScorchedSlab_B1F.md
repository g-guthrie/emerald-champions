# ScorchedSlab_B1F — wild distribution

**Decision: KEEP.** The hot cave combines Fighting support, dragons and Fire types. Preserve the varied roster and separate water methods; the slab's expansion route is part of its identity.

[World pathways and interactions](../../world/maps/ScorchedSlab_B1F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L29853)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_NOIBAT|13%|41–41|
|1|SPECIES_GURDURR|12%|42–42|
|2|SPECIES_DUGTRIO|11%|43–43|
|3|SPECIES_BOLDORE|10%|41–41|
|4|SPECIES_MACHOKE|10%|42–42|
|5|SPECIES_ZWEILOUS|8%|50–50|
|6|SPECIES_MAGMAR|8%|41–41|
|7|SPECIES_TURTONATOR|7%|42–42|
|8|SPECIES_HEATMOR|6%|43–43|
|9|SPECIES_DRUDDIGON|5%|41–41|
|10|SPECIES_DARMANITAN|5%|42–42|
|11|SPECIES_SLUGMA|5%|43–43|

**Final: KEEP the complete current slots above.**

### water_mons

Native field `water_mons`; encounter-rate value `4`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SLOWPOKE|35%|50–55|
|1|SPECIES_GOLDUCK|25%|50–55|
|2|SPECIES_SWANNA|18%|50–55|
|3|SPECIES_FLOATZEL|12%|50–55|
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
|3|SPECIES_OCTILLERY|30%|50–55|
|4|SPECIES_GOLDUCK|25%|50–55|

**Final: KEEP the complete current slots above.**

### super_rod

Native field `fishing_mons`; encounter-rate value `30`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|5|SPECIES_DREDNAW|30%|55–60|
|6|SPECIES_SHARPEDO|25%|55–60|
|7|SPECIES_LOMBRE|20%|55–60|
|8|SPECIES_QUAGSIRE|15%|55–60|
|9|SPECIES_DRATINI|10%|55–60|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

