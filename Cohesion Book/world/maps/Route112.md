# Route112

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route112/map.json) · [Scripts](../../baseline/source/data/maps/Route112/scripts.inc)

## Current map contract

`MAP_ROUTE112` · `LAYOUT_ROUTE112` · `WEATHER_SUNNY` · `MUS_ROUTE110`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route112:object_events:001` | 26,30 | [Route112_EventScript_MagmaGrunts](../../baseline/source/data/maps/Route112/scripts.inc#L18) — Route112_EventScript_MagmaGrunts at (26,30); Hey, man, is our leader really going /  to awaken that thing? / Sounds like it, yeah. But I heard /  we need a METEORITE to do it. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route112:object_events:002` | 24,34 | [Route112_EventScript_Brice](../../baseline/source/data/maps/Route112/scripts.inc#L105) — Route112_EventScript_Brice at (24,34); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route112:object_events:003` | 29,49 | [Route112_EventScript_Larry](../../baseline/source/data/maps/Route112/scripts.inc#L131) — Route112_EventScript_Larry at (29,49); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route112:object_events:004` | 22,46 | [Route112_EventScript_Carol](../../baseline/source/data/maps/Route112/scripts.inc#L136) — Route112_EventScript_Carol at (22,46); The mountain is lovely today. /  My sandwich is now toast. //  KOFFING takes to FIERY PATH better. /  Level it up there at Lv. 35 or higher /  for Galarian WEEZING. //  Outside that cave, it evolves into /  the usual WEEZING. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route112:object_events:005` | 15,40 | [Route112_EventScript_Trent](../../baseline/source/data/maps/Route112/scripts.inc#L110) — Route112_EventScript_Trent at (15,40); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route112:object_events:006` | 27,30 | [Route112_EventScript_MagmaGrunts](../../baseline/source/data/maps/Route112/scripts.inc#L18) — Route112_EventScript_MagmaGrunts at (27,30); Hey, man, is our leader really going /  to awaken that thing? / Sounds like it, yeah. But I heard /  we need a METEORITE to do it. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route112:object_events:007` | 27,6 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (27,6); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route112:object_events:008` | 28,6 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (28,6); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route112:object_events:009` | 29,6 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (29,6); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route112:object_events:010` | 30,6 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (30,6); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route112:object_events:011` | 8,50 | [Route112_EventScript_Hiker](../../baseline/source/data/maps/Route112/scripts.inc#L101) — Route112_EventScript_Hiker at (8,50); Eh, I'd like to get to MAUVILLE, but if /  I went down these ledges, it'd be no /  easy matter to get back to LAVARIDGE. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route112:object_events:012` | 14,43 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SKARMORITE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_112_SKARMORITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route112:object_events:013` | 31,7 | [Route112_EventScript_Bryant](../../baseline/source/data/maps/Route112/scripts.inc#L140) — Route112_EventScript_Bryant at (31,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route112:object_events:014` | 31,11 | [Route112_EventScript_Shayla](../../baseline/source/data/maps/Route112/scripts.inc#L145) — Route112_EventScript_Shayla at (31,11); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route112:object_events:015` | 25,29 | Passive/staged OBJ_EVENT_GFX_SPECIES(CHANSEY) at (25,29); visibility flag FLAG_HIDE_ROUTE112_VIAL_CHANSEY; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route112:object_events:016` | 2,52 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (2,52); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route112:coord_events:001` | 25,31 | [Route112_EventScript_VialChanseyEscape](../../baseline/source/data/maps/Route112/scripts.inc#L55) — Coordinate trigger at (25,31); VAR_CHANSEY_NURSE_STATE == 1 invokes Route112_EventScript_VialChanseyEscape. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route112:coord_events:002` | 26,30 | [Route112_EventScript_VialChanseyEscape](../../baseline/source/data/maps/Route112/scripts.inc#L55) — Coordinate trigger at (26,30); VAR_CHANSEY_NURSE_STATE == 1 invokes Route112_EventScript_VialChanseyEscape. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route112:coord_events:003` | 27,30 | [Route112_EventScript_VialChanseyEscape](../../baseline/source/data/maps/Route112/scripts.inc#L55) — Coordinate trigger at (27,30); VAR_CHANSEY_NURSE_STATE == 1 invokes Route112_EventScript_VialChanseyEscape. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route112:bg_events:001` | 19,44 | [Route112_EventScript_MtChimneySign](../../baseline/source/data/maps/Route112/scripts.inc#L93) — Route112_EventScript_MtChimneySign at (19,44); MT. CHIMNEY //  “For LAVARIDGE TOWN or the summit, /  please take the CABLE CAR.” //  A newer carving points into FIERY PATH: /  a flame, a mane, and TORKOAL's shell. | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01, W-SIGN-LOCAL |
| `Route112:bg_events:002` | 22,37 | [Route112_EventScript_MtChimneyCableCarSign](../../baseline/source/data/maps/Route112/scripts.inc#L89) — Route112_EventScript_MtChimneyCableCarSign at (22,37); MT. CHIMNEY CABLE CAR /  “A short walk {UP_ARROW} way!” | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route112:bg_events:003` | 4,49 | [Route112_EventScript_RouteSignLavaridge](../../baseline/source/data/maps/Route112/scripts.inc#L97) — Route112_EventScript_RouteSignLavaridge at (4,49); ROUTE 112 /  {LEFT_ARROW} LAVARIDGE TOWN | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route112:warp_events:001` | 28,27 | Warp from (28,27, elevation 0) to MAP_ROUTE112_CABLE_CAR_STATION warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:warp_events:002` | 29,27 | Warp from (29,27, elevation 0) to MAP_ROUTE112_CABLE_CAR_STATION warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:warp_events:003` | 6,46 | Warp from (6,46, elevation 3) to MAP_JAGGED_PASS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:warp_events:004` | 7,46 | Warp from (7,46, elevation 3) to MAP_JAGGED_PASS warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:warp_events:005` | 11,36 | Warp from (11,36, elevation 0) to MAP_FIERY_PATH warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:warp_events:006` | 22,10 | Warp from (22,10, elevation 0) to MAP_FIERY_PATH warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:connections:001` | up | up connection to MAP_ROUTE113, offset -60. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:connections:002` | left | left connection to MAP_LAVARIDGE_TOWN, offset 40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:connections:003` | right | right connection to MAP_ROUTE111, offset -20. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route112:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route112_OnTransition](../../baseline/source/data/maps/Route112/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route112_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
