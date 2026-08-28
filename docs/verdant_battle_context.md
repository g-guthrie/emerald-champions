# Verdant rolling battle context

## Next design: Battle 144 — `BATTLE_144_ASHEN_WOODS_ALANNAH`

- Location: AshenWoods
- Category: optional Ashen Woods trainer
- Strict cap: 45 (canonical-sequence-stage)
- Source format: single
- Rolling window: 10 encounter(s) (Battles 134–143)
- Ledger: v2 ledger loaded

## Hard errors

- None.

## Advisory warnings

- **NOVELTY_TAG_REPEAT**: One or more mechanics recur in at least three rolling encounters.
  Evidence: `{"route111-north": 10}`
- **PROTECT_DENSITY**: Rolling protect density is high enough to merit an editorial look, not an automatic change.
  Evidence: `{"density": 0.516, "threshold": 0.45}`
- **SPEED_CONTROL_DENSITY**: Rolling speed control or priority density is high enough to merit an editorial look, not an automatic change.
  Evidence: `{"density": 0.826, "threshold": 0.55}`
- **PREMIUM_ITEM_REPEAT**: A premium item appears across at least four rolling encounters.
  Evidence: `{"Assault Vest": 5, "Focus Sash": 5, "Life Orb": 6}`
- **SIGNATURE_MOVE_REPEAT**: A non-generic move appears across at least four rolling encounters.
  Evidence: `{"Close Combat": 4, "Earthquake": 4, "Rock Slide": 4, "Sludge Bomb": 4}`

## Rolling experience ledger

| # | Encounter | Format | Difficulty | Tempo | Primary question | Novelty tags |
| ---: | --- | --- | ---: | --- | --- | --- |
| 134 | `BATTLE_134_ROUTE_111_DAISUKE` | single | 8.9 | Sash setup and Baton Pass threaten a relay; Wonder Guard changes the answer; Marshadow steals any careless boost and closes. | Can the player deny Ninjask's relay without spending the move or passive-damage answer needed for Shedinja and Marshadow? | route111-north, three-shadow-relay, baton-pass, wonder-guard, spectral-thief |
| 135 | `BATTLE_135_ROUTE_111_WILTON` | mixed | 9.3 | Burn and room control open the ruin; Magic Guard and Rough Skin punish direct races; Lapras and Metagross deepen rematches before one psychic Mega finale. | Can the player solve a slow ruin in singles, then adapt when Trick Room, grounded spread damage, armor, and Mega Mewtwo Y become one doubles formation? | route111-north, ruin-guardians, trick-room, magic-guard, rough-skin, mega-mewtwo-y, mixed-rematch-family |
| 136 | `BATTLE_136_ROUTE_111_BROOKE` | double | 9.4 | Fake Out buys Empoleon control; Roserade and Haxorus force opposite answers; Scizor and Latios turn later rematches into a six-stage elemental relay. | Can the player keep changing defensive answers as Water, Fire, Grass, Dragon, Bug, Steel, and Psychic pass initiative across the board? | route111-north, elemental-relay, competitive-empoleon, fake-out, dragon-dance, mega-latios, double-rematch-family |
| 137 | `BATTLE_137_ROUTE_111_HAYDEN` | double | 9.1 | Two Toxic Orbs advertise opposite payoffs; Vaporeon supplies speed and Helping Hand while Drapion turns high-crit edges into the finish. | Can the player exploit the activation turn before poison becomes healing, boosted Facade, and Sniper pressure? | route111-north, toxic-edge, poison-heal, toxic-boost, helping-hand, sniper |
| 138 | `BATTLE_138_ROUTE_111_BIANCA` | double | 9.2 | Flare Boost Gourgeist can reverse speed while Gengar attacks immediately; Choice Band Sudowoodo and Assault Vest Magmortar make the reserves physical and special props. | Can the player read whether haunted props want fast offense or Trick Room before a false tree and living furnace enter? | route111-north, haunted-props, flare-boost, trick-room-option, rock-head, mixed-reserves |
| 139 | `BATTLE_139_ROUTE_111_TYRON` | single | 8.9 | Scyther tests patient setup defense, Kabutops risks a Shell Smash race, and Mega Beedrill ends with immediate frail speed. | Can the player answer three different blade tempos: Eviolite setup, Shell Smash priority, and one blistering Mega? | route111-north, three-blades, eviolite-scyther, shell-smash, mega-beedrill |
| 140 | `BATTLE_140_ROUTE_111_CELINA` | double | 9.3 | Sash Victreebel spends a turn removing a partner's liability; Slaking, Golisopod, and Durant each become a different liberated threat. | Can the player stop Gastro Acid from turning Slow Start, Emergency Exit, and Truant into three unrestricted attackers? | route111-north, broken-shackles, gastro-acid, slow-start, emergency-exit, truant |
| 141 | `BATTLE_141_ROUTE_111_CELIA` | single | 8.8 | Rain Dish and Giga Drain begin the growth, Dry Skin Parasect adds Spore and seeds, and Mega Venusaur closes behind Thick Fat recovery. | Can the player prevent three different forms of recovery from turning one short single into an attrition lock? | route111-north, draining-growth, rain-dish, dry-skin, spore, mega-venusaur |
| 142 | `BATTLE_142_ROUTE_111_BRYAN` | double | 9.2 | Spiritomb blunts both damage categories, Scarf Tyrantrum fires recoil-free Head Smash, Contrary Serperior snowballs, and Sturdy Carracosta threatens one Shell Smash. | Can the player identify four unrelated survival rules before Rock Head, Contrary, and Sturdy convert them into offense? | route111-north, ancient-survivors, infiltrator, rock-head, contrary, sturdy-shell-smash |
| 143 | `BATTLE_143_ROUTE_111_BRANDEN` | double | 9.2 | Fake Out buys Steely Spirit's boosted Iron and Anchor attacks; Sirfetch'd brings critical pressure and Kartana supplies the frail Beast Boost finish. | Can the player remove Steely Spirit before Anchor Shot traps the wrong answer and three different blades start collecting KOs? | route111-north, anchor-crew, steely-spirit, steelworker, critical-blade, beast-boost |

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
