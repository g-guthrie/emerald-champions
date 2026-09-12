# Working on Emerald Champions

Implement the approved Emerald Champions Game Book Draft 3.1 in the original
pokeemerald-expansion engine. The user explicitly authorized the new campaign,
overworld scenes, graphics and code refactor on September 11, 2026. The book is
`/Users/gguthrie/Desktop/Pokémon Emerald Champions/Game Blueprint/Emerald_Champions_Game_Book.txt`.
Keep the replacement engine parked. Consolidate rules at their actual owner;
moving code or lowering a heuristic seam count is not proof of simplification.
New or restaged scenes require headless screenshots and user visual review.

- Preserve concurrent user work. Use Luna with high reasoning for independent
  subagent tasks; the main agent owns integration and shared builds.
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
