# E0017 — Cyndy: bounded executable-AI repair complete

Historical pre-final-stat evidence below. Current stats, Sitrus/Protect revision
and native review: [September9 final Gen9 review](e0017_gen9_2026_09_09.md),8.5/10.

Intention rating: **8.5/10**, not a measured win rate or a full-playthrough claim.
Authority is the compiled party, native generation and battle code. Route115's
object at15,50 invokes `trainerbattle_double TRAINER_CYNDY_1`; the first encounter
is this record, while later Match Call tiers remain outside its branch count.
No map position, progression gate, party size or level calibration changed.

## Accepted team and executable changes

Normal zero-badge native generation gives all four level12:

| Pokémon | HP/Atk/Def/SpA/SpD/Spe | Item, ability | Moves |
| --- | --- | --- | --- |
| Riolu | 37/57/18/15/18/60 | Focus Sash, Prankster | Coaching, Drain Punch, Crunch, Protect |
| Meditite | 34/50/21/16/21/60 | Life Orb, Pure Power | Fake Out, Drain Punch, Psycho Cut, Bullet Punch |
| Crabrawler | 69/66/24/16/20/23 | Eviolite, Iron Fist | Drain Punch, Ice Punch, Thunder Punch, Protect |
| Clobbopus | 69/62/25/18/20/16 | Eviolite, Technician | Power-Up Punch, Feint, Taunt, Protect |

All16 prepared moves and selected abilities are native-legal. All four66-point
spreads are retained. Displayed Attack/Defense are raw stats before item/ability
multipliers: Pure Power doubles Meditite's physical Attack; Eviolite multiplies
eligible Defense/SpD by1.5; Iron Fist boosts punches by1.2; Technician boosts
eligible low-power attacks by1.5. Riolu's priority Coaching precedes ordinary
attacks despite its Speed tie with Meditite, but not opposing Fake Out.

- **Meditite: Zen Headbutt → Psycho Cut.** Native Psycho Cut is70 power/100
  accuracy, elevated critical rate and noncontact. Zen was80/90 with20% flinch;
  accepting less noncritical power and losing flinch buys reliable accuracy and
  contact safety. Both sampled first-turn Timburr/Mienfoo knockouts remain.
  A separate actual-party noncritical control confirms Psycho Cut defeats the
  prepared81HP Timburr without Coaching; do not generalize that KO to all builds.
  Psychic Fangs was checked and rejected as unavailable to native Meditite.
- **Clobbopus: Leftovers → Eviolite.** It needs immediate survival to use its
  slow Power-Up Punch buildup. Native comparisons separate this change from the
  later pressure preference; no invented boost or arbitrary stat buff was used.
- **Cyndy: PRESSURE plan.** A contextual HP-trade preference removes sampled
  opening waits while retaining useful low-HP Protect and Coaching. It does not
  read pending player commands or force a predetermined combo.
- **Executable Feint repair.** Previously the pair forecast let Feint bypass
  Protect for its own damage, then still blocked the partner's later attack.
  The forecast now clears the affected ordinary guard and modeled side guards
  after an eligible landed hit into a surviving recipient. Feint's effect is
  native primary, so Covert Cloak/Shield Dust do not prevent this payoff.
  Ghost immunity/zero damage and a defeated target do not earn guard removal.
  Individual Max Guard is retained; this is not a rewrite of native Max Guard
  hit legality. No new matrix, allocation or recursive search was introduced.

## Native evidence and comparisons

Final actual-party run: **27 parameters across 9 groups**, all executed. Six lead
observations, eight reserve observations through four turns, four Fairy/burn
counterplay observations, four calibration/timing observations, four Feint
controls/observations and one noncritical Psycho Cut control. Except explicit
mechanical controls, Cyndy's entire compiled team and moves remain unrestricted.
Player menus are full legal preparations except the explicitly guard-only Feint
diagnostics. A diagnostic PASS alone is not a tactical correctness assertion.

- Baseline guards with native reserves: Crabrawler Protect plus Clobbopus
  Power-Up Punch makes no progress into the known Protect board. Repaired AI
  selects Drain Punch plus Feint, dealing37 total to Munchlax through its guard.
  The separate forced mechanical comparison into Pachirisu measures Feint5
  plus partner Drain Punch15 versus zero damage without Feint. Ghost Duskull
  remains65/65 behind Protect even when the partner attempts Ice Punch.
- Eviolite alone, before pressure: against Natu's Psychic, Clobbopus survives
  two hits at5HP and earns two Power-Up Punch boosts. Old Leftovers dies turn3
  without using a Power-Up Punch in that board. The initial Eviolite physical
  board gained an unwanted double guard, so the item was not judged on bulk
  alone; pressure was then compared separately.
- Accepted physical reserve board: Drain Punch/Power-Up Punch on turns1/2,
  Feint finishes Timburr on turn3, then Crabrawler Protect/Clobbopus Power-Up
  Punch on turn4. Final ownHP25/37, foe Pachirisu44/77; Clobbopus reaches+3Atk.
  This is one turn faster at removing Timburr than the original Leftovers/no
  pressure sequence, with the same final Pachirisu HP in these samples.
- Accepted Psychic reserve board: Ice Punch/Power-Up Punch turns1/2 leaves
  Clobbopus5HP/+2Atk; its turn3 Protect survives while Crabrawler attacks; turn4
  Feint plus Ice Punch removes Natu. Both reserves survive21/69 and5/69, with
  Pachirisu43/77. The original pair lost Clobbopus and left Crabrawler at1HP.
- Lead board against Pachirisu/Timburr now uses Coaching/Psycho Cut immediately,
  defeats Timburr, then Coaching/Drain Punch reaches+2Atk/+2Def. Against
  Pachirisu/Lotad, Coaching/Drain Punch defeats Lotad turn1, followed by
  Riolu Protect/Psycho Cut rather than exposing injured Riolu unnecessarily.
- Faster Mienfoo Fake Out interrupts Riolu's first Coaching; Meditite still
  defeats Mienfoo with Psycho Cut, then receives Coaching turn2. The lost
  priority support is not incorrectly counted as an executed boost in reporting.
- Counterplay is real: Ralts/Togepi Fairy pressure defeats Meditite on turn1
  and Riolu on turn2, causing actual Crabrawler/Clobbopus entries. This is not
  hidden or patched away by removing the quartet's shared weakness.
- Intimidate Natu/Growlithe board: Meditite Fake Out stops Natu's attempted
  Reflect while Riolu uses Crunch; Growlithe burns Meditite. Turn2 the AI
  switches the burned attacker to actual Crabrawler while Riolu protects.
  Reflect never landed in this sample; do not report merely attempted support
  as successful, or claim the incoming Air Slash was absorbed harmlessly.
- Native difficulty fallback gives Easy10/Normal12/Hard14 with the same team.
  Four-/six-player native lead decisions measure41 warm frames/47 including
  cache rebuilding (about0.68/0.78 seconds). The active Feint reserve board is
 13/18 frames. These are specific measurements, not global freeze clearance.

## Regression discipline and remaining limits

One three-parameter generic Feint regression is retained: ordinary guards,
Covert Cloak guards, and Ghost immunity. Its first synthetic candidate also
passed with the repair disabled, so it was not accepted as regression evidence.
Revised fixed-stat boundaries reproduce the native failed choice when the
guard-clear consumer is disabled, and the repaired implementation chooses the
coordinated pair. The final test has no trainer ID or campaign loadout lock.
All temporary Cyndy diagnostic includes were removed after final native runs.
The clean retained suite then passed all **29 shared AI groups**, including
Feint and board/cache/RNG restoration. Static trainer/master projection passed
for all516 configured branches; this proves synchronization, not individual
native review of that catalogue. Whitespace checks passed.

The guard forecast conservatively excludes an initially blocking Substitute;
it does not simulate that Substitute breaking and the following effect. Guard
counter reset in future turns, full independent probability correlations and
other moves sharing the native Feint effect are not newly modeled. The existing
one-turn evaluator also does not fully model Taunt's same-turn denial or general
Leftovers recovery; native item comparisons, not that incomplete forecast,
support this change. Do not claim those broader systems fixed by this encounter.

All four species first appear here in the authored catalogue. Compiled counts
are Riolu2, Meditite2, Crabrawler1, Clobbopus2; later uses have distinct items or
partner roles. Early accessible tools include Route102 Ralts/Natu/Togepi and
Route103 Growlithe/Wingull. Psychic is neutral into Meditite, not super-effective;
Fairy threatens all four. Further improvement would come from stronger
conditional responses to sustained Fairy pressure and burn, richer Taunt/guard
forecasting and lower latency—not simply erasing counterplay or raising levels.
No fresh release ROM or full-game completion is claimed.
