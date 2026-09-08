# BattleFrontier_BattlePikeRoomNormal

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomNormal/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomNormal/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_NORMAL` · `LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_NORMAL` · `WEATHER_NONE` · `MUS_B_PIKE`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattlePikeRoomNormal:object_events:001` | 4,4 | [BattleFrontier_BattlePikeRoomNormal_EventScript_NPC](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomNormal/scripts.inc#L519) — BattleFrontier_BattlePikeRoomNormal_EventScript_NPC at (4,4); Ah, you're a lucky one. /  I'm in somewhat-good spirits now. //  I will restore one of your POKéMON /  to full health. / The best of luck to you. /  Farewell. | **INERT/EXCLUDED** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_BattlePikeRoomNormal:object_events:002` | 3,4 | [BattleFrontier_BattlePikeRoomNormal_EventScript_StatusMon](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomNormal/scripts.inc#L571) — BattleFrontier_BattlePikeRoomNormal_EventScript_StatusMon at (3,4); … … … … … … /  … … … … … … | **INERT/EXCLUDED** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:001` | 4,3 | [BattleFrontier_BattlePikeRoomNormal_EventScript_Exit](../../baseline/source/data/scripts/battle_pike.inc#L153) — Coordinate trigger at (4,3); VAR_TEMP_1 == 0 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_Exit. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:002` | 3,6 | [BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom](../../baseline/source/data/scripts/battle_pike.inc#L139) — Coordinate trigger at (3,6); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:003` | 3,7 | [BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack](../../baseline/source/data/scripts/battle_pike.inc#L144) — Coordinate trigger at (3,7); VAR_TEMP_3 == 1 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:004` | 4,6 | [BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom](../../baseline/source/data/scripts/battle_pike.inc#L139) — Coordinate trigger at (4,6); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:005` | 5,6 | [BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom](../../baseline/source/data/scripts/battle_pike.inc#L139) — Coordinate trigger at (5,6); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_SetEnteredRoom. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:006` | 4,7 | [BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack](../../baseline/source/data/scripts/battle_pike.inc#L144) — Coordinate trigger at (4,7); VAR_TEMP_3 == 1 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:coord_events:007` | 5,7 | [BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack](../../baseline/source/data/scripts/battle_pike.inc#L144) — Coordinate trigger at (5,7); VAR_TEMP_3 == 1 invokes BattleFrontier_BattlePikeRoomNormal_EventScript_NoTurningBack. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomNormal:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [BattleFrontier_BattlePikeRoom_OnResume](../../baseline/source/data/scripts/battle_pike.inc#L228) — MAP_SCRIPT_ON_RESUME calls BattleFrontier_BattlePikeRoom_OnResume. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePikeRoomNormal:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [BattleFrontier_BattlePikeRoom_OnTransition](../../baseline/source/data/scripts/battle_pike.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls BattleFrontier_BattlePikeRoom_OnTransition. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePikeRoomNormal:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattlePikeRoomNormal_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomNormal/scripts.inc#L8) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattlePikeRoomNormal_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePikeRoomNormal:map_scripts:004` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattlePikeRoom_OnWarp](../../baseline/source/data/scripts/battle_pike.inc#L44) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattlePikeRoom_OnWarp. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
