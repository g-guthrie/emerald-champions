# BattleFrontier_BattleTowerCorridor

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleTowerCorridor/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleTowerCorridor/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_TOWER_CORRIDOR` · `LAYOUT_BATTLE_FRONTIER_BATTLE_TOWER_CORRIDOR` · `WEATHER_NONE` · `MUS_B_TOWER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleTowerCorridor:object_events:001` | 9,2 | Passive/staged OBJ_EVENT_GFX_TEALA at (9,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattleTowerCorridor:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [BattleFrontier_BattleTowerCorridor_OnLoad](../../baseline/source/data/maps/BattleFrontier_BattleTowerCorridor/scripts.inc#L6) — MAP_SCRIPT_ON_LOAD calls BattleFrontier_BattleTowerCorridor_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleTowerCorridor:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleTowerCorridor_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleTowerCorridor/scripts.inc#L17) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleTowerCorridor_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
