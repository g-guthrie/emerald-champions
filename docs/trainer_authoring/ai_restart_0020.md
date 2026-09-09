# E0020 — Joey: bounded executable-AI review complete

Intention rating: **7.5/10**, not a measured win rate. Route116's native script
calls `trainerbattle_double TRAINER_JOEY`: one branch and one encounter. This
supersedes the previous source-only review. Progression, party size, species,
moves, items and Stat Points are unchanged in this restart.

## Actual team and accepted changes

Normal zero-badge native generation gives all four level 12. Stats below are
HP/Attack/Defense/SpAttack/SpDefense/Speed, before held-item modifiers.

| Pokémon | Native stats | Item / ability | Moves |
| --- | --- | --- | --- |
| Pawmi | 38/52/13/16/14/60 | Focus Sash / Iron Fist | Fake Out, Nuzzle, Mach Punch, Wild Charge |
| Lechonk | 52/56/20/15/19/35 | Eviolite / Thick Fat | Headbutt, Play Rough, Yawn, Protect |
| Tandemaus | 39/52/19/16/19/63 | Wide Lens / Own Tempo | Population Bomb, Super Fang, Encore, Protect |
| Zigzagoon | 36/51/18/13/18/55 | Figy Berry / Gluttony | Belly Drum, Extreme Speed, Seed Bomb, Protect |

All 16 moves, selected abilities and 66-point totals were reviewed against
native code. Pawmi is pure Electric: Mach Punch gets Iron Fist, not Fighting
STAB. Tandemaus has Own Tempo, not Technician; Own Tempo blocks Intimidate.
Lechonk's Speed 35 crosses many, not all, paralyzed attackers. Adamant Zigzagoon
does not dislike Figy's spicy flavor. Existing native Population Bomb forecasting
already handles individual accuracy/contact hits and early termination; this
review did not add an assumed ten-hit damage model.

- **Shared executable repair:** `PairTrySitrus` becomes `PairTryHealingBerry`.
  The paired forecast previously charged Belly Drum HP but omitted Figy-family
  recovery. It now uses native floor-half Gluttony / floor-quarter ordinary
  thresholds, configured healing amount, Ripen, effective held-item suppression,
  Heal Block and surviving opposing Unnerve/As One. Disliked flavors remain
  conservatively excluded because their confusion continuation is not modeled.
  Sitrus retains its separate native ceil-half exception. Supported berry removal
  prevents later phantom consumption; healing also runs after supported incoming,
  contact and recoil damage. No new allocation, matrix or extra search pass.
- **Individually authored executable preference:** SETUP becomes SETUP|PRESSURE,
  synchronized in canonical input, master branch and compiled plan table. On the
  Pachirisu/Lotad lead board, Pawmi now uses Wild Charge alongside Headbutt on
  turn 2: Lotad faints, Pawmi survives at 7 HP. Previously Mach Punch/Headbutt
  left Lotad at 1 HP and Pawmi at 14. This accepts a useful recoil trade, not a
  forced move sequence. Existing Fake Out and necessary protection remain.

## Native evidence and rejected candidates

The final actual-team run executes **30 battle parameters plus 3 synthetic berry
controls across 10 groups**, all passing. Actual-team parameters include six
lead, four reserve, four timing/calibration, three faster-Fighting, six constructed
mixed-phase, four Drum and three sleep-window observations. Two Drum controls
explicitly prescribe moves to establish native mechanics; other trainer decisions
are unrestricted. A diagnostic PASS alone is not tactical correctness.

- Actual leads Fake Out Timburr while Lechonk uses Play Rough; their follow-up
  finishes Timburr on turn 2. Faster Inner Focus Mienfoo can interrupt Pawmi's
  Fake Out, but Play Rough and Mach Punch still apply real damage. These are
  legal counters, not scenarios the trainer must automatically win.
- In a constructed healthy Pawmi/Zigzagoon phase versus Eevee/Lotad, Pawmi
  uses Nuzzle on turn 2 alongside Extreme Speed, survives at 1 HP through Sash,
  then voluntarily switches to Lechonk on turn 3; no trainer faints occur.
  Against Pachirisu/Lotad, Fake Out/Extreme Speed followed by Wild Charge/
  Extreme Speed removes Lotad on turn 2. Zigzagoon consumes Figy after native
  damage and survives at 10 HP. The constructed starting pair is not proof
  that the original unrestricted opening naturally reaches this exact state.
- Actual Tandemaus/Zigzagoon reserves still double-Protect first under the
  sampled awake Fighting pressure. Population Bomb/Extreme Speed follow,
  but Tandemaus is vulnerable to Drain Punch. With both foes explicitly given
  a valid carried sleep countdown, the pair instead attacks immediately,
  leaves Timburr 3 HP on turn 1 and finishes it on turn 2 while Super Fang
  pressures Pachirisu. This is a hypothetical Yawn payoff state, not a claim
  Joey naturally produced both sleeps. Production Sleep Clause remains off.
- Actual native Drum costs 18 of Zigzagoon's 36 HP. Without Figy it ends at
  18; Figy/Gluttony restores 12, ending at 30 with +6 Attack, consumed item and
  no confusion. These are prescribed mechanic controls. **Unrestricted Drum
  setup/payoff was not observed in the sampled pressure or sleep scenarios.**
- The isolated generic recoil boundary selects Double-Edge only with usable
  Figy, ending at 28 HP from 19. No item and live Unnerve select Protect at
  19; Unnerve retains the berry. Disabling only the flavor-berry forecast makes
  the useful attack assertion fail; restoring it passes. This justifies one
  small shared regression, not another permanent per-trainer suite.
- Encore → Helping Hand did not improve the sampled split-pressure choices;
  retain Encore and its existing distinct control role. Zigzagoon's candidate
  18 HP/32 Attack/16 Speed improves the passive-opponent sequence but loses
  both reserves by turn 2 under split pressure; reject it. Tandemaus's same
  balanced allocation reaches 55 HP/46 Speed but still dies to Drain Punch
  and does not improve initiative; retain original fast investment.
- Removing the extra generic positive Protect reward also leaves actual
  Joey choices unchanged. That experimental shared change was reverted;
  no gameplay adjustment was retained merely to satisfy a test.

Final timing: 54 warm frames; 60/61 with cache rebuild for four/six distinct
player members. This is bounded local measurement, not a campaign latency or
freeze guarantee. Native Easy/Hard fallback generates all four at level 10/14.
All Joey diagnostic includes were removed. The clean **32-group shared run
passes**, including the new berry boundary, state/RNG restoration, Room, weather,
partner and switching interactions. Master projection verifies 516 branches;
diff check passes. These are not individual reviews of the remaining branches.

## Variety and remaining limits

Pawmi and Lechonk debut uniquely here in the compiled catalogue. Tandemaus repeats
Billy's full set; Zigzagoon repeats Calvin's moves but has fast Figy/Gluttony
instead of that bulky Sitrus build. No species overlap with the immediately
preceding three encounters. Early Mienfoo/Timburr, Woods Ferroseed and Route116
Dreepy/Nincada supply Fighting, contact, Ghost and Ground counterplay; none is
claimed a universal answer to the team's coverage.

To exceed 7.5, make the lead-to-Drum opportunity more deliberate, reduce remaining
empty reserve guards and distinguish Tandemaus more from Billy without losing
useful control. The current one-turn planner has only heuristic future setup
value; the raw damaging-setup safety check does not fully credit sleeping foes.
Actual sleep-window attacks are productive, so no forced Drum rule was added.
Yawn and Headbutt retain native/raw AI support, not complete delayed-sleep and
probabilistic-flinch paired-state forecasts. This is a repaired fast pressure
team with a conditional setup option, not a demonstrated elaborate setup chain.

No fresh release ROM, full integration or exhaustive playthrough is claimed.
Restart progress: **24/516 branches, 19/468 encounters**. E0021 is next.
