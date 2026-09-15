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
