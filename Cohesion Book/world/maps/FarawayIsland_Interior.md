# FarawayIsland_Interior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/FarawayIsland_Interior/map.json) · [Scripts](../../baseline/source/data/maps/FarawayIsland_Interior/scripts.inc)

## Current map contract

`MAP_FARAWAY_ISLAND_INTERIOR` · `LAYOUT_FARAWAY_ISLAND_INTERIOR` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FarawayIsland_Interior:object_events:001` | 13,17 | [FarawayIsland_Interior_EventScript_Mew](../../baseline/source/data/maps/FarawayIsland_Interior/scripts.inc#L113) — FarawayIsland_Interior_EventScript_Mew at (13,17); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `FarawayIsland_Interior:warp_events:001` | 12,19 | Warp from (12,19, elevation 0) to MAP_FARAWAY_ISLAND_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FarawayIsland_Interior:warp_events:002` | 13,19 | Warp from (13,19, elevation 0) to MAP_FARAWAY_ISLAND_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FarawayIsland_Interior:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [FarawayIsland_Interior_OnResume](../../baseline/source/data/maps/FarawayIsland_Interior/scripts.inc#L25) — MAP_SCRIPT_ON_RESUME calls FarawayIsland_Interior_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `FarawayIsland_Interior:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [FarawayIsland_Interior_OnTransition](../../baseline/source/data/maps/FarawayIsland_Interior/scripts.inc#L35) — MAP_SCRIPT_ON_TRANSITION calls FarawayIsland_Interior_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `FarawayIsland_Interior:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [FarawayIsland_Interior_OnFrame](../../baseline/source/data/maps/FarawayIsland_Interior/scripts.inc#L47) — MAP_SCRIPT_ON_FRAME_TABLE calls FarawayIsland_Interior_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `FarawayIsland_Interior:map_scripts:004` | MAP_SCRIPT_ON_RETURN_TO_FIELD | [FarawayIsland_Interior_OnReturnToField](../../baseline/source/data/maps/FarawayIsland_Interior/scripts.inc#L8) — MAP_SCRIPT_ON_RETURN_TO_FIELD calls FarawayIsland_Interior_OnReturnToField. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
