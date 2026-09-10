# E0015 — Ivan: bounded executable-AI repair complete

Historical pre-final-stat evidence below. Current stats, retained changes and
limitations: [September9 final Gen9 review](e0015_gen9_2026_09_09.md),8/10.

Intention rating: **7.5/10**, not a measured win rate. Actual compiled party and
native generation are the authority. This completes the individual scenario
review, not every matchup, a whole campaign playthrough or release readiness.

## Actual party retained

Normal zero-badge generation produces all four at level12:

| Pokémon | HP/Atk/Def/SpA/SpD/Spe | Item, ability | Moves |
| --- | --- | --- | --- |
| Tentacool | 67/16/20/20/32/57 | Eviolite, Liquid Ooze | Acid Spray, Sludge Bomb, Icy Wind, Protect |
| Clamperl | 36/21/29/63/21/48 | Deep Sea Tooth, Shell Armor | Shell Smash, Muddy Water, Ice Beam, Protect |
| Chewtle | 39/61/20/15/17/51 | Life Orb, Strong Jaw | Waterfall, Ice Fang, Crunch, Protect |
| Chinchou | 75/15/19/59/22/24 | Sitrus, Volt Absorb | Thunder Wave, Muddy Water, Thunderbolt, Protect |

All16 prepared moves, abilities and66-point spreads are source-legal. Compiled
species uses are Tentacool3, Clamperl2, Chewtle3, Chinchou3. Tentacool and Chewtle
debut here; this fast Clamperl differs from Darian's bulky investment. No species,
move, item or point change was accepted during this AI restart. Retained Speed
allows Tentacool57 to prepare a target before unboosted Clamperl48; Shell Smash
would make Clamperl96 and reverse that partnership's order. Tooth doubles native
special Attack, not the already-rounded final damage. Strong Jaw boosts Ice Fang
and Crunch, not Waterfall. Waterfall retains reliable STAB and a flinch chance;
Liquidation is legal but its power/Defense-drop trade is not an automatic upgrade.

Early responses include Mienfoo's Fake Out/Taunt/Knock Off, Lotad or Roselia's
Grass pressure, Foongus's Spore/Clear Smog and Ferroseed's Grass/Knock Off tools.
Giga Drain into Tentacool pays Liquid Ooze; Chinchou's Volt Absorb protects only
its own slot and does not redirect an Electric attack away from its partner.

## Accepted executable changes

- Ivan now uses **SETUP,PRESSURE**. Its HP-trade preference is an executable
  per-trainer flag, not a compulsory turn script or a human-command prediction.
  Conservative remains: current code uses that flag for friendly-fire/Z-move
  caution, not as a general defensive personality. His current moves have no
  ally-damaging spread attack or Z item.
- Pair forecasting now removes the offensive benefit of **Deep Sea Tooth** or
  **Life Orb** after an earlier eligible Knock Off. Previously it could count
  Tooth's doubled special damage after losing the item; existing Orb removal
  already canceled recoil while incorrectly retaining the boosted damage.
- Native raw item-present/item-absent ranges preserve actual rounding. They
  reuse the existing disjoint Plus/Minus anchor storage, adding a move-slot
  source mask and a remover mask rather than another damage matrix. Calculation
  occurs once per eligible slot/target, never inside the candidate-pair search,
  and is skipped when no other living actor has usable Knock Off.
- Item loss follows targeting, protection, Substitute, sanitized Sticky Hold,
  native removability and hit/action chance. Uncertain loss uses the existing
  bounded effect branch; Orb damage and recoil cancellation share that branch.
  Item, known item, held effect, battle scratch and RNG are restored after the
  counterfactual. Raw ranges receive action-time Sash/Sturdy/Endure protection.

## Native evidence

The final actual-party run passed **28 parameters across 9 groups**: six opening
observations, four reserve observations, four setup-window observations, two
unrestricted Knock Off observations, two Tooth controls, two Orb controls, two
four-/six-player timing cases, two Easy/Hard calibration cases and four four-turn
follow-through cases. Trainer choices were unrestricted except the explicitly
mechanical item controls. PASS alone is not a tactical correctness assertion.

- Native Clamperl Ice Beam deals35 with Tooth retained versus18 after faster
  Mienfoo Knock Off; the latter removes14HP and leaves Clamperl22/36.
- Native Chewtle Waterfall deals27 and pays3HP Orb recoil when retained, versus
 21 damage and zero recoil after faster Knock Off. The removed-item case takes
 20HP from Knock Off and finishes19/39. Full trainer menus and native stats stay
  intact; only this mechanical comparison prescribes their moves.
- On the Grass/Electric board, old SETUP-only guards both leads turn1, then
  Acid Spray/Muddy Water trades Clamperl for Pachirisu turn2. Accepted pressure
  prepares Lotad turn1 and executes Acid Spray/Ice Beam turn2, defeating Lotad
  while Clamperl remains36/36 and Tentacool39/67. This is a sampled improvement,
  not a universal matchup result.
- Against Fake Out/Taunt, pressure exposes Tentacool to the initial Fake Out
  chip (61/67 afterward), but turn2 Acid Spray/Muddy Water defeats Pachirisu
  while Clamperl36/36 attacks through Taunt instead of attempting setup.
- The four-turn neutral board uses Acid Spray twice, then Sludge Bomb and an
  entering Chinchou's Thunderbolt to defeat Eevee. Tentacool40/67 and
  Chinchou13/75 survive; Clamperl and Chewtle remain healthy on the bench.
  Lotad replaces the fainted Eevee; Timburr finishes69/81.
- Against fast specially bulky Pachirisu/Timburr, Chinchou replaces Clamperl,
  then uses Muddy Water on turns3/4. Pachirisu and Tentacool faint turn4, actual
  Chewtle enters at39/39, Chinchou remains75/75 and Timburr37/51. Both four- and
  six-player variants reproduce these respective follow-through outcomes.
- Reserve-only boards still initially double-Protect. Their turn2 Waterfall
  pairs with Thunderbolt or Thunder Wave depending on the board. Neither this
  remaining wait nor unobserved Shell Smash use is concealed as perfection.
- Actual Easy/Hard lookup fallback produces levels10/14 with unchanged teams.
  Final normal decisions measure39–40 warm frames and45–46 including native
  move-cache rebuilding, about0.65–0.77 seconds at60Hz. This is still perceptible,
  not a worst-case guarantee or evidence every freeze is resolved. Skipping
  unavailable item-loss anchors reduced the prior42/48-frame pressure cases.

The longer diagnostic initially omitted player replacements at the newly
observed turn4 faints. Its incomplete-turn reports were fixed with explicit
Lotad send-outs, not gameplay edits or claims of engine freezes. A calibration
fixture also needed a bit-field cast to compile. All temporary Ivan includes
were removed after the accepted run.

One compact generic item-timing regression is retained, with four parameters:
Tooth favors physical Waterfall after fast Knock Off and boosted Ice Beam before
slow Knock Off; Orb favors fixed Seismic Toss after fast removal and Strength
before slow removal. Disabling each corresponding item path independently
reproduces the wrong fast-removal move. This uses synthetic stats/moves and does
not claim to be Ivan's team. The existing transaction regression now also
exercises Tooth/Orb restoration; no separate large per-encounter suite is kept.
After removing all temporary includes, all **28 retained shared AI groups**
passed, including item timing and transaction restoration. Trainer/master
projection verified all516 configured branches, and whitespace checks passed.
That projection count is static synchronization, not individual AI completion.

## Remaining limits / improvement path

Shell Smash was not selected in these unrestricted threatening-menu scenarios;
do not claim a demonstrated setup sweep or force it to make the book look true.
More convincingly exploiting safe setup windows and reducing reserve double
guards would raise this encounter's rating. Defensive matchups can still induce
longer pivots, and latency remains noticeable. Native full-game playtesting is
still required. Multihit/HP-powered/Max/Z cases, combined active Plus/Minus and
item boosts on the same special slot, and full independent effect correlations
remain outside the new item-loss model. Existing shared regressions and static
projection checks do not certify those unsupported cases or the entire game.
