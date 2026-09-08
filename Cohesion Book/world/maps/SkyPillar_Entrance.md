# SkyPillar_Entrance

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SkyPillar_Entrance/map.json) · [Scripts](../../baseline/source/data/maps/SkyPillar_Entrance/scripts.inc)

## Current map contract

`MAP_SKY_PILLAR_ENTRANCE` · `LAYOUT_SKY_PILLAR_ENTRANCE` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SkyPillar_Entrance:warp_events:001` | 6,16 | Warp from (6,16, elevation 3) to MAP_ROUTE131 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SkyPillar_Entrance:warp_events:002` | 14,4 | Warp from (14,4, elevation 3) to MAP_SKY_PILLAR_OUTSIDE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SkyPillar_Entrance:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SkyPillar_Entrance_OnTransition](../../baseline/source/data/maps/SkyPillar_Entrance/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls SkyPillar_Entrance_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
