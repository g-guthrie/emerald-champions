# Verdant headless runtime tests

This directory defines a deterministic mGBA runtime layer that is tied to the
current checkout instead of a copied fixture repository.

## Truth contract

- `scripts/verdant_runtime_harness.py boot` runs `make -B`, so headers and
  generated data cannot be hidden by stale object files.
- The runner is compiled from `tests/headless/verdant_mgba_runner.c` for the
  current invocation.
- Runtime addresses are resolved from the newly built `pokeemerald.elf`.
- RTC is fixed to 2000-01-01 UTC unless a test explicitly chooses another
  epoch.
- The runner only receives task-local scratch saves. The orchestrator copies or
  creates them before launch; no user save is ever passed to writable mGBA
  backing. Tests that need persistence dump a new image with `--save-out` and
  launch a second emulator process.
- Clean boot attaches a fresh erased 128 KiB save image before reset; mGBA does
  not expose clonable flash backing until a save file has been attached.
- A clean boot is run twice and must produce identical memory observations and
  video hashes.

The C runner provides only emulator primitives: timed keys, timed 8/16/32-bit
writes, final 8/16/32-bit reads, a masked stop condition, a final screenshot,
and a save-data dump. It contains no Verdant addresses or structure offsets.

## Commands

Validate the fixture inventory and compile the host runner independently:

```sh
python3 scripts/verdant_runtime_harness.py plan
python3 scripts/verdant_runtime_harness.py build-runner \
  --output /private/tmp/verdant-runtime/verdant_mgba_runner
```

From a stable working tree, force-rebuild production and run the deterministic
boot twice:

```sh
python3 scripts/verdant_runtime_harness.py boot \
  --out /private/tmp/verdant-runtime/boot
```

`boot` fingerprints the working tree before and after `make -B` and fails if
another process edits source during compilation.

## Current executable fixture

`boot.clean` needs no ROM hooks. It proves that the production ROM built from
the current tree boots deterministically, advances VBlank, installs a valid
main callback, initializes both save-block pointers, renders a nonblank frame,
and exposes 128 KiB flash geometry.

## Planned fixtures and the minimal test-only bridge

Save, deep UI, battle, and Frontier scenarios need native functions to prepare
state. Raw memory construction would duplicate private C layouts in Python and
would become stale silently, so it is intentionally rejected.

After the active battle-engine edits stabilize, add one compile-time-only
bridge with these patch points:

1. Add `tests/headless/verdant_fixture_bridge.c` and a matching header. Keep the
   ROM-patchable `const` configuration and each observed result as individually
   named 32-bit symbols, so the host resolves them from the current ELF.
2. In `src/main.c:168-176` (`InitMainCallbacks`), select
   `CB2_VerdantHeadlessFixture` only under `VERDANT_HEADLESS_FIXTURES`; retain
   `CB2_InitCopyrightScreenAfterBootup` in every production build.
3. In `src/main.c:179-186` (`CallCallbacks`), call a fixture observation
   function under the same define. Do not synthesize keys in `ReadKeys`; the
   host runner already owns input deterministically.
4. Add the bridge object and define only when a dedicated test-build variable
   is set. No fixture symbol or branch may appear in the production ROM.

The bridge should reuse the native setup calls demonstrated by the historical
isolated fixture, but keep scenarios out of `src/main.c`: `SetSaveBlocksPointers`,
`NewGameInitData`, `CreateMon`, `CB2_InitBattle`, `ShowFrontierPass`, and the
facility dispatch functions. Each scenario must expose three separate result
words: setup precondition, observed runtime outcome, and final pass/fail. This
prevents a test from passing because setup silently failed.

The required scenario inventory and assertions are machine-readable in
`fixtures.json`. The first bridge batch should implement, in order:

1. native save followed by a second-process reload;
2. party, tutor, Mart, storage, and target-selection UI paths;
3. focused single/double battle mechanics;
4. Battle Tower record playback and full first-round Battle Dome progression;
5. remaining Frontier facility lifecycle checks.

Do not reuse `/private/tmp/verdant-headless-final-gates` as a source tree. It is
useful historical evidence, but it can no longer establish current production
behavior.
