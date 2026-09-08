# NewMauville_Entrance

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../03-mauville.md) · [Map source](../../baseline/source/data/maps/NewMauville_Entrance/map.json) · [Scripts](../../baseline/source/data/maps/NewMauville_Entrance/scripts.inc)

## Current map contract

`MAP_NEW_MAUVILLE_ENTRANCE` · `LAYOUT_NEW_MAUVILLE_ENTRANCE` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `NewMauville_Entrance:coord_events:001` | 4,2 | [NewMauville_Entrance_EventScript_Door](../../baseline/source/data/maps/NewMauville_Entrance/scripts.inc#L23) — Coordinate trigger at (4,2); VAR_NEW_MAUVILLE_STATE == 0 invokes NewMauville_Entrance_EventScript_Door. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `NewMauville_Entrance:warp_events:001` | 4,6 | Warp from (4,6, elevation 3) to MAP_ROUTE110 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NewMauville_Entrance:warp_events:002` | 4,1 | Warp from (4,1, elevation 3) to MAP_NEW_MAUVILLE_INSIDE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `NewMauville_Entrance:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [NewMauville_Entrance_OnLoad](../../baseline/source/data/maps/NewMauville_Entrance/scripts.inc#L6) — MAP_SCRIPT_ON_LOAD calls NewMauville_Entrance_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `NewMauville_Entrance:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [NewMauville_Entrance_OnTransition](../../baseline/source/data/maps/NewMauville_Entrance/scripts.inc#L19) — MAP_SCRIPT_ON_TRANSITION calls NewMauville_Entrance_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
