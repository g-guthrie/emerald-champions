# PacifidlogTown

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/PacifidlogTown/map.json) · [Scripts](../../baseline/source/data/maps/PacifidlogTown/scripts.inc)

## Current map contract

`MAP_PACIFIDLOG_TOWN` · `LAYOUT_PACIFIDLOG_TOWN` · `WEATHER_SUNNY` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PacifidlogTown:object_events:001` | 10,23 | [PacifidlogTown_EventScript_Girl](../../baseline/source/data/maps/PacifidlogTown/scripts.inc#L18) — PacifidlogTown_EventScript_Girl at (10,23); The sea between PACIFIDLOG and /  SLATEPORT has a fast-running tide. //  If you decide to SURF, you could end /  up swept away somewhere else. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown:object_events:002` | 11,14 | [PacifidlogTown_EventScript_Fisherman](../../baseline/source/data/maps/PacifidlogTown/scripts.inc#L22) — PacifidlogTown_EventScript_Fisherman at (11,14); The SKY PILLAR? //  …Oh, you must mean that tall, tall /  tower a little further out. //  If you asked me, I wouldn't climb it. /  It's too scary to get up that high. //  Life at sea level in PACIFIDLOG, /  that suits me fine. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown:object_events:003` | 9,16 | [PacifidlogTown_EventScript_NinjaBoy](../../baseline/source/data/maps/PacifidlogTown/scripts.inc#L14) — PacifidlogTown_EventScript_NinjaBoy at (9,16); See, isn't it neat? /  These houses are on water! //  I was born here! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `PacifidlogTown:bg_events:001` | 9,15 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (9,15); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PacifidlogTown:bg_events:002` | 7,16 | [PacifidlogTown_EventScript_TownSign](../../baseline/source/data/maps/PacifidlogTown/scripts.inc#L26) — PacifidlogTown_EventScript_TownSign at (7,16); PACIFIDLOG TOWN //  “Where the morning sun smiles upon /  the waters.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PacifidlogTown:bg_events:003` | 10,15 | [Common_EventScript_ShowPokemonCenterSign](../../baseline/source/data/event_scripts.s#L1177) — Common_EventScript_ShowPokemonCenterSign at (10,15); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `PacifidlogTown:warp_events:001` | 8,15 | Warp from (8,15, elevation 0) to MAP_PACIFIDLOG_TOWN_POKEMON_CENTER_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:warp_events:002` | 16,13 | Warp from (16,13, elevation 0) to MAP_PACIFIDLOG_TOWN_HOUSE1 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:warp_events:003` | 3,22 | Warp from (3,22, elevation 0) to MAP_PACIFIDLOG_TOWN_HOUSE2 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:warp_events:004` | 12,24 | Warp from (12,24, elevation 0) to MAP_PACIFIDLOG_TOWN_HOUSE3 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:warp_events:005` | 2,12 | Warp from (2,12, elevation 0) to MAP_PACIFIDLOG_TOWN_HOUSE4 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:warp_events:006` | 17,21 | Warp from (17,21, elevation 0) to MAP_PACIFIDLOG_TOWN_HOUSE5 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:connections:001` | left | left connection to MAP_ROUTE132, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:connections:002` | right | right connection to MAP_ROUTE131, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PacifidlogTown:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [PacifidlogTown_OnTransition](../../baseline/source/data/maps/PacifidlogTown/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls PacifidlogTown_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `PacifidlogTown:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [PacifidlogTown_OnResume](../../baseline/source/data/maps/PacifidlogTown/scripts.inc#L10) — MAP_SCRIPT_ON_RESUME calls PacifidlogTown_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
