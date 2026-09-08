# VictoryRoad_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/VictoryRoad_1F/map.json) · [Scripts](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc)

## Current map contract

`MAP_VICTORY_ROAD_1F` · `LAYOUT_VICTORY_ROAD_1F` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `VictoryRoad_1F:object_events:001` | 33,22 | [VictoryRoad_1F_EventScript_Edgar](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L96) — VictoryRoad_1F_EventScript_Edgar at (33,22); I've made it this far a couple times, /  but the last stretch is so long… / My dream ends here again… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_1F:object_events:002` | 6,15 | [VictoryRoad_1F_EventScript_Hope](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L106) — VictoryRoad_1F_EventScript_Hope at (6,15); This seemingly infinite and harsh road /  lives up to its name of VICTORY. / Your battle style is fantastic… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_1F:object_events:003` | 27,34 | [VictoryRoad_1F_EventScript_Albert](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L101) — VictoryRoad_1F_EventScript_Albert at (27,34); I didn't come all this way to lose now. /  That possibility doesn't exist! / Impossible… /  I lost? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_1F:object_events:004` | 12,25 | [VictoryRoad_1F_EventScript_EntranceWally](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L79) — VictoryRoad_1F_EventScript_EntranceWally at (12,25); WALLY: I couldn't beat you today, but I /  saw the seam in my team. //  Next time it's closed. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `VictoryRoad_1F:object_events:005` | 40,26 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ZYGARDITE; root Common_EventScript_FindItem; flag FLAG_ITEM_VICTORY_ROAD_1F_ZYGARDITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `VictoryRoad_1F:object_events:006` | 37,39 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DARKRANITE; root Common_EventScript_FindItem; flag FLAG_ITEM_VICTORY_ROAD_1F_DARKRANITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `VictoryRoad_1F:object_events:007` | 31,9 | [VictoryRoad_1F_EventScript_ExitWally](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L84) — VictoryRoad_1F_EventScript_ExitWally at (31,9); WALLY: Hi! {PLAYER}! //  I rebuilt again since last time. I /  wanted you to see it first! //  Okay… Here I come! / You still had an answer for my answer… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_1F:object_events:008` | 29,17 | [VictoryRoad_1F_EventScript_Katelynn](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L116) — VictoryRoad_1F_EventScript_Katelynn at (29,17); I have nothing to say to anyone /  that's come this far. Come on! / This is a disgrace… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_1F:object_events:009` | 32,17 | [VictoryRoad_1F_EventScript_Quincy](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L111) — VictoryRoad_1F_EventScript_Quincy at (32,17); What is the VICTORY ROAD? /  I'll tell you if you win! / Okay! /  Well done! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `VictoryRoad_1F:object_events:010` | 4,14 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (4,14); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `VictoryRoad_1F:coord_events:001` | 2,23 | [VictoryRoad_1F_EventScript_WallyBattleTrigger1](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L20) — Coordinate trigger at (2,23); VAR_VICTORY_ROAD_1F_STATE == 0 invokes VictoryRoad_1F_EventScript_WallyBattleTrigger1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `VictoryRoad_1F:coord_events:002` | 3,23 | [VictoryRoad_1F_EventScript_WallyBattleTrigger2](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L29) — Coordinate trigger at (3,23); VAR_VICTORY_ROAD_1F_STATE == 0 invokes VictoryRoad_1F_EventScript_WallyBattleTrigger2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `VictoryRoad_1F:bg_events:001` | 30,39 | Hidden ITEM_ULTRA_BALL at (30,39); persistent flag FLAG_HIDDEN_ITEM_VICTORY_ROAD_1F_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `VictoryRoad_1F:warp_events:001` | 15,40 | Warp from (15,40, elevation 3) to MAP_EVER_GRANDE_CITY warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_1F:warp_events:002` | 39,5 | Warp from (39,5, elevation 3) to MAP_EVER_GRANDE_CITY warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_1F:warp_events:003` | 21,32 | Warp from (21,32, elevation 3) to MAP_VICTORY_ROAD_B1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_1F:warp_events:004` | 42,38 | Warp from (42,38, elevation 4) to MAP_VICTORY_ROAD_B1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_1F:warp_events:005` | 9,14 | Warp from (9,14, elevation 4) to MAP_VICTORY_ROAD_B1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `VictoryRoad_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [VictoryRoad_1F_OnTransition](../../baseline/source/data/maps/VictoryRoad_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls VictoryRoad_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
