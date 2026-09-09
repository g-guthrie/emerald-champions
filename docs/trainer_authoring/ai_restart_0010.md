# E0010 — Petalburg Woods Aqua grunt

Status: **COMPLETE — intention rating8.5/10.** Restart completion is now
15 branches / 10 encounters. Actual compiled trainer ID10, four members:
Carvanha, Wailmer, Grimer, Poochyena. Native Normal levels12/12/13/12.

## Harness correction before accepting evidence

The first six lead observations were invalid: inherited synthetic trainer10
overrode this real trainer with a level100 Eevee/Wynaut fixture (Normal98).
Discard those observations. The native test catalog now appends only the two
still-used synthetic RED/LEAF entries at unused test-only IDs855/856; production
TRAINERS_COUNT855, MAX864 and partner/flag layout are unchanged. Removed13
unused synthetic identities (15 difficulty-specific blocks), the synthetic NONE
override and blanket duplicate-initializer warning suppression. Regenerated the
trainer header through Makefile. Explicit four-species guards now pass.

Previously completed restart trainers all have IDs above15 and their logged
native teams match; this collision did not replace their parties.

## Corrected baseline

Three native groups /16 parameters pass after the namespace correction:
six unrestricted lead decisions, four unrestricted reserve decisions, six
forced Water Spout/Scald mechanics controls. Separate shared16 regressions pass.
A diagnostic PASS establishes execution, not tactical quality.

Native stats (HP/Atk/Def/SpA/SpD/Speed):

- Carvanha:38/68/13/21/13/56.
- Wailmer:88/22/19/62/17/23.
- Grimer:79/67/24/17/22/15.
- Poochyena:66/18/19/15/15/53.

Actual unrestricted observations:

- Physical leads: double Protect turn1; turn2 Carvanha switches to Grimer,
  Wailmer repeats Protect unsuccessfully and faints to Double-Edge. Grimer72,
  replacement Carvanha38; player Eevee46/Timburr81.
- Special leads: double Protect turn1; Crunch/Water Spout turn2 leaves
  Carvanha1/Wailmer46, player Lotad69/Pachirisu61.
- Fake Out/Super Fang: Carvanha Protect/Wailmer Spout turn1 leaves own38/66,
  player Mienfoo34/Pachirisu53. Turn2 Grimer enters, Wailmer Protects.
- Both reserve boards double Protect turn1 with no Speed Boost payoff.
  Turn2 Shadow Sneak/Foul Play against physical pressure leaves own9/30,
  foes26/71. Shadow Sneak/Snarl against special pressure leaves own17/33,
  foes35/68 and both foe Special Attack stages5.

Forced native Water controls against the same Eevee:

| Earlier Wailmer chip | Attack-time HP | Water Spout damage | Scald damage |
| --- | --- | --- | --- |
| None |88/88|27|19|
| Super Fang44, Sitrus heals22 |66/88|19|19|
| Thunderbolt42, berry not triggered |46/88|15|19|

Spout also hits the other unprotected foe, so these figures do not imply Scald
is always superior after chip. They demonstrate the action-time HP dependency
missing from the old paired forecast.

## HP-power repair checkpoint

Native raw min/median/max at1HP and full HP are cached outside candidate loops;
survival clipping is excluded from anchors and applied at local action-time HP.
The scorer interpolates by native floored base power, with one HP snapshot for
the entire spread action. Both foes have anchors. Ordinary cached damage is
retained when actor HP is unchanged. Max/Z and nonlinear ability cases
(Technician, Defeatist, pinch abilities, Parental Bond) retain their previous
forecast rather than receiving an invalid straight-line approximation.

Twenty native point comparisons (two targets, five HP values, initial1/full HP)
show exact endpoints and at most1HP interior differences for this Wailmer.
Actor state, cached HP percentage and both RNG streams restore. This does not
establish exact roll histograms or support excluded ability breakpoints.
Actual special lead turn2 changes from Crunch/Spout to Aqua Jet/Scald; own
survival is unchanged1/46, Pachirisu ends44 rather than61, Lotad71 rather than69.
Fake Out/Super Fang still selects useful Protect/Spout.

Six-player timing initially exhausted heap because the diagnostic retained an
extra8272-byte snapshot around the already-snapshotted production evaluation.
Removed that diagnostic allocation, not production restoration; packed damage
anchors also save512bytes. Native timing then passes:37frames warm,44 including
cache rebuild. This is a repeated-call timing probe, not bit-identical external
state restoration or an exhaustive production memory certification.

## Design trials, not final choices

E10-only PRESSURE with original sets trades into the physical lead but worsens
the special reserve line: Protect/Foul Play leaves Poochyena18HP, then Grimer
faints next turn. It is not accepted on the strength of activity alone.
Wailmer's8HP-to-Speed trial produces80HP/31Speed versus88HP/23Speed; under PRESSURE
it moves before the bulky physical foes and enables a turn2 Eevee knockout,
trading Carvanha while Wailmer stays healthy. Against special pressure Sitrus
now activates after42 Thunderbolt damage, leaving58/80HP, with Spout selected.
The smaller HP/Sitrus pool is a real cost. Further reserve comparison pending.

## Integrated candidate checkpoint

Retained all species, moves, abilities, natures and levels. Final authored changes
are Wailmer24/0/2/32/0/8 points, Grimer Eviolite, Poochyena32/0/2/0/16/16 points,
and E10-only PRESSURE. Poochyena's actual Speed is36, not the rough37 estimate.
Two Eviolites are legal: campaign trainers do not have Item Clause. Drain Punch
supplies Grimer recovery without Black Sludge. These were compared individually
and together; aggression alone with the original defenses was rejected.

Selective E0010 master regeneration and materialization through encounter10
completed15 branches; all516 current master-party projections compare exactly.
This projection is artifact consistency, not all-battle review. Native final
25-parameter checkpoint passes against the regenerated team:

- Physical leads: Crunch/Protect turn1; Crunch/Spout turn2 trades Carvanha for
  Eevee while Wailmer remains80HP; Grimer enters79, Timburr48. The old team lost
  Wailmer after empty waiting, leaving Eevee46/Timburr81.
- Special leads: retain useful Speed Boost Protect turn1; Aqua Jet/Spout turn2
  leaves Carvanha1/Wailmer58, foes71/48. Faster Thunderbolt now triggers Sitrus
  before Spout, so selecting Spout here is consistent with its action-time HP.
- Fake Out/Fang: retain Carvanha Protect + Wailmer Spout, own38/60, foes34/53;
  turn2 Grimer enters70 and Wailmer Protects. No forced first attack.
- Physical reserves: Drain Punch/Protect turn1 leaves own46/66, foes16/81;
  turn2 Drain Punch/Foul Play KOs Eevee, own49/30 and Timburr64.
- Special reserves: Poison Jab/Snarl turn1 leaves own35/50, foes65/27 with both
  Special Attack stages5; turn2 Protect/Foul Play leaves own35/34, foes45/19.
- Full-party native timing:39frames warm,46 including cache rebuild (~0.77s).

The two-point isolated regression exposed a separate spread heuristic defect:
Water Spout was scored only against its nominal immune target even though the
other foe was damageable. The independent heuristic now uses that damageable
recipient; actual spread targeting and ally risk stay unchanged. The regression
passes both public-speed variants: slow Thunderbolt permits Spout, faster
Thunderbolt produces Scald. Disabling only HP adjustment makes the fast case
fail (Spout instead of Scald); disabling the spread correction had made the
slow case fail. Retained this one two-parameter regression, not the authored
team experiments.

## Completion and limits

Final integrated evidence:29 parameters across8 native encounter groups, plus
17 shared paired-AI regression groups. The last four observations confirm:

- At Carvanha1HP/Wailmer10HP, with either healthy or fainted reserves, both
  Protect; native Sitrus heals Wailmer to30. No reckless forced attack or added
  faint in the sampled turn. Both active HP values and reserve availability
  were scenario inputs, not a claim that every route to that position was played.
- Special reserve turn3 trades Grimer for Natu; Poochyena survives10HP and the
  poisoned Charmander has11HP. This is a sampled trade, not a completed win.
- Physical reserve turn3 Protects through Drain Punch while poison chips the
  remaining Timburr to54; Grimer49/Poochyena30 survive and Eevee remains fainted.

The initial continuation fixture accidentally auto-inserted Celebrate for the
already-fainted Eevee, exceeding its four-move set. Explicit SKIP_TURN corrected
that recording error; the final four parameters pass. Harness outcome5 at its
requested stopping point is not used as an independently verified battle win.

Removed all six encounter diagnostic includes and the temporary native-anchor
query entry point from build inputs. Scratch investigations remain in work;
only the distinct two-case shared regression was retained permanently.

Rating8.5 reflects an actionable Aqua pressure opening, Speed Boost/Sash choices,
Sitrus-aware Spout timing, and a genuinely supported drain/poison reserve phase.
It is not a win-rate measurement. A stronger endgame against fast physical
pressure and more deliberate multi-turn pivot planning would raise the rating;
the current bounded evaluator is not an exhaustive multi-turn search. The
nonlinear HP-power ability exclusions and interior rounding approximation above
remain explicit limits. No full campaign playthrough or release ROM claimed.
