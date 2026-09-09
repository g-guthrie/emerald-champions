# E0021 — Jose: bounded executable-AI review complete

Intention rating: **8.5/10**, not a measured win rate. The native Route116 script
calls `trainerbattle_double TRAINER_JOSE`: one current branch/encounter. The
compiled team, levels, Stat Points, items and executable SETUP|TAILWIND|REDIRECTION
plan are retained after native comparisons. This supersedes the earlier
source-only review; it does not make that prose executable authority.

## Actual team

Normal zero-badge native generation gives all four level 12. Stats below are
HP/Attack/Defense/SpAttack/SpDefense/Speed before held-item modifiers.

| Pokémon | Native stats | Item / ability | Moves |
| --- | --- | --- | --- |
| Dustox | 76/13/32/25/30/61 | Black Sludge / Shield Dust | Tailwind, Bug Buzz, Protect, Moonlight |
| Beautifly | 50/18/23/67/23/58 | Focus Sash / Swarm | Quiver Dance, Bug Buzz, Air Cutter, Giga Drain |
| Kricketune | 46/61/20/18/20/61 | Life Orb / Technician | Bug Bite, Aerial Ace, Taunt, Protect |
| Venonat | 72/18/24/18/21/51 | Eviolite / Compound Eyes | Rage Powder, Sleep Powder, Struggle Bug, Protect |

All 16 moves, configured abilities and four 66-point spreads were checked against
native species/preparation code. These are customized species: Beautifly has
base 85 Speed/110 SpAttack, not vanilla stats. Dustox acts before Beautifly;
it does not automatically act before every opponent. Native neutral Compound
Eyes Sleep Powder accuracy is 97%, not guaranteed. Bug Buzz is single-target;
Air Cutter and Struggle Bug are the spread attacks. Shield Dust blocks applicable
secondary effects, including Fake Out's flinch, not primary Taunt.

## Executable repairs

- **Sleep Powder:** extend the existing Spore/Sing paired cache and execution
  path to this move. Previously its native raw preference existed, but the
  evaluator discarded its immediate ability to stop a later attack. Native
  Compound Eyes accuracy now weights the existing effect pass. Grass/Overcoat/
  Goggles, Substitute and immediate sleep-cure exclusions are retained; the
  shared primary-status gate rejects Magic Bounce, Magic Coat, Good as Gold and
  other supported prevention before granting denial. Sleep Clause remains off
  in current production; no rule change was made.
- **Quiver Dance:** reuse the existing Cotton Guard per-move scalar cache for
  native self-defense deltas: Defense for Cotton Guard, Special Defense for
  Quiver Dance. Apply the delta only after the actor acts, with native stage
  eligibility, Simple/Contrary adjustment, Snatch exclusion, paralysis effect
  weighting and existing damage-stage bypass/critical handling. Future offense
  and Speed remain native heuristic value; this repair adds immediate defense,
  not a full second-turn solver. No new matrix or allocation; both repairs use
  the existing bounded action/effect passes.

Both repairs have observed decision differences, not just source inspection.
Disabling only Sleep Powder's cache recognition makes its generic expected
choice fail (Protect replaces useful sleep). Disabling only Quiver Dance's
cache makes the actual fast-special lead choose Bug Buzz instead of Dance, and
also fails the independent generic timing boundary. Restoring each passes.

## Native evidence

Final run: **23 actual-team battle parameters plus 5 synthetic controls across
8 groups**, all passing. Actual parameters comprise six lead, six reserve,
six denial/transition, four timing/calibration and one additional special-pressure
observation. Trainer choices are unrestricted. Constructed reserve snapshots
promote the real Kricketune/Venonat with the original leads fainted; they do not
claim the opening naturally reaches that exact board. A diagnostic PASS alone
does not establish tactical correctness or an entire-battle win.

- On the actual Electrike/Lotad lead board, native Electrike Speed 59 lies
  between Dustox 61 and Beautifly 58. Dustox uses Tailwind; Beautifly executes
  Quiver Dance before the incoming Flamethrower, ending turn 1 at 30 HP and
  +1 SpAttack/SpDefense/Speed. Bug Buzz/Giga Drain follow on turns 2 and 3:
  Electrike falls on turn 3, Beautifly ends at 38 HP, Dustox at 73. With the
  Quiver cache disabled on the same board, Beautifly instead starts Bug Buzz.
- Against ordinary Pachirisu/Timburr reserve pressure, the old pair guards
  together on turn 1, then uses Aerial Ace/Rage Powder. The repair produces
  Aerial Ace/Sleep Powder immediately: Pachirisu sleeps, Timburr ends at 43 HP,
  Kricketune at 29 and Venonat at 72. Aerial Ace finishes Timburr on turn 2;
  Kricketune survives at 25 HP and Pachirisu remains asleep.
- Against Grass Lotad/Pachirisu, Venonat sleeps Pachirisu rather than targeting
  immune Lotad, while Bug Bite removes Lotad on turn 1. Kricketune remains at
  42 HP, versus 30 in the earlier Struggle Bug line. The follow-up uses Struggle
  Bug to lower Pachirisu's SpAttack. Powder immunity does not prevent Bug damage.
- Faster Mienfoo Fake Out genuinely interrupts Venonat, but Kricketune attacks
  rather than joining an empty guard. On turn 2 Rage Powder acts before Taunt;
  Kricketune removes Mienfoo, while Venonat takes redirected Thunderbolt and
  survives at 55 HP. A Taunt applied after Rage Powder does not retroactively
  cancel that turn's redirection.
- In the actual lead denial line, Fake Out interrupts Beautifly while Dustox
  sets Tailwind. Air Cutter pressure follows; a later Dustox switch preserves
  it while Beautifly at 10 HP uses Bug Buzz to remove Pachirisu on turn 3.
  The isolated Grass reserve denial line removes both player members by turn
  3. This is an endgame observation with two supplied player members, not a
  full six-versus-four campaign balance verdict.
- Generic Sleep Powder controls select it at native cached 97% accuracy, but
  select Protect against Goggles or Grass. No guaranteed hit/proc is asserted;
  the partner's unrelated Taunt choice is not locked by the retained check.
- Generic Quiver Dance timing: against a slower special attacker the AI
  dances and survives at 14 HP; against the faster attacker it uses Bug Buzz
  and survives at 2 HP, with no defense boost. This fixture deliberately has
  synthetic stats and a two-move menu, not an authored trainer loadout.

Final four/six-player decision timing is **36 warm frames / 41 with cache
rebuild**, unchanged from the sampled baseline. Native Easy/Hard lookup fallback
generates level 10/14 teams. All Jose scratch includes were removed; the clean
**34-group shared run passes**, including the two necessary new regressions,
transaction/RNG restoration, Room/weather, Cotton Guard and partner interactions.
Master projection verifies 516 branches; diff check passes. No fresh release
ROM, complete campaign integration or exhaustive playthrough is claimed.

## Rejected changes, variety and limits

Dustox Bug Buzz → Struggle Bug reduces opposing special attacks and takes a
different early knockout on the fast-special board, but by turn 3 leaves
Beautifly at 11 HP with no Quiver boost, versus the retained line's 38 HP and
boosted offense. Adding PRESSURE helps that candidate act earlier on one
physical board but does not improve the retained team's sampled choices.
Neither experimental change is retained. Keeping Bug Buzz also distinguishes
the lead's direct pressure from Venonat's existing spread-drop reserve role.

Dustox/Beautifly/Venonat debut here. Beautifly and Venonat occur only here in the
compiled catalogue; Dustox has four appearances. Kricketune appears three times
and exactly repeats James's current set. Its new redirection/sleep partner gives
it a different context, but this is real repetition, not a uniqueness claim.
Native early Bonsly/Rockruff Rock attacks, Natu's spread Heat Wave and Woods
Foongus's powder immunity/Clear Smog provide useful counterplay. Fire hurts all
four; Rock is especially dangerous to Beautifly. Their coverage prevents those
choices from being universal answers.

To exceed 8.5: improve the slow-opponent lead's unnecessary Tailwind/guard
sequence, reduce recurring Kricketune loadout repetition, and demonstrate more
natural lead-to-powder transitions and long endgames. Moonlight retains native
raw recovery opinion, not a complete weather-sensitive paired healing forecast.
An inherited positive-priority sleep/terrain forecasting gap remains outside
this actual priority-zero Compound Eyes user; this review does not certify all
Prankster sleep users. The repaired native setup and sleep sequences work,
without claiming every matchup or every decision is optimal.

Restart progress: **25/516 branches, 20/468 encounters**. E0022 has no authored
branch; the next current record is E0023.
