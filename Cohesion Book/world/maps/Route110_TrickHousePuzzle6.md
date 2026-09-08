# Route110_TrickHousePuzzle6

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE6` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE6` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle6:object_events:001` | 7,9 | [Route110_TrickHousePuzzle6_EventScript_Sophia](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc#L30) — Route110_TrickHousePuzzle6_EventScript_Sophia at (7,9); When I heard there was a strange /  house, I had to check it out. / I've discovered a tough TRAINER! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle6:object_events:002` | 11,10 | [Route110_TrickHousePuzzle6_EventScript_Benny](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc#L35) — Route110_TrickHousePuzzle6_EventScript_Benny at (11,10); Maybe I could get my BIRD POKéMON /  to fly over the wall… / Gwaaah! I blew it! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle6:object_events:003` | 4,5 | [Route110_TrickHousePuzzle6_EventScript_Sebastian](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc#L40) — Route110_TrickHousePuzzle6_EventScript_Sebastian at (4,5); I'm getting dizzy from these rotating /  doors… / Everything's spinning around and /  around. I can't take this anymore… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle6:object_events:004` | 11,21 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GLITTER_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_6_GLITTER_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle6:bg_events:001` | 0,10 | [Route110_TrickHousePuzzle6_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc#L19) — Route110_TrickHousePuzzle6_EventScript_Scroll at (0,10); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle6:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle6:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle6:warp_events:003` | 13,1 | Warp from (13,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle6:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHousePuzzle6_OnTransition](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHousePuzzle6_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHousePuzzle6:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [Route110_TrickHousePuzzle6_OnWarp](../../baseline/source/data/maps/Route110_TrickHousePuzzle6/scripts.inc#L10) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls Route110_TrickHousePuzzle6_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
