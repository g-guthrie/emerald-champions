# ShoalCave_LowTideStairsRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/ShoalCave_LowTideStairsRoom/map.json) · [Scripts](../../baseline/source/data/maps/ShoalCave_LowTideStairsRoom/scripts.inc)

## Current map contract

`MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM` · `LAYOUT_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ShoalCave_LowTideStairsRoom:object_events:001` | 13,12 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ICE_HEAL; root Common_EventScript_FindItem; flag FLAG_ITEM_SHOAL_CAVE_STAIRS_ROOM_ICE_HEAL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ShoalCave_LowTideStairsRoom:bg_events:001` | 11,11 | [ShoalCave_LowTideStairsRoom_EventScript_ShoalSalt3](../../baseline/source/data/maps/ShoalCave_LowTideStairsRoom/scripts.inc#L17) — ShoalCave_LowTideStairsRoom_EventScript_ShoalSalt3 at (11,11); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideStairsRoom:warp_events:001` | 3,12 | Warp from (3,12, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideStairsRoom:warp_events:002` | 7,4 | Warp from (7,4, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideStairsRoom:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [ShoalCave_LowTideStairsRoom_OnLoad](../../baseline/source/data/maps/ShoalCave_LowTideStairsRoom/scripts.inc#L5) — MAP_SCRIPT_ON_LOAD calls ShoalCave_LowTideStairsRoom_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
