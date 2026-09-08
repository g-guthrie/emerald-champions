# BattleFrontier_BattleTowerMultiBattleRoom

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleTowerMultiBattleRoom/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleTowerMultiBattleRoom/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_TOWER_MULTI_BATTLE_ROOM` · `LAYOUT_BATTLE_FRONTIER_BATTLE_TOWER_BATTLE_ROOM` · `WEATHER_NONE` · `MUS_B_TOWER`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleTowerMultiBattleRoom:object_events:001` | 5,1 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (5,1); visibility flag FLAG_HIDE_BATTLE_TOWER_OPPONENT; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerMultiBattleRoom:object_events:002` | 1,4 | Passive/staged OBJ_EVENT_GFX_TEALA at (1,4); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerMultiBattleRoom:object_events:003` | 1,5 | Passive/staged OBJ_EVENT_GFX_TEALA at (1,5); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerMultiBattleRoom:object_events:004` | 4,1 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (4,1); visibility flag FLAG_HIDE_BATTLE_TOWER_OPPONENT; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerMultiBattleRoom:object_events:005` | 4,8 | Passive/staged OBJ_EVENT_GFX_VAR_F at (4,8); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerMultiBattleRoom:object_events:006` | 5,8 | Passive/staged OBJ_EVENT_GFX_VAR_E at (5,8); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerMultiBattleRoom:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [BattleFrontier_BattleTowerMultiBattleRoom_OnTransition](../../baseline/source/data/maps/BattleFrontier_BattleTowerMultiBattleRoom/scripts.inc#L13) — MAP_SCRIPT_ON_TRANSITION calls BattleFrontier_BattleTowerMultiBattleRoom_OnTransition. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleTowerMultiBattleRoom:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattleTowerMultiBattleRoom_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattleTowerMultiBattleRoom/scripts.inc#L33) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattleTowerMultiBattleRoom_OnWarp. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleTowerMultiBattleRoom:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleTowerMultiBattleRoom_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleTowerMultiBattleRoom/scripts.inc#L41) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleTowerMultiBattleRoom_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
