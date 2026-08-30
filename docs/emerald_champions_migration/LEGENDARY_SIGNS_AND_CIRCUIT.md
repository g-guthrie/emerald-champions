# Legendary Signs and Champions Circuit

## Legendary acquisition layer

Emerald Champions has 82 complete Sign definitions and 101 legendary-class
families with a Hoenn acquisition root.

- Twenty-five visible Sign quests use native overworld objects, including giant
  Regigigas and the Darkrai-Cresselia-Dialga chain.
- Seven ordinary-wild roots place the curated Ultra Beasts in coherent
  restored areas without duplicating one-off sanctuaries.
- The visible Signs plus native/static encounters form 32 unique physical
  one-off Pokémon objects across 25 maps.
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

- The exact Showdown source lock and input digest live in the generated JSON
  manifest and are checked by the verifier; this player/developer overview does
  not duplicate a drift-prone digest.
- Pool: 311 species/form variants and 444 role templates
- Live rules: Species Clause by National Dex family, at most one Mega,
  type/weakness limits, weather and field compatibility, role-driven moves,
  Ability filtering, and Showdown item selection without a separate Item
  Clause
- Player party: temporarily normalized to level 80 and restored exactly on
  retirement or defeat
- Opponent scaling: one additional slot gains one level after each win, then
  the whole team advances
- State: dedicated variables; no Battle Tower record contamination

The GBA generator now samples each National Dex family in one linear pass
instead of nested full-table scans. Authored runtime tests generate 96 sets
across 16 seeds and check level, 66 Stat Points, moves, Ability, species, item,
and diversity. Transform-only Ditto and Fake Out plus Last Resort remain
intentional native Showdown sets. A fresh run is still required for the exact
candidate.

## Evidence

- Source verifiers currently report 82 complete Signs, 25 visible quests,
  seven ordinary-wild roots, 32 physical one-offs, and 101 rooted legendary
  families with 82 terminating prerequisite chains.
- The Circuit source verifier reports 311 variants and 444 templates and checks
  its generated source lock, configured Abilities, save variables, and reward
  schedule.
- Runtime coverage is part of the current 19-filter manifest, which requires
  at least 90 Champions tests and 42 Commander tests and accepts zero TODO or
  known-failing results. These minimums are a contract, not a recorded pass for
  a changed working tree.
- Static wiring and deterministic generation cannot prove visual encounter UX,
  real-device timing, long-run team quality, or human discovery/pacing.
