# Verdant source checkpoint

This repository is a source-only checkpoint for Verdant Inclement Emerald. It intentionally excludes compiled ROMs and proprietary base-ROM data.

## Current design

- Challenge Mode with strict level caps and Set-style trainer battles.
- Mostly doubles: 632 trainer records use doubles and 216 remain intentional singles.
- Doubles parties use even four- or six-Pokémon waves with foe-aware AI and curated archetype additions.
- Thirteen story bosses have hand-authored teams; Norman and Drake remain recurring singles duelists.
- Gym and League rematches use fixed authored formats rather than format/legend selectors.
- Seventeen remaining weak Pokémon/forms receive restrained stat corrections.
- Encounter upgrades provide high-value team-building options throughout the campaign.

## Reproducible checks

```sh
python3 scripts/verdant_doubles_conversion.py --check
python3 scripts/verdant_underused_balance.py --check
python3 scripts/verdant_encounter_upgrade.py --check
python3 scripts/verify_verdant.py
```

The compiled `.gba` is not part of this repository.
