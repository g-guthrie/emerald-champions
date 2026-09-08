# SkyPillar_Top

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SkyPillar_Top/map.json) · [Scripts](../../baseline/source/data/maps/SkyPillar_Top/scripts.inc)

## Current map contract

`MAP_SKY_PILLAR_TOP` · `LAYOUT_SKY_PILLAR_TOP` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SkyPillar_Top:object_events:001` | 14,7 | Passive/staged OBJ_EVENT_GFX_RAYQUAZA at (14,7); visibility flag FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SkyPillar_Top:object_events:002` | 14,6 | [SkyPillar_Top_EventScript_Rayquaza](../../baseline/source/data/maps/SkyPillar_Top/scripts.inc#L43) — SkyPillar_Top_EventScript_Rayquaza at (14,6); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `SkyPillar_Top:coord_events:001` | 14,9 | [SkyPillar_Top_EventScript_AwakenRayquaza](../../baseline/source/data/maps/SkyPillar_Top/scripts.inc#L87) — Coordinate trigger at (14,9); VAR_SKY_PILLAR_RAYQUAZA_CRY_DONE == 0 invokes SkyPillar_Top_EventScript_AwakenRayquaza. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `SkyPillar_Top:warp_events:001` | 16,14 | Warp from (16,14, elevation 3) to MAP_SKY_PILLAR_5F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SkyPillar_Top:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [SkyPillar_Top_OnResume](../../baseline/source/data/maps/SkyPillar_Top/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls SkyPillar_Top_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SkyPillar_Top:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [SkyPillar_Top_OnTransition](../../baseline/source/data/maps/SkyPillar_Top/scripts.inc#L17) — MAP_SCRIPT_ON_TRANSITION calls SkyPillar_Top_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SkyPillar_Top:map_scripts:003` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [SkyPillar_Top_OnWarp](../../baseline/source/data/maps/SkyPillar_Top/scripts.inc#L35) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls SkyPillar_Top_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
