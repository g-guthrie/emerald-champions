# Route127

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route127/map.json) · [Scripts](../../baseline/source/data/maps/Route127/scripts.inc)

## Current map contract

`MAP_ROUTE127` · `LAYOUT_ROUTE127` · `WEATHER_SUNNY` · `MUS_ROUTE120`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route127:object_events:001` | 45,42 | [Route127_EventScript_Camden](../../baseline/source/data/maps/Route127/scripts.inc#L23) — Route127_EventScript_Camden at (45,42); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route127:object_events:002` | 18,68 | [Route127_EventScript_Donny](../../baseline/source/data/maps/Route127/scripts.inc#L28) — Route127_EventScript_Donny at (18,68); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route127:object_events:003` | 14,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_NET_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_127_NET_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route127:object_events:004` | 64,39 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DIVE_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_127_DIVE_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route127:object_events:005` | 42,21 | [Route127_EventScript_Jonah](../../baseline/source/data/maps/Route127/scripts.inc#L33) — Route127_EventScript_Jonah at (42,21); Fishing teaches patience. /  This spot has a full curriculum. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route127:object_events:006` | 64,19 | [Route127_EventScript_Roger](../../baseline/source/data/maps/Route127/scripts.inc#L42) — Route127_EventScript_Roger at (64,19); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route127:object_events:007` | 54,14 | [Route127_EventScript_Henry](../../baseline/source/data/maps/Route127/scripts.inc#L37) — Route127_EventScript_Henry at (54,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route127:object_events:008` | 15,23 | [Route127_EventScript_Aidan](../../baseline/source/data/maps/Route127/scripts.inc#L47) — Route127_EventScript_Aidan at (15,23); Birds never need a boarding pass. /  I think they are showing off. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route127:object_events:009` | 63,63 | [Route127_EventScript_Koji](../../baseline/source/data/maps/Route127/scripts.inc#L56) — Route127_EventScript_Koji at (63,63); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route127:object_events:010` | 12,23 | [Route127_EventScript_Athena](../../baseline/source/data/maps/Route127/scripts.inc#L51) — Route127_EventScript_Athena at (12,23); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route127:object_events:011` | 13,20 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_127_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route127:object_events:012` | 77,41 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (77,41); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route127:bg_events:001` | 59,67 | None at (59,67); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route127:bg_events:002` | 59,72 | None at (59,72); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route127:bg_events:003` | 67,63 | None at (67,63); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route127:bg_events:004` | 61,21 | None at (61,21); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route127:bg_events:005` | 45,24 | None at (45,24); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route127:connections:001` | up | up connection to MAP_MOSSDEEP_CITY, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route127:connections:002` | down | down connection to MAP_ROUTE128, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route127:connections:003` | left | left connection to MAP_ROUTE126, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route127:connections:004` | dive | dive connection to MAP_UNDERWATER_ROUTE127, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route127:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route127_OnTransition](../../baseline/source/data/maps/Route127/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls Route127_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route127:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route127_OnLoad](../../baseline/source/data/maps/Route127/scripts.inc#L14) — MAP_SCRIPT_ON_LOAD calls Route127_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route127:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route127_OnFrame](../../baseline/source/data/maps/Route127/scripts.inc#L19) — MAP_SCRIPT_ON_FRAME_TABLE calls Route127_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
