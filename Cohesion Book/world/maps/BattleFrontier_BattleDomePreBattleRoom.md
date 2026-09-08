# BattleFrontier_BattleDomePreBattleRoom

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleDomePreBattleRoom/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleDomePreBattleRoom/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_DOME_PRE_BATTLE_ROOM` · `LAYOUT_BATTLE_FRONTIER_BATTLE_DOME_PRE_BATTLE_ROOM` · `WEATHER_NONE` · `MUS_B_DOME`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleDomePreBattleRoom:object_events:001` | 5,2 | Passive/staged OBJ_EVENT_GFX_TEALA at (5,2); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleDomePreBattleRoom:warp_events:001` | 6,8 | Warp from (6,8, elevation 3) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 1. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleDomePreBattleRoom:warp_events:002` | 7,8 | Warp from (7,8, elevation 3) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 1. | **INERT/EXCLUDED** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleDomePreBattleRoom:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleDomePreBattleRoom_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleDomePreBattleRoom/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleDomePreBattleRoom_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleDomePreBattleRoom:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattleDomePreBattleRoom_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattleDomePreBattleRoom/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattleDomePreBattleRoom_OnWarp. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
