# Overworld presentation pass — 2026-09-20

User priority: overworld presentation, correct movement choreography, NPC dialogue
and story coherence. Work scene by scene outside the separate core-file cleanup.
Preserve authored story and battle design; fix demonstrated defects and simplify
their implementation. Existing dirty source belongs to ongoing work.

## First completed slice

- Read the Rusturf rescue, Route116 interactions, tunnelers' rest house, Devon
  president introduction and Rustboro rival handoff scripts and relevant map data.
- Ran the native Rusturf rescue through the grunt interaction, postbattle item
  delivery, player sidestep, Briney's approach, reunion, farewell and departure.
  Inspected contact sheets and additional movement frames. Final controls return;
  Devon Goods quantity is 1 and FLAG_RECOVERED_DEVON_GOODS is set. The synthetic
  fixture forces the trainer win; this is presentation evidence, not combat proof.
  The initial 10,000-frame attempt ended during the farewell; extending the input
  budget completed the scene without a source change.
- Ran the native Devon president introduction through the employee's departure,
  return and escort, the player's desk approach, letter/PokeNav delivery and heal.
  Inspected its contact sheet. Final controls return, letter quantity is 1,
  FLAG_RECEIVED_POKENAV is set and VAR_DEVON_CORP_3F_STATE is 1.
- Fixed Rustboro rival farewell music cleanup. Both farewell branches previously
  consulted VAR_0x8008, but only May's invitation paths assigned it. Brendan could
  retain the encounter music override after victory when unrelated prior script
  work left this shared variable nonzero. Both rivals now share one unconditional
  cleanup tail; removed the obsolete May scratch writes. The existing music
  helper only fades when the current track differs from the map default.
  Dialogue, movement streams, trainer teams and invitation choices are unchanged.

## Verification and reproducibility

`test/rustboro_presentation.c` executes the assembled farewell routing and saved
music reset for both rivals with scratch values 0, 1 and 65535. It models dismissal
of the message and stops before the hardware fade. The corrected test failed on
the old script at the cleanup-target assertion and passes after the fix. This is
not an audible replay of the rival scene. Earlier test-harness iterations failed
on a tagged function-pointer comparison/constant spelling, not game behavior.

Native scenes: `tools/studio/scenarios/rusturf-rescue-presentation.json` and
`tools/studio/scenarios/devon-president-presentation.json`. Their synthetic native
fixtures establish explicit starting conditions; neither advances an earned save.

Evidence: `work/overworld-presentation-20260920/`, especially `rescue-complete/`,
`rescue-movement.png`, `devon-before/`, and `rival-{baseline,fixed}-tests.log`.
Visual captures bind ROM d480995719fdfd1b2a52f3311cf336f8a568fa8e21a2b6bae2b2414fd0605e29
and ELF 9cbed45449bca28ac6607e6e5164f728517585934722dc8b667bb066fc80c858.
These captures precede the rival-only fix; neither captured scene was edited.
The focused test build links the updated script successfully. No release ROM was
delivered and no claim of full campaign acceptance is made.

## Remaining work

The Rustboro rival replay gap is revisited below; audible music remains outside
the native image captures. The rescue's full-Bag retry and Devon return dialogue
are also revisited below. Check other rescue re-entry and
the transition through Briney's cottage to Dewford. Continue regional NPC review
with before/after story states, readable dialogue, facing, collisions, entrances,
exits and regained controls. The rest of the overworld remains unverified here.

## Persistent goal and analogous-bug pass

The user expanded this into a persistent whole-overworld/dialogue goal, explicitly
prioritizing Chansey, Gym blockers/guides, story order, and sprite metadata. Find
related failure classes as well as individual scenes; do not equate structural
checks with complete scene coverage. Inclement baseline is the local `v1.13` tag,
`cf41a95b68a39ca74fefeb934c460f6f47eb0b3b`.

Confirmed and fixed:

- Ashen Woods stage-4/5 reentry tried moving a live Chansey before objects spawned.
  Its relocation helpers now write the saved object-template coordinates too.
  Stage-5 native entry originally showed an inert ball, could not interact with
  Chansey, and stayed at 5; the repaired native encounter catches Blob and reaches
  6. The missing capture-ball hide initialization is restored, with entry cleanup
  for existing saves. No-ball refusal retains stage 5 and returns control.
- Route111's south approach forced the nurse right and player left. Reused
  `facetogether` so both face each other from their actual positions; before/after
  native south-approach captures confirm the repair.
- Jagged Pass's shove path attempted to restart Chansey's movement while its first
  escape was still running. It now waits for that movement after the player shove.
  Native replay retains the authored path and final quest stage 3.
- The Ashen Woods cook's mushroom text lacked EOS, consuming the next Blob text
  in its printer buffer. Restored EOS and visually checked the intended dialogue.
  Related scan of all 908 map scripts and 97 shared scripts found no second
  missing terminator; Route111's unreachable post-EOS page control was removed.
- Restored new-game hidden states for Roxanne, the Float Stone Hiker, Lucy,
  Cynthia, Heatran and the capture prop. Native baseline showed premature actors;
  talking to premature Roxanne advanced Rustboro 0 -> 9, and Heatran could start a
  battle without the Magma Stone. New-game tests read actual map object flag IDs,
  catching the mismatched old initializer names. Entry guards also correct
  early-state Roxanne/Lucy/Cynthia visibility; the apartment derives its two
  mutually exclusive actors from the Float Stone receipt on every entry.
- Petalburg's generated Wally's-dad identifier was attached to Wally's object.
  Corrected Wally=2/father=5 metadata names without changing their physical slots.

Search results and verification:

- Audited all 324 transition roots for the pre-spawn movement pattern. Ashen was
  the sole live-position setter case. The apartment's pre-spawn `removeobject`
  was redundant because its Ace already uses the receipt as a hide flag; removed
  that call while correcting inverse Hiker visibility.
- Do not infer missing persistence from absent script `setflag` calls:
  `RemoveObjectEventByLocalIdAndMap` sets the template hide flag implicitly.
  Removed Chanseys and the nurse do stay removed. The unused documented quest
  state 7 is not evidence of a repeating reward bug.
- Unequal movement lists alone do not prove a visual bug. Rusturf's Briney list
  outlasts Peeko's, but both are already outside the authored camera when removed.
- Native initial-visibility regressions failed before the fix. Six focused groups
  now pass: initial flags, apartment receipt selection, actor bindings, implemented
  static graphics, species normal/shiny/female graphics, and rival music routing.
  Repaired previously uncompilable metadata checks (API/name/bit-field issues).
  The intentionally unlinked FRLG bank and image-free OW_MON base are distinct
  from concrete Emerald sprite data, not silently accepted as valid map sprites.
- All 2,798 Hoenn map object graphics references resolve to the implemented static
  bank or recognized dynamic/species forms. This is not an animation/play proof.
- Rustboro's guide passed all four approaches, repeated full-Bag refusal, retry
  awarding exactly five waters, and completed reentry without duplicate gifts.
  Its old fixture/verifier used an unused guide variable; corrected to the actual
  city state. A first 4,800-frame attempt ended in the legitimate item message;
  the 9,600-frame bound accommodates slow text and all observable checks pass.
- Fresh Water quantities are authored Inclement behavior (Rustboro 5, Dewford 10).
- The broad text checker reports 18 possible width overflows and 72 box-length
  flags. Actual buffered-value review resolves the width hits as conservative
  placeholder false positives, and scrolling controls explain box-length hits.
  Do not rewrap valid text merely to silence those heuristic warnings.
- Structural progression verifier passes 540 maps, 3,992 physical events, 1,401
  warps, 17,548 references and 336 value-returning specials. It explicitly does
  not prove state-dependent reachability.

Evidence is under `work/overworld-presentation-20260920/{chansey,initial-actors,
gym-access,rustboro-guide-complete}`. Each scene records its immutable ROM/ELF.
Initial-visibility native logs and a preserved test ELF are in `initial-actors`.

Follow-up currently being verified: Lucy's real hot-springs doorway skips the
coordinate event on its forced arrival step (native replay, not just a warp
fixture). A one-shot arrival queue now runs the existing scene from the frame
callback. Verify normal entry, hot-springs reveal/decline and repeat entry before
calling this complete. Heatran's intended Magma Stone reveal still works natively.
Continue Brawly's return/Gym opening, both Wally approach exits, then regional
scenes and the remaining sprite animation metadata. Initial-state fixes do not
claim automatic reconstruction of already-corrupted historical story progress.

Latest user direction: stop messaging the other task and keep working on movement.
Native builds now use the isolated source snapshot whose path is stored in
`work/overworld-presentation-20260920/isolated-build.json`. Build and run scenes
there without shared-build coordination. Backport only reviewed, owned file changes
when the shared lock can be acquired, checking their saved pre-edit bytes first;
never overwrite independent changes. Do not resume cross-task messages. The whole
goal remains active.

## Continued movement checks

- Lucy's real hot-springs doorway now triggers her approach and refusal correctly.
  Ordinary front-door entry remains quiet; repeat hot-springs entry after refusing
  does not restart her introduction. Native captures inspected.
- Brawly's Slateport scene hides him and enables the Dewford entrance; native
  blocked/open approaches match the intended Inclement order.
- Wally and his uncle leave correctly from both accessible Mauville approaches,
  followed by Scott's departure and restored controls. These runs force a battle
  win and validate presentation, not combat. The first shorter south run stopped
  during Scott's dialogue; a longer run completed normally.
- Rusturf rock-clearing/reunion works from both sides. Lanette's three accessible
  approaches all return control and move her availability to Route114. Ivy/Evie's
  actual doorway introduction and repeat entry work. Mossdeep's five-person Magma
  departure finishes offscreen without blocking input. Inspected native sheets.
- Chansey's independent map copies could appear before their chase stages. The
  four map-entry callbacks now derive visibility from the existing quest variable:
  Route111 at0, Route112 at1, JaggedPass at2, AshenWoods at3-5, none at6/7.
  All32 map/stage combinations pass native flag, actor-presence and coordinate
  checks, including deliberately inverted saved hide flags. The four validated
  script updates were backported without touching unrelated work. Evidence:
  `chansey-stage-matrix/{results,actor-results,build}.json` and per-case recordings.
- Static sprite-frame review covered164 distinct Hoenn map graphics. Current
  reachable animations and allocations fit. Cycling-triathlete NPCs used a larger
  Acro Bike table with unsupported wheelie frames; changed the two pointers to
  Standard. All20 currently used animations are identical in both tables, so this
  removes unsupported metadata without changing authored motion. No current
  wheelie failure was claimed.

Remaining: complete the normal chase replay after the stage-visibility changes,
Meteor Falls initial/refusal/retry and victory choreography for both rivals,
continued regional movement/entrance/exit coverage, and retained dialogue cleanup
validation. Full map/script/overworld acceptance is still incomplete.

## Meteor Falls movement and cancellation repair

Native refusal tracing showed the camera completed two upward steps while the
player's four-step retreat was still running; release destroyed the movement task
and truncated the player at (14,17). The fourth destination (14,16) is water, so
blindly waiting for all four would have introduced another problem. Player and
camera now share a three-step retreat to the same safe ledge, and the script waits
for both before removing the camera. Both rival variants hand off at (14,17).

Real party-menu cancellation also reproduced a second defect: the grunt advanced
from (11,20) to (12,20), then (13,20), after two cancelled attempts. His forward
step now occurs only after party selection succeeds. Both rival variants pass two
complete cancellation/retry cycles with the grunt remaining at (11,20). Accepted
selection advances him once to (12,20), then the native postbattle scene reaches
state1 and returns control. Battle victory is fixture-assisted; tactics were not
validated. Contact sheets and task-observed input traces are in
`movement/meteor-cancel-observed-before`, `meteor-cancel-fixed-{60,61}` and
`meteor-accept-fixed`. The earlier B-only probe was a test-input failure: B rejects
the party menu's cancellation confirmation, so it did not reproduce a game hang.

The reviewed Meteor Falls script was backported with a byte-for-byte pre-edit
check. New movement edits continue in the isolated build. No cross-task messages
are needed. Next movement targets include Sootopolis escorts/camera sequences and
remaining route/house entrances and exits; this is still partial world coverage.

Continuation checkpoint: normal Route111 escape and Ashen Woods capture also pass
on the stage-gated build. Steven's long Cave of Origin escort passes from both
accessible approaches, with inspected native movement/arrival sheets. Evidence:
`movement/steven-guide-{east,south}`. The remaining Sootopolis legendary camera
scenes, later escorts, and wider regional coverage remain pending.

## Sootopolis and Sky Pillar native choreography continuation

On isolated ROM `e70d95c92880fa46`, both Center and Dive approaches pass the
legendary clash and Rayquaza resolution scenes. Wallace grants the Waterfall
license and clears the Gym doorway from south, east, and west approaches; each
run walks into the Gym afterward. Both Maxie-first and Archie-first departures
remove the leaders and restore control. Sky Pillar Wallace arrival/departure and
Rayquaza awakening also pass. Native contact sheets were inspected; these are
synthetic prerequisite scenarios, not earned campaign traversal. Evidence is in
`work/overworld-presentation-20260920/sootopolis/`. No source fix was warranted.
The initial leader probe sent a turn while the player was still walking and hit
the water interaction; adding a settled turn corrected the input, not game code.

Briney's moving-house first voyage, including Norman's automatic call, reaches
Dewford with the player and Briney correctly placed. Slateport return from the
west also passes with inspected boarding/travel/landing captures. Remaining boat
angles and route connections are being exercised independently of other tasks.
Full overworld coverage remains incomplete.

Boat continuation: Dewford-to-Slateport and Slateport-to-Dewford from all three
accessible approaches (west/east/north) pass. First-voyage, west/east return,
and Slateport arrival contact sheets were inspected. Early Dewford probes used
a non-walkable warp tile and then failed to advance the first dialogue paragraph
before choosing a destination; corrected inputs reach Route109 normally. Those
failures are fixture/input errors, not game defects. Evidence: `boats/` under the
same work directory. Dewford-to-Petalburg and further transport/re-entry checks
remain pending. No cross-task communication occurred during this continuation.

## Cable-car template-position repair (isolated, pending backport)

Both arrival scripts persist attendant coordinates (6,7), although native exit
movement ends at (6,6), matching the map definition. (6,7) is the player arrival
tile. Corrected both template coordinates to (6,6) in the isolated source. This
is a live/template mismatch; save/reload overlap was not reproduced and is not
claimed, because normal saves also retain live object events. Both native ride
directions pass on rebuilt ROM `71d3a73b4de12b6f`, with inspected contact sheets,
attendant (6,6), player (6,7), station state0 and control restored. Evidence:
`transport/cable-*-fixed`; original source is in `transport/cable-before.json`.
Canonical backport was deferred because the shared build lock was occupied; no
canonical game file was changed. The isolated fix remains ready for a byte-guarded
backport. Dewford-to-Petalburg also passes and reaches Briney's house; sheet
inspected. Repeat rides and wider transport coverage remain pending.

Continuation: cable-car round trips pass from both stations, including immediate
return boarding after arrival. All four Slateport submarine-theft approach paths
pass: harbor state2, submarine removed, Stern approaches, control restored.
First/fourth approach contact sheets inspected. Evidence: `transport/cable-round-*`
and `transport/submarine-{0,1,2,3}` on ROM `71d3a73b4de12b6f`. Source matches
canonical Harbor script. Same-block movement/template scan also checked Safari
attendant reset (31,34 + right ->32,34, correct); Fallarbor rival coordinates are
set before spawning, not stale end positions. Wider review remains incomplete.

Cable-car fix is now backported to both canonical station scripts after acquiring
the lock and verifying unchanged originals. Focused diff whitespace check passed.

## Fallarbor and Route119 movement continuation

All five Fallarbor trigger rows pass on ROM `71d3a73b4de12b6f`, alternating
May/Brendan fixtures: player ends at (13,10), state1, both walking/bike actors
hidden, control returned. May outer and Brendan inner contact sheets inspected.
Scene source matched that ROM. The walking actor was mislabeled
`LOCALID_FALLARBOR_RIVAL_ON_BIKE` and used RIVAL1 at legacy bit0x3C rather than
the authored rival hide flag at0x4DC. Clean-save fixtures still showed the
actor, but old Dome Fossil receipts could hide it. The later legacy-save pass
below reproduces and repairs this product defect; these first five probes
do not cover the old-save case.

Route119 both trigger lanes and both rival variants pass native entry, swap,
battle-return, bike departure and Scott arrival/departure. Victories are fixture
assisted: these runs verify overworld transitions, not battle tactics. State1 and
both departure hide flags are set with control restored. Evidence:
`movement/fallarbor-{0,1,2,3,4}`, `movement/route119-{85,88}`. Full regional and
re-entry coverage remains incomplete.

## Lilycove rival Fly scene

May and Brendan native postbattle Fly departures pass on isolated ROM
`71d3a73b4de12b6f`: met flag set, rival hidden, control restored. Victories were
fixture-assisted, not tactical validation. Brendan departure was inspected in
a denser final-frame sheet (`movement/lilycove-132/fly-detail.png`) because the
default contact sheet skipped the brief bird effect. Repeated May refusal twice
also passes, leaving the rival visible, met flag unset and refusal flag set.
Evidence: `movement/lilycove-{129,132}`, `movement/lilycove-refusal`.
Route128 departure setup initially activated OnFrame before the requested warp;
the replacement starts off-map to preserve native entry order. This is a recipe
setup error, not a gameplay failure. Its result is recorded separately below.

Route128 harness follow-up: off-map setup reached native entry, but the portable
save request raced the OnFrame scene beginning. Reran with portable-save creation
disabled, preserving a same-ROM raw initial state instead; no game code changed.
Canonical Route128 script matches the isolated build.

Route128 raw-start run passes: native Archie/Maxie departure and Steven flight
arrival/departure reach state2, all three actors hidden, player at (38,22), control
restored. Contact sheet inspected. Evidence: `movement/route128-departures-raw`.
No game-code fix was warranted in this batch; further world coverage is pending.

## Devon Goods direct-interaction facing repair

Native north-side interaction reproduced employee9 facing west (3) despite the
player standing north. ReturnGoodsSpokeToEmployee sets TEMP1=4 and its helper
forced left, overriding the initial faceplayer. Changed that helper's movement
to Common_Movement_FacePlayer. Rebuilt ROM `d1d784210333dc3a`: north now faces2,
east faces4, confirmed from native actors. Evidence: `movement/devon-direct-*`
and `devon-facing-verification.json`. Canonical backport applied: False.

Rustboro stolen-Goods chase passes outer trigger rows20/24, setting city3,
route/tunnel prerequisites and employee visibility. North-row sheet inspected.
Follow-up help probes used blocked tiles and were not valid tests; no game
conclusion drawn from those failures. Broader chase/re-entry coverage remains open.

## Related facing-call audit and schoolteacher circuit

Reviewed the 18 direct VAR_LAST_TALKED fixed-cardinal face movements across
12 map scripts. Ferry boarding turns, Fly departure facing, elevator operation,
Space Center threat-facing and teacher class-facing are intentional contexts,
not equivalent to the Devon helper overriding a conversation approach. This
search does not cover every actor-specific movement or prove all facing correct.

Native schoolteacher circuits from east/west pass on ROM `d1d784210333dc3a`: each
returns teacher6 to (5,3), grants Quick Claw, sets receipt and releases control.
West contact sheet inspected. Evidence: `movement/school-teacher-{east,west}`.
Devon fix canonical backport applied in this continuation: False.

## Oldale Mart guide movement

All three accessible initial approaches (north/south/west) pass native escort:
player (13,8), guide2 (13,7), receipt flag set, control restored. West run includes
a second conversation; no escort replay or actor displacement. North contact
sheet inspected. Adjacent east tile (14,14) has collision1, explaining the absence
of a DIR_WEST branch. Source matches canonical; no game-code change warranted.
Evidence: `movement/oldale-escort-{north,south,west}`, ROM `d1d784210333dc3a`.
Full-Bag/re-entry states and remaining Oldale rival/blocker scenes are not claimed
by these runs. Devon facing backport applied during this checkpoint: True.

## Oldale blocker and rival departure

Native repeated west-path rejection passes before ADVENTURE_STARTED: player
returns (1,10), blocker returns to his start, control restored twice. After that
flag (set in Birch's lab), entry callback sets town state1 and traversal into
Route102 passes. The first open-path probe ended during normal map transition;
180 settling frames confirmed readiness, not a game fix. Closed-path contact
sheet inspected. Rival approach lanes8/10 and direct north conversation pass,
ending rival state2 with hide flag set and control restored. Lane8 sheet inspected.
Evidence: `movement/oldale-blocker-closed`, `oldale-blocker-open-checked`,
`oldale-rival-8-checked`, `oldale-rival-10`, `oldale-rival-direct` on ROM
`d1d784210333dc3a`. No new defect confirmed; wider coverage remains incomplete.
Focused diff checks for applied Devon/cable-car script corrections also passed.

## Wally catching tutorial and Gym return

South and west Norman approaches pass the complete native tutorial on ROM
`d1d784210333dc3a`: Wally arrives, both exit, connected-map escort reaches
Route102, scripted catch battle completes, both return to Gym, Wally departs and
Norman returns control. Final Gym state2/city state3 and Wally hidden. These runs
use the native tutorial battle, not forced-win resolution. West sequence sheets
1/2/3 inspected across introduction, catching and return. Initial 17,800-frame
captures ended in legitimate farewell/Norman dialogue; a work-only runner with
26,000-frame allowance completes both. Evidence: `movement/wally-tutorial-*-complete`
and `extended_tutorial.py`. Gameplay source was not changed for this extension.
Current canonical Gym script differs from isolated build only in unrelated
post-Badge5 Regenerator explanation; tutorial movement scripts match. North/east
Norman approach variants and broader campaign coverage remain unverified here.

Wally tutorial continuation: north/east Norman approaches also pass the complete
native tutorial and return on the same ROM. All four approach directions are now
covered for this sequence. North introduction sheet inspected and native actor
coordinates confirm expected alignment. Evidence: `movement/wally-tutorial-
{north,east}-complete`. These are synthetic starts, not earned playthroughs.

Wally's father escort passes via the exact scripted Gym-door destination (15,8):
forced exit step, outdoor following, house-door animation, indoor facing, Surf
license and release all complete. Final city5/Surf flag1/outdoor father hide1.
Contact sheet inspected. Evidence: `movement/wally-father-native-door`. The normal
Studio warp validator rejected this collision-marked door tile, so a work-only
wrapper invoked its native field warp directly for the source-specified entry,
with no game-code change. First rejected probe is not a game failure. Current
canonical Petalburg City/house scripts match this build. This does not validate
the preceding Norman battle or every Gym exit branch.

Evidence qualification: the father-escort runner waited through outdoor movement
before recording began; its sheet shows the indoor handoff only. The end state
proves the chain completed, but outdoor trajectory/door visuals still require
an earlier recording start and are not yet visually accepted.

Father escort visual gap closed: `movement/wally-father-early-capture` starts
before the pending map transition, captures the outdoor route and house entry,
and passes the final state assertions. Sheet inspected; `father_early_capture.py`
preserves the work-only driver. No gameplay change. A bounded straight-line scan
of all map/shared scripts found zero applymovement→release candidates before
an intervening wait/call/goto/dialogue/delay/warp. This does not resolve branched
or concurrent timing and is not a whole-script correctness proof.

## Direction-switch review and Spenser departure

Enumerated 12 map-script VAR_FACING switches with fewer than four contiguous
cases. This is a candidate scan, not proof of missing branches: Space Center1F
has a preprocessor branch (BUGFIX is defined), and Steven has fallback movement.
Fortree Spenser north tile(4,1) is blocked; Safari attendant north/east tiles
(32,33)/(33,34) are blocked, with north a door warp. Museum's northern tile is
a stair warp; reachability from upstairs still needs a dedicated check.

Spenser's two distinct departures pass native south/west interactions on ROM
`d1d784210333dc3a`, with forced battle wins: rewards complete, actor removed via
hide flag and control restored. South sheets inspected through departure. Both
movement endpoints match the two Mart exit warps (3,7)/(4,7). Evidence:
`movement/spenser-exit-{south,west}`. This is not tactical battle validation.
No additional direction-lock defect confirmed by this batch.

## Invalid copyvar literal reads: five-script repair

Museum native tours from south/west finished with state8196 instead of1.
ScrCmd_copyvar dereferences GetVarPointer(source); GetVarPointer(1) returns NULL.
These scripts used copyvar with literal1 where setvar was required. Corrected
museum2F, contest lobby's unused debug setup, SkyPillar2F/4F and cave_hole shared
reset. Nonzero checks concealed the bad value; no softlock is claimed from it.
Scanned all map/shared copyvar sources: other non-VAR names resolve to variable
aliases, not literals.

ROM `ee5746fad80e7785` builds and focused diff check passes. Native museum tour now
sets state1; SkyPillar2F/4F, MirageTower2F/3F and MtPyre2F entry resets each produce1
from seeded999. Evidence: `movement/museum-curator-south-fixed`, `ice-reset-*`,
`copyvar-before.json`. Initial Granite probe raced a field transition/save at an
arbitrary tile; stair-adjacent replacement result recorded separately.
Canonical backport applied: False. Museum held-direction probe on old ROM also
completed with the same incorrect state; no separate control-lock bug confirmed.

Granite Cave B1F stair-adjacent entry also passes: seeded999 resets to1. All six
active floor-entry callers now have native reset evidence on the corrected ROM.
Five-script canonical backport remains pending the shared lock.

## Preventing recurrence of literal copyvar operands

The existing copyvar assembler macro already diagnosed out-of-range source
operands, but only emitted a warning. Upgraded that specific warning to an error,
retaining its existing variable-range predicate and explicit warn override.
Real ARM assembler probes verify literal1 fails while saved/special variable
sources pass. Full isolated build passes; ROM SHA remains exactly
`ee5746fad80e778550de85c1688412c50f9c502138ae3a7219ac4350bfa76067`, so previous
native tests bind to unchanged game bytes. Evidence: `copyvar-guard-results.json`,
`copyvar-guard-build.log`. No numeric destinations found by the related
setvar/addvar/subvar/setorcopyvar/map/shared scan. Canonical six-file backport
applied: False. Comprehensive overworld review remains unfinished.

## Museum refusal and return-path follow-up

Corrected-ROM not-yet refusal passes: control restored downstairs, curator
visible and introduction state0. Upstairs-to-downstairs-to-upstairs native stair
round trip passes with state1 preserved and no repeated introduction. Revisit
contact sheet inspected. Evidence: `movement/museum-not-yet`, `museum-revisit`.
The assembly macro has concurrent unrelated edits; pending backport must replace
only the copyvar diagnostic line, never overwrite the full macro file. Five
script originals still match preserved before-images. Shared lock remained
occupied during this continuation, so no main-source mutation was attempted.

East-side museum probe terminated with native worker EOF (IncompleteReadError);
no gameplay result was produced. It needs a fresh worker retry and is not counted
as either a game defect or a passed scene.

Five copyvar-literal corrections and assembler error guard are now applied to
canonical source. Replaced only the diagnostic line in event.inc, preserving
concurrent setmonmove/store_lock_anim edits. Focused diff check passes.

East museum retry diagnosis: native worker failed before its first60 boot frames.
core.log identifies dyld missing libx265.215.dylib through FFmpeg, not a ROM
crash. The exact matching library still exists at Homebrew Cellar x265/4.1/lib.
Rerunning with process-local DYLD_FALLBACK_LIBRARY_PATH pointing there; no global
symlinks, package changes, or game-code changes required. Full data-tree scan of
3,945 .inc/.s files found no remaining numeric copyvar sources after backport.

East museum tour passes with the exact-library fallback: upstairs state1,
curator hidden downstairs and control restored. Evidence: museum-curator-east-
runtime-fixed. Native test runtime is available again with that process-local
environment. All three accessible curator approaches have completion evidence.

## Safari exit and refusal choreography

Native south/west exits pass on ROM `ee5746fad80e7785`: attendant clears the
path, door transition and entrance-building walk complete at (9,4), Safari
mode cleared, control restored. West sheet inspected through doorway and indoor
walk. Repeated refusal twice leaves player (31,34), attendant1 (32,34), Safari
mode active and control restored. Evidence: `movement/safari-exit-{south,west}`,
`movement/safari-refusal`. Canonical scripts match the tested source. Synthetic
setup sets mode flag only; paid admission, ball counters, timeout and caught-mon
statistics are not validated by these choreography probes. No code fix warranted.

## Safari admission choreography

Native paid admission passes: counter dialogue, 500 charge visible (3000→2500),
interior walk, warp, player exit step, attendant reposition and control restored
in SafariSouth with mode active/state0. Sheet inspected. Full-party/full-PC
rejection stays in entrance with mode off/state0. Insufficient-money rejection
stays in entrance, restores control and preserves499. Evidence:
`movement/safari-admission-{125,127,128}`, ROM `ee5746fad80e7785`. These use actual
EnterSafariMode on success, unlike earlier flag-only exit probes. No game-code
change warranted; timed expiry and caught-mon statistics remain outside coverage.

## Safari feeder dialogue consistency repair

Three reachable NPCs still advised loading Pokeblock feeders or claimed to have
loaded them, while EventScript_PokeBlockFeeder only shows an empty-box message.
Updated South boy/youngster and RestHouse man to describe empty boxes and ordinary
encounters. Retained battle-use Pokeblock advice; no Safari mechanics changed.
Build and focused diff check pass. All three native dialogue probes pass and
release control on ROM `cc7b755d77abf596`; boy sheet inspected. Evidence:
`movement/safari-feeder-{boy,youngster,rest}`, `safari-dialogue-before.inc`.
Canonical backport applied: True. Retirement/timeout native checks remain
pending; inspecting their scripts does not count as runtime acceptance.

## Safari menu retirement and timeout probe

Paid admission followed by START→RETIRE→YES passes natively on ROM
`cc7b755d77abf596`: entrance return, mode0/state0, control restored. Evidence:
`movement/safari-menu-retire`. Timeout probes that seeded the u16 step counter
to1 did not trigger on the attempted movement; cause remains unresolved and no
game defect or successful timeout validation is claimed. The corrected probe
preserves the adjacent halfword when writing counter RAM. Evidence:
`safari-timeout-settled`, `safari_timeout.py`. Inspect actual counter reads and
step-callback reachability next before changing game code.

## Safari timeout resolved as probe alignment error

The native step counter is u16 at 0x02034f82 on this ELF. Studio's RPC writes
32-bit words; the earlier unaligned write did not reliably seed that halfword.
Corrected the work-only probe to read/modify/write its aligned containing word,
preserve the other halfword, then read back and assert counter1. Native walking
now triggers TimesUp and returns to the entrance with mode0/state0 and control
restored. Evidence: `movement/safari-timeout-aligned`, `safari_timeout_aligned.py`;
ROM `cc7b755d77abf596`. Contact sheet inspected. This validates final-step timeout
after real paid admission, not a complete 500-step walk. No game-code fix required.

## Mt Pyre summit choreography

All three Aqua departure approach lanes pass natively: Archie/player facing,
fade removal, elder approach, Magma Emblem handoff, summit state1 and release.
Right-side sheet inspected. Later orb-return center approach also passes: both
leaders descend, Maxie approaches the player then rejoins Archie, both depart,
state3 and both hide flags set, control restored. Sheet inspected. Source matches
canonical; ROM `cc7b755d77abf596`; synthetic prerequisites. Evidence:
`movement/mtpyre-lane-{147,148,149}`, `mtpyre-orb-return`. No fix warranted.
Orb-return outer lanes and complete re-entry/full-Bag paths remain outside these
particular checks.

Mt Pyre orb-return outer lanes22/24 now pass with state3/both leaders hidden and
control restored. Lane22 contact sheet inspected: Maxie approaches player's right,
returns to Archie and both descend clear of the stairs. Completed-state revisit
across the trigger stays quiet (no text), state3/hide flags retained. Evidence:
`movement/mtpyre-orb-return-{22,24}`, `mtpyre-orb-revisit` on ROM
`cc7b755d77abf596`. All three approach variants now have native completion
evidence. Sootopolis departure source sets summit state2 and unhides both leaders,
consistent with this later scene. No code change warranted in this continuation.

## Route116 NPC movement continuation

Devon employee departure passes from west (detour) and south (straight route),
with Aggronite receipt set, employee hidden and control restored. West sheet
inspected. Briney's coord trigger plus repeat conversation pass with route state2
and control restored. Evidence: `movement/route116-devon-{west,south}`,
`route116-briney`, ROM `cc7b755d77abf596`; source matches canonical. No fix warranted.
This batch does not retest Peeko's Rusturf rescue or claim employee gift full-Bag
coverage. Glasses-man paths and route re-entry behavior remain follow-ups.

## Route116 glasses-search NPC

Both departure paths pass after the local hidden-item flag is set: west-side
interaction takes the upward detour, south takes the straight left route; NPC
hide flag set and control restored. West contact sheet inspected. Owning Black
Glasses with the local pickup flag unset correctly leaves the NPC present after
rejecting that pair. FoundBlackGlasses reads exactly the map pickup flag.
Evidence: `movement/route116-glasses-{west,south,unfound}`, ROM
`cc7b755d77abf596`. No source fix warranted; bag-absent/found branch and actual
hidden-item collection are not covered by these synthetic starts.

Route116 glasses found-but-absent branch passes: Bag count0, correct no-glasses
text, departure hide flag1 and control restored. Initial setup attempted adding
quantity0, which the native command rejected; corrected start omits the item
and asserts absence. Evidence: `movement/route116-glasses-absent-checked`.

Route118 Steven outer trigger lanes43/45 pass ledge jump, dialogue and departure,
ending route state1/Steven hidden/control restored. Lane43 sheet inspected;
canonical script matches. Evidence: `movement/route118-steven-{43,45}`, ROM
`cc7b755d77abf596`. Center lane/re-entry remain untested in this batch.
No game-code changes warranted.

Route118 center approach and completed-state revisit pass on the same ROM,
completing native checks of all three trigger lanes. Evidence:
`movement/route118-steven-{center,revisit}`.

Weather Institute east-side Shelly interaction passes the forced-win transition,
grunt shove, Aqua removal and scientist handoff with control restored. Evidence:
`movement/weather-institute-handoff`, ROM `cc7b755d77abf596`; canonical script
matches. This does not assess battle tactics or every gift outcome. North/south
adjacent Shelly tiles are blocked; west is walkable in tile data and its actual
reachability/choreography needs follow-up. No game-code change in this batch.

## Weather Institute approach reachability and scientist re-entry

Collision-grid flood fill from the upstairs entrance, blocking only Shelly's
(4,6) tile and ignoring all other actor blockers, cannot reach (3,6); (5,6) is
reachable. Thus the west interaction is unreachable before defeat under normal
walking, despite its locally walkable tile. Evidence: weather-institute-
reachability.json. No unsupported alternate-approach fix was introduced.

Native scientist placement checks pass all four entry states: Aqua(1,6), reward
(4,6), completed(18,6), postgame(2,2). Route119 entry sets completed state2.
Evidence: `movement/weather-placement-*-near`, ROM `cc7b755d77abf596`. First probe
looked from the far staircase and the actor was outside the active object area;
nearby entry probes corrected that test assumption. This is placement evidence,
not comprehensive dialogue/reward validation. No source change in this batch.

## Weather Institute full-party and repeat dialogue

Full-party native handoff after fixture-assisted Shelly victory passes: recorded
text explicitly transfers Castform to Someone's PC/BOX1, receipt flag1, Aqua
hidden and control restored. Separately seeded received-state interaction gives
research dialogue and returns control. Evidence: `movement/weather-institute-
full-party`, `weather-institute-repeat`, ROM `cc7b755d77abf596`. Shared transfer
script routes no-space failure to message/release without setting receipt, so
retry remains available; full-PC failure was source-traced, not natively tested
here. Transfer message alone is not a direct storage-content count. No source
change warranted in this continuation.

Weather Institute full-storage branch now passes twice natively using the
full-party/full-PC fixture: no-space text, receipt remains0, Aqua remains hidden
and control restored. Evidence: `movement/weather-institute-full-storage` on
ROM `cc7b755d77abf596`. This verifies refusal availability, not freeing storage
and completing the subsequent retry.

Route120 Devon Scope scene passes both south/east starting approaches with
fixture-assisted wild capture: player reposition, Kecleon reveal/battle return,
Scope receipt, Steven Fly departure and control restored. East sheet inspected.
Evidence: `movement/route120-scope-{92,93}`, ROM `cc7b755d77abf596`; canonical
script matches. Defeat/run/loss outcomes, full-Key-pocket refusal and bridge
traversal after completion are separate outstanding checks. No tactical or
whole-route acceptance claimed.

## Devon Scope refusal and bridge traversal

Both starting approaches with full Key Items/PC storage refuse before the
Kecleon encounter: Scope flag0, Steven hide0, control restored. Successful
Scope scene followed by six westward steps crosses the former obstruction and
ends at (7,16); final native frame inspected. Evidence: `movement/route120-
refusal-{94,95}`, `route120-bridge-cross`, ROM `cc7b755d77abf596`. The successful
scene uses fixture-assisted capture. Other battle outcomes remain separate.
No source change warranted by these probes.

## Actor-range scan and Route121 departure

Checked1,606 direct numeric/local numeric actor references in map scripts against
map object counts; no out-of-range candidate found. Cross-map references, dynamic
variables and semantic identity require separate review. Evidence:
`movement/actor-id-range-scan.json`.
Route121 outer trigger rows5/8 pass native three-grunt departure, state1/shared
hide flag1, control restored. Row8 sheet inspected; all depart below the visible
area before removal. Evidence: `movement/route121-aqua-{5,8}`, ROM
`cc7b755d77abf596`; source matches canonical. No source change warranted.

Route121 completed-state revisit passes without replay. Evidence:
`movement/route121-revisit`, ROM `cc7b755d77abf596`.

New confirmed content mismatch requiring repair: TrickHousePuzzle5 retains the
15 vanilla question texts while src/data/script_menu.h supplies doubles-mechanics
answer lists. Example Mechadoll3Quiz1 asks Harbor Mail vs Burn Heal pricing but
answers are BURN/POISON/SLEEP. Script still accepts index0. Both source files are
clean in Git, so this is committed incoherence, not an in-progress edit. Current
menu last changed in43ac8d634e; its parent's puzzle script also has vanilla text.
Need trace intended quiz content and repair questions/choices/keys coherently,
then native puzzle movement/answer checks. Not fixed yet.

## Trick House authored quiz restored

Recovered matching doubles questions from13061e9ba2^, before the Inclement rebase
restored vanilla question text. Restored15 authored questions and doll3 intro;
corrected three stale keys: doll2Q3→0 (Taunt), doll5Q2→2 (Wide Guard), doll5Q3→0
(Dark). Existing doubles menus preserved. All15 key/menu mappings verified in
trick-answer-audit.json; current powder/Prankster settings retain modern rules.
Build/diff check pass. Native first-doll quiz shows restored question/menu, rejects
the selected wrong answer, performs failure choreography and returns control.
Evidence: `movement/trick-quiz-native`, ROM `df3fe5875aa071fc`. All15 native answer
branches/puzzle traversal still require verification; no whole-puzzle pass claim.
Canonical backport applied: False.

Trick House authored questions and answer keys successfully backported.

Native first-doll Quick Guard answer accepted with success dialogue/control
restored on ROM `df3fe5875aa071fc`; evidence `movement/trick-quiz-correct`.
Canonical restored script passes diff check. Remaining14 questions and full
puzzle traversal still need native checks; mapping verification alone is narrower.

## Trick House movement/reset follow-up

All22 placed doll triggers have matching TEMP9 offsets and movement step counts
that place their doll adjacent to the triggering player; source/map check only.
Evidence: `movement/trick-trigger-endpoints.json`. Native wrong answer returns
to (0,21) and resets all five TEMP progress variables to0. Correct first answer
permits movement past the trigger to (5,16) with TEMP1=1. Initial probe expected
(5,15), but that tile is blocked by the authored map; corrected path turns left
then north. Evidence: `trick-wrong-reset`, `trick-solved-pass`, and follow-up
`trick-solved-pass-path`. No movement code fix inferred from that input mistake.

## Trick House5 scroll and door

Native locked-door refusal preserves state0. Reading scroll twice sets/preserves
state1. Door with known code sets state2, changes stairs, and player walks into
TrickHouseEnd successfully. Evidence: `movement/trick-door-{locked,open,traverse}`,
`trick-scroll-repeat`, ROM `df3fe5875aa071fc`. Shared entrance script matches
canonical. These isolated end-section starts do not prove full puzzle traversal
through all five dolls; that remains pending. No additional source fix needed.

## Trick Master puzzle5 completion

Reward/departure from south/west passes natively: reward dialogue, spin/jump
exit, actor hidden, puzzle level4→5 and control restored. South sheet inspected;
continued A presses also interact with the revealed hidden item afterward. East
probe targeted the table's blocked tile and was corrected to west; not a game
failure. Evidence: `movement/trick-master-reward-{south,west}`, ROM
`df3fe5875aa071fc`. Source comparison of all seven ordinary immediate/deferred
prizes matches item identity; evidence trick-deferred-rewards.json. Deferred
claim mechanics and next-puzzle availability need further native coverage.

## Trick House next-puzzle gate

Native level5 entrance checks pass both Badge7 states: absent→entry state3
(waiting), present→state0 (next hiding challenge). Evidence:
`movement/trick-next-{False,True}`, ROM `df3fe5875aa071fc`. Deferred-claim initial
probe hit blocked tile(5,3); replacement uses walkable west approach(4,2), result
in trick-deferred-claim-west. No game failure inferred from rejected setup.

Deferred puzzle5 claim plus repeat conversation passes: exactly1 Rare Candy,
pending flag0, entrance state3, control restored. Result: trick-deferred-claim-west.
No source change needed.

## Trick Master interception and corridor return

Repeated exit interception before claiming reward passes: player returns(2,3),
level4 unchanged, control restored. Master jumps in place rather than relocating.
Completed-level5 corridor return reaches entrance state5 and clears corridor
marker. First probe stopped on exit tile(4,23); longer downward input completed
the directional exit, not a game repair. Evidence: `movement/trick-exit-intercept`,
`trick-corridor-return-exit`, ROM `df3fe5875aa071fc`. No code change warranted.

## Three corrected quiz keys: native branch checks

Direct native compiled-script entry plus real multichoice input verifies
doll2Q3 Non-damaging moves, doll5Q2 Wide Guard and doll5Q3 Dark are accepted.
All show success and restore control. Evidence: `movement/trick-branch-2-3-checked`,
`trick-branch-5-{2,3}-checked`, work-only `trick_branch_probe.py`; ROM
`df3fe5875aa071fc`. Synthetic context entry bypasses random question selection and
doll approach choreography, not the question/menu/answer script itself. Initial
probe pressed A on injection frame and started ordinary doll interaction instead;
corrected first-frame input prevented that harness interference. Remaining quiz
branches/full traversal remain pending.

## All restored quiz answers: native branch coverage

All15 correct question/menu branches now pass native selection and success
dialogue with restored control. Summary: `movement/trick-native-answer-summary.json`;
per-question captures `trick-branch-*-checked`, ROM `df3fe5875aa071fc`. Tests
enter each compiled quiz branch synthetically; they do not prove random question
distribution or full doll-to-doll traversal. Static collision-grid route through
five dolls, scroll and door has104 steps and crosses already-solved trigger rows;
planned paths saved as trick-static-traversal-paths.json for native follow-up.
This is planning evidence, not a traversal pass.

## Full Trick House5 walking traversal

Native traversal now passes from puzzle entrance to TrickHouseEnd:104 planned
walking steps through all five dolls, actual random question choices answered via
menus, already-solved trigger rows recrossed, scroll read and door opened. No
script-pointer injection or progress-variable editing during the walk; only
initial chapter/level/map setup is synthetic. Five completion markers at frames
2524/4628/6936/9106/11642, code12247, end13881. Evidence:
`movement/trick-full-walk-matched`, `trick_full_walk.py`; ROM `df3fe5875aa071fc`.
End-section sheet inspected. Initial matcher used just first question line and
confused Quick/Wide Guard wording at doll5; full-question matching fixed the
harness, not game logic. All15 answer branches additionally have separate native
checks. This completes this repaired puzzle's core success/failure/exit checks,
not the entire Trick House or overall overworld goal.

## Link battle menu/script mismatch repair

Menu-index scan checked238 immediate multichoice switches. Numeric127 candidates
are the documented cancel sentinel. Four real stale switches use five-choice
routing against four-choice MULTI_BATTLE_MODE: Double/Multi/Info/Exit selected
Single/Double/Multi/Info respectively. Corrected Hoenn wired/wireless and FRLG
counterpart switches to0 Double,1 Multi,2 Info,3 Exit; cancel unchanged.
Build/diff check pass. Eight live ROM case encodings verified against exact ELF
target addresses in link-menu-compiled-routing.json. FRLG labels absent in this
ELF, so counterpart coverage is source-only. No multiplayer connection attempted.
Evidence: menu-index-scan.json, link-menu-before.json, link-menu-build.log.
Canonical backport applied: True. Native visible menu checks remain pending.

## Link menu visible Info/Exit validation

Wired and wireless compiled menu branches entered synthetically, selected real
Info option2, returned to menu, selected Exit3, showed farewell and restored
field control. Info text includes both advertised formats. No connection/save
flow invoked. Wired sheet inspected. Original expected substring omitted “do”
in “Please do visit again.”, so raw result reports text assertion failure;
independent exact-text/ready/Info checks pass in link-menu-native-verification.json.
Driver assertion corrected for future runs. Evidence: `movement/link-menu-native-
{0,1}`, `link_menu_probe.py`; ROM `9092858f89d00566`. Direct branch start does
not verify reception-desk prerequisites or multiplayer operation.

## Mauville coin menu mismatch repaired

Second changed-count menu: GameCornerCoins shrank4→3 while live script retained
case2 Buy5000. Visible Exit therefore attempted a5000-coin purchase. Labels also
showed1000/10000 while scripts charge500/5000. Restored third5000-coin option and
labels500/5000/50000, matching v1.13/current purchase constants; no price change.
Build passes. Native compiled clerk/menu synthetic entry selects Exit3, gets
cancellation text, preserves6000 and restores control. Menu sheet inspected.
Evidence: `movement/coin-menu-exit`, coin-menu-before.txt, coin-menu-build.log;
ROM `a0caf103ef82c082`. Canonical block-only backport applied: False. Native
purchases and physical reception-counter approach still require checks.

Coin-counter menu block now backported, preserving unrelated menu edits.

Coin menu purchase checks: native options0/1 preserve5500/1000 from6000, matching
500/5000 charges. Option2 GAME_CORNER fixture visibly shows999999→949999 and
0→5000 coins; sheet inspected. Its final query0 is not valid evidence of zero
money because that scenario does not service the normal campaign query path;
raw expected-money assertion fails and is retained. Evidence: `movement/coin-menu-
buy-fixed-{0,1}`, `coin-menu-large-fixed-2`; ROM `a0caf103ef82c082`. Initial
wrapper missing sys import was fixed locally. Main menu block is now applied
and diff check passes; no price or purchase mechanic changed.

## Adjacent price/menu consistency checks

Lilycove vending labels200/300/350 match the Fresh Water/Soda Pop/Lemonade
script cost assignments. Lavaridge milk cases0/1/2 match one/dozen/pass menu.
Native dozen selection gives exactly12 Moomoo Milk and leaves0 from6000 with
control restored; evidence `movement/moomoo-dozen`, milk_menu_probe.py, ROM
`a0caf103ef82c082`. Synthetic script entry bypasses physical seller interaction.
Source matches canonical. No further price/menu mismatch confirmed in this batch.

## Oceanic Museum choreography

West/east Stern approaches pass full native scene with fixture-assisted battle
wins: grunt entry/reposition, Archie entry and return, fade departures, Devon
Parts handoff, delivered flag1/Stern hidden/control restored. East final sheet
inspected. Source matches canonical. Evidence: `movement/oceanic-museum-
{west,east}-complete`, extended_museum.py; ROM `a0caf103ef82c082`. Short17800-frame
west/north captures ended in legitimate Archie farewell; extended28000 window
used. South tile(13,7) is blocked by exhibit; initial rejected setup is not a
scene failure. North full-run result in corresponding complete directory.
Battle tactics and loss/retry outcomes are not validated here.

## Oceanic Museum loss/refusal branches

Fixture-assisted first-grunt loss returns to field with Devon Parts1, delivered0,
Stern hide0 and control restored. Evidence: `movement/oceanic-museum-loss`.
Both entrance-counter refusals return player one tile to row8, preserve6000,
leave admission state0 and release control. Evidence:
`movement/oceanic-entry-refusal-{9,10}`; source matches canonical, ROM
`a0caf103ef82c082`. This does not yet prove a complete loss→walk-back→victory
chain, nor loss against Archie after defeating the first grunt. No fix warranted.

## Museum no-money story-access exception

Native zero-money admission tests pass before/after Devon delivery: before,
attendant admits player (state1) to avoid blocking the story; after, refuses
(state0) and pushes player back. Both restore control and preserve0 money.
Evidence: `movement/museum-poor-{story,later}`, museum_poor_probe.py; ROM
`a0caf103ef82c082`. Synthetic money seed verifies existing499 against encryption
key before setting0; reproduction requires the work-only driver, not bare input
replay. No product-code change warranted.

## Museum familiar grunt reward/departure

South/west/north approaches pass native gift, direction-dependent escape,
receipt flag1/hide flag1 and control restored. South sheet inspected; detour
avoids player standing in direct south exit path. Scripts wait for grunt
movement before removal. Evidence: `movement/museum-familiar-{south,west,north}`,
ROM `a0caf103ef82c082`. Full-Bag refusal is source-visible before receipt/hide
but not exercised in these runs. No game-code fix warranted.

## Slateport Stern interview/harbor escort

Native interview from west passes Gabby/Ty departure, Stern reposition, crowd
reactions, Stern/player harbor entry and city state2/control restored. Contact
sheet inspected. Evidence: `movement/stern-interview-escort`, ROM
`a0caf103ef82c082`; source matches canonical. Combined with earlier harbor
submarine scene probes, this adds the preceding outdoor handoff but is not an
earned campaign traversal or proof of every approach/re-entry branch. No fix
warranted in this continuation.

## Scott museum-door approaches

Both actual museum exit warps pass Scott's scene: lane9 player(30,27)/Scott
(29,27), lane10 player(31,27)/Scott(30,27) during conversation. Scott departs,
outside-museum state2 and control restored. Revised progress-following dialogue
observed without obsolete Match Call registration promise. Lane10 sheet inspected.
Evidence: `movement/scott-museum-exit-{9,10}`, ROM `a0caf103ef82c082`. Starts seed
post-delivery prerequisites; actual doorway transition is exercised. No new fix.

## Petalburg Woods rescue choreography

Both trigger lanes pass Devon approach/flee-to-player, grunt battle return and
escape, reward and researcher departure. State1/employee hidden/pending0/control
restored. Right-side final sheet inspected. Full-Bag variant also completes
departure/progression with pending1, preserving deferred reward instead of blocking
the scene. Nurse source grants the matching three Dusk Balls and clears pending
only after success. Evidence: `movement/woods-rescue-{252,253,full}`, ROM
`a0caf103ef82c082`; fixture-assisted wins, canonical scripts match. Deferred
collection itself not retested here. No new source correction required.

## Woods deferred reward at Center

Native nurse interaction twice delivers exactly3 Dusk Balls and clears pending
when space exists; repeated interaction does not duplicate. Full-Bag variant
keeps pending1/balls0 after two interactions and restores control. Evidence:
`movement/woods-center-{reward,full}`, ROM `a0caf103ef82c082`; source matches
canonical. Starts seed pending state separately, so this complements Woods
full-Bag departure rather than claiming one continuous earned rescue→Center run.
No code correction needed.

## Route103 early rival departure

South/east approaches pass native battle-return/ledge departure, rival hide1,
Birch lab state4 and restored control on ROM `a0caf103ef82c082`; battles use
fixture wins. Evidence: `movement/route103-rival-{south,east}`; canonical script
matches. Route102 local dialogue/signs/source reviewed without confirmed
incoherence; battle trainer texts and every Route103 approach are not covered
by this narrow pass. No source change warranted.

## Route103 postgame facing repair

Remaining early north/west approaches pass, completing all four directions.
Postgame visits skipped pre-battle face-player movement; native south interaction
left rival facing east4. Added faceplayer in both postgame branches. Rebuilt
ROM `e4f90838c930c4a6`: May/Brendan each face south1 toward player, verified from
native actor state. Evidence: route103-postgame-facing-before, route103-postgame-
{facing,brendan}-fixed; before image route103-before.inc. Canonical backport:
True. Eon Ticket repeat/full-Bag branches still need separate checks.

## Route103 Eon Ticket gift/repeat

May and Brendan each pass gift plus repeat interaction: exactly1 Eon Ticket,
Southern Island enable1, control restored on ROM `e4f90838c930c4a6`. Harbor
source requires both ticket and enable flag, matching this handoff. Evidence:
`movement/eon-ticket-{may,brendan}`. Full-pocket probe using fixture94 is invalid
for this gift: FillHeadlessKeyPocket includes Eon Ticket (exclusions cover other
keys), so gifting stacked another and enabled travel. Do not count it as refusal
coverage or a game bug; custom full-pocket setup excluding ticket is needed.
No new product change in this continuation.

Eon Ticket full-pocket test gap closed: custom work-only setup replaces the
fixture's existing Eon Ticket slot with excluded Devon Scope, preserving occupied
slot/quantity and validating pocket layout. Repeated gift attempts now correctly
leave ticket0/unlock0 and restore control. Evidence: `movement/eon-ticket-full-
excluding-ticket`, eon_full_probe.py; ROM `e4f90838c930c4a6`. This corrects fixture
coverage, not game code. Postgame-named dialogue blocks were scanned for missing
local face commands; no additional direct candidates. That label-based scan does
not cover every conditional branch or establish whole-world facing correctness.

## Route104 cottage rival refusal

May/Brendan trigger entry passes player step-back, rival cottage exit, approach,
revised dialogue and battle refusal; route state2/rival visible/control restored.
Brendan sheet inspected. Evidence: `movement/route104-cottage-{may,brendan}`,
ROM `e4f90838c930c4a6`. Synthetic prerequisites; no battle attempted. Refusal
retains rival for retry. Native accepted battle/re-entry still separate follow-ups.
No new source correction warranted.

Route104 accepted cottage battles pass May/Brendan scene return, defeated flag1,
route state2 and control restored. Fixture-assisted wins; no tactical claim.
Completed-state re-entry interaction gives postbattle dialogue without a battle
and restores control. Evidence: `movement/route104-battle-{may,brendan}`,
`route104-completed-revisit`; ROM `e4f90838c930c4a6`. Revisit seeds completion
separately, so no continuous save/reload chain claimed. No code correction needed.

## Route110 Birch approach branches

All four trigger columns7–10 pass native approach/look-around/dialogue/departure,
registration state2/Birch hidden/control restored. Lane10 sheet inspected; revised
dialogue directs research review to Birch's lab. Evidence: `movement/route110-
birch-{7,8,9,10}`, ROM `e4f90838c930c4a6`; source matches canonical. Synthetic
chapter start retains unrelated Aqua blockers, so these runs establish scene
branch behavior rather than canonical post-Slateport world visibility/traversal.
No code change warranted.

## Route110 lost Itemfinder repaired

Native normal/full-pocket comparison reproduced permanent missed gift: rival
left and state1 advanced but Itemfinder0 with full pocket. Added deferred-delivery
explanation on failure. Nurse recovery uses existing Route110 state>=1 and
CheckEmeraldChampionsHandoffItem (Bag+PC), grants only when absent and space
exists; no new entitlement flag required. Build passes. Native Center twice
results in exactly1 Itemfinder; early state0 twice grants0. Evidence:
`movement/route110-rival-{normal,full}`, `itemfinder-center`,
`itemfinder-center-early`, itemfinder-before.json. Fixed ROM
`8d9107f005056625`. Canonical backport applied: True. PC-owned suppression
and newly added failure dialogue need additional native checks.

Itemfinder repair edge checks pass on `8d9107f005056625`: full-pocket rival
scene displays deferred Center message and still completes; repeated full-pocket
Center visits keep finder0/control restored; PC-owned synthetic slot suppresses
delivery (Bag0/PC1) across repeated visits. Evidence: `movement/itemfinder-rival-
message`, `itemfinder-center-full`, `itemfinder-pc`, itemfinder_pc_probe.py.
PC test driver seeds one local test-save slot and verifies via native PC query;
no user save modified. Focused canonical script diff check passes.

## Analogous key-gift source scan

Scanned explicit key-pocket item giveitem sites lacking nearby result checks.
Candidates recorded in movement/key-gift-candidates.json. Rydel's common
ComeBackToSwitchBikes checks delivery before received flag; exchange removes
one same-pocket bike before adding replacement. Norman/GoGoggles use real bundle
capacity prechecks (including separate-pocket gifts), Scope prechecks before
battle. These are not equivalent to the confirmed Itemfinder loss. Old Rod in
Mom's initial shoes scene remains unchecked, but normal full-key-pocket
reachability before that early scene is not established; do not claim a real
softlock or alter it solely from synthetic impossible inventory. Needs bounded
opening inventory/reachability review if revisited. No additional confirmed
defect in this source pass. Aliased item IDs are outside this simple parser.

## Lavaridge Goggles movement/retry

Normal Gym-side handoff passes herb-shop exit, approach, two-item delivery and
bike departure (state2/receipt1); sheet inspected. Full-key-pocket variant stays
pending state3/receipt0 and restores control. Separate pending-state re-entry
spawns rival at intended retry position; interaction grants exactly1 Go-Goggles
and1 Safety Goggles, state2 and control restored. Evidence: movement/goggles-
scene-{64,68}, goggles-retry; ROM `8d9107f005056625`; source matches canonical.
Alternate Center-side scene66 result recorded separately; no earned continuous
full-Bag→free-space chain claimed. No new source fix warranted.

Goggles follow-up: south retry grants both items/state2; completed-state revisit
keeps both rival sprites hidden with control restored. Evidence:
`movement/goggles-retry-south`, `goggles-completed-revisit`, ROM
`8d9107f005056625`.

Verdanturf family source review: Mauville completion unhides Wally/uncle; Flannery
completion hides Wally, matching uncle/aunt departed-dialogue branches. Wanda's
stone receipt is set only after successful delivery; reactions target actor5.
No new inconsistency confirmed. This is source tracing only, not a full native
family-dialogue state matrix.

## Wanda reward/repeat dialogue

Native eligible interaction twice grants exactly1 Gardevoirite/receipt1 and
restores control. Ineligible HM04-unset seeded branch grants0/receipt0. Evidence:
`movement/wanda-{reward,early}-visible`, ROM `8d9107f005056625`; source matches.
Initial probe retained pre-tunnel Wanda hide flag and therefore never interacted;
corrected fixture explicitly unhides her. Shared SetRusturfTunnelOpen in
data/event_scripts.s unhides Wanda/boyfriend at home after their tunnel exit.
The HM04-unset/home-visible combination is a branch probe, not earned chronology.
No new game-code defect confirmed.

## Verdanturf family dialogue state branches

Uncle home/after-Flannery/after-Victory-Road native branches each display their
matching account and restore control. Aunt tunnel-open and Victory-Road branches
also match; later milestone takes priority over tunnel state. Evidence:
`movement/wally-uncle-{home,away,victory}`, `wally-aunt-{tunnel,victory}`, ROM
`8d9107f005056625`. Synthetic flag combinations test branch priority, not earned
chronology. VictoryRoad script sets the shared defeated flag used by family.
No new dialogue defect confirmed.

## Victory Road Wally entrance choreography

Both trigger lanes pass Wally approach, assisted battle return, family milestone
flag1, visible Wally retained and control restored. Re-entry states1/2 correctly
place Wally at(2,24)/(3,24) respectively. Hall-of-Fame source hides this entrance
actor and unhides exit actor; no premature disappearance assumed. Evidence:
`movement/victory-wally-{2,3}`, `victory-wally-reentry-{2,3}`, ROM
`8d9107f005056625`; source matches canonical. Synthetic prerequisites and assisted
wins do not establish battle difficulty or full League/finale flow. No fix needed.

Victory Road Wally assisted-loss branch preserves state0/milestone0/entrance
hide1 and restores field control. Evidence: movement/victory-wally-loss.
League guard first admission from left/right passes player reposition and
simultaneous guard clearance with admitted1/control restored. Evidence:
`movement/league-guards-{left,right}`, ROM `8d9107f005056625`; source matches
canonical. Missing-badge refusal/re-entry are separate checks. No new fix.

## League guard refusal/repeat

Native missing-Badge1 and missing-Badge8 cases retain both guards at(9,2)/(10,2),
admitted0 and restore control. Previous-admission entry moves guards to(8,2)/
(11,2). Repeat interaction after first admission keeps those positions rather
than moving them again. Evidence: movement/league-missing-{1,8}, league-readmit,
league-guard-repeat; ROM `8d9107f005056625`. Middle badge checks source-visible
but not each natively exercised in this batch. No new correction required.

## Elite Four room entry choreography

All four actual hall→room entries perform six upward steps from row13 to7,
advance Elite state0→1/1→2/2→3/3→4 and restore control. Sidney sheet inspected
through door closure. Source matches canonical. Raw outcomes flag mistaken
expected row6; independent position/state assertions pass in elite-entry-
verification.json and recipes now expect7. Evidence: `movement/elite-entry-*`,
ROM `8d9107f005056625`. This is entry choreography, not battle/forward-door
opening/full League progression coverage. No game-code change required.

## Elite room forward-door state/traversal

All four seeded defeated-state room loads open the forward route; native walking
reaches Hall1/2/3/4 respectively and restores control. Sidney uncleared-state
negative probe remains(6,3) in room with control available. Evidence:
`movement/elite-forward-{SidneysRoom,PhoebesRoom,GlaciasRoom,DrakesRoom,locked}`,
ROM `8d9107f005056625`. These test on-load door state and traversal, not the
battle-victory animation that calls the shared opening routine. No source change.

Sidney assisted-win transition now verified in one native scene: victory flag
set, shared forward-door opening routine runs, player walks around Sidney and
through newly opened door into Hall1 with control restored. Evidence:
`movement/sidney-victory-door-walk`, ROM `8d9107f005056625`. Other three Elite
rooms call same routine but their battle transitions are not claimed tested.
Initial warp selected Sidney's occupied tile; validator rejected it and corrected
recipe starts adjacent. No game-code change needed.

Elite Four victory/forward traversal now passes separately for Phoebe, Glacia
and Drake, complementing Sidney: respective defeated flag1, opened door walked
through to correct next hall, control restored. Evidence: `movement/elite-victory-
{PhoebesRoom,GlaciasRoom,DrakesRoom}`, ROM `8d9107f005056625`; fixture-assisted
wins, not tactical validation. WhiteOut source calls ResetEliteFour, which clears
all four defeated flags and resets state0. Native loss/reset remains a follow-up.
No new product-code change warranted.

## Elite Four late-loss reset

Drake assisted-loss with Sidney/Phoebe/Glacia flags initially set clears all four
defeated flags and resets Elite state0, then restores field control. Evidence:
`movement/elite-loss-reset`, ROM `8d9107f005056625`. Fixture respawn is Petalburg
rather than earned League heal location; result proves flag/state reset, not
normal League respawn placement or continuous replay of the full challenge.
No game-code correction warranted.

League respawn gap closed: native lobby transition sets actual League heal
location before late-loss probe. Loss returns to League1F(3,4), matching heal
location table, with all four victory flags0/state0/control restored. Evidence:
`movement/elite-loss-league-respawn`, league_respawn_probe.py; ROM
`8d9107f005056625`. Battle loss remains fixture-assisted; initial earlier wins
are seeded. No user save modified and no complete earned League run claimed.

## Dewford guide stale ace advice corrected

Guide called Hariyama Brawly's strongest Pokémon; current authored E0040 team
explicitly centers Mega Heracross with support protection. Replaced those two
lines with Mega Heracross warning and partner protection. Teams/mechanics and
authored10 Fresh Waters untouched. Build passes; native two conversations show
new advice, receipt1 and exactly10 Fresh Waters, control restored. Evidence:
`movement/dewford-guide-advice`, dewford-guide-before.inc; ROM
`a3ca362c1b548dc7`. Canonical backport applied: False. Remaining guide advice
requires its own team/data comparisons; this is not a whole-Gym pass.

Dewford Mega Heracross guide advice now backported with unchanged-source guard.

## Other Gym guide claim review

Compared guide texts for Mauville/Lavaridge/Petalburg/Fortree/Mossdeep/Sootopolis
against local rules/layouts and authored team descriptors. Lavaridge sun advice
is supported by SUN strategy/Torkoal/Lilligant; local Mart lists all type Gems.
Petalburg room labels, Fortree rotating doors and Sootopolis ice-floor guidance
remain consistent. Mossdeep retains terrain-based battle setup. No additional
concrete stale-ace statement found. This is source/content review, not proof all
Gyms or every team recommendation are correct. Dewford fix now applied and
focused diff check passes.

Gym guide follow-up: Dewford victory state routes to congratulations without
issuing an unclaimed pre-challenge gift (water0/receipt0). Fortree
first-and-repeat interactions grant exactly10 Energy Roots with receipt1 and
restored control. Mauville worker failed before producing a result; not validated. Evidence: `movement/dewford-guide-victory`,
`guide-repeat-{Mauville,Fortree}`, ROM `a3ca362c1b548dc7`. No gift quantities
changed. Full-Bag branches and every guide approach remain outside these probes.

Mauville guide setup resolved: map10×21, guide(7,20) on bottom row; attempted
south tile(7,21) was outside map. West(6,20)/north(7,19) walkable. Corrected
west interaction twice passes receipt1/exactly12 Moomoo Milk/control restored.
Evidence: `movement/guide-repeat-Mauville-side`, ROM `a3ca362c1b548dc7`. No
layout defect or runtime alternate-layout issue; original coordinate assumption
was wrong. No source change needed.

### Map-edge placement and concurrent-movement sweep

Scanned all958 resolved map layouts:4,446 objects,2,695 warps and645 coordinate
triggers. No coordinate trigger is outside its layout. Of16 bounds candidates,
all9 object candidates are explicit FRLG connection clones, with their target
maps present in the corresponding connections. They are not misplaced local
NPCs. The7 Hoenn warp candidates (Battle Dome corridor/pre-battle room and
Slateport/harbor) exactly match Inclement v1.13. Battle Dome uses explicit
scripted destination coordinates; Slateport's legacy off-edge warp pair still
needs a separate reachability classification. No coordinates were changed.
Evidence: `movement/map-event-bounds.json`. This is a bounds check, not proof
that every in-bounds actor has a correct path or collision.

A separate source sweep selected47 consecutive multi-actor movement blocks
followed by `waitmovement 0` and nearby removal/release/warp commands. The engine
waits for the last selected actor, not every actor; these are review candidates,
not47 confirmed bugs. Rechecked Mossdeep's group departure against its existing
native recording: the trailing grunt is still moving when the selected grunt
finishes, but all departing actors are already outside the visible screen.
Opened the last pre-removal frame to confirm this; no visible disappearance
repair justified. Remaining candidates require duration, visibility and control
flow review. Evidence: `movement/concurrent-movement-candidates.json` and
`movement/mossdeep-magma/frame-00049.png`. No new runtime build or source fix
was needed for these classifications.

### Multi-actor movement duration review and motel repeat

Classified16 of the47 movement candidates in
`movement/concurrent-movement-reviewed.json`:15 source duration comparisons
show the selected actor has the equal or longer complete path;1 is the motel
owner tested natively. These comparisons follow shared movement-label tails,
not just instruction counts. They are not collision/traversal certification.
The motel TV trigger ran twice: player returned to9,2 facing right, owner to10,3
facing up, and control resumed. Inspected the contact sheet for both pushes
and returns. Relevant map script matches the isolated ROM source. Evidence:
`movement/motel-tv-repeat` on ROM a3ca362c1b54.

Green's normal Altering Cave scene passes a forced-win native approach/reward/
departure run (30 Bottle Caps, state1, hidden). A separate synthetic trainer-
defeated/reward-pending re-entry executes the real reward branch and also
completes, but visually confirms Green remains facing away during both dialogue
messages. Repair and revised native verification follow. `leaf-full-pocket`
did NOT reproduce full-Bag refusal: the fixture unexpectedly accepted30 Bottle
Caps. Preserve it as a failed prerequisite probe, not refusal coverage or a
game defect. The re-entry setup explicitly sets trainer flag0x500+770 and is
not represented as a continuation of that failed full-pocket probe.

Green reward-facing repair is now backported: at the shared reward label,
apply `Common_Movement_FacePlayer` to Leaf explicitly and wait before dialogue.
Using a named actor matters because this is a coordinate-trigger scene, not an
ordinary talk interaction. Before/after native recordings confirm facing up2
became down1 throughout the gift dialogue. Inspected both sheets; exit still
completes with30 Bottle Caps, state1, hidden actor and returned controls at20,19.
Rebuilt isolated ROM d157254be6d297d8720a4ee9359836b57dba107acb4db9f122f95a6bb9538d29;
source was byte-guarded against concurrent changes during the two-line backport.
Evidence: `movement/leaf-pending-reentry-fixed`. This is synthetic pending-reward
state coverage, not evidence that the preceding full-Bag refusal was reproduced.

Normal approach/forced-win/reward/departure also passes on the repaired ROM
(`movement/leaf-departure-fixed`): player20,18,30caps,state1,hidden1,ready.
This regression assertion is native state evidence; revised re-entry pixels
were the directly inspected visual comparison.

### Continued movement-wait classification and opening control checks

The47-candidate set now has36 classified entries in
`movement/concurrent-movement-reviewed.json`. Added equal-duration turns,
identical paired paths, the longer Scott/rival-mother departures, and the
previously repaired Chansey explicit wait. This remains narrow timing review;
it does not certify every actor path or collision in those maps.

Native `Brendans-moving-in` and `Mays-moving-in` execute each house's real
on-frame introduction from its exact source-script entry coordinate. After
dialogue the player completes the forced step at y7, then an ordinary held UP
advances to y6, ready with intro state4. Inspected both Intro-ends frames.
No stuck player reproduced despite the source waiting for Mom's shorter turn.
The May-house run deliberately uses the same male synthetic player and thus
proves the map branch, not a female opening playthrough. The wrapper skips
portable-save capture during the active script and uses native warp command
for the door tile because the validator rejects a hidden rival template there.
It does not change map collision or gameplay code. ROM d157254be6d297d8.

### Movement-wait candidate sweep closed; facing-lock scan

All47 selected consecutive multi-actor/wait0 blocks now have a scoped
classification in `movement/concurrent-movement-reviewed.json`. This closes
that search pattern only, not all movement or campaign audit coverage. Rechecked
Rusturf's shorter Peeko wait against the original native recording: last
pre-removal frame01586 shows Briney and Peeko already offscreen. Do not extend
the exit merely to make movement lengths equal. Route23's two candidates are
absent from the current Hoenn ELF; they retain source-only classification.

A separate scan parsed1,480 labelled movement blocks ending in step_end and
compared facing-lock/unlock counts. No positive net facing lock found in those
blocks. One negative net occurs in Tucker's entrance dance: an extra unlock
before the later balanced lock/unlock pairs. It is a candidate to interpret,
not a dangling-lock defect. Shared label tails and dynamic movement generation
are outside this regex scan's completeness claim. Evidence:
`movement/facing-lock-candidates.json`.

### Mirage Tower visibility handoff and both fossil collapses

Ran both actual 4F fossil interactions through collection, ceiling crumble,
Route111 warp, falling-player actor, tower disintegration, remaining fossil
sinking and final dialogue. Both native runs pass: chosen fossil1, state2,
Desert Underpass fossil enabled, ready at19,59 facing north; real player visible
and falling actor removed. Inspected both contact sheets and a falling-player
frame. Each recording has4 sampled frames with falling actor34 visible while
real player255 is hidden, confirming the handoff rather than merely a visible
ending. Evidence: `movement/mirage-collapse-{ROOT,CLAW}` and
`movement/mirage-visibility-handoff.json`, ROM d157254be6d297d8. Relevant scripts
and mirage_tower.c match canonical source. No battle assistance or source edit.
These synthetic summit starts do not prove earned tower ascent, refusal, full
Bag or post-collapse re-entry.

The apparent MAP_LITTLEROOT_TOWN argument in Route111 player hide/show is not
a wrong-map defect: dynamic/player local IDs resolve by local ID regardless of
map, as confirmed by GetObjectEventIdByLocalIdAndMap and the native handoff.
Do not rewrite map arguments solely from their appearance.

### Mirage refusal and completed-state re-entry

Both fossil menus were declined twice using B: native runs preserve both fossil
actors, state0, no fossil in Bag and Underpass fossil hidden; controls return.
Evidence: `movement/mirage-refusal-{ROOT,CLAW}`. A separate explicit state2
re-entry test deliberately starts with a stale tower-visible flag: callback
clears visibility, advances state3 and permits two normal UP steps from19,59
to19,57 through the former tower footprint. No falling duplicate or replayed
collapse; real player remains visible. Inspected re-entry contact sheet.
Evidence: `movement/mirage-completed-reentry`, ROM d157254be6d297d8. This uses
a synthetic completed state, not a saved continuation of the prior collapse.
Full-Bag branch remains unverified. No code change required in this batch.

### Remaining fossils and corrected full-pocket prerequisites

Both Desert Underpass chosen-fossil branches give exactly the opposite fossil,
set the object's hide flag and remove its actor. Additional A interaction does
not duplicate the reward. Native `underpass-fossil-{ROOT,CLAW}` pass; Claw pickup
sheet inspected. The chosen fossil is represented by its flag, not an earned
tower traversal or pre-existing inventory item.

Diagnosed the earlier Green full-pocket probe: BOOK_RESEARCH fills
GetItemPocket(ITEM_DEEP_SEA_TOOTH), now POCKET_BATTLE after item categorization;
Bottle Caps/fossils remain POCKET_ITEMS. That probe never filled their pocket.
The work-only corrected fixture copies a valid encrypted Fire Stone slot into
every primary/overflow slot of POCKET_ITEMS, reads each back and asserts the
pocket ID/capacity before recording. This synthetic saturation state exercises
failure handling, not a claim of naturally acquired inventory. No game/core
or fixture source changed.

All4 corrected native full-pocket fossil runs pass: both summit choices retain
both actors and state0, award nothing, and do not start collapse; both Underpass
choices refuse twice, award nothing and keep the pickup visible with hide0.
Inspected the repeated Underpass full-Bag sheet. Evidence:
`movement/{mirage-collapse,underpass-fossil}-full-items-{ROOT,CLAW}` and
`movement/fossil_full_pocket_probe.py`, ROM d157254be6d297d8.

The same corrected Items-pocket saturation also verifies Green's actual
postbattle refusal: no Bottle Caps, state0, hide0, Leaf remains visible at21,17
facing the player at21,18 and controls return. Evidence:
`movement/leaf-verified-full-items` on repaired ROM, with forced trainer win.
This closes the earlier missing full-Bag refusal evidence; deferred re-entry
was separately tested and visually repaired, not replayed from this run's save.

### Cycling Road entrance movement and return gates

Both North/South gatehouses pass native foot refusal twice (player pushed back
to6,4; cycling flag0 and temp gate0), plus Mach Bike and Acro Bike admission
(cycling1/temp1). All4 bicycle return crossings also clear both flags and
return control. Inspected North foot-refusal and South Acro admission sheets;
remaining cases carry state/actor evidence. Source gate scripts match the
isolated build. Evidence: `movement/cycling-{North,South}-{foot,ITEM_MACH_BIKE,
ITEM_ACRO_BIKE}` and `-return` runs, ROM d157254be6d297d8.

Synthetic fixture adds/registers each bike, preserving the adjacent save
halfword, then SELECT invokes ordinary native bike mounting before walking
across the actual trigger. It does not inject player bike flags or skip the
gatekeeper. This covers gatehouse triggers and return crossings, not a timed
Cycling Road challenge or the full outdoor bridge. No game-code fix needed.

### Cycling perfect-score silent gift failure repaired

The perfect-score tier promised Rare Candy then silently ended when delivery
failed; the good-score tier already displayed the full-Bag explanation. Reused
that existing helper for both tiers and renamed GoodRewardNoRoom to RewardNoRoom.
Native before run ended on the promise with candy0/receipt0/state3; revised
full Medicine-pocket run displays the full-Bag message with the same correct
state. Inspected sheet and frame00308 showing that message. Normal delivery
passes candy1/receipt1/state3; already-received setup gives no additional candy
and retains receipt1. All return control with the cyclist facing the player.

The work-only probe starts the actual compiled CyclingChallengeEnd script at
the finish-line location with a synthetic zero-collision/short timer setup.
It runs native score calculation, dialogue and reward branches; it does not
prove an earned perfect ride or timed outdoor traversal. Medicine-pocket
saturation uses verified primary/overflow storage. Evidence:
`movement/cycling-perfect-{full-before,full-fixed,normal-fixed,already-received}`.
Rebuilt ROM d5ed8500b3f587957dd90e65f4b4f8968cefd4c5f8a55ac5e38a94bc0b5ce163.
Byte-guarded backport preserves concurrent source. No rewards/prices changed.

### Gift-failure dialogue sweep and Contest Case reproduction

A broad nearby-branch gift search returned19 candidates; seven Trick House
branches were success checks, nurse failures already use pending-gift text,
and SS Ticket falls back to PC before a proper failure message. Tightened the
pattern to explicit FALSE comparisons: only legacy cable-club Eon distribution
remains without an obvious failure explanation. Its symbol exists in the ELF
but no current Hoenn map/C-source caller was found; kept as unresolved legacy
reachability rather than claimed campaign failure. Search results are snapshots,
not complete control-flow analysis. A separate result-ignored search produced
46 candidates, many protected by earlier space/bundle checks.

Confirmed live Contest receptionist bug with native interaction: full Key
Items pocket causes no Pokeblock Case delivery but receipt flag1, preventing
a later ordinary retry. Before evidence: `movement/contest-case-full-before`
(query case0/receipt1); actor and script are the actual counter interaction,
not direct branch injection. Added explicit giveitem-failure branch to the
existing full-Bag handler before setting receipt. Build/verification follows.

Contest Case repair verified and byte-guarded backported. Full-pocket repeated
interaction now yields case0/receipt0 with full-Bag explanation and ready;
normal first/repeat yields exactly case1/receipt1. Inspected full-pocket sheet.
Evidence: `movement/contest-case-{full-fixed,normal-fixed}`. ROM 47be608dff3e0058ec41573b7d55332a3410f0115c9da339b57443e49e3d0769.
No automatic correction of receipt flags already corrupted in older saves is
implemented or claimed. This change prevents newly losing the gift on failure.

### Existing-save Pokeblock Case recovery

Replaced receipt-only receptionist dispatch with the existing Bag-or-PC
ownership helper. Missing Case clears a stale receipt then attempts delivery;
owned Case restores the receipt without another gift. This supersedes the
prior no-old-save-recovery limitation. No save migration/new state added.
Native repeated interactions pass four synthetic states: old receipt/no Case
recovers exactly1; Bag Case/receipt0 repairs the flag without duplicate; PC
Case/receipt1 leaves Bag0/PC1; old receipt/full Key Items pocket yields
Case0/receipt0 and explanatory refusal twice. All return control.
Evidence: `movement/contest-recovery-{missing,bag,pc,full}`; recipe scripts
document the synthetic stale flag, inventory and PC slot. ROM b17779f5b0f7047f450f23d3139fba40c32b7217ffbd2a3f391cabc9579a4009.
Byte-guarded backport complete; validation did not modify a user save.

### Museum curator north-side reachability candidate closed

Both ordinary side positions15,2 and17,2 were tested with UP then movement
toward the stairs: native collision keeps the player at the initial side tile,
with curator16,2 blocking the stair approach. Inspected west-side sheet.
Source inbound-map search finds the1F stair and the three curator escort warps
as2F entrances; all escort branches remove the1F curator before warping.
RemoveObjectEventByLocalIdAndMap sets its hide flag. Thus an ordinary upstairs
return does not expose a north-side interaction with that curator. The missing
DIR_SOUTH branch is not a supported normal-traversal defect; no extra path added.
This resolves the earlier specific museum stair reachability question.
Evidence: `movement/museum-stair-blocked-{15,17}`, ROM b17779f5b0f7047f;1F script
matches canonical. Invalid externally edited saves/teleports are outside this
reachability conclusion.

### Space Center stair-guard persistent position repaired

Native forced-win interactions from south/west/east produce state2/3/1 and
guard endpoints14,2/14,2/12,2. State2 map re-entry previously restored13,3,
contradicting the rightward movement retained by Inclement v1.13 and placing
the guard back below the stairs. Reused the existing right-position callback
for state2 and deleted the obsolete down-position handler. State1 remains
left; state3 remains right. No battle or authored live movement changed.

Before/after native re-entry pixels inspected. Revised native states1/2/3 all
restore the expected endpoint, permit ordinary UP movement from13,4 to13,2
and return control. Evidence: `movement/space-stair-{south,west,east}`,
`space-stair-reentry-before`, `space-stair-reentry-fixed-{1,2,3}`. Battle wins
are forced; re-entry prerequisites explicitly reconstruct each saved state,
not earned traversal. This is a continuity/stair-approach repair, not a claim
that the old position made all routes upstairs impossible.
Rebuilt ROM e286f539383ff8de32705132a54cba3952e446303b71ed6d8c37296a4e6d1bab;
byte-guarded backport complete.

### Persistent-position sweep and upstairs refusal

Identified46 non-FRLG map scripts containing both movement and persistent
position commands (`movement/persistent-position-maps.json`); this is a work
list, not46 completed reviews. For Space Center2F, starting positions plus
defeat steps match all3 re-entry placements: grunt6 12,2→11,2; grunt5
13,3→13,4; grunt7 14,2→15,2. Steven's house approach/return ends6,5 facing
up, matching its state2 placement. Fossil Maniac's badge-driven relocation
is separate from his facing-only scene; no contradictory movement endpoint.

Executed real Space Center2F refusal from synthetic ambush state1 with the
1F south-approach guard already defeated. It returns through the actual
scripted stairs to1F13,2, guard14,2, player visible and controls ready.
Inspected contact sheet. Evidence: `movement/space-ambush-refusal`, ROM
e286f539383ff8de (full hash in recording);2F script matches canonical source.
No forced battle or player save changes. This validates the retreat interaction
with the recent downstairs placement repair, not the entire upstairs fight.

### Space Center ambush victory, persistence and next encounter

Native `space-ambush-victory` completes the authored two battles and the
non-fighting lookout's retreat (forced wins, not combat validation). State2
and all3 final grunt positions match the source-derived11,2/13,4/15,2.
Inspected final battle/retreat sheet. Separate completed-state re-entry restores
all3 exact positions. The west passage probe stops at12,3 against furniture;
its y5 assertion is invalid, not evidence of a game softlock. The east route
reaches14,5 and triggers Tabitha normally, so the initial ready assertion
fails while an intended encounter is active. Extending that same route through
Tabitha with a forced win returns control at14,5 with ambush state2 unchanged.
Evidence: `space-ambush-completed-{reentry,passage}` retain failed probe details;
`space-ambush-tabitha-passage` passes. ROM e286f539383ff8de. No additional fix.

### Space Center ambush loss/retry coverage in progress

First-fight forced loss returns control after blackout and leaves ambush
state1 (`movement/space-ambush-first-loss`). The synthetic chapter fixture's
heal destination is Petalburg; this is not evidence for the earned campaign's
Mossdeep checkpoint. Later-fight/retry probes explicitly seed only trainer588
as defeated and trainer590 as undefeated before map entry to exercise the
existing skip-first-battle branch. No player save or game source changed.

Later-fight loss and retry probes completed: late loss retains state1; retry
win reaches state2 with all3 exact retreat endpoints and controls ready, so
no double-step drift reproduced. Evidence: `space-ambush-{late-loss,retry-win}`
on the same ROM. The retry recording also exposes a dialogue-order issue to
address next: the already-defeated first grunt still says "I'll go first"
before the trainer-flag branch skips his battle. This is distinct from the
verified movement/blackout results and has not yet been repaired.

## September 22 continuation (Claude)

Pre-session uncommitted tree preserved at `refs/backup/codex-wip-20260922`.
Isolated-build/backport workflow retired: only one task edits the tree now, so
builds and native scenes run from the canonical source.

### Space Center retry dialogue repaired

`BattleThreeMagmaGrunts` printed Grunt5's "Okay, I'll go first!" before the
`goto_if_defeated` skip. Moved the defeat check ahead of the intro. Native
retry (Grunt5 defeated, Grunt7 not) now goes outnumbered → lookout → Grunt7,
state2, controls ready; first-time victory still plays Grunt5's intro and
passes. Scan of all live map/shared scripts for dialogue immediately preceding
a defeated-trainer skip found no other case (two FRLG-only hits, unlinked).
Evidence: `work/claude-20260922/space-ambush-{retry,victory}-fixed`.

### Birch rescue native replay (first full run)

New game from the main menu through intro, naming, truck, Mom and house entry.
Route101 rescue: chase ends Birch/Zigzagoon/Mightyena at (4,13)/(5,13)/(6,13),
player trapped at (10,15); bag pair selection; forced-win battle; Birch steps
to face the player; lab handoff, both nicknames, May directions. Forced loss
shows the two-page retry prompt: YES re-fights and a later win completes; NO
returns the player to (7,15) facing the bag with all three actors reset and
controls free. Added `playbgm MUS_HELP` to ReturnToBag (the post-battle
callback clears saved music, so the reset chase played the route theme).
The audit's save/reload template-position concern is not reachable: continue
restores live object events (`load_save.c:230`) and the trap prevents leaving.
Evidence: `work/claude-20260922/opening/`.

### Story doubles with one usable Pokemon (class fix)

All 68 `trainerbattle_no_intro_double` sites are scripted story fights whose
dialogue has already committed to battle. The native two-mon guard ended the
whole script mid-scene: music leaked (Mt Chimney, Lilycove rival), cutscenes
replayed on stale actors (Seafloor Room9, Magma Hideout 4F), lines contradicted
the refusal (Wally Mauville, Norman) and League rooms softlocked (doors closed,
no escape, no whiteout without a Revive). The macro now always passes
TRAINER_BATTLE_ALLOW_ONE_MON_IN_DOUBLES (folded in the unused `_allow_single`
variant); the third argument was removed from all sites. Ordinary sighted
`trainerbattle_double` trainers keep the native refusal. Native: Space Center
ambush with a one-Pokemon party completes (forced win) and a real unforced
1-vs-2 battle runs turns, loses, whites out and keeps the ambush retryable.
Evidence: `work/claude-20260922/one-mon-{fixture_win,native}`.

### Meteor Falls rival partner had no Pokemon

`multi_2_vs_2` used PARTNER_{MAY,BRENDAN}_{TREECKO,TORCHIC,MUDKIP}_METEOR_FALLS,
but battle_partners.party defined none of them (header warned about it; Codex's
forced-win replays could not expose it). Authored six sections from the
rival's own Route119 sets: final starter + Manectric + Mimikyu. FillPartnerParty
now levels rival partners to the cap like Steven and applies the regional
starter swap (split `ApplyRivalStarterToParty` out of battle_setup.c). Native:
"MAY sent out Sceptile!" beside the player at the fixture cap; May back sprite.
Evidence: `work/claude-20260922/meteor-partner-60`.

### Retired-feature and dialogue repairs (audit-driven, all built)

- Birch (lab, Route101/103) and Wally (Mauville step call) still played the
  Match Call registration fanfare and "Registered ... in the PokeNav" although
  the PokeNav has no Match Call. Removed both registrations and the text stub.
- Meteor Falls Steven: faces the player before the not-yet branch and the
  Bag-full retry; has his own not-yet line instead of shared directions that
  say "seek Steven"; ticket text no longer opens with a dangling "But...".
- Champion room: Codex's added `lockall/faceplayer` ran from the entry frame
  script with the player selected and turned the player away from Wallace;
  removed (native: player faces up at Wallace during the intro).
- Hall of Fame: Wallace and the player now face each other for the finale
  invitation (native: 6,5 facing right / 7,5 facing left).
- Victory Road Wally: finale directions now follow the first win (continue
  script) instead of only on a later talk.
- Birth Island sailor: at the Deoxys stage points inland instead of "take the
  Lilycove ferry". S.S. Tidal sailor: cabin guidance before the Steven stage,
  shared directions after (no more contradictory pair). Native checks pass.
- Voyage directions no longer claim an S.S. Ticket is required (neither ferry
  checks it, matching Inclement).
- Lilycove Harbor: Aurora first-time path checks the finale stage before
  spending FLAG_SHOWN_AURORA_TICKET/spawning the sailor; the multi-ticket menu
  gates Birth Island and exits cleanly when early.
- Route133 nurse: removed PC-login sound and "And... done!" left over from the
  retired teach-Surf favour.
- Mossdeep Gym: removed the Inverse Battle promise and dead flag lines
  (B_FLAG_INVERSE_BATTLE is 0; the authored team uses the normal chart).
- Lilycove Dept 4F: clerks sold nothing (Inclement TM lists emptied); restored
  the evolution-item specialist the floor sign/NPCs describe, without the old
  hardcoded prices (native: stone shop opens, real prices).
- Winona's badge text (obedience "LV 80") now explains Fly; Norman "Mega
  Bracelet" -> Mega Ring; Rustboro guide no longer hints at a Steel-type trade
  that is actually an item-less Fidough.
- Space Center refusal text before defeated-grunt skip (earlier entry).

### Villain-team story scenes (first native replays)

Forced-win native runs (battle outcome fixture-assisted, not tactics):
- Mt Chimney Maxie from south and west: full meteorite speech, battle, Magma
  removal, Archie arrives beside the player, both face each other, Archie
  leaves; FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY and Aqua hide flag set, controls
  return. Maxie's intro is ~8,000 frames; shorter windows end mid-speech.
- Magma Hideout 4F Maxie (west approach): orb effect, Groudon wakes and
  approaches, battle, shake, Groudon/Maxie cleared, awakening flag set.
- Seafloor Cavern Room9 trigger (17,42): Kyogre awakening, Archie/Maxie scene,
  handoff to Route128 with state1 (Route128 departures verified earlier).
- Aqua Hideout B2F Matt (east approach after notice): battle, submarine
  departs and is removed, escape flags set; re-entry triggers no notice and
  the submarine stays gone. A west-side probe used an unreachable dock tile at
  another elevation (no interaction) — invalid probe, not a game defect.
Evidence: `work/claude-20260922/{chimney-*,hideouts,aqua}` and `*-tail.png`.

### Shared scripts, towns and legendary audit repairs (Sep 22, all built)

Confirmed from source, repaired, with native checks where marked:
- Shop lists missing `.align 2`: Petalburg basic Mart and Sootopolis Mart were
  at odd addresses in the current ELF (misaligned u16 reads = wrong stock).
  All 51 pokemart/decoration list labels now carry `.align 2`; nm shows none odd.
- Center clerk and held-item vendor: a full Bag during starter-kit delivery
  ended the whole script, blocking all shopping. Kit delivery is one shared
  subroutine that returns either way (kit stays pending).
- Celebi harvest battle soft-locked: `waitstate` after `dowildbattle` stopped
  the script again after the field resumed it. Removed. Native (script entered
  at HarvestCelebiCheck, Celebi unlocked): battle -> harvest menu -> B -> free.
- Berry Master lost his "Harvest rewards" entry in the Inclement rebase, so
  the Harvest Pouch's "bring these to the Berry Master" could never pay out.
  Restored the three-option menu; Inclement's daily berry gift is unchanged.
  Native: menu opens, Harvest rewards reaches the pouch menu.
- Deoxys rock never shattered: hard-coded vanilla map numbers (58,26) for a map
  now numbered 41. Uses MAP_NUM/MAP_GROUP. Native: triangle vanishes on cue.
  Deoxys also retreats itself (the shared helper hid the triangle instead);
  a KO shows its own finale-resolving text rather than "return to try again".
- Legendary knock-outs: Regirock/Regice/Registeel/Diancie/Heatran/Moltres
  were removed forever (breaking Regigigas/Regidrago/Regieleki/Magearna
  chains and consuming Heatran's Magma Stone); Ho-Oh/Lugia/Mew/Lati set a
  blocker while saying "return to try again" (Ho-Oh stayed visible but inert).
  A KO now retreats exactly like fleeing. Kyogre/Groudon keep their weather
  quest resolution; Deoxys keeps finale resolution. Native (forced KO):
  Regirock retreats with no blocker and is present on re-entry.
- Mewtwo/Articuno/Zapdos/Regigigas skipped the badge gate their Center lead
  advertises; added the gate with a shared "earn more badges" line.
- Heatran "awake" Center hint read a legacy flag; now follows the object's
  hide flag. Moltres catch now sets FLAG_EC_CAUGHT_MOLTRES for its hint.
- Poipole lead claimed a Game Corner prize; it is a native Altering Cave B1F
  resident. Lead moved beside Mewtwo's (Slateport Center).
- Harvest list cursor was one row off; held-item category cursor could store
  the Back row id (8) as a row index. Both fixed.
- Mystic Ticket pending at a Center blocked Navel Rock despite "passage is
  approved"; its ferry entry now honors the entitlement flag like the others.
- Nurse first visit explains all four tools; "refills its one dose" wording.
- An earlier HM pass removed the "nobody can Surf" path when badges/licenses
  alone sufficed. This is historical: the later approved rule again requires
  a capable party member, though not a move slot; see the current HM replay
  section below.
- Harvest help no longer calls berries free (they cost 20; stocked from start).
- Lilycove Museum paintings: viewer restored (contests are live; the comment
  claiming contest.c was gone was false).
- Slateport market stall sold nothing (Inclement TMs removed): field supplies
  restored from the pre-rebase game.
- Trade follow-ups named the wrong species (Fortree Skitty/Type: Null,
  Verdanturf Igglybuff); Route116 "Keystone" -> Mega Ring; Greta promised a
  nonexistent rematch and gated on Lucy's hide flag (true at new game) — now
  checks Lucy/Spenser trainer defeats.
- 200 redundant explicit `waitstate` lines removed (assembler already ignored
  them): ROM byte-identical (sha 1c81735517c3034b), zero build warnings.

Deferred for the user (design decisions, not silently changed):
- Champions Circuit has no entry (Battle Tower lobby reverted to Inclement's
  Tower in the rebase); its legendary rewards and sign text are unreachable.
- Mauville Starter Archive / Genesect Game Corner prize also lost in the
  rebase; Genesect's lead and sign still point there.

### Static lock/release path check

`work/claude-20260922/lockcheck.py` follows every map entry (objects, triggers,
signs, frame scripts) through goto/call/branches/cases and reports paths that
reach `end` while locked, modelling the self-releasing MSGBOX_SIGN/NPC/AUTOCLOSE
boxes. Outside the Battle Frontier/Tent/Pike/Trainer Hill facilities it found
four: Contest Hall and the fan-club "shouldn't be reached" stub (vanilla,
harmless), the porthole (warps), and the Space Center Steven multi-battle
ending, which left the other NPCs frozen until a map reload (field controls
themselves unlock when any script ends, `script.c:283`). Added `releaseall`.

### Rebase-lost features still advertised: restored

Scan of specials no live script calls, traced to their pre-rebase callers
(`13061e9ba2^`). Three legendaries advertised by Center leads/signs had lost
their only source in the Inclement rebase; restored minimally with existing C:
- Arceus: Devon 2F dream researcher now runs TryGiveArceusLegendarySign-
  MasteryReward (8 badges + Sootopolis resolved) on both his fallback and
  Magearna-awake branches. Native: delivered with fanfare; repeat is quiet.
- Meloetta: Dewford Meadow warden (object re-added at 27,12, appended so
  local IDs are unchanged) unlocks the discovery for a Sing user after badge 2
  (old Mega Ring/manor-chapter gates dropped). Native: no-badge, no-Sing,
  song-unlock and repeat branches pass.
- Landorus: Route111 ruins archaeologist (re-added at 15,12) unlocks with a
  Castform after badge 5; reuses the unused FLAG_EC_RESOLVED_LANDORUS as the
  survey marker. Native: not-yet, unlock and repeat branches pass.
- Cable Club: Double/Multi (wired and wireless) now run the Champions link
  party check with its explanation instead of being rejected silently at the
  Colosseum seat; unreachable Single handlers and their texts removed.
Dead code removed: 19 unreferenced EC_Require* prerequisite blocks (223 lines).
Not restored (reward available elsewhere or deliberately replaced): Shoal Fen
trades, Castform survey flight, opening balls, prepared-gift helpers, Tents.

## Sept 22 — concurrent-movement candidates closed (static timing)

`work/claude-20260922/movetime.py` sums each movement script's frames
(walk 16, slow 32, fast 8, delays literal) and flags blocks where an actor
outlasts the one `waitmovement 0` waits for and is then removed, moved or
warped away. 40 of the 47 candidates have no such overrun. The other seven:
Mossdeep grunts (already confirmed offscreen on the native recording), four
Frontier/Tent corridor attendants (≤16 frames over, followed by the warp fade;
vanilla), Rusturf Briney (two tiles longer than Peeko, already offscreen after
nine; identical to Inclement), and the moving-in walk (releases after the
player's own step; vanilla). No repair needed; the review item is closed.

## Sept 22 — Safari timeout closed

Native: chapter-7 boot, Safari South, `FLAG_SYS_SAFARI_MODE` set and the u16
step counter seeded to 3 by aligned read-modify-write. Steps 3→2→1→0; the
third step plays "Ding-dong! Time's up!", exits to Route 121 Safari Zone
Entrance with Safari mode cleared and control restored. The earlier
non-trigger was the probe (value written while not in ordinary field
control), not the step code. Evidence: `work/claude-20260922/safari/`,
`safari_timeout.py`. No code change.

### Space Center retry dialogue confirmed against current source

The live canonical 2F script now checks the first trainer's defeated flag
before its "I'll go first" dialogue; a concurrent source change had already
placed that check correctly when this candidate was revisited. Rebuilt its
current script in the isolated ROM and replayed both paths: first visit
contains the line and forced-loss leaves encounter state1; second-grunt retry
skips the line, finishes state2 and retains all3 NPC endpoints. Evidence:
`movement/space-ambush-{first,retry}-current` on ROM
c7469f51717e6640500cd9a5744db550988617c286af10e73750fcfe9a43dcd7. No new canonical edit was needed for this
line. A bounded search for nearby msgbox-before-goto_if_defeated sequences in
live map scripts found no other direct match (`movement/retry-intro-candidates.json`);
indirect calls and branched dialogue are outside that search.

The isolated checkout had an older three-argument `trainerbattle_no_intro_double`
macro, while current canonical source uses the two-argument form with one-mon
doubles support. For this scene-only test, the isolated script names its
equivalent `trainerbattle_no_intro_double_allow_single` macro. The current
canonical macro and script already agree; the shim was not backported.

## Sept 22 — Chansey chase movement replay

Ran eight native scene segments: Route111 nurse/Chansey escape, Route112 jump/
escape, Jagged Pass east talk and west shove variants, Ashen Woods first and
second chase triggers, and the final interaction both without and with a Heal
Ball. Each returns control with the expected quest state 1→2→3→4→5→6 and
correct actor hide/position. Jagged shove places the player at11,31 without
losing control. Ashen exits land Chansey at6,40 and27,43; both stages remain
visible on their current map and hide only after capture. Final catch consumes
exactly one Heal Ball and removes both Chansey and the temporary ball prop;
no-ball refusal retains stage5 and Chansey.

Inspected contact sheets for Route111, Jagged shove, both Ashen escapes,
no-ball and catch. The early Ashen test at16,31 was rejected by the warp
validator because that tile is collision-marked; the corrected start15,30
walks right into the real16,30 trigger. This was a setup error, not a game
bug. All relevant chase blocks match current canonical source in the isolated
ROM; Route111's unrelated Mirage Tower waits and Jagged's doubles macro spelling
have since changed in canonical source, so these runs do not validate those
other paths. Evidence: `movement/chansey-{route111,route112,jagged-east,
jagged-shove,ashen-first,ashen-second,ashen-no-ball,ashen-catch}` plus
`chansey-ashen-catch-consumption`, ROM
c7469f51717e6640500cd9a5744db550988617c286af10e73750fcfe9a43dcd7. Synthetic stage setup does not establish one
earned continuous four-map walk. No source change was needed in these scenes.

Both nurse return approaches (south and east) run the corresponding departure
paths, set max/current Poké Vial charges to2, hide the nurse and restore
controls. The attempted west start17,102 is a rock collision tile and never
entered a scene; east19,102 is the accessible lateral approach. Screenshots
inspected. Evidence: `movement/chansey-nurse-{south,east}`.

### Route 101 north-exit overlap found and staged

Scanned551 typed coordinate triggers across958 map layouts against their
packed collision bits. Thirty-five collision-marked entries are inherited FRLG
content; the one live Hoenn entry is Route101's west-exit trigger at6,18.
Route101 row18 at x6 is blocked and the same west-exit script has reachable
triggers at6,15–17. Its event entries exactly match Inclement v1.13; no
coordinate change is justified (`movement/coord-trigger-collision.json`).

Native west/south early-story boundary interactions push the player back and
return control (`movement/route101-exit-{west,south}`). The north trigger is
reachable by walking from5,15 via5,13 and6,13 to7,13. Before the fix it pushes
the player south onto Birch's visible Bag: player and Bag both end at7,14.
The duplicate sprite/overlap is visible in the original sheet and actor state.
The isolated fix changes only `Route101_Movement_PreventExitNorth` from walk_down
to walk_left. Replaying the same physical approach now leaves player6,13,
Bag7,14, ready with the warning intact; after sheet inspected. Evidence:
`movement/route101-exit-north-walk-{before,after}`. Revised isolated ROM
365bbf6f32ff5ab7f822b64a81dbbbacdd3be014ab95a94262324f3c1c86ac9c. Current canonical Route101 differs from the
isolated script only in unrelated intro music. The verified one-line movement is now in canonical Route101 source, with an
exact-content guard and `git diff --check`. No canonical build was started while
another task held the shared build lock; the native before/after comparison
remains bound to the isolated ROM.

### Ashen Woods chase path traversed between triggers

After stage4, the packed layout has a16-step walkable route from16,30
to the next chase trigger at6,36. The initial native fixed-input run met
Elmer's trainer sight at15,33; later movement inputs were consumed during
the battle, so its failure did not establish a blocker. A retry with only
Repel still triggered a wild Salandit because the lead was not above the
wild encounter level; that is another test prerequisite failure. With the
existing BOOK_RESEARCH fixture's level80 lead, Repel and a forced trainer
win, the player walks the same actual tile route after Elmer and triggers
Chansey stage5 at6,36. Native final is ready, stage5, hide0. Captures and
the ending sheet were inspected. Evidence: `movement/chansey-ashen-connected-
highlead` (ROM 365bbf6f32ff5ab7f822b64a81dbbbacdd3be014ab95a94262324f3c1c86ac9c); two earlier interrupted
recordings remain `chansey-ashen-connected-{walk,retry,repel}`. This confirms
map path and presentation through the mandatory encounter, with synthetic
party and assisted battle resolution, not earned party combat.

### Follow-on player landing overlap scan

Expanded the direct coordinate-trigger movement scan through at most three
local goto/call labels:199 static trigger/movement branches yielded five
apparent template overlaps (`movement/trigger-player-destination-expanded.json`).
Four are Petalburg's state0 Gym escort landing at15,10, where Wally and his dad
share template coordinates but are hidden at state0; the later native tutorial
uses them after the state advances. The remaining candidate is Route101's
reachable north push onto Birch's Bag, confirmed with the full walk and staged
fix above. A separate direct NPC-talk scan has three adjacency candidates:
Slateport reporters start on the alleged player tile and so cannot be talked
through there, while the Pyramid attendant's climb removes Brandon to0,0 when
he is absent. These are candidate-classification checks, not whole-engine
collision proof; dynamic graphics, movement and deeper control flow require
independent review. Evidence: `movement/talk-player-destination-overlap.json`.

### Mossdeep Gym guide gift and battle advice

The guide's first and repeated native talks deliver exactly10 Revival Herbs
once (receipt flag1), then give only battle advice. A full Medicine pocket
causes two explanatory Bag-full refusals (herbs0/receipt0), leaving the gift
available. If the Gym is already defeated before claiming the gift, the
guide gives postvictory dialogue and no herbs, consistent with a challenge
aid. Screenshots of normal delivery inspected. Evidence:
`movement/mossdeep-guide-{first-repeat,full,victory}` on isolated ROM
365bbf6f32ff5ab7. The guide script block is byte-identical to current
canonical source; unrelated Tate/Liza inverse-battle lines differ.

Compared authored v1.13 guide rewards with current source: Mauville12
Moomoo Milk, Dewford10 Fresh Water, Fortree10 Energy Root, Lavaridge15
Energy Root, Petalburg15 Chilan Berries and Mossdeep10 Revival Herbs retain
their authored item and quantity. Rustboro's separate entrance scene grants
five Fresh Waters; its standing guide only gives advice. Mossdeep's advice
about altering the field remains accurate after removal of the inverse-battle
variant: authored E0400 still leads with Trick Room and brings Psychic Terrain.
This comparison concerns items and text, not native verification of every
Gym gift branch. No gameplay edit from this check.

### Eight Hoenn Gym exterior door placements

Enumerated all eight outdoor Gym warps with nearby NPC templates
(`movement/gym-exterior-door-actors.json`). Rustboro and Fortree have no
nearby actor; Mossdeep has only a distant ordinary NPC. The story-stage
actors near the other doors are Brawly at Dewford, Wally and uncle at Mauville,
rival-bike actor at Lavaridge, Wally/dad at Petalburg, and Maxie/Archie at
Sootopolis. All have explicit hide flags. The Brawly, Wally and Sootopolis
entrance sequences have separate native scene evidence elsewhere in this
ledger; this proximity inventory alone does not prove each flag is correct at
every campaign stage. No coordinate was changed from this source scan.

### Shared NPC template coordinates

Scanned4,448 object templates in current map JSON and found25 non-FRLG
coordinate/elevation groups with more than one template
(`movement/shared-object-template-tiles.json`). These are actor-identity and
visibility candidates, not25 observed overlaps. Several have explicit
mutual exclusion: Rustboro Flat2 switches its Ace and Hiker from the Float
Stone receipt; Sootopolis moves its Expert to30,18 before Wallace can occupy
31,18; Slateport Harbor hides patrons before Archie appears at8,10; and
Lilycove Contest Lobby sets complementary Blend Master/replacement flags on
transition. Seafloor Cavern uses asleep/awake Kyogre copies in sequence.
Frontier Battle Palace's three13,1 templates begin off the action stage and
are handled by its encounter scripts. Other groups, including camera crews,
Elite Four visitors and Secret Base decorations, remain to classify or replay;
no actor placement was changed from this static scan alone.

### Shared-tile handoff follow-up

Meteor Falls has Archie and the second Aqua grunt in the same saved template
tile, but both start hidden and are added for their entrance. Existing native
`movement/meteor-accept-fixed` captures show six frames where their live
coordinates coincide at22,18; inspected frames02028/02031/02033 and the tile
is offscreen during that interval. No visible merge or blocked tile was found.
Route110's walking/bike rival sprites swap with removeobject before addobject;
Fallarbor performs the same swap. Champions Room rival/Birch both begin hidden
and enter at different script moments. This is source and scoped existing-frame
evidence for these template groups, not a whole-game assertion.

Additional shared-template group trace: Seafloor Room9 initializes awake
Kyogre hidden, then removes the sleeping copy before adding the awake copy;
the existing native awakening run reaches the Route128 handoff. Mt. Pyre starts
with Maxie hidden; the Aqua exit hides Archie before the state2 transition
places him at22,6 beside Maxie23,6. Birch's three Johto ball templates start
hidden at6,8 and the lab's Johto setup places them at8/9/10,4 before adding
them. Mossdeep Space Center moves its unflagged old man away from the10,2
grunt spawn when the invasion stage is active. Camera-crew positions and
Secret Base decoration placement still need their own state/re-entry checks.
No live overlap fix follows from these source traces.

### Gabby and Ty Route118 duplicate crew and retired-save gap repaired

Current `GabbyAndTyBeforeInterview` skips retired battle counters2/3 by
advancing 1→4. `GabbyAndTy_EventScript_UpdateLocation` still followed the
old full sequence: case4 showed Route118 pair2 but hid the retired Route111
pair3, leaving Route118 pair1 visible from case1. Both reporter and cameraman
templates share Route118 coordinates33,8 and34,8, so the duplicate crew was
active and could receive interactions at the wrong trainer party. Native before
recording with battleNum4 has hide flags0/0 and actors4,5,7,8 together.

Added one reset of `FLAG_HIDE_ROUTE_118_GABBY_AND_TY_1` at the shared
location update before choosing the current stop. Native after recordings
show battleNum4 has only pair2 (actors7/8) and battleNum1 still has only
pair1 (actors4/5). Simulated the reachable counter path
0→1→4→5→6→7→8→6: exactly one materialized pair remains visible at each
state (`movement/gabby-ty-visibility-transitions.json`).

Historical saves with raw counter2 or3 previously selected retired stops with
no NPC templates, leaving the crew unreachable. `GabbyAndTyGetBattleNum` now
normalizes those values to4 once, using the existing save field and no new
flag. Native old-save probes write the verified battleNum byte at SaveBlock1
+0x2a45 before map entry; both2 and3 produce only the retained Route118 pair2
with old-pair hide1/new-pair hide0 and control ready. Evidence:
`movement/gabby-overlap-{before,after-4,after-1}` and
`movement/gabby-legacy-{2,3}`, isolated ROM
be8c57af2ded063ec4ece98bce6b832fa7e077459b9e62418f3b80f0cac9375a. Initial/after source behavior also built
in an isolated checkout. `data/scripts/gabby_and_ty.inc` and `src/tv.c` have
scoped canonical edits with `git diff --check`; no canonical build was started
while the other task held the shared lock. Other reporter dialogue/battle
branches still require broader audit.

The retained Route118 pair was then talked to from the adjacent tile in a
real native NPC interaction. Battle resolution was fixture-assisted, but the
compiled trainer selection and presentation are observable: Gabby/Ty send
Toxtricity and Kommo-o, matching authored TRAINER_GABBY_AND_TY_5 rather than
the stale second party. The postbattle interview dialogue finishes with player
controls restored and only actors7/8 present. Evidence:
`movement/gabby-retained-battle` on isolated ROM
be8c57af2ded063ec4ece98bce6b832fa7e077459b9e62418f3b80f0cac9375a. This validates trainer routing and scene
continuation, not tactical battle quality or an earned route playthrough.

### Lilycove couple retained frozen movement repaired

A static `lockall`→`end` screen found41 direct-end candidates among700
labelled lockall blocks. Source alone is insufficient: a no-painting Museum2F
sign and curator branch returned with all NPC frozen bits clear in native checks.
The two Lilycove City honeymoon visitors were different. Each began a
FaceOriginalDirection movement immediately before `end`, without waiting or
release. Native memory inspection of the actual `gObjectEvents` array shows
the talked-to speaker remained frozen1 after dialogue while player control
returned. This is a persistent presentation/movement defect (inherited from
Inclement v1.13), not a whole-field input softlock.

Added `waitmovement 0` and `releaseall` to each visitor after the turn. The
same native talks now leave all active NPC frozen bits0, ready, with the
original dialogue and facing. Evidence: `movement/lilycove-lock-{man,woman}-
{before,after}`; work-only probe reads documented ObjectEvent frozen bit at
offset1 for each 0x24-byte event. After isolated ROM
4cda2aed6e44b708756c96b6f73ad0af3c9e8f57f30a2f046828e8184b229449, scoped canonical source updated with
`git diff --check`. The shared build lock still prevents a canonical build.

The direct `lock`-movement scan found one wild-encounter script that waits
through its battle state; no additional short direct NPC lock script matched
this exact pattern. These scans do not prove that branched movement/release
paths are complete. Evidence: `movement/lockall-direct-end-candidates.json`
and `movement/lock-single-unwaited-movement.json`.

An additional immediate `applymovement`→`end` sweep inspected2,291
movement commands and selected four direct cases after this repair
(`movement/applymovement-direct-end.json`). Two are Trainer Hill player-facing
map callbacks; the others schedule initial invisibility for Battle Palace or
Tower room actors. They are setup/callback sequences, not conversations that
lock NPCs for a player return. No script was changed from those four; deeper
branched paths and runtime ownership still need independent coverage.

### Warp destination and scripted landing audit

Checked2,695 map warp records, including1,401 in non-FRLG maps, against
the current map registry and destination warp counts. Forty-five use the
engine's intentional MAP_DYNAMIC/WARP_ID_DYNAMIC return mechanism (Secret
Bases, event caves and link facilities). Every fixed non-FRLG destination
exists with an in-range destination warp index. The scan parses both numeric
and numeric-string indices; evidence `movement/warp-target-integrity-
resolved.json`.

Scanned126 literal coordinate warps in live map/shared scripts; each named
destination exists and its coordinates fall inside the destination layout.
Nine land on a source NPC template coordinate
(`movement/script-warp-{literal-bounds,template-overlap}.json`): house
intro NPCs and Trick Master are hidden at their relevant stages; Tower's
second Teala is made invisible on arrival; Battle Pyramid uses generated
floor actors, so its1,1 template overlap is procedural rather than a fixed
entrance actor. This is static destination integrity, not proof that every
warp is reachable or that all landing collisions resolve visually. No
warp target was changed.

### Fixed-warp return targets

Among1,356 fixed-index non-FRLG map warps,92 target an exit warp that does not
name the original map (`movement/warp-return-nonreciprocal.json`). This is
not automatically a wrong return:57 use a dynamic return map (elevators,
link rooms, event caves),23 are shared Trick House entrance/exit hubs (the
Puzzle5 native full walk was validated earlier), and four Battle Dome exits
use the common lobby/outside door. The remaining examples include Seafloor
Cavern's one-way shortcut, the Aqua Hideout teleport grid, Trainer Hill's
legacy entrance and an unused Lilycove Mart. Source traces found no concrete
return-target correction in this set; actual reachable exits remain a separate
playthrough requirement.

### Mom's Old Rod lost-gift and old-save recovery

A nearby-result gift sweep selected46 candidates across Hoenn scripts. Many
are guarded by earlier item-space checks or bundle checks; Mom's Running
Shoes scene was not. In a full Key Items pocket, native before evidence
finished state4, set both Old Rod and Running Shoes receipts, and sent Mom
inside while Bag Old Rod remained0. This is a lost gift and false receipt,
not a normal new-game capacity claim. Added an Old Rod space preflight before
the gift dialogue. Revised full-pocket native run explains the problem and
retains state3, Rod0, both receipts0 and Mom available; ordinary run still
gives Rod1, sets both receipts and completes state4. Evidence:
`movement/mom-rod-full-{before-long,after}` and `mom-rod-normal-after`.

For pre-fix saves with Old Rod receipt1 but no Rod, Mom's shared house talk
now checks Bag and PC ownership with the existing handoff helper. Native
missing-Rod run gives exactly1 and a repeat reverts to normal Mom dialogue;
PC-owned run gives none (Bag0/PC1); full-pocket recovery remains pending
without creating another false receipt. Sheet inspected for the recovery
text and item pocket placement. Evidence: `movement/mom-old-rod-recovery-
{before-2,after,full}` and `mom-old-rod-pc`, isolated ROM
527a5de644d6039b349f2671318be4853abf389a235b89711e5143971b222422. `data/maps/LittlerootTown/scripts.inc`
and shared `data/scripts/players_house.inc` were edited in canonical source
with scoped checks and `git diff --check`. A canonical build remains pending
while the shared build lock is owned by another task.

### Camera object lifetime sweep

Found17 explicit SpawnCameraObject calls in seven non-FRLG script files
(`movement/camera-create-remove-counts.json`). Sky Pillar, Navel Rock,
Southern Island and Meteor Falls have explicit removal in their scene files.
Sootopolis has two unmatched pan-up calls in Rayquaza approaches; both lead
to warpsootopolislegend/map reload, and earlier native clash/Rayquaza scenes
return control. Battle Pyramid Top's two first-brain speeches lead to the
win/loss lobby warp, while Battle Pike's Queen entrance proceeds to facility
battle exit rather than returning to ordinary movement in that room. These
source traces classify the count imbalance; they do not certify every
battle-abort state or frame-level camera timing. No camera command changed.

### Persistent NPC reposition candidates

Scanned137 literal `setobjectxyperm` moves across non-FRLG map scripts
against original template coordinates. Twenty-seven placements coincide with
a template tile (`movement/reposition-template-overlaps.json`), many for the
actor's own home tile. Focused source checks find the higher-risk shared ones
conditional on story state: Sootopolis moves its Expert/Wallace and Steven/
Archie/Maxie for distinct legendary stages; Slateport clears Team Aqua before
Scott's post-museum appearance; Ashen Woods hides the ball prop until Chansey
is caught; and Mom's TV placement follows moving-box cleanup. The Space Center
stair-guard mismatch was repaired earlier and its three states passed native
re-entry checks. This is a candidate review, not whole-scene proof for all137
moves. No further placement was changed in this pass.

### Trainer Fan Club first greeting

The eight fan-club members have distinct default and alternate table spots.
Exhaustively enumerating all256 near/far position combinations found no
member-member tile overlap (`movement/fanclub-member-position-matrix.json`).
Native synthetic state1 entry runs the first greeting: the little girl and
man approach, deliver their introductions, the girl retreats, state advances
to2 and controls return. Sheet inspected; no visible overlap at the moving
endpoints. Evidence: `movement/fanclub-greeting` on ROM
527a5de644d6039b349f2671318be4853abf389a235b89711e5143971b222422. The early-capture harness records one
pre-warp frame from the fixture's prior map before the fan-club map loads;
that frame is a harness artifact. This check does not claim an earned fan-count
progression or all eight dialogue branches. No source change.

### S.S. Tidal corridor introduction and first cabin return

Ran Scott's first corridor scene from synthetic chapter7/Scott state0. He
approaches, points to the five cabin teams and Steven/Birth Island, exits
alongside the sailor, and is removed with state1/hide flag1; controls return.
Inspected the contact sheet (`movement/tidal-scott-first`). Its early-capture
harness records one pre-warp fixture frame, not an in-game scene glitch.

Walked through corridor warp0 into the first cabin, then stepped toward
Lea/Jed to trigger their intended battle. An initial short input script
tried to return during warp/dialogue and stopped in the room; that was a
timing error in the probe. With a forced win and two south steps after
returning to field control, the player crosses the cabin's exit warp and
returns to corridor4,10, ready. Evidence: `movement/tidal-cabin-1-exit-two-
steps` on isolated ROM
527a5de644d6039b349f2671318be4853abf389a235b89711e5143971b222422. Source maps pair all nine corridor/cabin
warps in range (warp-target scan above), but this native run only covers the
first cabin and assisted Lea/Jed victory, not the other four fights or native
tactics. No door or choreography edit was justified.

The five cabin team positions are connected to their corresponding interior
door tiles in the packed S.S. Tidal Rooms layout. A collision-grid search
reaches an adjacent interaction tile in1 step for Lea/Jed,5 for Colton,
6 for Naomi,4 for Thomas and9 for Micah. This is static geometry (doors/
metatiles), separate from the first-cabin native round trip; it cannot
certify trainer sight, battle results, dialogue or ship timing for the other
four cabins.

### All five S.S. Tidal cabin door/fight/return scenes

Extended native door traversal to all five authored cabin teams on the
source-matched isolated ROM. Each enters through its corresponding corridor
warp, starts its actual trainer encounter, uses a fixture-assisted win,
then exits to the corridor ready with controls. Outcomes were checked from
recording battle frames and final map/position, not command exit codes.
- Lea/Jed: first lower cabin → corridor4,10.
- Colton: upper cabin → corridor4,3; Liepard/Fezandipiti appeared.
- Naomi: upper cabin → corridor7,3; Tsareena/Salazzle appeared.
- Thomas: upper cabin → corridor10,3; Weavile/Armarouge appeared.
- Micah: lower cabin via west aisle → corridor13,10; Jolteon/Lapras appeared.
Evidence: `movement/tidal-five-cabin-choreography-results.json` and its five
recordings; Naomi and Micah contact sheets inspected. The first attempted
upper-door warp from a collision-marked aisle tile and the first Micah straight
line through cabin furniture were probe errors. Upper doors require a second
southward step from the upper walkway, and Micah is reached around the west
aisle. None was a game blocker. Battle wins were forced: this certifies
overworld entry/exit, dialogue routing and return control, not fight balance
or earned five-win campaign progression. No source edit was needed.

### Canonical integrated build and focused native replay

The shared build lock cleared. Acquired it, built the current canonical
`pokeemerald-headless.gba` with the configured DEVKITARM and current
MAP_VERSION/fixture flags, stamped its inputs, and released only this task's
lock. Build exit0; log `movement/integrated-build-20260922.log`. The integrated
ROM SHA-256 is 1a196b998244dc3e43f90dc02d06fbeb32412a14214626f8df997ba0fbeef886.

Five focused native recordings on that exact ROM pass: Route101 north exit
lands clear of Birch's Bag; both Lilycove honeymoon NPCs finish with frozen
bits0; Mom's old-save Rod recovery gives exactly1 without a repeat; and
a legacy Gabby/Ty counter2 restores the retained Route118 pair with the old
pair hidden. Evidence: `movement/{route101-integrated-20260922,lilycove-lock-
man-integrated,lilycove-lock-woman-integrated,mom-old-rod-integrated-20260922,
gabby-legacy-integrated-2}`. These are synthetic scene checks, not earned
campaign traversal or a release verification claim.

### Finale ship cabin battles now wait for post-League Wally

`GetEmeraldChampionsFinaleStage` returns WALLY after Game Clear until
TRAINER_WALLY_VR_2 has been fought. Both ship harbors allow ferry boarding
after Game Clear, while all six S.S. Tidal cabin NPC scripts previously
started their trainer battles unconditionally. Native before evidence with
Game Clear=true/Wally flag0 started Colton's battle and set his defeated flag,
violating the authored League→Wally→five cabins sequence.

Each cabin interaction (Colton, Micah, Thomas, Lea, Jed and Naomi) now checks
the existing finale stage. Before VOYAGE it uses the shared finale direction
text and releases the player; from VOYAGE onward the original trainer script
runs. No ferry travel, team, trainer flag or dialogue reward was changed.
Native after evidence on the integrated canonical ROM: Wally flag0 yields
Victory Road directions, Colton flag0 and no battle frames; Wally flag1
starts Colton's battle, sets his flag1 and returns control. Both have the
same ROM SHA-256 ba3b4d297b9273a47fa7e8abfc922d8a67e9aad3e065975fbbcb121cccdccc04. Initial
before and isolated after probes remain under `movement/tidal-pre-wally-
{before,after}` and `tidal-post-wally-after`; canonical results are
`movement/tidal-{pre,post}-wally-integrated`. The canonical headless build
and input stamp passed (`movement/integrated-build-finale-20260922.log`).
Battle wins were fixture-assisted; these checks prove gate order/dialogue and
trainer routing, not native battle quality.

### S.S. Tidal completion receipt repaired

The sailor already switched from cabin advice to Steven directions when all
five trainer flags were set. That stage switch bypassed
`SSTidalCorridor_EventScript_CheckIfTrainersDefeated`, the only code setting
`FLAG_DEFEATED_SS_TIDAL_TRAINERS`; the completion receipt remained0 even
after the voyage. Native Wally/voyage/Steven dialogues all reached the
expected text; the original Steven-stage recording confirms receipt0.

The sailor's direction branch now sets the receipt for finale stage STEVEN
or later before giving the existing destination text. Older completed saves
normalize when speaking to him, while Wally and unfinished-cabin talks keep
receipt0. Native isolated stage tests cover all three branches. Current
canonical headless build and stamp pass
(`movement/integrated-build-voyage-receipt-20260922.log`); canonical native
Steven-stage replay keeps the same Meteor Falls/Aurora Ticket advice and
returns receipt1 on ROM 703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589. Evidence:
`movement/tidal-sailor-steven{,-after,-integrated}` plus Wally/voyage stage
recordings. This marker has no current gameplay gate outside the sailor
script; the repair makes saved story state consistent, not a new reward.

### Steven's Meteor Falls finale handoff

The Steven chamber layout has a35-step walkable collision-grid path from
its entrance(10,28) to the only adjacent talk tile(19,4); other adjacent
Steven tiles are walls. Native synthetic finale states cover: before the five
cabins, Steven gives a not-yet response and ship directions without a battle
or ticket; at the Steven stage, his face/exclamation and actual battle script
play, an assisted win gives Aurora Ticket1, receipt1 and Birth Island ship
flag1 with control restored; with Steven already defeated but ticket missing,
a repeat grants it without another battle. Both win contact sheets inspected.
A full Key Items pocket at the defeated/retry stage returns a Bag-full message
and keeps ticket/receipt/ship flags0, preserving the same retry path.
Evidence: `movement/steven-cave-{notyet,win,repeat-trainer804,ticket-full}`
on the canonical ROM 703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589. The first repeat probe
used an incorrect trainer ID (456 instead of804), caused an unrelated
blackout and is retained only as failed setup evidence; the corrected
probe passes. These are explicit synthetic trainer flags and fixture wins,
not earned voyage progression or battle tactics. No scene edit was needed.

### Lilycove first Aurora boarding and Birth Island map handoff

The first-time Aurora Ticket branch and repeat menu both check the finale
stage before Birth Island boarding. Native synthetic ticket-owner at STEVEN
stage gets Steven directions at Lilycove Harbor, ticket retained, and shown
flag0 (`movement/birth-ferry-deny`). At DEOXYS stage the first-time ticket
scene shows/records the ticket and reaches Birth Island Harbor. The broad
A-tap probe then talked to the harbor sailor and accepted a return, ending
in Lilycove; its final-map assertion is intentionally failed/too narrow,
but the recording contains the Birth Island arrival and sailor's inland
triangle guidance (`movement/birth-ferry-allow`, frame00330 inspected).

On the same canonical ROM 703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589, native harbor
8,4→exterior15,24 passes with the triangle visible and rock step count0.
The exterior return door needs two south inputs from15,23: the first enters
the doorway tile, the next starts the warp back to Harbor8,2. Evidence:
`movement/birth-island-harbor-inland` and `birth-island-exterior-return-
two-steps`. These segments use synthetic story prerequisites and do not
constitute a continuous earned voyage or triangle-puzzle playthrough. No
source correction was justified by the ferry/map movement checks.

### Birth Island first triangle movement from harbor

The packed exterior layout has a straight11-tile walkable line from the
landing at15,24 to the first triangle interaction at15,13 (stone15,12).
Native from the Harbor crosses the map warp, walks all11 tiles, talks to
the triangle, and completes its first movement to11,14: rock level1, step
counter reset0, triangle visible and player ready at15,13. Inspected the
contact sheet through ship fade, island stairs and rock movement. Evidence:
`movement/birth-island-first-rock-walk`, canonical ROM
703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589. No battle assistance was used. This is a
synthetic harbor start and first puzzle step; later shortest-path inputs,
Deoxys reveal and outcome branches have separate evidence in the story audit
and remain outside this specific walk. No additional source fix.

### Fortree Gym Kecleon gate and elevation

The Gym entrance is on the raised boardwalk. A collision-only path search
suggested a route around the invisible Kecleon, but the native west-side
approach stops on the lower ground at(22,14), below the boardwalk: that
candidate ignored elevation. A direct synthetic warp to(25,9) can enter
the Gym without the Scope, but starts on the already-raised side of the
gate and does not demonstrate an available approach from town. Keep this
distinction when reviewing other apparent blocker bypasses.

Native interaction from the gate's south tile without the Devon Scope says
``Something unseeable is in the way,'' keeps the Kecleon visible in object
state, and returns player control at(25,9). With the Scope, the reveal,
cry and one-step flee complete, both hide/fled flags set, and the player
can step through to(25,7); screenshots show the reveal and clear path.
Evidence: `movement/fortree-gym-{west-bypass,noscope-bypass}` and
`movement/fortree-kecleon-{no-scope,with-scope}` on canonical ROM
703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589.
The west-side route deliberately fails its Gym-reached assertion and is
retained as diagnostic evidence. No source change was warranted.

### Both cable-car station choreography paths

Native Route112→MtChimney and MtChimney→Route112 trips both complete the
attendant's invitation, player boarding, animated cable-car crossing and
opposite-station exit. Each leaves the player at(6,7), attendant back at
(6,6), station state0 and control ready. Inspected both contact sheets;
neither actor remains in the car doorway. Evidence:
`movement/route112-cable-front` and `movement/mtchimney-cable-front` on
canonical ROM 703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589. Synthetic side-talk
probes at(5,6)/(7,6) were rejected at warp setup as nonwalkable tiles, so
the fixed boarding path's front-approach assumption holds on this layout.
These were separate seeded station starts, not one continuous return trip;
the native travel animation and both arrivals passed. No script edit.

### Lilycove rooftop weather guard

At Sootopolis weather state1, native fifth-floor entry positions the woman
at(16,2) in front of the rooftop stair. Walking north stops at(16,3),
talking gives the storm-closure line, and another north input still cannot
pass. At state4, the woman remains at her normal(9,5) spot and walking from
the same start reaches the rooftop with control restored. The guarded scene's
contact sheet shows no actor overlap or facing defect. Evidence:
`movement/department-roof-stage1` and `movement/department-roof-stage4-complete`
on canonical ROM 703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589. An earlier stage4
probe ended during the doorway fade and failed only its ready assertion;
the completed recording waited through that transition. These are seeded
weather states, not an earned legendary-weather sequence. No source edit.

### Route 112 cable-car Magma guard

The two Magma grunts occupy both tiles of the narrow uphill path before the
cable-car station. From(26,31), native talk plays all four exchanges, turns
both actors back to their original facing and restores control; a north
input leaves the player at(26,31). With their hide flag set, the same input
path advances to(26,28). Meteor Falls' Aqua/Magma scene sets this flag after
the exit movements, matching their line about waiting for the other crew.
Evidence: `movement/route112-magma-guard-blocked-complete` and
`movement/route112-magma-guard-cleared`, canonical ROM
703523a450624fbba79c03f4b40a881cdb5b38eb94faa0359fd9bf1577ab7589. The first blocked probe
stopped while the fourth dialogue was still active; the completed recording
uses enough inputs to reach idle. This is a seeded hide-flag branch test,
not a continuous Meteor Falls-to-cable-car traversal. No source edit.

### Fallarbor rival meeting: all five entry rows

Fallarbor has five adjacent story triggers at(13,8) through(13,12), each
with a separate player approach. Native replays entered every row from the
east. All five converge at player(13,10) and rival(12,10) without overlap,
finish the Cozmo/Meteor Falls dialogue and bike departure, set state1,
hide both rival templates and return control. The north and south contact
sheets show the full approach and exit. A separate completed-state re-entry
walked left through the former rival tile to(11,10) with no scene retrigger.
Evidence: `movement/fallarbor-rival-trigger-y{8,9,10,11}-corrected`,
`movement/fallarbor-rival-trigger-y12`, and
`movement/fallarbor-rival-complete-reentry-corrected` on canonical ROM
129a0b9699131267ec2459002aad5fb28726b66f6d7a3cd88f32b93e00fdb4fa.
Initial probes for rows8-11 checked only one entry of an exact query map;
they observed the correct state and flags but failed the recipe assertion.
The corrected replays pass. The first re-entry probe expected two tiles
after a34-frame hold but the player walked three; its corrected assertion
passes. These are synthetic state branches, not an earned Fallarbor visit.
No game-source edit was needed.

### Steven's Mossdeep-house Dive handoff

The house's on-frame state1 scene walks Steven six tiles toward the door,
so I replayed entry through its actual Mossdeep exterior warp instead of
seeding a position inside. The player arrives at(3,7); Steven stops one
tile above at(3,6), delivers the Dive permission/explanation, then returns
to(6,5). Native screenshots show distinct sprites throughout, and the
end state has Dive flag1, house state2, control ready. The return position
matches the state2 persistent position set on re-entry. Evidence:
`movement/steven-house-dive-natural-door`, canonical ROM
129a0b9699131267ec2459002aad5fb28726b66f6d7a3cd88f32b93e00fdb4fa. The story state was
seeded after the Space Center fight; the door entry and handoff ran
natively. No movement edit was warranted.

### Mt. Chimney to Cozmo handoff and Lava Cookie transaction

Both reachable Maxie interaction tiles were replayed natively: west(12,6)
and south(13,7). The fixture-assisted win reaches Archie's matching approach
and departure, hides both factions, marks the summit cleared, unhides Cozmo
at home and enables the Lava Cookie vendor. The two other cardinal neighbors
of Maxie are collision tiles, so these are the live approach branches.
Inspected the west contact sheets and final south frame. Evidence:
`movement/mtchimney-maxie-{west,south}-win-complete`, canonical ROM
129a0b9699131267ec2459002aad5fb28726b66f6d7a3cd88f32b93e00fdb4fa.
The first probes ended during Archie's last dialogue; longer, faster-A
replays reach idle. Battle wins are forced fixtures, not tactics validation.

Cozmo's wife says he is at Meteor Falls before the summit and gives one
Galladite/sets its receipt after he returns. Cozmo accepts one Meteorite,
grants one Star Piece and sets the exchange receipt, with source-matching
thanks after the handover. The exchange contact sheet shows the scientist
and player on separate tiles throughout. Evidence:
`movement/cozmo-house-{wife-before,wife-after,meteorite-exchange}-corrected`
on the same ROM. The initial setup recipes tried to set absent item counts
to zero, which Studio rejects; corrected recipes omit those writes. These
are seeded state branches, not a continuous earned summit-to-house run.

The vendor had a confirmed dialogue/transaction ordering defect. With all
44 Medicine-pocket slots filled, her pre-fix native scene said “Thank you,
dear!” before “The Bag is full,” although cookie0 and money stayed ¥6000.
The script checked space only after the thank-you and attempted the giveitem
even when full. It now checks space first, grants the cookie, deducts ¥200
only after successful delivery, then thanks the player. Rebuilt and stamped
the canonical headless ROM (`movement/integrated-build-cookie-20260923.log`,
SHA-256 6584a607f6b3e6d9b55256610169fa85c4057c127b79cd67684bc8d3df971ce5).
Native full-pocket replay now shows only the Bag-full refusal, cookie0,
money ¥6000; normal replay shows the item receipt, money ¥5800 and then the
thank-you. Both pass and both contact sheets were inspected. Evidence:
`movement/mtchimney-cookie-full-medicine44-{before,after}` and
`movement/mtchimney-cookie-normal-{before-corrected,after}`. The first
vendor probes hit an Aqua grunt sharing the lady's template tile because
the synthetic faction-hide flags were omitted; corrected setups enforce
the actual post-summit visibility state.

The adjacent source scan found30 non-FRLG `checkitemspace` commands; 14 had
a `msgbox` within the preceding seven lines. Route124 and Shoal Cave ask
whether to trade before preflight, Slateport Fan Club branches each preflight
before the scarf announcement, and Lilycove's vending machine preflights a
second free can after the first paid can was delivered. Other hits cross
script labels or concern a refusal/sign. None of these nearby-message hits
has the vendor's unearned thank-you ordering. This narrow pattern scan does
not establish correctness for gifts without `checkitemspace` or for all
other dialogue paths.

### Shoal Cave two-reward craft at the high-tide entrance

A synthetic four-Shoal-Salt/four-Shoal-Shell interaction at the expert's
reachable south tile(18,16) crafts one Shell Bell and gives one Slowbronite,
consumes all eight ingredients, sets the Slowbronite receipt and returns
control. The native high-tide contact sheet shows both reward messages and
separate player/expert sprites on the small dry platform. Evidence:
`movement/shoal-expert-bell-slowbronite-items`, canonical ROM
6584a607f6b3e6d9b55256610169fa85c4057c127b79cd67684bc8d3df971ce5. An initial recipe
used a wall tile west of the expert; a second mistakenly assumed a different
fixture's parameter19 stocked the ingredients. The corrected replay supplies
the ingredients explicitly. This verifies the dialogue/reward branch, not
a continuous tide-dependent approach or full-Bag variant. No source edit.

### Late-finale native choreography and Buffel visual identity

A backward source pass checked Buffel's motel gate, Birth Island outcome and
return states, Steven/Aurora boarding, all five S.S. Tidal cabin scripts,
Victory Road Wally, League admission and first-clear Hall flags. It found
no further source-proven blocker; detailed per-file limits are in
`work/overworld-presentation-20260920/backwards-agent-notes.md`.

Native source-matched scene replays on ROM
6584a607f6b3e6d9b55256610169fa85c4057c127b79cd67684bc8d3df971ce5 passed for post-League Wally's
north-exit encounter, Buffel's No/decline and fixture-assisted win, the
first-clear Hall of Fame walk/record/Wallace invitation into credits, and
the full Birth Island triangle path through Deoxys descent and a fixture-
assisted capture. Wally's trainer flag, Buffel's trainer flag, Game Clear/
Champion/Wally visibility, and Deoxys resolution/rock hide all reached the
expected states. The Hall recording remains in credits (`ready=false` is
expected there); it does not establish postcredits control. I inspected its
contact sheet and the Birth Island reveal frame. Evidence:
`movement/backwards-{wally-exit-win,buffel-decline,buffel-win,
hof-full-entrance,birth-triangle}`. These are separate synthetic starts,
not a continuous earned finale or battle-quality validation. The current
whole-tree stamp later drifted as concurrent source changed, but the agent
verified the six scene-source files against Studio's same-ROM source archive.

The Buffel replay exposed a visual identity mismatch: his Inclement-derived
motel actor is the young `OBJ_EVENT_GFX_MAN_3`, while the authored current
trainer data used the white-haired `TRAINER_PIC_EXPERT_M`. Inclement Emerald's
original Buffel uses `MAN_3` and `TRAINER_PIC_COOLTRAINER_M`; the current
pre-integration battle data had already switched to Expert. Changed only
`Pic:` in `src/data/trainers.party` to Cooltrainer M, retaining the authored
class, team, AI, music and overworld actor. The generated trainer header
contains the new picture ID. After an initial stamp refusal caused by an
unrelated source edit landing after the ROM build, a serialized rebuild and
input stamp passed (`movement/integrated-build-buffel-portrait-retry-20260923.log`,
ROM ed7914c872801a50d75ac68062ae5417c9442fcd9fc5ad6a7406eb632a7f2cd9).
Replayed Buffel from the prior portable scene save against that ROM:
`movement/buffel-portrait-after` passes, retains the win/return-control
behavior, and the contact sheet now shows a younger portrait consistent
with the overworld cameo. This is an Inclement visual-identity restoration,
not a battle-plan change.

### Route 131 Herman trainer/overworld identity

The direct actor-to-battle comparison also found Herman on Route131: his
clothed `OBJ_EVENT_GFX_MAN_3` stands on the small island, but the authored
trainer data displayed a shirtless Swimmer M and Swimmer encounter music.
The current six-mon doubles plan is a non-water Trick Room crew. Inclement
Emerald uses the same MAN_3 actor with Cooltrainer class, Cooltrainer M
portrait and Cool encounter music; two other MAN_3/Dragon Tamer cases were
already present in Inclement and were left alone. Restored Herman's three
presentation fields in `src/data/trainers.party`; his six Pokémon, items,
moves, AI, level offsets, doubles format and prize multiplier are unchanged.

The pre-fix native island encounter shows the Swimmer portrait and label
beside the MAN_3 actor (`movement/route131-herman-portrait-before`, ROM
ed7914c872801a50d75ac68062ae5417c9442fcd9fc5ad6a7406eb632a7f2cd9). A serialized rebuild and
input stamp passed (`movement/integrated-build-herman-portrait-20260923.log`,
ROM 8c88815fed27740a0322cbf434354dbf8da748d9f753613d00f7c17009a1c9a2). Fresh native
replay `movement/route131-herman-portrait-after-fresh` shows Cooltrainer
Herman and the Cooltrainer M portrait at the same encounter tile, with the
same Bronzong/Gourgeist lead. The first attempt to replay from the prior
recording's portable save was rejected because the auto-sight battle began
during the warp; the fresh scene is the validated after evidence. These
short scenes show the intro, not a battle outcome.
The authored-team generator `--check`, campaign roster verifier and focused
diff-whitespace check passed after both portrait edits.

A bounded metadata scan of317 direct actor trainerbattle bindings found no
heuristic male/female actor-versus-portrait mismatch. In53 non-FRLG species
actor scripts with a direct `playmoncry`, the graphics species matched the
cry species; this excludes cries in called/shared scripts and does not prove
all sprite allocations, facing metadata or trainer portraits correct.

### Backward Sootopolis and Sky Pillar follow-up

On preserved ROM 6584a607f6b3e6d9b55256610169fa85c4057c127b79cd67684bc8d3df971ce5, native post-crisis
Sky Pillar replays cover Rayquaza knockout/retreat with no persistent
capture flag, fixture-assisted capture with its flag set, and seeded
re-entry after retreat that restores the still Rayquaza actor. A south-side
approach shows the player and Rayquaza separately before battle. Cave of
Origin B1F Wallace's wrong Rayquaza answer preserves city state2 and his
departure flag0; the Sky Pillar answer removes him, sets departure flag1,
advances state3 and restores control. Evidence:
`movement/backwards-rayquaza-{defeat,south,win,reentry}` and
`movement/backwards-cave-wallace-{wrong,correct}`. The wrong-answer replay
stops during its response rather than proving a full second menu cycle.
These are synthetic branches, with forced wild outcomes where stated. The
agent checked the relevant scene files against that ROM's archived inputs;
it is not a claim that the older ROM matches every later shared-tree edit.
No new game-source defect was found in this scoped replay.

### Lavaridge Gym one-way geysers repaired

The Inclement rebase made the final two 1F geyser destinations reciprocal:
warp23 at(14,6) fell to B1F warp21 at(14,6), and warp25 at(12,12) fell to
B1F warp23 at(12,12). The pre-rebase authored map instead sent warp23 to
B1F warp23, the isolated chamber below Flannery, and warp25 to B1F warp0
near the entrance. The map binary and warp source tiles are unchanged from
Inclement, so restoring those two destination IDs does not redraw the Gym.
The imported pairing removed the one-way entrance to Flannery's chamber;
before repair, native descent through warp23 landed at B1F(14,6)
(`movement/lavaridge-final-geyser-before`). It is a map-graph/progression
bug, not an invalid warp index. A generic check that every destination ID
exists or that warps are reciprocal would miss it.

Restored `dest_warp_id`23 on 1F warp23 and `dest_warp_id`0 on 1F warp25,
matching the authored pre-rebase one-way route. The rebuilt/stamped canonical
ROM `d87fd89a95488a48944f52a56a30c49f14f0fd09b2aa5a3603e2909c1464e1e3` passes the direct
final-geyser replay: fall to B1F(12,12), step off and back onto its active
geyser, rise into 1F(13,12), then walk to Flannery at(13,10) with control
ready (`movement/lavaridge-final-geyser-after-held`). The complete native
entrance route also passes on the repaired map: from 1F(13,17), through the
active geysers around the buried trainers, to 1F(13,10), with no overlap
or stuck movement (`movement/lavaridge-gym-full-path-unblocked`). Its contact
sheet was inspected. Trainer flags were synthetically set as defeated to
isolate path movement; this is not a battle or earned badge run.

An exit replay enters warp25 from its north tile(12,11), lands at B1F(10,18),
rises near the Gym door and exits to Lavaridge Town(5,16), controls ready
(`movement/lavaridge-gym-final-to-town-door`, preserved ROM
`b7452996bfcf5964cff588b3111f9d97cec5003eefb402bae92bfed009a42440`). Approaching
the exit warp from its east side at(13,12) is blocked by tile elevation;
the north approach is the working route. Earlier full-route probes used
short one-frame presses, treated every warp event as an active geyser, or
walked through a buried trainer's occupied tile; those failures were probe
models/inputs, not additional game defects. Active geysers are identified by
their metatile behavior, and B1F-to-1F geysers auto-jump the player one tile
east on arrival. The final entrance and exit replays account for both rules.

Follow-up on the same regression class: a semantic comparison of current
same-layout map warps against the pre-rebase authored checkout found24 other
changed destinations, all Secret Base exits changing the symbolic warp ID
from `WARP_ID_SECRET_BASE` to `WARP_ID_DYNAMIC`. `EnterSecretBase` stores the
return map in `dynamicWarp`, and `SetWarpDestinationToDynamicWarp` ignores
that symbolic ID argument and uses the stored map, so these24 changes do
not have the Lavaridge one-way-route effect. This scan excludes maps whose
layout/source warp tile changed and is not whole-world reachability proof.

### Backward coastal/Dive and Route 117–123 handoff

The backward worker reached the Route117/Mauville/Verdanturf boundary of the
main Fallarbor/Lavaridge pass. It source-reviewed Route123→117 and nearby
handoffs without duplicating prior Gabby/Ty, Kecleon, Weather Institute,
Steven or Wally replays. On preserved ROM
6584a607f6b3e6d9b55256610169fa85c4057c127b79cd67684bc8d3df971ce5, scoped native results pass
for the Berry Master's first gift and same-day repeat, the Day Care woman's
Egg decline/accept/repeat, and the outdoor Day Care man's pending(47,6)
versus clear(47,4) placement. Both Route125 storm-only Dive pools lead through
their distinct21-step undersea Marine Cave corridors and back; Route124 and
Route126 Dive/emerge and the underwater Sootopolis connection also pass on
that artifact. Evidence: `movement/backwards-route123-berrymaster`,
`backwards-route117-{egg-decline,egg-accept,man-pending,man-clear}`,
`backwards-route125-{west,east}-full-return`, and the other Dive recordings
named in `work/overworld-presentation-20260920/backwards-agent-notes.md`.
The preserved route/map sources match that ROM's archive; later field-move
engine changes mean these older Dive recordings are artifact-specific.

Route118's girl originally claimed a Pokémon must know Surf. An interim
correction said no Pokémon was needed, but the later approved HM rule in the
current `AGENTS.md` and `FieldMove_GetUserSlot` requires a party member able
to learn the move, without a move-slot requirement. The final Route118 line
now says to bring a Pokémon able to learn Surf, clarifies that it needn't
know the move, and tells the player to face water and press A. Wally's
father's Surf authorization speech was also restored to that rule. The
interim `movement/route118-surf-advice-after` recording on ROM
bc768491b7bbb269c42c0724f54f021c7c7bd35a71df481cdc8d97c07daa3290 is historical and superseded.
The serialized build after this text correction passed
(`movement/integrated-build-surf-guidance-20260923.log`, ROM
8268d062ec3729f2bcb24fc8ef1ccf6a4d728daaa3f4dfe7af37218081b58be4), though its whole-tree
stamp refused because `wild_encounters.json` changed after the ROM build.
The two scripts, field-move code and shared field-move dialogue match this
ROM's Studio source archive byte-for-byte. Native `movement/route118-surf-
advice-current-rule` and `wally-father-surf-current-rule` pass with the
final wording, Surf receipt1/city state5 and control returned; both sheets
were inspected. On the same ROM, Route126 Dive/emerge succeeds with a
compatible party member and refuses a Caterpie-only party with the matching
``able to learn Dive'' message (`movement/route126-dive-{compatible,
incompatible}-current`). These probes establish the current HM message
and a representative Dive gate, not every HM or campaign party state.
A repository-wide targeted text search for `knows/learns/teach` with the eight
field HMs found no other live Hoenn instruction requiring a move slot;
other able-to-learn guidance now matches the current capability rule. This
is a wording-pattern scan, not proof of every NPC's dialogue semantics.

On the earlier stamped ROM bc768491b7bbb269c42c0724f54f021c7c7bd35a71df481cdc8d97c07daa3290,
`movement/route126-dive-current-engine` passed a surface→underwater→surface
round trip at(10,30); that evidence binds to the older field-move rule.
The repaired Lavaridge Gym
entrance-to-Flannery and final-room-to-town replays also pass unchanged on
that integrated ROM (`movement/lavaridge-gym-full-path-integrated` and
`lavaridge-gym-final-to-town-integrated`). These are seeded scene tests,
not a continuous earned coastal/campaign run.

### Gym guides and the Flannery-to-Petalburg reveal

Compared every Hoenn Gym guide gift against the local Inclement source: the
same guides give Dewford10 Fresh Waters, Mauville12 Moomoo Milks,
Lavaridge15 Energy Roots, Petalburg15 Chilan Berries, Fortree10 Energy
Roots and Mossdeep10 Revival Herbs. Rustboro's five Fresh Waters come from
its separate entrance scene; its standing guide gives advice only.
Sootopolis's guide gives advice only. These gifts are inherited Inclement
behavior, not an accidental Champions dialogue addition.

Native Lavaridge guide first/repeat talks grant exactly15 Energy Roots once
and then advice; with all44 Medicine slots filled by other items, two talks
give the Bag-full refusal, roots0 and receipt0. Evidence:
`movement/lavaridge-guide-{normal,full}` on ROM
083e9f906ab7afc69c1494a967ed092aab30652cce861343d9854d54b6c677f4. The normal and
full-pocket contact sheets were inspected. Petalburg's guide is hidden at
new game, but the shared `Common_EventScript_ReadyPetalburgGymForBattle`
clears that flag when the fourth Gym advances Petalburg state5→6. Native
fixture-assisted Flannery victory proves guide hidden0, Badge4 set, state6,
Lavaridge rival state1 and Heatranite delivered (`movement/flannery-unhides-
petalburg-guide`). At synthetic pre-Norman state6, the guide gives15 Chilan
Berries exactly once and then describes the room doors; a defeated-Norman
state gives congratulations without a late gift. Evidence:
`movement/petalburg-guide-{stage6,victory}`. The Petalburg guide sheet was
inspected. The relevant Gym scripts/map JSON and shared event script match
the ROM's changed-source archive byte-for-byte. Wins are fixture-assisted,
and these are separate scene starts, not an earned four-Badge run.

The Berry pocket holds68 distinct Berry item types, including Chilan, so a
normal distinct-slot full pocket with Chilan absent is not constructible.
A later native stack-boundary replay supplies989 Chilan Berries: only10
fit in the first Berry slot, fewer than the guide's15, so two talks
show Bag-full without adding any Berry or setting the receipt.
With984 already held, the full15 fit exactly: one talk reaches999 and
sets the receipt, then repeat dialogue gives no extras. Both scenes
return control and their contact sheets were inspected
(`movement/petalburg-guide-chilan-{short-by-five,exact-fit}-valid`,
ROM `5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`).
The first setup requested more than the999-per-slot limit and was
rejected before gameplay; corrected quantities are the executed tests.

The named `FLAG_HIDE_ROUTE_111_DYNAMO_GUARD` and
`FLAG_HIDE_SLATEPORT_CITY_KNUCKLE_GUARD` currently have no map object or
script consumer; only their old constant declarations remain. Those
names do not describe live NPC blockers in this checkout. This
source check is narrower than walking every route boundary and does
not assert that all physical story gates are traversable.

### Petalburg Gym first doors and trainer handoff

At pre-Norman state6, the first two doors are sign interactions on collision
tiles, not walk-through warps. Native Speed-door Yes reaches its first room
at(7,85); Accuracy-door Yes reaches(1,98), both with control ready and their
labels displayed (`movement/petalburg-gym-{speed,accuracy}-door-interaction`).
Walking north into the Speed-door tile alone correctly stays at(1,106).
Randall's Speed Room battle, fixture-assisted, sets his trainer flag and
returns control; the next sign interaction enters the Confusion Room at
(7,46), demonstrating the postbattle door callback and room transition
(`movement/petalburg-speed-trainer-next-door-settled`). Its contact sheet
shows the battle, return to the room and next-door crossing. An earlier
probe stopped at the side wall, and another pressed Up while the avatar
was still finishing a step; the settled-input replay passes. Source pairs
each of the seven room trainers with the same unlock routine both in its
victory callback and in `OnLoad` for re-entry. This first batch covers one
native trainer and two entrance doors; the follow-up below extends it.
The replay used a forced win solely to test presentation and door state.

### Petalburg branch through Norman and Surf

The Speed→Confusion→Strength→leader branch now has native door/battle
handoffs beyond Randall. A fixture-assisted Parker win sets his trainer
flag and opens the Strength-room sign, whose Yes choice reaches(1,20).
A fixture-assisted Jody win opens the leader-room sign, whose Yes choice
reaches(1,7); all scenes return control, and the Jody door contact sheets
were inspected. Evidence: `movement/petalburg-parker-strength-door` and
`movement/petalburg-jody-norman-door`. Source pairs these victory callbacks
with their corresponding `OnLoad` defeated-trainer unlocks. These are
separate seeded room starts, so they establish each handoff, not one
uninterrupted walk through the three trainer battles. The Accuracy-side
branches and other room trainers remain for full native traversal.

Native Norman victory was replayed from the leader room with the Keystone
gift pre-owned and a fixture-assisted battle. It awards Badge5, sets Gym
state7, begins Wally's father's Gym exit, crosses Petalburg City to his
house and grants Surf authorization, ending with city state5 and control
returned. The ordinary18,000-frame Studio recipe stopped mid-father
dialogue with Surf0; a work-only copy of the scene runner with a30,000-frame
limit completed the same inputs (`movement/norman-victory-surf-escort-long`).
Its Gym/house contact sheet and original outdoor frames around13980,14100,
14250 show father and player on distinct tiles, following the street and
entering the house. The first recording's mid-dialogue state was a probe
limit, not a game lock. This validates the seeded victory and escort flow,
not an earned three-room battle run, Keystone full-Bag retry or Norman's
battle tactics. The full-run ROM `083e9f906ab7afc69c1494a967ed092aab30652cce861343d9854d54b6c677f4`
predates the later Surf wording correction; a separate current-ROM father
scene above verifies the final text and Surf receipt.

### Petalburg Accuracy-side room handoffs

The remaining four room trainers have native fixture-assisted victory and
next-door replays on ROM
8268d062ec3729f2bcb24fc8ef1ccf6a4d728daaa3f4dfe7af37218081b58be4. Mary opens both the
Defense path to(7,59) and Recovery path to(1,72). Alexia opens Strength
to(7,20) and OHKO to(1,33). George opens OHKO to(7,33), and Berke opens
Norman's room to(7,7). Each replay sets the corresponding trainer flag,
shows the matching door label/choice and returns control after the warp:
`movement/petalburg-{mary-defense,mary-recovery,alexia-strength,
alexia-ohko,george-ohko,berke-leader}-door`. Mary, George and Berke contact
sheets were inspected for actor and door choreography. Alongside the earlier
Randall/Parker/Jody branch, all seven Gym trainer callbacks now have scoped
native checks. The current Petalburg script matches this ROM's Studio source
archive; the whole-tree stamp drifted due unrelated shared edits. These are
separate seeded room starts with forced wins, not an uninterrupted earned
Gym traversal or battle-quality test. Return-door/re-entry persistence is
supported by the seven matching `OnLoad` flag checks in source but has not
been replayed for every room.

### Fortree Gym first rotating-gate movement and re-entry reset

Native movement from the east side of the gate centered at(12,22) turns
gate6 once without trapping the player: its packed orientation word changes
from `0x0200` to `0x0201`, control returns at(11,22), and the contact sheet
shows the arm rotating beside the player. A separate seeded entry with the
rotated word restores the map's initial `0x0200` orientation before player
movement. Evidence: `movement/fortree-gym-first-gate` and
`movement/fortree-gym-gate-reentry-reset`, ROM
8268d062ec3729f2bcb24fc8ef1ccf6a4d728daaa3f4dfe7af37218081b58be4. These checks bind to the
first of eight configured gates and its reset behavior; they do not prove
the entire rotating-door route to Winona. No source change was warranted.

### Fortree Gym complete route, Winona reward, and capacity retry

The Fortree gate state model found a 97-step route from the entrance-side
(16,23) to (14,2) beside Winona, accounting for all eight gate shapes and
orientations, walls and occupied actor tiles. The full sequence was then
walked natively on ROM
`8268d062ec3729f2bcb24fc8ef1ccf6a4d728daaa3f4dfe7af37218081b58be4`:
the door arms rotate, the player reaches Winona without a stuck state, and
control returns (`movement/fortree-gym-complete-solver-path`). Its two contact
sheets were inspected. The setup marks six ordinary Gym trainers defeated, so
this proves physical gate reachability, not their battle results or an earned
entrance-to-leader playthrough. Fortree's map layout is unchanged from the
base revision, and the current gate code and Gym script match the preserved
ROM's changed-source archive.

A separate fixture-assisted Winona victory on the same ROM visibly delivers
the Feather Badge and Staraptite, with native readbacks Badge6=1, Gym
defeated=1, legacy `FLAG_RECEIVED_TM51`=1, Staraptite count1 and field
control returned (`movement/fortree-winona-badge-staraptite-correct-facing`).
The trainer battle was forced solely to observe the postbattle sequence.
The first attempt to test capacity by filling an Items stack was invalid:
`GetItemPocket` routes Mega Stones to the separate Mega pocket and the Bag
supports split stacks. A corrected synthetic capacity boundary fills all101
Mega slots with 65,535 Venusaurite and34,965 Charizardite X. Winona still
awards Badge6, displays the full-Bag message and returns control, while
Staraptite remains0 and its receipt flag remains0
(`movement/fortree-winona-staraptite-full-mega-pocket`, ROM
`050a59f5c2a87882aeb16d01f9f9328877d469cb6a27bd9d774198b8de2f6623`).
The retry branch was then replayed from a separate synthetic defeated-
trainer/Badge6 state with room for the item. Talking to Winona gives one
Staraptite, sets the receipt, keeps Badge6, faces the player and returns
control (`movement/fortree-winona-staraptite-retry-run`, ROM
`15b701cec71e7f9c3dbe16c1e39cc00ad8ede610f30386444b2971b816809215`).
Its contact sheet was inspected. This verifies the retry interaction,
not one earned full-Bag→free-slot save chain. The relevant Gym script and
item code match their respective ROM archives; whole-tree input stamps
have since drifted under concurrent edits.

### Rotating-tile threshold defect and moving-statue checks

`MoveRotatingTileObjects` previously used the Mossdeep Gym metatile start
as its lower bound even when operating in Trick House Puzzle7. Trick House
tiles `0x291`-`0x297` lie below its own start `0x298`; dividing their negative
offset by eight truncates to zero, so a template on one of those tiles could
be misread as a yellow puzzle tile. This is a latent wrong-object movement
class, not a currently reproduced stuck NPC: the present Puzzle7 objects do
not start on those seven tiles. The guard now compares against the selected
map's `puzzleTileStart` in `src/rotating_tile_puzzle.c`, with no new state or
special-case code. A source-matched headless ROM build passed and was archived
as `a02168a9079b1a9b9f39afd5a77f8b438bc59f0e600787e5c0b6e3ff5d75bc3d`.

Native Puzzle7 yellow switch and green switch replays on that ROM return
control (`movement/trick-house-puzzle7-{yellow-switch-after,green-statue-after}`).
The green switch visibly shifts its statue from (4,6) to (3,6), preserving
the intended movement and facing. Mossdeep Gym's yellow switch moved and
turned its nearby statues with control returned both before and after the
one-line guard repair (`movement/mossdeep-gym-yellow-switch{,-after}`). All
four contact sheets were inspected. These are switch-scoped checks; a full
Mossdeep Gym and Puzzle7 traversal remains open.

### Backward early-game pass reached the Petalburg boundary

The backward worker continued from Route117 through Route110, Slateport and
Route109, Dewford, Rustboro, and Route104, stopping at the Petalburg Gym
boundary. It read live object bindings, callback and gift branches, and
avoided replaying already covered major scenes. Source-matched native
scenes passed for Dewford Eviolite first/repeat, Seashore House six Soda
Pops and shop decline, Route109 Soft Sand first/repeat, Roxanne's Rustboro
state8→9 departure, and Route104 White Herb first/repeat. The inspected
contact sheets and exact scene names are in
`work/overworld-presentation-20260920/backwards-agent-notes.md`. No new
source-proven defect was found in this bounded pass. Optional full-pocket
retries for those gifts and a continuous earned early-game route are still
unplayed.

### Mossdeep Gym complete route and postbattle story handoff

The Mossdeep Gym collision grid alone made the leader platform appear
unreachable. A work-only state model added its five moving-statue color
cycles and same-map warp pairs, then found a 116-step route from the
entrance at (6,34) to Tate and Liza at (23,8). Every step was walked in
the native game on ROM
`a02168a9079b1a9b9f39afd5a77f8b438bc59f0e600787e5c0b6e3ff5d75bc3d`
(`movement/mossdeep-gym-full-route-model`). It crosses five teleports,
operates the purple, green, blue and red switches, moves the statues and
trainers without overlap, and returns control beside the leaders with
Badge7 still unset. All three contact sheets were inspected. The synthetic
start marks the twelve ordinary Gym trainers defeated to isolate the
movement puzzle; it is not an earned trainer-battle run. The work-only
scene runner sets those trainer flags without changing game code or the
canonical Studio tool. The Gym script, tile-movement engine and relevant
field routing match this ROM's changed-source archive.

A separate fixture-assisted Tate and Liza victory gives the Mind Badge
and Meowsticite and advances Mossdeep city and Space Center state to1;
the exterior and Space Center Magma hide flags clear, and field control
returns (`movement/mossdeep-tate-liza-badge-space-center`). The battle
was forced only for postbattle choreography. With all101 Mega pocket slots
synthetically filled, the same victory still grants the badge and story
handoff but displays the full-Bag message, keeps Meowsticite count0 and
leaves its receipt flag unset (`movement/mossdeep-tate-liza-full-mega-pocket`).
A separate seeded postvictory revisit with a free pocket delivers the
owed Meowsticite, sets the receipt and returns control without restarting
the battle (`movement/mossdeep-tate-liza-gift-retry`). The victory and
retry sheets were inspected. These three starts establish the relevant
branches, not a continuous save in which the player manually frees a slot
or wins the leaders' battle without assistance.

### Sootopolis Gym ice path, fall recovery, and Juan handoff

The three first-floor ice regions contain 7, 19, and 38 thin-ice tiles.
A route that visits each tile once was walked natively from the Gym entry
(8,22) through all three stair openings to Juan at (8,3), with no basement
fall or stuck control. The final `VAR_ICE_STEP_COUNT` is68 (initial1,
64 tiles and three stair unlock increments), and Badge8 is still unset.
Evidence: `movement/sootopolis-gym-clean-ice-route-verified`, ROM
`a02168a9079b1a9b9f39afd5a77f8b438bc59f0e600787e5c0b6e3ff5d75bc3d`.
Its contact sheet was inspected for cracked ice, each opening, Juan's
position, and returned control. The first recipe reached the same endpoint
but had an incomplete expected-query dictionary; the corrected replay
passes. This is a synthetic post-crisis start, not an earned Gym entry.

An intentional repeat step breaks the first ice tile and drops the
player to basement (8,19). With basement trainers already defeated to
isolate movement, the player walks to the ladder and reappears on 1F at
(11,22), visible and controllable. The new entry resets the ice count to1
and its row19 visited mask to0; the sheet shows fresh first-floor ice.
Evidence: `movement/sootopolis-gym-fall-ladder-reset-corrected`, ROM
`d1363f5647e2a59648fe355fa5dd507b959861faf9fba2208289beb971ab19c2`.

The first fall probe triggered an ordinary basement trainer and stopped
during its greeting; that was not a control lock. Ten defeated-trainer
prerequisites let the recovery route be tested independently. The basement
trainer battles remain unplayed in this sequence.

A fixture-assisted Juan victory grants Badge8 and Feraligite, sets the
Gym-defeated flag and Sootopolis state6, changes the residents/return-NPC
visibility flags, and returns control (`movement/sootopolis-juan-badge-fearligite`).
With a synthetically full101-slot Mega pocket, the same victory keeps
the badge and story state while refusing Feraligite without setting its
receipt (`movement/sootopolis-juan-full-mega-pocket`). A separate seeded
postvictory revisit with space available delivers the owed gift and sets
the receipt (`movement/sootopolis-juan-gift-retry`). Juan's postvictory
branch with Badge6 missing directs the player to Fortree rather than the
League (`movement/sootopolis-juan-missing-fortree-badge`). All four sheets
were inspected; they are separate synthetic starts, not one save with
manual Bag management or an unforced Juan battle. The Gym scripts and ice
step code match the archived ROM sources used by these scenes.

### Mauville Gym barriers, Wattson victory, and Wally-aware exit

From the Gym entry (4,19), a native route walks around the defeated but
still occupying trainer Vivian, presses floor switches1,2,3 in order, and
reaches Wattson at (5,3) with controls ready. The switch state is3 and
the alternate barrier flag is set (`movement/mauville-gym-switch-route-to-wattson-avoid-vivian`).
An earlier route probe tried to walk through Vivian's occupied (1,16)
tile, never pressed switch1, and ended on the wrong side of the room;
that was a path-model error, not a blocked authored route. The corrected
contact sheet shows the barriers changing and no actor overlap.

A single fixture-assisted replay then walks the route, defeats Wattson,
receives Badge3 and Raichunite X, sees the barriers deactivate, walks
back through the room and exits to Mauville City with controls returned
(`movement/mauville-gym-victory-town-exit-coherent`). The five Gym trainer
receipts were seeded by the existing story fixture, and the Wattson win
was forced for presentation; the route is not an earned battle run. The
first city-exit probe omitted Wally's prior departure and placed Wally
on the exit landing. Source tracing shows `removeobject` sets Wally's
persistent hide flag when his earlier battle ends. A replay with the
Wally and uncle hide flags and battle receipt set before Gym entry shows
neither actor on the landing. The apparent overlap belonged to an
inconsistent synthetic setup, not a game-code defect. Its end sheet
was inspected.

Wattson's full101-slot Mega pocket branch still awards Badge3, displays
the full-Bag message and leaves Raichunite X count0 and its receipt unset
(`movement/mauville-wattson-full-mega-pocket`). A separate seeded
postvictory visit with space available grants the owed stone and sets the
receipt (`movement/mauville-wattson-gift-retry`). Both sheets were
inspected. These are separate capacity/retry scenes. The Mauville Gym
script, switch engine and actor map bindings match ROM
`d1363f5647e2a59648fe355fa5dd507b959861faf9fba2208289beb971ab19c2`.

### Dewford Gym dark-room route, light growth, and Brawly reward

A 37-step entrance-to-Brawly route was walked natively from (5,26) to
(4,4), respecting all visible trainer and wall occupancy. Six ordinary
Gym trainers were seeded defeated so the scene isolates physical room
reachability; the spotlight and actors render throughout, and controls
return at Brawly (`movement/dewford-gym-full-route-to-brawly`). A
separate fixture-assisted Takao victory starts in the dark, sets his
trainer receipt, increments the Gym light count to1 and returns control
as the visible circle grows (`movement/dewford-takao-light-expansion-verified`).
The first Takao probe completed its battle but failed during a query
because the Studio catalogue lacks the `VAR_0x8001` alias; the corrected
probe queries its numeric native ID and passes. Both sheets were
inspected.

Combining the complete room walk with a fixture-assisted Brawly win
grants Badge2 and Emboarite, advances Petalburg Gym state3→4, hides the
Slateport Aqua group, removes the dark-room spotlight and returns
control (`movement/dewford-gym-route-brawly-victory`). With the Mega
pocket synthetically full, the Badge and story state still advance but
Emboarite remains absent and its receipt flag remains clear
(`movement/dewford-brawly-full-mega-pocket`). A separate postvictory
revisit with space grants the owed stone and sets the receipt without
another battle (`movement/dewford-brawly-gift-retry`). All three sheets
were inspected. These use separate synthetic starts, and the leader and
Takao wins were forced solely to check presentation. The Dewford Gym
script and map match ROM
`d1363f5647e2a59648fe355fa5dd507b959861faf9fba2208289beb971ab19c2`.

### Rustboro Gym trainer maze and Roxanne gift retry

A 23-step native walk from the post-guide entry (5,18) reaches Roxanne
at (5,3) around the three still-occupying defeated trainers, with no
collision trap or actor overlap (`movement/rustboro-gym-room-to-roxanne`).
The three trainer receipts and the guide-completed state were seeded to
isolate this physical route; the guide's four entrance approaches and
five-Fresh-Water retry already have separate native coverage. A single
fixture-assisted replay then walks the route, defeats Roxanne, awards
Badge1 and Delphoxite, advances Rustboro state1→2 and Petalburg Gym
state2→3, and returns control (`movement/rustboro-gym-route-roxanne-victory`).
The two route/victory sheets were inspected. The forced battle establishes
postbattle presentation only.

With the Mega pocket synthetically full, Roxanne still grants Badge1
and the state transitions, but refuses Delphoxite and leaves its receipt
unset (`movement/rustboro-roxanne-full-mega-pocket`). A separate seeded
postvictory revisit with space delivers Delphoxite, sets the receipt
and returns control without another battle
(`movement/rustboro-roxanne-gift-retry`). Their sheets were inspected.
This does not represent a continuous save with manual Bag management.
The Gym script and actor layout match ROM
`d1363f5647e2a59648fe355fa5dd507b959861faf9fba2208289beb971ab19c2`.

### Route 133 nurse and Chansey crossing

The earlier Chansey chase pass did not cover the separate Route133 nurse
and Chansey pair. In the original native acceptance scene, the nurse said
"Chansey made it across" while both actors still stood next to the player;
they disappeared only afterward in a fade. This order was inherited from
Inclement. The repaired scene now has Chansey walk six tiles along the
current first, then the nurse follow six tiles, before the success and
Poké Vial upgrade dialogue. Its advice refers to the visible path rather
than claiming the stationary player led them. The final source-matched
native replay shows the two actors at the far edge before the success
line, then a clean fade/removal, maximum and current Vial charges3,
their hide flag set, and control returned
(`movement/route133-nurse-chansey-cross-final`). The player remains on
the bank; this is a local crossing scene, not a full Route133 traversal.

At Vial capacity3 with a stale clear hide flag, the old transition left
the nurse and Chansey visible again. Route133 now derives their visibility
on entry: only capacity2 shows them; capacity1 and completed capacity3
hide them. Native before/after re-entry is
`movement/route133-stale-visible-before` and
`movement/route133-stale-hidden-final`. A direct talk to Chansey makes it
jump and cry without changing progress
(`movement/route133-chansey-direct-talk`); declining the nurse leaves
both actors and capacity2 in place (`movement/route133-nurse-refuse-final`).
The acceptance, refusal, direct-talk and before/after sheets were
inspected. The final scene ROM is
`9d35ad0660fd719d477861e55815b640e8936b9bcce50324b5b8db4e391ad1ee`;
the same Route133 source matches the later integrated ROM below.

### Nurse rocks, mountain shove, and old-save flag collisions

The two breakable rocks at Route111 (18,101) and (19,100) were mapped
to persistent bits0x3A/0x3B. Before the Inclement rebase, those exact
bits recorded Cover/Helix Fossil pickups in Sandstrewn Ruins, while the
Route111 rocks used temporary flags. A save that had found both fossils
therefore hid two untouched rocks. This was reproduced natively with the
old fossil receipt flags set
(`movement/route111-legacy-fossil-rock-collision-before`). The rocks now
use unused persistent bits0x4C6/0x91F; old fossil receipts stay on their
original bits. The new flag IDs preserve Inclement's persistent-smash
behavior and leave the daily flag boundary unchanged. On the corrected
map entry both old receipts remain set but both rocks render
(`movement/route111-legacy-fossil-rocks-visible-after`). Native Rock Smash
removes either rock independently and both in sequence, setting only
their new hide flags (`movement/route111-both-rocks-sequential-after`).

An older Rock Smash license script also set bit0x34B to hide a retired
Route111 tip NPC. The Inclement nurse reused0x34B, making her disappear
before the Chansey quest on such saves. The current Route111 transition
now shows her while the Poké Vial has fewer than two maximum charges and
hides her after the Route111 upgrade. Before/after scenes are
`movement/route111-legacy-rock-smash-hides-nurse-before` and
`movement/route111-legacy-rock-smash-nurse-visible-after`; a capacity2
re-entry keeps her hidden. The integrated legacy-save replay shows nurse,
Chansey and both rocks at stage0, then the normal escape, one Heal Ball,
stage1 and returned control while retaining the old fossil receipts
(`movement/route111-legacy-save-chansey-escape-after`). All sheets were
inspected. A north-side conversation after smashing the left rock sends
the nurse south and upgrades the Vial without trapping the player
(`movement/chansey-nurse-north-broken-rock-after`). The initial escape
also passes with both rocks present, only the right rock removed, and
both rocks removed; these are bounded synthetic starts, not an earned
continuous chase.

The Jagged Pass map allows talking to Chansey from its east or west side;
the north and south adjacent tiles are collision-marked. From the west,
the script shoves the player down to (11,31) while Chansey escapes left;
from the east it runs left without touching the player. Both initial
native branches finished stage3 and returned control with Chansey hidden
(`movement/chansey-jagged-{shove,east}-current`). A closer frame inspection
found the west-side problem the player suspected: while both movement
scripts started together, Chansey crossed the player's original tile and
vanished behind the player sprite around frame204. The player now starts
the dodge16 frames before Chansey's run. In the corrected native frames
120-150 the sprites stay distinct, the player still lands at (11,31),
Chansey leaves, stage3 persists, and control returns. The east branch
still completes normally (`movement/chansey-jagged-{shove-staggered,
east-staggered-build}`), with contact sheets and close frames inspected.

The same alias scan found a Fallarbor rival bug: the walking actor used
RIVAL1 at old Dome Fossil receipt bit0x3C instead of the authored rival
hide flag0x4DC. With a legacy Dome Fossil receipt, the first Fallarbor
trigger played dialogue from an invisible rival
(`movement/fallarbor-legacy-dome-trigger-before`). The walking template
now uses the authored flag and `LOCALID_FALLARBOR_RIVAL`=5; the bike copy
has `LOCALID_FALLARBOR_RIVAL_ON_BIKE`=6. The local ID numbers, graphics,
movement and story state are unchanged. Under the same legacy receipt,
the rival is visible on entry, walks through the conversation, swaps to
the bike actor, departs, and advances Fallarbor state0→1 with control
returned (`movement/fallarbor-legacy-dome-trigger-after`). Its before/
after sheets were inspected.

The earlier integrated headless ROM
`ce8e035b8db4dc076571bacd5d3789de0cb772ea9f245c64084c9bafe0eed388`
builds and its Studio archive matches the Route111, Route133,
Fallarbor and flag sources from this pass. The
static progression verifier passes540 maps,3,996 events and1,401 warps;
it does not prove state-dependent reachability. Because the old fossil
bits cannot reveal whether a post-rebase player also smashed these rocks,
such a player may see either rock reappear once after this flag move.
They remain smashable with the existing Rock Smash license and Badge.
The targeted scan covered159 legacy-bit alias definitions for actor/rock
binding risks; optional item aliases and full earned-save traversal remain
outside this pass.

The newer headless ROM
`cc607814fb627361322189ade2fc3e183d67cc882f8b28bd3cefea8bacc156ff`
contains the Jagged timing correction and a Route110 sprite-metadata fix:
the walking rival actor23 had been named `LOCALID_ROUTE110_RIVAL_ON_BIKE`,
while the actual bike actor24 had no local ID. Their map JSON identifiers
now agree with the already-correct numeric script targets and graphics
slots. This changes no scene state or movement; the generated local-ID
header resolves23/24 correctly. The ROM archive matches both edited
source files, the build and scoped diff check pass, and the native Jagged
west/east replays above pass. This metadata sweep covered the dynamic
story-rival slots on Route110 and neighboring route/town scenes; it does
not establish that every dynamic sprite in inherited facilities is sound.

### Route 110 Dowsing Machine dialogue and rival sprite labels

The Route110 gift's actual Bag name and native obtain message say
"Dowsing Machine," but the rival still called it an "Itemfinder." The
deferred Pokémon Center handoff and field-use messages used that stale
name too. Both rival variants, the full-pocket explanation, Center
recovery, field-use prompts and Mauville storyteller now say Dowsing
Machine; internal item IDs and function names are unchanged. Current
native Route110 gift and Center recovery scenes pass with the correct
name, one item delivered, the normal walking-to-bike actor swap and
control returned (`movement/route110-rival-dowsing-after` and
`movement/dowsing-center-after`). Their contact sheets were inspected;
the Center recovery scene talks to the nurse twice and the item is
delivered once. The C field-use strings were build- and width-checked,
but their on-screen item-use interaction was not replayed here. All
changed message lines measure below the216-pixel dialogue-box limit.
The headless ROM for these checks is
`8f42a2b40d6a8e33428fd609e3dde30d4e08f1750c38b42f37f407401430845d`;
its archived source matches all six changed game files in this follow-up.

### Dynamic Lati overworld graphics and inaccessible old minigame room

On Southern Island, `VAR_ROAMER_POKEMON` selects the *other* Eon twin for
the static encounter. Native runs with value0 and value1 visibly render
Latios and Latias respectively, pan the camera, transition into the
matching species battle and return after fixture-assisted capture with
the caught flag set (`movement/southern-island-{latios,latias}-dynamic-run-2`).
Both contact sheets were inspected. A separate forced knockout of
Latios shows the retreat fade and message, leaves caught/defeated
flags clear, removes the live actor, and returns control
(`movement/southern-island-latios-defeat-validated`). The first
defeat probe's expectation omitted the extra defeated query; the
corrected expectation passes without a game edit. These helpers
establish overworld identity and scripted outcomes, not native combat
or a continuous exit/re-entry retry. The first fixture starts at the sign's
occupied template tile and was correctly rejected by Studio; walking up from
the adjacent free tile is the executed path. ROM
`9341630848c6097c79e3d5908056eedb9002f692bfc35fb9fe3b809089af4fa2`.

Mossdeep Game Corner's old basement has a VAR_0 sprite template with no
map-entry assignment, but its only first-floor destination warp is at
(2,0), a collision-marked tile behind collision-marked (2,1–3) wall
tiles. No current map script opens those tiles; only the separate
counter at(5,2–3) changes for wireless play. Studio rejected a warp to
the blocked entrance. The Sootopolis mystery basement's dynamic
template likewise never has an ordinary viewing interval: its entry
frame immediately restores the party and warps back upstairs. These
are source/geometry classifications, not a claim that all dynamic
facility sprites are covered.

### Current dialogue-width flags triaged

The current glyph-width scan reports11 literal-line warnings. Eight are
debug text, one is the FRLG-only Seagallop, and the shared Aide warning
is referenced only from FRLG map scripts. The remaining Trainer Hill
timer line is measured using the checker's conservative 10-character
placeholder width; its actual three timer buffers are each restricted
to two digits in `src/trainer_hill.c`. The reported 70 oversized boxes
are largely intentional `\\l` scrolling pages (or debug font tests),
not literal horizontal overflow. This triage found no current Hoenn
line needing a text edit from those warnings. The verifier's pass/fail
count is not an exhaustive native dialogue-rendering check.

### Littleroot Mom's Running Shoes and Old Rod handoff

Replayed all six map-coordinate triggers at town state3 with the male
player, plus north, side and south representative triggers with the
female player. Mom approaches from the correct house, gives exactly one
Old Rod and the Running Shoes, returns home, advances town state3→4,
and leaves field controls ready. Native readbacks show Old Rod1,
`FLAG_SYS_B_DASH`1 and Mom hidden1 in every run. The north, far
south, and side-approach contact sheets were inspected
(`movement/littleroot-mom-shoes-{0..5}-valid2` and
`movement/littleroot-female-mom-shoes-{0,3,5}-run`, ROM
`9341630848c6097c79e3d5908056eedb9002f692bfc35fb9fe3b809089af4fa2`).

The x11,y9 trigger also passes when the player walks north through the
hidden truck template at x11,y10, for both player genders
(`movement/littleroot-{female-}mom-shoes-3-south-run`). Studio's warp
setup rejects *starting* directly on that template even when its hide
flag is set; walking through it is valid. No retail south-side blocker
was found.

The first synthetic stage3 probes wrongly left both trucks visible.
The real Inside of Truck intro hides the opposite-gender truck, and the
player's own truck is hidden when the family enters the house; both
must be hidden by this scene. Their stale sprites made one approach
look occluded. Replays with both hide flags set remove the obstruction
without changing game source. The female synthetic fixture sets
gender but retains the test name BRENDAN, so its item message uses that
name; this does not establish a retail female-name error. A full Key
Items pocket before this early scene remains unproven as an earned
state; these runs validate ordinary delivery and movement, not that
failure branch.

### Secret Base shared-template trainer spots

The two remaining duplicate-template candidates in Yellow Cave2 and
Tree3 put the record-mix Trainer and a decoration slot at(1,1) or
(1,2). In both layouts that tile is metatile behavior0xB3,
`MB_SECRET_BASE_TRAINER_SPOT`. Sprite decorations require a
small/large-decoration behavior and floor decorations explicitly reject
the trainer spot in `CanPlaceDecoration`. The decoration object is
moved to its saved placement only after a valid decorating action;
its default template coordinate is not a live second occupant.
This closes those two *placement* candidates by source/terrain rules,
not by a native record-mixing visit or exhaustive decoration playtest.

### Briney's four early boat handoffs

Ran native first and repeat Route104→Dewford voyages. The first includes
Norman's PokéNav call, sets its receipt, and lands with Briney and boat
visible on Dewford; the repeat bypasses the call and reaches the same
usable landing. From Dewford, the pre-Steven-letter yes prompt returns
to Briney's Route104 house, while the post-letter Slateport selection
lands on Route109 with the player, Briney and boat placed in separate
tiles and both arrival flags clear. Their arrival messages match the
letter/Devon Goods state. All four return control; contact sheets were
inspected for sailing, cross-map camera, boarding and landing
(`movement/briney-route104-{first,repeat}-sail-run`,
`movement/briney-dewford-return-before-letter-run`, and
`movement/briney-dewford-slateport-after-letter-fixed-input-2`).
The first Slateport menu probe selected Exit because its Up input came
before the menu opened; a later probe captured the menu, then the
corrected Up/A sequence executed the intended branch. This was a
fixture-timing issue, not a gameplay defect. The changing little head
seen beside the boat in Route105 is its Swimmer NPC; player visibility
telemetry confirms the player is hidden during sailing. The ROM is
`8f42a2b40d6a8e33428fd609e3dde30d4e08f1750c38b42f37f407401430845d`;
the three modified sailing scripts match that build's archived source.
These are synthetic branch replays, not one earned early-game trip.

### Mossdeep Scott approach geometry

Scott's one-time outdoor conversation is at(61,29), elevation5. A native
west-side talk plays all warning-letter/Space Center dialogue, walks him
down and left past the player, increments `VAR_SCOTT_STATE` once and
returns control with Scott gone (`movement/mossdeep-scott-west-run`).
The north/east adjacent warp probes are collision-marked and rejected
before execution. The south adjacent tile accepts a player placement,
but is on the lower elevation and cannot talk to Scott; a bounded
interaction probe leaves the state unchanged
(`movement/mossdeep-scott-south-run`). The apparent face-to-face
spacing in that synthetic warp is a height separation, not a stuck
conversation or disappearing Scott. Both available contact sheets were
inspected. The source pass also caught a dialogue mismatch: Scott
called Mossdeep's two Gym Leaders a singular "Gym Leader." His line now
uses "Gym Leaders are," matching Tate and Liza and fitting the message
box at192 pixels. The corrected native west-side talk again returns
control with Scott gone and state1; its contact sheet was inspected
(`movement/mossdeep-scott-plural-after`, ROM
`9341630848c6097c79e3d5908056eedb9002f692bfc35fb9fe3b809089af4fa2`).

### Sandstrewn Odd Keystone lost pickup and legacy flag repaired

The Odd Keystone at(3,14) is the sole live item source for the
Abandoned Ship Spiritomb interaction. Its script played the found-item
message, called `additem` without checking success, then removed its
object. A native full *Items* pocket (93 distinct slots) reproduced the
loss: Stone0, hide flag1, ball gone, and a false "put away" message
(`movement/sandstrewn-odd-keystone-items-full-before-run`). The script
now checks room before the fanfare or dialogue, leaving the ball in
place and returning a Bag-full message on two consecutive talks. The
same build's ordinary pickup gives Stone1, hide flag1 and control ready
(`movement/sandstrewn-odd-keystone-{items-full,normal}-after-final`).
The hardcoded pocket line now says Items rather than Key Items,
matching `GetItemPocket(ITEM_ODD_KEYSTONE)`. Before/after and normal
contact sheets were inspected.

The prior Keystone hide bit0x20E was also the collectible Underwater
Route128 Dive Ball receipt before the Inclement rebase. Setting that
legacy receipt on the earlier ROM made an untouched Keystone
disappear. The Keystone now uses unused persistent system bit0x918;
the old receipt remains at0x20E, and the daily flag boundary/save
layout do not change. Native before/after re-entry shows old receipt1
with Keystone hidden1→0 while the item count stays0
(`movement/sandstrewn-odd-keystone-legacy-{before-run,after-final}`).
All final replays use ROM
`f1ba3c3ec8fdaec820c5598c85ad965d8c59c64ae6e8aa705db0067aaeae7b4a`;
its source archive matches the current flags and Sandstrewn script.
Build, scoped diff check, text widths and the static progression
verifier pass. Other live `additem`/removal paths either preflight
capacity or branch on the result in the bounded analogous scan.

As with the Route111 rock flags, a save that already collected the
Keystone under the old bit may see this one pickup respawn once. A
save that *lost* it to the old full-pocket bug has no separate
Spiritomb-attempt receipt to distinguish that loss from a legitimately
consumed Keystone; this source repair prevents new losses but does
not automatically reconstruct that ambiguous history.

### Sandstrewn fossil receipts restored by item identity

Eight fossils in the pre-rebase Sandstrewn map occupied the same
coordinates and represented the same species as the eight current
pickups, but the Inclement materialization bound their hide flags to
old receipts from unrelated maps. Native reproduction: a legacy
Granite Cave Dusk Ball receipt hid an uncollected Dome Fossil
(`movement/sandstrewn-dome-legacy-collision-run`). The current fossil
flags now alias their original same-fossil receipts0x35/0x37–0x3D,
including Cover0x3A, Helix0x3B and Dome0x3C. The earlier Route111 rock
and Fallarbor rival fixes had already removed their erroneous reuse of
those bits. Under eight unrelated old receipts, native queries show
all eight fossil flags clear and the Dome visible; under eight
same-fossil old receipts, all eight stay collected and the Dome stays
absent (`movement/sandstrewn-fossils-legacy-{unrelated,owned}-after-run`).
This preserves the old save's actual fossil ownership rather than
making eight pickups respawn.

Old Amber had no pre-rebase Sandstrewn pickup. Its previous bit0xE9
was a live Trick House Master Ball receipt, so it now uses the
previously unnamed persistent system bit0x919. A native legacy
Master Ball receipt leaves Amber visible
(`movement/sandstrewn-amber-legacy-after-run`). Normal Dome and
Amber pickups set only their intended hide flags and grant one item;
their contact sheets were inspected
(`movement/sandstrewn-{dome,amber}-pickup-after-run`). Build ROM
`7b720b57e38a9bdcde78171635f8592bd721c943cdf3e85d2ad07f482f9ef45e`
contains the current flag definitions and passes the scoped diff and
static progression checks. A player who picked up Old Amber under the
post-rebase0xE9 binding may see that one pickup reappear once; the
earlier campaign had no such Sandstrewn Amber receipt.

### Seaspray same-item pickup flags and three replacement receipts

Compared both Seaspray floors with the pre-rebase map JSON by item and
coordinate. Lure Ball(10,24), Blastoisinite(6,25), and King's Rock
(46,15) are the same pickups in both versions, but the current map
bound them to unrelated old save bits. A native old-save setup with
all three *same-item* receipts set showed all three current flags0,
with King's Rock still visible. An unrelated old Dawn Stone receipt
instead hid King's Rock before this repair
(`movement/seaspray-same-item-old-receipts-before-run` and
`movement/seaspray-king-unrelated-old-receipt-before-run`).

The three current hide flags now alias their original same-item
receipts. The displaced newer B1F pickups—Revive, Absolite Z and
Abomasite—use previously unused persistent bits0x2AA,0x466 and0x897
respectively. Native replays with the three same-item old receipts
keep exactly those pickups hidden while the three replacements remain
available; an old Dawn Stone receipt no longer hides King's Rock
(`movement/seaspray-same-item-old-receipts-after-run` and
`movement/seaspray-king-unrelated-old-receipt-after-run`).
All six normal pickups grant their intended item and mark their own
receipt; their result queries pass and representative contact sheets
were inspected (`movement/seaspray-{lure,blast,king,revive,absolite,
abomasite}-pickup-after-*`). The first Lure Ball fixture faced north
from the north-adjacent tile, so it did not interact; corrected south
facing passed. That failure was a fixture direction, not a game block.

ROM `15b701cec71e7f9c3dbe16c1e39cc00ad8ede610f30386444b2971b816809215`
contains the six final Seaspray definitions. The full flags file
changed afterward only for an unrelated Poipole flag; the six scoped
definitions match the ROM archive. Build, scoped diff and static
progression checks pass. Post-rebase saves that already collected one
of the three *replacement* pickups may see it respawn once after this
flag move; the retained Lure Ball, Blastoisinite and King's Rock keep
their old collected state. Other changed Seaspray pickups still need
the same source/old-save review.

### Three actor-ID metadata handoffs corrected

The Lilycove Harbor map named actor4 `LOCALID_LILYCOVE_HARBOR_BRINEY`,
but actor4 is the Sailor sprite and actor5 is Briney's Expert sprite.
Meteor Falls named Archie actor7 as Aqua grunt2 while the real grunt
actor9 lacked that ID. Route111 named Victoria actor2 as Vicky and
named the desert fossil actor33 as Victoria; Vivi actor3 and Vicky
actor4 lacked IDs. Corrected the map JSON names without changing any
actor number, graphic, flag or movement. The scripts' existing numeric
local IDs already targeted the right actors, and the regenerated map
header now names their actual identities.

Native verification on ROM
`5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`
passes the first Old Sea Map ferry trip: Sailor4 makes way, Briney5
appears and addresses the player, all three board, Faraway Island is
visited, and a separately selected return reaches Lilycove with
control ready (`movement/lilycove-harbor-old-sea-map-briney-validated`).
The first assertion expected to stop at Faraway Island, but repeated A
input chose the return trip; a visited-map assertion records the
actually executed path. Its contact sheet was inspected.

The native Meteor Falls accepted-party scene visibly stages Archie7
and both grunts8/9 after the fixture-assisted Magma fight, then
removes them, advances state1 and returns control
(`movement/meteor-archie-grunts-metadata-accepted-run`).
An earlier all-A fixture repeatedly toggled the first party slot;
the corrected sequence uses the menu's Start confirmation. The
Route111 Winstrate gauntlet also passes four fixture-assisted fights
with Victor1, Victoria2, Vivi3 and Vicky4 entering/exiting the house
in order. All four hide flags are set on return and control is ready
(`movement/route111-winstrate-four-exits-run`). Harbor, Meteor Falls
and Winstrate contact sheets were inspected. These are visual/script
checks, not tactical battle wins or one earned continuous campaign.
The ROM receipt records Git commit bce92012db for these map sources;
the static progression verifier passes540 maps and3,996 events.

### Fallarbor and Verdanturf Battle Tent corridor staging

The two simple Tent corridors start their OnFrame movement as soon as the
warp begins, so a normal ready-state scene capture starts too late to see
the walk. Captured from the warp transition instead, on ROM
`5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`.
Both native runs passed their Battle Room visit assertion. Their inspected
contact sheets show the attendant ahead of the player through the four-step
approach, a clear doorway as they enter in order, then the door/fade handoff
(`movement/fallarbortown-tent-corridor-immediate2` and
`movement/verdanturftown-tent-corridor-immediate2`). No visible collision,
actor overlap or blocked door appeared. Fallarbor returned to its lobby
after the Battle Room under this synthetic fixture state; Verdanturf entered
its opponent introduction. These runs establish corridor presentation, not
a completed Tent challenge.

Slateport uses a different rental/menu path. Its native warp-entry capture
shows the Scientist attendant ahead of the player, both aligned in the
corridor, and the attendant turning to deliver the first rental explanation
with the player one tile behind (`movement/slateport-tent-corridor-run`). The
contact sheet was inspected and the script remains live awaiting dialogue
input. The lobby source sets `VAR_0x8006` to zero before a new challenge and
warps to the same (2,7) entry used in this replay; this checks the entrance
presentation but does not establish rental selection or a completed challenge.

### Fallarbor Evie/Ivy actor metadata

The first-visit script explicitly moves Evie as actor1 and Ivy as actor2,
then has Evie explain EV training and Ivy explain IV training. The house map
instead named actor1 `LOCALID_MOVE_RELEARNER` and actor2
`LOCALID_EV_TRAINER`, neither of which describes the attached service or
speaker. Renamed the map metadata to `LOCALID_EVIE` and `LOCALID_IVY`,
matching the script's existing numeric IDs. Generated event constants are
now1/2 under those names; the map event data and scene commands are unchanged.
The earlier native doorway and repeat-entry runs (`movement/ivy-evie` and
`movement/ivy-evie-repeat`) already show actor1/2 completing the introduction
and returning control. This change corrects actor identity for subsequent
source work; it does not claim a new movement behavior or require a ROM fix.

### Rustboro rival approach, decline and retry

Ran the southern center trigger with May and Brendan on ROM
`5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`.
Both rivals visibly approach, face the player, offer a battle, use their
gender-correct overworld and battle portraits, deliver the Briney hint after
a fixture-assisted win, set met/defeated and Rustboro state10, and return
control (`movement/rustboro-rival-{may,brendan}-first-run`). May's No choice
also leaves defeated unset, keeps her present, restores control and shows
the decline dialogue (`movement/rustboro-rival-may-decline-corrected`). A
separate post-decline state10 entry talks to May from the north, accepts the
retry and completes the same postbattle return (`movement/rustboro-rival-may-
retry-facing-fixed`). The success, decline and retry contact sheets were
inspected. The two early retry probes faced away from May, so A did nothing;
that was a scene setup error. These are separate synthetic starts, not one
earned save or tactical battle wins. The earlier assembled script test checks
music cleanup routing; these visual replays do not capture audible music.
No new script defect surfaced.

The same initial May encounter was then replayed from all eight Rustboro
south-street trigger tiles, x12–19 at y53. Every compiled path leaves the
player at its trigger tile, places May directly north at (x,52), sets
state10/met/defeated, and returns control after the fixture-assisted battle.
The two longest outer approaches and a representative middle approach were
inspected visually: no rail crossing, actor overlap or wrong facing appears.
Evidence: `movement/rustboro-rival-outer-{12,19}-run`,
`movement/rustboro-rival-lane-{13,14,15,17,18}-run`, and the center x16 run
above, all on the same ROM. This closes the eight-trigger movement matrix;
it does not extend tactical or audible-music verification.

### Rusturf full-Key-Items reward and same-state recovery

The rescue's Devon Goods use the Key Items pocket. A native fixture-assisted
grunt battle with all55 slots synthetically occupied by an unrelated Key Item
reaches the Bag-full refusal with control restored, Goods0, stolen flag still1,
rescued flag0, and grunt/Peeko still present
(`movement/rusturf-rescue-full-goods-with-snapshot`). The full-pocket state was
saved, one slot freed, and the same native state resumed. Talking to the
already-defeated grunt then grants exactly one Devon Goods without a second
battle; the grunt leaves, Briney approaches Peeko, both depart, and the
Rustboro/Briney house story states advance to5/1. Final flags are rescued1,
stolen0 and grunt hidden1, with player control ready
(`movement/rusturf-rescue-retry-goods-run`). Both contact sheets were inspected.
The repeated synthetic filler item is a capacity probe, not an obtainable
inventory; the scene itself runs native scripts and movement. No source fix
was needed for this failure/retry branch.

### Devon president repeat before Letter delivery

After the introduction state1, the walkable desk-side tile is (14,5); (16,5)
is furniture collision. A native return interaction from(14,5) addresses
Mr. Stone through the map's intentional invisible actor at(15,5), shows
“I'm counting on you!”, leaves state1 and returns control
(`movement/devon-president-repeat-proxy`, same ROM). The contact sheet shows
the visible president at his desk and no stray proxy sprite. The first
probe's blocked (16,5) warp was a setup error, not an inaccessible NPC.

### Rusturf completed rescue re-entry

The rescue script removes the grunt, Briney and Peeko without separate
`setflag` lines, which initially looked like a reload risk. In this engine,
`ScrCmd_removeobject` calls `RemoveObjectEventByLocalIdAndMap`, and that routine
sets each removed object's template hide flag before despawning it. The saved
native state after the full-pocket retry confirms all three flags1. Rewarping
to Rusturf Tunnel from that same completed state leaves only the player in
the rescue area, shows no repeat rescue dialogue, and returns control
(`movement/rusturf-reentry-after-success-run`, ROM above). Its contact sheet
was inspected. No code change is warranted; this closes the re-entry doubt
using both engine behavior and a real map reload.

### Sootopolis Gym crisis door and Wallace handoff

Before the Magma/Aqua leaders depart, Wallace stands at(31,33) in front of
the Gym. Native talk from(31,34) delivers his request to hear both leaders,
does not grant Waterfall, and a continued north input cannot pass him
(`movement/sootopolis-gym-wallace-blocked-run`). Separately hiding Wallace
in a synthetic pre-departure setup isolates the physical door: the OnLoad
metatile closes (31,32), and north input stops the player at(31,33) without
entering the Gym (`movement/sootopolis-gym-door-closed-run`). This second
setup is a door test, not a claimed reachable story state.

With the leaders-departed flag set and Wallace present, native dialogue
grants the Waterfall license, explains the Rain Badge/capable-Pokémon rule,
moves Wallace one tile right and persists Wallace state1. The player then
walks straight through (31,32) into Sootopolis Gym1F with control ready
(`movement/sootopolis-gym-wallace-unblock-run`). A separate seeded re-entry
with that persisted state places Wallace right of the doorway and lets the
player enter again (`movement/sootopolis-gym-wallace-reentry-run`). All four
contact sheets were inspected on ROM
`5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`.
This covers the access choreography and physical blockage, not a continuous
earned weather-crisis run or Juan's battle. No source fix was warranted.

Wallace's lateral talk branches also pass. From the west tile(30,33), the
player faces east and Wallace steps right to(32,33), persisting state1;
from the east tile(32,33), the player faces west and Wallace steps left
to(30,33), persisting state2. Both Waterfall talks return control with
the center lane clear (`movement/sootopolis-wallace-{west,east}-talk-run`).
Native map re-entry with either stored Wallace state1 or2 places him on
the corresponding side and allows a straight north walk into Gym1F
(`movement/sootopolis-gym-wallace-{reentry,left-reentry}-run`). All four
additional contact sheets were inspected. These starts seed the post-crisis
state independently; they do not claim one continuous save through the
legendary scene.

### Sootopolis leaders-to-Gym local story handoff

At post-Rayquaza state5 with both leaders still present, native first talks
to Maxie and Archie each set only that leader's met flag; both actors stay
visible, Wallace stays at the center doorway, and control returns
(`movement/sootopolis-{maxie,archie}-first-talk-run`). If the other met flag
is already set, either leader's talk hides both, sets their departure flag,
updates Mt. Pyre state2, and warps the player to(31,34) directly south of
Wallace (`movement/sootopolis-archie-second-departure-run` and
`movement/sootopolis-maxie-second-departure-west`). Maxie's first attempted
approach at(33,34) was a collision tile; the corrected west approach(32,35)
is walkable. First and second talk contact sheets were inspected.

A single native scene then talked to Maxie, walked around the pair on the
southern edge, talked to Archie, accepted Wallace's Waterfall license and
walked into Gym1F without restarting or reseeding between conversations
(`movement/sootopolis-leaders-wallace-gym-continuous-run`). It finished
with both leaders hidden, both met flags1, departure1, Waterfall1,
Wallace state1, Mt. Pyre state2 and player controls ready in the Gym.
The contact sheet was inspected. This validates the local post-Rayquaza
handoff and both second-speaker orders; its starting state is synthetic,
so it does not prove the entire weather crisis through an earned save.
No scene or dialogue correction was needed.

### Cave of Origin Wallace wrong-answer recovery

Replayed the full native conversation on ROM
`5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`
with each of the three wrong menu choices—Cave of Origin, Mt. Pyre and
“Don't remember”—followed by Sky Pillar in the *same* interaction. Each
choice shows its distinct response, reopens the four-choice menu, and
accepts Sky Pillar. Wallace fades out, his Cave actor stays hidden, the
Sky Pillar actor becomes available, Sootopolis advances2→3 and control
returns (`movement/cave-wallace-wrong-then-correct-run` and
`movement/cave-wallace-{1,3}-then-correct-run`). Native task
state detected the actual menu before each input, avoiding frame-timed
guesses. The three contact sheets and recorded dialogue transitions were
inspected. These are synthetic city-stage starts; no source change was
warranted for the retry loop.

### Sealed Chamber underwater ascent split

The Sealed Chamber underwater map chooses its surface destination from the
player's exact coordinate. Native Dive from(12,44) takes the player to
Sealed Chamber Outer Room(10,19), while Dive from neighboring(12,45)
surfaces on Route134(60,31). Both return control with the player visible;
their contact sheets show the underwater prompt, Dive animation and distinct
surface landings (`movement/sealed-chamber-dive-{inner,outer}-run`, ROM
`5dd27b957ff6d4098736a50c8239edbeab5f7e722eb4514e25937bd18557103a`).
These starts seed the underwater positions and licenses, so they verify the
two branch destinations rather than a complete Route134 current traversal.
No source correction was needed.

### Route134 surface-to-Sealed-Chamber connected passage

The designated Route134 deep-water patch at(60,31) natively Dives to
Underwater Route134(8,6). From there, three south steps cross its downward
arrow warp and land at Underwater Sealed Chamber(7,2). A 49-step path
through the long underwater passage reaches the marked(12,44) ascent tile;
native Dive then lands at Sealed Chamber Outer Room(10,19) with control
ready (`movement/route134-surface-to-sealed-chamber-run`). Its two contact
sheets were inspected across the surface Dive, inter-map warp, passage
turns and chamber landing. The first entrance probe stopped on the arrow
tile after one south step; a second south step triggered the warp. That
was an input-boundary issue, not a blocked entrance. Collision-grid routing
selected the path, but native movement and rendered frames establish that
it is walkable. The start is a synthetic warp to Route134's deep-water
patch, so reaching that patch through the route's surface currents remains
outside this scene. No map or movement code change was warranted.

### Route134 current into the Sealed Chamber Dive patch

At Route134(65,31), a short west input follows the current into the
deep-water pocket and stops at(60,31) when released. A longer held input
overshot the pocket to(54,31), showing why the Dive opening requires
steering rather than simply holding west. The corrected native path keeps
the surfing sprite visible at the pocket (`movement/route134-current-to-
dive-patch-stopped`). Extended without restarting, the same current approach
then Dives through Underwater Route134, crosses the arrow entrance, walks
the49-step underwater passage and surfaces in Sealed Chamber Outer Room
(`movement/route134-current-to-sealed-chamber-run`). Both of its contact
sheets were inspected. This ties the current to the chamber, but begins on
Route134's current rather than at Pacifidlog or the Route133 boundary.

A separate Route133 boundary probe was not accepted as natural Surf evidence:
forcing a fresh session onto shallow/current water made its walking sprite
leave the Surf blob behind. Starting on a walkable Route133 shore tile and
using the real Surf interaction, then entering a westward current, kept the
surfing sprite attached (`movement/route133-native-surf-current-run`). The
apparent sprite separation therefore belongs to that synthetic warp setup;
the route's upstream current lanes still need a continuous native traversal
from a valid surfing start. No source edit followed from the invalid probe.
