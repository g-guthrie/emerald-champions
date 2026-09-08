# Route112_CableCarStation

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route112_CableCarStation/map.json) · [Scripts](../../baseline/source/data/maps/Route112_CableCarStation/scripts.inc)

## Current map contract

`MAP_ROUTE112_CABLE_CAR_STATION` · `LAYOUT_CABLE_CAR_STATION` · `WEATHER_NONE` · `MUS_ROUTE110`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route112_CableCarStation:object_events:001` | 6,6 | [Route112_CableCarStation_EventScript_Attendant](../../baseline/source/data/maps/Route112_CableCarStation/scripts.inc#L31) — Route112_CableCarStation_EventScript_Attendant at (6,6); The CABLE CAR is ready to go up. /  Would you like to be on it? / Please step this way. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route112_CableCarStation:object_events:002` | 6,3 | Passive/staged OBJ_EVENT_GFX_CABLE_CAR at (6,3); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route112_CableCarStation:warp_events:001` | 6,11 | Warp from (6,11, elevation 0) to MAP_ROUTE112 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112_CableCarStation:warp_events:002` | 7,11 | Warp from (7,11, elevation 0) to MAP_ROUTE112 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112_CableCarStation:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route112_CableCarStation_OnTransition](../../baseline/source/data/maps/Route112_CableCarStation/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls Route112_CableCarStation_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route112_CableCarStation:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [Route112_CableCarStation_OnFrame](../../baseline/source/data/maps/Route112_CableCarStation/scripts.inc#L16) — MAP_SCRIPT_ON_FRAME_TABLE calls Route112_CableCarStation_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
