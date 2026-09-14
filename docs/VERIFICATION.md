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
