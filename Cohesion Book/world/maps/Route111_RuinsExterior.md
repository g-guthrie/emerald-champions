# Route111_RuinsExterior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route111_RuinsExterior/map.json) · [Scripts](../../baseline/source/data/maps/Route111_RuinsExterior/scripts.inc)

## Current map contract

`MAP_ROUTE111_RUINS_EXTERIOR` · `LAYOUT_ROUTE111_RUINS_EXTERIOR` · `WEATHER_NONE` · `MUS_ROUTE110`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route111_RuinsExterior:object_events:001` | 10,22 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_STEELIXITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_STEELIXITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route111_RuinsExterior:object_events:002` | 8,11 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (8,11); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Route111_RuinsExterior:warp_events:001` | 13,11 | Warp from (13,11, elevation 0) to MAP_SANDSTREWN_RUINS_3F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
