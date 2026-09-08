# GraniteCave_B1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/GraniteCave_B1F/map.json) · [Scripts](../../baseline/source/data/maps/GraniteCave_B1F/scripts.inc)

## Current map contract

`MAP_GRANITE_CAVE_B1F` · `LAYOUT_GRANITE_CAVE_B1F` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `GraniteCave_B1F:object_events:001` | 15,21 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ALAKAZITE; root Common_EventScript_FindItem; flag FLAG_ITEM_GRANITE_CAVE_B1F_ALAKAZITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `GraniteCave_B1F:object_events:002` | 29,6 | [GraniteCave_B1F_EventScript_DuskBallPack](../../baseline/source/data/maps/GraniteCave_B1F/scripts.inc#L15) — Pickup ITEM_DUSK_BALL; root GraniteCave_B1F_EventScript_DuskBallPack; flag FLAG_EC_ITEM_GRANITE_CAVE_B1F_DUSK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `GraniteCave_B1F:warp_events:001` | 25,13 | Warp from (25,13, elevation 3) to MAP_GRANITE_CAVE_1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:warp_events:002` | 4,21 | Warp from (4,21, elevation 3) to MAP_GRANITE_CAVE_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:warp_events:003` | 29,13 | Warp from (29,13, elevation 3) to MAP_GRANITE_CAVE_B2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:warp_events:004` | 28,21 | Warp from (28,21, elevation 3) to MAP_GRANITE_CAVE_B2F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:warp_events:005` | 8,5 | Warp from (8,5, elevation 3) to MAP_GRANITE_CAVE_B2F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:warp_events:006` | 12,3 | Warp from (12,3, elevation 3) to MAP_GRANITE_CAVE_B2F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:warp_events:007` | 29,2 | Warp from (29,2, elevation 3) to MAP_GRANITE_CAVE_B2F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B1F:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [CaveHole_CheckFallDownHole](../../baseline/source/data/scripts/cave_hole.inc#L1) — MAP_SCRIPT_ON_FRAME_TABLE calls CaveHole_CheckFallDownHole. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `GraniteCave_B1F:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [CaveHole_FixCrackedGround](../../baseline/source/data/scripts/cave_hole.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls CaveHole_FixCrackedGround. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `GraniteCave_B1F:map_scripts:003` | MAP_SCRIPT_ON_RESUME | [GraniteCave_B1F_SetHoleWarp](../../baseline/source/data/maps/GraniteCave_B1F/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls GraniteCave_B1F_SetHoleWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
