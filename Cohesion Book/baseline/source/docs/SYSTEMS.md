# Game systems

This describes executable behavior, not a claim that every encounter is balanced or every campaign path has been completed.

## Retry and Reload

These are separate operations and both are intentional.

| Operation | What it restores | Entry point |
| --- | --- | --- |
| Retry the current battle | Allied parties as they entered the trainer battle; opponent parties are rebuilt through trainer generation | The Run action in eligible trainer battles offers Restart before the forfeit question |
| Reload the previous save | The saved game, discarding progress since that save | Start-menu Reload, with confirmation |

Retry uses `EcSnapshotForRestart`, `EcRestoreForRestart`, and `ReturnFromBattleToOverworld` in [battle_main.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/battle_main.c), plus `EmeraldChampions_RebuildTrainerBattleParties` in [battle_setup.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/battle_setup.c). Eligibility is defined by `CanPlayerForfeitNormalTrainerBattle`; do not infer that every battle format supports this path. It does not promise identical RNG outcomes.

Reload uses `CanReloadLastSave`, `ReloadLastSave`, and `ReloadSave` in [reload_save.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/reload_save.c). `ReloadSave` loads `SAVE_NORMAL` and returns through `CB2_ContinueSavedGame`. The confirmation and menu action live in [start_menu.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/start_menu.c). Reload is not a substitute for Retry.

## Difficulty and levels

The Options menu selects Hard, Medium, or Easy. The underlying enum calls Medium `DIFFICULTY_NORMAL`. `GetTrainerLevelReduction` in [difficulty.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/difficulty.c) applies Hard: 0, Medium: −2, Easy: −4 to generated opponent levels, with a minimum of level 1. `ApplyTrainerLevelDifficulty` recalculates stats and restores those opponents to full HP. Normal trainer setup, Retry rebuilding, and Circuit generation call it. This is not a universal reduction to every wild or scripted encounter.

`NewGameInitData` in [new_game.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/new_game.c) initializes Hard. Options changes are written by `Task_OptionMenuSave` in [option_menu.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/option_menu.c). `PrepareDifficultyForOptionMenu` runs the existing migration before title-screen Options accesses an already-loaded legacy save. `MigrateEmeraldChampionsCoreState` in [overworld.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/overworld.c) owns save-layout compatibility.

`GetCurrentLevelCap` in [caps.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/caps.c) returns the value associated with the first unset milestone. Before earning badges 1–8, the successive caps are 14, 20, 30, 40, 45, 55, 60, and 70. With all badges but before becoming Champion, the cap is 80; after all those flags are set, it is 100. Difficulty does not alter this table.

[caps.h](https://github.com/g-guthrie/emerald-champions/blob/main/include/config/caps.h) enables hard EXP caps and capped Rare Candy use. `ItemUseCB_RareCandy` in [party_menu.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/party_menu.c) handles the reusable Leveler and capped leveling. Ordinary wild generation clamps levels in `TryGenerateWildMon` and `GenerateFishingWildMon` in [wild_encounter.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/wild_encounter.c). Legendary Sign overrides have their own level calculation. A leveling cap is not proof that every acquisition or obedience path enforces an identical participation limit.

## Preparation

The native preparation menus are implemented in [emerald_champions.inc](https://github.com/g-guthrie/emerald-champions/blob/main/data/scripts/emerald_champions.inc). They expose move preparation, Nature changes, Stat Point editing, and authored battle sets. Nature changes call `SetHiddenNature` in [field_specials.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/field_specials.c) and recalculate stats. Ability selection uses `CursorCb_OpenAbilityMenu` and `CollectSelectableAbilitySlots` in [party_menu.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/party_menu.c); choices come from the species' available Ability slots.

`AdjustSelectedMonEmeraldChampionsStatPoints` in [field_specials.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/field_specials.c) enforces the remaining total budget and per-stat limit. `CalculateMonStatsCont` in [pokemon.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/pokemon.c) applies the Champions stat formula. [caps.h](https://github.com/g-guthrie/emerald-champions/blob/main/include/config/caps.h) disables EV gain from ordinary grinding.

Free item menus use `CreateFreePokemartMenu` and `GetShopItemPrice` in [shop.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/shop.c). Their item lists and archive entry functions live in [field_specials.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/field_specials.c); script conditions determine access. Free preparation does not mean every item is available everywhere: `IsEmeraldChampionsProtectedProgressionItem` and the preset-application checks in [emerald_champions_battle_sets.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/emerald_champions_battle_sets.c) distinguish ordinary equipment from protected transformation and progression items.

Trainer parties are consumed from [trainers.party](https://github.com/g-guthrie/emerald-champions/blob/main/src/data/trainers.party); wild tables are consumed from [wild_encounters.json](https://github.com/g-guthrie/emerald-champions/blob/main/src/data/wild_encounters.json). Their presence and structural validity do not establish strategic quality. Changes to authored sets require validation against the resulting game behavior.

## Legendary relic delivery

Legendary acquisitions grant their associated relics once. If neither the Bag nor item PC can accept an item, [legendary_signs.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/legendary_signs.c) records that individual item as pending. Free some item space and heal at a Pokémon Center to retry delivery. Captures and gift Pokémon keep their existing held items; partial deliveries remain recorded, so later retries do not recreate items already delivered or discarded.

Delivery state uses two existing save variables. Version 2 migration in [overworld.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/overworld.c) preserves supported prior progression and initializes only the new state. Historical Pokédex ownership suppresses replay of old grants; it never creates a pending reward. Items lost before this tracking existed cannot be safely reconstructed automatically.
