# Competitive battle-set migration

The move tutor and ordinary wild-catch pipeline now share one executable,
doubles-first corpus against the current Emerald Champions data model.

## Runtime corpus

- Authored defaults: 1,258
- Alternatives: 1,361 (274 preserved/source-backed plus 1,087 legal
  second-role syntheses)
- Total executable presets: 2,619
- Non-Mega orientations: 2,524; every one of the 1,258 direct species/form
  rows exposes at least two choices before Mega access
- Parsed wild-table species: 616, all covered (592 species/forms across
  reachable Hoenn wild maps)
- Every valid ordinary species and form resolves to a preset. Cosmetic, Totem,
  and temporary battle forms inherit the first authored preset in their native
  form table; regional and mechanically distinct forms have direct entries.
- A fail-closed generation and verification contract rejects any direct row
  with fewer than two non-Mega choices. It also rejects renamed roles,
  move-order shuffles, and item-only pseudo-alternatives. Narrow species such
  as Ditto and Unown use materially different Ability or nature/Stat Point
  orientations without inventing moves they cannot learn.
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
- Ordinary configured TM/tutor compatibility remains deliberately narrow: the
  integrated generator gives Mew 110 teachable moves, and the former composite
  relearner reached 119 after adding its Gen 9 level-up moves. The Center's
  `All Legal Moves` service now uses a separate all-generation preparation
  corpus: 1,108 normalized species identities, 88,073 legal species/move rows,
  and a 372-move maximum on Mew. This second corpus does not feed machines,
  ordinary tutors, random move generation, or wild-set selection.

The generator explicitly migrates every removed Verdant custom Ability to the
current species data. Runtime no longer has to silently substitute an Ability
for any direct preset. Mega-oriented presets use `requiredItem`, remain hidden
until the Mega Ring is owned, and never grant their stone.

## Player and wild behavior

- Every serviced Pokemon Center offers `Build a Battle Set` and the complete
  historical legal move relearner. Known moves are excluded from its menu, so
  a no-move Mew exposes 372 choices and the default four-move Mew exposes 368.
- Applying a set replaces moves, nature, Ability, Stat Points, and ordinary
  held item. The prior ordinary held item disappears; protected progression
  items are never overwritten.
- Ordinary wild Pokemon choose uniformly among their available non-Mega sets.
  The selected nature, Ability, moves, Stat Points, and item survive capture.
- Legendary, Mythical, Mega, Primal, Ultra, Gigantamax, Tera, and invalid
  zero-data species are excluded from ordinary assignment. Curated table-seeded
  Ultra Beasts and Paradox Pokemon deliberately receive battle-ready sets.

## Evidence

- The isolated fixture-link comparison added 194,668 ROM bytes and four EWRAM
  bytes. The resulting fixture used 27,252,028 ROM bytes (81.22%), 227,500
  EWRAM bytes (86.78%), and 28,376 IWRAM bytes (86.60%). These are measured
  integration bounds, not a substitute for the final production release link.
- Static corpus and wild-coverage verifier: 2,619 presets, 95 Mega roles across
  92 stones, and all 616 parsed wild-table species/forms covered
- Focused all-species/form, reviewed move-access, prepared-story-gift,
  item-protection, wild-pool, Mega-tutor, staged-Zygarde, and Circuit tests are
  selected by the current zero-debt runtime manifest; rerun them for the exact
  candidate rather than carrying forward an older pass count
- Production ROM size, memory, and hash evidence must be regenerated after the
  current parallel source audit is integrated; older release metrics are not
  evidence for this working tree.
