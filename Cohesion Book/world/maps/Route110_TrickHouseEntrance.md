# Route110_TrickHouseEntrance

**REVISE.** Preserve eight revisit gates and the detective-style hide-and-seek entrance. Pending rewards remain accessible; finished story text must not hide an unclaimed final part.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHouseEntrance/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_ENTRANCE` · `LAYOUT_ROUTE110_TRICK_HOUSE_ENTRANCE` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHouseEntrance:object_events:001` | 6,2 | [Route110_TrickHouseEntrance_EventScript_TrickMaster](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L221) — Route110_TrickHouseEntrance_EventScript_TrickMaster at (6,2); Hah? Grrr… //  How did you know I concealed myself /  beneath this desk? You're sharp! / Hah? Grrr… //  How did you know I concealed myself /  behind this tree? You're sharp! | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-TRICK-REWARDS |
| `Route110_TrickHouseEntrance:coord_events:001` | 4,7 | [Route110_TrickHouseEntrance_EventScript_TrickMasterHiding](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L662) — Coordinate trigger at (4,7); VAR_TRICK_HOUSE_BEING_WATCHED_STATE == 0 invokes Route110_TrickHouseEntrance_EventScript_TrickMasterHiding. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHouseEntrance:coord_events:002` | 5,6 | [Route110_TrickHouseEntrance_EventScript_TrickMasterHiding](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L662) — Coordinate trigger at (5,6); VAR_TRICK_HOUSE_BEING_WATCHED_STATE == 0 invokes Route110_TrickHouseEntrance_EventScript_TrickMasterHiding. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHouseEntrance:coord_events:003` | 6,6 | [Route110_TrickHouseEntrance_EventScript_TrickMasterHiding](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L662) — Coordinate trigger at (6,6); VAR_TRICK_HOUSE_BEING_WATCHED_STATE == 0 invokes Route110_TrickHouseEntrance_EventScript_TrickMasterHiding. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHouseEntrance:coord_events:004` | 7,7 | [Route110_TrickHouseEntrance_EventScript_TrickMasterHiding](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L662) — Coordinate trigger at (7,7); VAR_TRICK_HOUSE_BEING_WATCHED_STATE == 0 invokes Route110_TrickHouseEntrance_EventScript_TrickMasterHiding. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_TrickHouseEntrance:bg_events:001` | 5,1 | [Route110_TrickHouseEntrance_EventScript_Door](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L443) — Route110_TrickHouseEntrance_EventScript_Door at (5,1); It's a scroll. / There is a big hole behind the scroll! //  Want to go in? | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHouseEntrance:warp_events:001` | 5,7 | Warp from (5,7, elevation 3) to MAP_ROUTE110 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseEntrance:warp_events:002` | 6,7 | Warp from (6,7, elevation 3) to MAP_ROUTE110 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseEntrance:warp_events:003` | 5,2 | Warp from (5,2, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_PUZZLE1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseEntrance:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHouseEntrance_OnTransition](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L16) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHouseEntrance_OnTransition. | **REPAIR** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · W-TRICK-REWARDS |
| `Route110_TrickHouseEntrance:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [Route110_TrickHouseEntrance_OnFrame](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L195) — MAP_SCRIPT_ON_FRAME_TABLE calls Route110_TrickHouseEntrance_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route110_TrickHouseEntrance:map_scripts:003` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [Route110_TrickHouseEntrance_OnWarp](../../baseline/source/data/maps/Route110_TrickHouseEntrance/scripts.inc#L115) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls Route110_TrickHouseEntrance_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
