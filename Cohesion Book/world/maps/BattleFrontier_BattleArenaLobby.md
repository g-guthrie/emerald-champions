# BattleFrontier_BattleArenaLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_ARENA_LOBBY` · `LAYOUT_BATTLE_FRONTIER_BATTLE_ARENA_LOBBY` · `WEATHER_NONE` · `MUS_B_ARENA`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattleArenaLobby:object_events:001` | 7,7 | [BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit](../../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L445) — BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit at (7,7); shared behavior FACILITY | **REPAIR** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · W-CIRCUIT-RULES |
| `BattleFrontier_BattleArenaLobby:object_events:002` | 2,10 | [BattleFrontier_BattleArenaLobby_EventScript_Woman](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L312) — BattleFrontier_BattleArenaLobby_EventScript_Woman at (2,10); In the BATTLE ARENA, the order of /  POKéMON is totally important. //  For example, if your first POKéMON /  has certain type disadvantages, /  try making your second POKéMON one /  with moves that are super effective /  against the first one. //  I think that will be a good way of /  making an effective team. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-ANCILLARY |
| `BattleFrontier_BattleArenaLobby:object_events:003` | 14,11 | [BattleFrontier_BattleArenaLobby_EventScript_Man](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L304) — BattleFrontier_BattleArenaLobby_EventScript_Man at (14,11); I won in judging! //  Landing hits consistently on /  the opponent's POKéMON worked! | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-ANCILLARY |
| `BattleFrontier_BattleArenaLobby:object_events:004` | 14,12 | [BattleFrontier_BattleArenaLobby_EventScript_Camper](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L308) — BattleFrontier_BattleArenaLobby_EventScript_Camper at (14,12); Our match was declared a draw. //  When we ran out of time, both my /  POKéMON and the opponent's had about /  the same amount of HP left. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-ANCILLARY |
| `BattleFrontier_BattleArenaLobby:object_events:005` | 14,10 | [BattleFrontier_BattleArenaLobby_EventScript_Youngster](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L300) — BattleFrontier_BattleArenaLobby_EventScript_Youngster at (14,10); I lost on the REFEREE's decision… //  I don't think it was a good idea to only /  use defensive moves and not attack… | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-ANCILLARY |
| `BattleFrontier_BattleArenaLobby:bg_events:001` | 5,9 | [BattleFrontier_BattleArenaLobby_EventScript_ShowResults](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L292) — BattleFrontier_BattleArenaLobby_EventScript_ShowResults at (5,9); shared behavior BACKGROUND | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RECORDS |
| `BattleFrontier_BattleArenaLobby:bg_events:002` | 1,7 | [BattleFrontier_BattleArenaLobby_EventScript_RulesBoard](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L316) — BattleFrontier_BattleArenaLobby_EventScript_RulesBoard at (1,7); The Set KO Tourney's rules are listed. / Which heading do you want to read? | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RULES |
| `BattleFrontier_BattleArenaLobby:warp_events:001` | 7,12 | Warp from (7,12, elevation 3) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattleArenaLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattleArenaLobby_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattleArenaLobby_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattleArenaLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattleArenaLobby_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattleArenaLobby/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattleArenaLobby_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
