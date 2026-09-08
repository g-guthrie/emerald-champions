# AshenWoods — wild distribution

**Decision: REVISE.** Fire-adapted wildlife and strong insects are convincing. Two redundant evolved birds yield space for Brute Bonnet and Bloodmoon Ursaluna; their original families retain much earlier acquisition paths.

[World pathways and interactions](../../world/maps/AshenWoods.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L1340)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SALANDIT|13%|41–43|
|1|SPECIES_GROWLITHE|12%|41–43|
|2|SPECIES_PINSIR|11%|41–43|
|3|SPECIES_HERACROSS|10%|41–43|
|4|SPECIES_HOUNDOUR|10%|41–43|
|5|SPECIES_GROWLITHE_HISUI|8%|41–43|
|6|SPECIES_BUZZWOLE|8%|41–43|
|7|SPECIES_TRUMBEAK|7%|41–43|
|8|SPECIES_NOCTOWL|6%|41–43|
|9|SPECIES_SALAZZLE|5%|41–43|
|10|SPECIES_TOUCANNON|5%|41–43|
|11|SPECIES_LARVESTA|5%|41–43|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SALANDIT|13%|41–43|
|1|SPECIES_GROWLITHE|12%|41–43|
|2|SPECIES_PINSIR|11%|41–43|
|3|SPECIES_HERACROSS|10%|41–43|
|4|SPECIES_HOUNDOUR|10%|41–43|
|5|SPECIES_GROWLITHE_HISUI|8%|41–43|
|6|SPECIES_BUZZWOLE|8%|41–43|
|7|SPECIES_BRUTE_BONNET|7%|41–43|
|8|SPECIES_URSALUNA_BLOODMOON|6%|41–43|
|9|SPECIES_SALAZZLE|5%|41–43|
|10|SPECIES_TOUCANNON|5%|41–43|
|11|SPECIES_LARVESTA|5%|41–43|

- **WILD-11:** The ash forest gains an ancient mushroom. Pikipek remains on Route104 and evolves at14, so the Trumbeak family remains early and plentiful.
- **WILD-21:** Add the otherwise unattainable Bloodmoon form as a distinctive forest discovery. Hoothoot is available on Route103/Dewford Manor and evolves at20; the existing bear family is preserved.

### Dormant Hidden source data

These entries are not a currently usable native method: DexNav is disabled. Preserve this historical data as inert; do not count it to satisfy a family/form acquisition requirement or print it in the route guide.

|Species|Authored levels|
|---|---|
|SPECIES_AUDINO|41–43|
|SPECIES_SALAZZLE|41–43|
|SPECIES_ARCANINE_HISUI|41–43|

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

