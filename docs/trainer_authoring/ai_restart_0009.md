# E0009 Lyle — executable-AI review complete, 8.5/10 intention

September 9, 2026. Bounded native encounter review, not exhaustive matchups.
Actual compiled TRAINER_LYLE, Normal, zero badges. All four members become
level 12 through native generation/difficulty processing. The original team,
compiled loadouts, levels and progression are retained. Lyle alone now has an
explicit PRESSURE plan, selectively materialized through E0009.
The earlier source-only review is not encounter-AI completion.

## Actual team and role review

Native HP / Atk / Def / SpA / SpD / Speed, before ability/item modifiers:

| Member | Stats | Current role |
| --- | --- | --- |
| Nymble | 35 / 51 / 18 / 11 / 14 / 56 | Sash, Tinted Lens, Jolly. First Impression / Leech Life / Sucker Punch / Protect. |
| Sewaddle | 68 / 58 / 27 / 16 / 23 / 18 | Eviolite, Overcoat, Adamant. Bug Bite / Seed Bomb / Struggle Bug / Protect. |
| Anorith | 38 / 63 / 20 / 16 / 20 / 63 | Life Orb, Battle Armor, Jolly. Rock Blast / X-Scissor / Aqua Jet / Protect. |
| Dottler | 69 / 15 / 29 / 57 / 30 / 15 | Leftovers, Swarm, Modest. Psychic / Struggle Bug / Reflect / Helping Hand. |

All sixteen moves and four abilities were checked in native preparation/species
data. Nymble's U-turn candidates are also natively available. Champions First
Impression has 100 power and +2 priority; Fake Out has +3. Real re-entry resets
eligibility, not merely waiting a turn. All four species make their first
trainer appearance here. The Bug theme repeats Rick, but not his species or
exact roles. Preserve campaign-wide variety while finalizing this encounter.

## Implemented executable repairs

1. Struggle Bug now shares the existing Snarl Special Attack continuation.
   Eligibility is cached per actor, move slot, and target: Soundproof only
   blocks sound moves, and native Substitute handling distinguishes the two.
   Native secondary-effect protection, Sheer Force, Contrary/Simple,
   Competitive/Defiant and White Herb order remain intact. Independent source
   review found no blocking issue. Forced native slow Munchlax Ice Beam does
   15 without a drop and 10 after Sewaddle or Dottler's Struggle Bug. With the
   same HP/SpA but native faster Speed, Ice Beam does 15 before the drop in all
   three cases. Munchlax is a pre-Roxanne mechanic reference, not a pre-Lyle
   catch claim; derived Speed and damage were not invented.

2. Screen desirability now requires damage that native screens can reduce.
   Super Fang's physical category previously justified Reflect against an
   otherwise special Pachirisu/Lotad board, although Reflect cannot reduce it.
   The shared predicate excludes fixed/HP/level/reflected/OHKO damage,
   guaranteed criticals, hostile Infiltrator, and pre-damage screen breakers.
   Dottler now actually selects Struggle Bug instead of that useless Reflect.

3. The paired continuation models Reflect at its real action order and
   successful screen removal before a breaker's own damage. Existing screen
   factors are not stacked twice, including Aurora Veil. The native damage
   cache itself now temporarily clears screens for Brick Break/Psychic Fangs/
   Raging Bull and restores state afterward; it previously omitted their
   pre-attack script. Cached original/local screen-factor ratios also cover
   Population Bomb's individual strikes before survival/items. Earlier
   possible Snatch conservatively withholds setup credit.

Certain screen events apply in both existing forecast passes. Uncertain events
share the existing minimum-chance effect pass, not an independent probability
tree. Rescaling rounded native damage anchors approximates subsequent native
rounding. No additional native damage calculations run inside pair enumeration.
The existing limitation on newly paralyzed arbitrary status actions remains.

## Observed native results

- Special reserve board, unchanged compiled team: old turn 1 Protect/Reflect
  ended Anorith/Dottler 38/67 and foes Pachirisu/Lotad 77/71. Repaired turn 1
  Protect/Struggle Bug ends 38/69 and 68/52, both foes at -1 SpA. Turn 2 now
  X-Scissor/Psychic ends 33/69 with Lotad knocked out; old X-Scissor/Struggle
  Bug left 31/68 and Lotad 21. Lotad and Dottler tie at 15 Speed here; this is
  not evidence that Dottler always moves first.
- Physical reserve board still uses Rock Blast/Helping Hand, then Reflect
  after Anorith falls. Dottler ends 34 then 12 HP. Eevee also falls in the
  first turn; do not mislabel its recoil sequence as a Life Orb self-KO.
- Forced native Reflect reduces slower Body Slam 39 to 26. Critical Body
  Slam remains 58 either way. Native Brick Break deals 6 with or without prior
  Reflect and removes the screen before its own hit.
- Five initialized native AI-cache controls verify range/state restoration:
  Brick Break 6/7/8, Super Fang 34/34/34, guaranteed-critical Storm Throw
  8/9/10, and Infiltrator Aerial Ace 32/36/38 are unchanged by screens.
  Ordinary Inner Focus Aerial Ace changes 32/36/38 to 21/24/25. Both side
  flags and both RNG states restore. Throh/Zubat are mechanic references,
  not pre-Lyle availability claims. These are damage-consumer checks, not
  unrestricted AI choices.
- A separate forced four-turn U-turn/exit/re-entry sequence successfully
  executes First Impression twice, each dealing 13 to the reference Pachirisu.
  This proves native eligibility, not that the AI plans this cycle.

## Opening diagnosis and authored pressure repair

The unchanged leads choose double Protect on turn 1 against all three sampled
physical, special, and Fake Out boards. They repeat double Protect on physical
turn 3 after attacking on turn 2. First Impression legality is working; the
problem is the limited turn horizon and its value for preserved HP.

Temporary native pair traces on the Eevee/Timburr board show:

- First Impression/Seed Bomb forecasts Nymble dying to both focused attacks;
  score 258. Forcing First Impression would ignore genuine Sash-breaking focus.
- Protect/Seed Bomb scores 394 against both attacks into Nymble, but 320
  against split targeting; both allies survive that latter forecast, Sewaddle
  at 18 HP. The second opponent forecast is functioning, not reading commands.
- Double Protect scores 368 in both forecasts, preserves all HP, and makes no
  progress. Raw move scores are First Impression 104, Seed Bomb 100 and
  Protect 101, so this is not a huge raw Protect bonus.

Six separate forced physical counterfactuals confirm the actual tradeoffs:
double Protect leaves own 35/68, foes 75/81; First Impression/Protect leaves
1/68 and 42/81; Leech Life/Protect 1/68 and 46/81; Protect/Seed Bomb 35/50 and
54/81; First Impression/Seed Bomb 1/50 and 21/81; Leech Life/Seed Bomb 1/50 and
25/81. These use the human's specified split attacks, not every possible focus.

Two distinct team candidates were exercised, neither adopted:

- Sucker Punch -> U-turn: six unrestricted comparisons show no action changes
  through the sampled physical/special continuations. Adding the move does
  not establish working pivot planning.
- Protect -> U-turn, retaining Sucker Punch: eight comparisons produce real
  offensive First Impression openings. Physical turn 2 trades Nymble for
  Eevee, leaving Sewaddle 32, Dottler 69, Anorith 38 and Timburr 72; turn 3
  leaves Timburr 13 and Sewaddle 14. Against special pressure both leads fall
  by turn 2, with Charmander 27 instead of the old 51 and old Nymble surviving
  at 10. Against Fake Out the AI immediately switches Nymble to Dottler,
  preserves Nymble 35 and removes Mienfoo on turn 2. This is a mixed offensive
  role trade, not proof that deleting Protect fixes the planning horizon.

The adopted PRESSURE plan accepts nonlethal chip without making sacrifices
cheap. Ordinary living-member value is 80 + 100 x HP/maxHP; Lyle uses
155 + 25 x HP/maxHP. Both give a full-health member 180, while an injured
Lyle member costs MORE to lose. The same helper values active and reserve
members by their real trainer owner. It applies only to the evaluating side;
enemy and ordinary player-side evaluations retain the default valuation.
Move legality, damage probabilities, switching and the two opponent forecasts
are unchanged. This is explicit authored risk preference, not a new search
horizon or a numerical native-mechanics rule. Direct healing's non-KO HP value
is reduced; Lyle has no direct recovery move. No blanket First Impression bonus,
Protect ban, or pending-human-command read was added.

With the actual unchanged team, native opening decisions now differ by board:

- Physical pressure: Protect/Seed Bomb, ending 35/50 against 54/81. Nymble
  respects Sash-breaking focus while Sewaddle progresses. Turn 2 Leech Life/
  Protect ends 1/50 against 25/81; alternating protection supports attacks.
- Special pressure: First Impression/Protect, ending 10/68 against 47/77.
  Turn 2 protects Nymble while Sewaddle's repeated Protect fails and it falls;
  Anorith enters. No guaranteed-repeat Protect or immunity to Fire pressure
  is claimed.
- Fake Out: Protect/Seed Bomb denies the +3 interruption while damaging
  Mienfoo to 36. Turn 2 Nymble switches to Dottler and Seed Bomb leaves
  Mienfoo 17, versus 36 under the former double-wait opening.
- Lower-threat native Lotad pair: First Impression/Struggle Bug on turn 1,
  then Leech Life/Bug Bite on turn 2. One Lotad falls; Nymble recovers to 35
  and Sewaddle remains 62. Both native offensive entry and special mitigation
  operate without forced AI commands.

Five injured/limited-reserve comparisons distinguish aggression from throwing
away teammates. Nymble/Sewaddle at 7/18 with or without healthy reserves, and
1/35 without reserves, retain double Protect rather than two immediate losses.
With fainted leads and Dottler at 20, Reflect still cannot save it from focused
Round/Body Slam; this death occurs both before and after PRESSURE. Anorith at
12 keeps Protect; healthy Anorith instead uses X-Scissor, ending 35 and damaging
Munchlax from 98 to 66 without adding a faint. These controls do not establish
optimal long-term choices in doomed or deeply injured positions.

## Current verification checkpoint

57 parameters / 14 Lyle native groups pass after removing temporary source
score logging. This includes unrestricted team comparisons, forced mechanical
counterfactuals and initialized cache probes as distinguished above. A PASS
does not certify tactical quality. The shared 16 coordination/restoration
regressions also pass. Six-player-party Lyle timing is 39 GBA frames warm,
46 including cache rebuild (about 0.77 seconds), not a campaign worst-case bound.
An additional 11-parameter Cindy reserve/Snarl cross-check passes: unrestricted
Furfrou still uses Snarl against the slower special pair, while native damage
controls remain 26 baseline, 17 after Snarl, 26 after Herb, and 39 after
Competitive with Herb retained. Choice lock and Truant remain active.
The initialized policy-boundary query preserves both RNG states and gives
identical player-side scores (69/69). On the opponent side the pressured
score is 158 versus 19. An explicitly all-idle comparison isolates valuation:
its difference of 151 exactly accounts for both injured active members and
both injured reserves, without inventing a switching gain. The trainer-ID
substitution in this diagnostic changes policy context only, not Rick's team.

The U-turn fixture initially failed because it declared result fields without
a PARAMETRIZE; the harness misleadingly relabeled that setup error as an
ability-popup FLAKY. The fixture was corrected, not gameplay. Diagnostic source
line IDs must remain below 65536; the opening control uses 58000, not 66000.
Temporary files stay in work/ and are removed from the normal include list
after this checkpoint; they are not a new permanent per-trainer suite.
Only E0009 plan/prose was newly materialized; the master/party consistency
check passes all 516 branches, which is not 516 battles of AI evidence. The
compiled plan table matches all 14 reviewed-prefix branches. Later paused
Vivian, Ben and Wattson plan/tactic differences remain deliberately uncompiled;
the all-authored plan-table comparison is therefore not claimed to pass.
No new release ROM, commit, or push. Restarted ledger advances to
14/516 trainer branches and 9/468 encounters. E0010 is next.

## Rating and remaining limits

8.5/10 intention: four distinct insects now combine selective first-entry
pressure, physical attacks, real special suppression, fast Rock coverage,
priority finishing and a genuine special-support reserve. The pressure plan
corrects the sampled healthy opening waits while preserving protection in
dangerous injured positions. This is not a measured win rate or a full search.

To improve further, benchmark a Speed/bulk trade on one slow support or a
genuinely evaluated pivot/re-entry cycle; do not add U-turn merely because a
forced sequence works. Sewaddle/Dottler's 18/15 Speed often makes mitigation
help a later turn, not the current fast attacker. Fire/Flying pressure, focus,
Fake Out, Dark immunity to Psychic, Taunt and status counterplay remain real.
The paired planner still has a short horizon, two selected opposing target
patterns, rounded damage anchors and correlated effect approximations. Those
limits can still produce inferior choices outside these tested positions.
