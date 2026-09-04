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

Historical comparison checks require their pinned Git reference. Required CI fetches history. A source archive without that reference cannot claim those comparisons passed. Static checks establish their named invariants; they do not establish complete playability or difficulty.

## Runtime tests

```sh
python3 scripts/run_emerald_champions_runtime_gates.py --jobs 4
```

The [runtime runner](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/run_emerald_champions_runtime_gates.py) builds a selected test ELF and executes its configured filters. Its explicit per-filter debt is part of the result, not a claim that those defects are fixed. Counts, identities, and process outcomes must agree. `--build-only` and `--run-only` support separate build/runtime environments; copy the test ELF and its input stamp together.

The [weekly workflow](https://github.com/g-guthrie/emerald-champions/blob/main/.github/workflows/full-test-compile.yml) compiles the broader test corpus. Compilation does not mean those tests were executed. Focused host checks can be run with:

```sh
python3 -m unittest discover -s tests -p 'test_*integrity.py'
```

Those checks validate particular failure cases, parsers, or extracted production functions. Their fixture boundaries are described in each test; they do not replace emulator execution.

## Two separate gameplay pipelines

| Pipeline | Entry point | Evidence boundary |
| --- | --- | --- |
| Campaign traversal | [run_emerald_champions_campaign.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/run_emerald_champions_campaign.py), [playthrough manifest](https://github.com/g-guthrie/emerald-champions/blob/main/tests/campaign/playthrough.json) | Drives button inputs and checks scripted progress; its fixture automatically resolves ordinary battles. It does not measure combat difficulty. |
| Independent battle play | [agent_player.py](https://github.com/g-guthrie/emerald-champions/blob/main/tools/agent_player/agent_player.py), [battle_checkpoint_lab.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/battle_checkpoint_lab.py) | Starts from checkpoint-bound resources with campaign automation disabled. Existing semantic success records are evaluator reports, not independently inferred wins. |

Campaign checkpoints bind state bytes, parent identity, and artifact provenance. Explicit parent-run selection is honored; unrelated future manifest additions can remain compatible when the checkpoint's ancestry is unchanged. [verify_emerald_champions_campaign_run.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_campaign_run.py) compares completed evidence against an explicitly written baseline. Do not update a baseline simply to erase a failure.

[aggregate_results.py](https://github.com/g-guthrie/emerald-champions/blob/main/tools/agent_player/aggregate_results.py) rejects incompatible run identities and labels reported outcomes. Unknown Hard-mode or budget-exhaustion claims remain unknown. Harness checkpoint restore retains cumulative budgets. It is separate from the game's Retry and Reload controls.

Visual fixture comparison uses [verify_emerald_champions_visual_runtime.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_visual_runtime.py) and [its baseline](https://github.com/g-guthrie/emerald-champions/blob/main/tests/headless/inclement_visual_runtime_baseline.json). CI currently treats that job as advisory. Matching pixels are regression evidence, not an aesthetic judgment.

Record failures and incomplete coverage alongside successes. A release-readiness claim needs a fresh build, relevant runtime tests, native UI/failure-path checks, and an actual fresh-save campaign run; balancing and Nuzlocke difficulty additionally need appropriate play evidence.
