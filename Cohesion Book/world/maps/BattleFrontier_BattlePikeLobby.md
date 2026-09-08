# BattleFrontier_BattlePikeLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_PIKE_LOBBY` · `LAYOUT_BATTLE_FRONTIER_BATTLE_PIKE_LOBBY` · `WEATHER_NONE` · `MUS_B_PIKE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattlePikeLobby:object_events:001` | 5,5 | [BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit](../../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L445) — BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit at (5,5); shared behavior FACILITY | **REPAIR** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · W-CIRCUIT-RULES |
| `BattleFrontier_BattlePikeLobby:object_events:002` | 10,9 | [BattleFrontier_BattlePikeLobby_EventScript_Hiker](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L219) — BattleFrontier_BattlePikeLobby_EventScript_Hiker at (10,9); Arrgh! I blew my chance! /  I was one room away from the goal! //  In this place, you'd better watch out /  for poison, freezing, and so on. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattlePikeLobby:object_events:003` | 0,5 | [BattleFrontier_BattlePikeLobby_EventScript_Twin](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L223) — BattleFrontier_BattlePikeLobby_EventScript_Twin at (0,5); I've completed the challenge 10 times /  now, but I've never had to battle /  a TRAINER once. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattlePikeLobby:object_events:004` | 8,9 | [BattleFrontier_BattlePikeLobby_EventScript_Beauty](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L227) — BattleFrontier_BattlePikeLobby_EventScript_Beauty at (8,9); Listen! Listen! //  Don't you think that the special /  abilities of POKéMON will be useful /  here? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_BattlePikeLobby:bg_events:001` | 8,3 | [BattleFrontier_BattlePikeLobby_EventScript_ShowResults](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L197) — BattleFrontier_BattlePikeLobby_EventScript_ShowResults at (8,3); shared behavior BACKGROUND | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RECORDS |
| `BattleFrontier_BattlePikeLobby:bg_events:002` | 1,3 | [BattleFrontier_BattlePikeLobby_EventScript_RulesBoard](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L231) — BattleFrontier_BattlePikeLobby_EventScript_RulesBoard at (1,3); The Battle Choice's rules are listed. / Which heading do you want to read? | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RULES |
| `BattleFrontier_BattlePikeLobby:warp_events:001` | 5,12 | Warp from (5,12, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePikeLobby:warp_events:002` | 4,12 | Warp from (4,12, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePikeLobby:warp_events:003` | 6,12 | Warp from (6,12, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePikeLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattlePikeLobby_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L6) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattlePikeLobby_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePikeLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattlePikeLobby_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattlePikeLobby/scripts.inc#L13) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattlePikeLobby_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
