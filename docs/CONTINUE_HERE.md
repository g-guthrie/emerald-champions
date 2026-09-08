# Continue Emerald Champions development

The repository root is the live game source. Start with `AGENTS.md`, then the [Cohesion Book](../Cohesion%20Book/README.md). The book is the complete frozen September 8 specification, including its reviewed source snapshot and detailed inventories. Do not develop inside `Cohesion Book/baseline/source`: those files are historical evidence.

## Current checkpoint: 5.1

`pokeemerald-release.gba` is the latest Emerald Champions 5.1 ROM. SHA-256: `900a71942944e9ef07016f3f5dc55b83a8bffce8f5d38a545be174b38268a9d7`. The adjacent input stamp records its original compiled source digest and artifact identities. The ELF is a generated build artifact; rebuild it locally. The same ROM is on [Google Drive](https://drive.google.com/file/d/1eGbIyPYQV4Q0pi6XbWOShrZAZV1-Q2AH/view).

The book's historical wording about old-save migration is superseded by the user's explicit instruction: **fresh saves only; no old-save compatibility**. The major opening, trainer, world, preparation, forms and shared-AI changes are implemented. This is not a claim that every book acceptance case or the complete campaign has been verified.

## Build from a checkout

Clone this repository and run commands from its root. Use a complete ARM GCC/binutils/newlib toolchain; a bare compiler without `string.h`/newlib is insufficient. CI provisioning is in [.github/workflows/build.yml](../.github/workflows/build.yml); general guidance is in [VERIFICATION.md](VERIFICATION.md).

```sh
git clone https://github.com/g-guthrie/emerald-champions.git
cd emerald-champions
make -j4 release DEVKITARM=/absolute/path/to/arm-toolchain
python3 scripts/stamp_release_inputs.py
```

On the original Mac, the complete tested ARM15 toolchain is `/Users/gguthrie/.local/share/arm-gnu-toolchain-15.2-20260718/Payload`. Generated tools, objects, assets, ROMs and emulator state are not prerequisites to bring from another working directory. The Makefile generates its dependencies. Native compilers and the tools listed in CI must be installed.

## Evidence and next work

- Production build and ROM header checks passed. Seven focused native declarations passed: six tactical/state-restoration checks covering13 parameter scenarios, plus a temporary full-party timing probe. Profiling code was removed before the final ROM.
- The demanding full-party probe improved from1803 frames originally, to1428 in the first uploaded checkpoint, to515 (~8.6 seconds) in5.1. A warm explicit decision took374frames. These are instrumented native test measurements, not a universal production latency guarantee. Worst-case AI latency still warrants work.
- An earlier pinned intermediate ROM was traversed from blank flash using normal inputs: dual selection/cancel, preparation, doubles rescue, natural loss/restoration, retry win, both nicknames, Dex and20Balls. Full campaign, all facilities/services, multiplayer and every tactical acceptance case remain unverified.
- A broader retained-file test run passed59 declarations and failed3: After You/Trick Room2/2, weather-for-ally6/10, terrain-for-ally6/8 in `test/battle/ai/ai_doubles.c`. Preliminary inspection found over-specific setup expectations; final adjudication is unfinished. Do not weaken assertions merely to pass them.
- Continue performance work in `src/battle_ai_pair.c`, `src/battle_ai_util.c`, and `src/battle_ai_switch.c`. Current code preserves every legal current-turn move/target, uses native forecasts for the small future reserve-Mega preference, avoids redundant entry-cache fills, caches unchanged speed/reserve metadata, and skips irrelevant residual calculations.

Local `work/` contains large disposable experiments, logs, screenshots and emulator fixtures. It is not required to build or continue source development. Durable status is recorded here; the full specification and its evidence are in the book.

## Retained debugging material

The [manual timing probe](development/cohesion_ai_timing.c), [tactical fixture contracts](development/ai-native-validation.md), and [focused native output](development/ai-check-results.txt) are retained for continuing AI work. The probe is deliberately outside the automatically discovered test tree. To run it, copy it temporarily into `test/`, select that source explicitly with `TEST_SOURCE_ALLOWLIST`, and use the `EC expert pair: timing probe` prefix; remove the temporary copy afterward.

The optional normal-key opening driver and input trace are tracked at `work/runtime-opening/replay_final_opening.py` and `work/runtime-opening/intermediate-build3/normal-key-trace.json`. Build the existing headless runner before use; see `work/runtime-opening/runtime-plan.md`. The trace belongs to the intermediate artifact documented there, so replaying it on changed timing or game logic requires visual review, not automatic success claims.
