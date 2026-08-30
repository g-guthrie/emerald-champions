# Competitive battle-set migration

The move tutor and ordinary wild-catch pipeline now share one executable,
doubles-first corpus against the current Emerald Champions data model.

## Runtime corpus

- Authored defaults: 1,258
- Authored alternatives: 276
- Total executable presets: 1,534
- Parsed wild-table species: 616, all covered (592 species/forms across
  reachable Hoenn wild maps)
- Every valid ordinary species and form resolves to a preset. Cosmetic, Totem,
  and temporary battle forms inherit the first authored preset in their native
  form table; regional and mechanically distinct forms have direct entries.
- Eighty direct defaults and every one of the handbook's 75 ladder-backed Mega
  roles were recovered from the supplied 1,025-species Pokemon Champions
  doubles handbook. Twenty authored extension roles cover the remaining 17
  campaign stones, including all three Tatsugiri forms and both Magearna
  appearances, for complete coverage of 95 Mega orientations across all 92
  stones. Six additional
  regional/battle-distinct forms have explicit authored sets.
- Every set carries moves, held-item policy, nature, current legal Ability,
  and exactly 66 Stat Points with a 32-per-stat maximum.
- The 72 formerly unresolved move assignments are closed by an exact manifest:
  24 official historical/inherited, 41 pinned Inclement extensions, and seven
  legal replacements. The retained extensions enter the native move tutor
  through 38 unique species/move pairs.

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
- Legendary, Mythical, Mega, Primal, Ultra, Gigantamax, Tera, and invalid
  zero-data species are excluded from ordinary assignment. Curated table-seeded
  Ultra Beasts and Paradox Pokemon deliberately receive battle-ready sets.

## Evidence

- Static corpus and wild-coverage verifier: 1,534 presets, 95 Mega roles across
  92 stones, and all 616 parsed wild-table species/forms covered
- Focused all-species/form, reviewed move-access, prepared-story-gift,
  item-protection, wild-pool, Mega-tutor, staged-Zygarde, and Circuit tests are
  selected by the current zero-debt runtime manifest; rerun them for the exact
  candidate rather than carrying forward an older pass count
- Production ROM size, memory, and hash evidence must be regenerated after the
  current parallel source audit is integrated; older release metrics are not
  evidence for this working tree.
