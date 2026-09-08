# PetalburgWoods_3

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgWoods_3/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgWoods_3/scripts.inc)

## Current map contract

`MAP_PETALBURG_WOODS_3` · `LAYOUT_PETALBURG_WOODS_3` · `WEATHER_SHADE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgWoods_3:object_events:001` | 22,26 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SWEET_APPLE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_WOODS3_SWEET_APPLE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods_3:object_events:002` | 22,16 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_TART_APPLE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_WOODS3_TART_APPLE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods_3:object_events:003` | 32,22 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (32,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_3:object_events:004` | 32,23 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (32,23); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_3:object_events:005` | 26,17 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (26,17); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_3:object_events:006` | 26,16 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (26,16); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_3:object_events:007` | 38,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BEEDRILLITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_WOODS3_BEEDRILLITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods_3:object_events:008` | 4,23 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (4,23); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `PetalburgWoods_3:warp_events:001` | 8,7 | Warp from (8,7, elevation 0) to MAP_PETALBURG_WOODS_2 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:002` | 9,7 | Warp from (9,7, elevation 0) to MAP_PETALBURG_WOODS_2 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:003` | 7,21 | Warp from (7,21, elevation 0) to MAP_PETALBURG_WOODS_3 warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:004` | 22,7 | Warp from (22,7, elevation 0) to MAP_PETALBURG_WOODS_3 warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:005` | 7,39 | Warp from (7,39, elevation 0) to MAP_PETALBURG_WOODS_3 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:006` | 10,39 | Warp from (10,39, elevation 0) to MAP_PETALBURG_WOODS_3 warp 9. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:007` | 36,35 | Warp from (36,35, elevation 0) to MAP_PETALBURG_WOODS_3 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:008` | 39,35 | Warp from (39,35, elevation 0) to MAP_PETALBURG_WOODS_3 warp 8. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:009` | 41,6 | Warp from (41,6, elevation 0) to MAP_PETALBURG_WOODS_3 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_3:warp_events:010` | 22,24 | Warp from (22,24, elevation 0) to MAP_PETALBURG_WOODS_3 warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
