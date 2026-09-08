# ShoalCave_LowTideIceRoom

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/ShoalCave_LowTideIceRoom/map.json) · [Scripts](../../baseline/source/data/maps/ShoalCave_LowTideIceRoom/scripts.inc)

## Current map contract

`MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM` · `LAYOUT_SHOAL_CAVE_LOW_TIDE_ICE_ROOM` · `WEATHER_NONE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `ShoalCave_LowTideIceRoom:object_events:001` | 12,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_RAZZ_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_SHOALCAVE_LOWTIDEICEROOM_RAZZ_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ShoalCave_LowTideIceRoom:object_events:002` | 12,21 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ICE_STONE; root Common_EventScript_FindItem; flag FLAG_ITEM_SHOAL_CAVE_ICE_ROOM_ICE_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ShoalCave_LowTideIceRoom:object_events:003` | 2,26 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_POMEG_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_SHOALCAVE_LOWTIDEICEROOM_POMEG_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `ShoalCave_LowTideIceRoom:object_events:004` | 8,8 | [ShoalCave_LowTideIceRoom_EventScript_Articuno](../../baseline/source/data/maps/ShoalCave_LowTideIceRoom/scripts.inc#L7) — ShoalCave_LowTideIceRoom_EventScript_Articuno at (8,8); A CHAMPION'S SIGN marks the ice altar. //  Its light is dormant. HOENN's story /  has not yet reached this place. / The CHAMPION'S SIGN erupts with light! /  ARTICUNO answers the challenge! | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `ShoalCave_LowTideIceRoom:object_events:005` | 12,10 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (12,10); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `ShoalCave_LowTideIceRoom:warp_events:001` | 17,10 | Warp from (17,10, elevation 3) to MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
