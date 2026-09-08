# Early battles — complete individual review

This volume covers every snapshot trainer branch through E0250: **266 branches, 224 encounter groups, 1,146 baseline Pokémon slots**. Stable encounter IDs have gaps; none are silently invented to fill them. Every branch has an individually written assessment, complete baseline, exact disposition, shared AI dependencies and specific acceptance scenarios. Automated rendering reproduces loadouts and links; it does not substitute for those individual reviews.

Dispositions: {'KEEP': 116, 'REPAIR': 143, 'REVISE': 7}. **257 of266 complete baseline Pokémon loadouts are preserved exactly.** Roster edits are limited to the six newly authorized opening branches, Roxanne's iconic Nosepass restoration, Cristian's measured Falinks Speed repair, and Mary's inactive Rufflet ability repair. Other REPAIR entries correct mechanical claims, party-order explanations or dialogue while preserving exact Pokémon fields.

## Superseding user decisions

The first rival is now FOUR-Pokémon DOUBLES, after choosing TWO distinct regional starters and a doubles Birch rescue. The rival receives the unchosen third starter beside Pikachu. Ordinary wild/legendary encounters remain singles. All trainer classes use the most coherent available strategy; rank cannot disable tactical competence. Normal/Medium opponent levels never fall below the current live cap minus 2, including gyms. Historical baseline singles and negative offsets are evidence only, not the final specification.

The two-starter opening and 54-choice resolver are specified in [the opening chapter](../../chapters/01-opening-and-starters.md).

## Review blocks

- [E0001-0020](0001-0020.md) — 24 branches
- [E0021-0040](0021-0040.md) — 28 branches
- [E0041-0060](0041-0060.md) — 26 branches
- [E0061-0080](0061-0080.md) — 21 branches
- [E0081-0100](0081-0100.md) — 21 branches
- [E0101-0120](0101-0120.md) — 22 branches
- [E0121-0140](0121-0140.md) — 24 branches
- [E0141-0160](0141-0160.md) — 21 branches
- [E0161-0180](0161-0180.md) — 19 branches
- [E0181-0200](0181-0200.md) — 16 branches
- [E0201-0220](0201-0220.md) — 13 branches
- [E0221-0240](0221-0240.md) — 17 branches
- [E0241-0250](0241-0250.md) — 14 branches

## How to validate implementation

For a changed entry, first check exact final materialization, species/form/move/item/ability legality against this configured build and the shared party/floor contract. Then run the entry’s specific paired-action and replacement scenarios at the actual cap. Include valid alternative player lines, immunity/field changes and a disrupted opening. A team is not proven correct merely because its intended combo succeeds in one friendly fixture. Preserve successful clever solutions.

The source alignment comparison is read-only: `emerald_champions_teams.compile_master` on the full snapshot and native materialization through E0250 were compared as strings, without running write commands. Results: {"changed_team_move_legality_issues": [], "authored_branches": 266, "encounters": 224, "pokemon_slots": 1146, "all_individually_reviewed": true, "master_compile_identical": true, "native_early_rewrite_identical": true, "native_examined": 266, "native_problems": [], "runtime_tested": false}.

Read [regional rival profiles](regional-rival-profiles.md) for54stage profiles and270exact resolved variants. Read [exact Stat Point benchmarks](points-benchmarks.md) for the opening, Cristian and Roxanne. Read [gym dialogue repairs](gym-dialogue.md) for exact native text replacements. [Review JSON](../../review/early-battles.json) is keyed by trainer ID and contains complete baseline/final teams. [Validation JSON](../../review/early-battles.validation.json) records scoped static evidence.

Stat Points are fixed bonuses before nature in the snapshot Champions formula (`src/pokemon.c:1425`), so low-level Speed cannot be inferred from familiar EV-scaled expectations. Cristian’s repair specifically uses this formula. Ability names and move effects are drawn from the configured source; current simulator reputations are not substituted for this build.

No ROM was built, no game source or save was changed, and no fresh runtime wins are claimed.
