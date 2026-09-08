# Route110_SeasideCyclingRoadSouthEntrance

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/Route110_SeasideCyclingRoadSouthEntrance/map.json) · [Scripts](../../baseline/source/data/maps/Route110_SeasideCyclingRoadSouthEntrance/scripts.inc)

## Current map contract

`MAP_ROUTE110_SEASIDE_CYCLING_ROAD_SOUTH_ENTRANCE` · `LAYOUT_ROUTE110_SEASIDE_CYCLING_ROAD_ENTRANCE` · `WEATHER_NONE` · `MUS_SLATEPORT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_SeasideCyclingRoadSouthEntrance:object_events:001` | 7,2 | [Route110_SeasideCyclingRoadSouthEntrance_EventScript_Clerk](../../baseline/source/data/maps/Route110_SeasideCyclingRoadSouthEntrance/scripts.inc#L14) — Route110_SeasideCyclingRoadSouthEntrance_EventScript_Clerk at (7,2); On CYCLING ROAD, you can go all out /  and cycle as fast as you'd like. //  It feels great to go that fast, but try /  not to crash into anyone! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route110_SeasideCyclingRoadSouthEntrance:coord_events:001` | 7,4 | [Route110_SeasideCyclingRoadSouthEntrance_EventScript_BikeCheck](../../baseline/source/data/maps/Route110_SeasideCyclingRoadSouthEntrance/scripts.inc#L21) — Coordinate trigger at (7,4); VAR_TEMP_1 == 0 invokes Route110_SeasideCyclingRoadSouthEntrance_EventScript_BikeCheck. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_SeasideCyclingRoadSouthEntrance:coord_events:002` | 5,4 | [Route110_SeasideCyclingRoadSouthEntrance_EventScript_ClearCyclingRoad](../../baseline/source/data/maps/Route110_SeasideCyclingRoadSouthEntrance/scripts.inc#L47) — Coordinate trigger at (5,4); VAR_TEMP_1 == 1 invokes Route110_SeasideCyclingRoadSouthEntrance_EventScript_ClearCyclingRoad. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route110_SeasideCyclingRoadSouthEntrance:warp_events:001` | 1,5 | Warp from (1,5, elevation 0) to MAP_ROUTE110 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_SeasideCyclingRoadSouthEntrance:warp_events:002` | 2,5 | Warp from (2,5, elevation 0) to MAP_ROUTE110 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_SeasideCyclingRoadSouthEntrance:warp_events:003` | 12,5 | Warp from (12,5, elevation 0) to MAP_ROUTE110 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_SeasideCyclingRoadSouthEntrance:warp_events:004` | 13,5 | Warp from (13,5, elevation 0) to MAP_ROUTE110 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_SeasideCyclingRoadSouthEntrance:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route110_SeasideCyclingRoadSouthEntrance_OnTransition](../../baseline/source/data/maps/Route110_SeasideCyclingRoadSouthEntrance/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route110_SeasideCyclingRoadSouthEntrance_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
