# Working on Emerald Champions

Preserve the current game design. Fix demonstrated bugs and refactor without changing behavior. Discuss changes to progression, availability, economy, difficulty tuning, or player options before implementing them. Retry restarts the current battle; Reload restores the last saved game. Both are intentional and must remain separate.

## Canonical ownership

- `src/`, `include/`, and game files under `data/`: executable behavior and compiled tables.
- `data/emerald_champions/`: authored trainer/preset data and imported reference datasets used by generators. Preserve authored values; synchronize their materialized game tables through the relevant generator and comparison check.
- `tests/campaign/`: gameplay traversal definitions, checkpoint recipes, and campaign reference results.
- `tests/headless/` and `tests/reference/`: runtime harnesses and explicit reference baselines.
- `scripts/` and `tools/agent_player/`: verification, generation, and the two testing pipelines.
- `docs/`: explanations derived from code. Never use prose as proof or as a build input.

Keep one owner for each rule or dataset. Do not revive an obsolete generator or duplicate a rule in a verifier merely to make a gate pass. Inspect the actual consumer before changing generated data. Preserve save and map-object IDs unless a necessary migration is explicitly implemented and tested.

Work in this order: question the requirement, delete unnecessary code or artifacts, then simplify what remains. Do not preserve an implementation merely because it already exists, or optimize a process that can be removed. Preserve agreed gameplay behavior while changing its implementation.

## Verification

Build and toolchain commands are documented in `docs/VERIFICATION.md` and implemented in the Makefile and CI workflows. After a successful release build, bind both artifacts to their inputs:

```sh
python3 scripts/stamp_release_inputs.py
python3 scripts/verify_emerald_champions_release.py
```

The curated runtime suite builds once and runs its selected test groups:

```sh
python3 scripts/run_emerald_champions_runtime_gates.py --jobs 4
```

For a Docker-built test ELF, create its stamp inside the same built tree and copy both files together; `--run-only` must reject different source inputs. A content stamp verifies declared input and artifact identity, not that every cached object was rebuilt.

The gameplay pipeline uses automatic battle resolution to test traversal. The battle-testing pipeline disables that automation. Preserve both. A traversal result does not establish combat difficulty; a reported battle win is not independently verified outcome evidence.

Report exactly what was exercised. Static checks, host C regressions, actual battle tests, visual scenarios, and fresh-save traversal are different evidence. Do not turn known failures into success by broadening allowances. Do not claim complete campaign coverage or release readiness without corresponding runtime evidence.

## Shared work

Preserve concurrent edits and check the current file before integrating a patch. Do not overwrite another thread's generated or authored work with an older snapshot. Commit or publish only within the user's requested scope.
