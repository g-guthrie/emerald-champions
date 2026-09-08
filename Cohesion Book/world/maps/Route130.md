# Route130

**REVISE.** Mirage Island stays an optional mystery. Do not make its personality/day roll the sole source of any battle-relevant family; broad acquisition alternatives belong to the global catalogue.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route130/map.json) · [Scripts](../../baseline/source/data/maps/Route130/scripts.inc)

## Current map contract

`MAP_ROUTE130` · `LAYOUT_ROUTE130` · `WEATHER_SUNNY` · `MUS_ROUTE119`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route130:object_events:001` | 70,21 | [Route130_EventScript_Rodney](../../baseline/source/data/maps/Route130/scripts.inc#L36) — Route130_EventScript_Rodney at (70,21); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route130:object_events:002` | 7,21 | [Route130_EventScript_Katie](../../baseline/source/data/maps/Route130/scripts.inc#L41) — Route130_EventScript_Katie at (7,21); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route130:object_events:003` | 52,9 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (52,9); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route130:object_events:004` | 7,30 | [Route130_EventScript_Santiago](../../baseline/source/data/maps/Route130/scripts.inc#L46) — Route130_EventScript_Santiago at (7,30); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route130:connections:001` | left | left connection to MAP_ROUTE131, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route130:connections:002` | right | right connection to MAP_ROUTE129, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route130:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route130_OnTransition](../../baseline/source/data/maps/Route130/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route130_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
