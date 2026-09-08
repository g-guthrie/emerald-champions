# Route110_TrickHousePuzzle7

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE7` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE7` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle7:object_events:001` | 9,20 | [Route110_TrickHousePuzzle7_EventScript_Joshua](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L287) — Route110_TrickHousePuzzle7_EventScript_Joshua at (9,20); The TRICK MASTER always vanishes /  like smoke. How does he do it? / Aiyeeeh! You're much too strong! /  How do you do it? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle7:object_events:002` | 10,2 | [Route110_TrickHousePuzzle7_EventScript_Alexis](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L297) — Route110_TrickHousePuzzle7_EventScript_Alexis at (10,2); Whoever wins will get through here /  first. That's the feeling I get. / Oh! /  Well, go ahead, then! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle7:object_events:003` | 8,17 | [Route110_TrickHousePuzzle7_EventScript_Patricia](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L292) — Route110_TrickHousePuzzle7_EventScript_Patricia at (8,17); Going around the same spot… /  It begets ill fortune… / Defeated! /  It's a bad sign… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle7:object_events:004` | 5,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_TROPIC_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_7_TROPIC_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle7:object_events:005` | 9,2 | [Route110_TrickHousePuzzle7_EventScript_Alvaro](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L307) — Route110_TrickHousePuzzle7_EventScript_Alvaro at (9,2); I ever so closely watched you coming! / This outcome I didn't see coming… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle7:object_events:006` | 8,13 | [Route110_TrickHousePuzzle7_EventScript_Mariela](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L302) — Route110_TrickHousePuzzle7_EventScript_Mariela at (8,13); Nufufufu, here at last! /  Let's get right with it! / You're so casual about winning! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle7:object_events:007` | 9,12 | [Route110_TrickHousePuzzle7_EventScript_Everett](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L312) — Route110_TrickHousePuzzle7_EventScript_Everett at (9,12); It's awfully cramped in here… / Oh, yes, strong you are. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle7:object_events:008` | 4,17 | Passive/staged OBJ_EVENT_GFX_TRICK_HOUSE_STATUE at (4,17); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route110_TrickHousePuzzle7:object_events:009` | 4,6 | Passive/staged OBJ_EVENT_GFX_TRICK_HOUSE_STATUE at (4,6); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route110_TrickHousePuzzle7:coord_events:001` | 8,19 | [Route110_TrickHousePuzzle7_EventScript_YellowButton](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L226) — Coordinate trigger at (8,19); VAR_TEMP_1 == 0 invokes Route110_TrickHousePuzzle7_EventScript_YellowButton. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle7:coord_events:002` | 0,14 | [Route110_TrickHousePuzzle7_EventScript_BlueButton](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L238) — Coordinate trigger at (0,14); VAR_TEMP_1 == 0 invokes Route110_TrickHousePuzzle7_EventScript_BlueButton. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle7:coord_events:003` | 6,6 | [Route110_TrickHousePuzzle7_EventScript_GreenButton](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L250) — Coordinate trigger at (6,6); VAR_TEMP_1 == 0 invokes Route110_TrickHousePuzzle7_EventScript_GreenButton. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle7:coord_events:004` | 9,7 | [Route110_TrickHousePuzzle7_EventScript_PurpleButton](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L262) — Coordinate trigger at (9,7); VAR_TEMP_1 == 0 invokes Route110_TrickHousePuzzle7_EventScript_PurpleButton. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHousePuzzle7:bg_events:001` | 6,17 | [Route110_TrickHousePuzzle7_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L98) — Route110_TrickHousePuzzle7_EventScript_Scroll at (6,17); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle7:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:003` | 13,1 | Warp from (13,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:004` | 13,4 | Warp from (13,4, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:005` | 7,3 | Warp from (7,3, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:006` | 13,11 | Warp from (13,11, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:007` | 4,3 | Warp from (4,3, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:008` | 1,17 | Warp from (1,17, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:009` | 0,11 | Warp from (0,11, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:010` | 2,3 | Warp from (2,3, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:011` | 4,13 | Warp from (4,13, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:012` | 1,3 | Warp from (1,3, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:warp_events:013` | 8,12 | Warp from (8,12, elevation 0) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE7 warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle7:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route110_TrickHousePuzzle7_OnResume](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L11) — MAP_SCRIPT_ON_RESUME calls Route110_TrickHousePuzzle7_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHousePuzzle7:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHousePuzzle7_OnTransition](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L74) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHousePuzzle7_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHousePuzzle7:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [Route110_TrickHousePuzzle7_OnLoad](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L86) — MAP_SCRIPT_ON_LOAD calls Route110_TrickHousePuzzle7_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHousePuzzle7:map_scripts:004` | MAP_SCRIPT_ON_FRAME_TABLE | [Route110_TrickHousePuzzle7_OnFrame](../../baseline/source/data/maps/Route110_TrickHousePuzzle7/scripts.inc#L90) — MAP_SCRIPT_ON_FRAME_TABLE calls Route110_TrickHousePuzzle7_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
