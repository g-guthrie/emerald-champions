# Verdant dossier packet — ELITE_FOUR_SIDNEY

This packet is authoring context, not an implemented or closed design.

## Campaign anchor

- Tier: league_gauntlet
- Commitment: soft
- Target difficulty: 10
- Protected identity: The gauntlet opens with Dark-type information warfare, priority, and multiple independent setup endgames.
- Signature reveal: A Dark Mega and one rare threat make targeting and Protect prediction dangerous from turn one.
- Trainer IDs: `TRAINER_SIDNEY`

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

- `TRAINER_SIDNEY`: double, 6 Pokémon, source party `sParty_Sidney`

## Rolling context

- Available: False
- Reason: This future marquee anchor has campaign neighbors but no trustworthy previous-ten chronological window yet; refresh before implementation.
- Protected neighbors: SEAFLOOR_CAVERN_ARCHIE, SOOTOPOLIS_GYM_JUAN, ELITE_FOUR_PHOEBE, ELITE_FOUR_GLACIA

## Ranked competitive candidates

### 1. `showdown:gen4randomdoublesbattle:007`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Nidorina, Scizor, Fearow, Darkrai, Salamence, Shiftry
- Tags: choice-item, hazards, healing, priority, setup, sleep
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 2. `showdown:gen4randomdoublesbattle:012`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Azumarill, Kyogre, Loudred, Shroomish, Darkrai, Metang
- Tags: choice-disruption, choice-item, hazards, offense, priority, rain, setup, sleep
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 3. `showdown:gen5randomdoublesbattle:030`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Darkrai, Shroomish, Volcarona, Fraxure, Steelix, Ferrothorn
- Tags: active-speed-control, choice-disruption, choice-item, hazards, healing, setup, sleep
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 4. `showdown:gen7randomdoublesbattle:024`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Silvally-Dark, Rayquaza, Floatzel, Scrafty, Mawile, Excadrill
- Tags: active-speed-control, fake-out, pivoting, priority, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 5. `showdown:gen9randomdoublesbattle:016`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Mewtwo, Solgaleo, Darkrai, Sawsbuck-Autumn, Pecharunt, Honchkrow
- Tags: healing, offense, priority, setup, tailwind
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 6. `showdown:gen5randomdoublesbattle:022`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Glameow, Shellos, Gallade, Houndoom, Lucario, Jolteon
- Tags: fake-out, healing, pivoting, positioning-control, priority, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 7. `showdown:gen5randomdoublesbattle:009`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Pidgey, Shedinja, Arceus-Fighting, Reshiram, Dusknoir, Mollux
- Tags: hazards, healing, priority, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 8. `showdown:gen6randomdoublesbattle:019`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Ho-Oh, Crawdaunt, Relicanth, Delcatty, Houndoom, Clefable
- Tags: active-speed-control, fake-out, healing, offense, positioning-control, priority, redirection, screens, setup, tailwind
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 9. `showdown:gen8randomdoublesbattle:003`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Klefki, Primarina, Eternatus, Poliwrath, Greedent, Bewear
- Tags: active-speed-control, hazards, healing, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 10. `showdown:gen8randomdoublesbattle:008`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Zoroark, Gengar, Dunsparce, Qwilfish, Primarina, Golurk
- Tags: active-speed-control, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 11. `showdown:gen8randomdoublesbattle:009`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Hydreigon, Venusaur, Farfetch’d, Necrozma-Dawn-Wings, Vanilluxe, Dugtrio-Alola
- Tags: active-speed-control, offense, priority, setup, sleep, snow, tailwind
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 12. `showdown:gen9championsrandomdoublesbattle:014`

- Query: dark setup
- Evidence: full-sets / reproducible
- Roster: Incineroar, Annihilape, Arbok, Armarouge, Toucannon, Ditto
- Tags: choice-item, fake-out, offense, pivoting, setup, tailwind
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 13. `elite:marco-silva:turin-2026`

- Query: campaign reservation
- Evidence: roster-only-indexed-full-sheet / verified-event-champion
- Roster: Incineroar, Floette-Eternal-Mega, Sneasler, Kingambit, Gengar-Mega, Kommo-o
- Tags: clangorous-soul, disable, dual-mega-preview, perish, pivoting, swords-dance
- Strategy: Alternative-Mega team with Gengar control and independent Kommo-o and Kingambit setup endgames.
- Original gimmicks: none recorded
- Source: https://limitlessvgc.com/tournaments/435/teams

## Required next action

Inspect the strongest candidates in full, author every dossier field, run `verdant_marquee_design_audit.py`, report the design, and leave game source untouched until chronological implementation.
