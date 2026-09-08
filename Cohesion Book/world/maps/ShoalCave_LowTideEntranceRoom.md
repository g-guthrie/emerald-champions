# ShoalCave_LowTideEntranceRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/ShoalCave_LowTideEntranceRoom/map.json) · [Scripts](../../baseline/source/data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc)

## Current map contract

`MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM` · `LAYOUT_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ShoalCave_LowTideEntranceRoom:object_events:001` | 30,3 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BIG_PEARL; root Common_EventScript_FindItem; flag FLAG_ITEM_SHOAL_CAVE_ENTRANCE_BIG_PEARL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ShoalCave_LowTideEntranceRoom:object_events:002` | 18,15 | [ShoalCave_LowTideEntranceRoom_EventScript_ShellBellExpert](../../baseline/source/data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc#L18) — ShoalCave_LowTideEntranceRoom_EventScript_ShellBellExpert at (18,15); Level DEWOTT anywhere in SHOAL CAVE at /  Lv. 36 or higher for Hisuian SAMUROTT. //  Level BERGMITE in the low-tide ice room /  at Lv. 37 or higher for Hisuian AVALUGG. //  Elsewhere they take their usual forms. /  Galarian MR. MIME also lives in that /  ice room. It becomes MR. RIME at Lv. 42. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `ShoalCave_LowTideEntranceRoom:warp_events:001` | 20,30 | Warp from (20,30, elevation 3) to MAP_ROUTE125 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideEntranceRoom:warp_events:002` | 19,5 | Warp from (19,5, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideEntranceRoom:warp_events:003` | 6,2 | Warp from (6,2, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideEntranceRoom:warp_events:004` | 27,2 | Warp from (27,2, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideEntranceRoom:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [ShoalCave_LowTideEntranceRoom_OnTransition](../../baseline/source/data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls ShoalCave_LowTideEntranceRoom_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
