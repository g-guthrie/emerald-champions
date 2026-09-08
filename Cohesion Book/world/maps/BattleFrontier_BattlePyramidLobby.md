# BattleFrontier_BattlePyramidLobby

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_PYRAMID_LOBBY` · `LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_LOBBY` · `WEATHER_NONE` · `MUS_B_PYRAMID`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattlePyramidLobby:object_events:001` | 7,12 | [BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit](../../baseline/source/data/maps/BattleFrontier_BattleTowerLobby/scripts.inc#L445) — BattleFrontier_BattleTowerLobby_EventScript_ChampionsCircuit at (7,12); shared behavior FACILITY | **REPAIR** · [W-C-FACILITY](../common-contracts.md#w-c-facility) · W-CIRCUIT-RULES |
| `BattleFrontier_BattlePyramidLobby:object_events:002` | 14,13 | [BattleFrontier_BattlePyramidLobby_EventScript_HintGiver](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L209) — BattleFrontier_BattlePyramidLobby_EventScript_HintGiver at (14,13); Welcome… //  I shall be pleased to tell you what /  misfortunes await in the PYRAMID… / … … … … … … /  … … … … … … //  … … … … … … /  Aah! | **REPAIR** · [W-C-STORY](../common-contracts.md#w-c-story) · W-FRONTIER-ANCILLARY |
| `BattleFrontier_BattlePyramidLobby:object_events:003` | 2,15 | [BattleFrontier_BattlePyramidLobby_EventScript_Woman](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L478) — BattleFrontier_BattlePyramidLobby_EventScript_Woman at (2,15); Did you know? //  If you run fast, TRAINERS may notice /  and come after you for a battle. //  So, if you want to avoid TRAINERS, /  don't catch their eyes, but sneak /  cautiously and quietly past them. | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattlePyramidLobby:object_events:004` | 12,16 | [BattleFrontier_BattlePyramidLobby_EventScript_FatMan](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L482) — BattleFrontier_BattlePyramidLobby_EventScript_FatMan at (12,16); Awaaaaaaarrrrgh! //  I had a whole lot of items, but I lost /  them all when I lost! //  Awaaaaaaarrrrgh! | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FRONTIER-RESIDENTS |
| `BattleFrontier_BattlePyramidLobby:bg_events:001` | 5,12 | [BattleFrontier_BattlePyramidLobby_EventScript_ShowResults](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L364) — BattleFrontier_BattlePyramidLobby_EventScript_ShowResults at (5,12); shared behavior BACKGROUND | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RECORDS |
| `BattleFrontier_BattlePyramidLobby:bg_events:002` | 1,12 | [BattleFrontier_BattlePyramidLobby_EventScript_RulesBoard](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L486) — BattleFrontier_BattlePyramidLobby_EventScript_RulesBoard at (1,12); The Battle Quest rules are listed. / Which heading do you want to read? | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-CIRCUIT-RULES |
| `BattleFrontier_BattlePyramidLobby:warp_events:001` | 7,17 | Warp from (7,17, elevation 4) to MAP_BATTLE_FRONTIER_OUTSIDE_EAST warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `BattleFrontier_BattlePyramidLobby:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattlePyramidLobby_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc#L16) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattlePyramidLobby_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePyramidLobby:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattleDomeLobby_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattleDomeLobby/scripts.inc#L11) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattleDomeLobby_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
