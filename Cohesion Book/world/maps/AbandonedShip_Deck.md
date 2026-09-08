# AbandonedShip_Deck

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_Deck/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_Deck/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_DECK` · `LAYOUT_ABANDONED_SHIP_DECK` · `WEATHER_NONE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_Deck:warp_events:001` | 13,15 | Warp from (13,15, elevation 3) to MAP_ROUTE108 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Deck:warp_events:002` | 14,15 | Warp from (14,15, elevation 3) to MAP_ROUTE108 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Deck:warp_events:003` | 13,9 | Warp from (13,9, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Deck:warp_events:004` | 8,9 | Warp from (8,9, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Deck:warp_events:005` | 12,5 | Warp from (12,5, elevation 3) to MAP_ABANDONED_SHIP_CAPTAINS_OFFICE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Deck:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [AbandonedShip_Deck_OnTransition](../../baseline/source/data/maps/AbandonedShip_Deck/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls AbandonedShip_Deck_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
