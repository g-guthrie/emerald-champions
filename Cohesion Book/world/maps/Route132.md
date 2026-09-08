# Route132

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route132/map.json) · [Scripts](../../baseline/source/data/maps/Route132/scripts.inc)

## Current map contract

`MAP_ROUTE132` · `LAYOUT_ROUTE132` · `WEATHER_SUNNY` · `MUS_ROUTE119`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route132:object_events:001` | 40,13 | [Route132_EventScript_Gilbert](../../baseline/source/data/maps/Route132/scripts.inc#L4) — Route132_EventScript_Gilbert at (40,13); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:002` | 10,6 | [Route132_EventScript_Dana](../../baseline/source/data/maps/Route132/scripts.inc#L9) — Route132_EventScript_Dana at (10,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:003` | 10,11 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_132_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route132:object_events:004` | 9,15 | [Route132_EventScript_Kiyo](../../baseline/source/data/maps/Route132/scripts.inc#L19) — Route132_EventScript_Kiyo at (9,15); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:005` | 49,28 | [Route132_EventScript_Ronald](../../baseline/source/data/maps/Route132/scripts.inc#L14) — Route132_EventScript_Ronald at (49,28); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:006` | 33,26 | [Route132_EventScript_Paxton](../../baseline/source/data/maps/Route132/scripts.inc#L24) — Route132_EventScript_Paxton at (33,26); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:007` | 33,31 | [Route132_EventScript_Darcy](../../baseline/source/data/maps/Route132/scripts.inc#L29) — Route132_EventScript_Darcy at (33,31); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:008` | 21,30 | [Route132_EventScript_Makayla](../../baseline/source/data/maps/Route132/scripts.inc#L39) — Route132_EventScript_Makayla at (21,30); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:009` | 21,25 | [Route132_EventScript_Jonathan](../../baseline/source/data/maps/Route132/scripts.inc#L34) — Route132_EventScript_Jonathan at (21,25); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route132:object_events:010` | 20,27 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_QUICK_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_132_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route132:connections:001` | left | left connection to MAP_ROUTE133, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route132:connections:002` | right | right connection to MAP_PACIFIDLOG_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
