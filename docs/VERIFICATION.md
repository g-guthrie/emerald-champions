# Build, play, inspect and verify

## Emerald Studio in the Codex browser

Requires native libmGBA headers/library, a working ARM toolchain and Python3
with venv support. native_tools.py discovers Homebrew mGBA; set MGBA_PREFIX
elsewhere. The launcher creates an isolated .venv-studio with the pinned
aiohttp/Pillow dependencies and builds the development ROM if missing.

~~~sh
python3 tools/studio/cli.py start
python3 tools/studio/cli.py status
python3 tools/studio/cli.py command '{"op":"checkpoint","label":"Before my edit"}'
python3 tools/studio/cli.py command '{"op":"build"}'
.venv-studio/bin/python tools/studio/verify.py
~~~

Open the printed localhost URL in Codex's browser. Arrow keys move, Z/X are
A/B, Enter is Start, Shift is Select, Q/E are L/R. Click Enable sound once.
The native process runs at GBA frame timing and sends uncompressed RGBA and
32768Hz stereo PCM over a local socket. Video has at most one unacknowledged
frame per client; audio discards stale backlog. Fast-forward is silent.
The server binds loopback only, checks Host and requires a per-launch token
for commands/WebSocket connections. No remote service or API key is involved.

Map overview renders authored tiles with object/warp/player markers. Shift-click
a clear tile to warp; select an NPC to inspect dialogue and stand beside them.
Dialogue changes update the actual script plus one current record in the Game
Book, run the existing book import, then rebuild. Failed compilation retains
the running game. Build logs and edit backups are under work/studio.

Rebuild/return records the current state, falls back to the pre-interaction
checkpoint when needed, requests a native save at an idle field boundary, and
exports actual flash data. Flash programming needs hundreds of emulated frames;
the acknowledgment loop runs those without wall-clock pacing. A separate new
core clean-boots the new ROM and confirms the restored position before replacing
the old core. Changes to the checked save-layout headers require a new sandbox.
Bookmarks restore their original ROM/ELF and state together; they never inject
an old raw state into new code. Work files preserve the paused earned campaign.

Party preparation reuses the game's first Doubles preset and per-species cap.
Trainer tests reuse the native debug-battle lifecycle: no automatic victory,
NPC postbattle script, or earned receipt. Hoenn rival seed slots are used by this
direct launcher; use the real NPC interaction for regional starter substitution.
Fresh chapter fixtures currently cover C03 Oldale, C04 Norman/Wally and C07
Roxanne. They are synthetic setups, not full campaign prerequisite reconstruction.

verify.py runs in a separate work/studio/verification session. It checks real
movement/checkpoint restore, preparation, difficulty, tile/facing warp, native
save/clean-boot preservation, non-silent audio, trainer entry/retry and C04 setup.
Its result.json records the exact ROM and sampled native frame costs. Browser
layout/input/dialogue checks supplement these tests. No full-game performance
or difficulty claim follows from an idle-scene timing sample.

## Scene recordings, dialogue cohesion and agent workflow

Install the personal skill with `python3 tools/studio/install_skill.py`. Its
repository-owned source is tools/studio/skill/SKILL.md; the installer records
this checkout for discovery. The skill supports both direct local commands
and browser interaction without requiring external code indexing.

Studio's Library contains scene recordings/contact sheets, saved situations,
a searchable NPC dialogue index, source sprite sheets and Git/local-build history.
Record captures exact inputs and native frames. Mark moment creates named anchors;
Stop produces paginated contact sheets, original PNGs with hashes, a lossless
motion preview and the input/state trace. A bounded rolling buffer also supports
short same-build rewind. Built-in examples:

~~~sh
.venv-studio/bin/python tools/studio/run_scene.py tools/studio/scenarios/oldale-vial.json --out work/studio/scenes/new-vial-run
.venv-studio/bin/python tools/studio/run_scene.py tools/studio/scenarios/c04-norman.json --out work/studio/scenes/new-c04-run
python3 tools/studio/cli.py command '{"op":"dialogue.export"}'
python3 tools/studio/cli.py command '{"op":"dialogue.search","query":"Briney"}'
python3 tools/studio/cli.py command '{"op":"scene.replay","id":"RECORDING_ID","mode":"exact"}'
~~~

Use a fresh output directory per run. Command JSON can be supplied with
`command --file command.json`. A scene.run command starts a separate native
worker so the user can continue playing. Poll scenes for completion; failed
expectations still produce pictures. Recipes have a start chapter or saved
recording, optional explicit setup/party/difficulty/warp, and bounded steps.
Steps support press, hold, periodic tap/every, frames, labels, and until
idle/dialogue/battle. Exact replay uses the old ROM and state; latest replay
uses a compatible portable battery save and the newly built ROM. Neither
replays an old machine state on new code. Position in a changed map remains
subject to actual collision/event review.

Compare pairs by marker labels, falling back to relative frame timing with
that limitation recorded. Pixel differences are diagnostic, not a quality
score. Always inspect the sheets. Source sprites/map previews are labeled
separately from native screenshots. The runtime observer reports active actors,
facing, movement flags and the text buffer submitted to the window-zero printer;
it does not certify that every page was displayed or read. Latin decoding uses
the first charmap definition; control bytes remain annotations.

The generated dialogue index currently links 2,365 Hoenn NPC bindings to 3,976
referenced text blocks across 540 maps. It includes conditional branches and
reports native-handler boundaries. The combined reading copy groups local text
by map and shared services separately. Duplicate-text groups are review
candidates, not deletion instructions. Build receipts preserve the Git base,
workspace patch, changed/untracked source archive, ROM and ELF; they are local
history and do not publish experimental edits automatically.

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
  work/v4-c15-dwayne-revised-play downloaded-checkpoint
python3 scripts/playthrough/battle_read.py work/v4-c15-dwayne-revised-play
```

The current checkpoint is paused at a native turn-five command screen, so live
battle resources are expected. Published September13 assets predate this C15
checkpoint: inspect `handoff/assets.json` before fetching, and never claim those
older downloads restore the current run. Inspect an existing C15 session only
with its matching ROM/ELF. `--evidence` additionally downloads the archived preceding
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

## Authorized stage-legal benchmark preparation

The user authorizes assisted acquisition/build preparation before trainer benchmarks.
Preserve an actual pre-edit Save. Audit the chosen habitat's physical route, story
and field gates, evolution providers and equipment first; the manifest records
that human/source audit, not an automatic reachability proof.

```sh
python3 scripts/playthrough/session.py SESSION prepare-team --frames 180 \
  --prepare-party handoff/benchmarks/c15_dwayne.json
python3 scripts/playthrough/party_snapshot.py SESSION prepared-party
```

This narrow interface writes only the existing native agent-preparation API,
requires unlocked field controls and records the manifest/hash/result. It uses
native species caps, legal preparation moves, abilities, EV limits and random
identities with perfect IVs/max PP. Invalid preparation leaves the primary state
unchanged; `work/assisted-prep-validation/negative-result.json` verifies Aerodactyl
with Spore is rejected (BAD_MOVE, slot0). The original prototype is diagnostic,
not the continued party. Acquisition records Pokédex/legendary ownership, so it
must only include genuinely available species. It does not earn trainer wins or
story progression, prove native captures, or close functional-NPC acceptance.
Normal Save/Continue retains assisted ancestry in the trace. This API is excluded
from the normal release ROM. Generic progress writes remain forbidden.

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

## Deus codebase memory through real MCP

The September14 Gym review used direct local caller/source inspection after
automatic approval review rejected an optional Deus trace because it might
export repository content externally. Do not retry or bypass that rejection.
Existing historical trace receipts below remain evidence of their own runs.

Use `scripts/deus_mcp.py`: it initializes the official server over stdio and
calls its advertised MCP tools, preserving requests/results and stderr. This
is a real MCP transport, not an imitation graph search. No paid API or agent
configuration installation is needed. The existing canonical book remains the
design owner; do not create a competing Deus ADR automatically.

This Work runtime denies local Unix sockets. Official v0.10.8 requires its
daemon even for CLI calls. Diagnosis isolated an additional PID/procfs namespace
mismatch and secure-cache ancestry requirement; a local compatibility build
passed those checks but still received EPERM at socket creation. Do not weaken
those checks or change sandbox policy. Stock v0.9.0 is the compatible direct
stdio release, before the daemon architecture. Its pinned Linux x86_64 archive
was checked against the official release checksum. The working binary has no
local source patches. Download it on a fresh machine with:

```sh
python3 scripts/setup_deus.py --directory ../deus-v090
python3 scripts/deus_mcp.py --binary ../deus-v090/codebase-memory-mcp \
  --cache ~/.cache/emerald-champions-deus-v090 --out work/deus/projects.json list_projects
python3 scripts/deus_mcp.py --binary ../deus-v090/codebase-memory-mcp \
  --cache ~/.cache/emerald-champions-deus-v090 --out work/deus/index.json \
  index_repository '{"repo_path":"/absolute/path/to/emerald-champions","mode":"fast","name":"emerald-champions","persistence":false}'
python3 scripts/deus_mcp.py --binary ../deus-v090/codebase-memory-mcp \
  --cache ~/.cache/emerald-champions-deus-v090 --out work/deus/weather-callers.json \
  trace_path '{"project":"emerald-champions","function_name":"ScorePairWithImmediateEffects","direction":"inbound","depth":3}'
```

The first actual index contains70,574 nodes/334,405 edges. Actual `trace_path`
queries identified production tree picking as the harvest-credit caller, plus
fixtures, and both move/switch callers of the shared pair evaluator. Fast-mode
coverage excludes several directories, including data/scripts; it does not
certify book prose or every assembler script edge. Use `search_code` and direct
active-script inspection for these. Refresh the index after material code
changes; consult the stored response and limits, not only the node count.

## Trainer catalogue, roster equality and wide trainer levels

`python3 scripts/export_trainer_catalogue.py` produces the requested complete
plain-text export and validation receipt in `work/exports`. It reads native
parties, compares all book member fields, lists direct Hoenn callsites, resolves
regional-rival replacement presets, and includes every Circuit variant/template.
Empty retired trainer records are metadata only; do not count them as battles.
`python3 scripts/verify_campaign_trainer_roster.py` is also part of the canonical
book `--check`: book IDs = nonempty native party IDs = Hoenn script battle IDs.
The compiler removes retired loadouts without renumbering saved trainer flags.

For the trainer-level change, the focused native build uses the usual harness
plus `test/trainer_levels.c` and `test/battle/frontier_circuit.c`. Filters:
`EC trainer levels:`, `Champions Circuit level 255`,
`Champions Circuit Mega Evolution`. All three pass in the current source-bound
run (`work/trainer-level-tests.log`). Signed16 offsets replace signed4; ordinary
trainer opponents can exceed100 using transient levels with bounded boxed EXP.
The unchanged battle/controller byte representation remains1–255; player caps
and EXP tables are unchanged. Tests cover both opponent owners, Mega stats,
wide positive/negative offsets, difficulty, saturation, damage and player isolation.

For the approved first-five redesign, `test/battle/ai/primary_support_pair.c`
contains3 regression groups/22 parameter cases. `EC primary support:` passes
with the current native shared evaluator. The before-fix failures and repaired
run are recorded in `work/first-five-ai-baseline.log` and
`work/first-five-ai-tests.log`. They test partner choice/survival, timely and late
Speed/SpDef support, primary-versus-secondary immunity, White Herb, Contrary,
reflection and sound blocking. They are scoped mechanic/AI tests, not campaign
fight clears or evidence that the revised teams meet the desired difficulty.

The synchronized first-five production ROM builds successfully and passes
`scripts/verify_emerald_champions_release.py` (including exact 369-party roster
agreement and the source/artifact stamp). Evidence:
`work/first-five-release-final-build.log`, `work/first-five-release-gates.log`.
The post-change Deus index completed with70,638 nodes/334,552 edges;
`work/deus/first-five-index.json` records its coverage and exclusions.

Opening follow-up: the `OPTIONS` native fixture runs `NewGameInitData` and
visually verifies the new Medium default in `work/opening-medium-default`.
Native specialist checks use `MOVE_SPECIALIST`, with the original and fresh
builds preserved in `work/opening-specialist-baseline` and
`work/opening-specialist-current`. Move learning/return and Nature application,
explicit No and exit show no residual Yes/No overlay. This scopes the old
unreproduced report; it is not a claim of a new menu-code fix or all-service
acceptance. `work/deus/opening-yesno-callers.json` traces shared cleanup owners.

The user-directed instinctive level pass updates42 distinct U builds through
E0010 (60 party references including rival aliases). Current book/native-party
agreement passes in `work/first-ten-source-check.log`; the production ROM builds
and passes all release gates in `work/first-ten-release-gates.log`. No new
difficulty simulations or full battle clears are claimed for these levels.
The earlier Medium-default and menu screenshots remain scoped to their saved
matching ROM/ELF, before the subsequent trainer-level edits.

## Darian composition and opening area discussion (2026-09-14)

Approved E0007 uses new U1504 Eviolite/Liquid Ooze Tentacool, Medium15,
Sludge Bomb/Muddy Water/Acid Spray/Protect; all four moves pass the pinned
legal-pool check. U0029 is preserved for Elliot. Generation/source check and
369-party roster check pass (`work/darian-composition-generation.log`,
`work/darian-composition-check.log`). Existing shared Liquid Ooze and support
forecast paths were source-audited; Deus inbound trace is
`work/deus/darian-pair-callers.json`. No new AI implementation or battle
acceptance is claimed. Build/gates: `work/darian-composition-build.log` and
`work/darian-composition-release-gates.log`. Opening Woods Great Ball failure
was traced through Std_ObtainItem/AddBagItem and the researcher exit; the
missing pending receipt was OPEN at that review and subsequently repaired by
the early economy cleanup below. No reward or habitat placement changed in
that earlier discussion pass.

## Early economy cleanup and Mega handoff (2026-09-14)

Run `python3 scripts/audit/early_economy_runtime.py --out work/<new-directory>`
after a current headless build. This covers changed finite gifts, Bag/PC/full
storage/repeat receipts, Steven's Aerodactylite transfer and actual prices/sale
values. Passing evidence: `work/early-economy-native-v2/result.json`; direct
Woods rescue reward scene: `work/early-economy-woods-direct/trace.json` (synthetic
battle outcome). Normal release gates: `work/early-economy-release-gates.log`.
Contact sheet: `work/contact-sheets/early-economy-cleanup-native.png`.
The existing story-handoff starter retry fixture explicitly starts after the
Aerodactylite entitlement; the new audit tests Aero's own retry independently.
The extracted native dependencies need their `usr/bin` on PATH and their
`usr/lib/x86_64-linux-gnu` in LIBRARY_PATH/LD_LIBRARY_PATH, plus MGBA_PREFIX
pointing at their `usr`; this is the host setup, not a game-source change.

## Rustboro Gym redesign and Mega reveal (2026-09-14)

Book sections8/9/22 own the four revised six-member teams. Generate/check
with `scripts/emerald_champions_teams.py`. Focused test allowlist adds
`test/battle/ai/mega_reveal.c` and `test/battle/ai/emerald_champions_plans.c`
to the three runner files. Filters `EC Mega reveal:` and
`EC battle plans: compiled directives` pass; see work/rustboro-focused-tests.log.
Two pre-existing zero-byte test objects (battle_interface, battle_anim_effects_2)
were removed and rebuilt; no test assertions were weakened.

Headless evidence: work/roxanne-mega-native-v2/trace.json (parameter1, native
Mega turn, then weak fixture-party loss), work/roxanne-rewards-v2/trace.json
(parameter0, synthetic victory, real badge/speech/Amber/direction scripts).
Observed Mega species918 before player Ring; reward fixture result1, controls
restored, Old Amber1, Ring0, Aerodactylite0. Contact sheet:
work/contact-sheets/rustboro-mega-reveal-native.png. Neither is an earned
clear or difficulty benchmark. Only Roxanne authors MEGA_REVEAL; native
eligibility remains required. Production build/gates logs:
work/rustboro-final-release.log and work/rustboro-final-release-gates.log.

Studio verification on September14: thirteen native integration checks passed
(work/studio/verification/result.json). The Oldale Vial recording's exact replay
matched final pixels, and replay on the subsequent ROM clean-restored its portable
save. The C04 recipe captured Norman/Wally movement through Route102 tutorial
battle entry. Live CLI recording, contact-sheet output and an independent replay
also passed; browser dialogue search returned the expected Briney handoffs.
The normal production build and release gates passed with USE_LTO_ON_RELEASE=0
(work/studio/release-final.log, release-gates.log). LTO was disabled for this build
after the Mac exhausted disk space; failed linker temporary files were removed.
The normal ELF contains no Studio bridge symbols. Idle native frame timing was
below2ms in the measured samples; this is not a worst-case campaign benchmark.


## Book-scene debut through Emerald Studio — September14

C44: c44-wreck-aide and c44-wreck-pickup exercise the existing aide and Scanner
object. c44-scanner/c44-scanner-repeat exercise Stern's scan, choice, delivered
reward and repeated route clue. Observed report1, Scanner0, Tooth1 and money18000
from a6000 synthetic baseline after both first and repeat visits. The selected
reward/full-storage branch is source-reviewed; a full-storage native replay is
not claimed here. Shared Bag/PC handoff helpers consume the Scanner.

C45: c45-chamber confirms doors1 while chapter0, cap78 and money6000. The local
account recipes exercise repeat inscriptions; c45-rock-smash, c45-island-lap and
c45-flash execute the actual licensed field actions/walking puzzle and their
revealed accounts. Native third-account completion gives cap82 and money22000
from6000; c45-repeat retains those totals. These are synthetic prerequisites,
not earned navigation or Regi captures. Existing puzzle receipts determine the
three-account completion, and FLAG_EC_REPORT_C45_COMPLETE owns its cap/stipend.

C30: c30-flight/c30-return verify the existing actors, original landing points,
finite sound, fades and destination-owned arrival text. The first test exposed
an endlessly looping wing sound; SE_M_FLY replaced it. A second visual review
caught post-warp text that never displayed. Destination frame scripts now consume
one arrival receipt. Recipes assert the final displayed text as well as the map.
Studio's idle wait now requires twelve consecutive idle frames, so it does not
stop between a warp and a destination auto-script.

C25/C39: c25-moltres and c39-heatran capture the existing native reveals after
correct synchronization with the asynchronous quake task. Moltres reaches battle;
Heatran's Stone scene reveals its existing actor and returns control. Full battles
and postbattle outcomes are not re-certified by these scene samples.

Evidence is in work/studio/scenes/c44-scanner-final, c44-wreck-final,
c44-aide-verified, c45-chamber-final, c45-desert-v1, c45-island-v1,
c45-rock-smash-v1, c45-lap-v1, c45-flash-v1, c45-repeat-final,
c30-flight-final, c30-return-final, c25-moltres-final and c39-heatran-v1.
Each trace names its immutable ROM/ELF and original captures. Contact sheets were
inspected for dialogue, facing, transitions, returned control and preserved art.
A compact six-panel showcase is work/studio/scene-debut/showcase.png. Earlier
v1/v2 failures remain diagnostic evidence, not passing acceptance claims.

Normal build: make -j6 release USE_LTO_ON_RELEASE=0 with the configured ARM
newlib toolchain, then stamp_release_inputs.py and verify_emerald_champions_release.py.
Logs: work/studio/scene-debut/release-build.log and release-gates.log (PASS).
Book import/ability/370-party roster checks pass. Added dialogue fits the existing
font-width budget; the previously existing209px berry-vendor line is unchanged.
The full wreck key trail, Route134 currents, late story-gate traversal, remaining
section17 scene branches and user visual approval remain separate coverage.


## Remaining section17 choreography completion — 2026-09-14

The second pass finishes the nine-item source disposition in the canonical book.
Use the tracked `tools/studio/scenarios/c14*`, `c26*`, `c30-castform*`, `c34*`,
`c35*`, `c36*`, `c39-missing*`, `c42*`, `c43*`, `c46*`, `c48*` and
`map-ember-path.json` recipes. Each `result.json` contains the observed outcome;
the recording owns immutable ROM/ELF hashes, prerequisites, inputs and original
pixels. Directory names below are relative to `work/studio/scenes`.

- `complete-c14-native-song-v1`: retained song/response and Stern handoff.
- `complete-c26-permanent-approach-v1`: historical superseded behavior (tower
  remained present after collapse). The approved replacement is below.
- `complete-c30-castform-response-final`: five-badge prerequisites, native Castform
  portrait/cry, Landorus offer declined and return offer declined. The earlier
  v1 had only badge5 and exercised the blocked reminder; it is not response proof.
- `complete-c34-first-voyage-v1`, `complete-c35-onward-voyage-final`,
  `complete-c36-voyage-home-final`: Briney promise, ferry departure/fades,
  destination arrivals, Faraway grass guidance and Lilycove/Mt. Pyre return.
- `complete-c35-lati-resolution-v1`: native sanctuary reveal and onward text,
  with automatic wild capture; it does not test a non-capture victory.
- `complete-c39-missing-stone-final`: actual missing-item guidance displays and
  returns control. The earlier `c39-heatran-v1` remains reveal evidence.
- `complete-c42-meteorite-briefing-v1`, `complete-c43-harbor-guide-v1`,
  `complete-c43-harbor-return-v1`: meteorite/triangle and Scanner directions.
- `complete-c43-triangle-v2`: all eleven native triangle positions and Deoxys
  reveal, automatic wild capture, C43 receipt and cap78. The earlier failure
  was a recorded-input error: direction changes before walking finished. The
  recipe now releases movement between path segments. Added task waitstate is
  a separate source fix: read the rock result only after its asynchronous task.
- `complete-c46-wallace-recollection-v1`, `complete-c46-rayquaza-awakening-v1`:
  completed-ruins reference, Sky Pillar handoff, original awakening/flight and
  restored camera/control.
- `complete-c48-origin-observation-v1`, `complete-c48-meteor-observation-v1`:
  existing observation interactions and optional battle declines.
- `complete-c48-steven-conclusion-final`, `complete-c48-devon-gift-final`:
  research closure receipt, first/repeat Devon dialogue, cap94, money26000
  from6000, and the previously unclaimed Pidgeotite remains exactly one after
  subsequent visits. No repeated grant or return to the obsolete main quest.
- `complete-map-ember-path-v5`: actual Jagged Pass entrance, Ember Path name
  popup, landmark flag1 and native zoomed PokeNav showing Route112 / Jagged Pass /
  Ember Path at the established marker. A direct synthetic cave jump used the
  fixture's old escape-warp origin; a later approach entered from the wrong
  side. Neither was evidence of a game map defect. The final recipe asserts the
  destination map and landmark, and the contact sheet confirms presentation.

The source-bound contact sheets above were visually inspected. These are
synthetic scene tests, not earned route progression. Native fixture setup and
`battle_resolution:"fixture_win"` are optional independent-worker facilities;
the latter uses existing campaign automation (automatic wild capture / forced
trainer win) and is explicitly labeled, never combat acceptance. Default scene
execution retains native battle behavior. The user's live Studio and earned C15
save are separate. The earlier Scanner/ruin puzzle evidence remains valid for
those unchanged scripts.

Logs are in `work/studio/choreography-completion`. The book import/check confirms
370 variants/parties/battle IDs and legal configured abilities. The text-width
check found no added over-budget line. Build the normal release with
`make -j6 release USE_LTO_ON_RELEASE=0`, stamp it with
`scripts/stamp_release_inputs.py`, then run
`scripts/verify_emerald_champions_release.py`; keep its ROM, ELF and stamp together.
The normal build excludes all fixture/agent mutation interfaces.

Remaining acceptance: user visual approval; actual full wreck key/current,
Magma Hideout/Seafloor/Meteor Falls navigation; uncaptured victory/flee/loss,
full-storage and clean-save/reload branches not explicitly exercised above;
and the paused earned campaign. These limits do not imply unbuilt section17
presentation. The early Devon/Steven/Wattson copy refinements have source and
font-width checks, without an additional full early-story replay.

Final normal release build and gates passed in
`work/studio/choreography-completion/release-build.log` and `release-gates.log`.
The input stamp binds28,978 inputs plus ROM/ELF bytes; the ROM uses27,631,040
bytes, EWRAM232,368 bytes and IWRAM28,356 bytes. The six-panel native overview is
`work/studio/choreography-completion/showcase.png`; its JSON sidecar links each
unaltered source capture and recording. Full sheets and motion previews remain
in the Studio Library. This is a local normal build; no new public ROM upload or
earned-run migration is claimed.


## Story gates and permanent collapse — 2026-09-14

Source and the single Book implement the approved ordering, encounter-resolution,
finite-document and one-stop Steven conclusion contracts. The old C26 permanent
entrance recipe was replaced, and C45/C48 scene expectations were updated to
require battle resolutions. Former observation/optional-battle and Devon-payment
recordings above remain historical evidence, not current contracts.

Normal build uses `make -j6 release USE_LTO_ON_RELEASE=0` with the configured
DEVKITARM; then `scripts/stamp_release_inputs.py` and
`scripts/verify_emerald_champions_release.py`. Logs: `work/story-gates/`.
Headless builds use `BUILD_NAME=emerald-headless EC_HEADLESS_FIXTURES=1 TEST=0`.
The tracked Studio recipes run via the skill's `tools/studio/run_scene.py`.
Every recording includes its exact immutable ROM/ELF, synthetic setup, original
PNG hashes, inputs and contact sheets; no cross-ROM savestate was reused.

Evidence directories below are relative to `work/studio/scenes/`:

- `gates-c26-fossil-blocked`, `gates-c26-fossil-decline`,
  `gates-c26-fossil-collapse`, `gates-c26-claw-collapse`: C26 prerequisite,
  cancellation and both native collapses/fossil receipts with returned controls.
- `gates-c26-warning-pages`: every warning page, explicitly naming Zygarde and
  Stakataka, permanent loss, further exploration and the final declined choice.
- `gates-c26-sealed-approach`, `gates-c26-upper-entrance-open`,
  `gates-c26-upper-entrance-sealed`, `gates-c26-underpass-open-v2`,
  `gates-c26-underpass-sealed-v2`: ordinary approach versus permanent closure.
  The first Underpass open recipe stopped on the arrow tile; v2 actually walks
  through the exit. This was an input-duration correction, not a relaxed gate.
- `gates-final-c26-archaeologist-warning-v2`: repeatable early warning from the
  existing archaeologist. The old Mirage basement inscription has no map binding
  and is not claimed as an active warning.
- `gates-final-c27-norman-blocked`, `gates-c43-triangle-blocked`,
  `gates-c48-league-blocked`: native prerequisite messages and returned control;
  eight badges alone cannot bypass the final conclusion.
- `gates-c48-origin-observation`, `gates-c48-meteor-observation`: declining after
  observing leaves resolution unset. `gates-c48-origin-fixture_defeat` and
  `gates-c48-meteor-fixture_defeat`: forced no-capture victories set only the
  resolution receipt. `gates-c48-origin-fixture_win`,
  `gates-final-c48-meteor-fixture_win`: forced captures set both receipts.
  `gates-c48-origin-fixture_loss`, `gates-final-c48-meteor-fixture_loss`: forced
  loss leaves resolution/capture unset. These validate native postbattle scripts,
  not battle difficulty, normal strategic wins, native fleeing or whole routes.
- `gates-final-c45-guardian-pending`, `gates-final-c45-flash`: puzzles alone cannot
  award C45. `gates-c45-final-guardian-v2`: forced final Registeel victory grants
  cap82/16000 without a capture. Its final line is the existing resting reminder,
  after the route directions; the v1 terminal-text expectation was corrected.
  `gates-c45-repeat`: rereading never pays a second grant.
- `gates-c48-steven-incomplete`, `gates-c48-steven-conclusion`,
  `gates-c48-devon-conclusion`: missing challenge blocks C48; Steven grants
  cap94/20000 once; Devon pays nothing and preserves the finite Pidgeotite.
- `gates-travel-documents-full`, `gates-travel-documents-repeat`: all five pending
  papers remain unreceived with a full key pocket; native healing still runs.
  With space, Center delivery gives exactly one of each and subsequent visits
  give none. Synthetic full-pocket fixture28 preserves the four Center tools.
- `gates-final-c44-scanner-repeat`, `gates-final-c30-return`: current-prerequisite
  Scanner repeat-payment and survey return-service regressions.

Contact sheets were inspected for the warning, collapse/closed entrances,
legendary aftermath, research/League directions and Center delivery/healing.
Local recordings are synthetic and not tracked binaries; tracked recipes make
these cases reproducible. Full earned traversal, all possible ordinary loss/run/
reload permutations and user visual acceptance remain separate work. The earned
C15 checkpoint and all paused team-review work were preserved.

`gates-c48-optional-capture-return-v2` checks the completed challenge's optional
rematch offer and declining it without revoking its receipt. The first archaeologist
recipe faced him from water at a different elevation and did not start dialogue;
v2 uses the adjacent land tile and requires actual text. The original ready-only
expectation was insufficient and has been replaced.
