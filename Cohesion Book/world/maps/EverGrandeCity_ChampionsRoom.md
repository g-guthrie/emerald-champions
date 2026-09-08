# EverGrandeCity_ChampionsRoom

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity_ChampionsRoom/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity_ChampionsRoom/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY_CHAMPIONS_ROOM` · `LAYOUT_EVER_GRANDE_CITY_CHAMPIONS_ROOM` · `WEATHER_NONE` · `MUS_VICTORY_ROAD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity_ChampionsRoom:object_events:001` | 6,5 | [EverGrandeCity_ChampionsRoom_EventScript_Wallace](../../baseline/source/data/maps/EverGrandeCity_ChampionsRoom/scripts.inc#L40) — EverGrandeCity_ChampionsRoom_EventScript_Wallace at (6,5); WALLACE: Welcome, {PLAYER}. //  You crossed HOENN with every tool made /  available: moves, items, rare partners, /  and the power of Mega Evolution. //  Nothing was hidden behind grinding. Every /  loss asked you to build a better answer. //  You restored SOOTOPOLIS's balance and /  beat the ELITE FOUR without healing /  inside a single battle. //  Now face the region's complete exam. My /  team shifts rhythm until yours breaks. //  Show me the Champion you became! / Every time I changed the question, you /  found another answer. Magnificent. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `EverGrandeCity_ChampionsRoom:object_events:002` | 6,12 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (6,12); visibility flag FLAG_HIDE_CHAMPIONS_ROOM_RIVAL; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `EverGrandeCity_ChampionsRoom:object_events:003` | 6,12 | Passive/staged OBJ_EVENT_GFX_PROF_BIRCH at (6,12); visibility flag FLAG_HIDE_CHAMPIONS_ROOM_BIRCH; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `EverGrandeCity_ChampionsRoom:warp_events:001` | 6,12 | Warp from (6,12, elevation 3) to MAP_EVER_GRANDE_CITY_HALL4 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_ChampionsRoom:warp_events:002` | 6,2 | Warp from (6,2, elevation 0) to MAP_EVER_GRANDE_CITY_HALL_OF_FAME warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity_ChampionsRoom:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [EverGrandeCity_ChampionsRoom_OnTransition](../../baseline/source/data/maps/EverGrandeCity_ChampionsRoom/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls EverGrandeCity_ChampionsRoom_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_ChampionsRoom:map_scripts:002` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [EverGrandeCity_ChampionsRoom_OnWarp](../../baseline/source/data/maps/EverGrandeCity_ChampionsRoom/scripts.inc#L11) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls EverGrandeCity_ChampionsRoom_OnWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `EverGrandeCity_ChampionsRoom:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [EverGrandeCity_ChampionsRoom_OnFrame](../../baseline/source/data/maps/EverGrandeCity_ChampionsRoom/scripts.inc#L19) — MAP_SCRIPT_ON_FRAME_TABLE calls EverGrandeCity_ChampionsRoom_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
