# EverGrandeCity

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../07-league.md) · [Map source](../../baseline/source/data/maps/EverGrandeCity/map.json) · [Scripts](../../baseline/source/data/maps/EverGrandeCity/scripts.inc)

## Current map contract

`MAP_EVER_GRANDE_CITY` · `LAYOUT_EVER_GRANDE_CITY` · `WEATHER_SUNNY` · `MUS_EVER_GRANDE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EverGrandeCity:object_events:001` | 23,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BEAST_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_EVER_GRANDE_BEAST_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `EverGrandeCity:coord_events:001` | 17,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (17,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:002` | 16,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (16,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:003` | 18,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (18,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:004` | 19,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (19,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:005` | 20,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (20,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:006` | 21,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (21,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:007` | 22,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (22,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:008` | 23,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (23,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:009` | 24,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (24,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:010` | 25,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (25,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:coord_events:011` | 26,58 | [EverGrandeCity_EventScript_SetVisitedEverGrande](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L21) — Coordinate trigger at (26,58); VAR_TEMP_1 == 0 invokes EverGrandeCity_EventScript_SetVisitedEverGrande. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `EverGrandeCity:bg_events:001` | 19,43 | [EverGrandeCity_EventScript_VictoryRoadSign](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L9) — EverGrandeCity_EventScript_VictoryRoadSign at (19,43); ENTERING VICTORY ROAD | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `EverGrandeCity:bg_events:002` | 29,48 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (29,48); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `EverGrandeCity:bg_events:003` | 18,52 | [EverGrandeCity_EventScript_CitySign](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L13) — EverGrandeCity_EventScript_CitySign at (18,52); EVER GRANDE CITY //  “The paradise of flowers, the sea, /  and POKéMON.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `EverGrandeCity:bg_events:004` | 23,15 | [EverGrandeCity_EventScript_PokemonLeagueSign](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L17) — EverGrandeCity_EventScript_PokemonLeagueSign at (23,15); ENTERING POKéMON LEAGUE /  CENTER GATE | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `EverGrandeCity:bg_events:005` | 28,48 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (28,48); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `EverGrandeCity:warp_events:001` | 18,5 | Warp from (18,5, elevation 0) to MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity:warp_events:002` | 27,48 | Warp from (27,48, elevation 0) to MAP_EVER_GRANDE_CITY_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity:warp_events:003` | 18,41 | Warp from (18,41, elevation 0) to MAP_VICTORY_ROAD_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity:warp_events:004` | 18,27 | Warp from (18,27, elevation 0) to MAP_VICTORY_ROAD_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity:connections:001` | left | left connection to MAP_ROUTE128, offset 40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EverGrandeCity:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [EverGrandeCity_OnTransition](../../baseline/source/data/maps/EverGrandeCity/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls EverGrandeCity_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
