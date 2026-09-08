# IslandCave

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/IslandCave/map.json) · [Scripts](../../baseline/source/data/maps/IslandCave/scripts.inc)

## Current map contract

`MAP_ISLAND_CAVE` · `LAYOUT_ISLAND_CAVE` · `WEATHER_NONE` · `MUS_SEALED_CHAMBER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `IslandCave:object_events:001` | 8,7 | [IslandCave_EventScript_Regice](../../baseline/source/data/maps/IslandCave/scripts.inc#L90) — IslandCave_EventScript_Regice at (8,7); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `IslandCave:bg_events:001` | 8,20 | [IslandCave_EventScript_CaveEntranceMiddle](../../baseline/source/data/maps/IslandCave/scripts.inc#L52) — IslandCave_EventScript_CaveEntranceMiddle at (8,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `IslandCave:bg_events:002` | 7,20 | [IslandCave_EventScript_CaveEntranceSide](../../baseline/source/data/maps/IslandCave/scripts.inc#L67) — IslandCave_EventScript_CaveEntranceSide at (7,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `IslandCave:bg_events:003` | 9,20 | [IslandCave_EventScript_CaveEntranceSide](../../baseline/source/data/maps/IslandCave/scripts.inc#L67) — IslandCave_EventScript_CaveEntranceSide at (9,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `IslandCave:warp_events:001` | 8,29 | Warp from (8,29, elevation 3) to MAP_ROUTE105 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `IslandCave:warp_events:002` | 8,20 | Warp from (8,20, elevation 0) to MAP_ISLAND_CAVE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `IslandCave:warp_events:003` | 8,11 | Warp from (8,11, elevation 3) to MAP_ISLAND_CAVE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `IslandCave:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [IslandCave_OnResume](../../baseline/source/data/maps/IslandCave/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls IslandCave_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `IslandCave:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [IslandCave_OnLoad](../../baseline/source/data/maps/IslandCave/scripts.inc#L17) — MAP_SCRIPT_ON_LOAD calls IslandCave_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `IslandCave:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [IslandCave_OnTransition](../../baseline/source/data/maps/IslandCave/scripts.inc#L30) — MAP_SCRIPT_ON_TRANSITION calls IslandCave_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
