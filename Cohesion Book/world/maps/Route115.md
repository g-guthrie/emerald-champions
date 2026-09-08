# Route115

**REVISE.** Keep early Seaspray discovery alongside Meteor Falls’s later approach. A cold side cave can be exciting early; Shoal’s distinct later content is reconciled in the acquisition volume rather than restricting all early Ice options.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route115/map.json) · [Scripts](../../baseline/source/data/maps/Route115/scripts.inc)

## Current map contract

`MAP_ROUTE115` · `LAYOUT_ROUTE115` · `WEATHER_SUNNY` · `MUS_ROUTE104`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route115:object_events:001` | 14,64 | [Route115_EventScript_Woman](../../baseline/source/data/maps/Route115/scripts.inc#L22) — Route115_EventScript_Woman at (14,64); The shifted tide exposed SEASPRAY CAVE /  along the lower shore. //  Its first chamber is wet stone. The floor /  below is somehow frozen. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route115:object_events:002` | 5,15 | [Route115_EventScript_Timothy](../../baseline/source/data/maps/Route115/scripts.inc#L34) — Route115_EventScript_Timothy at (5,15); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:003` | 27,53 | [Route115_EventScript_Nob](../../baseline/source/data/maps/Route115/scripts.inc#L60) — Route115_EventScript_Nob at (27,53); I punched a rock for training. /  The rock remains undefeated. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route115:object_events:004` | 12,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (12,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route115:object_events:005` | 13,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (13,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route115:object_events:006` | 14,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (14,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route115:object_events:007` | 15,50 | [Route115_EventScript_Cyndy](../../baseline/source/data/maps/Route115/scripts.inc#L76) — Route115_EventScript_Cyndy at (15,50); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:008` | 19,15 | [Route115_EventScript_Koichi](../../baseline/source/data/maps/Route115/scripts.inc#L55) — Route115_EventScript_Koichi at (19,15); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:009` | 24,62 | [Route115_EventScript_Hector](../../baseline/source/data/maps/Route115/scripts.inc#L97) — Route115_EventScript_Hector at (24,62); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:010` | 20,60 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PINAP_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_ROUTE115_PINAP_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route115:object_events:011` | 18,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MEDICHAMITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_115_MEDICHAMITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route115:object_events:012` | 12,14 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NANAB_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_ROUTE115_NANAB_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route115:object_events:013` | 31,64 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (31,64); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route115:object_events:014` | 31,65 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (31,65); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route115:object_events:015` | 29,50 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (29,50); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route115:object_events:016` | 31,56 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PYROARITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_115_PYROARITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route115:object_events:017` | 10,15 | [Route115_EventScript_Kyra](../../baseline/source/data/maps/Route115/scripts.inc#L102) — Route115_EventScript_Kyra at (10,15); I race my own best time now. /  She is a very annoying rival. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route115:object_events:018` | 11,12 | [Route115_EventScript_Jaiden](../../baseline/source/data/maps/Route115/scripts.inc#L106) — Route115_EventScript_Jaiden at (11,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:019` | 15,7 | [Route115_EventScript_Helene](../../baseline/source/data/maps/Route115/scripts.inc#L116) — Route115_EventScript_Helene at (15,7); My coach told me to loosen up. /  I dropped my entire backpack. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route115:object_events:020` | 10,7 | [Route115_EventScript_Alix](../../baseline/source/data/maps/Route115/scripts.inc#L111) — Route115_EventScript_Alix at (10,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:021` | 28,62 | [Route115_EventScript_Marlene](../../baseline/source/data/maps/Route115/scripts.inc#L120) — Route115_EventScript_Marlene at (28,62); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route115:object_events:022` | 26,67 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_RAICHUNITE_Y; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_115_RAICHUNITE_Y. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route115:object_events:023` | 23,29 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_VICTREEBELITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_115_VICTREEBELITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route115:bg_events:001` | 32,6 | None at (32,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:002` | 21,18 | None at (21,18); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:003` | 16,64 | [Route115_EventScript_RouteSignRustboro](../../baseline/source/data/maps/Route115/scripts.inc#L26) — Route115_EventScript_RouteSignRustboro at (16,64); ROUTE 115 /  {DOWN_ARROW} RUSTBORO CITY | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route115:bg_events:004` | 25,38 | [Route115_EventScript_MeteorFallsSign](../../baseline/source/data/maps/Route115/scripts.inc#L30) — Route115_EventScript_MeteorFallsSign at (25,38); METEOR FALLS /  FALLARBOR TOWN THROUGH HERE | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route115:bg_events:005` | 8,30 | None at (8,30); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:006` | 32,39 | None at (32,39); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:007` | 26,15 | None at (26,15); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:008` | 23,8 | None at (23,8); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:009` | 32,46 | None at (32,46); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:010` | 7,20 | None at (7,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:011` | 8,20 | None at (8,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:012` | 25,24 | None at (25,24); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:013` | 20,53 | None at (20,53); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route115:bg_events:014` | 15,49 | Hidden ITEM_HEART_SCALE at (15,49); persistent flag FLAG_HIDDEN_ITEM_ROUTE_115_HEART_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route115:warp_events:001` | 27,37 | Warp from (27,37, elevation 0) to MAP_METEOR_FALLS_1F_1R warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route115:warp_events:002` | 21,6 | Warp from (21,6, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route115:warp_events:003` | 36,10 | Warp from (36,10, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route115:warp_events:004` | 8,70 | Warp from (8,70, elevation 0) to MAP_SEASPRAY_CAVE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route115:connections:001` | down | down connection to MAP_RUSTBORO_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route115:connections:002` | right | right connection to MAP_ROUTE114, offset -40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route115:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [Route115_OnLoad](../../baseline/source/data/maps/Route115/scripts.inc#L7) — MAP_SCRIPT_ON_LOAD calls Route115_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route115:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route115_OnTransition](../../baseline/source/data/maps/Route115/scripts.inc#L12) — MAP_SCRIPT_ON_TRANSITION calls Route115_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route115:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route115_OnFrame](../../baseline/source/data/maps/Route115/scripts.inc#L18) — MAP_SCRIPT_ON_FRAME_TABLE calls Route115_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
