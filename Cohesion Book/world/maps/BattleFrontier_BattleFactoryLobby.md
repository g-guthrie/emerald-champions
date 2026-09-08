# BattleFrontier_BattleFactoryLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_FACTORY_LOBBY` · `LAYOUT_BATTLE_FRONTIER_BATTLE_FACTORY_LOBBY` · `WEATHER_NONE` · `MUS_B_FACTORY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleFactoryLobby:object_events:001` | 4,7 | [BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit](../../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L445) — BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit at (4,7); shared behavior FACILITY | **REPAIR** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · W-CIRCUIT-RULES |
| `BattleFrontier_BattleFactoryLobby:object_events:002` | 3,11 | [BattleFrontier_BattleFactoryLobby_EventScript_Woman](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L258) — BattleFrontier_BattleFactoryLobby_EventScript_Woman at (3,11); Hi! /  You, there! //  Are you thinking that the events here /  are easy since you don't need to have /  a raised team of POKéMON? //  I wouldn't be too sure about winning /  that easily. //  If you don't have thorough knowledge /  about POKéMON and their moves, /  it will be tough to keep winning. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattleFactoryLobby:object_events:003` | 14,11 | [BattleFrontier_BattleFactoryLobby_EventScript_Camper](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L262) — BattleFrontier_BattleFactoryLobby_EventScript_Camper at (14,11); I swapped for a weak POKéMON… /  I thought it was a good kind to have… //  They wiped the floor with us… | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattleFactoryLobby:object_events:004` | 13,11 | [BattleFrontier_BattleFactoryLobby_EventScript_Picnicker](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L266) — BattleFrontier_BattleFactoryLobby_EventScript_Picnicker at (13,11); Things haven't been going my way /  at all. //  You need to check your opponent's /  POKéMON during battle to see if /  they're any good. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `BattleFrontier_BattleFactoryLobby:object_events:005` | 6,10 | [BattleFrontier_BattleFactoryLobby_EventScript_FatMan](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L272) — BattleFrontier_BattleFactoryLobby_EventScript_FatMan at (6,10); You know how the staff here give you /  a few hints about your next opponent? //  Well, I'm a full-grown man, but I have /  trouble figuring out their hints. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattleFactoryLobby:object_events:006` | 14,7 | [BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit](../../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L445) — BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit at (14,7); shared behavior FACILITY | **REPAIR** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · W-CIRCUIT-RULES |
| `BattleFrontier_BattleFactoryLobby:bg_events:001` | 2,7 | [BattleFrontier_BattleFactoryLobby_EventScript_ShowSinglesResults](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L242) — BattleFrontier_BattleFactoryLobby_EventScript_ShowSinglesResults at (2,7); shared behavior BACKGROUND | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RECORDS |
| `BattleFrontier_BattleFactoryLobby:bg_events:002` | 11,7 | [BattleFrontier_BattleFactoryLobby_EventScript_ShowDoublesResults](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L250) — BattleFrontier_BattleFactoryLobby_EventScript_ShowDoublesResults at (11,7); shared behavior BACKGROUND | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RECORDS |
| `BattleFrontier_BattleFactoryLobby:bg_events:003` | 9,4 | [BattleFrontier_BattleFactoryLobby_EventScript_RulesBoard](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L276) — BattleFrontier_BattleFactoryLobby_EventScript_RulesBoard at (9,4); The Battle Swap rules are listed. / Which heading do you want to read? | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RULES |
| `BattleFrontier_BattleFactoryLobby:warp_events:001` | 9,11 | Warp from (9,11, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleFactoryLobby:warp_events:002` | 10,11 | Warp from (10,11, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleFactoryLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleFactoryLobby_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleFactoryLobby_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleFactoryLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattleFactoryLobby_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattleFactoryLobby_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
