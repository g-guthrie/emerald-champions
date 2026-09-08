# Route116_TunnelersRestHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route116_TunnelersRestHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route116_TunnelersRestHouse/scripts.inc)

## Current map contract

`MAP_ROUTE116_TUNNELERS_REST_HOUSE` · `LAYOUT_ROUTE116_TUNNELERS_REST_HOUSE` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route116_TunnelersRestHouse:object_events:001` | 6,5 | [Route116_TunnelersRestHouse_EventScript_Tunneler1](../../baseline/source/data/maps/Route116_TunnelersRestHouse/scripts.inc#L9) — Route116_TunnelersRestHouse_EventScript_Tunneler1 at (6,5); That RUSTURF TUNNEL there… //  At first, we had a huge work crew boring /  through rock with the latest machinery. /  But, we had to stop. //  It turns out that we would have had /  a negative effect on wild POKéMON in /  the area. //  So, we've got nothing to do but loll /  around here doing nothing. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route116_TunnelersRestHouse:object_events:002` | 3,6 | [Route116_TunnelersRestHouse_EventScript_Tunneler3](../../baseline/source/data/maps/Route116_TunnelersRestHouse/scripts.inc#L17) — Route116_TunnelersRestHouse_EventScript_Tunneler3 at (3,6); To get to VERDANTURF without using /  this TUNNEL, you'd have to cross the /  sea to DEWFORD, sail on to SLATEPORT, /  then travel through MAUVILLE. / Did you hear? The TUNNEL to VERDANTURF /  has gone through! //  Sometimes, if you hope strongly enough, /  dreams do come true. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route116_TunnelersRestHouse:object_events:003` | 7,2 | [Route116_TunnelersRestHouse_EventScript_Tunneler2](../../baseline/source/data/maps/Route116_TunnelersRestHouse/scripts.inc#L13) — Route116_TunnelersRestHouse_EventScript_Tunneler2 at (7,2); There's a man digging his way to /  VERDANTURF all by his lonesome. /  He's desperate to get through. //  He says that if he digs little by little /  without using machines, he won't /  disturb POKéMON, and he'll avoid /  harming the natural environment. //  I wonder if he made it through yet. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route116_TunnelersRestHouse:warp_events:001` | 4,8 | Warp from (4,8, elevation 0) to MAP_ROUTE116 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route116_TunnelersRestHouse:warp_events:002` | 5,8 | Warp from (5,8, elevation 0) to MAP_ROUTE116 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route116_TunnelersRestHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route116_TunnelersRestHouse_OnTransition](../../baseline/source/data/maps/Route116_TunnelersRestHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route116_TunnelersRestHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
