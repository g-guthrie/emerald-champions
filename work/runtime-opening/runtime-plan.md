# Bounded native runtime verification

The production opening has already been exercised on the pinned intermediate build in `intermediate-build3/`. It used blank flash, normal buttons and read-only observations. `opening-result.json` records a natural loss, in-game retry, natural win, both permanent starters, the early Pokédex, and 20 Poké Balls before Route103. It is not final-build or competitive-strength certification.

## Final production replay

After root announces the final ROM/ELF/stamp, use a new output directory:

```sh
python3 work/runtime-opening/replay_final_opening.py \
  --rom pokeemerald-cohesion-20260908-release.gba \
  --elf pokeemerald-cohesion-20260908-release.elf \
  --stamp pokeemerald-cohesion-20260908-release.inputs.json \
  --out work/runtime-opening/final-pinned
```

The existing runner receives 357 normal key presses over 55,110 frames, no save input and no RAM writes. It captures 29 named moments plus the final frame/state. The script copies and hashes the artifacts first and checks their ROM/ELF relationship. A successful process is marked `rendered-awaiting-review`, not passed automatically. Inspect the first/second starter screens, cancellation, 2-member party, 66-point editor, 4-battler rescue, loss/Back field positions, victory return, both nickname prompts, Pokédex menu and 20-ball Bag. If timing diverges, continue from the new final state with ordinary input; do not load an intermediate-ROM state into final.

For the shorter essential startup/rescue UI check, append `--through-action 1176` (279 key presses, 36,480 frames). The full trace also covers the natural loss and win.

## Existing native tooling

- `build/headless/emerald_champions_mgba_runner` exists, matches its current source/output hashes, and links `/opt/homebrew/opt/mgba/lib/libmgba.0.10.dylib` (0.10.5).
- `tools/agent_player/agent_player.py --config CONFIG init`, `step BUTTON`, `checkpoint NAME`, and `summary` provide normal input with exact-ROM states and PNGs. The intermediate config is an executed example. Its battle flags are observations, not an in-battle metric, because flags persist after battle.
- Runner options include `--state-in/out`, `--save-out`, `--screenshot-at`, `--key FRAME:DURATION:KEY`, `--read WIDTH:ADDRESS`, and `--until`. Default save is `-`; it does not autoload an adjacent user save.
- Native mGBA.app, Homebrew mGBA, ffmpeg and ARM nm/readelf are installed. No GUI emulator or old ROM was launched for this preparation.

## Fixture capability and limits

Production intentionally omits `gEcHeadlessFixtureScenario` and `gEcAgentPrepCommand`. Existing `render_emerald_champions_ui.py`, campaign traversal, and battle-checkpoint setup cannot be pointed at the production ROM as if that bridge existed. A separately built **same-source fixture variant** is required for their synthetic setup; this worker has not built it.

The current `MAP_SWEEP` fixture can load every relevant real map; exact zero-based indices/positions are in `available-map-fixtures.json`: Fallarbor Tent81, Cozmo86, Verdanturf Tent88, aunt94, friendship rater95, Slateport Tent113, Contest Lobby164, Hill entrance467/floors468–471/roof472. `scripts/audit/map_sweep_render.py OUT START END --rom FIXTURE_ROM --elf FIXTURE_ELF --stamp FIXTURE_STAMP` renders the selected range. The range end is exclusive. This proves map/render setup, not a completed interaction.

The existing agent-prep bridge accepts 1–6 species, exact current-cap levels, preset/format and move/nature/ability/item/point arrays, then command1. It validates/builds the party through native functions and returns a result. No new fixture enum is needed for a six-member Tent entry/refusal/forfeit/restoration exercise from the map-sweep map. Compare the complete player-party bytes before entry and after current-run termination; observe native 6-member opponent count and doubles flags. Test each Tent origin independently and leave its pending prize field untouched until a real three-win run earns a reward.

Caution: named campaign query telemetry updates only while the fixture's battle-automation mode is active. Disabling that mode prevents autowin but leaves those particular probes stale. Native battle observations should read actual game symbols and inspect screenshots. Do not toggle automation merely to query a native battle outcome.

Hill's positive run and postgame Cozmo service need their actual game-clear condition. The generic map-sweep initializes fresh data and does not grant it. Existing scripted fixture setup can cover their refusal/rendering, but a positive test needs a current-run earned checkpoint or explicitly labeled synthetic flag setup. The production ELF has no `.debug_info`; vanilla comment offsets for save flags must not be assumed to describe the compiled ABI. The legacy battle-lab parser also contains hardcoded offsets and needs checking before use for such setup.

No Tent, Hill, new form-service, peer-link or full campaign runtime completion is claimed by the opening evidence. Source control-flow and exact authored-data checks for them are separate, recorded under `work/cohesion-implementation-2026-09-08/`.
