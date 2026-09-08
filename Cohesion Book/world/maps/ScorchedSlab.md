# ScorchedSlab

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/ScorchedSlab/map.json) · [Scripts](../../baseline/source/data/maps/ScorchedSlab/scripts.inc)

## Current map contract

`MAP_SCORCHED_SLAB` · `LAYOUT_SCORCHED_SLAB` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ScorchedSlab:object_events:001` | 7,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_SCORCHED_SLAB_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ScorchedSlab:warp_events:001` | 7,16 | Warp from (7,16, elevation 1) to MAP_ROUTE120 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ScorchedSlab:warp_events:002` | 9,3 | Warp from (9,3, elevation 0) to MAP_SCORCHED_SLAB_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ScorchedSlab:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [ScorchedSlab_OnTransition](../../baseline/source/data/maps/ScorchedSlab/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls ScorchedSlab_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
