# E0014 — Gina & Mia: bounded executable-AI repair complete

Actual source party and native generation, not the historical design book,
are the authority. This is bounded native-scenario verification, not every
possible matchup or a full-game completion claim. Later batches remain paused.
Intention rating: **8/10**. Main-chat completion reports this encounter separately.

## Current actual party

Normal zero-badge cap produces all four at level12:

| Pokémon | HP/Atk/Def/SpA/SpD/Spe | Item, ability | Moves |
| --- | --- | --- | --- |
| Plusle | 42/18/18/65/26/71 | Focus Sash, Plus | Thunderbolt, Nuzzle, Helping Hand, Protect |
| Minun | 42/16/20/58/33/71 | Sitrus, Minus | Thunderbolt, Charm, Encore, Protect |
| Volbeat | 73/23/28/57/29/29 | Lum, Prankster | Tail Glow, Roost, Bug Buzz, Protect |
| Illumise | 43/18/26/63/29/61 | Leftovers, Prankster | Dazzling Gleam, Bug Buzz, Encore, Helping Hand |

All16 prepared moves and selected abilities are source-legal. The species debut
here; compiled campaign uses are Plusle3, Minun3, Volbeat4, Illumise3. Actual
Plusle/Minun base Speed is105, not vanilla95. Equal lead Speed means no strict
opponent-speed interval between them; Nuzzle can instead enable slower Volbeat.
Early native counters include Volt Absorb Pachirisu, Lightning Rod Electrike,
Shellos/Nidoran♀ Earth Power, Fire coverage, and Mienfoo Fake Out/Taunt.
Shellos is pure Water here, not Electric-immune.

## Demonstrated native mechanics and current repairs

- Actual Minun Charm reduces a subsequent Timburr Drain Punch from40 to21;
  Sitrus leaves Minun at31 rather than12. Pair offensive-drop handling now
  includes this primary −2Attack move, retaining protection, reflection,
  native stat-loss eligibility and retaliation/White Herb handling. Unlike a
  secondary drop, Charm is not blocked by Covert Cloak or Shield Dust.
- Native Volbeat Roost heals36 (half73 floored), before the ordinary attack
  because of Prankster. Injured25HP control finishes48 after13 damage; attacking
  finishes12. Unrestricted native turn2 changes from Protect at12HP to Roost,
  finishing35HP. Only non-Flying Roost is newly modeled: type-changing Flying
  Roost still needs a separate incoming-type forecast. Snatch/Heal Block gates
  and existing recovery action-chance weighting remain.
- Native Plusle Thunderbolt deals34 with Minun surviving earlier Quick Attack,
  versus24 with Minun defeated first. The ordinary native cache ranges are
 34/39/42 versus24/25/28. Pair now caches native raw boosted/unboosted ranges
  once per relevant slot/target and uses action-time partner survival. It does
  not multiply rounded final damage by2/3. HP-powered/multihit and Max/Z cases
  are excluded; partial-survival mixture/correlation remains approximate.
- Actual mixed Plusle/Volbeat Nuzzle changes native Timburr Speed51→25, below
  Volbeat29. Covert Cloak blocks paralysis but not damage; explicit native full
  paralysis also prevents the reply. Pair now caches eligibility per move slot,
  separating primary Thunder Wave from damaging Nuzzle, and applies Nuzzle
  status after damage but before contact can faint its user. Cures, beneficial
  status interactions and unresolved live Lightning Rod remain conservative
  exclusions, not claims of complete secondary-effect simulation.

## Observations so far and limits

The initial repaired SETUP-only leads double-Protect turn1 on neutral special-bulk and Lightning Rod
boards; the latter switches Minun to Volbeat turn2. The disruption board instead
focuses Mienfoo and defeats it by turn2. Charm is selected/executed turn2 on the
physical pressure board after its repair. Initial protective waits remain an
improvement target; a one-turn forecast is not a multi-turn competitive solver.

Temporary mechanic controls prescribe trainer actions only to measure actual
effects. Separate encounter observations leave trainer moves unrestricted.
Generic synthetic regression candidates are separate from actual-team evidence.
Only distinct positively and negatively demonstrated regressions may be retained.
## Native followthrough and evidence

- Healthy reserves versus physical-bulk Pachirisu/Timburr: Tail Glow plus
  Dazzling Gleam turn1; Bug Buzz plus Gleam turn2 defeats Timburr. Volbeat ends
  at47/73HP and Illumise33/43. Against Pachirisu/specially bulky Lotad, Tail Glow
  plus Bug Buzz becomes double Bug Buzz on turn2 and defeats Pachirisu; own
  HP47/37. These are actual sampled knockouts, not a measured win rate.
- SETUP-only mixed Plusle/Volbeat: after the initial guards, Nuzzle plus Tail Glow turn2;
  second Tail Glow reaches+6 turn3; Plusle switches to actual Illumise turn4
  while Volbeat attacks, then Gleam/Bug Buzz turn5. The durable paralyzed foes
  remain alive in this sample. Do not present every setup as an immediate KO.
- The SETUP-only actual Charm scenario executes Thunderbolt/Charm turn2. Its native
  Thunderbolt can also paralyze: one unrestricted sample's reply is denied by
  full paralysis. The isolated retained Charm case controls only that native
  paralysis RNG to ensure the subsequent physical hit is measured; it still
  requires the AI to choose Charm without prescribing its command.
- Native Easy/Hard generation yields levels10/14, respectively, with the same
  roster and abilities (Normal12). An initial temporary fixture incorrectly
  indexed empty difficulty tables. It was corrected to the native
  `GetTrainerStructFromId` fallback; no game data changed to satisfy the fixture.
- SETUP-only actual four-/six-player-party decisions:52 warm frames,58 including native
  cache rebuilding, approximately0.87/0.97 seconds at60Hz. With all four repairs
  disabled for the negative run:45/51 frames. The added correctness costs about
  seven frames on these boards; this is noticeable latency, not a global
  worst-case guarantee. The retained unrelated full-party/Mega timing case
  remains30 frames. Both actual decisions succeed without allocation failure.
- Plus/Minus range arrays add784bytes per evaluator; per-slot paralysis masks
  add12bytes relative to the old layout. No new candidate-search dimension or
  recursive lookahead was added. Raw native damage anchors are calculated once
  per eligible board/move/target, never within candidate pairs.

The pre-pacing temporary run passed41 parameters in15 groups. The final
accepted actual-team run covers **36 parameters in11 groups**, including the
extended neutral lead scenario. Separately, the four new generic regression
groups cover7 parameters. Native controls, synthetic policy boundaries and
unrestricted encounter observations are deliberately distinguished above.
The final retained suite, rebuilt with temporary includes removed, passes all
27 shared groups after the accepted pacing change.
The existing state-restoration test now also exercises Plus/Minus anchors; it
checks board, caches, field, RNG and stack sentinels without a new redundant test.
Canonical/master projection:516 branches PASS; whitespace check PASS. Projection
coverage is not encounter-completion coverage. No release ROM or publication
was made at this checkpoint.

Four small generic regressions were retained, each with a demonstrated disabled-
repair failure: Roost became Protect; Charm became a different-target support
choice; Nuzzle's winning speed-control pair became Strength/Protect; Plusle
chose a falsely boosted Thunderbolt instead of preserving its threatened Minus
partner. All four negative mutations were restored. The Nuzzle case also checks
Covert Cloak and separate Thunder Wave eligibility; Plus/Minus checks the healthy
partner attacking alternative. Temporary per-encounter probes remain under
`work/` but are no longer built. They are not a permanent campaign test suite.

## Accepted encounter-specific pacing

Added **PRESSURE alongside SETUP** in the canonical team record, master branch
and executable plan table. This is a bounded scoring preference for worthwhile
chip trades, not an opening move script or access to the player's selected
commands. It does not make knockouts cheaper to the AI.

The same native inputs show:

- Physical pressure: Thunderbolt/Charm starts on turn1 instead of double guard;
  turn2 uses Thunderbolt/Encore. The sampled foe HP progresses81→56→31. The
  sample includes native paralysis; it is not a guaranteed no-damage line.
- Mixed Plusle/Volbeat: Protect/Tail Glow starts turn1; attacks start turn2,
  Illumise replaces Plusle turn3, and Pachirisu is defeated turn4. Turn5 leaves
  Timburr3HP, with own Illumise43/Volbeat64. The SETUP-only run leaves both foes
  alive at38/51 after turn5 and Volbeat73HP. The improvement trades9HP for much
  more progress in that sample rather than treating HP preservation as a win.
- Healthy reserve Tail Glow/attack outcomes and injured Roost followthrough
  remain intact. The native Mienfoo focus and Lightning Rod counterplay remain.
- Neutral special-bulk remains an opening double guard. By turn4, PRESSURE
  has all four teammates alive (active Plusle20/Minun10, reserve Illumise15 and
  Volbeat73) versus SETUP-only losing Minun. It has dealt less foe damage:
  foe48/81 rather than32/81. This is an explicit survival/progress tradeoff,
  not a claim that aggression improves every metric on every board.
- Final measured four-/six-player decisions:53 warm frames,59 rebuilt, roughly
 0.88/0.98 seconds at60Hz; one frame above the SETUP-only sample.

## Retained roster and improvement path

No species, move, item, nature, Stat Point or level changes were needed.
Both max-Speed electric attackers can exploit their native special
boost and timely disruption; Sash/Sitrus provide distinct immediate protection.
Bulky special Volbeat's Lum/priority recovery supports its own Tail Glow offense.
Fast Illumise supplies Fairy spread pressure or targeted support, and native
Leftovers recovery is observable. Keeping these infrequently used species
preserves campaign variety instead of replacing them with generic staples.

**8/10 for intention:** a clear electrical partnership followed by independent
setup/support reserves, now backed by the repaired executable effects and
native-tested encounter-specific pressure preference. To reach
10, improve the initial double-Protect choices and conversion of very defensive
matchups into progress, shorten decision latency, and test more natural reserve
transitions and player responses. Flying Roost, broader Encore locks, complete
secondary-effect interactions and exact correlated multi-turn forecasts are
not solved by this pass. A higher rating would require that additional evidence.
