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
