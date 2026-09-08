# DesertUnderpass

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/DesertUnderpass/map.json) · [Scripts](../../baseline/source/data/maps/DesertUnderpass/scripts.inc)

## Current map contract

`MAP_DESERT_UNDERPASS` · `LAYOUT_DESERT_UNDERPASS` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `DesertUnderpass:object_events:001` | 132,10 | [DesertUnderpass_EventScript_Fossil](../../baseline/source/data/maps/DesertUnderpass/scripts.inc#L9) — DesertUnderpass_EventScript_Fossil at (132,10); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `DesertUnderpass:object_events:002` | 107,3 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MASTER_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_EMBER_MASTER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DesertUnderpass:object_events:003` | 12,9 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (12,9); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `DesertUnderpass:warp_events:001` | 10,12 | Warp from (10,12, elevation 0) to MAP_ROUTE114_FOSSIL_MANIACS_TUNNEL warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DesertUnderpass:warp_events:002` | 132,15 | Warp from (132,15, elevation 0) to MAP_SANDSTREWN_RUINS warp 24. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DesertUnderpass:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [DesertUnderpass_OnTransition](../../baseline/source/data/maps/DesertUnderpass/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls DesertUnderpass_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
