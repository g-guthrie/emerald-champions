# LittlerootTown_MaysHouse_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/map.json) · [Scripts](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc)

## Current map contract

`MAP_LITTLEROOT_TOWN_MAYS_HOUSE_1F` · `LAYOUT_LITTLEROOT_TOWN_MAYS_HOUSE_1F` · `WEATHER_NONE` · `MUS_LITTLEROOT`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `LittlerootTown_MaysHouse_1F:object_events:001` | 8,6 | [PlayersHouse_1F_EventScript_Mom](../../baseline/source/data/scripts/players_house.inc#L299) — PlayersHouse_1F_EventScript_Mom at (8,6); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:object_events:002` | 6,5 | [PlayersHouse_1F_EventScript_Vigoroth1](../../baseline/source/data/scripts/players_house.inc#L368) — PlayersHouse_1F_EventScript_Vigoroth1 at (6,5); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:object_events:003` | 9,3 | [PlayersHouse_1F_EventScript_Vigoroth2](../../baseline/source/data/scripts/players_house.inc#L378) — PlayersHouse_1F_EventScript_Vigoroth2 at (9,3); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:object_events:004` | 8,7 | [RivalsHouse_1F_EventScript_RivalMom](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L112) — RivalsHouse_1F_EventScript_RivalMom at (8,7); Like child, like father. //  My husband is as wild about POKéMON /  as our child. //  If he's not at his LAB, he's likely /  scrabbling about in grassy places. / That {RIVAL}! //  I guess our child is too busy with /  POKéMON to notice that you came /  to visit, {PLAYER}{KUN}. | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:object_events:005` | 5,6 | Passive/staged OBJ_EVENT_GFX_NORMAN at (5,6); visibility flag FLAG_HIDE_PLAYERS_HOUSE_DAD; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:object_events:006` | 9,5 | [RivalsHouse_1F_EventScript_RivalSibling](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L138) — RivalsHouse_1F_EventScript_RivalSibling at (9,5); Hi, neighbor! //  Do you already have your /  own POKéMON? | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:object_events:007` | 2,8 | Passive/staged OBJ_EVENT_GFX_RIVAL_MAY_NORMAL at (2,8); visibility flag FLAG_HIDE_LITTLEROOT_TOWN_MAYS_HOUSE_MAY; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:coord_events:001` | 2,8 | [LittlerootTown_MaysHouse_1F_EventScript_GoSeeRoom](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L146) — Coordinate trigger at (2,8); VAR_LITTLEROOT_INTRO_STATE == 4 invokes LittlerootTown_MaysHouse_1F_EventScript_GoSeeRoom. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:coord_events:002` | 1,3 | [LittlerootTown_MaysHouse_1F_EventScript_MeetRival0](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L155) — Coordinate trigger at (1,3); VAR_LITTLEROOT_RIVAL_STATE == 2 invokes LittlerootTown_MaysHouse_1F_EventScript_MeetRival0. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:coord_events:003` | 2,4 | [LittlerootTown_MaysHouse_1F_EventScript_MeetRival1](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L161) — Coordinate trigger at (2,4); VAR_LITTLEROOT_RIVAL_STATE == 2 invokes LittlerootTown_MaysHouse_1F_EventScript_MeetRival1. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:coord_events:004` | 3,3 | [LittlerootTown_MaysHouse_1F_EventScript_MeetRival2](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L167) — Coordinate trigger at (3,3); VAR_LITTLEROOT_RIVAL_STATE == 2 invokes LittlerootTown_MaysHouse_1F_EventScript_MeetRival2. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:warp_events:001` | 1,8 | Warp from (1,8, elevation 0) to MAP_LITTLEROOT_TOWN warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:warp_events:002` | 2,8 | Warp from (2,8, elevation 0) to MAP_LITTLEROOT_TOWN warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:warp_events:003` | 2,2 | Warp from (2,2, elevation 0) to MAP_LITTLEROOT_TOWN_MAYS_HOUSE_2F warp 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [LittlerootTown_MaysHouse_1F_OnLoad](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L7) — MAP_SCRIPT_ON_LOAD calls LittlerootTown_MaysHouse_1F_OnLoad. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [LittlerootTown_MaysHouse_1F_OnTransition](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L26) — MAP_SCRIPT_ON_TRANSITION calls LittlerootTown_MaysHouse_1F_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |
| `LittlerootTown_MaysHouse_1F:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [LittlerootTown_MaysHouse_1F_OnFrame](../../baseline/source/data/maps/LittlerootTown_MaysHouse_1F/scripts.inc#L48) — MAP_SCRIPT_ON_FRAME_TABLE calls LittlerootTown_MaysHouse_1F_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
