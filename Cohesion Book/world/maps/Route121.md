# Route121

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route121/map.json) · [Scripts](../../baseline/source/data/maps/Route121/scripts.inc)

## Current map contract

`MAP_ROUTE121` · `LAYOUT_ROUTE121` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route121:object_events:001` | 29,14 | [Route121_EventScript_Woman](../../baseline/source/data/maps/Route121/scripts.inc#L45) — Route121_EventScript_Woman at (29,14); Ahead looms MT. PYRE… //  It is a natural monument to the spirits  /  of departed POKéMON… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route121:object_events:002` | 14,2 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (14,2); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:003` | 15,2 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (15,2); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:004` | 16,2 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (16,2); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:005` | 17,2 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (17,2); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:006` | 64,14 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (64,14); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:007` | 65,14 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (65,14); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:008` | 66,14 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (66,14); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:009` | 67,14 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (67,14); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route121:object_events:010` | 39,9 | [Route121_EventScript_Kate](../../baseline/source/data/maps/Route121/scripts.inc#L139) — Route121_EventScript_Kate at (39,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:011` | 40,9 | [Route121_EventScript_Joy](../../baseline/source/data/maps/Route121/scripts.inc#L144) — Route121_EventScript_Joy at (40,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:012` | 30,8 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (30,8); visibility flag FLAG_HIDE_ROUTE_121_TEAM_AQUA_GRUNTS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route121:object_events:013` | 30,7 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (30,7); visibility flag FLAG_HIDE_ROUTE_121_TEAM_AQUA_GRUNTS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route121:object_events:014` | 31,7 | Passive/staged OBJ_EVENT_GFX_AQUA_MEMBER_M at (31,7); visibility flag FLAG_HIDE_ROUTE_121_TEAM_AQUA_GRUNTS; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route121:object_events:015` | 32,5 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (32,5); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route121:object_events:016` | 65,4 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (65,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route121:object_events:017` | 63,5 | [Route121_EventScript_Vanessa](../../baseline/source/data/maps/Route121/scripts.inc#L109) — Route121_EventScript_Vanessa at (63,5); My POKéMON posed for a portrait. /  The artist requested hazard pay. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route121:object_events:018` | 55,8 | [Route121_EventScript_Walter](../../baseline/source/data/maps/Route121/scripts.inc#L113) — Route121_EventScript_Walter at (55,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:019` | 11,11 | [Route121_EventScript_Tammy](../../baseline/source/data/maps/Route121/scripts.inc#L134) — Route121_EventScript_Tammy at (11,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:020` | 22,5 | [Route121_EventScript_Jessica](../../baseline/source/data/maps/Route121/scripts.inc#L149) — Route121_EventScript_Jessica at (22,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:021` | 55,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_QUICK_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_121_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route121:object_events:022` | 26,12 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (26,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route121:object_events:023` | 11,6 | [Route121_EventScript_Cale](../../baseline/source/data/maps/Route121/scripts.inc#L170) — Route121_EventScript_Cale at (11,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:024` | 59,8 | [Route121_EventScript_Myles](../../baseline/source/data/maps/Route121/scripts.inc#L175) — Route121_EventScript_Myles at (59,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:025` | 59,13 | [Route121_EventScript_Pat](../../baseline/source/data/maps/Route121/scripts.inc#L180) — Route121_EventScript_Pat at (59,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:026` | 65,9 | [Route121_EventScript_Marcel](../../baseline/source/data/maps/Route121/scripts.inc#L185) — Route121_EventScript_Marcel at (65,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:027` | 72,9 | [Route121_EventScript_Cristin](../../baseline/source/data/maps/Route121/scripts.inc#L190) — Route121_EventScript_Cristin at (72,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route121:object_events:028` | 60,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_121_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route121:object_events:029` | 67,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NANAB_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_ROUTE121_NANAB_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route121:object_events:030` | 29,16 | [Route121_EventScript_Nurse](../../baseline/source/data/maps/Route121/scripts.inc#L4) — Route121_EventScript_Nurse at (29,16); MT. PYRE is a long climb. Shall I heal /  your party before you continue? / All better. Keep your partners close /  on the mountain. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route121:object_events:031` | 29,17 | [Route121_EventScript_Skitty](../../baseline/source/data/maps/Route121/scripts.inc#L20) — Route121_EventScript_Skitty at (29,17); SKITTY: Meenya? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route121:coord_events:001` | 25,5 | [Route121_EventScript_AquaGruntsMoveOut](../../baseline/source/data/maps/Route121/scripts.inc#L57) — Coordinate trigger at (25,5); VAR_ROUTE121_STATE == 0 invokes Route121_EventScript_AquaGruntsMoveOut. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route121:coord_events:002` | 25,6 | [Route121_EventScript_AquaGruntsMoveOut](../../baseline/source/data/maps/Route121/scripts.inc#L57) — Coordinate trigger at (25,6); VAR_ROUTE121_STATE == 0 invokes Route121_EventScript_AquaGruntsMoveOut. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route121:coord_events:003` | 25,7 | [Route121_EventScript_AquaGruntsMoveOut](../../baseline/source/data/maps/Route121/scripts.inc#L57) — Coordinate trigger at (25,7); VAR_ROUTE121_STATE == 0 invokes Route121_EventScript_AquaGruntsMoveOut. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route121:coord_events:004` | 25,8 | [Route121_EventScript_AquaGruntsMoveOut](../../baseline/source/data/maps/Route121/scripts.inc#L57) — Coordinate trigger at (25,8); VAR_ROUTE121_STATE == 0 invokes Route121_EventScript_AquaGruntsMoveOut. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route121:bg_events:001` | 32,14 | [Route121_EventScript_MtPyrePierSign](../../baseline/source/data/maps/Route121/scripts.inc#L49) — Route121_EventScript_MtPyrePierSign at (32,14); MT. PYRE PIER //  …The sign is old and worn out. /  The words are barely legible… | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route121:bg_events:002` | 40,11 | None at (40,11); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route121:bg_events:003` | 18,13 | None at (18,13); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route121:bg_events:004` | 43,7 | None at (43,7); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route121:bg_events:005` | 42,7 | None at (42,7); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route121:bg_events:006` | 39,6 | [Route121_EventScript_SafariZoneSign](../../baseline/source/data/maps/Route121/scripts.inc#L53) — Route121_EventScript_SafariZoneSign at (39,6); “Filled with rare POKéMON!” /  SAFARI ZONE | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route121:bg_events:007` | 23,10 | Hidden ITEM_BIG_NUGGET at (23,10); persistent flag FLAG_HIDDEN_ITEM_ROUTE_121_BIG_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route121:bg_events:008` | 58,3 | Hidden ITEM_NUGGET at (58,3); persistent flag FLAG_HIDDEN_ITEM_ROUTE_121_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route121:bg_events:009` | 72,5 | Hidden ITEM_FULL_HEAL at (72,5); persistent flag FLAG_HIDDEN_ITEM_ROUTE_121_FULL_HEAL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route121:bg_events:010` | 68,8 | Hidden ITEM_MAX_REVIVE at (68,8); persistent flag FLAG_HIDDEN_ITEM_ROUTE_121_MAX_REVIVE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route121:warp_events:001` | 37,5 | Warp from (37,5, elevation 0) to MAP_ROUTE121_SAFARI_ZONE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121:connections:001` | down | down connection to MAP_ROUTE122, offset 20. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121:connections:002` | left | left connection to MAP_ROUTE120, offset -80. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121:connections:003` | right | right connection to MAP_LILYCOVE_CITY, offset -10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
