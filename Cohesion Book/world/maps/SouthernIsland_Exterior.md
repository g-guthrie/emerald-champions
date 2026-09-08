# SouthernIsland_Exterior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/SouthernIsland_Exterior/map.json) · [Scripts](../../baseline/source/data/maps/SouthernIsland_Exterior/scripts.inc)

## Current map contract

`MAP_SOUTHERN_ISLAND_EXTERIOR` · `LAYOUT_SOUTHERN_ISLAND_EXTERIOR` · `WEATHER_NONE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SouthernIsland_Exterior:object_events:001` | 13,23 | [SouthernIsland_Exterior_EventScript_Sailor](../../baseline/source/data/maps/SouthernIsland_Exterior/scripts.inc#L9) — SouthernIsland_Exterior_EventScript_Sailor at (13,23); shared behavior TRAVEL | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `SouthernIsland_Exterior:object_events:002` | 13,25 | Passive/staged OBJ_EVENT_GFX_SS_TIDAL at (13,25); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SouthernIsland_Exterior:bg_events:001` | 16,7 | [SouthernIsland_Exterior_EventScript_Sign](../../baseline/source/data/maps/SouthernIsland_Exterior/scripts.inc#L51) — SouthernIsland_Exterior_EventScript_Sign at (16,7); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SouthernIsland_Exterior:warp_events:001` | 14,5 | Warp from (14,5, elevation 3) to MAP_SOUTHERN_ISLAND_INTERIOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SouthernIsland_Exterior:warp_events:002` | 15,5 | Warp from (15,5, elevation 3) to MAP_SOUTHERN_ISLAND_INTERIOR warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SouthernIsland_Exterior:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SouthernIsland_Exterior_OnTransition](../../baseline/source/data/maps/SouthernIsland_Exterior/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls SouthernIsland_Exterior_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
