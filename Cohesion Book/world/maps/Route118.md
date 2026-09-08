# Route118

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route118/map.json) · [Scripts](../../baseline/source/data/maps/Route118/scripts.inc)

## Current map contract

`MAP_ROUTE118` · `LAYOUT_ROUTE118` · `WEATHER_SUNNY` · `MUS_ROUTE118`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route118:object_events:001` | 35,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (35,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route118:object_events:002` | 36,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (36,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route118:object_events:003` | 37,5 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (37,5); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route118:object_events:004` | 64,10 | [Route118_EventScript_Perry](../../baseline/source/data/maps/Route118/scripts.inc#L241) — Route118_EventScript_Perry at (64,10); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:005` | 33,8 | [GabbyAndTy_EventScript_GabbyBattle2](../../baseline/source/data/scripts/gabby_and_ty.inc#L160) — GabbyAndTy_EventScript_GabbyBattle2 at (33,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:006` | 34,8 | [GabbyAndTy_EventScript_TyBattle2](../../baseline/source/data/scripts/gabby_and_ty.inc#L166) — GabbyAndTy_EventScript_TyBattle2 at (34,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:007` | 12,10 | [Route118_EventScript_Girl](../../baseline/source/data/maps/Route118/scripts.inc#L63) — Route118_EventScript_Girl at (12,10); Even if there isn't a boat, you can /  cross rivers and the sea if you have /  a POKéMON that knows SURF. //  POKéMON can be counted on to do so /  much! | **REPAIR** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · W-FIELD-CLUES |
| `Route118:object_events:008` | 33,8 | [GabbyAndTy_EventScript_GabbyBattle5](../../baseline/source/data/scripts/gabby_and_ty.inc#L196) — GabbyAndTy_EventScript_GabbyBattle5 at (33,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:009` | 34,8 | [GabbyAndTy_EventScript_TyBattle5](../../baseline/source/data/scripts/gabby_and_ty.inc#L202) — GabbyAndTy_EventScript_TyBattle5 at (34,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:010` | 33,8 | [GabbyAndTy_EventScript_GabbyBattle6](../../baseline/source/data/scripts/gabby_and_ty.inc#L208) — GabbyAndTy_EventScript_GabbyBattle6 at (33,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:011` | 34,8 | [GabbyAndTy_EventScript_TyBattle6](../../baseline/source/data/scripts/gabby_and_ty.inc#L214) — GabbyAndTy_EventScript_TyBattle6 at (34,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:012` | 38,8 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (38,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route118:object_events:013` | 28,8 | [Route118_EventScript_GoodRodFisherman](../../baseline/source/data/maps/Route118/scripts.inc#L23) — Route118_EventScript_GoodRodFisherman at (28,8); Hmm! /  A GOOD ROD is really good! //  Wouldn't you agree? / Hmm! /  We're of identical minds! //  Hmm! /  Take this GOOD ROD! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route118:object_events:014` | 7,12 | [Route118_EventScript_Rose](../../baseline/source/data/maps/Route118/scripts.inc#L191) — Route118_EventScript_Rose at (7,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:015` | 14,14 | [Route118_EventScript_Wade](../../baseline/source/data/maps/Route118/scripts.inc#L216) — Route118_EventScript_Wade at (14,14); That splash was a huge fish! /  Or my lunch. I hope it was a fish. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route118:object_events:016` | 56,7 | [Route118_EventScript_Chester](../../baseline/source/data/maps/Route118/scripts.inc#L246) — Route118_EventScript_Chester at (56,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:017` | 39,15 | [Route118_EventScript_Barny](../../baseline/source/data/maps/Route118/scripts.inc#L212) — Route118_EventScript_Barny at (39,15); WAILORD took the bait once. /  I am still paying for the rod. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route118:object_events:018` | 17,11 | [Route118_EventScript_Dalton](../../baseline/source/data/maps/Route118/scripts.inc#L220) — Route118_EventScript_Dalton at (17,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:019` | 44,7 | Passive/staged OBJ_EVENT_GFX_STEVEN at (44,7); visibility flag FLAG_HIDE_ROUTE_118_STEVEN; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route118:object_events:020` | 69,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SCIZORITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_118_SCIZORITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route118:object_events:021` | 7,7 | [Route118_EventScript_Deandre](../../baseline/source/data/maps/Route118/scripts.inc#L251) — Route118_EventScript_Deandre at (7,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route118:object_events:022` | 5,7 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (5,7); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route118:coord_events:001` | 43,11 | [Route118_EventScript_StevenTrigger0](../../baseline/source/data/maps/Route118/scripts.inc#L75) — Coordinate trigger at (43,11); VAR_ROUTE118_STATE == 0 invokes Route118_EventScript_StevenTrigger0. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route118:coord_events:002` | 44,11 | [Route118_EventScript_StevenTrigger1](../../baseline/source/data/maps/Route118/scripts.inc#L85) — Coordinate trigger at (44,11); VAR_ROUTE118_STATE == 0 invokes Route118_EventScript_StevenTrigger1. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route118:coord_events:003` | 45,11 | [Route118_EventScript_StevenTrigger2](../../baseline/source/data/maps/Route118/scripts.inc#L93) — Coordinate trigger at (45,11); VAR_ROUTE118_STATE == 0 invokes Route118_EventScript_StevenTrigger2. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route118:bg_events:001` | 47,14 | None at (47,14); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route118:bg_events:002` | 13,6 | [Route118_EventScript_RouteSignMauville](../../baseline/source/data/maps/Route118/scripts.inc#L67) — Route118_EventScript_RouteSignMauville at (13,6); ROUTE 118 /  {LEFT_ARROW} MAUVILLE CITY | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route118:bg_events:003` | 56,8 | [Route118_EventScript_RouteSign119](../../baseline/source/data/maps/Route118/scripts.inc#L71) — Route118_EventScript_RouteSign119 at (56,8); ROUTE 118 /  {UP_ARROW} ROUTE 119 | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route118:bg_events:004` | 67,6 | None at (67,6); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route118:bg_events:005` | 29,5 | None at (29,5); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route118:bg_events:006` | 47,5 | None at (47,5); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route118:bg_events:007` | 46,5 | None at (46,5); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route118:bg_events:008` | 31,13 | Hidden ITEM_NET_BALL at (31,13); persistent flag FLAG_HIDDEN_ITEM_ROUTE_118_NET_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route118:bg_events:009` | 12,14 | Hidden ITEM_HEART_SCALE at (12,14); persistent flag FLAG_HIDDEN_ITEM_ROUTE_118_HEART_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route118:warp_events:001` | 42,6 | Warp from (42,6, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route118:warp_events:002` | 9,6 | Warp from (9,6, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route118:connections:001` | up | up connection to MAP_ROUTE119, offset 40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route118:connections:002` | left | left connection to MAP_MAUVILLE_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route118:connections:003` | right | right connection to MAP_ROUTE123, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route118:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route118_OnTransition](../../baseline/source/data/maps/Route118/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls Route118_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route118:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route118_OnLoad](../../baseline/source/data/maps/Route118/scripts.inc#L14) — MAP_SCRIPT_ON_LOAD calls Route118_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route118:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route118_OnFrame](../../baseline/source/data/maps/Route118/scripts.inc#L19) — MAP_SCRIPT_ON_FRAME_TABLE calls Route118_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
