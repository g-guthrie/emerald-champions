# SandstrewnRuins

**KEEP.** Preserve the fossil archive, branching stair network and route from both Mirage Tower basement and Desert Underpass. The tower may disappear; the underpass connection must keep the ruin’s acquisition content available.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/SandstrewnRuins/map.json) · [Scripts](../../baseline/source/data/maps/SandstrewnRuins/scripts.inc)

## Current map contract

`MAP_SANDSTREWN_RUINS` · `LAYOUT_SANDSTREWN_RUINS` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SandstrewnRuins:object_events:001` | 5,56 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GARCHOMPITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_GARCHOMPITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:002` | 12,113 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GOLURKITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_GOLURKITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:003` | 14,18 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SAIL_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_SAIL_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:004` | 2,29 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_FLYGONITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_FLYGONITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:005` | 14,43 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ARMOR_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_ARMOR_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:006` | 3,58 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_PLUME_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_PLUME_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:007` | 10,71 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SKULL_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_SKULL_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:008` | 2,85 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_COVER_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_COVER_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:009` | 7,102 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HELIX_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_HELIX_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:010` | 3,118 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DOME_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_DOME_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:011` | 4,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_JAW_FOSSIL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_JAW_FOSSIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:012` | 4,3 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (4,3); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:013` | 5,2 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (5,2); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:014` | 13,18 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (13,18); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:015` | 8,32 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (8,32); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:016` | 7,31 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (7,31); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:017` | 7,103 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (7,103); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:018` | 3,127 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (3,127); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:019` | 11,126 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (11,126); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:object_events:020` | 3,14 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ODD_KEYSTONE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_RUINS_ODD_KEYSTONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SandstrewnRuins:object_events:021` | 8,30 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (8,30); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `SandstrewnRuins:bg_events:001` | 8,31 | Hidden ITEM_PROTECTOR at (8,31); persistent flag FLAG_EC_HIDDEN_ITEM_SANDSTREWN_PROTECTOR. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `SandstrewnRuins:warp_events:001` | 8,132 | Warp from (8,132, elevation 3) to MAP_MIRAGE_TOWER_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:002` | 4,125 | Warp from (4,125, elevation 3) to MAP_SANDSTREWN_RUINS warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:003` | 12,125 | Warp from (12,125, elevation 3) to MAP_SANDSTREWN_RUINS warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:004` | 4,111 | Warp from (4,111, elevation 3) to MAP_SANDSTREWN_RUINS warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:005` | 12,118 | Warp from (12,118, elevation 3) to MAP_SANDSTREWN_RUINS warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:006` | 12,111 | Warp from (12,111, elevation 3) to MAP_SANDSTREWN_RUINS warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:007` | 4,104 | Warp from (4,104, elevation 3) to MAP_SANDSTREWN_RUINS warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:008` | 4,76 | Warp from (4,76, elevation 3) to MAP_SANDSTREWN_RUINS warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:009` | 12,90 | Warp from (12,90, elevation 3) to MAP_SANDSTREWN_RUINS warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:010` | 12,76 | Warp from (12,76, elevation 3) to MAP_SANDSTREWN_RUINS warp 10. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:011` | 4,97 | Warp from (4,97, elevation 3) to MAP_SANDSTREWN_RUINS warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:012` | 4,83 | Warp from (4,83, elevation 3) to MAP_SANDSTREWN_RUINS warp 12. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:013` | 12,62 | Warp from (12,62, elevation 3) to MAP_SANDSTREWN_RUINS warp 11. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:014` | 12,83 | Warp from (12,83, elevation 3) to MAP_SANDSTREWN_RUINS warp 14. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:015` | 4,48 | Warp from (4,48, elevation 3) to MAP_SANDSTREWN_RUINS warp 13. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:016` | 4,69 | Warp from (4,69, elevation 3) to MAP_SANDSTREWN_RUINS warp 16. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:017` | 12,48 | Warp from (12,48, elevation 3) to MAP_SANDSTREWN_RUINS warp 15. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:018` | 12,69 | Warp from (12,69, elevation 3) to MAP_SANDSTREWN_RUINS warp 18. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:019` | 4,34 | Warp from (4,34, elevation 3) to MAP_SANDSTREWN_RUINS warp 17. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:020` | 12,27 | Warp from (12,27, elevation 3) to MAP_SANDSTREWN_RUINS warp 20. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:021` | 4,7 | Warp from (4,7, elevation 3) to MAP_SANDSTREWN_RUINS warp 19. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:022` | 8,41 | Warp from (8,41, elevation 3) to MAP_SANDSTREWN_RUINS warp 22. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:023` | 8,20 | Warp from (8,20, elevation 3) to MAP_SANDSTREWN_RUINS warp 21. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:024` | 13,2 | Warp from (13,2, elevation 3) to MAP_SANDSTREWN_RUINS_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:warp_events:025` | 4,13 | Warp from (4,13, elevation 0) to MAP_DESERT_UNDERPASS warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SandstrewnRuins:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SandstrewnRuins_OnTransition](../../baseline/source/data/maps/SandstrewnRuins/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls SandstrewnRuins_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
