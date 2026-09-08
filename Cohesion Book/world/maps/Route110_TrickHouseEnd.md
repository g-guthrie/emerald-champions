# Route110_TrickHouseEnd

**REVISE.** Preserve earned stage rewards and final personal farewell. W-TRICK-REWARDS fixes the King’s Rock retry mismatch and separates tent/Alakazite receipts without replaying the maze.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHouseEnd/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_END` · `LAYOUT_ROUTE110_TRICK_HOUSE_END` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHouseEnd:object_events:001` | 4,5 | [Route110_TrickHouseEnd_EventScript_TrickMaster](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L42) — Route110_TrickHouseEnd_EventScript_TrickMaster at (4,5); Aak! /  You've made it to me? /  Hmmm… You're sharp! / It took me all night to plant all those /  trees… //  You're almost my equal in greatness by /  one, two, three, four, five, six places! | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-TRICK-REWARDS |
| `Route110_TrickHouseEnd:coord_events:001` | 2,2 | [Route110_TrickHouseEnd_EventScript_TrickMasterExitTrigger](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L224) — Coordinate trigger at (2,2); VAR_TEMP_2 == 0 invokes Route110_TrickHouseEnd_EventScript_TrickMasterExitTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHouseEnd:bg_events:001` | 4,5 | Hidden ITEM_NUGGET at (4,5); persistent flag FLAG_HIDDEN_ITEM_TRICK_HOUSE_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route110_TrickHouseEnd:warp_events:001` | 10,1 | Warp from (10,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE1 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseEnd:warp_events:002` | 2,1 | Warp from (2,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_CORRIDOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseEnd:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route110_TrickHouseEnd_OnResume](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L8) — MAP_SCRIPT_ON_RESUME calls Route110_TrickHouseEnd_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHouseEnd:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHouseEnd_OnTransition](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L12) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHouseEnd_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHouseEnd:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route110_TrickHouseEnd_OnFrame](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L28) — MAP_SCRIPT_ON_FRAME_TABLE calls Route110_TrickHouseEnd_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHouseEnd:map_scripts:004` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [Route110_TrickHouseEnd_OnWarp](../../baseline/source/data/maps/Route110_TrickHouseEnd/scripts.inc#L18) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls Route110_TrickHouseEnd_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
