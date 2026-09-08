# BattleFrontier_BattlePikeRoomWildMons

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomWildMons/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomWildMons/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS` · `LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_ROOM_WILD_MONS` · `WEATHER_NONE` · `MUS_B_PIKE`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:001` | 4,3 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_Exit](../../baseline/source/data/scripts/battle_pike.inc#L191) — Coordinate trigger at (4,3); VAR_TEMP_1 == 0 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_Exit. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:002` | 3,18 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom](../../baseline/source/data/scripts/battle_pike.inc#L208) — Coordinate trigger at (3,18); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:003` | 4,18 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom](../../baseline/source/data/scripts/battle_pike.inc#L208) — Coordinate trigger at (4,18); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:004` | 5,18 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom](../../baseline/source/data/scripts/battle_pike.inc#L208) — Coordinate trigger at (5,18); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_SetEnteredRoom. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:005` | 3,19 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack](../../baseline/source/data/scripts/battle_pike.inc#L213) — Coordinate trigger at (3,19); VAR_TEMP_3 == 1 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:006` | 4,19 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack](../../baseline/source/data/scripts/battle_pike.inc#L213) — Coordinate trigger at (4,19); VAR_TEMP_3 == 1 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:coord_events:007` | 5,19 | [BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack](../../baseline/source/data/scripts/battle_pike.inc#L213) — Coordinate trigger at (5,19); VAR_TEMP_3 == 1 invokes BattleFrontier_BattlePikeRoomWildMons_EventScript_NoTurningBack. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePikeRoomWildMons:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [BattleFrontier_BattlePikeRoomWildMons_OnResume](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomWildMons/scripts.inc#L32) — MAP_SCRIPT_ON_RESUME calls BattleFrontier_BattlePikeRoomWildMons_OnResume. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePikeRoomWildMons:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattlePikeRoomWildMons_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomWildMons/scripts.inc#L7) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattlePikeRoomWildMons_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePikeRoomWildMons:map_scripts:003` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattlePikeRoomWildMons_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattlePikeRoomWildMons/scripts.inc#L23) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattlePikeRoomWildMons_OnWarp. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
