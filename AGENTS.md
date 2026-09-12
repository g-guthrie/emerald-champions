# Working on Emerald Champions

Implement the reviewed Emerald Champions Game Book v4 in the original
pokeemerald-expansion engine. The user created an active full-build goal on
September 12, 2026 covering the new story, required overworld scenes, tasteful
graphics/choreography, cohesive systems, hybrid AI and code refactoring. The book is
`/Users/gguthrie/Desktop/Pokémon Emerald Champions/Game Blueprint/Emerald_Champions_Game_Book.txt`.
Keep the replacement engine parked. Consolidate rules at their actual owner;
moving code or lowering a heuristic seam count is not proof of simplification.
The single authorized fresh Astra High full-book review is complete and its
dispositions are recorded in section 26. Keep design changes in this one book
and update authored inputs and their generated consumers together. Every retained
trainer encounter requires actual native doubles testing and individual strategy
assessment, reproducible cases and shareable evidence as specified in section 25.
New or restaged scenes require headless screenshots and user visual review;
collision validity alone does not establish good choreography or presentation.

- Preserve concurrent user work. The single Astra High book review is complete.
  The user subsequently authorized one additional Astra High implementation
  agent for native integration and visual QA of new/imported overworld areas,
  working in the separate `overworld-integration` checkout. No further agents.
  The main agent owns battle/AI, trainer data, caps/economy, existing story battle
  chains, final integration and publication. Coordinate shared files and flags
  before edits; keep builds and artifacts inside each owner's checkout.
  The main agent alone edits the canonical book; send proposed clarifications.
- Make the smallest coherent change. Prefer deletion and reuse over new layers.
- Preserve gameplay while refactoring; fix demonstrated defects. Discuss new
  progression, economy, difficulty, availability or overworld design unless the
  user has already authorized it. Do not add signs, actors or map tooling as a
  substitute for understanding the existing art and choreography.
- Source defines the game. Historical audits, prose snapshots and old test
  expectations must not restore superseded behavior.
- Keep one owner for each rule. Edit authored inputs and regenerate their actual
  consumers together; do not patch generated tables independently.
- Current saves target fresh games. Preserve current-run save integrity,
  no-room reward retries and temporary-party restoration; no legacy migrations.
- Retry restarts the battle; Reload restores the save. Field medicine remains
  available. Keep free preparation and its protection against resale exploits.
- Trainer teams and shared preparation presets have separate authorship.
  Preserve bespoke teams and exact EV choices. AI forecasts must share native
  mechanics; the complete opponent decision budget is 1.2 seconds.
- New games currently initialize Hard regardless of title-screen difficulty.
  Changing that preference behavior requires a design decision.

## Source ownership

`src/`, `include/` and `data/` contain the game. Authored trainer/preset inputs
and imported reference data live in `data/emerald_champions/`. Use their existing
consuming generators in `scripts/`; runtime move-access policy belongs in
`emerald_champions_battle_sets.c`, ordinary Pokémon operations in `pokemon.c`.
Tests belong in `test/` and `tests/`. Build and runtime commands are in
[docs/VERIFICATION.md](docs/VERIFICATION.md). Logs and scratch artifacts belong
in ignored `work/`, not new historical documents beside the source.

## Verification

Audit every failure before changing production code or expectations: inspect
the contract, fixture, configuration and source/artifact freshness. Correct
obsolete checks; never weaken a valid check just to pass. Keep the suite small
and test observable behavior in proportion to risk. Add a regression only when
it protects a distinct, necessary invariant absent from existing coverage.

Build and stamp the same source tree. A fresh stamp cannot repair stale objects.
Keep campaign traversal with automatic battle resolution separate from native
combat evaluation. Neither static checks nor automatic victories establish
balance, strategy quality or complete playability. Report exactly what ran and
what remains unverified. Commit or publish only within the authorized scope.
