# E0003 Rick — actual-team review and repair

Historical flat-stat record. Current completion and evidence:
[final Gen9 practical pass](e0003_gen9_2026_09_09.md).

One trainer branch, TRAINER_RICK. Native Normal/zero-badge generation creates
four level12 Pokemon with IV31 and actual flat Stat Points; the diagnostic
players are natively generated level14 opening catches/starters. Authored level
offsets, party size, progression and wild tables were preserved.

## Authored change

Retained Spinarak's Sash/Insomnia control lead, Dwebble's Eviolite/Sturdy and
24 Speed points, and Life Orb/No Guard Karrablast. Normal Speed is51/45/55/15.
Spinarak can therefore move before Dwebble and create a useful order crossing.
No Guard improves opposing attacks into Karrablast too, not only Megahorn.

Dewpider changed from Adamant32HP/32Atk/2Def with Liquidation/Lunge/Leech Life/
Protect to Modest32HP/2Def/32SpA with Scald/Bug Buzz/Giga Drain/Protect. Its
native base Attack and SpA are both40; Water Bubble boosts either Water category.
Normal current stats are66HP,16Atk,23Def,55SpA,26SpD,15Speed. All four moves
exist in its current native preparation array. Water Bubble and Sitrus remain.
This adds a special axis to an otherwise all-physical team and distinguishes it
from E0011 James's physical Dewpider. Losing guaranteed Lunge Attack reduction
is a real trade; Scald burn is probabilistic, not a replacement guarantee.

Only E0003 was synchronized into the master via the existing canonical compiler;
the existing implementation generator materialized that master. Current master
prefix verification passes. The earlier paused Vivian/Ben/Wattson canonical
differences remain unmaterialized; this is not a whole-canonical freshness claim.

## Executable AI repair

The paired scorer previously skipped Toxic Thread as a nondamaging status move,
so it never connected its native two-stage Speed loss with Dwebble's attack.
It now caches Thread's native stat permission/delta alongside Web/Wind and uses
the existing single effect/no-effect four-action pass. Its selected/redirected
target receives the changed Speed before the next action is ordered. There is
no hardcoded Thread move/target preference, extra damage calculation or new
multi-turn search, and no read of the player's pending commands.

Primary Thread is not incorrectly blocked by Shield Dust or Covert Cloak.
The cache respects Substitute, status reflection/protection, native stat-loss
rules, Contrary/caps and White Herb. Poison eligibility is not Speed eligibility:
Electric absorption does not block Thread, and poison immunity can leave the
Speed effect intact. Native raw poison valuation is retained, not a new poison
simulation. Newly poisonable Guts/Quick Feet/Toxic Boost targets conservatively
receive no additional modeled Thread Speed payoff, avoiding a fictitious free
slowdown while those immediate benefits remain unmodeled.

## Observed choices and counterfactuals

- Useful crossing: native Charmander47/Taillow49 are between Spinarak51 and
  Dwebble45. AI chooses Electroweb/Rock Slide; both foes faint before their
  attacks. With slower bulk-invested versions, it chooses Poison Jab/Rock Slide.
- Electric absorbers: full-health Volt Absorb Pachirisu48 and Lightning Rod
  Electrike49 make Web ineffective. AI retains Poison Jab/X-Scissor toward
  Electrike, ending Electrike23/51 and Dwebble17/47 in the sampled native turn.
- Injured Electrike20/51 exposes the old failure: AI slowed Pachirisu and used
  Knock Off into Electrike, leaving it10HP and Dwebble17HP. After repair, the
  unrestricted AI slows Electrike instead, then Knock Off removes it before
  Flamethrower. Dwebble stays47/47, Spinarak67/67 and Pachirisu77/77.
- Separate forced comparisons establish the timing mechanism: injured-target
  Thread/X-Scissor removes Electrike while keeping Dwebble47HP, whereas
  Jab/X-Scissor removes it only after Dwebble falls to17HP and burns. Forced
  commands are counterfactual evidence, not the AI's chosen command pair.
- Slowing is not always good: with full-health Electrike, Thread lets its
  partner's redirected Thunderbolt activate Lightning Rod before Flamethrower.
  Thread/X-Scissor leaves Dwebble3HP versus17 for Jab/X-Scissor. A sampled
  Thread/Rock Slide flinch kept Dwebble47HP but is not a guaranteed outcome.
  The unrestricted AI did not adopt Thread in this full-health control.
- Reserves: Karrablast Protect plus special Dewpider Scald removes Growlithe
  both with Flash Fire and with Intimidate, preserving Karrablast39HP and
  Dewpider57HP. The prior physical version left Intimidate Growlithe19HP.
- Against physical-bulk Pachirisu/Lotad, Karrablast Megahorn plus Dewpider
  Bug Buzz removes Lotad and preserves both reserves28/36HP. The earlier
  uncontrolled Thunderbolt sample fully paralyzed Dewpider before its action;
  the final execution probe explicitly suppresses Thunderbolt's secondary to
  examine the chosen attack. It does not prove paralysis cannot stop this team.

Four forced native controls confirm a real Substitute blocks Thread, White Herb
restores lost Speed but not poison, Contrary Snivy gains two Speed stages, and
Magic Bounce Natu reflects the Speed drop onto Poison-immune Spinarak. Three
unrestricted controls then show Thread aimed at the other target with White
Herb present, Poison Jab/Protect against Contrary, and Jab/Knock Off into Natu
rather than a reflected Thread. These are sampled decisions, not optimality
proofs across every position.

## Verification and limits

Final selected run: five native groups,20 parameters, all passed after the
production and team changes. A PASS means scenario execution; the decisions and
tradeoffs above are the tactical evidence. All16 retained expert groups then
passed, including board/cache/RNG restoration; the warmed full-party decision
still measured21 GBA frames, not proof of every battle's latency. Temporary encounter includes were
removed afterward. The four work/ fragments remain diagnostic evidence, not a
new permanent trainer-lock suite. Generic decision-time tracing is temporary
for the continuing pass and must be removed at final handoff.

The bounded forecast is not a complete battle simulator: poison residuals,
general drain healing, item-removal changes to later Eviolite damage and all
ability-redirection timing are not fully modeled. In particular, the scorer's
local redirection does not generally reconstruct Pachirisu's Thunderbolt into
Lightning Rod Electrike; the full-health native counterfactual exposes that
limit even though the observed actual choice is sensible. Reflection's result
is rejected for added Speed payoff rather than simulated. No full playthrough,
all-matchup certification, release ROM or measured win rate is claimed.

Intention rating:9/10. Distinct control, survivability, accuracy and special
Water reserve roles now execute more coherently. Below10: control recovery
after Spinarak falls could be richer. Icy Wind over Giga Drain is one possible
future trade, restoring reserve Speed control but losing healing and coverage;
it is not a strict upgrade or an unimplemented required fix.
