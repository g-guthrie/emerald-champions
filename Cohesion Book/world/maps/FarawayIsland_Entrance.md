# FarawayIsland_Entrance

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/FarawayIsland_Entrance/map.json) · [Scripts](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc)

## Current map contract

`MAP_FARAWAY_ISLAND_ENTRANCE` · `LAYOUT_FARAWAY_ISLAND_ENTRANCE` · `WEATHER_NONE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FarawayIsland_Entrance:object_events:001` | 13,39 | [FarawayIsland_Entrance_EventScript_Sailor](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc#L19) — FarawayIsland_Entrance_EventScript_Sailor at (13,39); shared behavior TRAVEL | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `FarawayIsland_Entrance:object_events:002` | 13,41 | Passive/staged OBJ_EVENT_GFX_SS_TIDAL at (13,41); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `FarawayIsland_Entrance:coord_events:001` | 9,18 | [FarawayIsland_Entrance_EventScript_SetCloudsWeather](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc#L9) — Coordinate trigger at (9,18); TRIGGER_RUN_IMMEDIATELY == 0 invokes FarawayIsland_Entrance_EventScript_SetCloudsWeather. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FarawayIsland_Entrance:coord_events:002` | 10,20 | [FarawayIsland_Entrance_EventScript_ClearWeather](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc#L14) — Coordinate trigger at (10,20); TRIGGER_RUN_IMMEDIATELY == 0 invokes FarawayIsland_Entrance_EventScript_ClearWeather. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FarawayIsland_Entrance:coord_events:003` | 22,9 | [FarawayIsland_Entrance_EventScript_SetCloudsWeather](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc#L9) — Coordinate trigger at (22,9); TRIGGER_RUN_IMMEDIATELY == 0 invokes FarawayIsland_Entrance_EventScript_SetCloudsWeather. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `FarawayIsland_Entrance:bg_events:001` | 3,32 | [FarawayIsland_Entrance_EventScript_Sign](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc#L42) — FarawayIsland_Entrance_EventScript_Sign at (3,32); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `FarawayIsland_Entrance:warp_events:001` | 22,7 | Warp from (22,7, elevation 3) to MAP_FARAWAY_ISLAND_INTERIOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FarawayIsland_Entrance:warp_events:002` | 23,7 | Warp from (23,7, elevation 3) to MAP_FARAWAY_ISLAND_INTERIOR warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FarawayIsland_Entrance:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [FarawayIsland_Entrance_OnTransition](../../baseline/source/data/maps/FarawayIsland_Entrance/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls FarawayIsland_Entrance_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
