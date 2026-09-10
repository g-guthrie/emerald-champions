# NPC B additional C string review — September 9, 2026

Read all **1,226 string-bearing source lines**, then distinguished **1,189 encoded text literals in five C files** from source comments, resource paths, and includes. These counts include empty, control-only, unused, and conditional fallback literals. They are separate from the 1,589 assembly dialogue blocks in the original NPC B assignment. Changed **13 text literals in two C files**. No gameplay code was edited by this reviewer.

| File | Encoded literals read | Text literals edited |
|---|---:|---:|
| `src/strings.c` | 1,069 | 11 |
| `src/party_menu.c` | 17 | 2 |
| `src/pokemon_summary_screen.c` | 20 | 0 |
| `src/pokemon_storage_system.c` | 82 | 0 |
| `src/emerald_champions_battle_sets.c` | 1 | 0 |

The precise reviewed literal inventory and before/after text is in `npcs_b-c-reviewed.json`. Included header contents are outside this five-file assignment; they are not counted or silently claimed as reviewed here.

## Corrections and consumer evidence

- **Seven active Frontier Pass descriptions** (`gText_BattleTowerDesc`, `gText_BattleDomeDesc`, `gText_BattlePalaceDesc`, `gText_BattleArenaDesc`, `gText_BattleFactoryDesc`, `gText_BattlePikeDesc`, `gText_BattlePyramidDesc`) formerly promised retired tournaments, autonomous Palace moves, rentals, path selection, and exploration. The landmark table and description printers in `src/frontier_pass.c` still consume these strings. The descriptions now identify each landmark and its current Circuit role. The Tower's current Circuit script describes the central arena, full-team Double Battles, and registration at every Frontier desk. The old facilities retain their identities without advertising unavailable rules.
- **Five EV item messages** now say EVs instead of base stats or base points. `GetMedicineItemEffectMessage` in `src/party_menu.c` selects `gText_PkmnBaseVar2StatIncreased` for the six EV effects. `ItemUseCB_ReduceEV` selects `gText_PkmnFriendlyBaseVar2Fell`, `gText_PkmnAdoresBaseVar2Fell`, or `gText_PkmnFriendlyBaseVar2CantFall` according to actual friendship and EV changes. `ItemUseCB_ResetEVs` uses `sText_BasePointsResetToZero` after resetting EVs. Identifiers remain unchanged; only their displayed language was corrected.
- **Catch-swap refusal grammar:** changed “a HM” to “an HM” in `sText_CannotSendMonToBoxHM`. Kept its actual claim: `DoesSelectedMonKnowHM` tests known HM moves, and `B_CATCH_SWAP_CHECK_HMS` is TRUE. This is distinct from the field-use capability system.

## Reviewed and retained

- Summary IV/EV utility labels are behind `P_SUMMARY_SCREEN_IV_EV_INFO == FALSE` with no enabling flag. They were read, but do not establish a player-facing IV progression feature in this build. “Recommended” is the sole encoded string defined directly in the battle-set implementation; its preset consumers set perfect IVs. The text does not imply a stat-point grind or an IV reward.
- Move-relearner TM consumption feedback remains a truthful fallback. Its actual consumer prints the consuming variant only when `ShouldConsumeTmItem()` returns true. The enabled `P_ENABLE_ALL_TM_MOVES` setting makes that return false, so the free current path does not promise consumption.
- HM-forgetting refusal remains consistent with `CannotForgetMove()` and `P_CAN_FORGET_HIDDEN_MOVE == FALSE`; the script-all-moves relearning path has its explicit bypass. No refusal rule was changed to fit prose.
- Recorded-battle strings remain historical fallback UI. `B_RECORDED_BATTLES_ENABLED == FALSE` prevents the Frontier Pass from reporting an available recording. They were not relabeled as Circuit streak records, which use a different consumer.
- The old singular `gText_BirchInTrouble` rescue prompt has no consumer in `src` or `data`; it was retained as unused source. The current two-starter flow is owned by the campaign reviewer.
- Party/storage commands, move information, summaries, labels, placeholder/control strings, menu descriptions, item feedback, and the remaining ordinary text were read and retained where they match their role. No Retry/Reload choice text is physically defined in these five files, so this scope does not constitute review of the defeat-choice consumer elsewhere.

## Evidence limits and concurrency

This is complete source-string reading with targeted consumer/configuration review, not runtime traversal or rendered UI validation. No builds, tests, native battles, or UI rendering were run. Source reachability for every historical fallback was not exhaustively proven.

The pre-edit snapshot is `npcs_b-c-before/`. Concurrent non-text changes appeared in `src/party_menu.c` and `src/emerald_champions_battle_sets.c` during this review and were preserved. The inventory compares encoded literals separately and records this explicitly. This reviewer's patch changes exactly 13 literal contents; a whole-file snapshot diff must not attribute other agents' mechanics to this dialogue work.
