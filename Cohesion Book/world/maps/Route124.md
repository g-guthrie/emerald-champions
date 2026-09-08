# Route124

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route124/map.json) · [Scripts](../../baseline/source/data/maps/Route124/scripts.inc)

## Current map contract

`MAP_ROUTE124` · `LAYOUT_ROUTE124` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route124:object_events:001` | 34,25 | [Route124_EventScript_Spencer](../../baseline/source/data/maps/Route124/scripts.inc#L13) — Route124_EventScript_Spencer at (34,25); I keep a map in my swim trunks. /  It is mostly ocean now. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route124:object_events:002` | 61,74 | [Route124_EventScript_Roland](../../baseline/source/data/maps/Route124/scripts.inc#L17) — Route124_EventScript_Roland at (61,74); CLAWITZER is a powerful swimmer. /  The brakes could use some work. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route124:object_events:003` | 49,45 | [Route124_EventScript_Jenny](../../baseline/source/data/maps/Route124/scripts.inc#L21) — Route124_EventScript_Jenny at (49,45); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route124:object_events:004` | 7,23 | [Route124_EventScript_Grace](../../baseline/source/data/maps/Route124/scripts.inc#L42) — Route124_EventScript_Grace at (7,23); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route124:object_events:005` | 58,58 | [Route124_EventScript_Chad](../../baseline/source/data/maps/Route124/scripts.inc#L47) — Route124_EventScript_Chad at (58,58); STOUTLAND found my lost goggles. /  I was wearing them. Good dog. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route124:object_events:006` | 28,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_RED_SHARD; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_124_RED_SHARD. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route124:object_events:007` | 31,53 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BLUE_SHARD; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_124_BLUE_SHARD. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route124:object_events:008` | 58,11 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_YELLOW_SHARD; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_124_YELLOW_SHARD. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route124:object_events:009` | 18,44 | [Route124_EventScript_Lila](../../baseline/source/data/maps/Route124/scripts.inc#L51) — Route124_EventScript_Lila at (18,44); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route124:object_events:010` | 17,44 | [Route124_EventScript_Roy](../../baseline/source/data/maps/Route124/scripts.inc#L70) — Route124_EventScript_Roy at (17,44); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route124:object_events:011` | 7,29 | [Route124_EventScript_Declan](../../baseline/source/data/maps/Route124/scripts.inc#L89) — Route124_EventScript_Declan at (7,29); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route124:object_events:012` | 69,74 | [Route124_EventScript_Isabella](../../baseline/source/data/maps/Route124/scripts.inc#L94) — Route124_EventScript_Isabella at (69,74); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route124:object_events:013` | 37,63 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_WEPEAR_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_ROUTE124_WEPEAR_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route124:bg_events:001` | 73,48 | [Route124_EventScript_HuntersHouseSign](../../baseline/source/data/maps/Route124/scripts.inc#L9) — Route124_EventScript_HuntersHouseSign at (73,48); HUNTER'S HOUSE | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route124:warp_events:001` | 70,48 | Warp from (70,48, elevation 3) to MAP_ROUTE124_DIVING_TREASURE_HUNTERS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124:connections:001` | down | down connection to MAP_ROUTE126, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124:connections:002` | left | left connection to MAP_LILYCOVE_CITY, offset 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124:connections:003` | right | right connection to MAP_ROUTE125, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124:connections:004` | right | right connection to MAP_MOSSDEEP_CITY, offset 40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124:connections:005` | dive | dive connection to MAP_UNDERWATER_ROUTE124, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route124_OnTransition](../../baseline/source/data/maps/Route124/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route124_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
