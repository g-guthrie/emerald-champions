# Route126

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route126/map.json) · [Scripts](../../baseline/source/data/maps/Route126/scripts.inc)

## Current map contract

`MAP_ROUTE126` · `LAYOUT_ROUTE126` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route126:object_events:001` | 51,65 | [Route126_EventScript_Barry](../../baseline/source/data/maps/Route126/scripts.inc#L9) — Route126_EventScript_Barry at (51,65); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route126:object_events:002` | 56,22 | [Route126_EventScript_Dean](../../baseline/source/data/maps/Route126/scripts.inc#L14) — Route126_EventScript_Dean at (56,22); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route126:object_events:003` | 63,43 | [Route126_EventScript_Nikki](../../baseline/source/data/maps/Route126/scripts.inc#L19) — Route126_EventScript_Nikki at (63,43); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route126:object_events:004` | 9,48 | [Route126_EventScript_Brenda](../../baseline/source/data/maps/Route126/scripts.inc#L24) — Route126_EventScript_Brenda at (9,48); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route126:object_events:005` | 14,1 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GREEN_SHARD; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_126_GREEN_SHARD. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route126:object_events:006` | 15,66 | [Route126_EventScript_Sienna](../../baseline/source/data/maps/Route126/scripts.inc#L37) — Route126_EventScript_Sienna at (15,66); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route126:object_events:007` | 7,66 | [Route126_EventScript_Pablo](../../baseline/source/data/maps/Route126/scripts.inc#L42) — Route126_EventScript_Pablo at (7,66); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route126:object_events:008` | 64,5 | [Route126_EventScript_Isobel](../../baseline/source/data/maps/Route126/scripts.inc#L33) — Route126_EventScript_Isobel at (64,5); My warm-up was a little long. /  I think I crossed a time zone. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route126:object_events:009` | 56,5 | [Route126_EventScript_Leonardo](../../baseline/source/data/maps/Route126/scripts.inc#L29) — Route126_EventScript_Leonardo at (56,5); I was racing a FLOATZEL. /  Now I am enjoying a quiet swim. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route126:object_events:010` | 7,71 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (7,71); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route126:connections:001` | up | up connection to MAP_ROUTE124, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route126:connections:002` | right | right connection to MAP_ROUTE127, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route126:connections:003` | dive | dive connection to MAP_UNDERWATER_ROUTE126, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route126:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route126_OnTransition](../../baseline/source/data/maps/Route126/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route126_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
