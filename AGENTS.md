# Working on Emerald Champions

Preserve the current game design. Fix demonstrated bugs and refactor without changing behavior. Discuss changes to progression, availability, economy, difficulty tuning, or player options before implementing them. Retry restarts the current battle; Reload restores the last saved game. Both are intentional and must remain separate.

The user-approved campaign direction (September 5, 2026) intentionally demands competitive knowledge from the opening. Easy/Normal/Hard are global opponent-level calibration offsets, not different teams or AI. Prefer coordinated, demanding battles over filler. The route-conversation reduction and gym coordination changes are deliberate; old encounter totals, percentage quotas, and historical prose must not restore retired fights or override the current authoring. This campaign revision does not require compatibility with pre-tuning save files. New design changes can be discussed and revised rather than treating old documents as immutable requirements.

The user subsequently authorized improvements to writing, pacing, choreography, and story during the campaign audit, including revisiting earlier scenes, toward a richer Pokémon odyssey. That authorization covers coherent improvements within the campaign direction above. The audit also includes the complete legendary-Sign pipeline and standard, modern, and imported Inclement custom Mega Evolutions; verify the actual compiled roster and runtime behavior rather than assuming imported content is present.

The user further approved mandatory Devon research for each individual Legendary Sign, one attempt for every one-off legendary encounter, with permanent removal after a knockout or escape, and a person-by-person audit of required story handoffs so players know their next destination and purpose without an external guide. Local Sign activation must not bypass Devon. Keep lost and caught states distinct, warn players to save before one-shot legendary battles, and consume the encounter after fleeing as well as a knockout. Reloading a save from before the encounter remains allowed. Older optional-Devon, knockout-retry, and fleeing-retry behavior and checks are superseded by this direction.

The user also approved keeping Terapagos in Cave of Origin B1F with a researched-only encounter in that otherwise quiet room, Dondozo as a wild encounter near the submerged submarine, and distinctive Dive encounter rosters with rare finds. No ordinary wild encounter may be rarer than 4%; preserve the flattened 14%–4% distribution to avoid grind. Preserve the separate Devon and one-shot rules for legendary species while implementing these underwater encounter improvements.

The separate full battle audit subsequently received the user's instruction not to build another ROM until every battle has been reviewed. Hold further ROM builds and combined integration until that audit reports completion. Existing immutable test artifacts may still be used for checks; do not present them as containing later concurrent battle edits.

That review-before-build condition was fulfilled when the battle-audit owner confirmed all 468 campaign encounter groups, 516 branches and the Frontier-generator review complete, with the stable boundary recorded in `work/battle-review-2026-09-06/final-source-hashes.json`. Combined builds and scoped runtime verification may now proceed under the existing task authorization. Source review still must not be described as empirical playtesting of every battle.

The user clarified on September 6, 2026 that every campaign trainer battle is doubles except the opening Route103 rival fight, which remains singles with one Pokemon. Preserve that exception and the one-opponent opening; multi battles remain doubles on the field. Review movesets, AI, and team composition in blocks of about 20 encounters. The user explicitly prohibited building any ROM until every battle has been reviewed; finish the full review before any further ROM build.

The all-battle source-review boundary was completed on September 6, 2026: all 468 campaign encounter groups and 516 branches, plus the live Frontier generator. See `work/battle-review-2026-09-06/REVIEW.md` and `final-source-hashes.json` for the stable review boundary. Static checks, production-function host tests and targeted object compilation pass; this review did not produce a ROM or claim full battle-play validation.

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

When a check fails, audit the check before treating the failure as a game defect. Verify its intended contract, current inputs, artifact freshness, and assertion logic. Correct stale or invalid tests while preserving valid invariants, and record the reason for each test or reference change. A passing stale binary is not evidence about current source.

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
