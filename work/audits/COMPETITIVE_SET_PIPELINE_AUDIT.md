# Emerald Champions competitive-set pipeline audit

## Executable contract

Ground truth is the generated runtime table in
`src/data/pokemon/emerald_champions_battle_sets.h` and its application code in
`src/emerald_champions_battle_sets.c`. The JSON manifest and generator are the
reproducible authoring layer; the supplied doubles handbook is evidence, not a
replacement for the configured species, item, form, and Ability tables.

- 1,258 species/form defaults.
- 1,361 alternatives: 274 preserved/source-backed orientations and 1,087
  second-role syntheses from the live learnable, stat, Ability, and move tables.
- 2,619 complete orientations.
- 2,524 non-Mega orientations eligible for ordinary wild sampling.
- 95 Mega orientations covering every one of the 92 campaign stones.
  - 75 are Pokemon Champions M-B ladder-backed handbook roles.
  - 20 are Emerald Champions extension roles derived from current form data
    and the authored doubles corpus.
  - Magearnite has roles for normal and Original Color Magearna.
  - Tatsugirinite has roles for Curly, Droopy, and Stretchy Tatsugiri.
- Pre-Mega distribution: 1,250 species/forms have two non-Mega roles and eight
  have three. Including Mega roles, 1,161 have two choices, 91 have three, and
  six have four.
- All 616 species/forms named anywhere in the parsed encounter data resolve a
  preset; the reachable Hoenn tables currently expose 592 unique forms.

## Defects closed

1. Wild selection no longer uses the tutor-visible list. Owning the Mega Ring
   therefore cannot add required-stone roles to a wild pool. The non-Mega
   reservoir sampler is inventory-independent and gives every eligible role
   equal probability.
2. Finite Legendary Sign encounters no longer enter ordinary wild-set
   randomization. Curated table-seeded Ultra Beasts and Paradox Pokemon remain
   eligible by design.
3. The previous importer preserved only 22 Mega roles. It now imports all 75
   ladder-backed handbook roles and adds the 20 extension orientations needed
   for complete campaign-stone/form coverage.
4. Required-stone roles are hidden until Mega Ring access, never supply their
   stone, and record the configured transformed Ability. The base Pokemon keeps
   a legal base Ability until Mega Evolution occurs.
5. Zygardite is authored on 50% Power Construct Zygarde, not unreachable base
   Zygarde. Applying the preset preserves Power Construct; the source contract
   proves the below-half-HP Complete transition followed by Zygardite Mega.
6. All 44 items in the live evolution archive are now protected from tutor
   replacement. This includes Deep Sea Tooth and Deep Sea Scale, whose held
   battle effects mean item sort type alone cannot identify them.
7. The Stat Point inference no longer reads `physical wallbreaker` as
   `physical wall` (or the special equivalent). Six affected breakers now
   receive their intended offensive maximum.
8. Thirty-two source-backed orientations received explicit executable review:
   unusable Protect slots on Choice/Assault Vest roles, three impossible
   Ability/item activations, two individually reviewed White Herb cases, one
   reversed Kingdra attack allocation, ten damage-independent support spreads,
   and two small nature/move corrections.
9. Duplicate names and duplicate complete orientations are rejected per
   species. Dead Light Clay, Power Herb, Eject Pack, Throat Spray, Poison Heal,
   Flare Boost, Harvest, Choice-item, and Assault Vest configurations are also
   rejected. White Herb is intentionally not governed by a false global rule;
   it can be legitimate anti-Intimidate or anti-speed-control tech in doubles.
10. All 72 former move-access gaps now have exact source disposition in
    `docs/emerald_champions_move_access_review.json`: 22 official inherited
    assignments, two official historical/event assignments, 41 deliberate
    Inclement extensions, and seven unsupported assignments replaced with
    legal role-equivalent moves. The 65 retained rows are runtime-tested; 58
    rows collapse to 38 unique native tutor extensions.
11. The pinned Showdown projection now inherits pre-evolution learnsets as the
    native tutor does. This directly resolves Raboot/Cinderace High Jump Kick
    and Leavanny Sticky Web instead of misclassifying them as exceptions.
12. Weather Institute Castform, Steven's Beldum, and every Devon fossil revival
    now use the same unbiased non-Mega prepared-Pokemon transaction. Party and
    PC delivery preserve the full orientation; no-room attempts remain
    retryable; party-held items receive the restoration baseline before the
    finite story flag advances.
13. Every direct species/form now has at least two genuinely distinct non-Mega
    orientations. The generator uses the supplied handbook (its exact SHA-256
    is pinned), current legal learnables, configured stats and Abilities, and
    doubles role blueprints. It removed the two preserved item-only duplicates
    (Chien-Pao and Kilowattrel) before supplying real control alternatives.
    The verifier compares unordered moves, Ability, nature, and Stat Points so
    a rename, move-order shuffle, or held-item swap cannot satisfy the floor.

## Proof

- Competitive-set static verifier: PASS.
- Wild distribution, campaign roster, regional starter, legendary
  availability, and solo-evolution gates: PASS.
- The current runtime manifest selects at least 90 Champions tests with zero
  accepted known-failing or TODO debt. A fresh candidate run, not an older
  retained ELF or focused pass count, is the runtime evidence required now.
- Runtime coverage includes all imported sets, every ordinary species/form,
  Mega visibility/no-grant behavior, wild Mega exclusion after Mega access,
  all evolution-item protections, and the staged Zygarde path.

## Remaining quality limits

- This is source/runtime verification, not empirical metagame validation of
  all 2,619 orientations. Individual damage benchmarks, partner fit, and usage
  quality still require campaign playtesting.
- No final production ROM or full release suite was built in this subtask; a
  release pass must regenerate ROM size/digest evidence after all parallel work
  is integrated.
