# ArtisanCave_1F — wild distribution

**Decision: KEEP.** Smeargle deserves stronger visual and dialogue identity, but preserve the useful broad collection. Its13% leading slot is a conscious broad-access choice rather than a broken probability. No automatic return to a monotypic cave.

[World pathways and interactions](../../world/maps/ArtisanCave_1F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L1202)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SMEARGLE|13%|40–40|
|1|SPECIES_WOOBAT|12%|41–41|
|2|SPECIES_GLIGAR|11%|42–42|
|3|SPECIES_SABLEYE|10%|43–43|
|4|SPECIES_MAWILE|10%|44–44|
|5|SPECIES_ARON|8%|45–45|
|6|SPECIES_CARBINK|8%|46–46|
|7|SPECIES_DODUO|7%|47–47|
|8|SPECIES_GIRAFARIG|6%|48–48|
|9|SPECIES_WOBBUFFET|5%|49–49|
|10|SPECIES_PINSIR|5%|50–50|
|11|SPECIES_HERACROSS|5%|50–50|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

