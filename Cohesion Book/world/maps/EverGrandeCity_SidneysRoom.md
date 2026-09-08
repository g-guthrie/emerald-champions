# EverGrandeCity_SidneysRoom

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_SIDNEYS_ROOM` · `LAYOUT_EVER_GRANDE_CITY_SIDNEYS_ROOM` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_SidneysRoom:object_events:001` | 6,5 | [EverGrandeCity_SidneysRoom_EventScript_Sidney](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L45) — EverGrandeCity_SidneysRoom_EventScript_Sidney at (6,5); Heh. SIDNEY. First door, best door. //  My crew fights dirty. Fake Out, sleep, /  a fast board that goes slow when you /  blink. You'll think you've read it. //  Then ABSOL shows up and the reading /  was wrong. Let's go, kid! / Ha! You saw every trick coming. Fine. /  That was a good time. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `EverGrandeCity_SidneysRoom:warp_events:001` | 6,13 | Warp from (6,13, elevation 3) to MAP_EVER_GRANDE_CITY_HALL5 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_SidneysRoom:warp_events:002` | 6,2 | Warp from (6,2, elevation 0) to MAP_EVER_GRANDE_CITY_HALL1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_SidneysRoom:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [EverGrandeCity_SidneysRoom_OnLoad](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L13) — MAP_SCRIPT_ON_LOAD calls EverGrandeCity_SidneysRoom_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_SidneysRoom:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [EverGrandeCity_SidneysRoom_OnWarp](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L26) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls EverGrandeCity_SidneysRoom_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_SidneysRoom:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [EverGrandeCity_SidneysRoom_OnTransition](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L8) — MAP_SCRIPT_ON_TRANSITION calls EverGrandeCity_SidneysRoom_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_SidneysRoom:map_scripts:004` | MAP_SCRIPT_ON_FRAME_TABLE | [EverGrandeCity_SidneysRoom_OnFrame](../../baseline/source/data/maps/EverGrandeCity_SidneysRoom/scripts.inc#L34) — MAP_SCRIPT_ON_FRAME_TABLE calls EverGrandeCity_SidneysRoom_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
