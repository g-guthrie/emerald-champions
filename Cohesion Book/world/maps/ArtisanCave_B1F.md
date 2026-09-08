# ArtisanCave_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/ArtisanCave_B1F/map.json) · [Scripts](../../baseline/source/data/maps/ArtisanCave_B1F/scripts.inc)

## Current map contract

`MAP_ARTISAN_CAVE_B1F` · `LAYOUT_ARTISAN_CAVE_B1F` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ArtisanCave_B1F:object_events:001` | 32,38 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_COMET_SHARD; root Common_EventScript_FindItem; flag FLAG_ITEM_ARTISAN_CAVE_B1F_COMET_SHARD. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ArtisanCave_B1F:bg_events:001` | 32,29 | Hidden ITEM_NUGGET at (32,29); persistent flag FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_NUGGET. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `ArtisanCave_B1F:bg_events:002` | 27,8 | Hidden ITEM_STAR_PIECE at (27,8); persistent flag FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_STAR_PIECE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `ArtisanCave_B1F:bg_events:003` | 7,5 | Hidden ITEM_BIG_PEARL at (7,5); persistent flag FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_BIG_PEARL. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `ArtisanCave_B1F:bg_events:004` | 19,43 | Hidden ITEM_PEARL_STRING at (19,43); persistent flag FLAG_HIDDEN_ITEM_ARTISAN_CAVE_B1F_PEARL_STRING. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `ArtisanCave_B1F:warp_events:001` | 8,48 | Warp from (8,48, elevation 0) to MAP_BATTLE_FRONTIER_OUTSIDE_WEST warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ArtisanCave_B1F:warp_events:002` | 38,5 | Warp from (38,5, elevation 0) to MAP_ARTISAN_CAVE_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ArtisanCave_B1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [ArtisanCave_B1F_OnTransition](../../baseline/source/data/maps/ArtisanCave_B1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls ArtisanCave_B1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
