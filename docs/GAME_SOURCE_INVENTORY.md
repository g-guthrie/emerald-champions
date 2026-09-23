# Game source inventory — September 19, 2026

This working audit shortlist contains **48 core C files**, totaling approximately **165,000 lines**. It is a curated system map, not an exhaustive dependency closure or a claim that every listed module is active in every campaign path. Related split-out helpers, configuration, headers and authored data are also required. Size alone is misleading: some large files are animation code or generated trainer tables, while small files can govern important progression rules.

## Battle mechanics

- [battle_main.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_main.c>) — 237 KiB
- [battle_util.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_util.c>) — 413 KiB
- [battle_script_commands.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_script_commands.c>) — 411 KiB
- [battle_move_resolution.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_move_resolution.c>) — 228 KiB
- [battle_controllers.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_controllers.c>) — 132 KiB
- [battle_setup.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_setup.c>) — 87 KiB
- [battle_gimmick.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_gimmick.c>) — 15 KiB
- [battle_stat_change.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_stat_change.c>) — 34 KiB
- [battle_hold_effects.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_hold_effects.c>) — 39 KiB
- [battle_message.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_message.c>) — 212 KiB

## Authored doubles AI

- [battle_ai_main.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_ai_main.c>) — 299 KiB
- [battle_ai_pair.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_ai_pair.c>) — 316 KiB
- [battle_ai_util.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_ai_util.c>) — 265 KiB
- [battle_ai_switch.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_ai_switch.c>) — 146 KiB
- [battle_ai_record.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/battle_ai_record.c>) — 3 KiB
- [emerald_champions_battle_plan.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/emerald_champions_battle_plan.c>) — 5 KiB
- [emerald_champions_perish.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/emerald_champions_perish.c>) — 4 KiB

## Pokemon, parties and progression

- [pokemon.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/pokemon.c>) — 231 KiB
- [party_menu.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/party_menu.c>) — 291 KiB
- [pokemon_storage_system.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/pokemon_storage_system.c>) — 279 KiB
- [pokemon_summary_screen.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/pokemon_summary_screen.c>) — 149 KiB
- [evolution_scene.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/evolution_scene.c>) — 63 KiB
- [daycare.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/daycare.c>) — 47 KiB
- [trade.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/trade.c>) — 170 KiB

## World, movement and story scripting

- [overworld.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/overworld.c>) — 118 KiB
- [event_object_movement.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/event_object_movement.c>) — 413 KiB
- [field_control_avatar.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/field_control_avatar.c>) — 50 KiB
- [field_player_avatar.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/field_player_avatar.c>) — 73 KiB
- [field_move.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/field_move.c>) — 10 KiB
- [field_effect.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/field_effect.c>) — 142 KiB
- [field_specials.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/field_specials.c>) — 226 KiB
- [script.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/script.c>) — 19 KiB
- [scrcmd.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/scrcmd.c>) — 88 KiB
- [event_data.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/event_data.c>) — 6 KiB
- [wild_encounter.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/wild_encounter.c>) — 60 KiB

## Items, economy and player menus

- [item.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/item.c>) — 40 KiB
- [item_use.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/item_use.c>) — 57 KiB
- [item_menu.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/item_menu.c>) — 100 KiB
- [shop.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/shop.c>) — 44 KiB
- [start_menu.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/start_menu.c>) — 46 KiB
- [region_map.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/region_map.c>) — 93 KiB
- [pokenav.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/pokenav.c>) — 17 KiB

## Boot, saves and campaign-specific entrypoints

- [main.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/main.c>) — 11 KiB
- [new_game.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/new_game.c>) — 7 KiB
- [save.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/save.c>) — 33 KiB
- [load_save.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/load_save.c>) — 10 KiB
- [emerald_champions_story.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/emerald_champions_story.c>) — opening, regional rival setup and finale progression (combined from the two former modules)
- [champions_circuit.c](</Users/gguthrie/Projects/Pokémon Emerald Champions/game-source/src/champions_circuit.c>) — 67 KiB

## Authoritative campaign data and build inputs

- `data/emerald_champions/emerald_champions_battle_teams.txt`: authored teams, offsets, tactics and plans.
- `scripts/emerald_champions_teams.py` and `scripts/implement_emerald_champions_master_battles.py`: materialize trainer data and profiles.
- `src/data/trainers.party`, `src/data/emerald_champions_battle_plans.h`: materialized battle inputs.
- `src/data/pokemon/`, `src/data/moves_info.h`, `src/data/items.h`: species, learnsets, forms, moves and items.
- `data/maps/*/scripts.inc`, `data/maps/*/map.json`: NPC behavior, map callbacks, event placements and connections.
- `data/scripts/`, `data/event_scripts.s`, `data/battle_scripts_*.s`: shared event and battle scripts.
- `src/data/wild_encounters.json`: wild encounter tables; `data/scripts/wild_encounter.inc`: shared encounter scripts.
- `include/`, especially `config/` and `constants/`: mechanics settings, IDs, flags, variables and structural definitions.
- `Makefile`, `.mk` rules, linker scripts, `charmap.txt`, `asm/`, relevant `scripts/` and `tools/`: build and runtime support.

## Live source-tree counts

- 394 top-level engine C files.
- 491 headers under include/.
- 186 source-data files under src/data/ with C/header/JSON/party/include suffixes.
- 908 map script files and 958 map definitions (this repository includes alternate/donor content, not just active Hoenn maps).
- 97 shared event-script includes.

These counts exclude artwork/audio but are not a minimal build manifest. A full rebuild additionally needs the original assets, binary map/layout data, generated-input recipes, and the configured compiler/toolchain. Keep the entire repository for reproducibility; do not copy only this shortlist. Test and headless harnesses are valuable verification support rather than normal player-facing game dynamics.
