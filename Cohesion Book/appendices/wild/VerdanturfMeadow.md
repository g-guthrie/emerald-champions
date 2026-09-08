# VerdanturfMeadow — wild distribution

**Decision: REVISE.** Keep Cottonee, Deerling and the unusual Fairy/Psychic roster. Redundant evolved forms yield Galarian Ponyta and Iron Leaves; a separate hospitality gift supplies Indeedee-F without deleting the male form.

[World pathways and interactions](../../world/maps/VerdanturfMeadow.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L38616)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `20`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MUNNA|13%|41–43|
|1|SPECIES_ESPURR|12%|41–43|
|2|SPECIES_FLABEBE_WHITE|11%|41–43|
|3|SPECIES_COTTONEE|10%|41–43|
|4|SPECIES_DEERLING_SPRING|10%|41–43|
|5|SPECIES_VIVILLON_POKEBALL|8%|41–43|
|6|SPECIES_FLOETTE_WHITE|8%|41–43|
|7|SPECIES_FLOETTE_ETERNAL|7%|41–43|
|8|SPECIES_HATENNA|6%|41–43|
|9|SPECIES_INDEEDEE|5%|41–43|
|10|SPECIES_MILCERY|5%|41–43|
|11|SPECIES_ALCREMIE|5%|41–43|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_MUNNA|13%|41–43|
|1|SPECIES_ESPURR|12%|41–43|
|2|SPECIES_FLABEBE_WHITE|11%|41–43|
|3|SPECIES_COTTONEE|10%|41–43|
|4|SPECIES_DEERLING_SPRING|10%|41–43|
|5|SPECIES_VIVILLON_POKEBALL|8%|41–43|
|6|SPECIES_IRON_LEAVES|8%|41–43|
|7|SPECIES_FLOETTE_ETERNAL|7%|41–43|
|8|SPECIES_HATENNA|6%|41–43|
|9|SPECIES_INDEEDEE|5%|41–43|
|10|SPECIES_MILCERY|5%|41–43|
|11|SPECIES_PONYTA_GALAR|5%|41–43|

- **WILD-05:** The meadow gains the missing Galarian Ponyta line. Milcery remains here and evolves at30, retaining practical Alcremie access at this cap.
- **WILD-18:** Add an exceptional mechanical meadow guardian while keeping White Flabebe, Eternal Floette and the ordinary White Floette evolution at19. This is added choice, not a forced late-game power gate.

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|41–43|
|SPECIES_RIBOMBEE|41–43|
|SPECIES_MILCERY|41–43|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

