# Route110_TrickHouseCorridor

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHouseCorridor/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHouseCorridor/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_CORRIDOR` · `LAYOUT_ROUTE110_TRICK_HOUSE_CORRIDOR` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHouseCorridor:warp_events:001` | 13,3 | Warp from (13,3, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseCorridor:warp_events:002` | 14,3 | Warp from (14,3, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseCorridor:warp_events:003` | 4,23 | Warp from (4,23, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseCorridor:warp_events:004` | 5,23 | Warp from (5,23, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHouseCorridor:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route110_TrickHouseCorridor_OnTransition](../../baseline/source/data/maps/Route110_TrickHouseCorridor/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route110_TrickHouseCorridor_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
