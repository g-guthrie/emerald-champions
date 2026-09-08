# Build and verification

Run commands from the repository root. The canonical toolchain/package setup is in [.github/workflows/build.yml](https://github.com/g-guthrie/emerald-champions/blob/main/.github/workflows/build.yml); [Makefile](https://github.com/g-guthrie/emerald-champions/blob/main/Makefile) defines the actual build. A compatible ARM GCC/binutils/newlib toolchain and native build tools are required. A usable emulator alone is not a complete ROM build environment.

## Release artifacts

```sh
make -j4 release
python3 scripts/stamp_release_inputs.py
python3 scripts/verify_emerald_champions_release.py
```

Stamp the same source tree that actually produced the ROM and ELF. If a container performs the build, create the stamp there and copy the ROM, ELF, and stamp together. A new stamp is not a substitute for a successful build.

[stamp_release_inputs.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/stamp_release_inputs.py) binds declared source/generator inputs and artifact hashes. [rom_artifacts.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/rom_artifacts.py) checks that the ROM matches the ELF-derived binary plus the Makefile's padding. [verify_emerald_champions_release.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_release.py) runs named static checks and validates the release header, memory bounds, and absence of fixture/test interfaces.

CI builds and stamps the ROM; it does not run regression or design gates. Optional check tools cover map/script structure, configured trainer abilities, trainer formats, authored/generated agreement, encounters, rewards, and artifact identity. Historical art/story snapshots, prose matching, strategy quotas and dormant facility loadouts are not release requirements. Static checks do not establish complete playability or difficulty.

## Runtime tests

`generate_emerald_champions_battle_sets.py --check` compares current authored
presets with their materialized tables and validates structural/ability bounds.
Circuit projection has the same current-data integrity boundary. Historical
role quotas, source-prose rules and strategy heuristics have been retired.
These checks do not establish competitive strength.

The native "exposes named Doubles and Singles sets" test enumerates both compiled catalogs,
including item-gated roles, and verifies visibility and the applied Pokémon state. The
"imported battle sets remain legal" test separately exercises the doubles opponent API.

Run a selected native filter only when it answers a concrete behavior question. Do not run a blanket suite after each change.

The [runtime runner](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/run_emerald_champions_runtime_gates.py) builds a selected test ELF and executes its configured filters. Its explicit per-filter debt is part of the result, not a claim that those defects are fixed. Counts, identities, and process outcomes must agree. `--build-only` and `--run-only` support separate build/runtime environments; copy the test ELF and its input stamp together.

The inherited full-corpus workflow was retired. `test/` contains the focused runtime corpus, current project regressions and harness checks. Selected runtime filters must execute named results with coherent counters and exit status; there are no historical test-count floors. Add a test only for an essential invariant that existing checks do not cover.

Those checks validate particular failure cases, parsers, or extracted production functions. Their fixture boundaries are described in each test; they do not replace emulator execution.

The Cohesion build targets fresh saves. Compatibility migrations and historical-save recovery paths have been removed. Current-run saves, pending rewards, and temporary-party restoration still need to behave correctly.

## Two separate gameplay pipelines

| Pipeline | Entry point | Evidence boundary |
| --- | --- | --- |
| Campaign traversal | [run_emerald_champions_campaign.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/run_emerald_champions_campaign.py), [playthrough manifest](https://github.com/g-guthrie/emerald-champions/blob/main/tests/campaign/playthrough.json) | Drives button inputs and checks scripted progress; its fixture automatically resolves ordinary battles. It does not measure combat difficulty. |
| Independent battle play | [agent_player.py](https://github.com/g-guthrie/emerald-champions/blob/main/tools/agent_player/agent_player.py), [battle_checkpoint_lab.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/battle_checkpoint_lab.py) | Starts from checkpoint-bound resources with campaign automation disabled. Existing semantic success records are evaluator reports, not independently inferred wins. |

The campaign runner currently selects `CAMPAIGN_AUTOWIN` for traversal. Per-segment `battle_automation` fields are descriptive legacy metadata, not executable mode controls: `native` there does **not** disable battle automation. Native battle claims require the separate battle pipeline or an explicitly recorded same-ROM diagnostic mode change with readback; screenshots before a battle are insufficient.

Trainer defeat assertions can use test-only aliases such as `FLAG_DEFEATED_TRAINER_TAKAO` in `expected.flags`. The runner derives these from authored `constants/opponents.h` IDs and the reserved trainer flag range; these aliases are not declarations added to the game.

Campaign checkpoints bind state bytes, parent identity, and artifact provenance. Explicit parent-run selection is honored; unrelated future manifest additions can remain compatible when the checkpoint's ancestry is unchanged. [verify_emerald_champions_campaign_run.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_campaign_run.py) compares completed evidence against an explicitly written baseline. Do not update a baseline simply to erase a failure.

[aggregate_results.py](https://github.com/g-guthrie/emerald-champions/blob/main/tools/agent_player/aggregate_results.py) rejects incompatible run identities and labels reported outcomes. Unknown Hard-mode or budget-exhaustion claims remain unknown. Harness checkpoint restore retains cumulative budgets. It is separate from the game's Retry and Reload controls.

Visual fixture comparison uses [verify_emerald_champions_visual_runtime.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_visual_runtime.py) and [its baseline](https://github.com/g-guthrie/emerald-champions/blob/main/tests/headless/inclement_visual_runtime_baseline.json). This is an optional manual comparison, outside CI. Historical pixels and scenario inventories must not veto deliberate UI changes; matching pixels alone are not an aesthetic judgment.

Record failures and incomplete coverage alongside successes. A release-readiness claim needs a fresh build, relevant runtime tests, native UI/failure-path checks, and an actual fresh-save campaign run; balancing and Nuzlocke difficulty additionally need appropriate play evidence.

## Keeping the suite small

A retained test needs a concrete current failure it can detect, an assertion that observes that failure, and a reason existing checks do not already cover it. Prefer an executed production-function regression over another source-token scanner. Empty TODOs belong in issue notes. Benchmarks, historical imports and speculative strategy judgments do not belong in required game gates.

When gameplay changes deliberately, revise or delete its old expectation in the same change. Do not restore old rewards, encounters, prose, coordinates, ratios or counts merely to satisfy a test. Preserve actual save integrity, transactional no-room behavior, safe battle actions and the separate traversal/battle pipelines. Broad inherited mechanic coverage was deliberately retired in the September 8 audit; the remaining suite is not an exhaustive engine certification.
