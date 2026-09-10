# Protect playtest repair — verified full-ROM playtest delivered

LATEST USER CORRECTION: the retroactive-audit requirement described below was
the assistant's overinterpretation and is withdrawn. The user wants the opening
Protect problems corrected and the full playable ROM immediately, without a
new campaign-wide or last-opponent audit gate. The verified1848 full ROM is
offered at `playtests/20260909-1848-protect/pokeemerald-champions-protect-20260909-1848-release.gba`.
Its copied bytes match the artifact-verified release and SHA256
`098d3a3fe471ae5c917d6350798d797bfa24a7c27e4e66d2db63ab4088d1c94c`.
The native evidence and its limits below remain valid; no new fallback or
trainer modifications were made during the withdrawn audit's read-only start.

## Historical interpretation — withdrawn by the user

Latest user clarification: all repairs must apply retroactively to every battle.
The paired correction is shared, not trainer-ID-specific. However,
`AI_ComputeDoublesDecisions` requires both own active battlers and smart-mon
flags on both; the one-remaining-opponent fallback uses separate legacy scoring.
Audit dispatch/flags and this fallback, then recheck the already-reviewed
encounters' main strategies before new authoring. Existing28-branch/22-record
completion is historical review coverage, not completed retroactive validation.

The fresh1848 named release built successfully and passes all release gates.
ROM used27,901,896 bytes (+1,472 versus1728), EWRAM232,282 and linker IWRAM28,320
(unchanged RAM). It was booted natively to title rendering without save/state
injection. It has NOT been delivered as a replacement; retroactive coverage is
pending. The artifact/source stamp is
`pokeemerald-champions-protect-20260909-1848-release.inputs.json`; build and gate
logs are `work/protect-release-20260909-1848-{build,verification}.log`.

## Current checkpoint — empty pair guards corrected

The repeat margin below is supplemented by a bounded pair rule: when a usable
alternative exists, do not choose two ordinary self-Protect actions without an
identifiable benefit from waiting. One guard beside an attack, setup or switch
remains eligible. Contact-effect shields and side guards are not classified as
ordinary passive self-Protect. Both-only-Protect boards retain a legal fallback.

`PairWaitingHasPayoff` checks public state once per candidate board: useful orb
activation, a Speed Boost crossing (not already-faster accumulation), healing,
authored Perish progress, residual damage/Yawn, opposing Fake Out/Truant windows
and relevant field countdowns. This permissive heuristic is not a full end-turn
simulator or proof that every allowed wait is optimal. No recursive search,
additional simulation pass or persistent state was added.

Evidence:

- `work/protect-empty-pair-before.log` reproduces Allen's empty double guard
  and fails the new no-empty-pair assertion before the runtime correction.
- `work/protect-empty-pair-final.log`:61 groups pass (46 retained,15 temporary).
  Against two Wacan Natu, Allen now uses Fake Out while switching Paras into
  Bunnelby, then Volt Tackle/Rock Slide. After two executed turns own HP11/34;
  one Natu is knocked out and the other has28HP. The former opening left both
  teams' HP unchanged. This is a scripted comparison, not a win-rate estimate.
- The actual rival reserves retain Protect/Protect to activate Taillow's Toxic
  Orb. Aron's earlier repeat correction retains its useful turn3 guard and
  attacks on turn4. Hoenn rival/rescue and early-route samples execute, but the
  user's unknown exact party/sequence remains unreproduced.
- A focused five-case regression covers empty waits, Guts orb, useful Speed
  Boost, already-faster Speed Boost and only-Protect fallback. Disabling ONLY
  the new filter makes it fail: `work/protect-empty-regression-disabled.log`.
  The filter was restored immediately afterward.
- Complete Devan timing stays48 frames (7+41), about0.8seconds; the existing
  full-party decision benchmark is30 frames. These are samples, not a verified
  campaign-wide maximum. Release ROM growth has not yet been measured.
- After removing scratch includes and temporary logging,
  `work/ai-shared-protect-repair.log` passes all46 retained shared groups.

### Three old assertions audited, not treated as gameplay requirements

The initial candidate run failed three old expectations:

1. Powder-immunity controls prescribed Protect. They now require no sleep on
   the immune foe and no selected Sleep Powder targeting it. The positive
   timely-sleep and97% native-accuracy checks remain.
2. A berry/recoil AI-choice fixture prescribed empty double-Protect when a
   berry was absent/blocked. With only one progressing action it no longer
   discriminates the berry forecast under this policy. Retired that synthetic
   group rather than claiming relaxed expectations preserve equivalent
   forecast coverage. Native berry mechanics are unchanged; deleted tests
   are not counted as passing.
3. Charm was prescribed on turn2. Native tracing shows Charm on turn1, then
   Encore against the other foe on turn2. The retained test requires Charm
   before native Drain Punch damage, Attack reduced by two stages and Minun
   surviving, without scripting the second-turn action/target.

One necessary reported-bug regression replaces one obsolete AI-choice group;
the retained shared count remains46. No additional trainer was signed off.
A fresh release and artifact verification are next before a replacement
download. Full campaign authoring remains active after this repair checkpoint.

## Earlier investigation history — superseded where noted

The user reports constant Protect in the first couple of battles of the frozen
1728 playtest. Sequential authoring remains paused. A partial repeated-guard
policy correction is implemented below; no corrected ROM has been delivered.
The user's exact starter pair and move
sequence are not known. Do not require those details before addressing the
shared policy, or treat negative samples as refuting the report.

## Implemented checkpoint — repeat commitment margin

`PairPlanScore` now charges35 points for self-protection after a move using
the Protect counter. This is the existing voluntary-switch commitment margin:
a small forecast advantage no longer justifies repeatedly spending the turn
on defense. It is explicitly a policy heuristic, not a change to probability
or a ban on guards. Meaningful partner damage, healing and Perish value still
compete. Modern side guards are excluded. The cost is cached once per board
action; no new simulation pass, search branch or persistent state was added.

The history input is `gLastMoves` (attempted move), NOT `gLastResultingMoves`.
A native success/failure/success diagnostic verifies counters1/0/1 and next
denominators3/1/3. After failure the attempted move remains Protect while the
result is MOVE_UNAVAILABLE. An initial diagnostic assertion assumed otherwise
and failed; the test and policy history input were corrected from that evidence.
The native probability cache continues to use last RESULT, as required by the
engine's reset semantics. Policy and mechanics intentionally use different facts.

Before/after evidence:

- `work/protect-repeat-regression-before.log` fails the new no-repeat assertion
  in the known Devan turn4 case, before the runtime edit.
- `work/protect-repeat-verified.log`:55 groups pass, including all46 retained
  shared groups and nine temporary diagnostic groups. Aron retains Protect
  on turn3 while Sandile takes the knockout; on turn4 it uses Iron Head instead
  of another Protect. Own HP remains6/5, while remaining Pachirisu HP drops to14
  instead of20. This is one native scripted comparison, not a general win-rate
  estimate. A turn5 Protect is precomputed but not executed.
- Existing Wish and Perish regressions remain green. Complete Devan cold
  decision remains48 frames (7 setup +41 both actors), about0.8seconds, including
  switch/form consideration and a six-member player party. This is a sampled
  position, not a campaign-wide upper bound or a release ROM size measurement.
- Current Allen versus two Wacan Natu still uses double-Protect on turn1, then
  Volt Tackle/Wide Guard on turn2. The opening wait leaves both teams' HP
  unchanged. **This separate empty first-use guard defect remains unresolved.**

Do not resume trainer authoring or call the whole report fixed on this partial
improvement. Next work targets the observed empty double guard, with positive
waiting controls (orb/Speed Boost, Wish and countdown) before changing first-use
policy. The user's exact opening sequence remains unreproduced.

The initial52-group run and boundary run preceded the corrected history input;
use `protect-repeat-verified.log` for the complete current diagnostic batch.
The boundary log contains the intentionally investigated failed reset assertion,
not a native Protect-mechanic defect. Scratch includes are removed afterward;
no new permanent battle-test catalogue was added.
After cleanup, `work/ai-shared-protect-repeat-margin.log` passes all46 retained
shared groups with the production policy edit and no scratch includes.

## Pre-correction native evidence

- `work/protect-opening-trace.log`: ten opening-trainer scenarios (three Hoenn
  rival branches, Calvin, Rick) did not reproduce frequent consecutive guards.
  These use prepared level-14 Pachirisu/Eevee, not the user's unknown party.
  The separate lone-Mudkip case requested too many turns; its captured first
  turn used Liquidation. With only one own active, it exercises legacy AI,
  not the paired evaluator. The scratch fixture was corrected but not rerun.
- `work/protect-rescue-baseline.log`: six native-factory rescue cases, all
  three Hoenn starter pairs at level5 versus actual level2 opponents, in the
  FIRST_BATTLE/DOUBLE opening context. Zigzagoon protects on turn1 and attacks
  on turn2 in these samples. This is not a full-map playthrough or coverage of
  regional starters.
- `work/protect-known-repeat-baseline.log`: the existing Devan core diagnostic
  reproduces Aron choosing and attempting Protect on both turns3 and4 with
  Sandile attacking. On turn4, the best observed attack pair scores56; the
  repeated-Protect pair scores61, with a cached success chance of33% and a
  raw Protect score of88. The latter already includes a substantial legacy
  penalty. Protect fails and Aron drops from12 to6HP; Sandile leaves Pachirisu
  at20HP after its berry. The precomputed turn5 selection is another Protect,
  but turn5 was NOT executed. This later encounter's current authored team
  is not in the frozen download; the shared decision code is unchanged.

These are scripted native outcomes, not measured population frequencies or
win rates. A diagnostic PASS means the scenario executed; it does not approve
the tactical behavior. The observed repeat is real, but its small forecast
advantage alone does not prove every repeated guard is strategically wrong.

## Source-backed diagnosis

`CachePairMoveEffects` already shares the native consecutive-use denominator,
including the previous-result reset and modern side-guard exceptions. It models
100/33/11/3 percent success. A failed Protect resets the native counter, so a
subsequent attempt can again be reliable. This is not a missing probability
calculation; do not change the underlying battle mechanics to hide AI behavior.

The pair evaluator maximizes a short-horizon board value over two bounded foe
forecasts. Staying alive contributes substantial value, but deferring an
unfavorable exchange does not itself establish a better future position.
Earlier E0001 diagnostics already identified empty double-Protect under this
objective. Replacing rival Pikachu's Protect with Helping Hand removed one
combination, not the shared design limitation. Historical per-team completion
must not be read as certification that this policy issue was solved.

Next correction must address unproductive guarding/repeated gambling while
retaining meaningful partner offense/setup, Wish, orb/Speed Boost, Perish and
valid side guards. Define a small acceptance set before changing the scorer;
do not resume scalar-tuning loops or add recursive search. Keep complete
decisions below1.2seconds. The exact opening reproduction remains incomplete.

Temporary score logging and scratch includes were removed from production/test
source after capturing this evidence. Scratch scenarios remain in `work/` for
targeted reuse; they were not added to the retained regression suite.
