# Legendary Signs and Champions Circuit

## Legendary acquisition layer

Emerald Champions has 81 complete Sign definitions and 101 legendary-class
families with a Hoenn acquisition root.

- Twenty-three visible quests use native overworld objects, including giant
  Regigigas and the Darkrai-Cresselia-Dialga chain.
- Eighteen ordinary-wild roots place Ultra Beasts and other special species in
  coherent restored areas without duplicating one-off sanctuaries.
- Conditional quests accept any member of the required evolution family.
- Every prerequisite chain terminates; no Sign depends on collapsible Mirage
  Tower or Safari capture rules.
- Manaphy and Ditto produce Phione through the Day Care.
- Genesect and Poipole are one-time Game Corner prizes.
- Four postgame island passes are obtainable through the native harbor.
- Twelve finite Circuit rewards complete before the win-40 mastery reward.
- Arceus mastery uses all required Sign state and clamps its level to 100.

The restored 22-map side-area network contains seventeen Sign objects, four
static sanctuaries, forty unique progression pickups, and twenty-one themed
wild tables. All restored maps are reachable from the directed Hoenn campaign
graph, including the permanent post-collapse Sandstrewn Ruins route and the
restored Cave of Origin crystal chamber.

## Live Showdown Champions Circuit

The Battle Frontier lobby exposes an indefinitely replayable doubles Circuit.
Teams are generated inside the ROM rather than selected from a finite seed
list.

- Pinned Pokemon Showdown commit:
  `bb179fbf8449e3c31632bd56f671ffb4404fa6e7`
- Source data SHA-256:
  `851114e68805aafbecbed0aaee7994164d199d890deed78bafc2beaf3c2221d8`
- Pool: 311 species/form variants and 444 role templates
- Live rules: species and item uniqueness, one Mega, type/weakness limits,
  weather and field compatibility, role-driven moves, Ability filtering, and
  doubles item selection
- Player party: temporarily normalized to level 80 and restored exactly on
  retirement or defeat
- Opponent scaling: one additional slot gains one level after each win, then
  the whole team advances
- State: dedicated variables; no Battle Tower record contamination

The GBA generator now samples each National Dex family in one linear pass
instead of nested full-table scans. Sixteen seeded live generations (96 sets)
pass level, 66-Stat-Point, move, Ability, species, item, and diversity checks.
Transform-only Ditto and Fake Out plus Last Resort remain intentional native
Showdown sets.

## Evidence

- Legendary dependency and source verifier: PASS
- Restored-world verifier: PASS
- Circuit source-lock verifier: PASS
- Circuit runtime suite: 3 passed, 0 failed
- Commander suite: 41 passed, 0 failed
