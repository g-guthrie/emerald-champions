# Emerald Champions migration source lock

> **Historical input record.** The hashes below intentionally identify the
> frozen migration inputs; they are not the current branch, ROM, or release
> candidate identity.

This migration rebuilds Emerald Champions on the modern
`pokeemerald-expansion` `upcoming` battle and data foundation. It does not
merge that tree into the old Inclement-derived tree: the histories have
diverged too far for that to be reviewable or safe.

## Frozen inputs

- Emerald Champions campaign checkpoint:
  `33202c162ebc34a1dbe2000acd26b0720baa109d`
- `pokeemerald-expansion` Champions foundation (`upcoming`):
  `a994c5e8613bf54479a24b5d9710844ab63a8ef0`
- `pokeemerald-expansion` non-Champions comparison baseline (`master`):
  `b05016d5b48e8ca9376245e19ef71d5496f84232`
- Pokemon Showdown Champions reference:
  `bb179fbf8449e3c31632bd56f671ffb4404fa6e7`

## Save-compatibility reference points

- Exact bounded legacy save contract supported by the modern migration:
  `81e288b51995c59c1dbc640f77907b8120788bc9`
- Unversioned modern e7 layout that is stamped without rewriting its current
  state: `e7df953860be1a250f6720fa4579aebe30436a04`

These hashes classify historical save layouts only. They are not current ROM
or branch identities; `SAVE_COMPATIBILITY.md` defines the supported behavior.

The expansion `upcoming` implementation is the GBA source of truth. The
pinned Showdown `data/mods/champions` implementation is an independent
executable oracle for Champions-specific rules and edge cases. If the two
disagree, record the case, determine the intended current Champions behavior,
and cover the chosen behavior with a regression test instead of silently
selecting one.

Pokemon Showdown is MIT-licensed. If any Showdown implementation or data is
copied into this repository, retain its copyright and license notice with the
copied material. Merely using Showdown as a black-box test oracle does not copy
its source.

## Product contract

- Every Pokemon family through Generation 9 remains enabled initially.
- Import the complete modern species, form, type, stat, Ability, move,
  learnset, item, animation, icon, cry, and battle behavior data supplied by
  the pinned expansion foundation.
- Include every official Mega Evolution represented by the pinned expansion,
  including the new Pokemon Champions / Legends Z-A Megas and their art.
- Preserve Emerald Champions' existing custom Megas as an explicit extension
  layer applied after upstream data.
- Mega Evolution is the campaign's only selectable battle gimmick.
- Primal Reversion remains enabled for Groudon and Kyogre by prior design
  decision; it is treated as a species-specific transformation, not a second
  selectable gimmick.
- Z-Moves, Dynamax/Gigantamax, Terastallization, and Ultra Burst are not
  player- or trainer-accessible in the campaign.
- Preserve Emerald's overworld presentation and Emerald Champions' authored
  campaign identity: its trainer sequence, dialogue that still serves the
  game, difficulty setting, level caps, tutors, battle-ready wild sets, free
  battle-item vendor, Legendary Signs, and Champions Circuit.
- Inclement Emerald compatibility is not a product goal. Legacy maps, scripts,
  data changes, rewards, and mechanics are reference material only. Import a
  piece when it makes Emerald Champions better; prefer the modern native
  expansion implementation when the old behavior adds no player value.
- Do not prune species, forms, art, or cries until a measured build proves that
  ROM, EWRAM, IWRAM, save, or runtime budgets require it. Any pruning proposal
  must include measured bytes saved and its gameplay cost.

## Compatibility gates

The migration is not complete merely because it compiles. It must pass, in
order:

1. Clean pinned-expansion baseline build with recorded ROM and memory sizes.
2. Campaign-content transplant build and boot smoke test.
3. Save-layout migration decision and explicit old-save compatibility result.
4. Champions mechanics regression suite and Showdown differential fixtures.
5. All-species/form/asset completeness manifest.
6. Custom-Mega legality, graphics, evolution, AI, and battle tests.
7. Full campaign script, map, trainer, wild-encounter, item, and progression
   validation.
8. Emulator boot plus representative new game, wild battle, trainer double
   battle, Mega, Primal, tutor, save/load, and postgame smoke tests.
9. ROM, EWRAM, IWRAM, save-block, and graphics headroom report.

Only after these gates pass should competitive presets and the master trainer
document be regenerated against the new battle standard.
