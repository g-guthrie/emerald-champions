# MirageTower_3F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/MirageTower_3F/map.json) · [Scripts](../../baseline/source/data/maps/MirageTower_3F/scripts.inc)

## Current map contract

`MAP_MIRAGE_TOWER_3F` · `LAYOUT_MIRAGE_TOWER_3F` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MirageTower_3F:object_events:001` | 3,7 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (3,7); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `MirageTower_3F:object_events:002` | 4,8 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (4,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `MirageTower_3F:warp_events:001` | 18,12 | Warp from (18,12, elevation 3) to MAP_MIRAGE_TOWER_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MirageTower_3F:warp_events:002` | 2,4 | Warp from (2,4, elevation 0) to MAP_MIRAGE_TOWER_4F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MirageTower_3F:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [CaveHole_CheckFallDownHole](../../baseline/source/data/scripts/cave_hole.inc#L1) — MAP_SCRIPT_ON_FRAME_TABLE calls CaveHole_CheckFallDownHole. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MirageTower_3F:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [CaveHole_FixCrackedGround](../../baseline/source/data/scripts/cave_hole.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls CaveHole_FixCrackedGround. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MirageTower_3F:map_scripts:003` | MAP_SCRIPT_ON_RESUME | [MirageTower_3F_SetHoleWarp](../../baseline/source/data/maps/MirageTower_3F/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls MirageTower_3F_SetHoleWarp. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
