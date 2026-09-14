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
habitats, economy and AI contracts, not just trainer moves.
Exact trainer authoring lives in data/emerald_champions/emerald_champions_battle_teams.txt.
The Game Book's prose is the design guide; its marked reference is generated from
source. Edit the source record, never both a copied table and its implementation.

```sh
python3 scripts/emerald_champions_teams.py --write
python3 scripts/emerald_champions_teams.py --check
python3 scripts/sync_game_book.py --write
python3 scripts/sync_game_book.py --check
```

The first check compares master/plans/native parties and configured abilities.
The second re-renders the reference and rejects stale bytes. Neither establishes
prose correctness, physical access or native battle quality. Shared presets retain
their own authoring/generator. Game_Guide_Reading_Copy.txt is a generated short view.

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

Before every battle: find its TRAINER identifier in the generated book reference
and inspect the authored teams file, native party and applicable shared AI.
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
required by the Game Book playtesting rules; those are not all complete in the current run.

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
team `--check`: authored IDs = nonempty native party IDs = Hoenn script battle IDs.
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

The authored team file now owns the four revised six-member teams. Generate/check
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


## September 14 early-access and documentation reconciliation

The Game Book battle/access section owns the corrected pre-Rusturf cutoff:22 trainer
encounters available before the Stone Badge (including Roxanne), plus Dawson,
Sarah, Janice and Jerry after Cut. Cyndy/Marlene are later Route115 actors; a
review-list position is not access order.

`work/early-access-audit-20260914.json` records source collision/obstacle and
directed-ledge checks. Its exclusion probe relaxes elevation to overapproximate
walking; this is not native traversal or inspection of a personal save. Shared
Briney/rival/field-move gates were traced separately in their production scripts.

`work/early-team-audit-20260914.json` verifies all1,759 native/book slots, all50
literal ability-carrier rows and the two changed trainer branches. Calvin is
unchanged. Joey now uses Leek/Scrappy Galarian Farfetch'd with supporting partners;
Marlene's unsupported Dazzling Gleam becomes legal Fake Tears. Both changed teams
pass the pinned move-pool check and the global configured-ability check.

`work/early-audit-book-final.log` and `work/early-audit-export-check.log` cover
book/materialized-team/plan/party agreement and the corrected catalogue exporter.
Historical master chapter/cap fields are explicitly labeled as preview metadata;
they do not govern current access or override the live-cap formula.

The normal release was built from a fresh object directory with the complete
local ARM toolchain, then relinked after final authoring/projection updates:
`work/early-audit-release-final.log`. Artifact/input checks, map integrity, team
and preset/Circuit projections, wild integrity, header and memory-region checks
pass in `work/early-audit-release-gates.log`. ROM, ELF, stamp and receipts are
preserved together under `../downloads/early-audit-20260914/`; both current user
ROM copies were updated with matching bytes. Saves and the paused campaign were
not advanced. No new native battle-quality or full-game acceptance is claimed.


## Generated book reference (current workflow)

The short guide above the marked appendix is hand-authored. Exact team data,
configured stats/evolutions, wild methods, literal reward providers and dialogue
are regenerated by `scripts/sync_game_book.py`; the book is no longer compiler
input. `--check` performs a full deterministic re-render and compares bytes. CI
runs that check. Unknown dynamic/native paths remain explicit rather than guessed.

```sh
python3 -m unittest discover -s tests -p test_game_book_reference.py -v
python3 scripts/emerald_champions_teams.py --check
python3 scripts/sync_game_book.py --write --inventory work/book-economy-audit-20260914/reward-inventory.json
python3 scripts/sync_game_book.py --check
```

The full book contains the reference the user requested and is necessarily larger
than the few-thousand-word guide. Its generated short reading view contains only
the guide. Speech is a literal-source atlas, not a simulated transcript: shared
text, build variants, native UI/battle strings, placeholders and unattributed
services are distinguished. The inventory's dialogue_routing maps source labels
to branch/call references without filling the readable book with script plumbing.
No global NPC IDs, source map coordinates or gameplay were changed for this tooling.


### Automatic reference updates

Normal `make` and `make release` refresh the book after the ROM succeeds. For
live edits, run `python3 scripts/sync_game_book.py --watch`; the watcher checks
source changes, waits for a completed save, and runs a fresh exporter process.
It also materializes changes to the authored team catalogue when projections
were not edited concurrently. Conflicting authoring/projection edits stop the
refresh with a visible error instead of silently picking an owner.

The watcher is a local process, not a scheduled cloud task. Restart it after
reboot/session shutdown. Build-time refresh and the CI freshness check remain
available when it is not running. The source reference is deterministic; manual
guide prose still requires editorial review when game behavior changes.

Procedural Circuit/Tent templates and generation rules are intentionally excluded
from the book at the user’s request. Campaign parties, regional starter/rival
inputs and story allies remain included. This does not remove game content.


## Full economy source atlas

Generated book section C is built by `scripts/economy_reference.py`. It includes
map pickups/quantities, hidden items, planted and empty berry plots, NPC/menu
entrypoints, fixed and random gifts, Pokémon trades, ordinary and decoration
shop stock, native free/evolution counters, BP/Coins/barter offers, wild held
items, global transfer hooks, native selector tables, configured cash prices and
a reverse reward index. Native economic code and incoming script conditions are
linked once rather than described by guessed reward rules.

```sh
python3 scripts/economy_reference.py --out work/economy-map-20260914
python3 -m unittest discover -s tests -p 'test_*reference.py' -v
python3 scripts/verify_mega_stone_rewards.py
python3 scripts/sync_game_book.py --check
```

The price projection evaluates configured C expressions; it does not maintain a
second gen-price table. Native selectors distinguish the ten evolution stones
from thirty-five other evolution items. Random berry gifts are one item chosen
from a range, not delivery of every berry in that range. Pickup amount uses the
actual object quantity field; hidden quantities are separate. Prototypes and
function definitions are not counted as transfer calls.

The earlier Mega source check omitted Steven's additem/addpcitem delivery. It
now recognizes those paths. Its harvest check is scoped to the actual trade
function; unrelated rollback of an ordinary two-berry gift is legitimate.

Source graph associations are potential paths. Build-condition alternatives,
unbound callbacks/legacy operations and configured items without a mapped fixed
source are visible in C6; they are not claims of first access or unobtainability.
The atlas cannot certify every native transaction, physical route, price budget
or narrative match. The user-approved shop roles remain distinct from stock
changes not yet implemented, particularly the proposed Lilycove Mega shelf.
No pickup position, price, stock, reward, trainer or saved-game progress changes
are part of the atlas work.


## Post-Roxanne Cut-enclosure audit

`work/post-roxanne-cut-audit/` contains the before/after authored teams, complete
native-slot comparison, nearby access/source inventory and campaign species/Mega
coverage snapshot. The first block is Janice/Jerry/Dawson/Sarah, all first
accessible with the Stone Badge and Cut; E-number order is not travel order.
Dawson is retained; Sarah's support/Speed IVs, Janice's allied Lightning Rod
conflict and Jerry's timely Acid Spray support are revised. No levels changed.

The current focused test build uses the three runner files plus
`test/battle/ai/primary_support_pair.c` and
`test/battle/ai/emerald_champions_plans.c`. All six selected test groups pass:
`EC primary support:` (three), `EC Tailwind:` (two), and
`EC battle plans: compiled directives` (one). These are existing native mechanic/
AI regressions, not four complete authored-team battles. See native-tests.log
and the matching test stamp in the audit directory. Full Protect-cadence and
optimized Medium difficulty benchmarks remain pending.

The normal production ROM builds and passes all release gates in
`work/post-roxanne-cut-audit/release-build.log` and `release-gates.log`.
`release-stamp.log` binds the ROM/ELF to current inputs. No earned save was
advanced, and this is not full native battle-quality acceptance.


## Rusturf-to-Dewford designs and Soak/Charge forecasts

Authoring, access inventory, before/after sets, all24 non-Hoenn middle-stage
rival replacements, affected-team index and baseline/fixed receipts are under
`work/rusturf-dewford-team-audit/`. Exact teams and abilities pass the existing
materializer/roster checks. Do not treat the three unused Hoenn replacement
presets as the actual bespoke Hoenn party branches.

The focused build uses the three runner files plus
`test/battle/ai/primary_support_pair.c` and
`test/battle/ai/emerald_champions_plans.c`. All17 selected groups pass in
`complete-block-native-tests.log`: `EC reactive Charge:`, `EC shoreline`, `EC Soak`,
`EC special suppression:`, `EC suppression mechanics:`, `EC primary support:`,
`EC Tailwind:` and `EC battle plans:`. These cover actual smuggler inheritance
from all three partners; Ned's authored Soak and Wind Power KOs; changed Ground
immunity, blocked/late conversion, protection/Fake Out, state/RNG restoration;
timely Eerie Impulse; and Electromorphosis with/without a real preceding hit.
Ned's first-decision samples are43 and53 frames, below72. They do not prove the
worst case or complete battle/Protect quality across the campaign.

Baseline fixture corrections remain explicit: required Speed declarations were
missing from the first inheritance control; suppression's original80-HP board
had a legitimate damage-roll risk, and its initial AI profile did not select the
shared evaluator. The corrected100-HP/profile case fails when Eerie Impulse is
removed from PairTargetDropStat and passes with it restored; this controlled
baseline is labeled separately from the historical full-tree ELF. A fake-out
countercheck now permits Thunderbolt to KO the other foe and instead rejects
using it on unconverted Ferrothorn. No meaningful assertion was weakened.

Broader previously failing `EC authored strategy:` Flannery/Parker expectations
and `EC doubles budget:` remain open. Verified old-artifact comparisons are in
`neighbor-old-artifact-baseline.log` and `budget-old-artifact-baseline.log`;
current diagnostics show Sleep Powder/Psychic and ordinary Kangaskhan. Audit the
actual choices, locks, eligibility and outcomes before changing game AI or
retiring those checks. These are not newly introduced Soak/Charge regressions.

Arrokuda's actual authored Propeller Tail bypasses allied Finneon Storm Drain
in the added native reserve-pair test. Normal release build and gates pass in
`release-build.log` and `release-gates.log`; the generated book check passes.
Artifacts and SHA256 hashes are in `artifacts.json`. No earned save advanced.


## Brawly Gym designs, Choice commitment, Rage Fist and setup dances

`work/brawly-gym-audit/` records seven before/after six-member parties, source
agreement, species reuse, affected-owner lists and native baseline/fixed logs.
All42 member move pools and the whole370-variant roster agree with source.
Initial levels are Medium cap20 seeds; no complete Gym fight or balance pass is
claimed. Map actors, rewards, independent clears and the C15 save are unchanged.

The test allowlist is the three runner files plus `primary_support_pair.c`,
`emerald_champions_plans.c`, `ai_doubles.c`, `coaching_pair.c`,
`test/battle/ability/blitz_boxer.c` and `test/battle/move_effect/rage_fist.c`.
`verified-native-tests.log` passes51 selected groups: `EC Gym`, `EC Coaching:`,
`Blitz Boxer`, `Rage Fist`, `EC expert pair: candidate evaluation`,
`EC authored strategy:`, `EC doubles budget:`, `EC shoreline`, `EC Soak`,
`EC primary support:`, `EC special suppression:`, `EC reactive Charge:`,
`EC Tailwind:` and `EC battle plans:`. Brawly/Cristian/Jocelyn samples are60/50/44
frames; the full-bench cold-Mega guarded/attacking cases are60/60. These are
sampled native decisions, not a global worst-case guarantee.

The original Flannery assertion omitted the actual Rock Slide commands; supplying
them produces After You, two KOs and full Torkoal HP. Parker was really locking
Body Slam during a double-Protect turn. A small first-lock future matchup value
fixes that while leaving current blocked damage at zero. His regression now
asserts initial Earthquake, legal room order, subsequent Instruct and both KOs;
obsolete absolute Speed values are gone. The cold-Mega fixture now differentiates
an idle guarded board from one that needs Mega Evolution and provides the actual
player replacement input. No gameplay Mega permission was relaxed.

Configured Oricorio Speed103 invalidated the first EV seed: it attacked before
Lilligant. The revised native Medium order is84/83; its Acrobatics comparison
shows25 before and37 after the copied Victory Dance. Easy/Hard78/77 and92/89
are source-formula checks, not separately played fights. A Taunt countercheck
allows legitimate direct offense rather than requiring Protect.

The larger forecast exposed a real heap limit. Production search owners now
restore their existing saved board before candidates and on exit, without a
second8-KiB snapshot. The pure evaluation API still saves/restores itself. Rage
Fist tables allocate only for participating users. The unchanged full purity
regression checks caches, battlers, fields, RNG and stack guards and passes.
Raw Rage Fist queries also preserve the live hit counter. Variable/per-strike
hit sequences use explicit lower bounds; these are bounded forecasts.

The inherited forced-switch Rage Fist assertion assumed Gen9 while the build
uses Champions. It now explicitly exercises both configurations, preserving
retention in Gen9 and reset in Champions; no native rule changed.

The subsequent Dancer follow-up below supersedes the old copied-attacks gap.
Teeter Dance/Lunar Dance forecasting remains open; the subsequent item-sequencing
pass below supersedes the demonstrated consumption gap. Current setup/attacking-copy, Rage Fist activation, Costar entry and
Mega/guard lines have focused evidence. Do not promote that
to full Dancer semantics, complete battles, Protect cadence or final balance.

Normal ROM build, source/artifact release gates and generated-book freshness
pass in `work/brawly-gym-audit/release-build.log`, `release-gates.log` and
`book-check.log`. SHA256 bindings are in `artifacts.json`. This is a source-bound
normal ROM, not a claim that full Gym playtesting or Dancer coverage is finished.


## Dancer forecast follow-up — September 14, 2026

`work/dancer-forecast-audit/Review.md` scopes the repairs, counterexamples and
remaining work. The normal source adds optional native copied-move damage caches,
nonrecursive extra actions, both-side Dancer consequences, item-loss query states,
copied Feather Dance and Clangorous Soul HP costs. Native damage queries restore
battlers, known abilities/items and RNG. Move selection and switch evaluation
share this path. No battle-engine Dancer mechanic was changed.

Build the Brawly test allowlist above plus `test/battle/ai/dancer_pair.c` and
stamp the resulting ELF. `verified-native-tests.log` passes64 selected groups:
the previous51 filters plus `EC Dancer` (11 groups) and `EC attacking Dancer`
(2 groups, including5 denial/protection parameters). Full-state/cache/field/RNG
purity now includes a Dancer/Choice Specs/defensive-item board. The mixed full-bench
Dancer sample is69 frames and cold-Mega samples61/60; these do not establish a
worst-case deadline. Initial copied-attack baseline failures are preserved in
`baseline-ai.log` with their original ELF/stamp. Disabling only Feather Dance's
copy makes the new support fixture select Water Gun and fail; restoring it
passes (`feather-control-tests.log` versus `verified-native-tests.log`).

Invalid first drafts were corrected from evidence: all native participants need
explicit Speed when any has it; an Assault Vest holder cannot spend the fixture
turn on Celebrate, so the vest query starts after that harmless turn. Fake Out
cannot deny a copy through the receiver's Protect; the permanent control covers
both the actually flinched receiver and the correctly guarding receiver.

Teeter Dance confusion and Lunar Dance sacrifice/replacement AI forecasting remain
open (zero authored trainer owners; player-triggered cases remain possible).
The native Lunar Dance two-sacrifice/two-heal control passes independently.
The following item-sequencing pass closes the demonstrated ordinary/copy
consumption gap; its native anchors and remaining approximation limits are
recorded below.
Full Gym battles, difficulty and Protect cadence remain pending. No earned save
was advanced. See `release-build.log`, `release-gates.log`, `book-check.log` and
`artifacts.json` in this evidence directory for the integrated normal build.


## Item sequencing and the Laura native benchmark — September 14, 2026

`work/dancer-item-sequence-audit/` contains the current follow-up. The same test
allowlist as the Dancer section passes71 groups in `final-native-tests.log`:
add `EC item sequence`, `EC Laura pivot` and `EC pivot exits` to the earlier
filters. Native controls show29 Fire-source /116 resisted Electric-copy /232
unresisted selected Revelation Dance damage. A controlled build withholding only
ordinary itemless anchors fails the corrected follow-up attack choice; restored
anchors pass. Plus/Minus interpolation now preserves hit/consumption metadata.
Knock Off respects Sticky Hold. Full purity and earlier support/Rage/Gym checks
pass. The current full-bench Dancer sample is70 frames; cold-Mega61/60.

The initial item fixture mistakenly paired Fiery Dance with Wacan Berry: the
copied move was still Fire. It was corrected to two Revelation Dance forms before
the controlled baseline was used as proof. An ordinary-hit fixture's260 HP was
outside its actual damage budget and became220; the boosted parameter uses320.
The Sticky Hold negative permits a better Knock Off target rather than forcing
a wasted attempt. An interim stamp correctly rejected an ELF predating a new
source edit; only the rebuilt `final-test-*` receipts establish current evidence.

`work/brawly-gym-benchmark-laura/` is a separate branch of the archived pre-Gym
native save, fetched by verified `v4-evidence-30.tar.gz`. It has no Knuckle Badge,
Laura initially unbeaten, and cap20. Native Meadow entry and Munkidori eligibility
were checked before assisted preparation. `laura-benchmark-party.json` and
`pre-gym-native-access.json` in `work/brawly-gym-audit/` bind the legal party, backup,
prepared battery and acquisition scope. No capture or primary C15 progression is
claimed. The15-turn clear has one player faint, native outcome1, trainer flag1,
returned field control and native held-item restoration. The won battery save
is `laura-cleared.sav`; the reusable prebattle battery is `pre-laura-ready.sav`.

`laura-decision-times.json` replays all15 completed decisions read-only and finds
max63 frames. It binds the exact immutable baseline ROM/ELF; it predates the pivot
repair. `laura-t1-native-decisions.txt` proves Mienfoo chose immediate SWITCH.
A controlled native U-turn line reaches the same Croagunk at34 HP while dealing12
extra damage to Sylveon. The corrected AI fixture now chooses that exit and its
reserved recipient; faster Hyper Voice or earlier Fake Out retain immediate
switching. Generic recoil, contact and Sitrus-trigger controls also retain it.
No RNG or battle state from these counterfactuals enters the live benchmark.

The runner syntax is `--until WIDTH:ADDR:MASK:VALUE`. Early ad hoc controller
stops used reversed mask/value; each observed turn endpoint was checked before
continuing. The zero-valued final-battle stop consequently ended after one frame,
without submitting the last attack. It was corrected before the single actual
turn15 action. These were driver stops, not failed battles or replayed RNG.
`time_decisions.py` already uses the correct symmetric mask/value test.

Native contact sheets `laura-baseline-opening.png` and `laura-baseline-finish.png`
were inspected; their sidecars preserve original hashes. System Python lacked
Pillow, so the existing renderer used the bundled workspace Python. Normal
release receipts and hashes are under this follow-up's `release-*`, `book-check`
and `artifacts.json`. This completes a baseline battle and focused fixes, not
Laura's final difficulty, the other Gym fights, all Dancer effects or the campaign.


## Laura calibration — September 14, 2026

`work/laura-calibration/results.json` records two further native runs: current
pivot code with a double-Cloak Munkidori/Sylveon lead at the old opponent levels
(14 turns, zero faints,14 decisions max53 frames), then Mienfoo26 with all other
members unchanged (12 turns, one faint,12 decisions max60 frames). These used
different tactical choices/RNG, so the result difference is not an isolated
causal estimate of two levels. The native threshold fixture isolates the actual
reason for the change: Mienfoo24 HP63/SpDef36 takes Psychic66–80;25 HP65/SpDef37
takes66–78;26 HP67/SpDef39 takes62–74. Player Munkidori20 has75 Special Attack
and Covert Cloak. The revised opener used focused Fake Out/Moonblast rather than
assuming the old guaranteed Psychic KO. Final pressure calibration stays open.

`final-native-tests.log` passes73 groups across21 filters on the current authored
levels. Add `EC Laura levels:` and `EC Laura Sash:` to the prior71-group filters.
The Sash check uses controlled native Toxic Chain outcomes: one case leaves
Tyrogue at1 HP; the other poisons/faints it and requests a replacement. The initial
compulsory-Sash-switch AI assertion was rejected because its guaranteed-survival
premise was false. No AI selection rule was changed for that hypothesis.

Mienfoo's level increase invalidated the older interruption fixture's premise:
its weak Hyper Voice no longer required an escape. The synthetic player Fake Out
now precedes any response and its Hyper Voice remains lethal; the original
immediate-switch assertion stays intact. Native threshold, Sash, pivot and
neighboring mechanics tests pass. The three late Play Rough misses in the
zero-faint run were inspected through read-only past-input replay; no live RNG
was read or rerolled. Both runs have actual clear flags/outcomes, native saves
and held-item restoration. Contact sheets `laura-double-cloak.png` and
`laura-level26.png` were inspected; black transition frames were replaced with
actual recorded battle moments. Original hashes remain in sidecars.

Sessions `work/brawly-gym-laura-retest/` and `work/brawly-gym-laura-level26/` retain
matching immutable ROM/ELF/trace sets and won battery saves. Continue the newer
branch only with its own artifacts, or use normal Save/clean Continue after any
build change. C15 remains paused. Native-control helper `work/laura-calibration/step.py`
uses the existing session API and correct WIDTH:ADDR:MASK:VALUE stop syntax.
Normal release/book/artifact checks are recorded in this review's `release-*`,
`book-check.log` and `artifacts.json`.

## Economy shop transactions (September14)

Build the headless ROM and stamp it, then run:

```sh
python3 scripts/audit/economy_shops_runtime.py --out work/economy-shops-new-run
```

This uses the ECONOMY_SHOPS synthetic prerequisite fixture and real NPC/menu
input. It asserts actual inventory/money after purchase/cancel/owned/full-Bag/
insufficient-money/bracelet branches, the six different stones plus Cord6000
itinerary, a Lilycove tool purchase and representative native price/resale rules.
It preserves ROM, ELF, input stamp, input/screenshot trace and a panel manifest.
No campaign save or trainer flag is advanced. The accepted implementation run is
`work/economy-map-20260914/native-shops-verified`; inspect its mega-shops.png and
evolution-shops.png for the native UI. Normal-ROM delivery is separate.
Deus index/ownership caller receipts are alongside it in the parent directory;
partial parser coverage and excluded script directories remain explicit limits.

## Slateport arrival design audit — September14

Seven trainers reviewed: Hailey, Edmond, Lola, Chandler, Dwayne, Johanna and Simon.
Exact before/after builds, role/stat reviews and nearby/full-campaign reuse
references are in `work/slateport-arrival-design-audit/`. The report follows
Stats, Could it be cooler?, then Overrepresented? for every trainer.

The final28 builds/112 moves match pinned learnsets; configured ability, roster
and authoring/materialization checks pass in `teams-check.log`. Normal release
build, artifact stamp and deterministic release gates pass in `release-build.log`,
`release-stamp.log` and `release-gates.log`. `book-check.log` owns final book
freshness. `artifacts.json` binds the normal ROM/ELF/stamp and current authoring.
No native battles or new AI tests were run; old headless/test artifacts are not
current-source evidence for these team changes. Difficulty levels are design
seeds. No saves, scenes, rewards or shared AI code changed in this block.

## Retrospective team similarity review — September14

`work/early-team-similarity-audit/Review.md` records44 reviewed first-access
encounters plus Cyndy/Marlene from earlier historical discussion,56 authored
variants and1035 concept-pair comparisons. Comparison data includes evolution
families, exact item/ability/move sets, lead pairs and manually reviewed battle
identities. Full-campaign owners of the three revised roles are indexed too.

Johnson, Jose and Cyndy received targeted revisions; the previous Slateport
changes were confirmed applied. The erroneous Tirtouga/Omastar Shell Smash
comparison was corrected in the guide/current review. `teams-check.log`,
`release-build.log`, `release-stamp.log`, `release-gates.log` and `book-check.log`
own source/build validation. No native battle or new AI tests were run.
The old source-bound native sessions remain historical, not replayed acceptance.

## Approved next20 implementation — September14

`work/trainer-next20-implementation/Implementation.md` and `applied-teams.json`
record the implemented20 concepts /25 authored variants. The original Astra
proposal remains under `work/trainer-review-next20/`. Approved parent changes
retain Carvanha and lead Rattata/Wailmer in the museum, and lead Lopunny with
fast bulky Imprison Kirlia for Sally. Her Calm Mind/SETUP role is retired;
Wally keeps the separate setup lesson. Lola's current reference now reflects
Daisy's resolved sun overlap.

All110 direct builds pass scoped move/ability/stat checks; current-source
materialization, configured ability/roster checks, normal release build/gates
and generated-book freshness are logged in this implementation directory.
`artifacts.json` binds normal ROM/ELF/stamp, source and book. No fresh native
battles or test-ROM runs were performed; older test/headless artifacts are
historical. Existing Jaclyn and Archie fixtures were inspected and remain
valid frozen-behavior/namespace checks, not current-party snapshots.

Native and ordinary AI handlers for Imprison and Sap Sipper activation were
traced. Their newly authored complete-turn pair valuation and battle quality
remain future focused checks; no new AI engine code or guaranteed battle
execution is claimed. Imprison cannot undo an already executed Protect.

## Implemented review65–84 — September14

`work/trainer-65-84-implementation/Implementation.md` records all20 approved
teams and94 exact builds. The source-backed ledger now marks indices1–84
implemented, plus Cyndy/Marlene as separately reviewed but access-deferred.
The next frontier is the post-Dynamo unlock, not more pre-Wattson indexing.

The Winstrate invitation clarifies the existing four-battle sequence and
map-transition reset until Vicky is beaten. No battle order, flag logic,
rewards or actor movement changed. Actual FONT_NORMAL glyph widths pass
within208px; maximum new line177px (`dialogue-widths.json`).

Current-game move checks include native preparation-preset grants as well as
pinned and explicit reviewed access. This preserves valid Wormadam Struggle
Bug, Vespiquen Tailwind and Pincurchin Rising Voltage; Sunflora Overheat is
an explicit extension. `static-checks.json` covers all94 builds. Live
materialization/ability/roster checks, normal release build/gates, book
freshness and artifact hashes are recorded alongside it. No native battle,
new test-ROM run or final difficulty acceptance is claimed.


September14, trainer review85–100 implementation: all16 approved encounters /64 builds are materialized, with100 first-access reviews implemented and343 retained encounters /370 variants. `work/trainer-85-100-implementation/` holds exact changes, source-to-proposal checks, materialization, normal release build/gates, book checks, hashes and cut reconciliation. The first PATH-selected compiler lacked newlib headers; the successful full rebuild uses `DEVKITARM=/Users/gguthrie/.local/share/arm-gnu-toolchain-15.2-20260718/Payload`. No AI C or battle scripts changed in this batch. Existing Lung/Jaylen native regressions use frozen sets and required no fixture changes. Full battle calibration and Protect cadence remain pending.

The adopted cut ledger was restored to the book and independently reconciled:139 first-pass cut groups /141 identities remain after Gina/Mia's restoration, plus five additional v4 retired identities. All146 are absent from active authored teams, nonempty native parties and Hoenn battle opcodes. This is source coverage, not earned traversal. The Deus index was refreshed with its existing partial-parse/exclusion limitations.
