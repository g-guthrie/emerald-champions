# JaggedPass — wild distribution

**Decision: KEEP.** Fighting, Ground, Fire and mountain dragons fit the slopes. Preserve the broad roster and bike/path distinctions; no proposal delays early Bagon merely to enforce a growth chapter.

[World pathways and interactions](../../world/maps/JaggedPass.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L8658)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_PRIMEAPE|13%|28–28|
|1|SPECIES_GLIGAR|12%|21–21|
|2|SPECIES_GRUMPIG|11%|32–32|
|3|SPECIES_SALANDIT|10%|20–20|
|4|SPECIES_MUDBRAY|10%|20–20|
|5|SPECIES_FEAROW|8%|20–20|
|6|SPECIES_JANGMO_O|8%|21–21|
|7|SPECIES_TURTONATOR|7%|22–22|
|8|SPECIES_BAGON|6%|22–22|
|9|SPECIES_DEINO|5%|22–22|
|10|SPECIES_ABSOL|5%|22–22|
|11|SPECIES_HAKAMO_O|5%|35–35|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|20–22|
|SPECIES_SALAZZLE|33–33|
|SPECIES_HAKAMO_O|35–35|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

