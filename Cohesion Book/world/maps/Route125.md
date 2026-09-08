# Route125

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route125/map.json) · [Scripts](../../baseline/source/data/maps/Route125/scripts.inc)

## Current map contract

`MAP_ROUTE125` · `LAYOUT_ROUTE125` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route125:object_events:001` | 7,31 | [Route125_EventScript_Nolen](../../baseline/source/data/maps/Route125/scripts.inc#L23) — Route125_EventScript_Nolen at (7,31); I know a shortcut to the shore. /  First, decide which shore. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route125:object_events:002` | 45,9 | [Route125_EventScript_Stan](../../baseline/source/data/maps/Route125/scripts.inc#L27) — Route125_EventScript_Stan at (45,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:003` | 38,24 | [Route125_EventScript_Tanya](../../baseline/source/data/maps/Route125/scripts.inc#L32) — Route125_EventScript_Tanya at (38,24); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:004` | 30,28 | [Route125_EventScript_Sharon](../../baseline/source/data/maps/Route125/scripts.inc#L37) — Route125_EventScript_Sharon at (30,28); SLOWBRO never seems rushed. /  I am trying to learn from that. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route125:object_events:005` | 21,30 | [Route125_EventScript_Ernest](../../baseline/source/data/maps/Route125/scripts.inc#L41) — Route125_EventScript_Ernest at (21,30); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:006` | 17,19 | [Route125_EventScript_Kim](../../baseline/source/data/maps/Route125/scripts.inc#L62) — Route125_EventScript_Kim at (17,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:007` | 18,19 | [Route125_EventScript_Iris](../../baseline/source/data/maps/Route125/scripts.inc#L67) — Route125_EventScript_Iris at (18,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:008` | 43,19 | [Route125_EventScript_Presley](../../baseline/source/data/maps/Route125/scripts.inc#L72) — Route125_EventScript_Presley at (43,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:009` | 48,19 | [Route125_EventScript_Auron](../../baseline/source/data/maps/Route125/scripts.inc#L77) — Route125_EventScript_Auron at (48,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route125:object_events:010` | 46,17 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BIG_PEARL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_125_BIG_PEARL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route125:object_events:011` | 20,22 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (20,22); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route125:bg_events:001` | 53,10 | None at (53,10); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route125:bg_events:002` | 55,11 | None at (55,11); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route125:bg_events:003` | 7,25 | None at (7,25); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route125:bg_events:004` | 24,32 | None at (24,32); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route125:warp_events:001` | 22,19 | Warp from (22,19, elevation 0) to MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route125:connections:001` | down | down connection to MAP_MOSSDEEP_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route125:connections:002` | left | left connection to MAP_ROUTE124, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route125:connections:003` | dive | dive connection to MAP_UNDERWATER_ROUTE125, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route125:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route125_OnTransition](../../baseline/source/data/maps/Route125/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls Route125_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route125:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route125_OnLoad](../../baseline/source/data/maps/Route125/scripts.inc#L14) — MAP_SCRIPT_ON_LOAD calls Route125_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route125:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route125_OnFrame](../../baseline/source/data/maps/Route125/scripts.inc#L19) — MAP_SCRIPT_ON_FRAME_TABLE calls Route125_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
