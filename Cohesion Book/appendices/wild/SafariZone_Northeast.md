# SafariZone_Northeast — wild distribution

**Decision: KEEP.** The extension's familiar Johto and evolved species remain useful postgame alternatives. Clearly record the extension opening rather than treating all Safari sections as immediately accessible.

[World pathways and interactions](../../world/maps/SafariZone_Northeast.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L28126)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_HOUNDOUR|13%|33–33|
|1|SPECIES_PINECO|12%|34–34|
|2|SPECIES_FORRETRESS|11%|35–35|
|3|SPECIES_MILTANK|10%|36–36|
|4|SPECIES_TAUROS|10%|34–34|
|5|SPECIES_LEDIAN|8%|33–33|
|6|SPECIES_NIDORINO|8%|35–35|
|7|SPECIES_NIDORINA|7%|34–34|
|8|SPECIES_STANTLER|6%|36–36|
|9|SPECIES_GRANBULL|5%|37–37|
|10|SPECIES_SKARMORY|5%|39–39|
|11|SPECIES_HERACROSS|5%|40–40|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `25`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SHUCKLE|35%|25–30|
|1|SPECIES_GRAVELER|25%|25–25|
|2|SPECIES_STEELIX|18%|30–35|
|3|SPECIES_HERACROSS|12%|30–35|
|4|SPECIES_TYRANITAR|10%|55–55|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

