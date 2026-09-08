# SkyPillar_Outside

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SkyPillar_Outside/map.json) · [Scripts](../../baseline/source/data/maps/SkyPillar_Outside/scripts.inc)

## Current map contract

`MAP_SKY_PILLAR_OUTSIDE` · `LAYOUT_SKY_PILLAR_OUTSIDE` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SkyPillar_Outside:object_events:001` | 13,7 | [SkyPillar_Outside_EventScript_Wallace](../../baseline/source/data/maps/SkyPillar_Outside/scripts.inc#L132) — SkyPillar_Outside_EventScript_Wallace at (13,7); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SkyPillar_Outside:warp_events:001` | 17,13 | Warp from (17,13, elevation 3) to MAP_SKY_PILLAR_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SkyPillar_Outside:warp_events:002` | 14,5 | Warp from (14,5, elevation 0) to MAP_SKY_PILLAR_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SkyPillar_Outside:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SkyPillar_Outside_OnTransition](../../baseline/source/data/maps/SkyPillar_Outside/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls SkyPillar_Outside_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SkyPillar_Outside:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [SkyPillar_Outside_OnLoad](../../baseline/source/data/maps/SkyPillar_Outside/scripts.inc#L20) — MAP_SCRIPT_ON_LOAD calls SkyPillar_Outside_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SkyPillar_Outside:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [SkyPillar_Outside_OnFrame](../../baseline/source/data/maps/SkyPillar_Outside/scripts.inc#L29) — MAP_SCRIPT_ON_FRAME_TABLE calls SkyPillar_Outside_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
