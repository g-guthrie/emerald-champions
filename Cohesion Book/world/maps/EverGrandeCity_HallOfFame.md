# EverGrandeCity_HallOfFame

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_HallOfFame/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_HallOfFame/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_HALL_OF_FAME` · `LAYOUT_EVER_GRANDE_CITY_HALL_OF_FAME` · `WEATHER_NONE` · `MUS_HALL_OF_FAME_ROOM`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_HallOfFame:object_events:001` | 6,16 | Passive/staged OBJ_EVENT_GFX_WALLACE at (6,16); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `EverGrandeCity_HallOfFame:warp_events:001` | 7,11 | Warp from (7,11, elevation 3) to MAP_EVER_GRANDE_CITY_CHAMPIONS_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_HallOfFame:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [EverGrandeCity_HallOfFame_OnFrame](../../baseline/source/data/maps/EverGrandeCity_HallOfFame/scripts.inc#L14) — MAP_SCRIPT_ON_FRAME_TABLE calls EverGrandeCity_HallOfFame_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_HallOfFame:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [EverGrandeCity_HallOfFame_OnWarp](../../baseline/source/data/maps/EverGrandeCity_HallOfFame/scripts.inc#L6) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls EverGrandeCity_HallOfFame_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
