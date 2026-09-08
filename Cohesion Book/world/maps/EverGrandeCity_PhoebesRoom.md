# EverGrandeCity_PhoebesRoom

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_PhoebesRoom/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_PhoebesRoom/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_PHOEBES_ROOM` · `LAYOUT_EVER_GRANDE_CITY_PHOEBES_ROOM` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_PhoebesRoom:object_events:001` | 6,5 | [EverGrandeCity_PhoebesRoom_EventScript_Phoebe](../../baseline/source/data/maps/EverGrandeCity_PhoebesRoom/scripts.inc#L39) — EverGrandeCity_PhoebesRoom_EventScript_Phoebe at (6,5); Welcome. I am PHOEBE. //  I trained on MT. PYRE, where the ones /  who are gone still take part. //  My friends borrow your boosts, shelter /  from your spread attacks, and one empty /  shell will swallow your best move. //  Keep something for the rider in the /  shadow. It is always closer than it looks. / You honored what you could not touch. /  My grandmother would like you. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `EverGrandeCity_PhoebesRoom:warp_events:001` | 6,13 | Warp from (6,13, elevation 3) to MAP_EVER_GRANDE_CITY_HALL1 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PhoebesRoom:warp_events:002` | 6,2 | Warp from (6,2, elevation 0) to MAP_EVER_GRANDE_CITY_HALL2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_PhoebesRoom:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [EverGrandeCity_PhoebesRoom_OnLoad](../../baseline/source/data/maps/EverGrandeCity_PhoebesRoom/scripts.inc#L26) — MAP_SCRIPT_ON_LOAD calls EverGrandeCity_PhoebesRoom_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_PhoebesRoom:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [EverGrandeCity_PhoebesRoom_OnWarp](../../baseline/source/data/maps/EverGrandeCity_PhoebesRoom/scripts.inc#L7) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls EverGrandeCity_PhoebesRoom_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_PhoebesRoom:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [EverGrandeCity_PhoebesRoom_OnFrame](../../baseline/source/data/maps/EverGrandeCity_PhoebesRoom/scripts.inc#L15) — MAP_SCRIPT_ON_FRAME_TABLE calls EverGrandeCity_PhoebesRoom_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
