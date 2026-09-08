# EverGrandeCity_DrakesRoom

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_DrakesRoom/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_DrakesRoom/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_DRAKES_ROOM` · `LAYOUT_EVER_GRANDE_CITY_DRAKES_ROOM` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_DrakesRoom:object_events:001` | 6,5 | [EverGrandeCity_DrakesRoom_EventScript_Drake](../../baseline/source/data/maps/EverGrandeCity_DrakesRoom/scripts.inc#L40) — EverGrandeCity_DrakesRoom_EventScript_Drake at (6,5); DRAKE. Last of the four, and the one /  who's sailed the longest. //  Dragons remember every age of battle. /  DIALGA runs the clock backwards, /  RESHIRAM runs it forward, and two of /  mine set up while you decide which. //  Kill the first clock. Then meet /  SALAMENCE with something left. Come on! / Hah! Both clocks, and you still stood. /  Superb, Trainer. Superb. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `EverGrandeCity_DrakesRoom:warp_events:001` | 6,13 | Warp from (6,13, elevation 3) to MAP_EVER_GRANDE_CITY_HALL3 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_DrakesRoom:warp_events:002` | 6,2 | Warp from (6,2, elevation 0) to MAP_EVER_GRANDE_CITY_HALL4 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_DrakesRoom:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [EverGrandeCity_DrakesRoom_OnFrame](../../baseline/source/data/maps/EverGrandeCity_DrakesRoom/scripts.inc#L16) — MAP_SCRIPT_ON_FRAME_TABLE calls EverGrandeCity_DrakesRoom_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_DrakesRoom:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [EverGrandeCity_DrakesRoom_OnLoad](../../baseline/source/data/maps/EverGrandeCity_DrakesRoom/scripts.inc#L27) — MAP_SCRIPT_ON_LOAD calls EverGrandeCity_DrakesRoom_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_DrakesRoom:map_scripts:003` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [EverGrandeCity_SidneysRoom_OnWarp](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L26) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls EverGrandeCity_SidneysRoom_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
