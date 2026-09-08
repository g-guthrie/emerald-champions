# MtPyre_Exterior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/MtPyre_Exterior/map.json) · [Scripts](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc)

## Current map contract

`MAP_MT_PYRE_EXTERIOR` · `LAYOUT_MT_PYRE_EXTERIOR` · `WEATHER_NONE` · `MUS_MT_PYRE_EXTERIOR`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MtPyre_Exterior:object_events:001` | 27,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_POTION; root Common_EventScript_FindItem; flag FLAG_ITEM_MT_PYRE_EXTERIOR_MAX_POTION. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MtPyre_Exterior:object_events:002` | 19,40 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DUSK_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_MT_PYRE_EXTERIOR_DUSK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `MtPyre_Exterior:object_events:003` | 12,45 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (12,45); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `MtPyre_Exterior:coord_events:001` | 24,21 | [MtPyre_Exterior_EventScript_FogTrigger](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc#L18) — Coordinate trigger at (24,21); TRIGGER_RUN_IMMEDIATELY == 0 invokes MtPyre_Exterior_EventScript_FogTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MtPyre_Exterior:coord_events:002` | 25,21 | [MtPyre_Exterior_EventScript_FogTrigger](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc#L18) — Coordinate trigger at (25,21); TRIGGER_RUN_IMMEDIATELY == 0 invokes MtPyre_Exterior_EventScript_FogTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MtPyre_Exterior:coord_events:003` | 22,27 | [MtPyre_Exterior_EventScript_SunTrigger](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc#L23) — Coordinate trigger at (22,27); TRIGGER_RUN_IMMEDIATELY == 0 invokes MtPyre_Exterior_EventScript_SunTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MtPyre_Exterior:coord_events:004` | 23,28 | [MtPyre_Exterior_EventScript_SunTrigger](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc#L23) — Coordinate trigger at (23,28); TRIGGER_RUN_IMMEDIATELY == 0 invokes MtPyre_Exterior_EventScript_SunTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MtPyre_Exterior:coord_events:005` | 26,21 | [MtPyre_Exterior_EventScript_FogTrigger](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc#L18) — Coordinate trigger at (26,21); TRIGGER_RUN_IMMEDIATELY == 0 invokes MtPyre_Exterior_EventScript_FogTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MtPyre_Exterior:bg_events:001` | 9,8 | Hidden ITEM_ULTRA_BALL at (9,8); persistent flag FLAG_HIDDEN_ITEM_MT_PYRE_EXTERIOR_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `MtPyre_Exterior:bg_events:002` | 16,22 | Hidden ITEM_MAX_ETHER at (16,22); persistent flag FLAG_HIDDEN_ITEM_MT_PYRE_EXTERIOR_MAX_ETHER. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `MtPyre_Exterior:warp_events:001` | 10,42 | Warp from (10,42, elevation 3) to MAP_MT_PYRE_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_Exterior:warp_events:002` | 19,10 | Warp from (19,10, elevation 3) to MAP_MT_PYRE_SUMMIT warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_Exterior:warp_events:003` | 20,10 | Warp from (20,10, elevation 3) to MAP_MT_PYRE_SUMMIT warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MtPyre_Exterior:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [MtPyre_Exterior_OnTransition](../../baseline/source/data/maps/MtPyre_Exterior/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls MtPyre_Exterior_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
