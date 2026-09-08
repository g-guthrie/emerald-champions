# ShoalCave_LowTideLowerRoom — wild distribution

**Decision: REVISE.** Alolan Sandshrew replaces a repeated Seel occurrence, adding a missing Ice/Steel choice. Spheal/Snorunt and the remaining roster preserve the cave's signature.

[World pathways and interactions](../../world/maps/ShoalCave_LowTideLowerRoom.md) · [Frozen native encounter source](../../baseline/source/src/data/wild_encounters.json#L34294)

## Common final behavior

Preserve all unlisted slots, slot weights, authored levels, encounter-rate fields and map identities. Effective ordinary wild levels use the native live-cap clamp. Individual Pokémon levels do not establish physical accessibility; use the linked map pathways and the actual rod/Surf/Dive/Rock Smash requirements. Pokémon caught through an ordinary Sign route retain its capture-state checks. WILD-ENGINE-01 fixes rejected Sweet Scent generation; GUIDE-01 removes unavailable Hidden advertising.

Minimum ordinary species chance is5% per method, evaluated after aggregating duplicate slots and also under Sweet Scent reversal. Feebas retains its explicit exception. Ordinary encounter frequency is a separate field from species-slot chance.

## Header row 1

### land_mons

Native field `land_mons`; encounter-rate value `10`.

**Current slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SEEL|13%|26–26|
|1|SPECIES_SPHEAL|12%|26–26|
|2|SPECIES_SWINUB|11%|28–28|
|3|SPECIES_SNORUNT|10%|28–28|
|4|SPECIES_SNEASEL|10%|30–30|
|5|SPECIES_BERGMITE|8%|30–30|
|6|SPECIES_SNOVER|8%|32–32|
|7|SPECIES_CUBCHOO|7%|32–32|
|8|SPECIES_VANILLITE|6%|32–32|
|9|SPECIES_SNOM|5%|32–32|
|10|SPECIES_CRYOGONAL|5%|32–32|
|11|SPECIES_EISCUE|5%|32–32|

**Final slots**

|Slot (zero-based)|Species|Chance|Authored levels|
|---:|---|---:|---|
|0|SPECIES_SANDSHREW_ALOLA|13%|26–26|
|1|SPECIES_SPHEAL|12%|26–26|
|2|SPECIES_SWINUB|11%|28–28|
|3|SPECIES_SNORUNT|10%|28–28|
|4|SPECIES_SNEASEL|10%|30–30|
|5|SPECIES_BERGMITE|8%|30–30|
|6|SPECIES_SNOVER|8%|32–32|
|7|SPECIES_CUBCHOO|7%|32–32|
|8|SPECIES_VANILLITE|6%|32–32|
|9|SPECIES_SNOM|5%|32–32|
|10|SPECIES_CRYOGONAL|5%|32–32|
|11|SPECIES_EISCUE|5%|32–32|

- **WILD-07:** Add the missing Ice/Steel line without removing early Seel access or the Shoal entrance population. This lower cold chamber becomes distinct from the entrance.

## Implementation and acceptance

- Confirm the linked map path exposes the required terrain/method at the intended story state; retained layout names can differ from saved map IDs.
- Apply only the named changes, then regenerate water/fishing data from its authoring source when applicable. Confirm all other existing maps remain byte-identical.
- Recheck configured species, valid level ranges, per-method probability sums and minimum aggregated species chances, including reversed selection.
- For a changed family, verify both its new acquisition and the stated remaining source of the displaced family; do not substitute a dead script or disabled Hidden table.
- Exercise capture, full-party PC transfer, repeat visits and applicable Sign restrictions in an actual implementation fixture. These runtime checks have not been performed for this proposed revision.

