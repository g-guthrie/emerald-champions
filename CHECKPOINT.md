# Verdant source checkpoint

This repository is a source-only checkpoint for Verdant Inclement Emerald. It intentionally excludes compiled ROMs and proprietary base-ROM data.

## Current design

- Challenge Mode with strict level caps and Set-style trainer battles.
- Mostly doubles: 574 trainer records use doubles and 280 remain intentional singles after low-stakes route relief.
- Opponent levels are fixed to the active strict cap plus each slot's authored offset; they no longer scale down to the player's current level.
- Doubles opponents use four- or six-Pokémon waves; the six story-partner branches use native three-Pokémon multi-battle parties.
- All 854 trainer records / 3,394 party slots pass the engine-effective team audit with no incomplete moves, invalid offsets, incompatible items, major doubles-plan gaps, repeated rematch rosters, or route showcase gaps.
- Thirteen story bosses have hand-authored teams; Norman and Drake remain recurring singles duelists.
- The competitive reference library contains 983 complete teams: 390 Showdown random teams, 390 verified VGC event winners, and 203 Smogon Gen 4-9 OU/UU/NU samples.
- A campaign-ordered guide documents 810 reachable battle definitions with cap levels, items, moves, team purpose, AI behavior, difficulty, uniqueness, and counterplay; 44 unreachable internal definitions are explicitly excluded.
- A three-layer AI audit guards ally/collateral scoring, tactical switching, and 56 trainer-specific profile assignments.
- Gym and League rematches use fixed authored formats rather than format/legend selectors.
- Seventeen remaining weak Pokémon/forms receive restrained stat corrections.
- Encounter upgrades provide high-value team-building options throughout the campaign.
- A source-backed 30-question audit guards progression, incentives, dialogue, menus, AI, and encounter logic.

## Reproducible checks

```sh
python3 scripts/verdant_doubles_conversion.py --check
python3 scripts/verdant_underused_balance.py --check
python3 scripts/verdant_encounter_upgrade.py --check
python3 scripts/verdant_team_polish.py --check
python3 scripts/verdant_team_quality_audit.py --check
python3 scripts/verdant_battle_guide.py --check
python3 scripts/verdant_ai_audit.py
python3 scripts/verdant_logical_audit.py
python3 scripts/verify_verdant.py
```

The compiled `.gba` is not part of this repository.
