# GraniteCave_B2F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/GraniteCave_B2F/map.json) · [Scripts](../../baseline/source/data/maps/GraniteCave_B2F/scripts.inc)

## Current map contract

`MAP_GRANITE_CAVE_B2F` · `LAYOUT_GRANITE_CAVE_B2F` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `GraniteCave_B2F:object_events:001` | 4,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_REPEL; root Common_EventScript_FindItem; flag FLAG_ITEM_GRANITE_CAVE_B2F_REPEL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `GraniteCave_B2F:object_events:002` | 29,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_GLIMMORANITE; root Common_EventScript_FindItem; flag FLAG_ITEM_GRANITE_CAVE_B2F_GLIMMORANITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `GraniteCave_B2F:object_events:003` | 13,11 | [GraniteCave_B2F_EventScript_TimerBallPack](../../baseline/source/data/maps/GraniteCave_B2F/scripts.inc#L11) — Pickup ITEM_TIMER_BALL; root GraniteCave_B2F_EventScript_TimerBallPack; flag FLAG_EC_ITEM_GRANITE_CAVE_B2F_TIMER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `GraniteCave_B2F:object_events:004` | 5,14 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (5,14); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:005` | 3,14 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (3,14); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:006` | 2,16 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (2,16); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:007` | 7,12 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (7,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:008` | 4,22 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (4,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:009` | 6,22 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (6,22); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:010` | 3,21 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (3,21); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `GraniteCave_B2F:object_events:011` | 16,2 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (16,2); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `GraniteCave_B2F:bg_events:001` | 28,6 | Hidden ITEM_EVERSTONE at (28,6); persistent flag FLAG_HIDDEN_ITEM_GRANITE_CAVE_B2F_EVERSTONE_1. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `GraniteCave_B2F:bg_events:002` | 15,11 | Hidden ITEM_EVERSTONE at (15,11); persistent flag FLAG_HIDDEN_ITEM_GRANITE_CAVE_B2F_EVERSTONE_2. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `GraniteCave_B2F:warp_events:001` | 29,13 | Warp from (29,13, elevation 3) to MAP_GRANITE_CAVE_B1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B2F:warp_events:002` | 28,21 | Warp from (28,21, elevation 3) to MAP_GRANITE_CAVE_B1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B2F:warp_events:003` | 8,5 | Warp from (8,5, elevation 3) to MAP_GRANITE_CAVE_B1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B2F:warp_events:004` | 12,3 | Warp from (12,3, elevation 3) to MAP_GRANITE_CAVE_B1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B2F:warp_events:005` | 29,2 | Warp from (29,2, elevation 3) to MAP_GRANITE_CAVE_B1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_B2F:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [GraniteCave_B2F_SetFlashRadius](../../baseline/source/data/maps/GraniteCave_B2F/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls GraniteCave_B2F_SetFlashRadius. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
