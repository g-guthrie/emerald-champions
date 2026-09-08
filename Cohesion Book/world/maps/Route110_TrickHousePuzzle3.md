# Route110_TrickHousePuzzle3

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE3` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE3` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle3:object_events:001` | 7,19 | [Route110_TrickHousePuzzle3_EventScript_Justin](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L289) — Route110_TrickHousePuzzle3_EventScript_Justin at (7,19); I keep coming back to this same place! / I'm already having trouble, and then /  you have to beat me? It's not fair! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle3:object_events:002` | 4,4 | [Route110_TrickHousePuzzle3_EventScript_Martha](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L294) — Route110_TrickHousePuzzle3_EventScript_Martha at (4,4); I don't know what's going on here. /  I'm starting to feel sad… / You… You're awful! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle3:object_events:003` | 10,10 | [Route110_TrickHousePuzzle3_EventScript_Alan](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L299) — Route110_TrickHousePuzzle3_EventScript_Alan at (10,10); I don't get it. What would anyone want /  with a house this bizarre? / I don't get it. /  How did I lose? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle3:object_events:004` | 1,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_WOOD_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_3_WOOD_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle3:object_events:005` | 4,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SHADOW_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_3_SHADOW_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle3:object_events:006` | 1,20 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (1,20); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle3:object_events:007` | 2,21 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (2,21); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle3:coord_events:001` | 4,14 | [Route110_TrickHousePuzzle3_EventScript_Button1](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L202) — Coordinate trigger at (4,14); VAR_TEMP_1 == 0 invokes Route110_TrickHousePuzzle3_EventScript_Button1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle3:coord_events:002` | 3,11 | [Route110_TrickHousePuzzle3_EventScript_Button2](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L208) — Coordinate trigger at (3,11); VAR_TEMP_2 == 0 invokes Route110_TrickHousePuzzle3_EventScript_Button2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle3:coord_events:003` | 12,5 | [Route110_TrickHousePuzzle3_EventScript_Button3](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L214) — Coordinate trigger at (12,5); VAR_TEMP_3 == 0 invokes Route110_TrickHousePuzzle3_EventScript_Button3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle3:coord_events:004` | 8,2 | [Route110_TrickHousePuzzle3_EventScript_Button4](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L220) — Coordinate trigger at (8,2); VAR_TEMP_4 == 0 invokes Route110_TrickHousePuzzle3_EventScript_Button4. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle3:bg_events:001` | 0,14 | [Route110_TrickHousePuzzle3_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L278) — Route110_TrickHousePuzzle3_EventScript_Scroll at (0,14); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle3:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle3:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle3:warp_events:003` | 13,1 | Warp from (13,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle3:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route110_TrickHousePuzzle3_OnResume](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls Route110_TrickHousePuzzle3_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHousePuzzle3:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHousePuzzle3_OnTransition](../../baseline/source/data/maps/Route110_TrickHousePuzzle3/scripts.inc#L12) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHousePuzzle3_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
