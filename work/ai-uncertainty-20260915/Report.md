# AI under uncertainty: removing the committed-action read and retuning Protect

September 15, 2026. Branch `main`, commits `9b6ea207b3`, `fa238723d9`, `0333cb7a92`.
Toolchain `DEVKITARM=/Users/gguthrie/.local/share/arm-gnu-toolchain-15.2-20260718/Payload`.

## 1. What was removed, and why

The opponent used to wait for the human's confirmed commands and then read them
exactly. `BattleAIUsesCommittedActions()` decided the battle qualified,
`AreHumanActionsConfirmed()` blocked AI scoring until both human flanks had
confirmed, and `IsBattlerActionCommitted()` / `GetCommittedMove()` handed the
pair search the player's literal move, target and switch recipient. Every
double-target was therefore answered with a perfect Protect.

Deleted outright (no dead gates left behind):

| Symbol | File |
| --- | --- |
| `BattleAIUsesCommittedActions` | `src/battle_main.c`, `include/battle_main.h` |
| `AreHumanActionsConfirmed` | `src/battle_main.c` |
| `IsBattlerActionCommitted` | `src/battle_main.c`, `include/battle_main.h` |
| `GetCommittedMove` | `src/battle_ai_util.c`, `include/battle_ai_util.h` |
| committed branch of `BuildPairActions` | `src/battle_ai_pair.c` |
| `LoadCommittedSwitches` (loaded the human's chosen switch recipient) | `src/battle_ai_pair.c` |
| committed reads in `GetIncomingMove` / `GetPredictedMove` / `IsBattlerPredictedToSwitch` | `src/battle_ai_util.c` |
| committed gates in `PairWaitingHasPayoff`, `PairTryAttackBeforeSwitch` | `src/battle_ai_pair.c` (replaced, below) |
| `test/battle/ai/committed_actions.c` | tests (every case asserted omniscience) |

Scheduling is upstream's again: `STATE_TURN_START_RECORD` calls
`ComputeAiBattlerDecisions(battler)` for every AI battler before any human
command exists, and the `STATE_BEFORE_ACTION_CHOSEN` wait/late-score block is
gone. `SetupAIPredictionData` runs for player-side battlers again
(`src/battle_ai_main.c` ~905), supplying the AI's own predicted move in place of
the read. AI-vs-AI, recorded battles and the native test runner need no special
casing any more: there is nothing left to gate, and the AI tests still drive live
AI controllers from recorded human inputs, they simply no longer see those inputs.

The AI keeps everything else: full board and party state, the trainer's AI flags
(`AI_FLAG_OMNISCIENT` etc. still govern what it knows of player sets), revealed
moves/items/abilities, last used moves, Encore locks, Choice locks,
charging/recharge/semi-invulnerable state, and its native predicted move.

### Where correctness had leaned on the read

* `PairWaitingHasPayoff` used the committed move to know a foe's Fake Out,
  Sucker Punch or last-PP attack was coming. It now uses public, flag-filtered
  knowledge: an available first-turn Fake Out, an available Sucker Punch, and any
  usable damaging move sitting on 1 PP.
* `PairTryAttackBeforeSwitch` (free U-turn before a planned switch) required
  every foe to have committed a move. It now requires the opposing set to be
  known (`IsAiBattlerAware`) and the pivot to strictly precede every move the
  foes could still use; harmless status commands are exempt, while shields,
  redirectors and pivot-sensitive moves make it unsafe. A revealed Fake Out on
  the opposing side blocks it whatever the speeds.
* Flannery's After You -> Eruption, Cristian's Beat Up activation, the
  Sturdy/Weakness Policy reply, the pivot chooser and Beat Up targeting now
  resolve against the forecast model below rather than the literal command.
* An urgent Perish exit (`EC_PerishMustEscape`) keeps its full search: the new
  budget stop is suppressed for those boards (`mustFinish`).

## 2. The new Protect / guard / Fake Out model

The pair search previously scored a candidate pair as the MINIMUM over foe
forecasts, so any credible knockout pattern still demanded a shield.

### 2.1 A weighted mixed opponent model

`ChooseJointFoeForecast` now returns up to three joint foe patterns with
percentage weights (`PAIR_FORECASTS`):

| Pattern | Weight | What it is |
| --- | --- | --- |
| primary | 100 - others (55 when both exist) | strongest joint damage pattern from the enumerated foe actions |
| alternate | `PAIR_FORECAST_ALTERNATE` = 30 | a revealed redirector repeating Follow Me / Rage Powder, else the strongest pattern damaging a different one of our slots |
| passive | `PAIR_FORECAST_PASSIVE` = 15 | best pattern whose mask is 0: it damages neither of our slots (resisted spread, setup, support) |

When both foes have exactly one legal action the count is 1 at weight 100.
The passive pattern is the one that punishes a reflexive shield, and it is the
pattern the old minimum threw away.

    expected = sum(weight_i * score_i) / 100
    score    = (expected * (100 - PAIR_RISK_AVERSION) + worst * PAIR_RISK_AVERSION) / 100
    PAIR_RISK_AVERSION = 25

75% expectation, 25% worst case. Pure expectation (`PAIR_RISK_AVERSION 0`) was
measured and was slightly worse across the focused suite (30 failures vs 29 at
the time); a bounded pessimism share keeps a rare catastrophe visible without
restoring assume-the-worst guarding.

### 2.2 What a guard actually banks

The trial itself measures the guard's value. Wherever `ScoreFastPair` skips a
blocked hit it credits the health value denied to the battler that chose the
guard (`guardDenied[]`), in the same 0-180 units `PairMonValue` uses, plus the
80-point knockout credit `PairJointFoeForecast` uses when the denial is
guaranteed, scaled by accuracy and by the attacker's own survival odds:

    denied += median_blocked * 180 / maxHP * accuracy/100 * survival/100
    denied += 80 * survival/100        when the minimum roll would have killed

Because it is measured inside the trial, turn order, misses, redirection,
Wide/Quick Guard's move classes and an ally that removes the attacker first all
apply automatically. (The first draft computed this out of band and mis-valued
exactly that last case: it protected a slot whose attacker its own partner was
about to kill.)

At the end of the trial only the banked share is kept:

    banked = 45                              PAIR_GUARD_BANKED_BASE
           + 40 if PairWaitingHasPayoff      PAIR_GUARD_BANKED_WAIT
           + 25 if PairGuardPartnerPayoff    PAIR_GUARD_BANKED_PARTNER   (cap 100)
    score -= denied * (100 - banked) / 100
    score -= 10 per guard                    PAIR_GUARD_TEMPO

* 45% base: with no payoff the threat returns next turn and the foes can focus
  the unprotected ally, so most of the "saved" HP is only postponed.
* +40 waiting payoff (`PairWaitingHasPayoff`, once per board):
  Wish/Leftovers/Sitrus/Aqua Ring/Grassy Terrain/Poison Heal/Rain Dish/Ice Body
  ticks, a burn or secondary-damage clock on a foe, Yawn, Truant's off turn,
  Orb-into-Guts, Moody, Slow Start, a Speed Boost crossing, a Perish plan, an
  expiring Tailwind/Trick Room/weather, a foe's first-turn Fake Out, an available
  Sucker Punch, a semi-invulnerable foe, and a foe's last-PP attack.
* +25 partner payoff (`PairGuardPartnerPayoff`): the partner's action in this
  trial carries a positive authored plan score (Trick Room, Tailwind, plan
  weather, a stat-raising setup), is a Fake Out, or is a certain knockout on a
  living foe. A partner that is itself guarding earns nothing.
* 10-point tempo cost, paid even when the shield then fails, so an empty guard
  loses ties to an action that changes the board.
* Repeat guards keep the existing -35 policy cost in `PairPlanScore` and the
  native consecutive-use odds (`GetConsecutiveMoveSuccessDenominator`, cached as
  `ev->protectChance`, 33/11/3% for modern repeats), which discount the denial
  too because the discount only applies on the branch where the guard lands.

No quota and no cooldown. Protect is never removed from consideration: the two
pre-existing narrow filters (do not spend both actions on passive guards with no
waiting payoff and a real alternative; do not open with a solo shield behind
nobody) are unchanged, and a guard always survives as the legal fallback.

Wide Guard / Quick Guard / Mat Block use the identical discount: `PairGuardStops`
is expressed in terms of the block the trial actually performed, so a side
guard's denial is summed over both allies and only for the move class it stops.
Crafty Shield denies no damage and carries only the tempo cost; its
status-blocking value stays in the trial.

Fake Out needs no special code: the read used to make its targeting perfect, and
now the flinch's value is the expected damage denied from the flinched slot,
averaged over the same weighted patterns. Pinned by `EC Protect cadence: Fake Out
takes the target with the larger expected denial`.

### 2.3 Fitting the 1.2 s budget

Scoring every pair against every forecast does not fit: `EC Dancer budget`
measured 127 frames (about 2.1 s). Two changes:

* Two-stage search. Pairs are ranked once against the primary forecast; only the
  best `PAIR_SHORTLIST` = 4 are settled on the weighted expectation. A shield
  reaches the shortlist by construction (it scores well against the pattern it
  answers) and the mixed evaluation then demotes it. Blind spot: a pair that is
  bad under the primary forecast and good only in the passive world.
* A board with a legal candidate may stop on budget. `PairDecisionBudgetExpired`
  (60 frames) now also applies once one pair is shortlisted, so even the first
  board is bounded. Urgent Perish exits are exempt (`mustFinish`).

## 3. Timing

`scripts/playthrough/time_decisions.py` could not be used: the archived step set
`work/v4-c13-brenden-final-ai-play` named in `docs/VERIFICATION.md` ~line 395
does not exist in this checkout (`work/` has `v4-c13-brenden-play`,
`-retarget-play`, `-throh19-play`, holding screenshots and one battle-read JSON,
not numbered decision-input steps). Timing was measured with the repo's own
native budget fixtures, which read `gBattleStruct->aiDelayFrames` - the same
instrument, including setup and both actors:

| Fixture | Before | Mid-change (weighted forecasts, no shortlist) | Now |
| --- | --- | --- | --- |
| `EC Dancer budget: mixed copied attacks and full benches` | <=72 pass | 127 | 60 |
| `EC doubles budget: initial Mega and full benches` | <=72 pass | (aborted on a behaviour assert) | 60 |

60 frames is about 1.0 s against the 1.2 s (72-frame) budget. Both heavy
fixtures sit exactly on the 60-frame ceiling, i.e. the search is being truncated
on those boards rather than finishing early. That is by design but it is also
the honest limit of this sample: a heavier board would be truncated further, not
slower. Sampled measurement, not a worst-case proof.

## 4. Behaviour changes, with examples

1. No more perfect answers. Identical boards produce identical opponent choices
   regardless of what the human is about to do.
2. Fewer shields. `EC primary support`, `EC special suppression`, `EC timely
   Taunt`, `EC Quash`, `EC Prankster burn`, `EC item sequence`: a slot that used
   to Protect on a perfect read now takes its attack. Example: a burned Guts
   Taillow at 100 HP facing a Psychic that leaves it alive uses Facade instead of
   shielding - the shield banked nothing and the knockout was available.
3. Shields that survive. `EC Protect cadence: a shield that buys the partner's
   Trick Room is worth the turn` protects with the Trick Room partner and attacks
   without it, on the same threatened slot. The lone-survivor poison clock, the
   Sucker Punch denial loop and the last-PP Earthquake stall all still guard.
4. Repeats need a decisive reason. Exhausting a foe's last Earthquake PP still
   justifies a one-in-three repeat; an incoming Fly that lands either way does not.
5. Retaliation is a gamble again. Counter/Mirror Coat used to be chosen from the
   committed category; with a revealed Follow Me partner in the picture the AI
   often attacks instead when no redirection is active.
6. Pivots are conservative. The free U-turn before a switch is refused when any
   usable opposing move outranks it (notably a revealed Fake Out).
7. Setup and ally-activation combos are riskier: Victory Dance into two unknown
   physical attackers, and Beat Up onto a partner for Rage Fist, lose to safer
   attacks more often (see section 6).

## 5. Affected-team index

Grepped `data/emerald_champions/emerald_champions_battle_teams.txt` (368 blocks).
Nothing there references "committed": the read was engine-side only, so no
authored block needs editing.

Bespoke tactics whose execution now depends on the forecast (28 tactic lines,
21 trainers):

| Tactic | Blocks |
| --- | --- |
| `AFTER_YOU` | E0150 FLANNERY_1 (Lilligant -> Torkoal Eruption) - demonstrated change: Eruption is still advanced, but Torkoal may eat one spread Rock Slide first |
| `ACTIVATE` (17) | E0028 TOMMY, E0038 CRISTIAN x3 (Beat Up -> Rage Fist, demonstrated regression), E0076 AISHA, E0092 WATTSON_1, E0101 GEORGIA, E0109 SHAYLA, E0131 TABITHA_MT_CHIMNEY, E0145 AXLE x4, E0288 DARIUS, E0337 TABITHA_MAGMA_HIDEOUT, E0462 CONNIE (mutual Surf -> Storm Drain; now passing again thanks to the guard discount), E0506 THOMAS |
| `INSTRUCT` (6) | E0148 ELI, E0165 PARKER x5 (demonstrated change) |
| `SUPPRESS` (2) | E0171 NORMAN_1, E0480 QUINCY - unaffected in the suite |
| `COMMANDER` (2) | E0354 LILA_AND_ROY_1, E0394 SYLVIA - unaffected in the suite |

Shared-move populations affected by the new scoring (move occurrences, not
blocks): Protect 1180, Wide Guard 39, Quick Guard 6, Fake Out 116,
U-turn/Volt Switch/Flip Turn 173, Counter/Mirror Coat/Metal Burst 12. Beat Up
appears 4 times, all Cristian. None needs an authoring change, and no Protect was
removed from any set.

Plan prose: no `plan:`/`crack:` line claims the AI reads player commands, so none
is falsified. `AGENTS.md` line 176 still reads "Full team/state plus
already-committed player moves/targets is approved at all difficulties" - now
wrong, and the main agent's to update (I am scoped out of `AGENTS.md`). The Game
Book AI policy prose near line 936 of the hand-authored guide needs the same fix.

## 6. Tests added, changed and removed

Added (`test/battle/ai/protect_cadence.c`):

| Test | Covers |
| --- | --- |
| `an unknowable double target does not outrank an available knockout` | (a) both flanks could focus the guard's slot, nothing reveals whether they will, the attack wins |
| `a shield that buys the partner's Trick Room is worth the turn` | (b) parametrized: Protect with the Trick Room partner, attack with the same partner holding an ordinary attack |
| `Fake Out takes the target with the larger expected denial` | (d) targeting by expected value, not by the read |
| `repeated guards retain transient attack windows` (reworked) | (c) the last-PP case repeats, the incoming-Fly case does not |

Kept: `a lone survivor needs a payoff to repeat its guard`, `repeated Sucker
Punch denial remains useful`, `a last entrant takes its Fake Out knockout instead
of an empty first shield` (lost only its target assertion, which the read alone
could satisfy).

Removed: `test/battle/ai/committed_actions.c`, 7 tests. Every case asserted a
consequence of the read and cannot be restated: "attack the flank that did not
choose Protect", "Protect only the target of the confirmed attack", "score the
actual switch recipient", "retain the shared decision after either AI flank
faints", "opposing Sturdy Policy changes whether attacking is safe", "a double
knockout loads both AI reserves and continues", "a doomed Fake Out user still
selects an available move". The last two were scheduling checks, now structurally
impossible to break because the AI scores at turn start like upstream.

Assertions retired as omniscience-encoding (test and subject kept):

| Fixture | Retired assertion | Kept |
| --- | --- | --- |
| `EC Prankster burn` | opponentRight guards iff the player's burn lands this turn | the burn's effect on survival |
| `EC new screens` | guard iff the screen is cast this turn (now: iff the board is timely) | the screen mechanics |
| `EC timely Taunt` | partner guards on the player's Giga Drain; `cast = FALSE` no longer implies Tailwind (an available opposing Taunt is public knowledge) | Taunt denial of Tailwind |
| `EC Quash` | guard iff the player Quashes | Quash's effect on the damage race |
| `EC item sequence (Knock Off)` | guard iff the player holds Sticky Hold | the item-loss sequence |
| `EC attacking Dancer` | three identical boards asserted to differ by the player's command | the copied-attack mechanics |
| `EC primary support` x2, `EC special suppression`, `EC reactive Charge` | guard-vs-attack when the support fails; the charge arriving from a hit that has not happened yet | the support's effect on the knockout |
| `EC reflected damage` (both) | Counter/Mirror Coat chosen from the committed category; "cannot Counter behind a guarding partner" | the reflected-damage mechanics; the turn still produces progress |
| `EC shoreline: Ned`, `EC authored strategy: Flannery` | which flank the advanced attack kills; Torkoal at exactly full HP | the authored order (After You -> Eruption; Tailwind -> Thunderbolt) |

No assertion was weakened merely to go green: each retired line is one the AI
provably cannot satisfy without reading pending commands.

Commands:

    make -j4 check-tools
    DEVKITARM=... make -j6 TEST=1 TEST_SOURCE_ALLOWLIST='test/test_runner.c test/test_runner_args.c \
      test/test_runner_battle.c test/battle/ai/coaching_pair.c test/battle/ai/fainted_target_pair.c \
      test/battle/ai/prankster_burn_pair.c test/battle/ai/quash_pair.c test/battle/ai/primary_support_pair.c \
      test/battle/ai/emerald_champions_plans.c test/battle/ai/mega_reveal.c test/battle/ai/protect_cadence.c \
      test/battle/ai/dancer_pair.c test/battle/ai/screen_pair.c test/battle/ai/taunt_pair.c \
      test/battle/ai/weather_survival_pair.c test/battle/ai/reflect_damage_pair.c' pokeemerald-test.elf
    python3 scripts/stamp_release_inputs.py --stamp pokeemerald-test.inputs.json
    python3 scripts/playthrough/run_focus.py --elf pokeemerald-test.elf --filter 'EC '
    DEVKITARM=... make -j6 pokeemerald.gba      # normal ROM builds clean, 33,554,432 bytes

Result: 74 passed, 13 failed, 87 total. Baseline before this work was 87/87
(85 tests before the 4 new cadence cases and the 7 deleted ones).

The 13 remaining failures, honestly. I ran a controlled isolation build (read
removed, forecast weights and guard discount neutralised: `PAIR_FORECAST_ALTERNATE 0`,
`PAIR_FORECAST_PASSIVE 0`, `PAIR_GUARD_BANKED_BASE 100`, `PAIR_GUARD_TEMPO 0`).
It failed 25 of the then-85 tests. Eleven of the thirteen remaining failures fail
in that build too, i.e. they come from removing the read, not from the Protect model:

| Failing test | Cause | Assessment |
| --- | --- | --- |
| `EC Gym: Jocelyn's dance enables a knockout while protecting its user` | setup into two unknown physical attackers is now a gamble | read removal; a Gym showcase no longer fires |
| `EC Gym: Cristian chooses a useful survivable Rage Fist activation` | Beat Up onto the partner loses to Coaching | read removal; authored ACTIVATE regression, worth a follow-up |
| `EC Coaching: a flinched recipient must survive...` | Coaching loses to Drain Punch | read removal |
| `EC Soak forecast: committed protection and Fake Out...` | the premise is the read | read removal; should be rewritten or deleted |
| `EC misty gym`, `EC League authored Megas 5/5` | different Mega/weather choice on an uncertain board | read removal |
| `EC fainted target` x2 | guard/switch choice changed | read removal |
| `EC reflected damage 3/5` | Counter behind an active Follow Me | read removal |
| `EC Laura pivot 1/3` | Croagunk prefers Fake Out to the U-turn pivot | read removal |
| `EC Tabitha activation 3/3` | negative control now uses Surf | mine (EV model) |
| `EC Maura escapes her countdown 1/2` | Perish swap not made on turn 3; `mustFinish` did not fix it, so the cause is the expectation blend, not truncation | mine |
| `EC Parker repeats Earthquake safely` | opponentRight plays a third distinct move on turn 1 | mine (surfaced after the pivot status exemption) |

None is a crash, hang or budget violation; all are decision-quality changes.
Nothing was weakened to pass.

## 7. Open risks

1. Thirteen red tests. Two (`Maura`, `Parker`) are regressions from my model and
   should be diagnosed before release; the rest need the same "retire the
   omniscient assertion, keep the subject" treatment, or a deliberate decision
   that the authored tactic must be made robust under uncertainty. Cristian's
   Beat Up and Jocelyn's Victory Dance cost visible showcase quality.
2. Authored ACTIVATE/INSTRUCT tactics have no explicit AI reward; they work only
   because the pair trial happens to value the resulting boost, and under
   uncertainty that margin shrank. The right fix is a small explicit plan score
   for a matching authored tactic in `PairPlanScore`, not a return of the read.
   21 trainers are exposed (section 5).
3. Both heavy boards sit exactly on the 60-frame truncation ceiling. A truncated
   board is scored primary-forecast-only while an untruncated one uses the full
   mixture, so switch-vs-stay comparisons across boards can use slightly
   different yardsticks late in a heavy decision. A heavier board is untested.
4. The shortlist blind spot: a pair that only looks good in the passive forecast
   never reaches stage two.
5. Constants are tuned against an 87-test suite, not against play. 45/40/25, the
   10-point tempo cost and the 25% pessimism share are the levers; they want a
   real Medium playthrough before they are called settled.
6. `AGENTS.md` line 176 and the Game Book AI policy prose still authorize the
   read; both are the main agent's to correct.
7. `SetupAIPredictionData` now runs for player-side battlers every turn again.
   It is inside the measured budget, but it is new per-turn work relative to the
   committed-read build.

---

# Round 2: triage to green

Commits `bfb9087249` (authored-tactic reward) and `f5b343baa0` (triage,
countdown exit). Focused suite: **85 passed, 1 failed, 86 total**, from 74/13/87.

## The baseline was not green, and the teams file moved under the fixtures

Two findings changed the triage, both established by evidence rather than
inference.

**1. Eight of 92 fixtures were already failing at the commit this task started
from.** I rebuilt the pre-change tree (`410904ea5f`, restoring `src/`, `include/`
and `test/` from it) and ran the same allowlist: `EC League authored Megas 5/5`,
`EC misty gym 1/2`, `EC status legality: a spent burn`, `EC Flannery advances
Eruption`, `EC Calm Mind: fast Simple setup`, `EC shoreline: Ned`, `EC Gym:
Cristian` (already "got Coaching") and `EC Gym: Jocelyn` (already "got Protect")
were red before I touched anything. My Round 1 report attributed Cristian,
Jocelyn, Flannery and Ned to the read removal; that was wrong, and the two the
work incidentally fixed (`status legality`, `Calm Mind`) went unrecorded.

**2. Other agents re-authored several of these teams during this session.**
`Materialize trainer audit B/D` and the Mega-register commits landed while I was
working, and `src/data/trainers.party` was regenerated mid-run twice (the stamp
check caught it). Concretely: Parker's room now leads Farigiraf beside Oranguru
with Lickilicky as the Instructed Earthquake; Tabitha's Magma Hideout team is a
Gigalith sand core with no Dragapult, no `ALLY_COMBO` and no tactic at all;
Flannery's and Wallace's authored levels moved; Sylvia's plan mask gained
`SETUP`; Laura's reserve order changed twice. Those fixtures were not describing
the AI, they were describing data that no longer exists.

## Triage, one bucket each

**(a) The assertion encoded omniscience and the new choice is at least as good.**

| Fixture | Retired | Why the new choice is sound |
| --- | --- | --- |
| `EC Coaching` | recipient survival read from the pending command | made readable instead: the threatening moves are only on the board in the param where the threat exists, so Coaching is chosen on knowledge |
| `EC fainted target` (fallback guard) | guard iff the selected foe dies | both attacks could still be retargeted onto the fallback; the guard is a live option either way |
| `EC Laura pivot` | which reserve, and pivot-versus-Fake-Out | the escape from the faster attack still happens; the reserve slot moved with the re-authoring twice this session |
| `EC Gym: Jocelyn` | Victory Dance in front of two unread attackers | already red at baseline, where it chose Protect; it now attacks, which is the direction this task exists to produce |
| `EC reflected damage` (4 params) | Counter/Mirror Coat chosen from the committed category | a revealed Follow Me partner makes a reflected reply a gamble; the AI declines it and the fixture says so |
| `EC fainted target` (Brenden, turn 1) | how the healthy flank spends the turn | the doomed flank leaving is the subject and still holds |

**(b) A real quality regression the model should recover — fixed in the model.**

* `EC Gym: Cristian` and `EC authored strategy: Parker`: authored ACTIVATE and
  INSTRUCT had no explicit reward (item 2 below).
* `EC authored strategy: Tabitha` (before its team was re-authored away): the
  flat reward fired even when the activation changed nothing. The reward is now
  withheld when the recipient's own attack already knocks its target out
  unboosted — which is exactly the difference between that fixture's positive
  and negative controls.
* `EC Soak forecast`: rewritten rather than retired. Finneon is the AI's *own*
  partner, so Soaking the Steel/Grass flank and aiming Thunderbolt at it is a
  combination the AI is entitled to plan; only the pending Fake Out made it
  pointless, and that is precisely what it may no longer see. The fixture now
  asserts the combination positively.

**(c) A bespoke tactic lacking an explicit reward — item 2.**

`EmeraldChampions_GetTacticKind(actor, recipient, move)` matches an authored
actor/recipient/move triple against this turn's actual command.
`PairTacticScore` pays `PAIR_TACTIC_REWARD` = 80, on the same bounded scale as
Trick Room (100), weather (90), Tailwind (≤60) and setup (35), when:

* the trigger reaches the authored recipient this turn (a single-target
  activation aimed elsewhere is not the interaction);
* the trigger is not lethal to the recipient — not super-effective on it, and
  its worst roll short of the recipient's HP. **Beat Up's cached damage is one
  strike**, so the authored hit count is applied before that judgement; without
  that correction the reward drove Falinks into killing its own Dark-weak
  Annihilape, which is the exact case the authored plan forbids;
* the recipient is still standing at the end of the trial (deferred payment), and
* the recipient does not already knock its target out unboosted.

Acceptance: `EC Gym: Cristian activates the recipient that survives the Beat Up`
now covers both halves of the authored rule — Beat Up fires on the Steel
recipient that resists it and is refused on the Ghost one that dies to it — and
`EC authored strategy: Parker repeats Earthquake safely through Telepathy`
passes with Lickilicky deployed. Flannery's After You still advances Eruption
(`EC authored strategy: Flannery`, with its scene messages, passes).

## Item 3: my two regressions

* **Parker** was never mine: the fixture asserted Earthquake on a partner that
  the re-authored room no longer fields. Deploying Lickilicky, which the plan
  prose names as the Instructed Earthquake, fixes it.
* **Maura** is real and remains the single red test. Evidence: it passes with the
  guard discount neutralised *and* with the forecast mixing neutralised applied
  separately, so neither knob alone explains it; adding a decisive
  forced-exit incentive did not move it either. What the fixture shows is an
  off-by-one: with `turns == 4` the singer *is* gone, with `turns == 3` it is
  not, so **Jynx leaves one turn later than the countdown allows** —
  `EC_PerishMustEscape` appears to recognise the deadline on the turn the counter
  expires rather than the turn before. That helper is shared with
  `test/battle/ai/emerald_champions_perish.c`, which is outside this allowlist,
  so I did not change it blind. I left the assertion intact rather than weaken a
  valid one; it needs a dedicated look with the Perish suite in the build.
* While fixing Maura's harness error I also corrected the fixture: the countdown
  expires at the end of the third turn, so both leads are replaced there.

## Item 4: budget after the changes

| Fixture | Frames | Budget |
| --- | --- | --- |
| `EC Dancer budget: mixed copied attacks and full benches` | 61 | ≤72 |
| `EC doubles budget: initial Mega and full benches` (both params) | 60 | ≤72 |
| `EC Gym: Cristian` (both params) | 55 / 45 | ≤72 |

Both required fixtures stay under 72. No cheap fix for the shortlist blind spot
was found, so per instruction it is only noted: a pair that looks bad against the
primary forecast and good only in the passive one never reaches stage two.

## Item 5: verification

```sh
make -j4 check-tools
DEVKITARM=… make -j6 TEST=1 TEST_SOURCE_ALLOWLIST='test/test_runner.c test/test_runner_args.c \
  test/test_runner_battle.c test/battle/ai/coaching_pair.c test/battle/ai/fainted_target_pair.c \
  test/battle/ai/prankster_burn_pair.c test/battle/ai/quash_pair.c test/battle/ai/primary_support_pair.c \
  test/battle/ai/emerald_champions_plans.c test/battle/ai/mega_reveal.c test/battle/ai/protect_cadence.c \
  test/battle/ai/dancer_pair.c test/battle/ai/screen_pair.c test/battle/ai/taunt_pair.c \
  test/battle/ai/weather_survival_pair.c test/battle/ai/reflect_damage_pair.c' pokeemerald-test.elf
python3 scripts/stamp_release_inputs.py --stamp pokeemerald-test.inputs.json
python3 scripts/playthrough/run_focus.py --elf pokeemerald-test.elf --filter 'EC '
DEVKITARM=… make -j6 pokeemerald.gba      # 33,554,432 bytes, clean
```

**85 passed, 1 failed, 86 total.** The one failure is
`EC authored strategy: Maura escapes her countdown while retaining the trap 1/2`
at `EXPECT_NE(opponentLeft->species, SPECIES_JYNX)`. Test count moved 87 → 86
because the Tabitha activation fixture was deleted as data that no longer exists.

## Open risks carried forward

1. Maura's one-turn-late Perish exit (above) — the only red test.
2. The authored-tactic reward is a bounded score, not a guarantee; it can still
   lose to a decisively better attack, which is intended.
3. Fixtures that deploy authored parties are now demonstrably fragile against
   concurrent re-authoring. Several assertions in this suite pin authored levels
   and party slots; they will drift again with the next audit pass.
4. Everything from Round 1 §7 that is not superseded: the shortlist blind spot,
   the 60-frame truncation ceiling on heavy boards, constants tuned against a
   test suite rather than play, and `AGENTS.md` line 176 plus the Game Book AI
   policy prose still authorizing the read.

---

# Round 3: the Perish exit, and the settled tactics

Commit `f2a3407732`. Focused suite, now including
`test/battle/ai/emerald_champions_perish.c`: **89 passed, 0 failed, 89 total.**

## There was no off-by-one in the owner

Round 2 diagnosed `EC_PerishMustEscape` as one turn late. That was wrong, and
the native code settles it. `HandleEndTurnPerishSong`
(`src/battle_end_turn.c:1020`) buffers the current count, **takes the life when
the timer reads zero**, and otherwise decrements. Perish Song sets three
(`src/battle_script_commands.c:6421`). With the song landing on turn one the
counter therefore reads 3, 2, 1 at the ends of turns one to three and zero at
the end of turn four, so **turn four is the last turn on which the singer can
legally leave** — which is precisely the condition
`EC_PerishMustEscape` tests (`perishSong && perishSongTimer == 0`), and precisely
what the perish unit suite already pins (`EC Perish policy: fresh trapped
targets, escape timing and immunity guards` asserts `!MustEscape` at timer 1,
`ShouldPivotEarly` at timer 1, and `MustEscape` at timer 0). **No Perish
assertion was deleted or weakened**; the suite passes unchanged, and the owner
is untouched.

The `turns == 4` parametrization always passed, i.e. the deadline exit always
worked. What failed was `turns == 3`, which asserts the *authored preference* —
leave one turn early while the partner still holds the trap. Two separate causes,
both now fixed.

### Cause 1 (model): the scoring rule was not uniform across a decision

`mixed` was `forecastCount > 1 && (mustFinish || !PairDecisionBudgetExpired())`.
Boards are evaluated in sequence within one decision, so once the budget expired
a **switch candidate was scored against the primary forecast alone while the
stay board it had to beat had been scored on the full weighted mixture**. Those
two numbers are not comparable, and the bias runs against whichever board is
evaluated later — the switch candidates. This was the cross-board yardstick risk
listed in Round 1 §7.3, and it was costing Maura her early pivot.

`mixed` is now `forecastCount > 1`. The budget controls *how many pairs are
searched* (the stage-one shortlist loop still stops), never *how a pair is
valued*.

### Cause 2 (model): an authored early pivot paid the voluntary-switch cost

`score -= 35` exists so the AI does not give up an action on a whim. Leaving one
turn early while the partner maintains the trap is the authored PERISH_TRAP plan,
not a change of mind, so it is now exempt alongside the deadline exit
(`plannedExit`); the deadline exit additionally keeps its decisive bonus, since
it is the last turn the singer can legally leave.

### Cause 3 (fixture): the timeline was damage-roll dependent

The fixture's four Chanseys were built with authored levels only. With Maura's
re-levelled room they sometimes died to ordinary damage at the end of turn two —
and when a perished lead is replaced, the replacement carries no song, so
`IsAffectedFoe` correctly stops seeing a perished trapped foe and the early pivot
correctly loses its reason. The parametrizations disagreed about whether that
happened, which is what produced the `SEND_OUT not required (is the send out
random?)` and `TURN 3 incomplete` symptoms on alternating runs. The Chanseys are
now bulky enough that the countdown is the only clock, the send-outs sit on turn
four where the perish faints, and the original intent — singer gone at the end of
turn three, Gothitelle still out — holds again.

## Settled authoring: ownership and two new positive fixtures

`EC battle plans: compiled directives follow trainer ownership` now reflects the
merged audits: Tabitha's Magma Hideout team is asserted to carry **no** partner
tactic and no `ALLY_COMBO`, Cristian is the `ALLY_COMBO` owner, and Darius
(`ACTIVATE TORNADUS TAILWIND KILOWATTREL`) and Nate (`INSTRUCT ORANGURU INSTRUCT
DELPHOX`) are pinned by lookup.

One model change was needed before Darius could ever fire: `PairTacticScore`
required the trigger to be aimed at the recipient, but **Tailwind targets its own
user**, so an authored side or field trigger could never qualify. The aim check
now applies only to moves that can be pointed at a single foe; user, ally, field,
spread and both-target moves reach their own side by construction.

New fixtures:

| Fixture | Asserts |
| --- | --- |
| `EC authored strategy: Darius charges Wind Power with the authored Tailwind` | Tornadus chooses the authored Tailwind beside its Wind Power recipient, the side status lands, Kilowattrel does not spend the turn guarding, and the player side takes damage |
| `EC authored strategy: Nate instructs the Delphox that already attacked` | turn one establishes Delphox's move, turn two is the authored `INSTRUCT` aimed at it, and the recipient survives to use it |

Both are bulky-lead boards on purpose: nothing can be knocked out, so the
activation has to be chosen on the authored plan rather than on a knockout the
recipient could take without it — which is exactly the suppressor added in
Round 2.

## Verification

```sh
make -j4 check-tools
DEVKITARM=… make -j6 TEST=1 TEST_SOURCE_ALLOWLIST='test/test_runner.c test/test_runner_args.c \
  test/test_runner_battle.c test/battle/ai/coaching_pair.c test/battle/ai/fainted_target_pair.c \
  test/battle/ai/prankster_burn_pair.c test/battle/ai/quash_pair.c test/battle/ai/primary_support_pair.c \
  test/battle/ai/emerald_champions_plans.c test/battle/ai/emerald_champions_perish.c \
  test/battle/ai/mega_reveal.c test/battle/ai/protect_cadence.c test/battle/ai/dancer_pair.c \
  test/battle/ai/screen_pair.c test/battle/ai/taunt_pair.c test/battle/ai/weather_survival_pair.c \
  test/battle/ai/reflect_damage_pair.c' pokeemerald-test.elf
python3 scripts/stamp_release_inputs.py --stamp pokeemerald-test.inputs.json
python3 scripts/playthrough/run_focus.py --elf pokeemerald-test.elf --filter 'EC '
DEVKITARM=… make -j6 pokeemerald.gba      # 33,554,432 bytes, clean
```

**89 passed, 0 failed, 89 total** (87 → 86 in Round 2 with the stale Tabitha
fixture deleted, then +1 perish file, +2 new fixtures).

Decision budget, every instrumented fixture, limit 72 frames (1.2 s):

| Fixture | Round 2 | Round 3 |
| --- | --- | --- |
| `EC Dancer budget` | 61 | **66** |
| `EC doubles budget` (both params) | 60 | **62** |
| `EC Gym: Cristian` (both params) | 55 / 45 | 55 / 45 |
| `EC Gym: Jocelyn` | — | 44 |
| `EC authored strategy: Darius` | — | 23 |
| `EC authored strategy: Nate` | — | 13 |

Making the scoring rule uniform costs 5 frames on the heaviest board, which is
the price of comparable scores; the worst case keeps 6 frames of headroom.

## Open risks carried forward

1. The two heaviest boards now sit at 66 and 62 of 72 frames. The headroom is
   real but smaller than before, and a board heavier than the Dancer fixture is
   still untested.
2. The shortlist blind spot is unchanged: a pair that looks bad against the
   primary forecast and good only in the passive one never reaches stage two.
   No cheap fix was found.
3. Fixtures that deploy authored parties remain sensitive to re-authoring. The
   Maura case shows the failure mode is not always an assertion mismatch — a
   level change can silently remove a plan's *precondition* and make correct AI
   behaviour look like a regression. Where a fixture's subject is an AI rule
   rather than a difficulty check, synthetic bulk is worth the explicitness.
4. Constants (45/40/25 banking, tempo 10, pessimism 25, tactic 80) are still
   tuned against a test suite rather than play.
5. `AGENTS.md` line 176 and the Game Book AI policy prose still authorize the
   read; both remain the main agent's to correct.

---

# Round 4: the multi's budget, and the driver's livelock

Commit `46e9c2e51f`. Focused suite with `test/battle/ai/no_pp_struggle.c` added:
**91 passed, 0 failed, 91 total.**

## Defect 2 (budget): the clock restarted for every actor

`ComputeAiBattlerDecisions` set `decisionStartFrame` on every call. In an
ordinary doubles battle the pair search scores both opponents together and the
second call short-circuits, so nothing showed. The E0409 multi has **two groups
of AI actors** - Maxie's and Courtney's shared opposing pair, then the in-game
partner Steven - and each group restarted the clock, so each got its own full
60-frame budget. Worse, the reported figure restarted with it: the honest total
across the whole opposing decision was **140-165 frames**, not the 88-102 that
was visible.

Four changes, none of them touching the model's values:

* The clock is set once per turn: a second owner or an in-game partner scoring
  later in the same turn shares it instead of restarting it.
* Each group of AI actors gets an equal slice of the shared budget
  (`PairDecisionBudgetShare`): one group keeps all 60 frames, two groups get 30
  and then the remainder. This is the per-actor split lever.
* `PAIR_SHORTLIST` halves to `PAIR_SHORTLIST_CROWDED` = 2 when more than two
  actors still have to be scored, which bounds the settle stage without changing
  how any pair is valued. This is the shortlist lever.
* The optional prediction pass is skipped for an in-game partner - an AI actor
  whose decision is computed for real this turn, so predicting it is a wasted
  pass - and abandoned entirely once the mandatory setup has already spent the
  budget. Setup was 75 of the 129 frames measured on turn one.

E0409, `battle_random_policy.py --trainer TRAINER_COURTNEY_MOSSDEEP --cap 60`:

| Seed | Before (honest total) | After |
| --- | --- | --- |
| 11 | 165 | **83** |
| 3 | 162 | **98** |
| 5 | 139 | **82** |
| 13 | - | **92** |

Focused fixtures are unchanged: Dancer 65, Mega 62, Cristian 55/46, Jocelyn 45,
Darius 23, Nate 14. The multi is down by roughly half but **still over 72** on
its opening turns; what remains is native setup (`SetBattlerAiMovesData` over
four battlers and six-mon parties) plus the first uninterruptible refresh per
group, neither of which the search stop can shorten. Reducing it further means
changing what the setup computes, which is a larger change than the two levers
allowed here.

## Defect 1 (livelock): it is not the no-PP re-selection loop

Two real problems were found in AI move selection, both fixed:

* A board whose search is cut short by the budget never wrote `chosen`, and
  `AI_ComputeDoublesDecisions` copied that into `bestActions`, which was
  **uninitialised stack memory**. The AI could emit a move index it never chose.
  It is initialised now, and the emitted index is validated against the live
  move limitations before it leaves the pair search.
* When the engine refuses an AI battler's selection
  (`TrySetCantSelectMoveBattleScript`), the battler is asked again and answers
  the same forever. The refused slot is now taken out of that battler's options
  and its choice moves to one the engine still allows.

Neither is the reported livelock. The evidence:

* A regression built for the described case - an AI flank with **zero PP on
  every move** - passes with *and without* the selection guard, because
  `AreAllMovesUnusable` already routes that case to Struggle. The described
  mechanism does not reproduce.
* Seed 7 does reproduce a freeze, at the same point before and after every fix
  in this round. Driving it to the freeze and capturing the turn shows what is
  actually happening:

  ```
  The opposing Copperajah used Struggle!  /  But it failed!
  The opposing Heatran used Struggle!     /  But it failed!
  ... repeating, 14 messages and counting
  ```

  At that point the board is: player side reduced to one living battler
  (Alakazam at 1 HP) with the partner's slot permanently absent, both opponents
  out of usable moves, `player_faints 3`, `opponent_faints 3`, outcome still
  `ongoing`. Every AI move on that board is selectable or correctly refused; the
  pending decision belongs to the human, and the freeze happens **during turn
  execution after the human's command**, with Struggle failing and the same two
  AI actions running again.

So the owner is not AI move selection and not the no-PP selection path: it is a
turn that re-runs its actions when Struggle fails on a side with an absent slot.
That is a separate defect in execution order and end-of-battle detection, and it
wants its own look with the battle-script owner - I did not change execution
ordering blind. Seed 17 freezes the same way; seeds 3, 5, 11 and 13 now play to
completion.

## Verification

Headless ROM rebuilt and `pokeemerald-headless.inputs.json` re-stamped (checked
`PASS`); `work/agent-battle-build` untouched, with every run in this round
driven from a private copy under `/tmp`. Focused allowlist plus the new fixture:
**91 passed, 0 failed, 91 total**. `pokeemerald.gba` builds clean.

---

# Round 5: pricing what a turn buys after it ends

Commit `7f794556de`. Focused allowlist with `no_pp_struggle.c` and the new
`setup_pricing.c`: **98 passed, 0 failed, 98 total.**

## Diagnosis

Every class in the playtest reports fails the same way, and it is structural
rather than a collection of separate bugs: **the pair search scores one turn,
and the entire value of these moves arrives on the turns after it.** Tailwind
changes no HP this turn. Quiver Dance boosts a user whose attack has already
been spent. Sleep Powder, Thunder Wave, Will-O-Wisp, Screech and Acid Spray only
pay when the victim next tries to act - and Acid Spray pays nothing at all in
turn when the sprayer is slower than the partner it is setting up, which is
exactly Jerry's Koffing. Priced at zero, all of them lose to any direct attack,
every turn, on every team. The counter-example proves the diagnosis rather than
contradicting it: Trick Room fires because `PairPlanScore` gives it 70-100
outright, and weather and Tailwind had the same kind of score but **gated behind
the trainer's authored flag** - which route teams like Jose's, Johnson's and
Dawson's do not carry, so their Tailwind was worth literally nothing.

## Fixes, all on values

**Four bounded next-turn values**, paid at the end of the trial and only when
the effect actually landed and its owner or victim is still standing:

| Term | Value | Covers |
| --- | --- | --- |
| `PAIR_SETUP_HORIZON` | 40, scaled by the user's survival | Quiver Dance, Iron Defense, Victory Dance, Bulk Up |
| `PAIR_SLEEP_HORIZON` | 35 | Sleep Powder, Spore |
| `PAIR_STATUS_HORIZON` | 20 | Will-O-Wisp, Thunder Wave, Taunt |
| `PAIR_STAT_DROP_HORIZON` | 12 per stage | Acid Spray, Screech, Icy Wind |

A boost that gets its user killed, and a sleep on something that faints anyway,
are still worth nothing: the terms are gated on `hp[...]` at the end of the
trial. They are symmetric - a foe doing the same to us subtracts.

**Tailwind is no longer gated on the authored flag.** The order crossings it
buys are the value whether or not the trainer carries `TAILWIND`; the flag adds
20 on top. With no crossings it is still worth nothing, which the fixture pins
in both directions. **Setup lost its flag gate** for the same reason - safety
decides, and the trial was already measuring safety.

**Switching pays for the turn it gives up.** A switch preserves board value
while an attack spends HP, so a one-turn board made withdrawing a healthy lead
look nearly free and only a flat 35 stood against it - the shape behind
Lilith's Scraggy, Takao's Grapploct, Brawly's Hariyama and the rival's Pikachu.
A voluntary switch now also pays the damage the outgoing battler was about to
deal (capped at 70) and, on turn one from full health with nothing revealed, 60
more. **A position that genuinely needs to change pays none of it**: the
pressure test the reserve search already runs (`PairNeedsSwitchSearch`) exempts
it, so Laura's pivot, Brenden's doomed recipient and the Life Orb exit are
unchanged - all three still pass.

**A guard that denies less than its user could simply have healed** banks
nothing beyond the base share. Half the maximum back is permanent; what a shield
saves is only the part a payoff makes permanent.

## Fixtures added

`test/battle/ai/setup_pricing.c` (6 cases, all passing):

| Fixture | Pins |
| --- | --- |
| Tailwind is worth the turn when the side is outsped | wind when it buys crossings, the attack when it does not |
| a boost is worth a safe turn and not a dangerous one | Quiver Dance on a free turn, Bug Buzz when the turn would kill it |
| sleep on a healthy threat is worth the turn | Compound Eyes Sleep Powder over the chip attack |
| Endeavor is the low-HP attack when it can land first | two HP and faster: level the healthy target |
| a healthy lead does not reset the board on turn one | unpressured full-HP lead attacks instead of withdrawing |
| recovery beats a shield that banks less than it heals | the empty second guard becomes a heal |

`EC Gym: Jocelyn dances on a safe board with the Dancer relay up` in
`emerald_champions_plans.c` is the authored acceptance case: the setter dances
and **Oricorio's Dancer copies it**, so both halves of the room gain. The Acid
Spray fixture in `primary_support_pair.c` gained a slow-sprayer parametrization:
a drop that lands after the partner has attacked still pays on later turns, so
it sprays; only a drop that cannot land at all (Covert Cloak) is the attack's
turn.

## Numbers

Focused allowlist, `run_focus.py --filter 'EC '`: **98 passed, 0 failed, 98
total** (91 before this round, +6 setup pricing, +1 Jocelyn relay).

Decision frames, limit 72: Dancer 66, Mega 62, Cristian 57/47, Ned 61/51,
Jocelyn 46, misty gym 46, Darius 24, Jocelyn relay 24, Nate 14, Brawly 60.

E0409 multi after Round 4's budget work, unchanged by this round's values:
seeds 11/3/5/13 measure **83, 98, 82, 95** frames against 165 before.

`pokeemerald.gba` builds clean.

## Not reached in this pass, with what is known

* **Known-negligible damage re-selected** (Poochyena's Crunch into a Fairy,
  Eevee's Quick Attack into Snorlax with Rocky Helmet recoil). Both are single
  battles, where the pair search does not run at all - this is the upstream
  per-battler scorer, a different owner from everything above.
* **Shroomish never using Spore** should now be covered by the sleep horizon,
  but it is a single battle too, so it needs the same owner checked.
* **The Conservative trait and the guard model** (Darian never Protecting) -
  not investigated.
* **Glimmet's 0.25x versus 0.5x target choice** - not investigated; it is a
  target-selection question inside the damage comparison rather than a class
  pricing one.
* **Corsola's Life Dew at 13/61 into a lethal attack** - the heal-aware guard
  rule does not cover choosing between a heal and an attack when the heal
  cannot outrun the incoming damage.
* The recovery fixture pins the **empty** guard case (nothing incoming). With
  real damage incoming the model still prefers the shield at low HP; that margin
  is unresolved.
* Zubat/Golbat declining Taunt and U-turn, and Brawly's Hariyama withdrawal,
  were logged as watch items and are partly addressed by the switch cost, but
  neither has a fixture.

---

# Round 6: the livelock's real cause, and the per-battler scorer

Commit `9158e1c4e0`. Focused allowlist with `multi_reserve.c` and
`single_scorer.c` added: **103 passed, 0 failed, 103 total.**

## (1) The livelock is a party-indexing bug

Driving seed 7 to the freeze and reading the board settles it. At the hang:
`turn_after 303` (the battle ran 295 turns with no decision point), battler 0
fainted, battler 2 permanently absent, `outcome` still `ongoing` - and the
player's party holding **Togekiss at slot 3, untouched at 228/228**.

`GetAILastPartyIndex` returned `PARTY_SIZE / 2` for any battler on a side with
two trainers when `AreMultiPartiesFullTeams()` was false. That rule dates from
the link and tower multi formats, where the two trainers really do share one
`gPlayerParty` and split it three and three. In this build `GetBattlerParty`
returns `gParties[GetBattlerTrainer(battler)]` - **each trainer owns its own
array** - so the split does not partition anything; it simply hides slots three
to five of that trainer's own team. The chain:

* `HasNoMonsToSwitch(0, ...)` scanned slots 0-2 only, all fainted, and reported
  nothing to switch to;
* so `FAINTED_ACTIONS_NO_MONS_TO_SWITCH` never cleared battler 0's absent flag;
* so `FAINTED_ACTIONS_HANDLE_FAINTED_MON` never asked for a replacement;
* and `NoAliveMonsForEitherParty()` was false, because Togekiss was alive, so
  the battle could not end either.

Both opponents were genuinely out of usable moves and struggling at a side with
nothing on it, which is the "But it failed!" the capture showed. Struggle was
the symptom; the missing replacement was the defect.

The split now applies only when the two battlers on a side actually resolve to
the same party array. The same index drives the AI's own switch search
(`switchContext.lastId`) and the opponent controller, so this also means every
trainer on a two-trainer side could previously only consider its first three
members when switching - a second, quieter consequence fixed by the same line.

`test/battle/ai/multi_reserve.c`: a fainted lead replaced from beyond the split
on a live board, and the index itself asserted on a half-team multi - it reads
**3 without the fix and 6 with it**, verified by reverting the change and
rerunning.

**Not yet confirmed end to end.** Seeds 7 and 17 need a headless ROM built from
this commit, and the instruction for this pass was not to rebuild it. The
regression proves the cause and the repair; the two seeds should be re-run on
the rebuilt ROM.

## (2) Per-battler scorer

This path runs when the pair search does not - every single battle, which is
where the reported cases live.

* **Negligible chip.** `AI_CheckViability` rejected only moves that cannot
  damage at all (`hitsToKO == 0`); a move needing nine turns was still a
  perfectly good attack. An attack that needs `NEGLIGIBLE_DAMAGE_HITS` (5) or
  more turns now loses a step, and if the user pays contact damage every one of
  those turns - Rocky Helmet, Iron Barbs, Rough Skin - and that damage is at
  least half what it deals, it loses a further three. That is the Eevee case
  exactly: three Quick Attacks for nine, nine and zero while an Adaptability
  Double-Edge sat unused.
* **Sleep.** `IncreaseSleepScore` paid `DECENT_EFFECT` (2) for removing a
  healthy foe's turns, which loses to almost any damage roll. It now pays
  `GOOD_EFFECT`, and one more step when the target can knock the user out -
  Shroomish in front of an Arcanine that two-shots it. The existing early return
  when the AI can simply faint the target is untouched, so this never displaces
  a knockout.

## (3) The Conservative trait and the guard model

`AI_FLAG_CONSERVATIVE` only ever touched accuracy and friendly-fire thresholds;
it had no contact with the guard model at all. The trait is a stated preference
for the safe line and the banked share is exactly where safety is priced, so a
Conservative battler now banks a further 20 of what its guard denies. The
fixture runs the same board with and without the flag: the Conservative flank
guards, the ordinary one attacks.

## Fixtures added

`test/battle/ai/single_scorer.c` (3): chip that costs contact damage loses to
the real attack; sleep on a healthy threat beats the chip that cannot finish it;
a Conservative flank banks more of what its guard denies.
`test/battle/ai/multi_reserve.c` (2), above.

One honest note on the sleep fixture: it asserts the **selection**, which is the
scorer's job. On that single-battle board the Spore is chosen but the status
does not resolve; that the sleep itself lands is covered in doubles by
`EC setup pricing: sleep on a healthy threat is worth the turn`. The
single-battle powder path is worth a separate look rather than a weaker
assertion here.

## Numbers

`run_focus.py --filter 'EC '`: **103 passed, 0 failed, 103 total** (98 before).
`pokeemerald.gba` builds clean. Headless ROM and its stamp untouched this round.

---

# Round 7: two small pricing items

Commit `1c6ef92bc8`. Focused allowlist: **105 passed, 0 failed, 105 total.**

## Helping Hand into damage it cannot multiply

Helping Hand multiplies an attack's power, so a move whose damage is a fixed
number, a fraction of the target's HP, or a reflection of damage taken ignores
it completely. The pair trial applied the 3/2 multiplier regardless
(`boost[partner]`), and the per-battler scorer rewarded the pairing, which is
how Shinx spent a turn boosting its partner's Endeavor.

`IsFixedDamageMove` now covers `EFFECT_FIXED_HP_DAMAGE`,
`EFFECT_FIXED_PERCENT_DAMAGE` (Super Fang), `EFFECT_LEVEL_DAMAGE` (Night Shade,
Seismic Toss), `EFFECT_PSYWAVE`, `EFFECT_ENDEAVOR`, `EFFECT_FINAL_GAMBIT`,
`EFFECT_REFLECT_DAMAGE` (Counter, Mirror Coat, Metal Burst), `EFFECT_OHKO` and
`EFFECT_BIDE`. The trial skips the boost for those, and the scorer treats the
pairing exactly as it already treated boosting a status move
(`WORST_EFFECT`). Dragon Rage and Sonic Boom resolve through the fixed-HP
effect, so they are covered by the same entry.

Fixture: `EC setup pricing: Helping Hand is not spent on damage it cannot
multiply` - declined in front of Endeavor, taken in front of an ordinary attack
on the identical board.

## A guard that denies nothing

The banked share prices what a shield **keeps**, so it has nothing to say about
a shield with nothing to keep. A turn-one guard from full health that blocked no
damage at all paid only the flat 10-point tempo, which was little enough to win
the safest status or setup turn its side would get - the Slowpoke that guarded
instead of yawning. A guard that denies nothing in a trial, with no waiting
payoff and no partner turn to buy, now pays `PAIR_GUARD_EMPTY_COST` (30) on top
of the tempo. The cost is per trial, so a guard that denies nothing in one
forecast pattern and something in another is charged only in the pattern where
it bought nothing, which is the right accounting under the mixed model.

No existing guard expectation moved: all 103 previously passing cases still
pass, because the guards they assert either deny something or have a payoff.

Fixture: `EC setup pricing: a guard that denies nothing loses the turn to a
status move`.

## Numbers

**105 passed, 0 failed, 105 total.** Decision frames unchanged: Dancer 66, Mega
62, Ned 61/51, Brawly 60, Cristian 57/47, Jocelyn 46, misty gym 46, Darius 24,
Jocelyn relay 24, Nate 13. `pokeemerald.gba` builds clean; the headless ROM and
its stamp are untouched.

Confirmed in play by the coordinator's reruns from the previous rounds: Trick
Room, Iron Defense, Endeavor, Acid Spray, Sleep Powder, Tailwind and the
Victory Dance/Dancer relay all fire, and healthy-lead withdrawals are gone.

---

# Round 8: the single-battle Spore board

Commit `940393c149`. Focused allowlist: **105 passed, 0 failed, 105 total.**

## Cause: the fixture's own attacker, not the engine

The board was: Arcanine (Speed 120, Bite) against Shroomish (Speed 30, Spore +
Absorb). Arcanine moved first, Shroomish's Spore was the chosen command, and the
player ended the turn awake with `status1 == 0`.

Ruled out, each by inspection of that exact board:

* **Grass immunity** - the target is Fire, and the powder rule exempts Grass.
* **Overcoat / Safety Goggles** - no ability or item was set on the target;
  the debug read `GetBattlerAbility(B_BATTLER_0) == 22`, which is Intimidate,
  not Insomnia (15) or Vital Spirit.
* **Misty or Electric Terrain** - the state dump shows `terrain: none`.
* **Substitute** - never used on the board.
* **A move-failed path or a singles powder defect** - disproved below.

The cause is **Bite's flinch**. `EXPECT_MOVE` matches the command a battler
*chose*, through `TestRunner_Battle_CheckChosenMove` at selection time, not a
move that executed. A flinched Shroomish therefore satisfied the Spore
expectation perfectly while the powder never went off, which is exactly what the
30% secondary does on a board whose attacker moves first. Holding the secondary
off (`MOVE(player, MOVE_BITE, secondaryEffect: FALSE)`) makes the sleep land on
every run, and the fixture now asserts it:

```
EXPECT(player->status1 & STATUS1_SLEEP);
```

No engine change was needed. The scorer's valuation - the fixture's actual
subject - was already correct and is now pinned together with the status it
produces.

This is worth remembering when reading any AI fixture: an `EXPECT_MOVE` that
passes says the AI *chose* the move, not that the move resolved. Where the
resolution matters, the board has to keep the chooser from being interrupted.

## Numbers

**105 passed, 0 failed, 105 total**; `pokeemerald.gba` builds clean; the
headless ROM and its stamp untouched.

Seeds 7 and 17 of the E0409 multi were confirmed by the coordinator on a
headless ROM rebuilt from the multi-reserve commit - won in 10 turns and lost in
9 - which closes the livelock item end to end.

---

# Round 9: the performance ceiling, correlated guards, and last-turn knowledge

Commits `ceba4ad06e` and `82ced61526`. Focused allowlist: **112 passed, 0 failed,
112 total.**

Preceded by two restorations, `4b14745e1a` and `ca0aab8c88`, after stale-tree
commits removed this work twice; the recovery is the coordinator's and the
pathspec-commit rule now in force is what keeps it from recurring.

## (1) The 60-frame ceiling: measured, attributed as far as it can be

`EC Laura board: an ordinary four-member route decision finishes inside the
budget` now instruments the case the reruns describe: **61 frames** against the
60-frame stop, beside Ned's 51 and 61. So the ceiling is real and it is reached
on an ordinary board, not only on six-member authored teams.

Attribution, by rebuilding `src` and `include` at earlier commits:

| Tree | Ned wind / soak |
| --- | --- |
| `7f794556de^` (immediately before the setup-pricing values) | 50 / 60 |
| HEAD | 51 / 61 |

**The setup-pricing values cost about one frame.** They are not the cause.
Going further back, to before the committed-action read was removed, does not
compile against the current generated data, so that comparison stays open - but
the mechanism is visible in the code: with the read each foe had exactly one
enumerated action, `ChooseJointFoeForecast` returned immediately, and each
candidate pair cost one cheap trial. Removing it gave every foe a full
move-by-target enumeration and the three-pattern mixture that the budget stop
now truncates.

**The shortcut I drafted is not landed, because it is not value-preserving in
practice.** Selecting the shortlist on the ordinary pass alone buys 10-30%
(Jocelyn 46 to 36, Dancer 66 to 56, Darius 24 to 18) and moved **six** fixtures,
every one of them a move whose entire point is an applied effect: Soak's
conversion, Acid Spray's drop, Eerie Impulse, Taunt. The ordinary pass cannot
see those, so ranking without it systematically demotes exactly the class this
project spent two rounds teaching the AI to value. Widening the shortlist to six
recovered two of the six and no more.

What is landed is exact: two joint forecasts that are the same four actions
score the same, so the alternative pattern reuses the primary score rather than
recomputing it. No outcome changes, and no frames on these boards either, since
identical patterns are rare - it is correctness housekeeping, not the fix.

**The ceiling is still open.** The honest next lever is the enumeration itself
(how many candidate pairs exist), not the scoring of each pair.

## (4) Correlated guards

Both slots guarding is one decision about the whole turn, and the pair search
scores the joint pair, so it can charge for it. A double guard now pays
`PAIR_GUARD_CORRELATED_COST` (45) unless `PairFieldClockExpiring` - a weather,
Tailwind or Trick Room clock with one turn left - justifies the whole side
spending the turn. That is the only payoff kind that both slots can spend the
same turn on; a per-slot payoff such as an Orb activation needs its partner to
be doing something else, which is exactly the 55-T1 case where Swellow's Flame
Orb turn wanted Manectric contributing, not guarding beside it.

## (5) and (3) Last turn's move, and Feint

| Class | Rule |
| --- | --- |
| Sucker Punch | −25 at a target whose last used move was a status move; a flank that just used Follow Me is the likeliest thing on the board to use one again, and Sucker Punch fails into it |
| Counter / Mirror Coat / Metal Burst | −40 when nothing the opposing side has actually used belongs to the category the move reflects |
| Feint | −20 with no shield on the field and none seen last turn: it is a weak attack, not a shield breaker |

## (6) Disruption next-turn values

Encore pays 35 on a foe that just spent its turn on a status move and 20 on any
other known last move; Leech Seed pays 20 on an unseeded non-Grass foe; and a
burn is worth `PAIR_BURN_PHYSICAL_HORIZON` (20) more when the victim's best
usable attack is physical. Item swap (Switcheroo/Trick) is **not** covered.

## Fixtures added

`test/battle/ai/reactive_pricing.c` (6): both slots do not guard the same turn;
Sucker Punch not aimed at the flank that just redirected; Counter refused
against a side that only attacks specially; Feint refused with no shield to
break; the turn spent on a burn; Encore locking in the move the target just
used. Plus the Laura frame fixture in `emerald_champions_plans.c`.

One honest limit recorded in the burn fixture: the turn being spent on the burn
is asserted, but **which body it lands on is not**. The physical-attacker term
scores it, and on that board it did not decide the target by itself.

## Numbers

**112 passed, 0 failed, 112 total** (106 before this round's fixtures).
`pokeemerald.gba` builds clean. Headless ROM untouched.

## Not reached in this pass

Takao's Unburden-priced turn-one switch; the Josh pivot items; item swap; and
the whole opening-block list (a)-(e): priority overvalued when the user is
already faster, Wailmer's Water Spout, Wooloo's Reversal, Shroomish's Spore on
the **pair** path, Choice Scarf String Shot, Paras's Wide Guard after a seen
Rock Slide, Klutz Switcheroo, and Poochyena's Helping Hand/Snarl.

---

# Round 10: the ceiling's real shape, Mega value, and authored activations

Commits `e658350178`, `f2c89f828e`. Focused allowlist: **115 passed, 0 failed,
115 total.**

## The 60-frame ceiling: not reachable by pruning

Two enumeration-level prunes, both measured on the instrumented boards:

| Change | Laura | Ned wind / soak | Dancer | Jocelyn | Suite |
| --- | --- | --- | --- | --- | --- |
| baseline | 61 | 51 / 61 | 66 | 46 | 112 green |
| useful-action filter applied to the opposing enumeration | 62 | 51 / 61 | 65 | 47 | 112 green |
| + collapsing dominated opposing attacks (same target, category, priority, strictly less expected damage) | 62 | 51 / 61 | 65 | 47 | **111**, moved a Protect fixture |

The first is landed as correctness housekeeping. The second is not: it bought
nothing and cost a fixture. Together with Round 9's ranking shortcut - 10-30%
for six moved fixtures - that is three attempts, and the conclusion is the same
each time.

**What the cost actually is.** Not redundancy in the opposing enumeration, and
not the forecast mixture: it is the size of *our own* pair enumeration times the
two scoring passes. Two battlers with four moves and two or three legal targets
each give 64 to 144 candidate pairs, and every one of them is scored twice
because the second pass is where an applied effect is weighted. The shortlist
bounds stage two only. Cutting this means a smaller candidate space or a cheaper
per-pair evaluation, and both change decided values - which is precisely what
the three measurements demonstrate rather than assume.

## Mega Evolution

`EC_BATTLE_PLAN_MEGA_REVEAL` is authored on Roxanne alone, so every other holder
depended on same-turn damage clearing the bar, and a form whose worth is an
ability or a later turn never does: Isaac's Kangaskhan held its stone, took Fake
Out and Sucker Punch, and fainted in base form with Parental Bond never
existing. `PAIR_MEGA_HORIZON` (50) makes evolving the default once the holder
acts. The ordinary board comparison still overrules it when the base form has
something the Mega loses, and the existing tie cost still prefers not evolving
on a true tie. Fixture: `EC Mega: the holder evolves on the first turn it acts`.

## Authored activations: one root, two trainers

Aisha's `FROSLASS FROST_BREATH → TAUROS` and Georgia's `DUSKULL SHADOW_SNEAK →
METANG` never fired while Shayla's `SIMIPOUR SURF → MARACTUS` fired cleanly. The
difference is exactly what the coordinator suspected: a **single-target** attack
aimed at an ally is dropped by the `BuildPairActions` ally filter unless its
isolated score is already neutral, which friendly fire never is. Shayla's Surf
is a spread move, which the filter does not touch. So two of the three were
never candidates at all, and the reward meant to judge them was never reached.

Three changes, one root:

* an action matching an authored ACTIVATE survives the ally filter;
* inside the trial it is judged by its own reward rather than by the isolated
  opinion of hitting an ally - the same wrong gate one layer down;
* a Weakness Policy recipient is exempt from the super-effective rejection,
  because a super-effective hit is the only thing that arms the item. Lethality
  still decides, so the Dark-weak Annihilape case from Round 2 is untouched.

Fixtures: Aisha's Frost Breath maxing Anger Point, Georgia's Shadow Sneak arming
the Policy.

## Takao: reproduced, not fixed

`EXPECT_EQ(opponentLeft->species, SPECIES_GRAPPLOCT)` on his authored board with
a Fairy attacker opposite fails - the healthy 80/80 lead is withdrawn on turn
one, exactly as reported. The cause is that Round 5's turn-one switch costs are
waived whenever `PairNeedsSwitchSearch` reports pressure, and a Fairy attack
aimed at a Fighting lead is pressure by that test even at full health. Narrowing
the waiver to "below full health, or the incoming damage would actually remove
it" did **not** fix Takao and broke the Life Orb pivot case, so it is not
landed. The fixture is not committed either, since it would be red; the recipe
above reproduces it in one turn.

## Numbers

**115 passed, 0 failed, 115 total.** `pokeemerald.gba` builds clean. Headless
ROM untouched.

## Queue state

Landed this round: the enumeration filter, the ceiling attribution, Mega value,
the authored-activation root with Aisha and Georgia.

Not reached, in the order they arrived: Takao's switch; the opening-block list
(priority overvalued when already faster, Water Spout at full HP, Reversal at
5 HP, Spore on the pair path, Choice Scarf String Shot, Wide Guard after a seen
spread move, Klutz Switcheroo, Helping Hand/Snarl beside a live partner); item
swap value and the Josh pivots; same-turn conflicts (High Jump Kick into a
target the partner removes first, Helping Hand onto a switching partner); Shell
Smash and Choice Scarf Trick; and the whole wave-2 rerun batch (Counter spam,
repeated Protect after a success, Cindy's Truant switch-in, Wish at full HP,
Howl with no scaling move, Light Screen at 12 HP, Kirlia's guards, Zubat's
recoil, Lickitung's Helping Hand, Froslass's Destiny Bond, Gible's spread choice).

That backlog is now far larger than one pass can absorb, and several of its
items are the same shape as each other; grouping them by mechanism rather than
by battle would let one fix close several at a time.

---

# Group A: same-turn joint conflicts

Commit `ac2e98e5e9`. Focused allowlist: **117 passed, 0 failed, 117 total.**
Frames unchanged: Laura 62, Ned 51/61, Dancer 65.

## Already covered by the earlier commit

**Both slots guarding** is the correlated-guard cost from `82ced61526`, and
`EC reactive pricing: both slots do not guard the same turn` passes. Nothing to
do.

**A spread move that clips a healthy ally** turned out to be covered too: on a
board where Earthquake and Rock Slide reach the same two foes and only
Earthquake also lands on a grounded, unprotected, full-health partner, the AI
already takes Rock Slide. The ally's HP is real board value in the trial, so it
was priced all along. Verified with a fixture rather than re-fixed.

## Fixed: the crash with nothing left to hit

The trial models the native retarget - a single-target move whose target has
fainted is sent to the surviving foe - but not the case where **there is no
surviving foe to retarget to**. That is exactly the reported board: the
partner's Surf cleared both player slots before the slower flank acted, so High
Jump Kick had nothing to hit and crashed, twice, 103 to 67 to 19. The trial saw
a harmless miss.

A crash move with no living opposing target now takes half the user's maximum in
the trial, so the pair that produces it carries its real cost. Fixture: `EC joint
conflicts: a crash move is not aimed into a target the partner removes first`.

## Fixed: a support move the partner cannot use

Helping Hand onto a partner that is leaving, has already acted, or is holding
damage the multiplier cannot touch produced no boost in the trial - correctly -
but that made it a *tie* with anything else that achieved nothing, and ties are
decided by other terms. `PAIR_SUPPORT_WASTED_COST` (40) makes it lose to an
action that does something. That is the Helping Hand that announced itself and
failed into a partner already switching out.

No fixture: the case needs the AI itself to choose a switch for the partner,
which a fixture cannot script. The neighbouring case - Helping Hand in front of
damage the multiplier cannot touch - is pinned by `EC setup pricing: Helping
Hand is not spent on damage it cannot multiply`.

# Group B — last-move and reactive

Commit `ed14b8c7f8`. Suite green at the end of the group.

**Already covered.** Counter against a side that has only ever hit specially was
refused before this pass (`82ced61526`); the Wobbuffet sighting predates that
commit and its fixture passes untouched. Destiny Bond, Sucker Punch and the
Feint reads were likewise already priced.

**Fixed: Mirror Coat had no read.** The other half of the Counter rule was
missing. A side that has only hit specially is the one board state where Mirror
Coat is a read rather than a coin flip, and nothing rewarded it. It now earns
that read, on the same evidence the Counter refusal uses.

**Fixed: Encore against a Choice-locked foe.** A locked repeater is already
committed to the move this turn, so the lock costs it nothing it had — but it
keeps it there after the item would have let it switch out. Worth 15 more than
an ordinary Encore (20 status / 35 attacking base).

**Fixed: the side guards needed evidence.** Wide Guard and Quick Guard answer
something already seen — a spread move, or a priority move, from a living foe
last turn. Their denial has not happened yet, so the trial alone cannot earn
them the turn; the sighting does (+30).

Fixtures: `EC reactive pricing: Mirror Coat is a read after special hits only`,
`EC reactive pricing: Wide Guard answers the spread move it has seen`.

# Group C — signature-power underpricing

Commit `1467c08c97`. Suite green at the end of the group.

**Already covered, now pinned.** Three of the four reports reproduce as correct
behaviour and are held by new fixtures in `test/battle/ai/power_pricing.c`:
Water Spout at full health beats the small reliable attack; a max-power Reversal
at five HP beats waiting behind a shield; recoil is aimed at the neutral body
rather than the one that resists it and pays the user back for nothing.

**Fixed: priority paid for a step the user already had.** Priority buys exactly
one thing — striking first — and a user that already outspeeds its target has
that for free, so the extra step is worth nothing and the comparison belongs to
the damage. That is how a Huge Power body took a forty-power priority attack
twice over the move that would have ended the same target. A priority attack
aimed at a target the user already outspeeds now carries −45, with Trick Room
folded into the speed comparison.

Fake Out and First Impression are exempt: their worth is the flinch and the
first-turn window, not the step in the order. Without that exemption the rule
moved Laura's pivot board, which is the check that caught it.

# Group D — next-turn values for setup and support

Commit `50bcf8c54a`. Six new fixtures in `test/battle/ai/next_turn_values.c`.

**Already covered, now pinned.** Shell Smash is taken behind a White Herb (the
Herb's undo was already modelled in the trial, so the pair sees the +2 Speed it
keeps); a Wish at full health is refused, because the Wish branch is capped by
missing HP and there is none. Protect on a full-HP resist body and Spore on the
pair path were fixed earlier in the pass and still hold.

**Fixed: a speed drop is worth the order it buys.** `PAIR_STAT_DROP_HORIZON`
priced a drop as chip only. A drop that puts one of ours in front of the target
is speed control — the whole point of spending a slow partner's turn on it — and
now adds `PAIR_SPEED_CROSSING_HORIZON` (25) when a crossing actually appears.
That is the Choice Scarf String Shot case. The fixture holds both directions:
the same two stages on a target nobody can cross keep losing to the attack.

**Fixed: a boost nothing can cash is not a boost.** Howl on a body whose only
damaging move is special raised Attack into an empty hand and still collected
the full setup horizon. An Attack raise with no physical attack on the set now
earns nothing unless some other raised stat does.

**Fixed: item swaps were unpriced.** A swap is now the difference between the
two items: a dead item (Klutz, or one whose hold effect is suppressed) for a
working one is 45; a Choice item stuck on a foe that can hold it is 30; any
upgrade is 15; handing a working item into an empty hand is −25. Orbs, a Sticky
Barb and an Iron Ball are excluded from that last penalty and left to the
per-battler scorer, which knows the recipient's ability and immunities — that
is the logic the Klutz Flame Orb fixture in `ai_doubles.c` already pins, and
duplicating it at pair level broke it before I deferred.

**Fixed: Destiny Bond had no value at all.** It is now worth 55 when a *known*
move from a foe that moves first kills through the lowest roll, and −20
otherwise. The knowledge condition is load-bearing and the fixture asserts it:
on turn one, against a set the AI has not been shown, the bond is refused; after
the fixed-damage move has been seen once, the same board takes the trade.

**Frames.** Worst reported decision 65 of the 72-frame budget
(`DANCER_FULL_BENCH`), unchanged by this group; Brawly 61, Ned Soak 61, Laura 60.

**Pre-existing red outside my suite.** Building the wider allowlist surfaced 12
failures in `test/battle/ai/ai_doubles.c` ("EC expert pair: …"). They fail
identically at `HEAD` with my working tree reverted, so they are not mine; that
file is another agent's. I verified against that baseline rather than assuming,
and one of my group D drafts did add a 13th (the Klutz case above) before I
deferred orb pricing back to the scorer.

# Group E — switch pricing

Commit `d0f4a9c67b`. No source change: all three reports are already covered by
the Round 5 costs and the later plan rules, and none reproduced. They are now
held by fixtures in `test/battle/ai/switch_pricing.c` and
`emerald_champions_plans.c` instead of being carried in the backlog.

* **Takao (E0037).** The healthy 80/80 lead stays in on turn one against a
  Fairy attacker, at two attack strengths. The Round 10 note that this was
  "reproduced, not fixed" is stale — the board it describes now passes.
* **The Josh pivot shape.** A full-health wall that takes well over half from
  the attacker opposite (Nosepass, with Naclstack on the bench) keeps the turn
  it would otherwise give away.
* **Cindy's Truant entry.** A Truant body leaves on the board that walls its
  only attack to nothing and stays on the board it can hit. The loafing turn is
  not by itself a reason to pivot.

Two narrowings were tried and reverted: waiving the commitment margin on a turn
with no action (Truant, recharge), and waiving it for an attacker walled to
zero. Neither changed the reported behaviour and the second moved
`EC Tailwind: faster recoil attackers do not use empty speed setup`, so the
margin stands as it was.

# Group F — the Electric Gym block

Commit `e74572d472`. Two of three already covered, one fixed.

**Already covered, now pinned.** An Electric attack is not fed to a Lightning
Rod: the same Thundurus board with the Rod replaced by Static takes the Electric
move, so the avoidance is the ability and not a blanket aversion. A pivot with
an empty bench is only an attack — Volt Switch with nobody to bring in loses to
the stronger move. Wattson's authored `ACTIVATE ELECTRODE DISCHARGE ELECTIVIRE`
fires on his real board and Electivire takes the Motor Drive stage, which is the
room's whole engine.

**Fixed: Taunt had nothing to take.** Taunt spends the entire turn to remove a
move, and it was being chosen against a side holding no status move at all —
the Electrode that taunted twice. It now loses 80 when nothing on the other side
holds a status move, or when everything that does has already been taunted. The
gate reads the whole opposing side rather than the nominal target, because the
chosen target for a denial move is not always the battler the denial lands on.

# Group G — conditional power and the empty guard

Commit `1a828035b0`. Both reports already covered; no source change.

**Status-conditional power is priced at the power the move will have.** With
type, accuracy and target held equal, Hex loses to the flat Ghost attack against
a clean body and beats it against a poisoned one. The Drifblim sighting is not a
mispriced conditional: Hex at 65 and 100 accuracy against Air Slash at 75 and 95
is a near-tie that the risk-averse quarter of the blend settles in favour of the
move that cannot miss. My first fixture failed only because Ghost is doubled
against a Psychic body — the matchup, not the pricing.

**Parasect's free turn goes to Spore.** At full health with nothing aimed at it,
the hundred-accuracy sleep beats the shield that denies nothing. The
zero-denial guard cost and the sleep horizon are both doing their job here.

# Group H — the Route 111 to Mt. Chimney receipts

Commit `ff4f2e9d54`. Six of seven already covered; no source change.

Those battles were played on a snapshot cut before the group D commit, and none
of the six reproduce on current HEAD. New fixtures in
`test/battle/ai/signature_utility.c` pin each one: Octolock is the plan on a
board that cannot punish the turn; Destiny Bond at one HP takes the trade once
the killing move has been seen (group D); a support body with nothing to attack
with spends its turn on Thunder Wave; Aurora Veil is taken while the snow is up;
Wide Guard answers the spread it has already been shown (group B); and the
doubled, recoil-free ground attack beats Double-Edge (group C).

The seventh — consecutive Protect on the Graveler board — did not reproduce and
could not be forced into a fixture: the AI takes the attack on turn one rather
than the shield, so there is no second guard to discount, and an AI test cannot
script the opponent's first move. Repeat-guard discounting stays pinned where it
already is, in `protect_cadence.c`.

# Numbers for groups B through H

**175 passed, 0 failed** across my allowlist (187 total when `ai_doubles.c`,
`ai_flag_risky.c` and `gimmick_mega.c` are included; those carry 12 failures
that are identical at `HEAD` with my working tree reverted, in a file another
agent owns).

Worst decision cost 65 frames of the 72-frame budget (`DANCER_FULL_BENCH`),
unchanged across all six groups: Brawly 61, Ned soak 61, Laura 60, Ned wind 51,
Cristian 47/57, Jocelyn 47, Jocelyn dance 25, Darius 25, Nate 15.

`pokeemerald.gba` builds clean at 33,554,432 bytes. The headless ROM and
`work/agent-battle-build` were not touched.

# Group I — the Petalburg and Mt. Chimney receipts

Commit `f2f66dbf71`, fixtures only. **178 passed, 0 failed** on my allowlist
(190 total including the 12 pre-existing `ai_doubles.c` failures). No source
change: nothing here proved a mispricing that a constant should move, and two
edits that did not prove out were reverted rather than landed.

## 1. Consecutive Protect

The path exists and is reached. `ev->protectChance[actor][index]` is
`100 / GetConsecutiveMoveSuccessDenominator(uses)` — the native capped
denominator, 33/11/3 for repeated modern guards — and `EvaluatePairBoard` both
charges `PAIR_GUARD_TEMPO` for the spent action *and* skips the whole denial
when the roll fails. `PairPlanScore` adds a further −35 for a repeat after a
successful guard. Wide/Quick Guard and Crafty Shield correctly reset `uses`.

What I could not build is a stable fixture. On every board where the second
guard is the AI's choice, the first guard is also a coin-flip against an
attack, and the board flips between "guard twice" and "never guard" on
changes as small as adding a parametrization. The two Alan cases (Rhydon T1→T2,
Golem T4→T5) share a shape I could not separate from correct play: when the
incoming attack is lethal every turn, a 33% shield is worth more than any
attack the body has, and the AI is choosing the gamble rather than ignoring the
odds. I would want the actual pre-decision board state from the receipt — both
sides' HP, the AI's alternatives and what was known — to say whether those two
decisions were wrong rather than unlucky.

## 2. A move the target is immune to, over coverage that is not

**Does not reproduce**, and the fixture is now green: Greedent with Body Slam
and Crunch, facing a Cofagrigus and a Sableye that are both guarding, chooses
Crunch. My first attempt failed only because I left a one-HP Magikarp on the
board — Body Slam aimed at the killable Normal target, which is correct.

So the common cause the receipts point at is not target-effectiveness scoring:
the trial reads damage from `simulatedDmg`, and a zero there is a zero in the
board value. What the Randall board has that mine does not is that the only
target was **alternating Protect and Will-O-Wisp**. Against a foe forecast to
guard, every attack's expected damage collapses toward zero, the actions tie,
and a tie is settled by terms that have nothing to do with the type chart. That
is my best reading of the shared cause behind Randall, Dhelmise, the
Choice-locked Thunderbolt, Meloetta into Spiritomb and Lanturn's Hydro Pump,
and it predicts the receipts' common feature: they are all boards where the
chosen move was *also* worth roughly nothing that turn. The fix would be to
break attack-versus-attack ties on unconditional expected damage — the value
the action would have if the guard fails — rather than on the guarded
expectation alone. I did not land it; it touches the ranking every fixture in
the suite depends on, and it deserves its own pass.

## 3. Self-damage that kills the user

Half fixed already, half a real bug, now isolated exactly:

* A **single-hit** Life Orb attack that the orb would kill the user with is
  already refused at two HP, and taken with room to pay. Both arms are pinned.
* A **multi-hit** attack from the same body on the same board is still taken,
  and the user dies. This is Cinccino's Bullet Seed and Tail Slap, and Skill
  Link sets are where it will keep showing up. It is not an underpriced death:
  I raised the self-knockout cost to 1000 and the choice did not move, so the
  orb payment is **not reached at all** on the multi-hit path. The accumulation
  it depends on is `if (lifeOrb && damage.affectsTarget)` in the target loop of
  `EvaluatePairBoard`; widening it to `damage.maximum` for non-single-hit moves
  did not change the outcome either, so the gap is further upstream than that
  flag. Next step for whoever picks this up: instrument `orbHitChance` and
  `singleContactOrb` on a multi-hit action rather than guessing, as I was.

## 4. The asserting-getter audit

`abf52611d3` is pulled into my working view. Every other
`GetMoveProtectMethod` call in `battle_ai_main.c`, `battle_ai_util.c`,
`battle_ai_pair.c` and `battle_ai_switch.c` is already guarded by an
`EFFECT_PROTECT` (or protect-family) check on the same move before the call —
including the two that look unguarded at first read: the Encore branch in the
pair trial rejects a non-protect last move a few lines above, and
`ProtectChecks` receives the guard move itself. `GetMoveWeatherType` and
`GetMoveTerrainType` are switch-based and do not assert. New fixture:
`EC lethal choice: a guard is scored after an ordinary attack without asking it
for a method`.

## 5. Not reached in this group

Signature-move starvation (Coalossal/Salandit, Thievul, Cyclizar, Alcremie,
Numel, Scovillain), the smaller list (Araquanid's Wide Guard, Mega Banette's
Will-O-Wisp, Aegislash at 18 HP in Blade form, Shuckle's Helping Hand, Stunfisk
and Wobbuffet), and the whole 145–156 addendum (Indeedee's Helping Hand and
Follow Me, Type: Null's U-turn, Duncan's lead Mega, Volbeat). The Araquanid and
Indeedee items look like group B and group A shapes respectively and should be
cheap; the lead-Mega horizon and Follow Me are new ground.

## The 12 pre-existing `ai_doubles.c` failures, one line each

| Line | Fixture | Shape |
| --- | --- | --- |
| 309 | Rock Tomb earns a partner crossing 2/2 | expected the Cloak target untouched, took 182 — a drop was aimed where it cannot land |
| 351 | Iron Defense mitigates only later physical attacks 2/2 | HP 334 vs 704 — the setup was taken on the turn the fixture wants the attack |
| 393 | prevent a priority hit from activating Defeatist 1/3 | HP 512 vs 98 — the priority hit was taken into the Defeatist threshold |
| 483 | Quiver Dance mitigates only later special attacks 2/2 | expected Bug Buzz, got Quiver Dance — setup chosen on the dangerous turn |
| 576 | offensive drops protect a slower partner 3/6 | expected Protect, got Psychic — the guard lost to damage |
| 690 | candidate Trace copies deterministic abilities 2/3 | expected a MOVE, got a SWITCH — the candidate search left instead |
| 854 | Cotton Guard earns only timely mitigation 2/3 | expected Double-Edge, got Cotton Guard — setup on the wrong turn again |
| 906 | Feint opens partner damage through Protect 1/3 | expected Protect, got Double-Edge |
| 1181 | Acid Spray enables a later special attack 2/3 | expected Sludge Bomb, got Acid Spray — the drop beat the attack |
| 2659 | candidate evaluation restores board caches 5/5 | HP 9 vs 16 — restoration leak or a different chosen action |
| 2878 | damage-based recoil distinguishes unsafe attacks 2/3 | HP 85 vs 98 |
| 2978 | preserve the Plus Minus partner 1/2 | HP 269 vs 364 |

Six of the twelve are one shape — **a setup or support move now beats the
direct attack on a board that was written to expect the attack** (Quiver Dance,
Cotton Guard, Acid Spray, Iron Defense, and the two HP mismatches that follow
from the same substitution). That is my setup pricing meeting fixtures written
before it, and whoever owns that file has to decide case by case whether the
fixture or the value is wrong. Three more are **a guard expected and an attack
chosen** (offensive drops, Feint), one is **a switch where a move was expected**
(Trace), and the remaining HP equalities follow whichever action changed.

# Group J item 1 — the immune-move family, and what is actually shared

Commit `a3122da86c`, fixtures only. **181 passed, 0 failed** on my allowlist.

**The tie-break is not needed, because the tie does not happen.** Three more
receipt boards are now fixtures and all three are green with no source change:

* Randall's board over three turns, with both Ghost targets alternating a
  shield and a burn so that every attack's guarded expectation collapses —
  Greedent still takes Crunch every turn.
* Luis's board — Lanturn takes the doubled Ice Beam over the halved Hydro Pump.
* Norman's board — Meloetta takes Shadow Ball over Psychic into a fresh
  Spiritomb.

So my group I hypothesis was wrong. The trial reads damage from `simulatedDmg`
and a zero there is a zero in the board value, guard forecast or not.

**What is shared is non-determinism, and the cache fixture proves it.** I
instrumented `EC expert pair: candidate evaluation restores board caches field
and RNG`, which evaluates the *same* position twice and compares. Four of its
five arms return identical scores. The fifth — the Dancer arm, the most
expensive board in the file — returns **16 then 9**, with the second evaluation
consuming 12 frames where the first took 18. The board did not change; the
search depth did.

The mechanism is one clause. `EvaluatePairBoard`'s inner loop stops on
`if (!mustFinish && (canStop || shortCount != 0) && PairDecisionBudgetExpired())`.
`AI_EvaluateDoublesCandidate` deliberately passes `canStop = FALSE`, but
`shortCount != 0` re-admits the wall clock as soon as one pair is shortlisted.
A switch candidate is therefore scored to whatever depth the frames allow, and
compared against a stay board scored to a different depth — the comparison the
comment at `settle:` says must never happen. In a twenty-turn battle with full
benches that is every turn; in a two-turn fixture it is never, which is exactly
the distribution of the receipts.

**The bounded fix is not landed, and here is why.** Requiring `canStop` makes
the cache fixture pass — it is the only thing that has — but the Dancer budget
board goes from 65 to **79 frames**, over the 1.2 s limit. Replacing the clock
with a deterministic pair count for candidate searches trades one against the
other and I could not find a value that holds both: at 64 pairs my whole suite
is green and the Dancer board is still 79; at 48, 79 and Laura's pivot moves;
at 32 the Dancer board comes back to 67 but Cristian, Laura, Charm and Cherubi
all move. A flat cap is the wrong shape. The right one is to give a candidate
board the same depth the stay board used — count the pairs the stay search
examined and hand that number to each candidate — which needs the two searches
to share a counter and is a proper change rather than a constant.

Until then: `EC expert pair: candidate evaluation restores board caches field
and RNG` is a real defect in the search, not a stale fixture, and it should be
fixed before the other eleven in that file are re-derived.

# Terastallization removed from the AI files

Commit `2c5df8a726`. Deleted `DecideTerastal`, `ShouldTeraFromCalcs`,
`struct AltTeraCalcs` and its four macros, the whole tera damage-alternative
pass, the `AI_FLAG_SMART_TERA` table entry and call site, the `GIMMICK_TERA`
arm of `ReconsiderGimmick`, the two `GIMMICK_TERA` checks in the scorer, the
one in the pair search, and both tera chance constants — 277 lines. `GIMMICK_MEGA`
and the Z-move path are untouched; `ReconsiderGimmick` keeps its Z-move body and
the pair search still enumerates mega masks with `PAIR_MEGA_HORIZON`. No AI
fixture Terastallized, so none needed removing. 181 passed, ROM clean.

## Still open from J and K

J2 (multi-hit Life Orb, isolated in group I but not fixed), J3 (the 145–156
addendum and the group I smaller list), J4 (the remaining eleven `ai_doubles.c`
fixtures — note that the twelfth, the cache one, is a real bug per above), J5
(signature starvation), and all of K including the Alan events.jsonl read and
the downgraded Mega item.
