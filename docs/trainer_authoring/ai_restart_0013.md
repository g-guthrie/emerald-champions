# E0013 — Haley, Route104

Status: **bounded executable-AI review complete, intention8/10**.
Restarted completion is18/516 branches and13/468 encounters.
Current accepted team change: Snubbull Fire Fang→Fire Punch, synchronized in
canonical teams/master/compiled party; projection verification passes516.
Wish/Trace repairs have passed the final checks below. The initial
sections below are chronological pre-repair observations.

## Actual code and initial native evidence

TRAINER_HALEY_1 has no extra battle-plan flags. Actual full party is Eevee,
Ralts, Marill, Snubbull; Normal's current live cap makes all four level12,
despite stored11/12/11/13. Native baseline records show Eevee58 Attack/21 Speed;
Ralts51 SpA/55 Speed; Marill74HP/49 raw Attack/18 Speed; Snubbull72HP/64 Attack/
15 Speed. Huge Power is a damage-engine multiplier, not a doubled displayed
raw stat. Full native lead inputs remain to be logged.

All16 moves and four abilities are native legal; canonical/compiled agree.
Eevee repeats Tiana's exact set (also later Robin), Snubbull nearly repeats
Cindy, Ralts is unique, Marill first appears here. Compiled species occurrences
are3/1/3/2 respectively, not independently completed battle counts.

Four temporary actual-native reserve parameters pass in one group. Leads are
explicitly fainted and reserves start healthy; no prior Wish, paralysis or
Helping Hand is invented. Moves are unrestricted on the opponent side.

- Physical Eevee Double-Edge into Marill and Timburr Ice Punch into Snubbull:
  Brick Break/Fire Fang focus Eevee and KO it on turn1, own38/61HP. Turn2 both
  Play Rough into remaining Timburr, own38/50, foe29. Both sampled Play Rough
  Attack drops occur; this is not a guaranteed two-drop outcome. Snubbull's
  initial Intimidate also applies natively.
- Special Pachirisu Thunderbolt into Marill and Natu Psychic into Snubbull:
  both Protect on turn1 with no residual recovery. Turn2 both Play Rough KO
  Natu, own38/12, Pachirisu77. Healthy simultaneous waiting is a review target,
  not deemed correct merely because the diagnostic executes.

Initial ten-parameter lead/reserve run passes in two groups. The six lead
parameters use the full actual team against native early players:

- Bulky Pachirisu Thunderbolt/Eevee and specially bulky Timburr Ice Punch/Ralts:
  Eevee Protects while Ralts switches to Intimidate Snubbull. Turn2 Double-Edge/
  Play Rough, own53/50, foes55/37; all four alive. Preserving fragile Ralts and
  applying Intimidate are real native decisions, not evidence of Helping Hand.
- Ferroseed Gyro Ball/Ralts and Intimidate Growlithe Flamethrower/Eevee:
  Double-Edge/Psychic on turn1 leaves own43/1, Growlithe35. Ralts natively copies
  Intimidate and survives via Sash. Turn2 Quick Attack/Psychic finishes Growlithe,
  Ralts falls to Gyro Ball and Snubbull replaces it; own Eevee43/Snubbull72,
  Ferroseed72. This one-for-one is observed counterplay, not a won match.
- Mienfoo Fake Out then Taunt/Ralts and Pachirisu Thunderbolt/Eevee:
  turn1 Double-Edge with Ralts's selected Gleam interrupted; own47/22, Mienfoo32.
  Ralts copies Volt Absorb. Turn2 Double-Edge/Psychic finishes Mienfoo, own30/22,
  Pachirisu55. Taunt does not stop the selected attack; all four remain alive.

Eevee's actual HP/Atk/Def/SpA/SpD/Speed is70/58/22/17/24/21; Ralts is
34/12/14/51/17/55. Trace's copied ability is observed after native entry, not
assumed from its base Trace label.

Current temporary includes:work/haley_reserve_probe.inc,haley_lead_probe.inc
andhaley_wish_probe.inc. All14 parameters across four groups now pass; no
permanent tests added or new Haley gameplay changes accepted at this checkpoint.

Wish controls use Eevee35/70HP with native bulky Pachirisu Thunderbolt and Lotad
Protect; both players retain full legal menus. Pachirisu's Eviolite is inert,
not a claimed defensive boost. Forced Wish takes10damage and ends25HP with
counter1; forced Protect on turn2 allows35healing and ends60HP, counter0.
Ralts's forced Helping Hand into Wish is solely a non-damaging mechanics control,
not a sensible AI script. Ralts Psychic on turn2 leaves Pachirisu56HP.

Unrestricted same initial board: Eevee Double-Edge and Ralts switches to
Snubbull. Turn1 Eevee18HP/Pachirisu55; turn2 Double-Edge/Play Rough leaves
Eevee1HP/Pachirisu18, Snubbull72, other two reserves untouched, no Wish. This
trades immediate health for pressure; the healthier forced path does not alone
prove tactical superiority. Native future support still needs evaluation.

Next: isolate pending-Wish evaluation versus native healing (no future-turn
heal credited immediately), then a deterministic native/candidate Trace parity
comparison. Only accept changes after actual decision comparisons; retain the
healthy double-Protect reserve opening and reliable Fire coverage as authoring
review targets. This was the pre-repair checkpoint; see the continuation below.

## Source questions / possible authoring

Helping Hand, Trace, Adaptability, Huge Power, Intimidate and paralysis must
be assessed through actual decisions. Trace copies eligible opposing abilities,
not partner Eevee's Adaptability. Wish heals the user's battle position next
turn, not adjacent Ralts. Paired evaluation has no explicit Wish branch; native
controls and decision observations must precede a failure/fix claim. Source
review also finds pending counter1 Wish healing absent from pair scoring, while
the main doubles Wish opinion calls immediate ShouldRecover(50). Native end
turn handling uses the caster's maxHP and Heal Block. A missing model is not
alone proof that every unselected Wish was tactically better.

Secondary source gap: hypothetical switch candidates initialize Ralts to Trace
but do not apply native Trace copying; already-entered native Ralts is correct.
No bad switch has yet been demonstrated. A deterministic same-ability opposing
pair could isolate candidate/native parity without random-copy ambiguity.

Candidate Snubbull Fire Fang→Fire Punch: both legal,65power/95accuracy versus
75/100. Fang can flinch after a paralysis speed crossing, but zero Speed makes
unassisted flinch unreliable. Punch would improve reliability, not variety:
it would match Cindy's repaired coverage. No change accepted yet.

Real early counterplay includes Mienfoo Fake Out/Taunt/Drain Punch, Pachirisu
Follow Me/Super Fang/Thunderbolt and first-visit Woods Ferroseed's Normal/Fairy
resistance/contact punishment. These are conditional tools: Psychic threatens
Mienfoo and Fire/Brick Break threaten Ferroseed. Native Nidoran♀/Foongus Sludge
Bomb is available but must also respect Psychic.

## Executable repair continuation

Accepted Snubbull Fire Punch improves65power/95accuracy to75/100, retaining
10% burn but losing Fang's conditional flinch. Native14-parameter comparison
preserves the sampled decisions/outcomes, with Fire Punch replacing Fang in the
physical focus KO. This is reliable coverage, not a variety improvement; the
roster/abilities/points/natures/items/levels remain unchanged. All three data
representations are synchronized. Generated strategy stays NONE.

Pending Wish now contributes actual end-of-current-turn healing to paired
evaluation when counter1 and not Heal Blocked. It uses the caster's native
party maxHP (or configured pre-Gen5 recipient rule), current occupied slot,
native minimum1 and maxHP cap. Healing occurs only after surviving attacks and
recoil, never revives a fainted candidate, and does not avert Perish expiry.
Counter2 does not heal immediately. The existing finite one-turn model does
not fully simulate weather/residual ordering, new Heal Block or replacement
trees; this change is not certification of those combinations.

New Wish no longer borrows the immediate Recover opinion in pair scoring.
It grants no current HP. A successfully modeled cast can instead gain half
the useful future HP value after present attacks, weighted by survival, when
a legal self-Protect continuation is available. Half-value is an explicit
finite-horizon opportunity-cost heuristic, not native healing arithmetic or
a guaranteed future move. Choice locking, Gorilla Tactics, Truant, known
paralysis/new modeled paralysis, Yawn, known residuals (including burn), known
Protect bypass, Taunt, Encore, Heal Block and possible Snatch exclude this
narrow option. No speculative Wish passing or extra turn search is added.
Eligibility and caster HP are cached once per board (12 additional cache bytes).

Correctly initialized native post-Wish queries previously scored counters1/0/2
identically171. With repairs, due counter1 scores221 and attacks; absent0
scores173 and selects Wish; counter2 scores171 and attacks. These are identical
post-native board counterfactuals, not three naturally traversed battle states.
The earlier DOUBLE query lacked ordinary AI initialization; after correcting
that, its extra outer8272-byte snapshot caused diagnostic heap exhaustion.
Separate fresh parameters remove that artificial allocation. Neither fixture
problem was labeled a new gameplay freeze.

Actual injured full-party play now selects Wish on turn1, Eevee25HP, while
Ralts switches to Snubbull. Turn2 Double-Edge/Play Rough plus native35 Wish
healing gives Eevee43HP and Pachirisu40 (formerly Eevee1/Pachi18 after attacking
twice). Turn3 continues attacking: own26/72, Pachi3/Lotad71, no faint. Health
and damage are a tradeoff, not proof of a full win. Healthy lead follow-through
uses Wish on turn2 and attacks on turn3 while it heals: Eevee70/Snubbull42,
Timburr fainted, Pachirisu77. This is an actual healing payoff, not forced Protect.

Hypothetical Ralts entries now resolve a known deterministic Trace outcome
before existing entry-effect handlers. One eligible foe, or two with the same
eligible native raw ability, can be copied; native suppression, traceability,
activation and Ability Shield are respected. Unknown or two-different legal
outcomes remain unresolved, not a favorable chosen coin flip or exact expected
value. Actual entered Ralts still uses native random Trace. Native/candidate
comparisons agree on Volt Absorb and Intimidate, including the latter's extra
Attack drops from5/5 to4/4. Trial loaders may use RNG internally; the outer
snapshot must restore live state. A prior post-loader RNG assertion tested the
wrong contract and was removed while restoration assertions remain.

Support observations using full actual menus:

- Specially bulky Eevee/Pachirisu: Helping Hand boosts Double-Edge into Eevee;
  foe Eevee11/Pachi77, own39/20. Ralts copies Volt Absorb.
- Happiny/physical Eevee: direct Double-Edge/Psychic, own43/1 and foes88/1;
  Ralts copies opposing Eevee's Adaptability, not its partner's ability.
- Specially bulky Eevee/fast special Pachirisu: Thunder Wave into Eevee and
  Double-Edge into Pachirisu, which faints; own30/34, Eevee75 and paralyzed.
  This sample includes paralysis, not guaranteed denial of Bite every run.

An authored PRESSURE trial was rejected and restored to NONE. It removes the
healthy lead's first Protect but weakens recovery/Steel follow-through. Injured
turn3 ends Eevee2/Pachi25/Lotad51 instead of26/3/71. Special reserves still both
Protect first and now both Protect again on turn3. The accepted special reserve
sequence instead attacks on turn3, ownMarill20/Snubbull12, Pachirisu52/Natu0.
The initial both-Protect tempo loss remains an improvement target, not a claim
that the evaluator perfectly plans every phase.

Two necessary self-contained shared regression groups replace temporary probe
includes: Trace copying/Shield with restoration, and successful Wish with delayed
healing and relative due-heal value. No campaign trainer ID or team generator
is a dependency. Temporarily disabling Trace fails Volt Absorb parity; disabling
due healing fails the relative value assertion (266 versus269); disabling only
the future option changes Wish to Double-Edge and fails the decision assertion.
All mutations are restored. The mixed-Trace approximation is not retained as a
permanent expected-behavior lock. Existing21 shared groups also pass after repair.

Final current-team30 parameters in10 groups pass, followed by all23 retained
shared paired-AI groups after removing all seven temporary Haley includes.
Diagnostic scratch files remain unbuilt in work/. Projection and whitespace
checks pass; no negative mutation or plan trial remains active.
Measured native six-player/four-opponent decision timing is42 warm frames and48
including cache rebuild (about0.8 seconds at60Hz), with no extra outer snapshot.
This board repeats three Pachi/Timburr pairs; it is not a worst-case latency bound
or complete six-versus-four match. No release ROM or full-game completion claim.

Intention8/10: genuine support-driven mixed offense, native Helping Hand and
Thunder Wave alternatives, usable Wish recovery and distinctive Trace/Huge
Power/Intimidate roles. It is not a scripted combo that must be selected on
every board. Better than8 needs cleaner reserve-phase tempo (the healthy
both-Protect opening remains) and valuation of mixed Trace outcomes/longer
Wish passing. Repeated Eevee/Snubbull roles limit novelty; retaining the unique
Ralts and first Marill is preferable to a gratuitous species replacement.
