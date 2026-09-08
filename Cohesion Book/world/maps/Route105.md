# Route105

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/Route105/map.json) · [Scripts](../../baseline/source/data/maps/Route105/scripts.inc)

## Current map contract

`MAP_ROUTE105` · `LAYOUT_ROUTE105` · `WEATHER_SUNNY` · `MUS_ROUTE104`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route105:object_events:001` | 19,60 | [Route105_EventScript_Luis](../../baseline/source/data/maps/Route105/scripts.inc#L33) — Route105_EventScript_Luis at (19,60); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route105:object_events:002` | 27,36 | [Route105_EventScript_Dominik](../../baseline/source/data/maps/Route105/scripts.inc#L38) — Route105_EventScript_Dominik at (27,36); I challenged the current. /  It swept the entire match. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route105:object_events:003` | 8,45 | [Route105_EventScript_Beverly](../../baseline/source/data/maps/Route105/scripts.inc#L42) — Route105_EventScript_Beverly at (8,45); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route105:object_events:004` | 19,9 | [Route105_EventScript_Imani](../../baseline/source/data/maps/Route105/scripts.inc#L47) — Route105_EventScript_Imani at (19,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route105:object_events:005` | 7,75 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DUSK_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ROUTE105_DUSK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route105:object_events:006` | 17,48 | [Route105_EventScript_Foster](../../baseline/source/data/maps/Route105/scripts.inc#L28) — Route105_EventScript_Foster at (17,48); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route105:object_events:007` | 4,54 | [Route105_EventScript_Josue](../../baseline/source/data/maps/Route105/scripts.inc#L52) — Route105_EventScript_Josue at (4,54); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route105:object_events:008` | 4,58 | [Route105_EventScript_Andres](../../baseline/source/data/maps/Route105/scripts.inc#L57) — Route105_EventScript_Andres at (4,58); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route105:bg_events:001` | 15,68 | Hidden ITEM_HEART_SCALE at (15,68); persistent flag FLAG_HIDDEN_ITEM_ROUTE_105_HEART_SCALE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route105:bg_events:002` | 5,56 | Hidden ITEM_BIG_PEARL at (5,56); persistent flag FLAG_HIDDEN_ITEM_ROUTE_105_BIG_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route105:warp_events:001` | 9,20 | Warp from (9,20, elevation 0) to MAP_ISLAND_CAVE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route105:connections:001` | up | up connection to MAP_ROUTE104, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route105:connections:002` | down | down connection to MAP_ROUTE106, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route105:connections:003` | dive | dive connection to MAP_UNDERWATER_ROUTE105, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route105:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [Route105_OnLoad](../../baseline/source/data/maps/Route105/scripts.inc#L7) — MAP_SCRIPT_ON_LOAD calls Route105_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route105:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Route105_OnTransition](../../baseline/source/data/maps/Route105/scripts.inc#L18) — MAP_SCRIPT_ON_TRANSITION calls Route105_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route105:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route105_OnFrame](../../baseline/source/data/maps/Route105/scripts.inc#L24) — MAP_SCRIPT_ON_FRAME_TABLE calls Route105_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
