# PetalburgWoods — wild distribution

**Decision: KEEP.** Preserve Shroomish/Slakoth/Wurmple with Caterpie, Ferroseed and the newly restored Burmy. This already meets broad early ammunition and woodland identity without another large reshuffle.

[World pathways and interactions](../../world/maps/PetalburgWoods.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L14751)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SHROOMISH|13%|6–8|
|1|SPECIES_SLAKOTH|12%|6–8|
|2|SPECIES_WURMPLE|11%|6–8|
|3|SPECIES_CATERPIE|10%|7–8|
|4|SPECIES_BUNEARY|10%|6–8|
|5|SPECIES_PICHU|8%|6–8|
|6|SPECIES_TAROUNTULA|8%|6–8|
|7|SPECIES_FOONGUS|7%|8–8|
|8|SPECIES_BURMY_PLANT|6%|6–8|
|9|SPECIES_BLIPBUG|5%|6–8|
|10|SPECIES_FERROSEED|5%|7–8|
|11|SPECIES_SEWADDLE|5%|6–8|

**Final: KEEP the complete current slots above.**

### rock_smash_mons

Native field `rock_smash_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_AIPOM|35%|10–15|
|1|SPECIES_CHERUBI|25%|5–10|
|2|SPECIES_PINECO|18%|15–16|
|3|SPECIES_EXEGGCUTE|12%|15–16|
|4|SPECIES_HERACROSS|10%|15–16|

**Final: KEEP the complete current slots above.**

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|6–8|
|SPECIES_PIKACHU|6–8|
|SPECIES_CATERPIE|6–8|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

