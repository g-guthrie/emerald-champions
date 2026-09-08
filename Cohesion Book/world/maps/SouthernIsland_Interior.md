# SouthernIsland_Interior

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/SouthernIsland_Interior/map.json) · [Scripts](../../baseline/source/data/maps/SouthernIsland_Interior/scripts.inc)

## Current map contract

`MAP_SOUTHERN_ISLAND_INTERIOR` · `LAYOUT_SOUTHERN_ISLAND_INTERIOR` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SouthernIsland_Interior:object_events:001` | 13,12 | Passive/staged OBJ_EVENT_GFX_VAR_0 at (13,12); visibility flag FLAG_HIDE_SOUTHERN_ISLAND_EON_STONE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SouthernIsland_Interior:object_events:002` | 13,2 | Passive/staged OBJ_EVENT_GFX_VAR_1 at (13,2); visibility flag FLAG_HIDE_SOUTHERN_ISLAND_UNCHOSEN_EON_DUO_MON; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `SouthernIsland_Interior:object_events:003` | 8,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_LATIASITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_LATIASITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SouthernIsland_Interior:object_events:004` | 10,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_LATIOSITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_LATIOSITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SouthernIsland_Interior:bg_events:001` | 13,11 | [SouthernIsland_Interior_EventScript_TryLatiEncounter](../../baseline/source/data/maps/SouthernIsland_Interior/scripts.inc#L46) — SouthernIsland_Interior_EventScript_TryLatiEncounter at (13,11); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SouthernIsland_Interior:warp_events:001` | 13,18 | Warp from (13,18, elevation 3) to MAP_SOUTHERN_ISLAND_EXTERIOR warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SouthernIsland_Interior:warp_events:002` | 14,18 | Warp from (14,18, elevation 3) to MAP_SOUTHERN_ISLAND_EXTERIOR warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SouthernIsland_Interior:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [SouthernIsland_Interior_OnResume](../../baseline/source/data/maps/SouthernIsland_Interior/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls SouthernIsland_Interior_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SouthernIsland_Interior:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [SouthernIsland_Interior_OnTransition](../../baseline/source/data/maps/SouthernIsland_Interior/scripts.inc#L16) — MAP_SCRIPT_ON_TRANSITION calls SouthernIsland_Interior_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
