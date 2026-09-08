# VerdanturfTown_BattleTentBattleRoom

**INERT/EXCLUDED.** FAC-01 local exhibitions supersede this legacy tent staging room. Retain map IDs and any earned recovery state; no exhibition should warp here or consume the old challenge pipeline.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/VerdanturfTown_BattleTentBattleRoom/map.json) · [Scripts](../../baseline/source/data/maps/VerdanturfTown_BattleTentBattleRoom/scripts.inc)

## Current map contract

`MAP_VERDANTURF_TOWN_BATTLE_TENT_BATTLE_ROOM` · `LAYOUT_VERDANTURF_TOWN_BATTLE_TENT_BATTLE_ROOM` · `WEATHER_NONE` · `MUS_B_TOWER_RS`

FAC-01 local exhibitions supersede this legacy tent staging room. Retain map IDs and any earned recovery state; no exhibition should warp here or consume the old challenge pipeline.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VerdanturfTown_BattleTentBattleRoom:object_events:001` | 2,8 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (2,8); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · FAC-01 |
| `VerdanturfTown_BattleTentBattleRoom:object_events:002` | 11,1 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (11,1); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · FAC-01 |
| `VerdanturfTown_BattleTentBattleRoom:object_events:003` | 2,4 | Passive/staged OBJ_EVENT_GFX_EXPERT_M at (2,4); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · FAC-01 |
| `VerdanturfTown_BattleTentBattleRoom:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [VerdanturfTown_BattleTentBattleRoom_OnTransition](../../baseline/source/data/maps/VerdanturfTown_BattleTentBattleRoom/scripts.inc#L11) — MAP_SCRIPT_ON_TRANSITION calls VerdanturfTown_BattleTentBattleRoom_OnTransition. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |
| `VerdanturfTown_BattleTentBattleRoom:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [VerdanturfTown_BattleTentBattleRoom_OnFrame](../../baseline/source/data/maps/VerdanturfTown_BattleTentBattleRoom/scripts.inc#L31) — MAP_SCRIPT_ON_FRAME_TABLE calls VerdanturfTown_BattleTentBattleRoom_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |
| `VerdanturfTown_BattleTentBattleRoom:map_scripts:003` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [VerdanturfTown_BattleTentBattleRoom_OnWarp](../../baseline/source/data/maps/VerdanturfTown_BattleTentBattleRoom/scripts.inc#L127) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls VerdanturfTown_BattleTentBattleRoom_OnWarp. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · FAC-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
