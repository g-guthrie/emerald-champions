# Universal HP-triggered berry convenience

September 9 user override: natural HP-triggered items activate at
`floor(maxHP / fraction) + 1`, capped at `maxHP - 1`. At 100 maximum HP,
half-HP items activate at 51 and quarter-HP items at 26. At 97 maximum HP,
the thresholds are 49 and 25. Fainted Pokémon do not consume naturally.

This supersedes the former Sitrus-only ceil-half rule and the entire automatic
Belly Drum EV-parity adjustment. The intended convenience is one HP above the
ordinary cutoff, not an extra point above an already rounded-up cutoff.

## One runtime owner

`src/battle_util.c:GetBerryActivationThreshold` is pure and shared by native
consumption and AI forecasts. `HasEnoughHpToEatBerry` retains native forced-use
overrides and records Gluttony only when its larger cutoff is needed.

Covered items: Oran, Berry Juice, Sitrus, Figy, Wiki, Mago, Aguav, Iapapa,
Liechi, Ganlon, Salac, Petaya, Apicot, Lansat, Starf, Micle and Custap.
Gluttony gives eligible berries the half-HP cutoff plus one. Unnerve and both
As One abilities still block natural berries; Berry Juice remains an item,
unaffected by Gluttony, Unnerve and Ripen. Healing amounts, flavor confusion,
Heal Block, stat ceilings and other item eligibility conditions are unchanged.
Stuff Cheeks and other forced consumption retain their separate override.
Status-, PP- and hit-triggered berries are not HP thresholds and are unchanged.

The paired healing forecast, Population Bomb's existing Sitrus forecast,
switch-entry healing and stat-pinch forecasts, and Endure's pinch-berry score
use the same cutoff. Pair healing now also accounts for Oran/Berry Juice's
fixed recovery; it does not treat Juice as a berry for Unnerve or Ripen.
This does not add general joint simulation of every stat/Custap/Micle payoff.

## Remove the workaround, keep exact EVs

Deleted `TryNormalizeEmeraldChampionsBellyDrumHpParity`, its item/move predicates,
the shared ±40 EV tolerance, its preset-application and Leveler calls, and the
matching tolerance. Preset recognition again requires exact EVs. No Circuit-
specific parity patch, per-species exception or extra runtime search was added.
Canonical sets remain unchanged by this rule; players' custom EVs are not
automatically altered during leveling. Already customized spreads are not
silently reset; reapplying a preset restores its exact authored allocation.

Level-98 Circuit Linoone can keep 353 HP: Drum leaves 177 and Gluttony's Figy
cutoff is now 177. Level-14 Snorlax can keep 81 HP instead of losing 28 HP EVs.
These two examples are native-formula arithmetic, separate from battle evidence.
The superseded Circuit diagnostic showed unadjusted level-14 Linoone at59HP;
its even-HP assertion is obsolete, not a remaining game defect.

The one-point allowance does not promise every Substitute sequence activates
at the same turn across every level. For example, three Substitutes from103HP
leave28HP, still above the quarter-HP cutoff26. Such sequence timing belongs
to individual set/encounter design, not another hidden EV correction.

## Verification

- Replaced the existing Sitrus-only boundary group with native coverage for all
  17 HP-triggered items: even/odd maximum HP, the new cutoff and one above it,
  Gluttony, and full-HP1 non-consumption. All153 parameter cases pass.
- Expanded the existing Drum group to Sitrus, Figy/Gluttony, Berry Juice and
  Oran at96/97 maximum HP. All8 parameter cases pass.
- All46 retained shared `EC ` AI groups pass. The recoil/Gluttony fixture now
  starts at20/36, not19/36:19 is already inside the new natural cutoff. Its
  expected post-recoil recovery moves from28 to29; the intended move and
  Unnerve control remain unchanged. No expected move was relaxed.
- Removed the two obsolete parity-normalization test groups. The existing
  catalogue application test now requires exact authored EVs for every stat,
  rather than accepting a bounded HP deviation.

- Two temporary exception groups pass (9 cases): berry-only Ripen/Unnerve
  behavior, fixed20 Berry Juice recovery under Unnerve, and full-HP Stuff
  Cheeks consumption through Unnerve. The temporary include was removed;
  the scratch file is retained only as reproduction evidence.
- The existing complete named Doubles/Singles catalogue application group passes
  with exact EV assertions after removing the normalizer. The 6,923-preset
  canonical source is unchanged; its scratch HP inventory has been regenerated
  using the new rule. It is arithmetic evidence, not a battle-play certificate.
- The retained full-HP Shedinja group passes after removing the temporary
  diagnostic include. Both canonical preset/Circuit output comparisons pass.
  The warmed paired-decision probe remains30 GBA frames; this is not a complete
  cold-decision latency certificate.

Logs: `work/berry-threshold-boundaries.log`, `work/berry-drum-convenience.log`,
`work/berry-policy-controls.log`, `work/ai-shared-berry-threshold.log`, and
`work/preset-exact-ev-application.log`.
No trainer encounter receives completion credit from these checks.
