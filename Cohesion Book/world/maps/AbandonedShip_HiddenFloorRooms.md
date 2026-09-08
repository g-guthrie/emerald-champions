# AbandonedShip_HiddenFloorRooms

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS` · `LAYOUT_ABANDONED_SHIP_HIDDEN_FLOOR_ROOMS` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_HiddenFloorRooms:object_events:001` | 41,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_LUXURY_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_6_LUXURY_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_HiddenFloorRooms:object_events:002` | 16,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SCANNER; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_2_SCANNER. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_HiddenFloorRooms:object_events:003` | 5,11 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_1_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_HiddenFloorRooms:object_events:004` | 31,11 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_WATER_STONE; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_HIDDEN_FLOOR_ROOM_3_WATER_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_HiddenFloorRooms:object_events:005` | 16,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_OLD_SEA_MAP; root Common_EventScript_FindItem; flag FLAG_RECEIVED_OLD_SEA_MAP. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_HiddenFloorRooms:bg_events:001` | 42,10 | Hidden ITEM_KEY_TO_ROOM_1 at (42,10); persistent flag FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_1_KEY. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `AbandonedShip_HiddenFloorRooms:bg_events:002` | 20,5 | Hidden ITEM_KEY_TO_ROOM_2 at (20,5); persistent flag FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_2_KEY. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `AbandonedShip_HiddenFloorRooms:bg_events:003` | 1,12 | Hidden ITEM_KEY_TO_ROOM_4 at (1,12); persistent flag FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_4_KEY. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `AbandonedShip_HiddenFloorRooms:bg_events:004` | 1,2 | Hidden ITEM_KEY_TO_ROOM_6 at (1,2); persistent flag FLAG_HIDDEN_ITEM_ABANDONED_SHIP_RM_6_KEY. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `AbandonedShip_HiddenFloorRooms:bg_events:005` | 8,5 | [AbandonedShip_HiddenFloorRooms_EventScript_Trash](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L108) — AbandonedShip_HiddenFloorRooms_EventScript_Trash at (8,5); It's bright and shiny! /  But it's just trash… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorRooms:bg_events:006` | 11,3 | [AbandonedShip_HiddenFloorRooms_EventScript_Trash](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L108) — AbandonedShip_HiddenFloorRooms_EventScript_Trash at (11,3); It's bright and shiny! /  But it's just trash… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorRooms:bg_events:007` | 10,10 | [AbandonedShip_HiddenFloorRooms_EventScript_Trash](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L108) — AbandonedShip_HiddenFloorRooms_EventScript_Trash at (10,10); It's bright and shiny! /  But it's just trash… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorRooms:bg_events:008` | 16,3 | [AbandonedShip_HiddenFloorRooms_EventScript_Trash](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L108) — AbandonedShip_HiddenFloorRooms_EventScript_Trash at (16,3); It's bright and shiny! /  But it's just trash… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorRooms:bg_events:009` | 25,2 | [AbandonedShip_HiddenFloorRooms_EventScript_Trash](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L108) — AbandonedShip_HiddenFloorRooms_EventScript_Trash at (25,2); It's bright and shiny! /  But it's just trash… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorRooms:bg_events:010` | 24,6 | [AbandonedShip_HiddenFloorRooms_EventScript_Trash](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L108) — AbandonedShip_HiddenFloorRooms_EventScript_Trash at (24,6); It's bright and shiny! /  But it's just trash… | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_HiddenFloorRooms:warp_events:001` | 6,14 | Warp from (6,14, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:002` | 7,14 | Warp from (7,14, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:003` | 21,14 | Warp from (21,14, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:004` | 22,14 | Warp from (22,14, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:005` | 36,14 | Warp from (36,14, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:006` | 37,14 | Warp from (37,14, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:007` | 6,1 | Warp from (6,1, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:008` | 21,1 | Warp from (21,1, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:warp_events:009` | 36,1 | Warp from (36,1, elevation 3) to MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_HiddenFloorRooms:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [AbandonedShip_HiddenFloorRooms_OnFrame](../../baseline/source/data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc#L5) — MAP_SCRIPT_ON_FRAME_TABLE calls AbandonedShip_HiddenFloorRooms_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
