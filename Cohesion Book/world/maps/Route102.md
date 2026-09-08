# Route102

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route102/map.json) · [Scripts](../../baseline/source/data/maps/Route102/scripts.inc)

## Current map contract

`MAP_ROUTE102` · `LAYOUT_ROUTE102` · `WEATHER_SUNNY` · `MUS_ROUTE101`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route102:object_events:001` | 18,11 | [Route102_EventScript_LittleBoy](../../baseline/source/data/maps/Route102/scripts.inc#L4) — Route102_EventScript_LittleBoy at (18,11); I'm…not very tall, so I sink right /  into tall grass. //  The grass goes up my nose and… /  Fwafwafwafwafwa… //  Fwatchoo! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route102:object_events:002` | 33,14 | [Route102_EventScript_Calvin](../../baseline/source/data/maps/Route102/scripts.inc#L20) — Route102_EventScript_Calvin at (33,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route102:object_events:003` | 25,15 | [Route102_EventScript_Rick](../../baseline/source/data/maps/Route102/scripts.inc#L61) — Route102_EventScript_Rick at (25,15); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route102:object_events:004` | 8,7 | [Route102_EventScript_Tiana](../../baseline/source/data/maps/Route102/scripts.inc#L66) — Route102_EventScript_Tiana at (8,7); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route102:object_events:005` | 37,4 | [Route102_EventScript_Boy](../../baseline/source/data/maps/Route102/scripts.inc#L16) — Route102_EventScript_Boy at (37,4); I'm going to catch a whole bunch of /  POKéMON! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route102:object_events:006` | 11,15 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_POTION; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_102_POTION. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route102:object_events:007` | 24,2 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (24,2); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route102:object_events:008` | 25,2 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (25,2); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route102:object_events:009` | 19,4 | [Route102_EventScript_Allen](../../baseline/source/data/maps/Route102/scripts.inc#L71) — Route102_EventScript_Allen at (19,4); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route102:bg_events:001` | 17,2 | [Route102_EventScript_RouteSignPetalburg](../../baseline/source/data/maps/Route102/scripts.inc#L12) — Route102_EventScript_RouteSignPetalburg at (17,2); ROUTE 102 /  {LEFT_ARROW} PETALBURG CITY | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route102:bg_events:002` | 40,9 | [Route102_EventScript_RouteSignOldale](../../baseline/source/data/maps/Route102/scripts.inc#L8) — Route102_EventScript_RouteSignOldale at (40,9); ROUTE 102 /  {RIGHT_ARROW} OLDALE TOWN | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route102:connections:001` | left | left connection to MAP_PETALBURG_CITY, offset -10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route102:connections:002` | right | right connection to MAP_OLDALE_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
