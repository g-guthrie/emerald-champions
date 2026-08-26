# Verdant rolling battle context

## Next design: Battle 57 — `BATTLE_057_ROUTE_110_DALE`

- Location: Route110
- Category: optional double
- Strict cap: 30 (canonical-sequence-stage)
- Source format: double
- Rolling window: 10 encounter(s) (Battles 47–56)
- Ledger: v2 ledger loaded

## Hard errors

- None.

## Advisory warnings

- **NOVELTY_TAG_REPEAT**: One or more mechanics recur in at least three rolling encounters.
  Evidence: `{"native-pair": 3}`
- **SPEED_CONTROL_DENSITY**: Rolling speed control or priority density is high enough to merit an editorial look, not an automatic change.
  Evidence: `{"density": 0.602, "threshold": 0.55}`
- **PREMIUM_ITEM_REPEAT**: A premium item appears across at least four rolling encounters.
  Evidence: `{"Eviolite": 7, "Life Orb": 4}`
- **SIGNATURE_MOVE_REPEAT**: A non-generic move appears across at least four rolling encounters.
  Evidence: `{"Dazzling Gleam": 4, "Encore": 4, "Energy Ball": 4, "Flamethrower": 4, "Heat Wave": 4, "Shadow Ball": 4, "Thunderbolt": 6, "U Turn": 5, "Will O Wisp": 4}`

## Rolling experience ledger

| # | Encounter | Format | Difficulty | Tempo | Primary question | Novelty tags |
| ---: | --- | --- | ---: | --- | --- | --- |
| 47 | `BATTLE_047_SLATEPORT_MUSEUM_AQUA_GRUNT_2` | double | 9.5 | Conditional Venoshock and Brine conversion into a Focus Sash Illusion reveal and Assault Vest physical cleanup. | Can the player survive a no-heal cleanup wave that rewards, but never requires, carried poison and half-HP states while recognizing one ordered Illusion? | story-double, museum-gauntlet, no-heal-cleanup, venoshock-payoff, brine-payoff, one-slot-possible, illusion-grimer, assault-vest-cleaner, conditional-conversion |
| 48 | `BATTLE_048_SLATEPORT_MUSEUM_ARCHIE` | double | 10 | Freshly healed primordial rain pressure into Eject Button and Swift Swim positioning, then fast Crobat denial and a Contrary Mega snowball. | Can the player dismantle Primal Kyogre's Lightning Rod-protected rain board without spending the Bug, Fairy, special, or boost-control resources needed for Mega Malamar? | boss-double, archie, first-primal, primal-kyogre, lightning-rod, worlds-2016-positioning, wide-guard-feint, swift-swim, mega-malamar, contrary-superpower, full-heal-boss |
| 49 | `BATTLE_049_ROUTE_110_ISABEL_KALEB` | native-pair double | 9 | Immediate Round chaining and Throat Spray pressure into Fairy, sound, healing, Protean, and split-only Grass support without a field engine. | Can the player interrupt a Round duet that immediately calls and doubles the second singer while adapting to joint 3+3 or either independently coherent split four? | native-pair, joint-3-plus-3, split-four, round-chorus, meloetta, throat-spray, heal-pulse, unburden-slurpuff, protean-frogadier, match-call-owner, one-slot-split-safe |
| 50 | `BATTLE_050_TRICK_HOUSE_PUZZLE_1_SALLY` | single | 8.8 | A legal young Protean pivot into critical-hit coverage, a self-activating Facade breaker, and a source-last Beast Boost snowball with no field engine. | Can the player preserve the right physical-control or type answer as four unrelated cutting styles escalate from Protean positioning to critical hits, Toxic Boost, and Beast Boost? | puzzle-single, four-cutting-disciplines, floragato, stage-appropriate-gen9, dawn-stone-gallade, toxic-boost-zangoose, kartana, beast-boost-closer, collision-forced, no-field-engine |
| 51 | `BATTLE_051_TRICK_HOUSE_PUZZLE_1_EDDIE` | double | 9 | One trapped Ground Gem spread turn into mixed Levitate/Flying coverage, resistance berries, and a Tinted Lens special closer without a speed mode. | Can the player break Arena Trap and its safe one-use Ground Gem Earthquake board before mixed Ground-immune reserves punish a grounded lead that cannot simply switch? | puzzle-double, no-exit, arena-trap, ground-gem-earthquake, all-partners-ground-immune, misdreavus, carnivine, tinted-lens-sigilyph, no-speed-mode, one-slot-supported |
| 52 | `BATTLE_052_TRICK_HOUSE_PUZZLE_1_ROBIN` | double | 9.2 | Visible redirection and cap-even mythical pressure into Frisk item punishment and a Magic Guard Life Orb Psychic Terrain cleaner without speed control. | Can the player identify the real target under Follow Me when each obvious knockout feeds Magearna's Soul-Heart, then reassess held items before Duosion converts the remaining Psychic Terrain? | puzzle-double, false-focus, indeedee-f, psychic-surge, follow-me, soul-heart-magearna, frisk-poltergeist-shuppet, magic-guard-duosion, no-trick-room, one-slot-supported |
| 53 | `BATTLE_053_ROUTE_110_TIMMY` | single | 8.7 | A modest Sash rare opener, mixed young disruptor, fast Ground-cushioned coverage, and an exploitable Choice-locked Levitate closer with no shared field engine. | Can the player navigate four unrelated just-caught matchup tests without assuming one Ground answer sweeps the route's Electric-heavy ecology? | route-single, exact-local-catches, gimmighoul-roaming, focus-sash, stunky, aftermath, plusle, shuca-berry, choice-specs-rotom, palate-cleanser, no-team-engine |
| 54 | `BATTLE_054_ROUTE_110_RIVAL` | double | 9.7 | One finite Icy Wind positioning layer into Choice Band Strong Jaw pressure, a dynamic middle starter, young pseudo-legendary risk, and Decorate-backed mixed endgame. | Can the player disrupt the opening Fishious Rend timing, then adapt through a generation-correct counter-starter, Weakness Policy decision, flexible Decorate support, and redirection-ignoring special closer? | required-rival-double, may-brendan-parity, 21-starter-variants, type-null, dracovish, fishious-rend, weakness-policy-metang, decorate-alcremie, stalwart-duraludon, itemfinder-reward, no-transformation |
| 55 | `BATTLE_055_ROUTE_110_EDWIN_JOSEPH` | native-pair double | 9.2 | One visible ally activation into direct physical, mixed, and young support reserves with no weather, field, Tailwind, or Protect cycle. | Can the player interrupt the fragile Beat Up user or immediately control the boosted recipient, then answer the young direct-offense reserves loaded by the actual branch? | native-pair, joint-3-plus-3, split-four, beat-up, justified, terrakion, cottonee, houndoom, growlithe, young-reserves, edwin-match-call, zero-protect |
| 56 | `BATTLE_056_ROUTE_110_EDWARD_ALYSSA` | native-pair double | 9 | Finite equipment denial into delayed Psychic pressure and itemless Acrobatics/pivoting with zero Protect, setup, or speed field. | Can the player deny or outlast five Magic Room turns, switch to item-independent play, and exploit the permanently itemless roster's ordinary type seams? | native-pair, asymmetric-splits, magic-room, itemless-opponents, held-item-denial, future-sight, acrobatics, cycling-pivots, zero-protect, no-speed-field |

## Design questions

- What does this trainer and location naturally suggest?
- Which primary player question is fresh relative to the rolling window?
- What should a fair first loss teach the player to change?
- Which three or more broad counterplay families remain viable?
- What intentional weakness is real and not silently erased?
- Does a reserved boss mechanic or historic team need to remain untouched?
- Which complete competitive references are worth reading before authoring this roster?
- If the sound strategy is overtuned, which level relative to the fixed cap should change first?

Warnings are prompts, not scores, quotas, bans, or automatic rewrite instructions.
