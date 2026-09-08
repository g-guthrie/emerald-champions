# Route110_TrickHousePuzzle2

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE2` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE2` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle2:object_events:001` | 13,10 | [Route110_TrickHousePuzzle2_EventScript_Ted](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L87) — Route110_TrickHousePuzzle2_EventScript_Ted at (13,10); Which switch closes which hole? / After that battle, I'm even more /  confused! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle2:object_events:002` | 10,17 | [Route110_TrickHousePuzzle2_EventScript_Paul](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L92) — Route110_TrickHousePuzzle2_EventScript_Paul at (10,17); Oh! You're on your second TRICK HOUSE /  challenge! / You're good at battling too? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle2:object_events:003` | 11,9 | [Route110_TrickHousePuzzle2_EventScript_Georgia](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L97) — Route110_TrickHousePuzzle2_EventScript_Georgia at (11,9); I want to make my own GYM one day. /  So, I'm studying how to set traps. / I didn't study battling enough! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle2:object_events:004` | 8,17 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_WAVE_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_2_WAVE_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle2:object_events:005` | 3,13 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HARBOR_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_2_HARBOR_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle2:coord_events:001` | 11,12 | [Route110_TrickHousePuzzle2_EventScript_Button1](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L31) — Coordinate trigger at (11,12); VAR_TEMP_1 == 0 invokes Route110_TrickHousePuzzle2_EventScript_Button1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle2:coord_events:002` | 0,4 | [Route110_TrickHousePuzzle2_EventScript_Button2](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L40) — Coordinate trigger at (0,4); VAR_TEMP_2 == 0 invokes Route110_TrickHousePuzzle2_EventScript_Button2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle2:coord_events:003` | 14,5 | [Route110_TrickHousePuzzle2_EventScript_Button3](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L49) — Coordinate trigger at (14,5); VAR_TEMP_3 == 0 invokes Route110_TrickHousePuzzle2_EventScript_Button3. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle2:coord_events:004` | 7,11 | [Route110_TrickHousePuzzle2_EventScript_Button4](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L58) — Coordinate trigger at (7,11); VAR_TEMP_4 == 0 invokes Route110_TrickHousePuzzle2_EventScript_Button4. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle2:bg_events:001` | 14,14 | [Route110_TrickHousePuzzle2_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L20) — Route110_TrickHousePuzzle2_EventScript_Scroll at (14,14); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle2:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle2:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle2:warp_events:003` | 13,1 | Warp from (13,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle2:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route110_TrickHousePuzzle2_OnResume](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls Route110_TrickHousePuzzle2_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHousePuzzle2:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHousePuzzle2_OnTransition](../../baseline/source/data/maps/Route110_TrickHousePuzzle2/scripts.inc#L13) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHousePuzzle2_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
