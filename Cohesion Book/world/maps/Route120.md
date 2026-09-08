# Route120

**REVISE.** Preserve optional Kecleon captures, lake and grove discoveries, Scope demo and Feather gate. Keep the field landmarks and remove obsolete Gardevoir/Xatu silhouettes as implied prerequisites.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route120/map.json) · [Scripts](../../baseline/source/data/maps/Route120/scripts.inc)

## Current map contract

`MAP_ROUTE120` · `LAYOUT_ROUTE120` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route120:object_events:001` | 4,79 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (4,79); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:002` | 5,79 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (5,79); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:003` | 6,79 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (6,79); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:004` | 7,79 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (7,79); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:005` | 34,24 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (34,24); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:006` | 35,24 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (35,24); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:007` | 36,24 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (36,24); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:008` | 9,92 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (9,92); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:009` | 10,92 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (10,92); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:010` | 11,92 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (11,92); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route120:object_events:011` | 5,22 | [Route120_EventScript_Colin](../../baseline/source/data/maps/Route120/scripts.inc#L300) — Route120_EventScript_Colin at (5,22); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:012` | 32,14 | [Route120_EventScript_Robert](../../baseline/source/data/maps/Route120/scripts.inc#L305) — Route120_EventScript_Robert at (32,14); My bird knows the way home. /  I am trying not to look jealous. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route120:object_events:013` | 27,51 | [Route120_EventScript_Lorenzo](../../baseline/source/data/maps/Route120/scripts.inc#L321) — Route120_EventScript_Lorenzo at (27,51); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:014` | 36,45 | [Route120_EventScript_Jenna](../../baseline/source/data/maps/Route120/scripts.inc#L326) — Route120_EventScript_Jenna at (36,45); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:015` | 19,80 | [Route120_EventScript_Jeffrey](../../baseline/source/data/maps/Route120/scripts.inc#L331) — Route120_EventScript_Jeffrey at (19,80); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:016` | 20,55 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GENGARITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_120_GENGARITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route120:object_events:017` | 37,5 | [GabbyAndTy_EventScript_TyBattle3](../../baseline/source/data/scripts/gabby_and_ty.inc#L178) — GabbyAndTy_EventScript_TyBattle3 at (37,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:018` | 36,5 | [GabbyAndTy_EventScript_GabbyBattle3](../../baseline/source/data/scripts/gabby_and_ty.inc#L172) — GabbyAndTy_EventScript_GabbyBattle3 at (36,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:019` | 36,5 | [GabbyAndTy_EventScript_GabbyBattle6](../../baseline/source/data/scripts/gabby_and_ty.inc#L208) — GabbyAndTy_EventScript_GabbyBattle6 at (36,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:020` | 37,5 | [GabbyAndTy_EventScript_TyBattle6](../../baseline/source/data/scripts/gabby_and_ty.inc#L214) — GabbyAndTy_EventScript_TyBattle6 at (37,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:021` | 35,32 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (35,32); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route120:object_events:022` | 7,89 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_WEPEAR_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_ROUTE120_WEPEAR_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route120:object_events:023` | 14,92 | [Route120_EventScript_BerryBeauty](../../baseline/source/data/maps/Route120/scripts.inc#L100) — Route120_EventScript_BerryBeauty at (14,92); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route120:object_events:024` | 31,37 | [Route120_EventScript_Jennifer](../../baseline/source/data/maps/Route120/scripts.inc#L352) — Route120_EventScript_Jennifer at (31,37); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:025` | 9,60 | [Route120_EventScript_Chip](../../baseline/source/data/maps/Route120/scripts.inc#L357) — Route120_EventScript_Chip at (9,60); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:026` | 16,6 | [Route120_EventScript_Clarissa](../../baseline/source/data/maps/Route120/scripts.inc#L362) — Route120_EventScript_Clarissa at (16,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:027` | 18,34 | [Route120_EventScript_Angelica](../../baseline/source/data/maps/Route120/scripts.inc#L367) — Route120_EventScript_Angelica at (18,34); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:028` | 22,13 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ZERAORITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_120_ZERAORITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route120:object_events:029` | 23,82 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HYPER_POTION; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_120_HYPER_POTION. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route120:object_events:030` | 12,16 | [Route120_EventScript_BridgeKecleon](../../baseline/source/data/maps/Route120/scripts.inc#L288) — Route120_EventScript_BridgeKecleon at (12,16); Something unseeable is in the way. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route120:object_events:031` | 13,15 | [Route120_EventScript_Steven](../../baseline/source/data/maps/Route120/scripts.inc#L168) — Route120_EventScript_Steven at (13,15); STEVEN: Hm? {PLAYER}{KUN}, hi. /  It's been a while. //  There's something here that you can't /  see, right? //  Now, if I were to use this device on /  the invisible obstacle… //  No, no. Rather than describing it, /  I should just show you. /  That would be more fun. //  {PLAYER}{KUN}, are your POKéMON ready for /  battle? / STEVEN: No? //  I'll wait here, so you can get ready. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route120:object_events:032` | 20,1 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (20,1); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route120:object_events:033` | 15,1 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (15,1); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route120:object_events:034` | 10,72 | [Route120_EventScript_Keigo](../../baseline/source/data/maps/Route120/scripts.inc#L372) — Route120_EventScript_Keigo at (10,72); You cannot see a true ninja. /  Please pretend you cannot see me. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route120:object_events:035` | 19,28 | [Route120_EventScript_Riley](../../baseline/source/data/maps/Route120/scripts.inc#L376) — Route120_EventScript_Riley at (19,28); I practiced hiding in the grass. /  The grass had other occupants. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route120:object_events:036` | 12,16 | Passive/staged OBJ_EVENT_GFX_KECLEON_BRIDGE_SHADOW at (12,16); visibility flag FLAG_HIDE_ROUTE_120_KECLEON_BRIDGE_SHADOW; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route120:object_events:037` | 20,11 | [Route120_EventScript_Kecleon1](../../baseline/source/data/scripts/kecleon.inc#L1) — Route120_EventScript_Kecleon1 at (20,11); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route120:object_events:038` | 27,2 | [Route120_EventScript_Kecleon2](../../baseline/source/data/scripts/kecleon.inc#L8) — Route120_EventScript_Kecleon2 at (27,2); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route120:object_events:039` | 4,77 | [Route120_EventScript_Kecleon3](../../baseline/source/data/scripts/kecleon.inc#L15) — Route120_EventScript_Kecleon3 at (4,77); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route120:object_events:040` | 7,51 | [Route120_EventScript_Kecleon5](../../baseline/source/data/scripts/kecleon.inc#L29) — Route120_EventScript_Kecleon5 at (7,51); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route120:object_events:041` | 19,48 | [Route120_EventScript_Kecleon4](../../baseline/source/data/scripts/kecleon.inc#L22) — Route120_EventScript_Kecleon4 at (19,48); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route120:object_events:042` | 19,32 | [Route120_EventScript_Callie](../../baseline/source/data/maps/Route120/scripts.inc#L385) — Route120_EventScript_Callie at (19,32); Every good partner needs a break. /  Mine insisted I take one first. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route120:object_events:043` | 14,34 | [Route120_EventScript_Leonel](../../baseline/source/data/maps/Route120/scripts.inc#L380) — Route120_EventScript_Leonel at (14,34); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route120:object_events:044` | 24,33 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_120_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route120:object_events:045` | 3,54 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (3,54); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route120:coord_events:001` | 7,15 | Coordinate weather at (7,15); weather COORD_EVENT_WEATHER_RAIN. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:002` | 7,16 | Coordinate weather at (7,16); weather COORD_EVENT_WEATHER_RAIN. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:003` | 22,61 | Coordinate weather at (22,61); weather COORD_EVENT_WEATHER_RAIN. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:004` | 12,64 | Coordinate weather at (12,64); weather COORD_EVENT_WEATHER_RAIN. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:005` | 35,63 | Coordinate weather at (35,63); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:006` | 36,63 | Coordinate weather at (36,63); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:007` | 37,63 | Coordinate weather at (37,63); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:008` | 38,63 | Coordinate weather at (38,63); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:009` | 32,88 | Coordinate weather at (32,88); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:010` | 28,15 | Coordinate weather at (28,15); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:011` | 28,16 | Coordinate weather at (28,16); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:012` | 28,17 | Coordinate weather at (28,17); weather COORD_EVENT_WEATHER_SUNNY. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:013` | 32,89 | Coordinate weather at (32,89); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:014` | 32,90 | Coordinate weather at (32,90); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:015` | 32,91 | Coordinate weather at (32,91); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:016` | 10,75 | Coordinate weather at (10,75); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:017` | 11,75 | Coordinate weather at (11,75); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:018` | 12,75 | Coordinate weather at (12,75); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:coord_events:019` | 13,75 | Coordinate weather at (13,75); weather COORD_EVENT_WEATHER_SUNNY_CLOUDS. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route120:bg_events:001` | 27,3 | [Route120_EventScript_RouteSignFortree](../../baseline/source/data/maps/Route120/scripts.inc#L292) — Route120_EventScript_RouteSignFortree at (27,3); ROUTE 120 /  {LEFT_ARROW} FORTREE CITY //  Two weathered marks show GARDEVOIR /  beside water and XATU beside grass. | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01, W-SIGN-LOCAL |
| `Route120:bg_events:002` | 38,88 | [Route120_EventScript_RouteSign121](../../baseline/source/data/maps/Route120/scripts.inc#L296) — Route120_EventScript_RouteSign121 at (38,88); {RIGHT_ARROW} ROUTE 121 /  {LEFT_ARROW} ROUTE 120 | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route120:bg_events:003` | 28,62 | None at (28,62); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:004` | 30,62 | None at (30,62); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:005` | 26,10 | None at (26,10); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:006` | 29,85 | None at (29,85); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:007` | 18,12 | None at (18,12); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:008` | 38,54 | None at (38,54); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:009` | 31,23 | None at (31,23); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:bg_events:010` | 9,1 | Hidden ITEM_ULTRA_BALL at (9,1); persistent flag FLAG_HIDDEN_ITEM_ROUTE_120_ULTRA_BALL_1. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route120:bg_events:011` | 31,11 | Hidden ITEM_REVIVE at (31,11); persistent flag FLAG_HIDDEN_ITEM_ROUTE_120_REVIVE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route120:bg_events:012` | 0,86 | Hidden ITEM_ULTRA_BALL at (0,86); persistent flag FLAG_HIDDEN_ITEM_ROUTE_120_ULTRA_BALL_2. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route120:bg_events:013` | 24,42 | Hidden ITEM_LUXURY_BALL at (24,42); persistent flag FLAG_HIDDEN_ITEM_ROUTE_120_LUXURY_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route120:bg_events:014` | 5,76 | None at (5,76); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route120:warp_events:001` | 7,55 | Warp from (7,55, elevation 0) to MAP_ANCIENT_TOMB warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route120:warp_events:002` | 19,23 | Warp from (19,23, elevation 1) to MAP_SCORCHED_SLAB warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route120:connections:001` | left | left connection to MAP_FORTREE_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route120:connections:002` | right | right connection to MAP_ROUTE121, offset 80. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route120:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Route120_OnResume](../../baseline/source/data/maps/Route120/scripts.inc#L8) — MAP_SCRIPT_ON_RESUME calls Route120_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route120:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route120_OnTransition](../../baseline/source/data/maps/Route120/scripts.inc#L56) — MAP_SCRIPT_ON_TRANSITION calls Route120_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route120:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [Route120_OnLoad](../../baseline/source/data/maps/Route120/scripts.inc#L34) — MAP_SCRIPT_ON_LOAD calls Route120_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
