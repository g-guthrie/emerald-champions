# Verdant rolling battle context

## Next design: Battle 71 — `BATTLE_071_ROUTE_117_DYLAN`

- Location: Route117
- Category: optional moving solo double
- Strict cap: 40 (canonical-sequence-stage)
- Source format: double
- Rolling window: 10 encounter(s) (Battles 61–70)
- Ledger: v2 ledger loaded

## Hard errors

- None.

## Advisory warnings

- **NOVELTY_TAG_REPEAT**: One or more mechanics recur in at least three rolling encounters.
  Evidence: `{"guarded-double": 6}`
- **SPEED_CONTROL_DENSITY**: Rolling speed control or priority density is high enough to merit an editorial look, not an automatic change.
  Evidence: `{"density": 0.728, "threshold": 0.55}`
- **PREMIUM_ITEM_REPEAT**: A premium item appears across at least four rolling encounters.
  Evidence: `{"Assault Vest": 4, "Eviolite": 4, "Expert Belt": 4, "Focus Sash": 4}`
- **SIGNATURE_MOVE_REPEAT**: A non-generic move appears across at least four rolling encounters.
  Evidence: `{"Body Press": 4, "Dazzling Gleam": 4, "Giga Drain": 4, "Ice Beam": 4, "Ice Punch": 4, "Psychic": 4, "Rock Slide": 4, "Roost": 4, "Thunderbolt": 5}`

## Rolling experience ledger

| # | Encounter | Format | Difficulty | Tempo | Primary question | Novelty tags |
| ---: | --- | --- | ---: | --- | --- | --- |
| 61 | `BATTLE_061_ROUTE_110_CYCLING_BENJAMIN` | single | 9.1 | Young room-and-rock lead, special Throat Spray pressure, Choice Band slow breaker, and rare Beast Boost clock reset. | Can the player deny, reverse, or count out two finite Trick Room clocks while exploiting the young lead, Choice lock, shared Fighting pressure, and one-hit Balloon seam? | cycling-single, slow-lane, trick-room, two-clock, young-bronzor, throat-spray-drampa, choice-band-crabominable, stakataka, air-balloon, beast-boost, match-call-owner |
| 62 | `BATTLE_062_ROUTE_110_CYCLING_JASMINE` | double | 9.1 | Branch-sensitive Grass-Fire lead into player-selected Rainbow, Swamp, Sea of Fire continuation, or no-combo Water endgame. | Can the player choose the knockout order that creates the least dangerous Pledge course, then exploit the earned Water-plus-Water endgame before Liquid Voice recovers momentum? | cycling-double, guarded-double, pledge-relay, player-chosen-branch, sea-of-fire, rainbow, swamp, middle-starters, libero-raboot, liquid-voice-brionne, no-forced-ai |
| 63 | `BATTLE_063_ROUTE_110_CYCLING_JACOB` | double | 9 | Two-body contact toll, frail Rough Skin reserve, then rare special Flame Body spread finish with no speed or field mode. | Can the player recognize that only contact is taxed, change attack class or move geometry, and exploit ordinary type seams before chip and Moltres spread pressure accumulate? | cycling-double, guarded-double, contact-tax, iron-barbs, rocky-helmet, rough-skin, young-ferroseed, sharpedo, moltres, flame-body, noncontact-counterplay, no-speed-mode |
| 64 | `BATTLE_064_MAUVILLE_WALLY` | double | 9.7 | Fast item-removal and Tailwind lead, target-order-sensitive young Ground reserve, returning mixed partners, then source-last signature Mega balance closer. | Can the player break a six-way balance team with no single engine, denying Tailwind and preserving the right checks through mixed lanes until Wally's Wide Guard Mega Gallade? | required-rival-double, declinable-retry, explicit-two-mon-guard, published-balance-chassis, weavile-zapdos-lead, target-order-ground-geometry, young-drilbur, wally-continuity, zapdos, mega-gallade, wide-guard, no-single-engine |
| 65 | `BATTLE_065_MAUVILLE_GYM_VIVIAN_KIRK` | native-pair double | 9.3 | Ally-safe Punk Rock spread in the 3+3 joint, Vivian's disruptive/bulky coverage single, or Kirk's direct Ghost-backed special single. | Can the player stop the lone Boomburst amplifier or bring a sound answer, then recognize that the split branches no longer have a partner engine? | gym-native-pair, joint-3-plus-3, split-four-singles, boomburst, soundproof-prefix, ghost-immunity, punk-rock-toxtricity, flare-boost-pumpkaboo, normal-gengar, safe-one-usable, reusable-spread-ai |
| 66 | `BATTLE_066_MAUVILLE_GYM_BEN` | double | 9.3 | Two passive-but-dangerous trap leads, Good as Gold removal tax, then rare bulky phazing cashout with no speed field. | Can the player stop the maze before layers accumulate, remove hazards through the correct target, and block forced switching before every reserve pays another entry toll? | gym-double, guarded-double, hazard-maze, toxic-debris-glimmet, red-card-skarmory, good-as-gold-gholdengo, guzzlord, dragon-tail, removal-tax, switch-puzzle, no-speed-mode, reusable-phazing-ai |
| 67 | `BATTLE_067_MAUVILLE_GYM_SHAWN_ANGELO` | native-pair double | 9.2 | Proactive Stuff Cheeks opening, reactive berry thresholds, cross-trainer second-item transfers, and two independent item-economy singles. | Can the player control when a berry threshold occurs and which flower still owns the second item, or deny the donor before one threshold becomes two resources? | gym-native-pair, joint-3-plus-3, split-four-singles, symbiosis, second-item-relay, stuff-cheeks, cheek-pouch, gluttony, eternal-floette, flower-collection, safe-one-usable, unburden-rejected |
| 68 | `BATTLE_068_MAUVILLE_GYM_WATTSON` | double | 10 | One guaranteed Terrain/Tailwind opening followed by native resistance-and-base-damage reserve selection, reciprocal speed modes, priority denial, and a dynamically deployed story Mega. | Can the player dismantle the fixed fast lead, then read the adaptively selected reserve pair and choose whether to contest Terrain, priority, or move order before Mega Raichu appears? | required-gym-boss, guarded-double, target-ten, four-of-six-electric, world-champion-spine, electric-terrain, adaptive-reserve-selection, lightning-rod-ally-activation, dual-speed-mode, trick-room, tailwind, armor-tail, iron-hands, tapu-koko, emolga, mega-raichu-y, no-guard, reciprocal-mega, no-scripted-heal |
| 69 | `BATTLE_069_ROUTE_117_ANNA_AND_MEG` | double | 9 | One visible ally-targeting activation, Defense-powered physical pressure, then conditional inheritance or immediate reserve offense selected by the native matchup logic rather than a speed field. | Can the player stop the initial multi-hit Defense conversion, then choose a knockout order that limits Receiver or a matchup-selected Power of Alchemy branch before Pure Power Medicham creates another phase? | route-double, guarded-double, beat-up-stamina, body-press-conversion, receiver, power-of-alchemy, ability-inheritance-relay, knockout-order, passimian, alolan-muk, pure-power-medicham, no-speed-field, no-mega, no-legendary |
| 70 | `BATTLE_070_ROUTE_117_ISAAC` | double | 9.2 | Slow protected setup lead, then adaptive physical priority, redirection and sleep, or fragile special pressure without weather, terrain, room, Tailwind, screens, or hazards. | Can the player deny or contain Munchlax's Belly Drum through Togepi's active redirection, then correctly reclassify the board when native matchup logic selects a guardian, second redirector, or special attacker? | route-double, guarded-double, day-care-class, five-young-plus-guardian, munchlax, togepi, follow-me, belly-drum, gluttony-figy, mega-kangaskhan, parental-bond, foongus-redirection, abra-sash, staryu-analytic, adaptive-reserve-selection, no-speed-field, no-weather, no-legendary |

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
