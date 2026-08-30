# Emerald Champions Verification Audit

Audit date: 2026-08-29
Audited branch: `release/full-source-audit`
Authority: executable source, build rules, test runner, and GitHub run logs. Design documents are checked only as generated or synchronized projections of source.

## Verdict

The verification stack is useful and mostly well-factored. The static release suite is not the source of the long development loop: all 25 source gates complete locally in about 12 seconds. The expensive step was compiling the complete test ELF, which contained the whole engine and all upstream tests even when only a narrow filter would run. Required PR/release CI now derives a 41-file source allowlist from its filter manifest; a scheduled/manual workflow preserves full-corpus syntax/link coverage.

The stack proves structural consistency, selected runtime mechanics, ROM layout, and a minimal entrypoint boot. It does **not** prove that 513 encounters are fun, correctly rated, or human-playable from a fresh save. A green release run must never be described as a full campaign playthrough.

Two high-risk false assurances existed:

1. CI deleted and relinked the same test ELF for every curated filter even though `gTestRunnerArgv` is a post-link fixed-size buffer.
2. A metadata-free source bundle silently skipped `git diff --check` while still presenting the rest of the release verifier as authoritative.

Both are corrected in this audit.

The first clean Linux compile also exposed a duplicate hidden-item flag name that every C translation unit warned about but CI did not treat as fatal: `FLAG_HIDDEN_ITEM_UNDERWATER_124_BIG_PEARL` named both `+0x0B` and `+0x24`. The second event is now `FLAG_HIDDEN_ITEM_UNDERWATER_124_BIG_PEARL_2`, and the state-ID verifier now detects duplicate macro names plus resolvable base/offset collisions instead of parsing only direct numeric literals.

## Evidence levels

1. **Source invariant:** parser or graph proves a property of checked-in source.
2. **Generated freshness:** generated evidence matches its declared source snapshot.
3. **Runtime regression:** a filtered test runs the compiled engine in mGBA and checks player-visible behavior or state.
4. **Entrypoint smoke:** the release ROM reaches its first GBA VBlank call without crashing.
5. **Scenario test:** a scripted save/state traverses a real campaign sequence.
6. **Human playthrough:** a person completes the campaign and evaluates difficulty, pacing, presentation, and fun.

The current required CI reaches levels 1-4. It does not reach levels 5-6 for the complete campaign.

## GitHub workflow inventory

### `.github/workflows/build.yml`

Required release workflow. It builds `pokeemerald-release.gba`, runs the source verifier, performs a first-VBlank entrypoint smoke, builds the test ELF once, runs the curated runtime manifest, and reports ROM identity/memory use.

Latest observed `main` run before this audit (run `33275660117`):

| Step | Wall time |
|---|---:|
| Toolchain installation | 18 s |
| Release ROM build | 5 m 00 s |
| Static release verifier | 10 s |
| First-VBlank smoke | <1 s |
| Runtime gates | 6 m 06 s |
| Entire job | 11 m 46 s |

Within the runtime step, approximately 4 m 46 s was the one-time compilation of the test objects. Eleven redundant link/fix/patch cycles totaled 5.57 seconds; repeated Make startup added a little more. Eliminating relinks is correct and measurable, but it does not turn a cold build into a seconds-long job.

Changes:

- `actions/checkout@v4` -> `actions/checkout@v7`.
- `actions/cache@v4` -> `actions/cache@v6`.
- Removed five ineffective environment variables (`GAME_REVISION` and `GAME_LANGUAGE` were not Make inputs; the others restated defaults).
- Renamed the boot step to say exactly what `-S 5` proves: first VBlank, not a campaign boot or menu test.
- Replaced eleven destructive ELF rebuild calls with one explicit curated runner.

The action majors were verified against the primary GitHub release records on 2026-08-29: [checkout v7.0.1](https://github.com/actions/checkout/releases/tag/v7.0.1), [cache v6.1.0](https://github.com/actions/cache/releases/tag/v6.1.0), [configure-pages v6.0.0](https://github.com/actions/configure-pages/releases/tag/v6.0.0), [upload-pages-artifact v5.0.0](https://github.com/actions/upload-pages-artifact/releases/tag/v5.0.0), [deploy-pages v5.0.0](https://github.com/actions/deploy-pages/releases/tag/v5.0.0), and [request-action v3.0.0](https://github.com/octokit/request-action/releases/tag/v3.0.0).

### `.github/workflows/docs.yml`

Main-branch GitHub Pages deployment, not a ROM correctness gate. It builds the mdBook and deploys only if Pages is enabled. The runner is pinned to Ubuntu 24.04, actions use the current supported majors, and the downloaded mdBook 0.5.1 archive is now checked against its published SHA-256 before extraction. mdBook can return success after logging failed `{{#include}}` directives, so the workflow now treats any emitted `ERROR` as a failed documentation build.

### `.github/workflows/full-test-compile.yml`

Weekly and manual broad compile/link coverage. This workflow deliberately compiles all 968 upstream test C files but does not claim that the full upstream runtime suite passes. It keeps syntax/link drift visible without charging every pull request for hundreds of unrelated test translation units.

### Deleted `.github/workflows/labels.yml`

This inherited contribution-governance workflow was not a release test. Its main job evaluated `github.event.review.state` on ordinary `pull_request` events, so GitHub created skipped workflow runs on PR open/synchronize/label events. The repository has no branch protection or ruleset requiring it. Removing it eliminates noise without reducing game verification.

### Remote enforcement

GitHub reported `main` as unprotected and returned no repository rulesets. Therefore CI is currently advisory: a green `build` is not technically required before a direct push or merge. Enable a branch ruleset after the final workflow lands if release enforcement is desired.

## Static release-gate inventory

| Gate | What it actually proves | Boundary |
|---|---|---|
| Core services | Difficulty, Leveler/Vial, tutor, Ability menu, vendors, Rare Candy, field licenses, and 16 Center layouts retain exact hooks | Structural; does not render or play the menus |
| Finite rewards | 47 authored script rewards and map TM replacements remain finite progression rewards | Enumerated producers, not a general economy simulation |
| Reward economy | Inert vitamins/X items are unobtainable, finite rewards are coherent, world Mega Stones are unique | Policy-specific source scan |
| Wild distribution | Live wild JSON conforms to chosen coverage and exclusion rules | Contains design policy; must not freeze rejected early species |
| Route signs | 32 physical signs call the live method-grouped encounter formatter and percentages remain absent | Wiring/text, not visual rendering |
| Competitive presets | Preset schema, move legality references, wild coverage, and generated table alignment | Legality/coverage, not set quality |
| Species stat rebalances | Exact manifest of 102 selective Inclement stat ports plus three paired Mega HP changes; unselected species cannot drift | Exact data parity, not balance quality |
| Upstream critical fixes | Required source tokens for buried trainers, Keldeo forms, recording disablement, and PC item text remain | Token contracts can be fooled by dead/commented code; runtime tests carry stronger weight |
| Campaign roster | Champions and Kanto family acquisition graph plus Mega/evolution-item unlock coverage | Reachability graph, not catch-rate or player-time proof |
| Game Corner starter archive | All alternate starters and special prizes use unique, transactional claim flags; pricing, dialogue, and five runtime tests remain present | Source transaction contract, not economy/fun proof |
| Trainer Ability legality | Host C preprocessor resolves configured species/forms and validates all 2,147 authored Abilities | Strong source legality; does not test strategy quality |
| Trainer runtime coherence | Attack-nature contradictions, unsupported charge moves, and exactly five reviewed dual-speed teams | Narrow data-quality check, not a complete per-team audit |
| Story/dialogue | Critical plot tokens/assets, League retirement paths, and 47,628 literal Hoenn lines fit the width budget | Static language/width, not tone, sequencing feel, or rendered kerning |
| Rematch-free Match Call | Disabled rematch entry points remain dormant and reachable dialogue makes no rematch promise | Source/control-flow contract |
| Progression graph | 540 maps, 4,086 events, 1,402 warps, 17,957 references, and selected critical gates resolve | Broad structural graph, not every possible runtime state |
| Legendary availability | 101 legendary families have roots; 82 dependency chains terminate; harbor/Circuit roots exist | Acquisition graph, not encounter UX |
| Legendary Signs/Circuit | 82 sign definitions and generated Circuit manifest/hash agree | Data generation, not battle quality or endless-randomness proof |
| Regional starters | Nine trios, 81 stage presets, and rival counter-family mappings remain | Selection/mapping contract |
| Restored world | 22 maps, objects, pickups, encounter tables, signs, and sanctuaries are connected | Structural, not visual or collision playtesting |
| Solo evolutions | 30 trade evolutions and required items have solo paths | Availability, not tutorial discoverability |
| Fossil revival | All 11 complete fossils are recognized, repeatable, and safely delivered | Script contract |
| Poké Vial quest | Native Chansey-chain scripts and state tokens grant the second charge once | Script/state contract |
| Campaign battle master | 513 dossiers/561 branches satisfy schema, legality ledgers, chronology rules, and coverage declarations | Targets and difficulty labels are authored intent, not observations |
| Campaign evidence freshness | Evidence report exactly matches its declared source snapshot | Synchronization only; source may still be badly designed |
| Battle script formats | All 561 physical branches use the authored single/double/multi macro | Format wiring only |

Additional inline release checks prove that the master materializes exactly into `trainers.party`, state IDs are unique, player-facing branding is current, Git patches have no whitespace errors, artifacts are newer than build inputs, ROM header/checksum are valid, and ROM/EWRAM/IWRAM regions fit.

The artifact freshness check is timestamp-based, not a cryptographic proof that an arbitrary local ELF/ROM was produced from the current source. CI closes that gap by building in the same clean job immediately before verification. Local users must still perform a fresh build.

## Gate that had encoded a rejected design decision

The prior `scripts/emerald_champions_wild_distribution.py` required Sprigatito, Fuecoco, Dreepy, Scyther, and Axew across the first six areas. That was not a correctness invariant; it froze the exact early-starter/power distribution under redesign and could force tables back into a local minimum.

The replacement gate now checks opening-act generation bias, early doubles-role coverage, starter-family exclusion from ordinary wild tables, quest dependencies, restored-area Ultra Beasts, encounter rates, and source/preset legality. It no longer treats previously present early starters as inherently correct.

## Runtime test inventory

The inherited test corpus contains:

- 968 C test files.
- 953 function/overworld tests.
- 4,248 implemented battle tests.
- 585 declared `TO_DO_BATTLE_TEST` cases.
- 9 active `KNOWN_FAILING` markers outside the newly repaired curated paths.
- 390 assumption blocks and 4,982 individual `ASSUME` calls.

Historically, compiling the required test ELF compiled this entire corpus. Required CI now selects only the 41 source files containing its named filters (including the three runner-support files), then proves at runtime that every filter still reaches its minimum count. The complete corpus remains covered by the scheduled/manual compile workflow rather than being represented as a green runtime suite.

The curated manifest now requires at least 220 runtime tests across 14 filters:

| Filter family | Minimum | Accepted debt |
|---|---:|---|
| Emerald Champions services/presets/mechanics | 68 | none |
| Item-description layout | 1 | none |
| Egg inheritance safety | 1 | none |
| Upstream critical fixes | 4 | none |
| Commander | 42 | none |
| Forecast | 18 | none |
| Flower Gift | 12 | none |
| Simultaneous manual switches | 3 | none |
| Sleep Clause + Imposter interaction | 1 declaration / 2 Ability trials | none after current repair |
| Three explicit Imposter targeting scenarios | 3 | none |
| Save-layout compatibility | 4 | none |
| Doubles AI | 63 | exactly three named TODOs at most: Wide Guard, Instruct, Quick Guard |

Debt is identity-checked, not just counted. A new known failure cannot replace an old one while preserving a misleading aggregate. Losing selected tests also fails. The shared test ELF is SHA-256 checked before and after all filters so post-link copies cannot mutate the base artifact.

The remaining three AI TODOs are honest missing tests/behavior, not passing coverage. They remain visible until implemented.

## Native runtime closure for the audited candidate

The retained Linux-built ARM test ELF was executed with the universal native macOS mGBA runner after byte-synchronizing the final runtime files. All 14 manifest filters passed:

| Family | Result |
|---|---:|
| Champions | 68 pass |
| Item description / egg / upstream fixes | 6 pass |
| Commander | 42 pass |
| Forecast / Flower Gift | 30 pass |
| Manual switch order | 3 pass |
| Raw, singles-AI, Sleep-Clause, and Billy Imposter scenarios | 4 pass |
| Save compatibility | 4 pass |
| Doubles AI | 60 pass, 3 exact TODO |
| **Total** | **217 pass, 3 TODO, 0 fail, 0 known-failing** |

Native filter execution took 85.29 seconds; orchestration wall time was 85.82 seconds. The independently captured log is `/Users/gguthrie/Projects/ec-runtime-linux.CZpFep/final-host-runtime.log`.

Runtime execution caught issues that compilation and the previous green CI could not:

- Helping Hand still lost to a weak Mud-Slap despite a much stronger ally; scoring now recognizes a materially stronger partner.
- Left-side Beat Up/Justified coordination targeted a foe and allowed Protect to block the combo; target selection now resolves the intended ally activation in either processing order.
- Billy's Imposter lead copied Spore but targeted the powder-immune foe; the AI now evaluates the copied move and live target state correctly.
- The Commander test accidentally gave Inclement Pidgeot its new No Guard default, which legitimately lets Scratch connect through Fly; assigning legal Big Pecks makes the regression isolate Commander cleanup.
- The old Sleep Clause/Imposter test used an incomplete recorded action stream. The replacement retains opponent-side Imposter re-entry, supplies explicit copied move slots/actions, checks both Insomnia and Vital Spirit, and proves sleep plus the clause clear.

None of those paths is accepted as known failing now.

## Build-once implementation

`Makefile` now has a phony `patch-test-filter` target. Repeated `make check TESTS=...` calls re-patch `gTestRunnerArgv` rather than forcing the linker. `TEST_SOURCE_ALLOWLIST` optionally limits test translation units while retaining the three required runner sources; an empty value preserves the upstream compile-everything default.

CI goes further: `scripts/run_emerald_champions_runtime_gates.py` maps its filters back to declaring C files, builds one curated `pokeemerald-test.elf`, copies it for each filter, patches only the copy's filter/headless flags, runs mGBA Hydra, validates the named summary, and verifies the base digest did not change. Minimum runtime counts protect against a faulty allowlist silently dropping selected tests. The runner supports split `--build-only` / `--run-only` operation so a Linux-built ARM ELF can be exercised by the native universal macOS mGBA runner when the local cross-toolchain is incomplete.

## Platform findings

The claim that macOS lacks `mgba-rom-test-mac` is false for this checkout. `tools/mgba/mgba-rom-test-mac` is a tracked universal Mach-O containing both arm64 and x86_64 slices. The local Hydra, patchelf, scaninc, and related native tools are valid arm64 Mach-O binaries.

The real local blocker is the Homebrew `arm-none-eabi-gcc` installation: it lacks newlib standard headers (`string.h`, downstream `stdint.h`). A direct macOS test build therefore fails before game compilation. Linux CI installs `libnewlib-arm-none-eabi` and is the canonical clean-build environment. A disposable Ubuntu 24.04 Docker snapshot is also used for Linux parity; its object tree is retained at `/Users/gguthrie/Projects/ec-runtime-linux.CZpFep/repo` for incremental verification because Docker Desktop exposes only two CPUs.

## Mutation sensitivity

Representative source mutations were performed in a disposable Git worktree and then removed:

| Mutation | Expected detector | Result |
|---|---|---|
| Rename the native Difficulty option token | Core services | caught |
| Remove the explicit seventh League badge check | Progression graph | caught |
| Give Gigalith illegal Drizzle | Trainer Ability legality | caught with exact trainer/species/legal slots |
| Change a campaign script without regenerating evidence | Campaign evidence freshness | caught |
| Change a materialized trainer move without changing the master | Master materialization | caught |

The default release verifier was separately tested in a metadata-free temporary directory: it now fails closed. `--allow-source-bundle` is the only explicit escape hatch.

The Linux compile itself acted as an additional mutation detector by surfacing the duplicate expression-valued hidden-item flag that the former direct-literal parser missed. This is why warning output and clean compilation remain evidence rather than disposable noise.

Workflow syntax passes actionlint 1.7.12. The exact pinned mdBook 0.5.1 archive checksum and a full-source amd64 documentation build also pass without emitted errors after repairing the stale changelog include and invalid image closing tag.

## Defensive-code appraisal

The engine contains approximately 172 ordinary `assertf` calls and 39 `errorf` calls that intentionally become recovery/no-op paths in release builds, plus 10 `fatal_assertf` and 4 `fatalf` calls that remain fatal. Most are inherited engine diagnostics, not Emerald Champions-specific defensive clutter.

The former trainer-Ability fallback was not acceptable because it silently changed authored strategy in release. It is now protected three ways: exact configured legality gate, runtime preset coverage, and a fatal engine assertion if invalid authored data somehow ships.

Do not broadly convert or delete inherited assertions. Review a fallback when it can silently alter a campaign promise, duplicate state, corrupt a save, or change battle strategy; otherwise retain the engine's established recovery behavior.

## Remote divergence warning

At audit time, `origin/main` (`2f6737758`) was not the intended release baseline. Relative to this release branch it deleted the Docs workflow and a large documentation tree, removed trainer-Ability legality from the release verifier, and changed the Ability/data layer. Its green CI therefore proves a weaker and different tree. Do not merge it wholesale over this branch; review or cherry-pick independent changes.

## What still requires play

No static or filtered runtime suite can establish the requested final quality bar. Release confidence still requires:

- A fresh-save campaign traversal of every required state transition.
- Scenario saves before each Gym, villain milestone, League room, restored area, legendary chain, and Frontier mode.
- Visual inspection of menus, dialogue, maps, sprites, collision, and animations on hardware-like rendering.
- An encounter-by-encounter human difficulty/pacing record that replaces `UNPLAYED` with observed outcomes.
- Native Frontier streak testing over enough generated teams to expose repetition and impossible states.

These are not reasons to discard the current gates. They are the evidence layer the gates cannot supply.
