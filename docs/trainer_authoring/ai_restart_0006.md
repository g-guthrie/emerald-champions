# E0006 Billy — executable-AI review complete, 9/10 intention

Historical flat-stat record. Current completion and evidence:
[final Gen9 practical pass](e0006_gen9_2026_09_09.md).

September9,2026. Bounded native scenarios, not exhaustive battle certification.
Actual TRAINER_BILLY Normal
zero-badge native generation gives all four members level12:

| Pokemon | HP | Atk | Def | SpA | SpD | Speed |
| --- | --- | --- | --- | --- | --- | --- |
| Lotad | 37 | 13 | 15 | 50 | 20 | 51 |
| Wingull | 51 | 13 | 15 | 58 | 15 | 47 |
| Ditto | 69 | 20 | 57 | 20 | 22 | 18 |
| Tandemaus | 39 | 52 | 19 | 16 | 19 | 63 |

Compiled team, all13 distinct native preparation moves and four abilities were
reviewed. Retained E0006's current team during this restart. Lotad precedes Wingull
at initial Easy/Normal/Hard:50>43,51>47,53>51. Manual rain plus Swift Swim and
Hurricane is new among completed E0001–5; Ditto and Tandemaus bring distinct
reactive copying and contact pressure, not additional rain abusers.

## Actual initial decisions

Six unrestricted lead scenarios (three native player pairs, one/two turns):

- Bulky Timburr/Eevee: Rain Dance/Hurricane removes Timburr; Eevee knocks out
  Wingull. Tandemaus enters, then Muddy Water/Population Bomb removes Eevee.
- Pachirisu/Eevee: Rain Dance/Protect, then Lotad Muddy Water while Wingull
  switches to Tandemaus. The repeated player attacks knock Tandemaus out and
  Wingull returns. This costly switch is recorded, not labeled optimal.
- Mienfoo/Eevee: actual Fake Out prevents rain, but Hurricane hits and removes
  Mienfoo; Eevee removes Wingull. Tandemaus removes Eevee next turn before
  Lotad's selected Rain Dance executes. This sample hit does not guarantee
  Hurricane outside rain, and the unexecuted rain is not an infinite setup loop.

Initial exact-reserve observations: Imposter copies the diagonal Timburr,
retains69HP/Scarf, copies31Atk/61Def/19rawSpeed and Iron Fist, starts copied PP
at5, and uses/locks Drain Punch. Tandemaus Protect then Super Fang under ordinary
pressure; after a genuine opposing Protect it instead selects Encore and
denies the next Ice Punch. In the Helmet third-turn follow-through, Tandemaus
has15HP and selects Population Bomb into28HP Pachirisu. Only three contacts
land for9 total damage before Helmet knocks Tandemaus out, leaving Pachirisu19.
This is the actual failure motivating the per-hit forecast repair; the former
ten-hit cache could credit damage after the attacking Pokemon had already fainted.

## Implemented repairs and native evidence

Weather forecast previously retained rain's100 Hurricane accuracy after another
actor replaced rain with sun, and mishandled Umbrella in changed-weather damage
and Speed. The scoped patch caches native weather-sensitive accuracy outside
pair evaluation and respects native Umbrella damage/Swift Swim rules. Forced
native controls: Wingull Muddy Water does21 without rain,32 with rain,21 with
defender Umbrella in either weather. Lotad Rain Dance, intermediate-Speed Natu
Sunny Day, then Wingull Hurricane has native50 accuracy; rain alone100; an
explicit attacker-Umbrella mechanics variant70. Forced hits establish sequence,
not empirical hit frequencies. Source was independently reviewed.

Candidate entry previously never applied Imposter. Shared native deterministic
Transform and diagonal eligibility now feed that forecast. Six candidate/native
comparisons match boosted Timburr, genuine Substitute failure and a swapped
diagonal Pachirisu; snapshot restoration passes. This is postpatch parity, not
an invented prepatch runtime result. A follow-up native phase review found that
copied Hospitality/Costar should still receive the later ally-entry phase.
That correction is implemented: native and hypothetical copied Hospitality
both heal injured Wingull from25 to37/51HP. Full board/party/BattleStruct/AI/RNG
checksum restores. The explicitly later-access Poltchageist is a mechanic
control, not an opening wild availability claim. Commander now checks original
party species and copied forms respect the native transformed-form exclusion.
These shared helper changes were independently source-reviewed.

Population Bomb's repaired producer captures native per-strike low/median/high
damage into a dedicated primary cache. The bounded paired walk uses actual
current HP, repeated accuracy, contact retaliation, Sash/Sturdy and Sitrus
between hits. Single-contact alternatives also pay Helmet cost. No native
damage calls run inside candidate pairs; completing the fixed ten-strike
producer bound can add calls versus the old early-KO short circuit.
Native current berry control: each hit does4 to47HP Pachirisu; without Sitrus
it ends7HP, with Sitrus it heals11 between hits and ends18HP. This demonstrates
timing, NOT a measured KO/no-KO difference in this particular parameterization.

The identical native three-turn counterfactual confirms the bad Helmet line:
Population Bomb ends Ditto26/Tandemaus0HP, player Pachirisu19/Timburr75;
Super Fang ends32/0 versus14/75; Protect ends32/15 versus28/75.
Bomb hits3+3+3; Helmet costs6+6+3, so the attacker faints after three hits.
That early self-KO also lets the player's later Ice Punch retarget Ditto.
Super Fang does more damage without that additional ally injury; Protect
retains Tandemaus. These are forced comparisons alongside an actual AI-selected
Bomb failure, not prescribed future opponent commands.

Ice Beam-to-Protect experiment, eight native one/two-turn parameters:
under Mienfoo/Pachirisu both menus first choose Rain Dance/Hurricane. Lotad
survives at6HP, Wingull and Mienfoo faint, Pachirisu remains77HP. Turn2 original
Giga Drain/Population Bomb ends Lotad14/Tandemaus21 versus Pachirisu50; the
Protect variant instead uses Protect/Super Fang, ending6/21 versus58.
Under Natu/Pachirisu focusing Lotad, both first choose Rain Dance/Protect and
finish Lotad4/Wingull51. Turn2 original Ice Beam/Hurricane leaves Lotad fainted,
Wingull23 and replacement Tandemaus39, versus Natu4/Pachirisu77. The Protect
variant's Protect/Muddy Water preserves Lotad4/Wingull46 versus Natu36/Pachirisu45.
Protect offers genuine preservation but is not a universal improvement. Retain
Ice Beam's reliable single-target coverage rather than sacrifice it on these
mixed outcomes; those sampled outcomes are not comparative win rates.

Actual repaired reserve choices now avoid the bad Helmet sequence from its
beginning: Ditto targets Timburr with Drain Punch rather than repeatedly paying
Helmet on Pachirisu; Tandemaus uses Super Fang then Protect and remains15HP.
At turn3 the pair has54/15HP, versus26/0 before repair, with player39/63 versus
19/75. These are different trajectories, not the identical isolated final-turn
board. Against the non-Helmet control Tandemaus still selects Population Bomb;
against genuine prior Protect it still selects Encore. Copied Scarf attack
locks and PP remain native.

The contact probability model was independently reviewed. Whole-move guards
and prior survival are not raised to the tenth power with per-hit accuracy.
Actor/target HP merges account for the other's prior survival, using bounded
quotient/remainder arithmetic; uncertain berry histories conservatively prevent
a second heal, and Unburden is credited only on certain-consumption branches.
Three damage anchors (1:2:1), initial Substitute/Disguise/Ice Face fallback,
HP-dependent modifiers after different earlier healing, and retargeting after
hypothetical earlier KOs remain explicit approximations, not a full simulator.

Final executed checkpoint:10 native groups/39 parameters pass in one current
build, separating actual AI decisions from forced mechanic/candidate comparisons.
The 16 retained shared paired-AI groups also passed on these production changes.
No release ROM, full battle win-rate or whole-game claim. Temporary work/
probes are investigations, not a new permanent per-trainer suite.
Actual Billy with a six-member native player party measures34 GBA frames for
the warmed paired decision and41 including a full AI cache rebuild (~0.7s).
These are observed board timings, not campaign-wide maximum guarantees.

## Intention rating and remaining improvements

9/10: same-turn manual rain/Hurricane, Swift Swim speed conversion, reactive
Scarf Imposter and contact/Encore pressure give this encounter several distinct
decisions and concrete counterplay. This rating judges the intended encounter,
not perfect execution. Below10: improve reserve preservation and longer-horizon
switch evaluation (the observed Tandemaus sacrifice remains costly); a stronger
rain-linked reserve could improve weather continuity but would trade away the
current independent reactive modes. The bounded model's documented mechanical
approximations remain limitations, not certified optimal play.
