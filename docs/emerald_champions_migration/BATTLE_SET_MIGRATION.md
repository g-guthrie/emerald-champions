# Competitive battle-set migration

The move tutor and ordinary wild-catch pipeline now share one executable,
doubles-first corpus against the current Emerald Champions data model.

## Runtime corpus

- Authored defaults: 1,258
- Authored alternatives: 203
- Total executable presets: 1,461
- Current wild-table species: 613, all covered
- Every valid ordinary species and form resolves to a preset. Cosmetic, Totem,
  and temporary battle forms inherit the first authored preset in their native
  form table; regional and mechanically distinct forms have direct entries.
- Eighty direct defaults and nineteen alternatives were recovered from the
  supplied 1,025-species Pokemon Champions doubles handbook. Six additional
  regional/battle-distinct forms have explicit authored sets.
- Every set carries moves, held-item policy, nature, current legal Ability,
  and exactly 66 Stat Points with a 32-per-stat maximum.

The generator explicitly migrates every removed Verdant custom Ability to the
current species data. Runtime no longer has to silently substitute an Ability
for any direct preset. Mega-oriented presets use `requiredItem`, remain hidden
until the Mega Ring is owned, and never grant their stone.

## Player and wild behavior

- Every serviced Pokemon Center offers `Build a Battle Set` and the complete
  legal move relearner.
- Applying a set replaces moves, nature, Ability, Stat Points, and ordinary
  held item. The prior ordinary held item disappears; protected progression
  items are never overwritten.
- Ordinary wild Pokemon choose uniformly among their available non-Mega sets.
  The selected nature, Ability, moves, Stat Points, and item survive capture.
- Legendary, Mythical, Ultra Beast, Paradox, Mega, Primal, Ultra, Gigantamax,
  Tera, and invalid zero-data species are excluded from ordinary assignment.

## Evidence

- Static corpus and wild-coverage verifier: PASS
- mGBA all-species/form coverage and exact-Ability application: PASS
- Emerald Champions custom suite: 14 passed, 0 failed
- Champions Circuit live-generation suite: 3 passed, 0 failed
- Production ROM SHA-256:
  `d5a9a17ce23a00c449b84b2b28e983107f862adfa0c3427df01913e94aefa008`
- Linked ROM: 26,995,964 / 33,554,432 bytes (80.45 percent)
- EWRAM: 227,456 / 262,144 bytes (86.77 percent)
- IWRAM: 28,372 / 32,768 bytes (86.58 percent)
