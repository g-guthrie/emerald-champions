# Verdant dossier packet — ELITE_FOUR_PHOEBE

This packet is authoring context, not an implemented or closed design.

## Campaign anchor

- Tier: league_gauntlet
- Commitment: soft
- Target difficulty: 10
- Protected identity: The League's definitive supernatural survival clock, with trapping and redirection executed by bespoke-capable AI.
- Signature reveal: Two credible Perish singers and a Ghost Mega make switching, preservation, and timer arithmetic the battle.
- Trainer IDs: `TRAINER_PHOEBE`

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

- `TRAINER_PHOEBE`: double, 6 Pokémon, source party `sParty_Phoebe`

## Rolling context

- Available: False
- Reason: This future marquee anchor has campaign neighbors but no trustworthy previous-ten chronological window yet; refresh before implementation.
- Protected neighbors: SOOTOPOLIS_GYM_JUAN, ELITE_FOUR_SIDNEY, ELITE_FOUR_GLACIA, ELITE_FOUR_DRAKE

## Ranked competitive candidates

### 1. `elite:wolfe:orlando-2023`

- Query: perish
- Evidence: full-sets / exact-public-sheet
- Roster: Scream Tail, Palafin, Arcanine, Flutter Mane, Amoonguss, Gothitelle
- Tags: dedicated-perish, disable, fake-out, heal-pulse, healing, perish, perish-trap, positioning-control, priority, redirection, shadow-tag, sleep, survival-clock, two-singers
- Strategy: Dedicated Perish mode with two singers and multiple survival tools. Reserve once for a late notorious trainer; ordinary damage AI is insufficient.
- Original gimmicks: Tera in original format
- Source: https://limitlessvgc.com/tournaments/298/teams

### 2. `elite:wolfe:euic-2025`

- Query: perish
- Evidence: full-sets-strategy-documented / player-explained
- Roster: Scream Tail, Flutter Mane, Incineroar, Koraidon, Gothitelle, Amoonguss
- Tags: active-speed-control, encore-disable, fake-out, low-variance, perish, perish-trap, pivoting, positioning-control, preview-pressure, primary-offense, redirection, secondary-perish, shadow-tag, sleep, sun
- Strategy: Wolfe described Koraidon offense as the primary plan and Perish trapping as a credible secondary mode that taxes preparation even when unused.
- Original gimmicks: Tera in original format
- Source: https://limitlessvgc.com/teams/3499

### 3. `elite:wolfe:toronto-2024`

- Query: perish
- Evidence: full-sets-strategy-documented / player-explained
- Roster: Archaludon, Kingdra, Politoed, Incineroar, Gothitelle, Amoonguss
- Tags: eject-redirection, encore-trap, fake-out, healing, perish, perish-trap, pivoting, positioning-control, primary-rain-offense, rain, redirection, secondary-perish, shadow-tag, sleep, swift-swim, trick-room, trick-room-option
- Strategy: Politoed enables both direct Kingdra and Archaludon offense and the secondary Gothitelle Perish mode. Encore plus Shadow Tag punishes passive choices.
- Original gimmicks: Tera in original format
- Source: https://limitlessvgc.com/tournaments/366/teams

### 4. `elite:marco-silva:turin-2026`

- Query: perish
- Evidence: roster-only-indexed-full-sheet / verified-event-champion
- Roster: Incineroar, Floette-Eternal-Mega, Sneasler, Kingambit, Gengar-Mega, Kommo-o
- Tags: clangorous-soul, disable, dual-mega-preview, perish, pivoting, swords-dance
- Strategy: Alternative-Mega team with Gengar control and independent Kommo-o and Kingambit setup endgames.
- Original gimmicks: none recorded
- Source: https://limitlessvgc.com/tournaments/435/teams

### 5. `elite:wolfe:naic-2026`

- Query: trap
- Evidence: full-sets / exact-public-sheet
- Roster: Mamoswine, Toxapex, Sneasler, Hydreigon, Charizard, Incineroar
- Tags: choice-item, fake-out, fast-mode, infestation-trap, pivoting, priority, role-compression, stall-offense-hybrid, sun, toxic-clock, wide-guard
- Strategy: Toxapex creates a trapping and Toxic clock while Charizard-Y, Hydreigon, and Sneasler prevent the team from becoming passive. Strong late Admin or boss template.
- Original gimmicks: Mega
- Source: https://limitlessvgc.com/teams/6694

### 6. `showdown:gen6randomdoublesbattle:006`

- Query: trap
- Evidence: full-sets / reproducible
- Roster: Butterfree, Blissey, Sunflora, Dugtrio, Slowbro, Qwilfish
- Tags: healing, priority, setup
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

### 7. `showdown:gen8randomdoublesbattle:019`

- Query: trap
- Evidence: full-sets / reproducible
- Roster: Salamence, Bouffalant, Palkia, Arctozolt, Vileplume, Dugtrio
- Tags: active-speed-control, healing, tailwind
- Strategy: not documented
- Original gimmicks: none recorded
- Source: https://github.com/smogon/pokemon-showdown

## Required next action

Inspect the strongest candidates in full, author every dossier field, run `verdant_marquee_design_audit.py`, report the design, and leave game source untouched until chronological implementation.
