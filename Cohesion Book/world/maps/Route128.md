# Route128

**REVISE.** Preserve the seafloor crisis return coordinates and Steven’s explicit north→west→Dive directions into Sootopolis. This scene cannot substitute for testing the actual underwater/city entrance route.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route128/map.json) · [Scripts](../../baseline/source/data/maps/Route128/scripts.inc)

## Current map contract

`MAP_ROUTE128` · `LAYOUT_ROUTE128` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route128:object_events:001` | 35,33 | [Route128_EventScript_Isaiah](../../baseline/source/data/maps/Route128/scripts.inc#L177) — Route128_EventScript_Isaiah at (35,33); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route128:object_events:002` | 78,24 | [Route128_EventScript_Katelyn](../../baseline/source/data/maps/Route128/scripts.inc#L198) — Route128_EventScript_Katelyn at (78,24); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route128:object_events:003` | 40,22 | Passive/staged OBJ_EVENT_GFX_STEVEN at (40,22); visibility flag FLAG_HIDE_ROUTE_128_STEVEN; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route128:object_events:004` | 37,22 | Passive/staged OBJ_EVENT_GFX_ARCHIE at (37,22); visibility flag FLAG_HIDE_ROUTE_128_ARCHIE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route128:object_events:005` | 38,21 | Passive/staged OBJ_EVENT_GFX_MAXIE at (38,21); visibility flag FLAG_HIDE_ROUTE_128_MAXIE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route128:object_events:006` | 63,28 | [Route128_EventScript_Wayne](../../baseline/source/data/maps/Route128/scripts.inc#L229) — Route128_EventScript_Wayne at (63,28); DHELMISE is not fishing tackle. /  Important lesson. Expensive rod. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route128:object_events:007` | 47,9 | [Route128_EventScript_Ruben](../../baseline/source/data/maps/Route128/scripts.inc#L224) — Route128_EventScript_Ruben at (47,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route128:object_events:008` | 24,8 | [Route128_EventScript_Alexa](../../baseline/source/data/maps/Route128/scripts.inc#L219) — Route128_EventScript_Alexa at (24,8); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route128:object_events:009` | 101,29 | [Route128_EventScript_Carlee](../../baseline/source/data/maps/Route128/scripts.inc#L238) — Route128_EventScript_Carlee at (101,29); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route128:object_events:010` | 101,22 | [Route128_EventScript_Harrison](../../baseline/source/data/maps/Route128/scripts.inc#L233) — Route128_EventScript_Harrison at (101,22); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route128:bg_events:001` | 49,9 | Hidden ITEM_HEART_SCALE at (49,9); persistent flag FLAG_HIDDEN_ITEM_ROUTE_128_HEART_SCALE_1. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route128:bg_events:002` | 57,21 | Hidden ITEM_HEART_SCALE at (57,21); persistent flag FLAG_HIDDEN_ITEM_ROUTE_128_HEART_SCALE_2. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route128:bg_events:003` | 31,33 | Hidden ITEM_HEART_SCALE at (31,33); persistent flag FLAG_HIDDEN_ITEM_ROUTE_128_HEART_SCALE_3. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route128:connections:001` | up | up connection to MAP_ROUTE127, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route128:connections:002` | down | down connection to MAP_ROUTE129, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route128:connections:003` | right | right connection to MAP_EVER_GRANDE_CITY, offset -40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route128:connections:004` | dive | dive connection to MAP_UNDERWATER_ROUTE128, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route128:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route128_OnTransition](../../baseline/source/data/maps/Route128/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls Route128_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route128:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [Route128_OnFrame](../../baseline/source/data/maps/Route128/scripts.inc#L10) — MAP_SCRIPT_ON_FRAME_TABLE calls Route128_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
