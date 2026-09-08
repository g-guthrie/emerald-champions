# BattleFrontier_BattleTowerBattleRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleTowerBattleRoom/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_TOWER_BATTLE_ROOM` · `LAYOUT_BATTLE_FRONTIER_BATTLE_TOWER_BATTLE_ROOM` · `WEATHER_NONE` · `MUS_B_TOWER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleTowerBattleRoom:object_events:001` | 5,1 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (5,1); visibility flag FLAG_HIDE_BATTLE_TOWER_OPPONENT; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerBattleRoom:object_events:002` | 1,7 | Passive/staged OBJ_EVENT_GFX_TEALA at (1,7); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerBattleRoom:object_events:003` | 4,8 | Passive/staged OBJ_EVENT_GFX_TEALA at (4,8); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerBattleRoom:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleTowerBattleRoom:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleTowerBattleRoom:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleTowerBattleRoom_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleTowerBattleRoom_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleTowerBattleRoom:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattleTowerBattleRoom_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattleTowerBattleRoom_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
