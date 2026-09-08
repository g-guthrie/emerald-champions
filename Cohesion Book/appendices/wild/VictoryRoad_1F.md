# VictoryRoad_1F — wild distribution

**Decision: KEEP.** Preserve the mature final-team roster and first-floor Iron Valiant. Late acquisition should be immediately usable through the Leveler and preparation services.

[World pathways and interactions](../../world/maps/VictoryRoad_1F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L38883)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_LAIRON|13%|40–40|
|1|SPECIES_MEDICHAM|12%|40–40|
|2|SPECIES_NOIVERN|11%|48–48|
|3|SPECIES_NOIBAT|10%|40–40|
|4|SPECIES_PUPITAR|10%|36–36|
|5|SPECIES_GABITE|8%|36–36|
|6|SPECIES_IRON_VALIANT|8%|38–38|
|7|SPECIES_METAGROSS|7%|45–45|
|8|SPECIES_KOMMO_O|6%|45–45|
|9|SPECIES_DRAGAPULT|5%|60–60|
|10|SPECIES_VOLCARONA|5%|59–59|
|11|SPECIES_BAXCALIBUR|5%|54–54|

**Final: KEEP the complete current slots above.**

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

