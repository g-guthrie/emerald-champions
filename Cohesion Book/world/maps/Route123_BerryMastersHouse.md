# Route123_BerryMastersHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route123_BerryMastersHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route123_BerryMastersHouse/scripts.inc)

## Current map contract

`MAP_ROUTE123_BERRY_MASTERS_HOUSE` · `LAYOUT_HOUSE2` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route123_BerryMastersHouse:object_events:001` | 4,4 | [Route123_BerryMastersHouse_EventScript_BerryMaster](../../baseline/source/data/maps/Route123_BerryMastersHouse/scripts.inc#L9) — Route123_BerryMastersHouse_EventScript_BerryMaster at (4,4); A good garden always has something /  to share. What brings you here? / Berry gifts | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route123_BerryMastersHouse:object_events:002` | 7,4 | [Route123_BerryMastersHouse_EventScript_BerryMastersWife](../../baseline/source/data/maps/Route123_BerryMastersHouse/scripts.inc#L137) — Route123_BerryMastersHouse_EventScript_BerryMastersWife at (7,4); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route123_BerryMastersHouse:warp_events:001` | 3,7 | Warp from (3,7, elevation 0) to MAP_ROUTE123 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route123_BerryMastersHouse:warp_events:002` | 4,7 | Warp from (4,7, elevation 0) to MAP_ROUTE123 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route123_BerryMastersHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route123_BerryMastersHouse_OnTransition](../../baseline/source/data/maps/Route123_BerryMastersHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route123_BerryMastersHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
