# Route108

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/Route108/map.json) · [Scripts](../../baseline/source/data/maps/Route108/scripts.inc)

## Current map contract

`MAP_ROUTE108` · `LAYOUT_ROUTE108` · `WEATHER_SUNNY` · `MUS_ROUTE104`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route108:object_events:001` | 52,13 | [Route108_EventScript_Jerome](../../baseline/source/data/maps/Route108/scripts.inc#L4) — Route108_EventScript_Jerome at (52,13); I counted every wave out here. /  Then somebody used SURF. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route108:object_events:002` | 35,12 | [Route108_EventScript_Tara](../../baseline/source/data/maps/Route108/scripts.inc#L12) — Route108_EventScript_Tara at (35,12); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route108:object_events:003` | 13,13 | [Route108_EventScript_Matthew](../../baseline/source/data/maps/Route108/scripts.inc#L8) — Route108_EventScript_Matthew at (13,13); MANTINE makes this look easy. /  I would also like some wings. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route108:object_events:004` | 8,7 | [Route108_EventScript_Missy](../../baseline/source/data/maps/Route108/scripts.inc#L17) — Route108_EventScript_Missy at (8,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route108:object_events:005` | 41,5 | [Route108_EventScript_Carolina](../../baseline/source/data/maps/Route108/scripts.inc#L22) — Route108_EventScript_Carolina at (41,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route108:object_events:006` | 43,5 | [Route108_EventScript_Cory](../../baseline/source/data/maps/Route108/scripts.inc#L27) — Route108_EventScript_Cory at (43,5); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route108:object_events:007` | 42,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_STAR_PIECE; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_108_STAR_PIECE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route108:bg_events:001` | 38,14 | Hidden ITEM_ULTRA_BALL at (38,14); persistent flag FLAG_HIDDEN_ITEM_ROUTE_108_ULTRA_BALL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route108:warp_events:001` | 29,6 | Warp from (29,6, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route108:connections:001` | left | left connection to MAP_ROUTE107, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route108:connections:002` | right | right connection to MAP_ROUTE109, offset -40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
