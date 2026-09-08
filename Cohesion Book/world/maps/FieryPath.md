# FieryPath

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/FieryPath/map.json) · [Scripts](../../baseline/source/data/maps/FieryPath/scripts.inc)

## Current map contract

`MAP_FIERY_PATH` · `LAYOUT_FIERY_PATH` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `FieryPath:object_events:001` | 8,3 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HOUNDOOMINITE; root Common_EventScript_FindItem; flag FLAG_ITEM_FIERY_PATH_HOUNDOOMINITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `FieryPath:object_events:002` | 10,15 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (10,15); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `FieryPath:object_events:003` | 17,15 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (17,15); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `FieryPath:object_events:004` | 8,11 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (8,11); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `FieryPath:object_events:005` | 3,12 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (3,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `FieryPath:object_events:006` | 6,23 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (6,23); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `FieryPath:object_events:007` | 5,24 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (5,24); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `FieryPath:object_events:008` | 7,32 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_CAMERUPTITE; root Common_EventScript_FindItem; flag FLAG_ITEM_FIERY_PATH_CAMERUPTITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `FieryPath:object_events:009` | 25,32 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (25,32); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `FieryPath:warp_events:001` | 26,36 | Warp from (26,36, elevation 3) to MAP_ROUTE112 warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FieryPath:warp_events:002` | 26,4 | Warp from (26,4, elevation 3) to MAP_ROUTE112 warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `FieryPath:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [FieryPath_OnTransition](../../baseline/source/data/maps/FieryPath/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls FieryPath_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
