# Working on Emerald Champions

Preserve the current game design. Fix demonstrated bugs and refactor without changing behavior. Discuss changes to progression, availability, economy, difficulty tuning, or player options before implementing them. Retry restarts the current battle; Reload restores the last saved game. Both are intentional and must remain separate.

The user-approved campaign direction (September 5, 2026) intentionally demands competitive knowledge from the opening. Easy/Normal/Hard are global opponent-level calibration offsets, not different teams or AI. Prefer coordinated, demanding battles over filler. The route-conversation reduction and gym coordination changes are deliberate; old encounter totals, percentage quotas, and historical prose must not restore retired fights or override the current authoring. This campaign revision does not require compatibility with pre-tuning save files. New design changes can be discussed and revised rather than treating old documents as immutable requirements.

The user subsequently authorized improvements to writing, pacing, choreography, and story during the campaign audit, including revisiting earlier scenes, toward a richer Pokémon odyssey. That authorization covers coherent improvements within the campaign direction above. The audit also includes the complete legendary-Sign pipeline and standard, modern, and imported Inclement custom Mega Evolutions; verify the actual compiled roster and runtime behavior rather than assuming imported content is present.

On September 8 the user superseded the old mandatory-Devon and one-attempt legendary design. Devon is an optional guide. Field Signs use deliberate landmark interactions instead of random encounter hunts, preserve badge/story gates, and use species requirements sparingly. A failed encounter returns after leaving and re-entering its area; captures and earned rewards remain permanent. Restore previously lost encounters without resetting catches or reward entitlements. Keep the full, revisitable legendary guide accurate.

The user also approved keeping Terapagos in Cave of Origin B1F with a deliberate landmark encounter in that otherwise quiet room, Dondozo as a wild encounter near the submerged submarine, and distinctive Dive encounter rosters with rare finds. The September 7 revision raises the minimum ordinary species encounter chance to 5% per encounter method, with Feebas the sole exception. Preserve the flattened distribution to avoid grind. Use the relaxed legendary rules above while preserving these underwater habitats.

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

## September 7 exploration and encounter direction

Wild distribution changes require a game-wide appraisal of habitat identity, useful early unevolved/fast-evolving options, later discoveries, physical access and Mega-capable evolution timing. Preserve Magnemite's power-station association and the Duskull line's ghost-tower identity. Mega Stones now come from world pickups, meaningful NPC gifts and three one-time Berry Master exchanges (20 garden berries each); the Mega Ring enables use but does not unlock a free stone archive. Keep free battle supplies, the evolution-item archive and accessible Poké Balls. See `docs/REGIONAL_ENCOUNTERS_AND_REWARDS.md` for the current regional pass and source catalogue.

On September 8 the user revised the Daycare plan: babies should be available during the low-cap opening, not locked behind breeding. Munchlax and Mime Jr. belong before Roxanne, Chingling in Rusturf Tunnel, Happiny in the opening meadow roster, and Mantyke on Dewford's Old Rod. Preserve displaced families elsewhere. The Daycare rewards one egg produced by two deposited compatible parents and then hatched with Kangaskhanite (gift eggs, including Togepi, do not qualify), and Audinite becomes an ordinary Route117 NPC gift. Keep short breeding/hatching waits and free stat/move editing. Do not introduce the proposed Hitmon breeding restriction. The user asked for an explanation of current generation rules and Mega Kangaskhan nerfs, not a battle-rules rebalance.

## September 8 test-suite reduction

The user requested an aggressive deletion of tests and gates that distract from development or freeze old design. Keep a small suite protecting current behavior and demonstrated failure modes. Do not restore the retired inherited corpus, historical parity gates, prose locks, strategy quotas or test-count floors. A new test needs distinct observable regression value; update or delete superseded expectations with the gameplay change. Preserve save/artifact integrity and meaningful transaction/battle regressions, without claiming the focused suite certifies the whole game.

The September 8 trainer hierarchy discussion preserves demanding four-Pokémon ordinary doubles and the badge-level floor. Difficulty offsets are the user's global playtesting calibration tool. Trainer-roster hierarchy and stat-point scaling remain discussion-only in the legendary-overhaul task.
