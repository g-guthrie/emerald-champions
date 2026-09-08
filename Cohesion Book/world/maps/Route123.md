# Route123

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route123/map.json) · [Scripts](../../baseline/source/data/maps/Route123/scripts.inc)

## Current map contract

`MAP_ROUTE123` · `LAYOUT_ROUTE123` · `WEATHER_SUNNY` · `MUS_ROUTE122`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route123:object_events:001` | 11,3 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (11,3); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:002` | 12,3 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (12,3); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:003` | 14,3 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (14,3); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:004` | 15,3 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (15,3); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:005` | 81,1 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (81,1); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:006` | 82,1 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (82,1); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:007` | 83,1 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (83,1); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:008` | 70,12 | [Route123_EventScript_Wendy](../../baseline/source/data/maps/Route123/scripts.inc#L45) — Route123_EventScript_Wendy at (70,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:009` | 58,7 | [Route123_EventScript_Braxton](../../baseline/source/data/maps/Route123/scripts.inc#L50) — Route123_EventScript_Braxton at (58,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:010` | 14,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (14,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:011` | 15,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (15,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:012` | 17,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (17,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:013` | 18,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (18,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:014` | 17,3 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (17,3); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:015` | 18,3 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (18,3); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:016` | 11,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (11,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:017` | 12,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (12,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:018` | 101,13 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (101,13); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route123:object_events:019` | 129,14 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (129,14); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route123:object_events:020` | 92,9 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (92,9); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route123:object_events:021` | 57,16 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_REVIVAL_HERB; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_123_REVIVAL_HERB. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route123:object_events:022` | 108,13 | [Route123_EventScript_SweetAppleGirl](../../baseline/source/data/maps/Route123/scripts.inc#L9) — Route123_EventScript_SweetAppleGirl at (108,13); I love GRASS-type POKéMON! //  Do you have any GRASS-type POKéMON? / Oh? //  You like GRASS-type POKéMON, too, /  don't you? //  I'm so happy, you can have this! /  It's a token of our friendship. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route123:object_events:023` | 109,13 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (109,13); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:024` | 110,13 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (110,13); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:025` | 111,13 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (111,13); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route123:object_events:026` | 18,9 | [Route123_EventScript_Violet](../../baseline/source/data/maps/Route123/scripts.inc#L55) — Route123_EventScript_Violet at (18,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:027` | 38,13 | [Route123_EventScript_Yuki](../../baseline/source/data/maps/Route123/scripts.inc#L107) — Route123_EventScript_Yuki at (38,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:028` | 37,13 | [Route123_EventScript_Miu](../../baseline/source/data/maps/Route123/scripts.inc#L102) — Route123_EventScript_Miu at (37,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:029` | 138,12 | [Route123_EventScript_Cameron](../../baseline/source/data/maps/Route123/scripts.inc#L60) — Route123_EventScript_Cameron at (138,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:030` | 49,16 | [Route123_EventScript_Jacki](../../baseline/source/data/maps/Route123/scripts.inc#L81) — Route123_EventScript_Jacki at (49,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:031` | 87,12 | [Route123_EventScript_Kindra](../../baseline/source/data/maps/Route123/scripts.inc#L112) — Route123_EventScript_Kindra at (87,12); That shadow is following you. /  Most shadows do. Carry on. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route123:object_events:032` | 31,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_TIMER_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_123_TIMER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route123:object_events:033` | 75,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ELIXIR; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_123_ELIXIR. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route123:object_events:034` | 138,14 | [Route123_EventScript_Jonas](../../baseline/source/data/maps/Route123/scripts.inc#L136) — Route123_EventScript_Jonas at (138,14); My smoke bomb worked perfectly. /  Finding my lunch afterward did not. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route123:object_events:035` | 138,16 | [Route123_EventScript_Kayley](../../baseline/source/data/maps/Route123/scripts.inc#L131) — Route123_EventScript_Kayley at (138,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:036` | 87,17 | [Route123_EventScript_Ed](../../baseline/source/data/maps/Route123/scripts.inc#L126) — Route123_EventScript_Ed at (87,17); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:037` | 66,16 | [Route123_EventScript_Fernando](../../baseline/source/data/maps/Route123/scripts.inc#L150) — Route123_EventScript_Fernando at (66,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:038` | 66,19 | [Route123_EventScript_Alberto](../../baseline/source/data/maps/Route123/scripts.inc#L121) — Route123_EventScript_Alberto at (66,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:039` | 49,19 | [Route123_EventScript_Frederick](../../baseline/source/data/maps/Route123/scripts.inc#L116) — Route123_EventScript_Frederick at (49,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:040` | 43,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PP_UP; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_123_PP_UP. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route123:object_events:041` | 14,16 | [Route123_EventScript_Jazmyn](../../baseline/source/data/maps/Route123/scripts.inc#L140) — Route123_EventScript_Jazmyn at (14,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:042` | 14,12 | [Route123_EventScript_Davis](../../baseline/source/data/maps/Route123/scripts.inc#L145) — Route123_EventScript_Davis at (14,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route123:object_events:043` | 27,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_123_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route123:object_events:044` | 7,14 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (7,14); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route123:coord_events:001` | 90,16 | Coordinate weather at (90,16); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:002` | 92,9 | Coordinate weather at (92,9); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:003` | 93,12 | Coordinate weather at (93,12); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:004` | 92,13 | Coordinate weather at (92,13); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:005` | 91,14 | Coordinate weather at (91,14); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:006` | 19,13 | Coordinate weather at (19,13); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:007` | 19,12 | Coordinate weather at (19,12); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:008` | 34,13 | Coordinate weather at (34,13); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:009` | 19,14 | Coordinate weather at (19,14); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:010` | 19,15 | Coordinate weather at (19,15); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:011` | 19,16 | Coordinate weather at (19,16); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:012` | 108,14 | Coordinate weather at (108,14); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:013` | 109,15 | Coordinate weather at (109,15); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:014` | 110,16 | Coordinate weather at (110,16); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:015` | 111,17 | Coordinate weather at (111,17); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:016` | 112,18 | Coordinate weather at (112,18); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:017` | 9,12 | Coordinate weather at (9,12); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:018` | 9,13 | Coordinate weather at (9,13); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:019` | 9,14 | Coordinate weather at (9,14); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:020` | 9,15 | Coordinate weather at (9,15); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:021` | 9,16 | Coordinate weather at (9,16); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:coord_events:022` | 94,10 | Coordinate weather at (94,10); weather COORD_EVENT_WEATHER_ROUTE123_CYCLE. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route123:bg_events:001` | 117,10 | [Route123_EventScript_RouteSignMtPyre](../../baseline/source/data/maps/Route123/scripts.inc#L37) — Route123_EventScript_RouteSignMtPyre at (117,10); {UP_ARROW} MT. PYRE /  “Forbidden to the faint of heart.” //  A horned ward below the words resembles /  TAUROS. | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01, W-SIGN-LOCAL |
| `Route123:bg_events:002` | 47,3 | None at (47,3); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route123:bg_events:003` | 49,3 | None at (49,3); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route123:bg_events:004` | 10,12 | [Route123_EventScript_RouteSign](../../baseline/source/data/maps/Route123/scripts.inc#L33) — Route123_EventScript_RouteSign at (10,12); {RIGHT_ARROW} ROUTE 123 /  {LEFT_ARROW} ROUTE 118 | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route123:bg_events:005` | 75,1 | Hidden ITEM_SUPER_REPEL at (75,1); persistent flag FLAG_HIDDEN_ITEM_ROUTE_123_SUPER_REPEL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route123:bg_events:006` | 20,7 | [Route123_EventScript_BerryMastersHouseSign](../../baseline/source/data/maps/Route123/scripts.inc#L41) — Route123_EventScript_BerryMastersHouseSign at (20,7); BERRY MASTER'S HOUSE | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route123:bg_events:007` | 57,5 | None at (57,5); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route123:bg_events:008` | 12,1 | Hidden ITEM_REVIVE at (12,1); persistent flag FLAG_HIDDEN_ITEM_ROUTE_123_REVIVE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route123:bg_events:009` | 91,15 | Hidden ITEM_HYPER_POTION at (91,15); persistent flag FLAG_HIDDEN_ITEM_ROUTE_123_HYPER_POTION. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route123:bg_events:010` | 139,15 | Hidden ITEM_PP_UP at (139,15); persistent flag FLAG_HIDDEN_ITEM_ROUTE_123_PP_UP. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route123:bg_events:011` | 138,18 | Hidden ITEM_ULTRA_BALL at (138,18); persistent flag FLAG_HIDDEN_ITEM_ROUTE_123_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route123:warp_events:001` | 22,6 | Warp from (22,6, elevation 0) to MAP_ROUTE123_BERRY_MASTERS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route123:connections:001` | up | up connection to MAP_ROUTE122, offset 100. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route123:connections:002` | left | left connection to MAP_ROUTE118, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route123:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route123_OnTransition](../../baseline/source/data/maps/Route123/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route123_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
