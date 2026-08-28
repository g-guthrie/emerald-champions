# Verdant dossier packet — ELITE_FOUR_DRAKE

This packet is authoring context, not an implemented or closed design.

## Campaign anchor

- Tier: league_gauntlet
- Commitment: soft
- Target difficulty: 10
- Protected identity: A deliberate singles concentration test built around Dragon lures, setup restraint, and punishing preservation decisions.
- Signature reveal: A Dragon Mega plus a legendary-scale closer make the final Elite Four room feel like a duel, not a lesser doubles team.
- Trainer IDs: `TRAINER_DRAKE`

## Current observed mechanics baseline

- Strict Cap: `80`
- Level Semantics: `Trainer levels are offsets from the active strict cap.`
- Authored Difficulty Setting: `Hard`
- Medium Trainer Level Delta: `-2`
- Easy Trainer Level Delta: `-4`
- Difficulty Changes Only Opposing Trainer Levels: `True`
- Allowed Battle Transformations: `['Mega Evolution', 'Primal Reversion']`
- Forbidden Battle Transformations: `['Terastallization', 'Z-Moves', 'Dynamax', 'Gigantamax']`
- Automatic Healing Between Members: `False`
- Manual Overworld Bag Between Members: `True`
- Pokemon Menu Between Members: `True`
- Save Menu Between Members: `True`
- Items During Trainer Battles: `False`
- Party Composition Locked After Entry: `True`
- Drake Releases Player Control After Victory: `True`
- Hall4 Allows Normal Overworld Menu: `True`
- Champion Room Forces Approach After Entry: `True`
- Mechanics Proposal: `None`

## Current source baseline

- `TRAINER_DRAKE`: single, 6 Pokémon, source party `sParty_Drake`

## Rolling context

- Available: False
- Reason: This future marquee anchor has campaign neighbors but no trustworthy previous-ten chronological window yet; refresh before implementation.
- Protected neighbors: ELITE_FOUR_PHOEBE, ELITE_FOUR_GLACIA, CHAMPION_WALLACE, POSTGAME_GYM_REMATCH_CYCLE

## Ranked competitive candidates

### 1. `showdown:gen9randombattle:001`

- Query: dragon setup
- Evidence: full-sets / reproducible
- Roster: Chandelure, Blastoise, Dragonite, Enamorus-Therian, Giratina-Origin, Espeon
- Tags: choice-disruption, choice-item, healing, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 2. `showdown:gen9randombattle:010`

- Query: dragon setup
- Evidence: full-sets / reproducible
- Roster: Iron Valiant, Dragonite, Crabominable, Gyarados, Swampert, Iron Boulder
- Tags: choice-item, pivoting, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 3. `smogon:gen4ou:009`

- Query: dragon setup
- Evidence: full-sets / published-sample
- Roster: Dragonite, Bronzong, Gengar, Magnezone, Metagross, Tyranitar
- Tags: active-speed-control, hazards, offense, sand, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://pkmn.github.io/smogon/data/teams/index.json

### 4. `showdown:gen5randombattle:021`

- Query: dragon setup
- Evidence: full-sets / reproducible
- Roster: Hitmontop, Zebstrika, Exploud, Scrafty, Banette, Dialga
- Tags: choice-item, pivoting, priority, setup, trick-room
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 5. `showdown:gen7randombattle:022`

- Query: dragon setup
- Evidence: full-sets / reproducible
- Roster: Rotom-Frost, Sceptile, Dragonite, Empoleon, Celesteela, Furret
- Tags: choice-item, hazards, pivoting, priority, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 6. `smogon:gen4ou:001`

- Query: dragon setup
- Evidence: full-sets / published-sample
- Roster: Empoleon, Bronzong, Dragonite, Jirachi, Starmie, Tyranitar
- Tags: choice-item, hazards, offense, priority, sand, setup, trick-room
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://pkmn.github.io/smogon/data/teams/index.json

### 7. `smogon:gen5ou:012`

- Query: dragon setup
- Evidence: full-sets / published-sample
- Roster: Garchomp, Breloom, Dragonite, Scizor, Starmie, Volcarona
- Tags: hazards, priority, setup
- Strategy: Gen 5 OU Smurf Double LO Plate Volc
- Original gimmicks: none recorded
- Source: https://pkmn.github.io/smogon/data/teams/index.json

### 8. `smogon:gen5ou:013`

- Query: dragon setup
- Evidence: full-sets / published-sample
- Roster: Garchomp, Breloom, Dragonite, Scizor, Starmie, Terrakion
- Tags: hazards, priority, setup
- Strategy: Terrak Chomp HO
- Original gimmicks: none recorded
- Source: https://pkmn.github.io/smogon/data/teams/index.json

### 9. `smogon:gen8ou:003`

- Query: dragon setup
- Evidence: full-sets / published-sample
- Roster: Tapu Koko, Clefable, Dragonite, Ferrothorn, Landorus-Therian, Victini
- Tags: active-speed-control, choice-disruption, electric-terrain, hazards, healing, pivoting, setup
- Strategy: Sticky Barb Clef + Encore Tini
- Original gimmicks: none recorded
- Source: https://pkmn.github.io/smogon/data/teams/index.json

### 10. `smogon:gen8uu:011`

- Query: dragon setup
- Evidence: full-sets / published-sample
- Roster: Krookodile, Cobalion, Gyarados, Necrozma, Salamence, Scolipede
- Tags: hazards, setup
- Strategy: DD Triple
- Original gimmicks: none recorded
- Source: https://pkmn.github.io/smogon/data/teams/index.json

### 11. `showdown:gen4randombattle:005`

- Query: dragon setup
- Evidence: full-sets / reproducible
- Roster: Latios, Prinplup, Shelgon, Sealeo, Hitmonlee, Staraptor
- Tags: hazards, healing, priority, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 12. `showdown:gen4randombattle:017`

- Query: dragon setup
- Evidence: full-sets / reproducible
- Roster: Slakoth, Dewgong, Shelgon, Bronzong, Primeape, Wormadam
- Tags: hazards, offense, pivoting, rain, screens, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 13. `elite:giovanni-cischke:worlds-2025`

- Query: campaign reservation
- Evidence: roster-only-indexed-full-sheet / world-champion
- Roster: Brute Bonnet, Lunala, Ursaluna, Chi-Yu, Koraidon, Flutter Mane
- Tags: choice-scarf, dual-speed-mode, guts, redirection, spore, sun, trick-room, wide-guard
- Strategy: Fast Koraidon, Chi-Yu, and Flutter mode transitions into Lunala Trick Room and Guts Ursaluna.
- Original gimmicks: none recorded
- Source: https://limitlessvgc.com/tournaments/399/teams

## Required next action

Inspect the strongest candidates in full, author every dossier field, run `verdant_marquee_design_audit.py`, report the design, and leave game source untouched until chronological implementation.
