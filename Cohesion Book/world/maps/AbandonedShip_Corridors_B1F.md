# AbandonedShip_Corridors_B1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_CORRIDORS_B1F` · `LAYOUT_ABANDONED_SHIP_CORRIDORS_B1F` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_Corridors_B1F:object_events:001` | 2,8 | [AbandonedShip_Corridors_B1F_EventScript_TuberM](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/scripts.inc#L23) — AbandonedShip_Corridors_B1F_EventScript_TuberM at (2,8); Yay! /  It's a ship! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `AbandonedShip_Corridors_B1F:object_events:002` | 9,6 | [AbandonedShip_Corridors_B1F_EventScript_Duncan](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/scripts.inc#L51) — AbandonedShip_Corridors_B1F_EventScript_Duncan at (9,6); When we go out to sea, we SAILORS /  always bring our POKéMON. /  How about a quick battle? / Whoops, I'm sunk! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Corridors_B1F:bg_events:001` | 11,4 | [AbandonedShip_Corridors_B1F_EventScript_StorageRoomDoor](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/scripts.inc#L27) — AbandonedShip_Corridors_B1F_EventScript_StorageRoomDoor at (11,4); The door is locked. //  “STORAGE” is painted on the door. / {PLAYER} inserted and turned the /  STORAGE KEY. //  The inserted KEY stuck fast, /  but the door opened. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AbandonedShip_Corridors_B1F:warp_events:001` | 6,4 | Warp from (6,4, elevation 3) to MAP_ABANDONED_SHIP_ROOMS2_B1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:002` | 3,4 | Warp from (3,4, elevation 3) to MAP_ABANDONED_SHIP_ROOMS2_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:003` | 5,7 | Warp from (5,7, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:004` | 8,7 | Warp from (8,7, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:005` | 11,7 | Warp from (11,7, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_B1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:006` | 11,4 | Warp from (11,4, elevation 3) to MAP_ABANDONED_SHIP_ROOM_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:007` | 0,2 | Warp from (0,2, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:warp_events:008` | 8,2 | Warp from (8,2, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_B1F:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [AbandonedShip_Corridors_B1F_OnResume](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls AbandonedShip_Corridors_B1F_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `AbandonedShip_Corridors_B1F:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [AbandonedShip_Corridors_B1F_OnLoad](../../baseline/source/data/maps/AbandonedShip_Corridors_B1F/scripts.inc#L10) — MAP_SCRIPT_ON_LOAD calls AbandonedShip_Corridors_B1F_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
