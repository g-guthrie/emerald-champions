# Ability mechanics audit — September 20, 2026

Scope: 14 native ability test files, 149 test groups. Initial result: 143 pass,
six fail. This is partial battle-mechanics coverage, not full campaign acceptance.

## Commander attachment state

Yawn calls CancelMultiTurnMoves, which previously cleared STATE_COMMANDER while
Tatsugiri remained attached to Dondozo. Preserve that attachment state alongside
STATE_SKY_DROP_TARGET; continue clearing move-owned semi-invulnerability.
All 43 Commander groups pass after the change. A read-only follow-through of
remaining direct state clears found move-specific guards or deliberate attachment
release on Dondozo faint/exit; no additional reachable defect was demonstrated.

## Fixture corrections

Berserk single-hit timing now starts two HP above half: the existing game's
Berry threshold already activates at half plus one. The multihit case specifies
stats and maximum noncritical damage, asserting each hit and Berry restoration.
Pickpocket specifies lethal damage so Focus Sash is actually consumed before
attempting item theft. Unseen Fist's historical no-contact-effect expectation
explicitly selects Gen 9; Champions intentionally applies the contact effect.
All 38 groups across these three files pass. No production behavior was changed
for these four failed fixtures.

## Imposter doubles targeting: reproduced and repaired

The isolated Spore case fails with both the original reduced-information flags
and the current campaign expert flags: opponent Ditto targets its Wimpod partner
instead of the vulnerable opposing Smeargle. Each profile was run first in its
own diagnostic invocation because a failed parameter stops the remaining cases.
The retained test now parameterizes both profiles, current campaign first.
The single-battle AI and explicit copied-slot execution tests pass.

Removed the misleading Billy-lead test name: current TRAINER_BILLY leads Lotad
and Ducklett; Ditto is a later party member. This test isolates a possible board
state, not the actual whole Billy battle. Temporary native traces established planned target=3 and final target=3 for
actor1: the controller preserved the planner decision. Before this repair both
AI files matched pre-integration commit 508775fad8. The defect was already present
there, rather than introduced by the integration.

Two connected planner omissions caused the reproduction:
- Ally sleep actions were enumerated but omitted from the effect cache, so their
  harmful sleep state was absent from the forecast.
- Sleep set its future-state flags but published its probability only when it
  denied an unacted target's current action. A target that already used Aqua Jet
  therefore lost its future sleep penalty during probability blending.

Cache allied sleep using the existing native immunity/clause checks, and publish
sleep probability independently of whether an action remains to stop. Existing
signed sleep-state value, scores, weights, teams and information permissions are
unchanged. Legal allied actions remain available. Cache-only failed the original
reproduction; both corrections together passed. The regression now covers both
flag profiles with both Imposter and an ordinary Smeargle (four cases).

Broader comparison: same five test files, 117 groups, on unchanged AI versus
candidate. Baseline 100 pass/17 fail; candidate 101 pass/16 fail. The sole resolved
group is the new sleep regression; no new failing group appeared. The remaining
16 are not accepted as valid gameplay or dismissed as stale fixtures: each needs
triage. See sleep-comparison.json for their exact identities. No full campaign
balance/playtesting claim follows from this bounded comparison.

Follow-through review identified additional pre-existing limitations to pursue:
sleep sets simulated status/clause bits before rejecting zero weighted probability;
sleep and primary paralysis omit same-turn actionChance weighting. Cure-berry
consumption and some beneficial allied status interactions remain unmodeled. These
were not folded into this demonstrated targeting repair without focused evidence.

## Evidence

work/ability-audit-20260920/ contains initial build/tests, commander-tests.log,
fixture-tests.log, imposter-build.log and imposter-tests.log. The final Imposter
log records the current-profile failure; the original-profile failure was also
observed before parameter order was changed. Do not report the full suite green.

Strict normal release build and release gates passed for the Commander fix:
release-build.log and release-gates.log. ROM size 27,561,380 bytes; the recorded
release input digest is a512d6aabf0e. Later edits are test/documentation only.

## Sleep repair evidence

sleep-regression-tests.log and sleep-baseline-tests.log are the matched native
comparison; sleep-comparison.json records the failure-set difference. Temporary
tracing was removed. sleep-release-build.log and sleep-release-gates.log record
the integrated release validation for the retained planner correction.

## Follow-up: isolate candidate-state tests from tactical preferences

The Trace candidate-loader test failed before reaching its snapshot assertions:
its opening AI chose a voluntary switch rather than Protect. Converted that setup
to an explicit native doubles turn and then initialized the expert AI caches for
the candidate-loader call. All original species, copied-ability, overwritten
ability, Intimidate stat-stage and full saved-state hash assertions are retained.
Added mixed Intimidate/Volt Absorb foes: the deterministic forecast must retain
Trace rather than inventing a favorable random copy. Four native cases now pass:
trace-fixture-tests.log. No production AI changes were made in this follow-up.

Defeatist's40%-HP parameter was overconstrained: the ability is already active,
and either attack can KO the1HP foe while Archen survives. It now checks that
survival/KO outcome; the60% crossing and100% healthy controls retain exact move,
HP and target assertions. This exposed a previously unexecuted failure at60%:
Acrobatics chosen instead of Quick Attack. defeatist-fixture-tests.log records
parameter2/3 failing. Earlier failures stop later parameters; source inference
must not be described as a passing native result. The100% parameter remains
unverified in this focused run. Do not mark this test group resolved.

Rock Tomb/Cloak remains open: source analysis supports the expected speed-crossing
contrast, but the Cloak case leaves Oranguru with no executed move instead of
Protect. Trace actual chosen targets and pair values before changing scoring.
Only Trace is resolved among the16 broader failures so far; no new aggregate run
has been performed after this fixture-only follow-up.

## Follow-up: target uncertainty in synthetic threshold tests

Temporary native score traces (defeatist-scores.log, rock-tomb-scores.log) showed
untruncated searches mixing different legal player attack targets. They did not
read the player's submitted target. Therefore the original Rock Tomb fixture
was not proof of a protection/mechanics defect: against its primary forecast,
Protect+Strength scored-122 versus Psychic+Strength-149; after alternative-target
weighting, they scored-98 and-68. The test had implicitly demanded a particular
prediction of the player's pending target. Earlier descriptions of this as a
proven mechanics defect were too strong.

Corrected the synthetic Rock Tomb board: Hisuian Zorua replaces Smeargle, retaining
explicit stats and Normal STAB on Strength; Ghost immunity makes Oranguru the
only effective target of the foe's sole Tackle. All original move, HP and survival
assertions remain intact. Both Cloak/no-Cloak cases pass with unchanged AI.
Evidence: rock-tomb-isolated-tests.log and rock-tomb-final-tests.log.

The analogous Defeatist board now uses passive Duskull instead of Pachirisu,
removing an alternative Quick Attack target through public Ghost immunity. The
60%-HP assertion still fails and is deliberately retained. Further native trace
(defeatist-isolated-trace.log) shows incoming median damage12 and conditional
Acrobatics anchors22 healthy/12 weakened. Against the now unambiguous forecast,
Acrobatics aimed at Eevee scores136, Quick Attack at Eevee103, and Acrobatics at
Munchlax115. Thus the threshold is modeled; the remaining mismatch involves
existing move valuation, not a missing Defeatist damage endpoint. Investigate
isolated native opinion/overkill bonuses before any proposed retuning. No
production AI changes were retained in this follow-up; temporary prints removed.

Trace and Rock Tomb are now resolved among the16 broader failures. Defeatist
remains unresolved; no updated aggregate suite claim is made.

## Defeatist follow-through: redundant-priority predicate repaired

The next native trace disproved the suspected ordinary move-opinion difference:
Acrobatics and Quick Attack both score112 against Eevee. Their pair prescores
were+8 and-37, exactly45 apart. PairPlanScore imposed-45 on priority whenever
raw Speed already beat the target, overlooking the target's own priority move.
The simulation correctly predicted48HP after Acrobatics versus60 after Quick
Attack; the faulty penalty outweighed the12HP benefit by33 points. This is a
predicate defect in an existing rule, not a change to its authored weight.

Retain-45 for redundant priority, but exempt a response that genuinely changes
order: a known (HasMove), usable damaging move from the target, with nonzero
cached damage to the actor and positive priority no greater than the actor's.
The actual slot supplies both limitations and damage while HasMove gates move
knowledge, avoiding history-slot misalignment. No selected move/target/switch
reads; no team or information-profile changes. Preserve the remaining plan
scoring after an exemption. Temporary traces were removed.

All four Defeatist cases pass:40%,60%,100% with Quick Attack, plus60% against
ordinary Tackle. The Tackle control retains the redundant-priority behavior and
checks the original damage/KO outcome. Full five-file117-group rerun:105pass,
12fail. Compared with the earlier101pass/16fail, no new failed group appears.
Trace/RockTomb fixture repairs and Defeatist/PlusMinus regressions now pass.
priority-comparison.json lists remaining failures; do not dismiss them or call
this full campaign acceptance. Native logs: priority-regression-tests.log.

Scope limitation from source review: priority threats from the other opposing
flank are not considered by this target-specific redundancy rule. Add a focused
regression before extending it. The separate sleep/primary-paralysis probability
limitations recorded above also remain open.

Strict integrated release evidence: priority-release-build.log and
priority-release-gates.log under work/ability-audit-20260920/.

## Stat-drop and recoil fixture follow-through

No production changes in this pass. Two remaining groups were overconstrained.

Stat-drop setup: forced Wonder Guard on the synthetic Smeargle so both known
Tackle and Ice Beam can affect only Oranguru. Species, stats, damage, STAB and
speed remain unchanged. This removes the submitted-target oracle requirement.
The late-drop parameter then exposed another invalid premise: Lunge/Skitter can
retain next-turn value after the threatening attack, even though they cannot
protect Oranguru on the current turn. Allow either Strength or the corresponding
drop move in that late case; retain Protect for Oranguru, full surviving HP, and
no damage to the irrelevant opposing Chansey. Timely/Cloak move and target
assertions remain exact. All six cases pass: drop-final-tests.log.

Recoil setup: native cache16/19/21 damage with33% recoil produces5..6 recoil.
At5HP a landed attack is fatal; at7 it is survivable;6 is a risk choice. Native
trace showed the selected6HP attack dealt16, paid5 recoil, then recovered4 via
Leftovers, ending5HP with no faint. The former fixture's mandatory Protect at6
encoded one risk preference rather than the mechanical boundary. Retain mandatory
Protect at5 and Double-Edge at7; allow either at6 and assert native recoil range,
HP/item/no-faint receipts and target damage according to the executed action.
All three cases pass: recoil-final-tests.log. The test still detects incorrect
recoil boundaries, premature Leftovers, and unaccounted HP/item changes.

Evidence: work/ability-audit-20260920/drop-isolated-tests.log,
recoil-range-trace.log, drop-final-tests.log, recoil-final-tests.log. Temporary
trace output removed. Two of the prior12 failed groups now pass in focused runs;
the other10 remain unresolved. No full117-group rerun was needed for these local
fixture-only edits; do not describe this as a new aggregate run or ROM change.

## Utility Umbrella trade costs

Two production scoring omissions in battle_ai_main.c left harmful trades tied
at100 with Scratch: giving up Dry Skin's sun protection, and receiving Umbrella
while the user's own ability benefits from active weather. Add the existing
-WEAK_EFFECT for those costs. Preserve all positive reward branches and leave
Umbrella-for-Umbrella exchanges exempt from the lost-protection penalty. Native
GetAttackerWeather/IsBattlerWeatherAffected, end-turn ability handlers and speed
calculation confirm the relevant weather suppression. No team, knowledge-profile
or scoring-constant changes; no selected human command is inspected.

Both unchanged existing test groups pass all20 cases (9gifting/11receiving).
Full five-file117-group run:109pass/8fail, no new failed groups versus the preceding
105pass/12fail run. That aggregate also includes the previously focused stat-drop
and recoil fixture corrections. Evidence: umbrella-gift-tests.log,
umbrella-steal-tests.log, umbrella-regression-tests.log, umbrella-comparison.json
under work/ability-audit-20260920/. The remaining Harvest/orb score ties are not
silently converted to passes; tradeoffs must be traced before changing them.

Related source-review findings remain open: gifting Umbrella to opposing Dry Skin
in sun protects that foe yet is neutral; removing an opposing weather-benefit
holder's Umbrella can reactivate that foe's ability while receiving a positive
reward. Add focused cases and assess competing costs/benefits before extending
the rule. These are pre-existing gaps, not resolved by the narrow patch.

Integrated release evidence: umbrella-release-build.log and
umbrella-release-gates.log. Broader game/core/campaign acceptance remains pending.

## Utility Umbrella recipient-side follow-through

Added native score comparisons for gifting protection to opposing Dry Skin in
sun and stealing the Umbrella that suppresses opposing Chlorophyll/Swift Swim.
Before correction, gifting scored100 versus Scratch100; theft scored101 versus
Scratch100. Both new groups failed. The gift regression includes Air Lock, where
no active weather means no weather cost and the scores remain equal. These tests
compare a specific target's score without forcing an unrelated target choice.

The offered-Umbrella branch now charges existing-WEAK_EFFECT when giving opposing
Dry Skin sun protection. The receiving branch charges that same existing cost
when the exchange would restore the opponent's beneficial weather ability, using
the shared ability/weather predicate. Previous positive Dry Skin-sun branches
and all20 older Umbrella cases remain intact. Removed one obsolete commented
raid-condition fragment beside the edited item switch. No team, information
permission, selected-command access or scoring-constant change.

All four new parameter cases pass. Full119-group run:111pass/8fail; exact same
remaining failure set as prior117-group run (109pass/8fail). Evidence under
work/ability-audit-20260920/: umbrella-seams-baseline-tests.log,
umbrella-seams-tests.log, umbrella-seams-comparison.json and strict release logs
umbrella-seams-release-build.log / umbrella-seams-release-gates.log.

Related scan found an older offered-orb omission to investigate next: non-Klutz
Flame Orb/Toxic Orb shedding rewards inspect whether the giver needs its orb but
can omit whether the recipient benefits from burn/poison. Add native regressions
with Guts/Poison Heal recipients before changing that scoring. The remaining
eight groups and full core/campaign/wild-distribution coverage stay open.

## Offered status-orb ownership and recipient benefits

New eight-case matrix covers Flame/Toxic Orb × Trick/Bestow × ordinary or
status-benefiting recipient. The giver already has its status, making its orb
redundant. Baseline native scoring rewarded giving Flame Orb to Guts102 versus
Scratch100; the regression failed. Offered orb scoring now uses existing self-
status-benefit queries on both prospective holders: penalize empowering the foe
or giving up one's own still-needed orb, otherwise preserve the existing shedding
reward. Existing Klutz Flame Orb handling stays separately gated for harmful
recipient burn. No new scoring constants, team edits or knowledge changes.

Self/self queries match native TryToxicOrb/TryFlameOrb: use the holder's own
ability and immunities (including Corrosion), not the former giver's. Recipient
benefit only matters while status can still be inflicted; existing status remains
after transfer, so an already activated beneficial orb can be shed. Partner-
targeted viability uses its separate existing branch and is not inverted here.

All eight new cases and all six older orb cases pass; the older expectations
were unchanged. Full120-group run:113pass/7fail, no new failed groups; original
orb group now passes. Evidence in work/ability-audit-20260920/:
orb-baseline-tests.log, orb-fix-tests.log, orb-existing-tests.log,
orb-regression-tests.log, orb-comparison.json, orb-release-build.log and
orb-release-gates.log. Seven remaining failures and broader campaign/core/wild
coverage are still incomplete. No literal perfect-game or full-balance claim.

## Contrary mixed-stat support

Improved native EXPECT_MOVE(S) failure diagnostics to include the actual target.
The old Spicy Extract regression then proved the move targeted its physical
Contrary ally, not a potentially useful opposing recipient. Contrary's generic
stat-drop activation reward returned early before the normal per-effect ally
scorer could reject reversed Attack loss.

For moves with both target-applied raises and drops, consult the existing
GetAllyStatChangeScore before granting that shortcut. Reject a nonbeneficial
mixed change with the existing-10 partner penalty. Pure lowering activations
retain their path. MoveHasAdditionalEffect excludes self effects, so Superpower,
Hammer Arm and other self-lowering attacks do not accidentally enter this gate.
The helper is evaluated once on this returning branch, with its existing Haze
RNG/status/KO checks; no persistent stat mutation was found.

Original Good as Gold/physical Contrary regression passes unchanged. Added useful
special-only Contrary Spicy Extract control and pure Charm reward control pass.
Charm can reasonably target a foe instead, so that control asserts positive ally
score rather than demanding the globally selected recipient. No teams, scoring
constants or information permissions changed.

Full121-group run:115pass/6fail; no new failures versus113pass/7fail. Evidence:
spicy-target-tests.log, contrary-negative-tests.log, contrary-regression-tests.log,
contrary-comparison.json, contrary-release-build.log and contrary-release-gates.log
under work/ability-audit-20260920/. The intermediate positive-control log records
an overconstrained Charm-target assertion that was corrected before the full run.

Related source-only finding remains: AI_CheckBadMove blanket-penalizes stat drops
against Contrary foes, including mixed changes. It may skip a useful reversed
Attack drop; requires its own native tradeoff regression before changing. Full
core/campaign/wild-distribution coverage and the remaining six groups are open.

## Harvest and After You fixture contracts

No production changes in this pass. Harvest's negative control demanded Scratch
strictly outrank Trick when the foe held Leftovers, although the helper correctly
awarded no Harvest bonus and the legal moves tied at100. Stealing Leftovers is
not inherently harmful. Retain the positive berry-backed Trick expectation; test
the neutral-score contract for Leftovers and add an empty-item control. All three
cases pass in harvest-fixture-tests.log.

After You's former Moonblast negative control put the setter atSpeed5, partner3
and foes4, so advancing the ally was genuinely useful. Preserve that board as a
positive damaging-move control, retain the original Trick Room positive, and
make the negative control's foesSpeed1 so the partner already moves ahead of
them. All three cases pass in after-you-fixture-tests.log. No score changes or
forced knowledge of player commands were needed.

Both logs and test builds are under work/ability-audit-20260920/. Four of the
previously six failing groups remain untriaged/unresolved: Helping Hand, weather,
terrain, and Protect while partner switches. This is focused verification, not a
new aggregate121-group run or a new release build. Entire game goal remains open.

## Strict ally-target scoring and explicit support fixtures

The initial safe-HP Helping Hand neutral-score hypothesis was disproved by a
native score assertion: valid ally score90, not100. A target trace then showed
planned foe0 at score100 while the controller silently remapped execution to
ally3. LowHP had103 against both illegal foes and the ally. Root cause:
ShouldConsiderMoveForBattler excluded foes for TARGET_USER_OR_ALLY but omitted
TARGET_ALLY. Extend the existing guard to strict ally targets. Existing
BattleAI_DoAIProcessing rejection already zeros the score, and the shared scorer
returns0, so no extra abstraction or blanket target rewrite was needed. Other
target classes and score weights stay unchanged.

Retain the original lowHP Helping Hand/highHP attack expectations and add zero
Helping Hand scores against both foes. They pass. The fix exposed an unrelated
fixture premise: 'partner already chose Helping Hand' actually processed that
partner second and offered Explosion too. Make the helper commit first with
Helping Hand as its sole move. Retain all original no-status-choice and three
below-default target-score assertions; all271 parameter cases pass. Future
strongly valuable emergency-status failures should still be evaluated on their
merits, not forced below default by inflating a penalty.

Weather/terrain fixtures also had implicit player Celebrate injected by CloseTurn
when no action was specified, even after adding explicit Tackle moves. Their
expert planner legitimately had a status move to Taunt. Specify both known
Tackle sets and actual Tackle actions, preserving all eight weather and eight
terrain expected setup choices. Both groups pass without changing setup scoring.

Full121-group run now120pass/1fail: only Protect while partner switches remains.
This aggregate includes earlier Harvest/AfterYou fixture corrections. Evidence:
helping-hand-target-trace.log, field-setup-explicit-tests.log,
ally-target-regression-tests.log (intermediate new fixture failure),
ally-target-final-tests.log, ally-target-release-build.log and
ally-target-release-gates.log under work/ability-audit-20260920/. Temporary trace
removed. Prior warning that the guard alone would not zero rejected scores was
incorrect; source and native zero-score assertions establish that it does.
Full game/core/campaign/wild-distribution objective remains unfinished.

## Protect activation lookup uses the resolved partner slot

ShouldAvoidProtectingAgainstPartnerMove resolves partnerMoveIndex from committed
choice or predicted partnerMove, but its Weakness Policy effectiveness read used
the old chosen index regardless. Use the already resolved local slot for that
read. No new helper, scoring weight, team or information access is needed.

New scripted native test calculates real Tackle/Earthquake effectiveness against
Weakness Policy Pikachu and verifies: predicted EQ is independent of stale chosen
slot; once committed, the chosen move controls the check; a switching partner
provides no activation. The first fixture lacked explicit speed on all battlers,
then lacked per-battler AI cache initialization in this non-AI battle. Those setup
failures were not proof of the source defect. Corrected fixture asserts the held-
effect and both effectiveness values before querying Protect. It PASSES fixed
code (protect-slot-focused-tests.log). Reverting only the lookup makes that same
corrected fixture FAIL2vs-10 (protect-slot-old-lookup-tests.log), establishing
causality. The fixed source was restored in a finally block afterward.

The intervening122-group run passed120 groups and failed original switch-decision
plus the then-uninitialized new fixture. The corrected fixture was rerun focused;
do not represent this as a clean aggregate122-group run. Original Protect/switch
choice remains open. The test ELF from the old-lookup probe is intentionally
stale versus current source; rebuild before further native tests. Normal release
uses restored fixed source, with logs protect-slot-release-build.log and
protect-slot-release-gates.log. All evidence under work/ability-audit-20260920/.

Related exact producer/consumer mismatch found in Fake Out scoring:
battle_ai_main.c's EFFECT_FIRST_TURN_ONLY branch has four partner-order queries
that read chosenMoveIndex even when SetAllyMove has only predicted partnerMove.
Needs a native regression and guarded resolved-move handling before change.
Entire core/campaign/wild-distribution audit remains incomplete.

## Fake Out partner-move ownership

Native regression holds predicted partner action fixed while varying stale chosen
slot between Tackle and Quick Attack. After correctly initializing first-turn
state before move-limit caches, old scoring changed107to101 solely from that stale
slot. SetAllyMove already supplies the committed or simulated partner move, and
joint pair enumeration intentionally supplies NONE. Replace all FIVE stale-slot
reads in Fake Out order comparisons with one local copy of that existing cache.
Earlier scan counts of four missed a nested comparison. No additional resolver,
scoring weight, team or human-command access is introduced.

New test verifies stale-slot invariance and distinguishes the slow attack that
needs flinch protection from the priority attack that can KO first. Baseline full
run123groups:121pass/2fail; fixed run122pass/1fail, only original Protect/switch
remaining. Evidence: fakeout-slot-baseline-tests.log,
fakeout-slot-full-baseline-tests.log, fakeout-slot-tests.log and strict release
fakeout-slot-release-build.log / fakeout-slot-release-gates.log, all under
work/ability-audit-20260920/.

## Remaining Protect/switch failure: item-independent, target-isolated

A temporary no-item duplicate chose Tackle exactly like the Weakness Policy
fixture (protect-item-control-tests.log); the duplicate was then removed. The
retained fixture now uses Golurk as the switch recipient instead of Rampardos.
Ghost immunity leaves Pikachu the only effective Scratch target after switching,
using public typing and moves. Existing switch, Protect, PikachuHP50 and item
assertions remain. This isolated case STILL FAILS, so the issue cannot be dismissed
as merely a Weakness Policy penalty or an alternate-target forecast. Evidence:
protect-switch-isolated-tests.log. No aggregate rerun after this fixture-only
isolation is claimed; the original failure remains unresolved.

Next hypothesis to verify: guardDenied accumulates uncapped blocked damage and
KO credit per hit, then subtracts its unbanked share. Multiple overkill hits may
charge more temporary guard value than the body could lose. Source review/native
score tracing are required before changing it; no guard weight changed here.
Full core/campaign/wild-distribution goal remains unfinished.

Guard review clarification: PairForecastDamage already caps each individual hit
to the target's current HP. The suspected overcount is across hits: each lethal
hit can credit180health+80KO for the same body, twice yielding520. At55% unbanked,
the286 deduction exceeds that body's180 value. A per-guard global cap would break
side guards protecting distinct allies; any fix must distinguish recipients and
should consider sequential chip becoming lethal. This is source arithmetic, not
yet native causal verification or an implemented guard correction.

## Guard accounting experiment — NOT RETAINED

Native trace confirmed denied520/banked45 for one50HP Pikachu facing two lethal
hits (guard-accounting-baseline-tests.log). Each hit credits180health+80KO to the
same body, then55% is deducted as unbanked value. This explains why the original
Protect candidate loses, but changing that arithmetic interacts with authored
cadence policy.

Candidate1 capped accumulation separately per guard user and recipient, preserving
side-guard scope. Original123groups all passed, but two newly included Protect-
cadence groups regressed (129/131 overall): Trick Room no-payoff control and the
transient attack-window repeat control. Old accounting passed all eight cadence
groups in a matched source probe (guard-cadence-baseline-tests.log), proving those
were regressions, not stale tests.

Candidate2 tried applying existing empty-guard cost to no-payoff shields and
crediting a partner's switch as payoff. It still failed three groups (128/131),
including previously valid Rock Tomb/stat-drop protection. Review also found
that noActionMask may include already-acted battlers in GetBestMonDoubles, so it
cannot be relabeled switchingMask. Neither policy extension nor mask change is
retained. No cadence assertions were relaxed to accept the experiments.

Restored src/battle_ai_pair.c byte-for-byte from the pre-experiment copy. Current
release input/artifact stamp check PASSES against that restored source; the prior
verified ROM remains current. Candidate sources are archived locally in work/
ability-audit-20260920/pair-guard-cap-only.c and pair-guard-policy-experiment.c.
Logs: guard-accounting-tests.log, guard-accounting-final-tests.log and baseline
logs above. Test ELF contains an experiment and must be rebuilt before reuse.

Next design/debug step must distinguish actual prevented loss, repeated incoming
pressure, and genuine partner progress. If switch payoff is represented, carry
an explicit switching mask separately from action suppression; after-KO/after-
move candidate contexts differ. Preserve all eight original cadence boundaries
and the actual Protect/switch failure before accepting a repair. The per-target
cap alone also does not model cumulative uncertain KOs or target-survival
correlation. The guard issue remains unresolved; no production AI change from
this turn is retained. Full game goal remains active and incomplete.

## Optional AI switch legality — September 22, 2026

Compared the live switch path with pre-integration `508775fad8`. The inherited
`CanBattlerConsiderSwitch` manually checked Wrap, escape prevention and Ingrain,
but omitted Fairy Lock and Sky Drop. It now reuses the same escape, Shed Shell,
ability, Commander and Arena gates as the engine's actual switch command.
No damage estimate, move score, team, AI information or tactical ordering changed.

The inherited `IsSwitchinValid` had two identical double-battle branches and
checked only one partner collision. `AI_TrySwitchOrUseItem` emitted SWITCH before
resolving a reserve; a stale explicit or preferred index could point at the
active partner or a fainted mon. Replaced that split validation/fallback with
one resolver that checks current party eligibility and partner choice, then
emits SWITCH only for a legal slot. An invalid explicit/preferred plan falls
back to move or item selection; a generic switch retains its high-to-low
reserve preference. This changes illegal-action handling, not candidate scores.

Three focused native cases pass: Fairy Lock/Sky Drop, active or fainted stale
reserve, and a doubles partner slot. Switch-focused native runs passed 97 and
16 cases. Full `make check` passed 1,622, with nine expected failures and only
the existing Sturdy setup failure. The current release ROM SHA-256 is
`2261ca8e52a2384b17b17522ffeec2178124c7d1237ad85345f3ea1cc3d4f442`;
release build/gates and input stamp pass. Evidence is in
`work/ai-switch-legality-20260922/`. No full tactical playtest of every
authored trainer is claimed.

## Sturdy and Focus Sash setup guard — September 23, 2026

The original authored `CanBattlerKOTargetIgnoringSturdy` already contained a
guard meant to reject setup when an attack would KO before Sturdy or Focus
Sash, but its cached damage had survival applied. Native fixture evidence:
Skarmory had 271 HP, cached Fire Blast was clipped to 270, and the same move
without survival produced 386–456 damage (422 median). The old comparison
could never identify the raw KO, so Moltres chose Agility. This path matches
pre-integration source `508775fad8`; it was not caused by a new type chart.

Only for a usable move whose cached hit reaches HP−1 and `CanEndureHit` is
true, the guard now uses the existing raw single-hit simulation and the same
configured attacking roll. It leaves ordinary cached damage, team data and
AI information unchanged. The helper also protects ally stat
boosts from wasting a turn when an ally already has a raw KO through survival.

The original Sturdy test and a new Focus Sash twin pass. Full `make check` is
green: 1,627 passed, nine expected failures, zero unexpected failures out of
1,636. Release build, release gates and content stamp pass for ROM SHA-256
`089f74a248b8ffca598aa334a2622630dd52f623453262db8bbf3d11a68d8cd5`.
Evidence is in `work/sturdy-diagnosis-20260922/`; the temporary diagnostic
expectation and print calls were removed from source after measuring the bug.
The full authored campaign's tactical difficulty remains unverified.
