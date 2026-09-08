# Route131

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route131/map.json) · [Scripts](../../baseline/source/data/maps/Route131/scripts.inc)

## Current map contract

`MAP_ROUTE131` · `LAYOUT_ROUTE131` · `WEATHER_SUNNY` · `MUS_ROUTE119`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route131:object_events:001` | 25,32 | [Route131_EventScript_Richard](../../baseline/source/data/maps/Route131/scripts.inc#L18) — Route131_EventScript_Richard at (25,32); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:002` | 40,21 | [Route131_EventScript_Herman](../../baseline/source/data/maps/Route131/scripts.inc#L23) — Route131_EventScript_Herman at (40,21); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:003` | 22,21 | [Route131_EventScript_Kara](../../baseline/source/data/maps/Route131/scripts.inc#L33) — Route131_EventScript_Kara at (22,21); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:004` | 44,21 | [Route131_EventScript_Susie](../../baseline/source/data/maps/Route131/scripts.inc#L28) — Route131_EventScript_Susie at (44,21); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:005` | 9,16 | [Route131_EventScript_Reli](../../baseline/source/data/maps/Route131/scripts.inc#L38) — Route131_EventScript_Reli at (9,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:006` | 8,16 | [Route131_EventScript_Ian](../../baseline/source/data/maps/Route131/scripts.inc#L43) — Route131_EventScript_Ian at (8,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:007` | 52,20 | [Route131_EventScript_Kevin](../../baseline/source/data/maps/Route131/scripts.inc#L53) — Route131_EventScript_Kevin at (52,20); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:008` | 52,27 | [Route131_EventScript_Talia](../../baseline/source/data/maps/Route131/scripts.inc#L48) — Route131_EventScript_Talia at (52,27); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route131:object_events:009` | 42,20 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BEAST_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ROUTE131_BEAST_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route131:warp_events:001` | 36,6 | Warp from (36,6, elevation 3) to MAP_SKY_PILLAR_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route131:connections:001` | left | left connection to MAP_PACIFIDLOG_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route131:connections:002` | right | right connection to MAP_ROUTE130, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route131:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route131_OnTransition](../../baseline/source/data/maps/Route131/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route131_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
