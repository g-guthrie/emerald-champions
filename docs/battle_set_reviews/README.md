# Emerald Champions battle-set reviews

The generated preset table is only draft scaffolding until every selectable
species or form has an authored review. Review batches are ordinary JSON files
in this directory so multiple reviewers can work without editing one giant
file.

Each batch uses `schema_version: 1`, a unique `batch_id`, and a `reviews` list.
Every review must explicitly record:

- `species`, `role`, `moves`, `nature`, and `ability`;
- advisory-only `suggested_item` and `item_context` (the game never grants or
  equips the item);
- `source_refs`, `doubles_rationale`, and `dependency_notes`;
- `reviewed_by` and `review_notes` so later balance passes have an audit trail.

An exact cosmetic alias may add `alias_of`, but the generator accepts it only
when base stats, typing, ability slots, the complete local legal move pool, and
the reviewed moves all match the already-reviewed base species. Mechanical
forms must be authored independently.

`python3 scripts/verdant_battle_set_presets.py --check` validates the current
draft and prints authored/remaining counts. The release gate is stricter:

```sh
python3 scripts/verdant_battle_set_presets.py --check --strict-reviewed
```

That command fails until all 1,143 selectable species/forms are reviewed.

Reviewers do not need to reverse-engineer the C parsers. Generate a
self-contained slice with local stats, typing, ability slots, every exact legal
move, the current draft, source item/dependency context, and all compatible
direct-doubles candidates:

```sh
python3 scripts/verdant_battle_set_presets.py --review-packet /tmp/batch-00.json \
  --review-start 0 --review-count 96 \
  --smogon-gen4 /path/to/gen4.json \
  --smogon-gen5 /path/to/gen5.json \
  --smogon-gen6 /path/to/gen6.json \
  --smogon-gen7 /path/to/gen7.json \
  --smogon-gen8 /path/to/gen8.json \
  --smogon-gen9 /path/to/gen9.json
```

Use starts `0, 96, 192, ...` to create twelve non-overlapping packets from the
current remaining population. Copy only completed `review_template` objects
into a uniquely named batch file in this directory.

For scopes that must stay stable while other review batches land, use inclusive
local species-ID bounds instead of a moving remaining-list slice:

```sh
--review-id-min 1 --review-id-max 106
```

Reviewed or unsupported IDs inside that range are simply omitted; the range
itself never shifts.
