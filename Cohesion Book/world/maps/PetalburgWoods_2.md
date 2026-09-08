# PetalburgWoods_2

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/PetalburgWoods_2/map.json) · [Scripts](../../baseline/source/data/maps/PetalburgWoods_2/scripts.inc)

## Current map contract

`MAP_PETALBURG_WOODS_2` · `LAYOUT_PETALBURG_WOODS_2` · `WEATHER_SHADE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `PetalburgWoods_2:object_events:001` | 5,5 | [PetalburgWoods_2_EventScript_QuickBallPack](../../baseline/source/data/maps/PetalburgWoods_2/scripts.inc#L13) — Pickup ITEM_QUICK_BALL; root PetalburgWoods_2_EventScript_QuickBallPack; flag FLAG_EC_ITEM_PETALBURG_WOODS_2_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods_2:object_events:002` | 36,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SUN_STONE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_WOODS2_SUN_STONE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `PetalburgWoods_2:object_events:003` | 4,12 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (4,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_2:object_events:004` | 25,6 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (25,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_2:object_events:005` | 26,6 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (26,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_2:object_events:006` | 45,19 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (45,19); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `PetalburgWoods_2:object_events:007` | 2,31 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (2,31); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `PetalburgWoods_2:warp_events:001` | 4,34 | Warp from (4,34, elevation 0) to MAP_PETALBURG_WOODS warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_2:warp_events:002` | 5,34 | Warp from (5,34, elevation 0) to MAP_PETALBURG_WOODS warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_2:warp_events:003` | 42,34 | Warp from (42,34, elevation 0) to MAP_PETALBURG_WOODS_3 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `PetalburgWoods_2:warp_events:004` | 43,34 | Warp from (43,34, elevation 0) to MAP_PETALBURG_WOODS_3 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
