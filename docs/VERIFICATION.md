# Build, play, inspect and verify

Run from the repository root. This is the native GBA engine using libmGBA for
headless play; no browser remake or paid model/API is required. The active save,
next work and scope are in `CONTINUE.md`. Do not run a blanket suite before
continuing the campaign unless a concrete change warrants it.

## Dependencies and portable build

Ubuntu24.04 x86-64 is the documented common build/test environment:

```sh
sudo apt-get update
sudo apt-get install -y binutils-arm-none-eabi build-essential gcc-arm-none-eabi \
  libnewlib-arm-none-eabi libpng-dev libmgba-dev pkg-config python3 python3-pil \
  fonts-dejavu-core git ca-certificates
```

The included Dockerfile provides the same environment. On an ARM machine, use
`--platform linux/amd64` if you want to run the bundled Linux regression-test
binary. Native ARM libmGBA can run the campaign harness, but the bundled Linux
`mgba-rom-test` executable is x86-64. The Mac regression binary is universal.

```sh
docker build --platform linux/amd64 -t emerald-champions-v4 .
docker run --rm -it --platform linux/amd64 \
  --user "$(id -u):$(id -g)" -v "$PWD:/game" -w /game \
  -e GIT_CONFIG_COUNT=1 -e GIT_CONFIG_KEY_0=safe.directory \
  -e GIT_CONFIG_VALUE_0=/game emerald-champions-v4
```

Inside that shell, run the commands below. A normal clone has its `.git` directory
inside the mount. A linked worktree also needs its actual Git metadata accessible;
prefer a normal clone on the new machine. Do not copy old machine binaries in
`tools/*` over tools freshly built for this host.

On macOS, provide a working ARM GCC/binutils/newlib toolchain on PATH and native
libmGBA (`MGBA_PREFIX` can name its installation), libpng, pkg-config, C/C++ build
tools and Python with Pillow. The screenshot runner discovers Homebrew libmGBA.
Use a local Python environment with Pillow if system Python does not have it;
all host helpers use the Python interpreter that launched them. The old account's
absolute bundled-Python path is no longer an execution dependency.

Build the normal ROM for a user/emulator:

```sh
make -j4 release
python3 scripts/stamp_release_inputs.py
python3 scripts/stamp_release_inputs.py --check
python3 scripts/verify_emerald_champions_release.py
```

Build the separate observable campaign/scene ROM:

```sh
make -j4 BUILD_NAME=emerald-headless MAP_VERSION=emerald \
  EC_HEADLESS_FIXTURES=1 TEST=0 pokeemerald-headless.gba
python3 scripts/stamp_release_inputs.py --stamp pokeemerald-headless.inputs.json
python3 scripts/stamp_release_inputs.py --check --stamp pokeemerald-headless.inputs.json
```

Production explicitly excludes the fixture bridge. The headless ROM is for
native development and separately labeled synthetic fixtures; do not present it
as the normal player release. Keep each ROM, ELF and input stamp together.
After source changes, let make rebuild actual dependencies and inspect failures;
do not stamp an old ROM to claim freshness. An input digest does not itself prove
that every stale object was rebuilt. A clean-clone build is part of this handoff's
verification. Host-native and cross-compiled tools must use the right architecture.

## Keep the book synchronized

Edit the current book's rule and exact relevant records whenever gameplay changes.
This includes implemented/pending status, discarded experiments, handoff text,
habitats, economy and AI contracts, not just trainer moves. For trainer changes,
sections8/9/22 own the E/B parties, U builds, EV/IV data, tactics and executable
intent. Use the existing import/materialization pipeline:

```sh
python3 scripts/emerald_champions_teams.py \
  --book 'Game Blueprint/Emerald_Champions_Game_Book.txt' --write
python3 scripts/emerald_champions_teams.py \
  --book 'Game Blueprint/Emerald_Champions_Game_Book.txt' --check
git diff --check
```

A prose-only book edit changes its recorded hash header; regenerate that header
with the same command. Do not hand-edit generated trainer tables. The check
compares the book-derived authored teams, master/plans and native parties and
validates configured Abilities. It does not prove tactical quality or world
reachability. Shared player presets have their own authored inputs/generator.

## Fetch and resume the exact earned session

```sh
python3 scripts/playthrough/fetch_handoff.py --list
python3 scripts/playthrough/fetch_handoff.py
python3 scripts/playthrough/party_snapshot.py \
  work/v4-c13-brenden-final-ai-play downloaded-checkpoint
python3 scripts/playthrough/battle_read.py work/v4-c13-brenden-final-ai-play
```

The current checkpoint is in the overworld, so `battle_resources_live: false` is
expected. The default fetch restores the complete current session, not a new
synthetic fixture. `--evidence` additionally downloads the archived preceding
campaign/scene/native-test evidence; `--roms` adds the current normal ROM and
historical releases. `--all` gets everything. It refuses to overwrite different
existing files so a later session is not silently rolled back.

Advance using normal buttons and explicitly chosen frame timings:

```sh
python3 scripts/playthrough/session.py work/v4-c13-brenden-final-ai-play \
  next-observation --frames 120 --at 119:overworld
```

For the next actual action, replace/add `--key FRAME:DURATION:KEY`, for example
`--key 1:1:A`. The driver preserves a numbered `.before.ss1`, screenshot(s),
matching `scene.gba`/`scene.elf`, and `trace.json`; `current.ss1` is the mutable
primary checkpoint. It saves the whole requested step only after the runner
succeeds. Inspect the newest trace if a tool call was interrupted, since its
process may still have completed. Never run two drivers on the same session.

On a new ROM build, first save natively in-game (open Save, confirm Yes and any
overwrite dialogue; inspect the result). Export only after that native save:

```sh
python3 scripts/playthrough/session.py work/v4-c13-brenden-final-ai-play \
  export-confirmed-save --frames 1 \
  --save-out work/v4-c13-brenden-final-ai-play/earned-next-checkpoint.sav
```

Create a **new output directory** for the clean Continue on the root headless
ROM/ELF. This is the exact clean-boot schedule used successfully in the run:

```sh
python3 scripts/playthrough/session.py work/next-build-play continue-earned \
  --boot 0 --scenario CAMPAIGN_NATIVE \
  --save work/v4-c13-brenden-final-ai-play/earned-next-checkpoint.sav \
  --frames 2200 --key 601:1:START --key 1050:1:START --key 1600:1:A \
  --at 2199:continued
```

To resume the supplied checkpoint on a rebuilt ROM, substitute
`earned-machine-handoff-granite-b1f.sav` as input. Verify location, party and
progress, then make the new directory primary and update the handoff state.
Do not overwrite the existing session's ROM/ELF or load its savestate on the new
ROM. Save/Continue is also the portability fallback across emulator versions.

## Native controls and efficient play

Use one-frame menu key pulses with about200 frames between them initially;
longer dialogue pauses are often600–900 frames. Shorten only after observing
actual controller state. Held movement is usually16 frames per tile, with a gap
between turns. The static `map_path.py MAP START_X START_Y END_X END_Y` helper
suggests native movement keys; inspect the resulting map and coordinates.
It ignores some dynamic obstacles and allows requested end tiles through its
collision filter, so it does not certify a route.

At a doubles command menu, player battlers are0/2 and enemies1/3. Fight `A`
opens the 2×2 moves: indices0 top-left,1 top-right,2 bottom-left,3 bottom-right.
Each battler remembers its move cursor. A damaging move needs a target-confirm
`A`, including spread moves and a lone enemy; the normal default is the first
living enemy. `LEFT` toggles the usual two opposing targets. Self moves such as
Protect, Tailwind, Follow Me and Helping Hand do not use that target step.
Read the returned controller/cursor state before blindly queuing a second action.

Switching starts `DOWN` from Fight, then `A` to the party. Choose the actual
current party slot, `A` to its command, then `A` again after the menu settles.
Forced replacements reorder the battle party and two simultaneous faints need
two choices. Inspect the party screenshot; field order is restored only after
battle. `battle_read.py` reports native HP, moves, stages, selected targets,
controllers, cursors and timing fields in a JSON sidecar. Its member offsets
match the current engine structs; update the inspector if those layouts change.
It reads one ephemeral emulator frame without saving it, so it does not advance
or mutate the primary session. `party_snapshot.py` likewise preserves raw native
party bytes and a hash without modifying the primary state.

The Start menu currently includes Pokédex, Pokémon, Bag, PokéNav, Trainer Card,
Save, Reload and Option. Do not assume the cursor resets after closing it.
The Bag last used Key Items: Poké Vial, Leveler, Repel Spray, Flight Beacon,
Wailmer Pail, Devon Parts, Old Rod. The actual screenshots take precedence over
these convenience notes. Native Retry repeats a fight; Reload restores the saved
game. Use ordinary captures, legal preparation and available healing deliberately.

For read-only campaign queries, `--query KIND ID` uses the observation bridge:
1 flag,2 variable,3 object,4 bag item,12 party species by slot,13 species-specific
player cap (species0 gives the chapter cap). Trainer clear query IDs are trainer
ID+0x500, for example Cristian1854. These query-selection writes only request
observation; they do not alter the game result. `--read-symbol WIDTH:NAME+OFFSET`
reads an ELF-resolved address. Never use `--write` to manufacture earned progress,
Pokemon, RNG or battle outcomes; the session tool rejects setup writes in native
mode. Separate synthetic fixtures remain available for focused branch diagnosis.

## Capture and inspect contact sheets

Choose useful moments, not just the final state. Fast text/battle options can
complete an action before a large scheduled offset; if all panels show the same
menu, replay the archived before-state read-only with earlier frame captures.
Preserve original shots, actions and result reads. Do not infer an animation
from a skipped frame or confuse an AI choice with proof that it executed.

```sh
python3 scripts/playthrough/contact_sheet.py \
  work/v4-c13-brenden-final-ai-play cristian-first-attempt \
  --range 53 68 --moments
```

This writes `work/contact-sheets/LABEL.png`, its input manifest and a sidecar
recording original SHA256s and placements. `--range` is inclusive; `--last N`
is a convenient alternative. Original-machine trace paths are resolved to the
restored local `work/` paths. The underlying
`scripts/audit/render_contact_sheet.py` uses nearest-neighbor scaling and never
redraws game pixels. Open the sheet with the image/file tools and actually inspect
it. A command succeeding does not mean the screenshot has been reviewed.

For new/restaged scenes inspect staging, movement, facing, timing, arrivals/exits,
camera/map labels, dialogue layout and the important outcomes/re-entry paths.
Synthetic fixtures in `src/emerald_champions_headless.c` and
`scripts/render_emerald_champions_ui.py` can exercise targeted prerequisites;
label them synthetic. The earned path still needs natural discovery and handoffs.
User visual acceptance cannot be supplied by the agent's own inspection.

## Trainer/AI review and focused regression loop

Before every battle: find the E/B/T entry in book sections8/22, exact U builds in
section9, and current `emerald_champions_battle_teams.txt`/native party definition.
Check leads, reserves, levels, moves/items/Abilities/EVs, conditional strategies
and partner tactics. Follow the relevant code in `battle_ai_main.c`,
`battle_ai_pair.c`, `battle_ai_switch.c`, `battle_ai_util.c` and native mechanic
owners. Some encounters share one tactical card or several trainer aliases;
actual script ownership determines whether it is single-owner doubles or multi.

Play the complete fight using a credible legal team. After wins AND losses,
review opposing Protect, setup/activation payoff, targeting, switching, priority,
redirection, locks, native damage/status/order and wasted turns. Diagnose the
smallest real defect; inspect fixture/config/artifact freshness before changing
code. A stronger enemy or the eventual winner can still contain an AI bug.
Do not strip meaningful moves or soften correct choices merely to force a win.

When a distinct defect needs regression protection, reproduce it against the
actual affected state/authoring, keep the baseline failure, repair its owner,
then run the focused case and meaningful neighboring checks. A generic fixture
without Brenden's authored SETUP preference failed to reproduce the Coaching bug;
using the actual trainer identity did. Plain DOUBLE tests often have recorded/link
flags; campaign caps need explicit campaign flags. Preserve valid assertions and
remove stale checks rather than accumulating counts.

Example focused test build, with only the required harness and actual regressions:

```sh
make -j4 check-tools
make -j4 TEST=1 TEST_SOURCE_ALLOWLIST='test/test_runner.c test/test_runner_args.c test/test_runner_battle.c test/battle/ai/coaching_pair.c test/battle/ai/fainted_target_pair.c test/battle/ai/prankster_burn_pair.c test/battle/ai/quash_pair.c' pokeemerald-test.elf
python3 scripts/stamp_release_inputs.py --stamp pokeemerald-test.inputs.json
python3 scripts/playthrough/run_focus.py --elf pokeemerald-test.elf \
  --filter 'EC Coaching:' --filter 'EC fainted target:' \
  --filter 'EC Prankster burn:' --filter 'EC Quash:'
```

`run_focus.py` checks the source/artifact stamp, then reuses the existing native
runtime-gate verifier with only the supplied filters. It does not bless archived
ELFs against a newer tree. On macOS, build `check-tools` for macOS before running;
Docker-written Linux tools cannot be executed there. Explicit `--patchelf`,
`--hydra`, `--romtest` and `--runtime-cwd` support separated native host tools.
The generic `scripts/run_emerald_champions_runtime_gates.py` remains available
for its curated suite when the change actually warrants it.

Measure real opponent decision time by replaying an archived step's exact input
until both living opponents finish scoring. This includes setup and both actors,
not host wall time, battle text/animation or player deliberation:

```sh
python3 scripts/playthrough/time_decisions.py \
  work/v4-c13-brenden-final-ai-play 30 31 32 34 35 37 39 40 41 42 43 45 \
  --out work/brenden-decision-timing-check.json
```

Choose actual decision-input step numbers from the trace; non-decision steps
cannot stand in for a benchmark. The helper never overwrites the primary state.
It uses the current AiLogicData layout offsets and native VBlank counters;
reconcile them when changing those structs. Report maximum measured frames and
approximately seconds against the1.2-second budget, with sampled-board limits.
A good sample is not a proof of the worst case across the game.

Record exact source/artifact identity, player/enemy builds, complete inputs and
native outcomes, failures, meaningful screenshots, measured timing and remaining
limits. Add broader variants, randomized seeds and alternate player archetypes as
required by book section25; those are not all complete in the current run.

## Delivery checks

Run applicable source checks and a normal release build from the actual source
snapshot, then verify ROM/ELF/input agreement and a clean boot/Continue. Preserve
useful focused baseline/fixed evidence and actual campaign saves. Update the book,
`CONTINUE.md`, checkpoint/asset manifests and remaining limits before publication.
Use release assets for large immutable artifacts rather than committed build trees.
Verify the uploaded bytes through actual download; upload status or a local
server alone does not establish a working phone-accessible handoff.
