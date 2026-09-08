# EverGrandeCity_Hall2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_Hall2/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_Hall2/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_HALL2` · `LAYOUT_EVER_GRANDE_CITY_SHORT_HALL` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_Hall2:warp_events:001` | 5,12 | Warp from (5,12, elevation 3) to MAP_EVER_GRANDE_CITY_PHOEBES_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_Hall2:warp_events:002` | 5,2 | Warp from (5,2, elevation 0) to MAP_EVER_GRANDE_CITY_GLACIAS_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_Hall2:warp_events:003` | 4,12 | Warp from (4,12, elevation 3) to MAP_EVER_GRANDE_CITY_PHOEBES_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_Hall2:warp_events:004` | 6,12 | Warp from (6,12, elevation 3) to MAP_EVER_GRANDE_CITY_PHOEBES_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_Hall2:map_scripts:001` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [EverGrandeCity_Hall2_OnWarp](../../baseline/source/data/maps/EverGrandeCity_Hall2/scripts.inc#L5) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls EverGrandeCity_Hall2_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
