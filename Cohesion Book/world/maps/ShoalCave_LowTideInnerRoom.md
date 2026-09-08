# ShoalCave_LowTideInnerRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/map.json) · [Scripts](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc)

## Current map contract

`MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM` · `LAYOUT_SHOAL_CAVE_LOW_TIDE_INNER_ROOM` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ShoalCave_LowTideInnerRoom:object_events:001` | 26,14 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_SHOAL_CAVE_INNER_ROOM_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ShoalCave_LowTideInnerRoom:bg_events:001` | 31,8 | [ShoalCave_LowTideInnerRoom_EventScript_ShoalSalt1](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L111) — ShoalCave_LowTideInnerRoom_EventScript_ShoalSalt1 at (31,8); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideInnerRoom:bg_events:002` | 14,26 | [ShoalCave_LowTideInnerRoom_EventScript_ShoalSalt2](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L127) — ShoalCave_LowTideInnerRoom_EventScript_ShoalSalt2 at (14,26); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideInnerRoom:bg_events:003` | 41,20 | [ShoalCave_LowTideInnerRoom_EventScript_ShoalShell1](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L62) — ShoalCave_LowTideInnerRoom_EventScript_ShoalShell1 at (41,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideInnerRoom:bg_events:004` | 41,10 | [ShoalCave_LowTideInnerRoom_EventScript_ShoalShell2](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L78) — ShoalCave_LowTideInnerRoom_EventScript_ShoalShell2 at (41,10); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideInnerRoom:bg_events:005` | 6,9 | [ShoalCave_LowTideInnerRoom_EventScript_ShoalShell3](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L89) — ShoalCave_LowTideInnerRoom_EventScript_ShoalShell3 at (6,9); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideInnerRoom:bg_events:006` | 16,13 | [ShoalCave_LowTideInnerRoom_EventScript_ShoalShell4](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L100) — ShoalCave_LowTideInnerRoom_EventScript_ShoalShell4 at (16,13); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `ShoalCave_LowTideInnerRoom:warp_events:001` | 34,29 | Warp from (34,29, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:002` | 38,15 | Warp from (38,15, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:003` | 42,4 | Warp from (42,4, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:004` | 19,14 | Warp from (19,14, elevation 4) to MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:005` | 15,19 | Warp from (15,19, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:006` | 30,25 | Warp from (30,25, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:007` | 14,33 | Warp from (14,33, elevation 5) to MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:warp_events:008` | 40,33 | Warp from (40,33, elevation 5) to MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `ShoalCave_LowTideInnerRoom:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [ShoalCave_LowTideInnerRoom_OnLoad](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L18) — MAP_SCRIPT_ON_LOAD calls ShoalCave_LowTideInnerRoom_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `ShoalCave_LowTideInnerRoom:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [ShoalCave_LowTideInnerRoom_OnTransition](../../baseline/source/data/maps/ShoalCave_LowTideInnerRoom/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls ShoalCave_LowTideInnerRoom_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
