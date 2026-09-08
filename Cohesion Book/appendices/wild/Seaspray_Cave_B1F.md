# Seaspray_Cave_B1F — wild distribution

**Decision: KEEP.** Preserve the early Ice roster, babies and Frigibax. The user's choice-first direction supersedes a blanket plan to move these families later merely to reserve Shoal's novelty.

[World pathways and interactions](../../world/maps/Seaspray_Cave_B1F.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L32170)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SEEL|13%|10–12|
|1|SPECIES_SWINUB|12%|10–12|
|2|SPECIES_SNORUNT|11%|10–12|
|3|SPECIES_SNEASEL|10%|10–12|
|4|SPECIES_SPHEAL|10%|10–12|
|5|SPECIES_SMOOCHUM|8%|10–12|
|6|SPECIES_SNOVER|8%|10–12|
|7|SPECIES_FRIGIBAX|7%|10–12|
|8|SPECIES_VANILLITE|6%|10–12|
|9|SPECIES_CUBCHOO|5%|10–12|
|10|SPECIES_CRYOGONAL|5%|10–12|
|11|SPECIES_BERGMITE|5%|10–12|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_CUBCHOO|10–12|
|SPECIES_SNOVER|10–12|
|SPECIES_CRYOGONAL|10–12|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

