# Emerald Champions agent-player benchmark (v1)

This is a separate, model-facing play harness. It does not use campaign
fixtures to choose actions and it does not own trainer teams, presets, or
generated battle sets.

Its primary benchmark mode is `battle_lab`: the deterministic campaign
pipeline supplies a legitimate checkpoint at the contemporaneous cap and with
the resources earned by that point. Navigation and routine setup may be
assisted. The intended suite is a generated sequence of every trainer battle as
an independent puzzle. The model receives the full generated opponent dossier,
then owns legal team construction, battle choices, loss diagnosis, adaptation,
and retries. It receives no handwritten counter-team or solution.

The harness uses the repository's native libmGBA runner as a safe step loop:
each action loads the last exact savestate, presses one ordinary GBA button,
runs a fixed release interval, captures a 240x160 PNG, and atomically replaces
the state. A killed or timed-out step leaves the previous accepted state intact.
The fixed RTC, fixed frame schedule, ROM/ELF hashes, config, and policy seed are
recorded for reproducibility. The ROM, ELF, runner, context, and normalized
config hashes are checked before every mutation. A nonblocking per-run lock
prevents two model drivers from advancing one state concurrently.

The source-derived `player_context.md` is part of the run identity and its hash
is recorded. A model driver should include that context in its initial prompt.
It explains the game's preparation systems and requires resourceful retries,
but contains no trainer-by-trainer counters or bespoke solutions.

## Keyless smoke test

No API key or network service is used:

```sh
python3 tools/agent_player/agent_player.py \
  --run-dir work/agent-player/smoke init --replace
python3 tools/agent_player/agent_player.py \
  --run-dir work/agent-player/smoke run-manual START A WAIT B
python3 tools/agent_player/agent_player.py \
  --run-dir work/agent-player/smoke checkpoint after-smoke
python3 tools/agent_player/agent_player.py \
  --run-dir work/agent-player/smoke summary
```

For interactive/manual use, call `step A`, `step LEFT`, and so on. Supported
tools are exactly `A B START SELECT UP DOWN LEFT RIGHT L R WAIT`.

## Observation boundary

`observation_mode: vision_only` puts only the PNG path and hash in
`agent_observation`. Probe values are still stored under `evaluator` so the
benchmark can score battles and progress without leaking RAM to the player.
`instrumented` additionally places configured telemetry in the model-visible
observation. This makes mode comparisons explicit in the same log schema.

Every accepted action appends to `events.jsonl`. `session.json` holds totals,
budgets, hashes, and aggregate `attempts`, `deaths`, `battles`, and
`progress_events`. Probe roles convert counters or state transitions into those
metrics. Add a game-over/death counter probe when the ROM exposes one; v1's
example pipeline build has battle and map telemetry but no reliable death
sentinel, so deaths remain zero rather than being guessed from pixels.

Events that cannot be inferred honestly from a frame hash use an explicit
scoring command. Examples:

```sh
python3 tools/agent_player/agent_player.py --run-dir RUN record battle_attempt --battle-id roxanne
python3 tools/agent_player/agent_player.py --run-dir RUN record whiteout --battle-id roxanne
python3 tools/agent_player/agent_player.py --run-dir RUN record team_change --detail "swapped party member"
python3 tools/agent_player/agent_player.py --run-dir RUN record adaptation \
  --battle-id roxanne --rationale "need speed control and safer focus-fire turns"
```

The semantic ledger attributes per-battle attempts and records whiteouts,
catches, turns, successes, retries, team/move/preset/item/Ability/Stat Point
changes, Center/vendor/Leveler use, and adaptation rationale alongside exact
action/frame/elapsed-time totals. `battle_attempts_by_id` supports aggregation
of success rate and effort across repeated seeds, runs, and models.
Provider drivers should emit a declaration only after the corresponding native
UI action is observed. This avoids pretending image-only inference is exact.

Named checkpoints are immutable directories containing `state.ss1` and hashed
metadata. `restore NAME` verifies both ROM and state hashes and starts another
attempt. A config or ROM hash mismatch fails closed.

## Connecting a model

The CLI deliberately has no provider SDK. A model driver reads the latest
`agent_observation`, chooses one supported button, then invokes `step BUTTON`.
That keeps credentials, retry policy, and model versions outside the emulator
truth layer. Set the `model` object and `seed` in a copied run config so results
remain attributable. Difficulty comparisons should start from the same named
checkpoint, budget, RTC, seed, and policy/model configuration, and use separate
run directories.

For battle-lab runs, copy `battle_lab.template.json`, replace the checkpoint,
ROM, ELF, battle ID, and run directory, and confirm their hashes. The required
init write changes `gEcHeadlessFixtureActiveScenario` to zero while cloning the
checkpoint. This disables campaign auto-win/capture automation before the
tested battle; subsequent catches and battles are native.

Battle lab also fails closed without a source-derived `arsenal_manifest`, bound
to the exact ROM, checkpoint hash, and battle ID. It must enumerate every
species obtainable from routes, gifts, statics, and starters reachable at that
checkpoint; the active cap; canonical move/preset sources; contemporaneously
available items/stones; whether the Mega bracelet is accessible; and the full
generated opponent dossier. `arsenal.schema.example.json` documents the
exchange format, and `arsenal` prints it for the player driver.

Use `prep MUTATION TARGET VALUE` to ledger a strategic request. A canonical
prep executor or observed native UI flow then records the same mutation with
`--status applied --source JUSTIFICATION`, or records `rejected` with the source
gate that forbids it. Every request, application, and rejection is timestamped
and counted by mutation type. V1 does not edit party RAM itself: until the
campaign pipeline exposes a canonical legality/application command, claiming
an applied mutation requires observing it through native UI.

`generate_battle_suite.py` generates the exact opponent dossier for every
branch in the current master battle design and joins each one to a generated
arsenal at the same campaign order. It hashes trainer parties, battle sources,
learnsets, presets, wild encounters, constants, and every map script into one
source fingerprint; a stale or incomplete arsenal index fails closed. This
prevents trainer, encounter, shop, gift, learnset, preset, or Mega-unlock edits
from silently reusing obsolete puzzles. The remaining required producer is the
full source reachability-to-arsenal index; v1 does not pretend chronology alone
proves availability.

Difficulty runs always use authored Hard. Set distinct `rng_delay_frames` from
the same prepared checkpoint for at least the configured number of independent
runs. `aggregate_results.py` preserves each run first, then reports win rate,
first-plan success, attempts and turns to first win, whiteouts, preparation
revisions/types, roster diversity, distinct winning teams, and any supplied
surviving-party/HP detail. Its categorical rule is included verbatim in the
output; zero wins is labeled `unbeaten within budget`, never “impossible.” The
machine-readable contract is `result.schema.json`.

## Attribution and design provenance

The architecture was informed by
[Clad3815/gpt-play-pokemon-firered](https://github.com/Clad3815/gpt-play-pokemon-firered),
released under CC BY-NC 4.0. We inspected its screenshot/action loop, persisted
runtime state, progress tracking, and budget configuration. No source code was
copied into this implementation; this implementation uses a materially smaller
native-runner design. If code is later copied or adapted from that repository,
retain its attribution and CC BY-NC 4.0 non-commercial terms in the affected
files and distribution.

## Known v1 limitations

- Process-per-action stepping is slower than a persistent emulator daemon.
- Metrics are only as complete as configured symbols; the example has no death
  detector and no semantic story milestone counter. Semantic operations depend
  on honest driver/evaluator declarations until dedicated ROM sentinels exist.
- Savestates are exact-ROM/libmGBA artifacts, not portable save files.
- The harness supplies tools and evidence but does not yet include an OpenAI or
  other provider adapter, prompt-memory policy, dashboard, OCR, or video.
- `seed` records the policy seed. Game determinism comes from exact state, RTC,
  and fixed frame/input timing; the harness does not patch the game's RNG.
