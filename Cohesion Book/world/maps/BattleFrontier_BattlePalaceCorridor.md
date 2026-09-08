# BattleFrontier_BattlePalaceCorridor

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattlePalaceCorridor/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattlePalaceCorridor/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_PALACE_CORRIDOR` · `LAYOUT_BATTLE_FRONTIER_BATTLE_PALACE_CORRIDOR` · `WEATHER_NONE` · `MUS_B_PALACE`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattlePalaceCorridor:object_events:001` | 8,12 | Passive/staged OBJ_EVENT_GFX_EXPERT_M at (8,12); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:object_events:002` | 3,5 | Passive/staged OBJ_EVENT_GFX_AZURILL at (3,5); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:object_events:003` | 12,6 | Passive/staged OBJ_EVENT_GFX_KIRLIA at (12,6); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:object_events:004` | 15,5 | Passive/staged OBJ_EVENT_GFX_PIKACHU at (15,5); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:object_events:005` | 4,9 | Passive/staged OBJ_EVENT_GFX_ZIGZAGOON_2 at (4,9); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:object_events:006` | 13,9 | Passive/staged OBJ_EVENT_GFX_AZUMARILL at (13,9); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:object_events:007` | 3,10 | Passive/staged OBJ_EVENT_GFX_WINGULL at (3,10); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePalaceCorridor:warp_events:001` | 8,13 | Warp from (8,13, elevation 3) to MAP_BATTLE_FRONTIER_BATTLE_PALACE_LOBBY warp 2. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePalaceCorridor:warp_events:002` | 9,13 | Warp from (9,13, elevation 3) to MAP_BATTLE_FRONTIER_BATTLE_PALACE_LOBBY warp 2. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePalaceCorridor:warp_events:003` | 6,3 | Warp from (6,3, elevation 3) to MAP_BATTLE_FRONTIER_BATTLE_PALACE_BATTLE_ROOM warp 0. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePalaceCorridor:warp_events:004` | 10,3 | Warp from (10,3, elevation 3) to MAP_BATTLE_FRONTIER_BATTLE_PALACE_BATTLE_ROOM warp 0. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePalaceCorridor:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattlePalaceCorridor_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattlePalaceCorridor/scripts.inc#L5) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattlePalaceCorridor_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
