# Proposed persistent state allocation

No save structure is enlarged. These are reservations for the book's implementation, not changes to the current game. The frozen-source symbolic scan found only the retired/unused declarations for each reclaimed name. Recheck against live source when implementing.

|New symbol|Address|Replaced declaration|
|---|---|---|
|`VAR_EC_SECOND_STARTER`|`0x40E1`|`VAR_EC_RETIRED_HATCH_SPECIES_1`|
|`VAR_EC_OPENING_STATE`|`0x40E2`|`VAR_EC_RETIRED_HATCH_SPECIES_2`|
|`VAR_EC_CIRCUIT_BEST_WINS`|`0x40E3`|`VAR_EC_RETIRED_HATCH_SPECIES_3`|
|`FLAG_EC_GIFT_INDEEDEE_F`|`0x2AB`|`FLAG_UNUSED_0x2AB`|
|`FLAG_EC_GIFT_COSPLAY_PIKACHU`|`0x2AC`|`FLAG_UNUSED_0x2AC`|
|`FLAG_EC_GIFT_BATTLE_BOND_GRENINJA`|`0x2AD`|`FLAG_UNUSED_0x2AD`|
|`FLAG_EC_GIFT_PARTNER_PIKACHU`|`0x2AE`|`FLAG_UNUSED_0x2AE`|
|`FLAG_EC_GIFT_PARTNER_EEVEE`|`0x2AF`|`FLAG_UNUSED_0x2AF`|
|`FLAG_EC_GIFT_SECOND_KUBFU`|`0x2B0`|`FLAG_UNUSED_0x2B0`|
|`FLAG_EC_GIFT_SECOND_COSMOG`|`0x2B1`|`FLAG_UNUSED_0x2B1`|
|`FLAG_EC_TRICK_FINAL_TENT_RECEIVED`|`0x2B2`|`FLAG_UNUSED_0x2B2`|
|`FLAG_EC_TRICK_FINAL_ALAKAZITE_RECEIVED`|`0x2B3`|`FLAG_UNUSED_0x2B3`|
|`FLAG_EC_TRICK_FINAL_STATE_INITIALIZED`|`0x2B4`|`FLAG_UNUSED_0x2B4`|

## Migration order

One version 4 migration owns all reclamation. Run it before the existing version 3 early return can bypass new initialization. Preserve saved parties, boxes, catches, difficulty, original scene flags and actual earned receipts. Clear retired nursery values from the three reused variables, then populate opening/best state from the specific trustworthy old fields. Initialize the ten reused flags to zero exactly once. Then reconstruct Trick House legacy receipt state according to its ambiguity-aware world contract; do not clear its initializer on later loads.

Existing starter-stone delivery bits 0/1 remain first-family receipts. On version 4 migration, mask the old delivery field with `0x3` to preserve those actual receipts and initialize the newly assigned bits 2/3 to zero before the second-family entitlement is calculated. Bits 2/3 then record second-family receipts; absent stones are not owed. The 85-Sign revision uses additional bits within the existing 96-bit words: clear only new caught/unlocked bits 82–84 on version 4 initialization, preserve the original 82 Sign entries and earned Arceus, and never shift previous Sign IDs.

Best recorded initializes from observed active current Circuit wins only; zero means no best has been recorded yet. Never copy old nursery species or lifetime wins into it. Subsequent loads must not reset new receipts or best.

Acceptance covers new games, pre-version-4 saves at all opening phases, completed version 3 games, interrupted gifts/storage failure, already-owned first-starter stones, pending second-starter stones, ambiguous final Trick prizes, caught base/Galarian birds, and idempotence on the next ordinary load.
