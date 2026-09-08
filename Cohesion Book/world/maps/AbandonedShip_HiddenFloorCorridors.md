# AbandonedShip_HiddenFloorCorridors

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS` · `LAYOUT_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_HiddenFloorCorridors:bg_events:001` | 3,8 | [AbandonedShip_HiddenFloorCorridors_EventScript_Room1Door](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc#L53) — AbandonedShip_HiddenFloorCorridors_EventScript_Room1Door at (3,8); The door is locked. //  “RM. 1” is painted on the door. / {PLAYER} inserted and turned the /  KEY. //  The inserted KEY stuck fast, /  but the door opened. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorCorridors:bg_events:002` | 6,8 | [AbandonedShip_HiddenFloorCorridors_EventScript_Room2Door](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc#L67) — AbandonedShip_HiddenFloorCorridors_EventScript_Room2Door at (6,8); The door is locked. //  “RM. 2” is painted on the door. / {PLAYER} inserted and turned the /  KEY. //  The inserted KEY stuck fast, /  but the door opened. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorCorridors:bg_events:003` | 3,4 | [AbandonedShip_HiddenFloorCorridors_EventScript_Room4Door](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc#L81) — AbandonedShip_HiddenFloorCorridors_EventScript_Room4Door at (3,4); The door is locked. //  “RM. 4” is painted on the door. / {PLAYER} inserted and turned the /  KEY. //  The inserted KEY stuck fast, /  but the door opened. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorCorridors:bg_events:004` | 9,4 | [AbandonedShip_HiddenFloorCorridors_EventScript_Room6Door](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc#L95) — AbandonedShip_HiddenFloorCorridors_EventScript_Room6Door at (9,4); The door is locked. //  “RM. 6” is painted on the door. / {PLAYER} inserted and turned the /  KEY. //  The inserted KEY stuck fast, /  but the door opened. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorCorridors:warp_events:001` | 3,8 | Warp from (3,8, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorCorridors:warp_events:002` | 6,8 | Warp from (6,8, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorCorridors:warp_events:003` | 9,8 | Warp from (9,8, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorCorridors:warp_events:004` | 3,3 | Warp from (3,3, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorCorridors:warp_events:005` | 6,3 | Warp from (6,3, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorCorridors:warp_events:006` | 9,3 | Warp from (9,3, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorCorridors:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [AbandonedShip_HiddenFloorCorridors_OnResume](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls AbandonedShip_HiddenFloorCorridors_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `AbandonedShip_HiddenFloorCorridors:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [AbandonedShip_HiddenFloorCorridors_OnLoad](../../baseline/source/data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc#L10) — MAP_SCRIPT_ON_LOAD calls AbandonedShip_HiddenFloorCorridors_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
