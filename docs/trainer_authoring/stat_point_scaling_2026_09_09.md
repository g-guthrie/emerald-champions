# Level-scaled Stat Points — September 9, 2026

User-approved change: retain 66 points total, 32 per stat and effective IV31.
Replace flat bonuses with the main-series formula using `max(2P - 1, 0)`
as the EV/4 term. This matches Champions at level50, scales below and above50,
and applies to HP and all other stats before Nature. Levels/caps/difficulty
are unchanged. Shedinja and fainted-HP handling are unchanged.

`pokemon.c` owns native stats. Battle Dome's duplicate ranking/info calculation
now agrees, without its old eight-bit Nature-result truncation. The live editor
already recalculates and reads native stats; its help text was corrected. Saved
party loading already refreshes native stats, and boxed withdrawal recalculates.

## Quick AI recheck, not a new full campaign audit

Shared pair/switch/damage/Speed and authored-policy source review found no old
flat-SP arithmetic to compensate inside AI. Decisions use calculated stats.
Previous per-encounter stat numbers and exact outcomes are historical; this
change does not certify all26 previously reviewed branches under the new curve.

- **Joey:** move Zigzagoon's2 HP points to Sp.Defense, retaining all66 points,
  Attack32 and Speed32. Native level12 HP34, Attack25, Speed30. Forced mechanical
  Belly Drum costs17HP; Gluttony/Figy heals11 to28HP. Actual AI still uses
  Fake Out/Nuzzle support and productive sleep-window attacks, not a forced Drum
  script. Intention rating remains7.5/10; improve deliberate setup transitions
  and empty reserve guards. Easy10/Normal12/Hard14 HP parity is even for this spread.
- **Jose:** Beautifly changes10HP/32SpA/24Speed to26HP/32SpA/8Speed. At Normal12,
  Dustox34Speed precedes Beautifly33, restoring the intended Tailwind order.
  Against the sampled fast Electrike/Lotad board, immediate Bug Buzz removes
  Lotad rather than forcing the old Quiver Dance choice. Rating remains8.5/10;
  unnecessary Tailwind/guard sequences and repeated Kricketune design remain.
- **Clark:** no further team or AI edit needed for the sampled lead/reserve
  boards. Iron Defense→Body Press, native Stamina and Defeatist handling survive
  the curve change. Native evaluator warm/rebuilt32/38 frames with both4 and6
  player members, compared with prior31/37. Rating remains8.5/10; single-target
  special pressure and repeated Protect decisions remain improvement targets.

Player preset integration also required a fix: the first Zigzagoon Drum preset
has32HP points, giving47HP at cap14. Reducing to30 still gives47;29 gives46.
Parity normalization and recognition now share a fixed three-point bound.
Other stats are untouched. A shrinking level-based tolerance was rejected
because it loses recognition after a cap jump; arbitrary-level even HP is not
guaranteed when no legal adjustment exists within this bound.

## Evidence and test scope

Native temporary rerun:10 groups/38 parameter cases passed, comprising35 actual
party/native cases and3 explicitly synthetic recoil controls. Actual cases:
Joey leads6, Drum mechanics2, reserve decisions2, carried sleep3; Jose leads6;
Clark leads6, reserves6, timing2 and difficulty calibration2. A diagnostic PASS
means execution, not optimal tactics. Existing shared EC groups and the new
literal-number formula check also passed in the combined57-group run.

The old flat-bonus test and its duplicate expected-formula helper were removed,
replaced by one literal-number native check covering16 low/50/100-level cases,
Nature, effective perfect IVs, Shedinja and fainted HP. Existing preset parity
checks are reused. All temporary battle includes were removed afterward.
Final clean native run:47 retained EC groups passed. Both existing player
Belly Drum parity/recognition groups also passed with the three-point bound.
Both existing editor groups passed after correcting the old flat-bonus display
expectation: level20 Zigzagoon with2 HP points has52HP, not53. The32/66 caps
and displayed remaining budget remain unchanged.
Canonical/master/compiled party projection passes for516 branches; this is
consistency evidence, not516 battle validations. No release ROM or playthrough
claim. Continue Johnson separately after this user-requested detour.
