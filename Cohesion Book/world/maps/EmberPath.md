# EmberPath

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/EmberPath/map.json) · [Scripts](../../baseline/source/data/maps/EmberPath/scripts.inc)

## Current map contract

`MAP_EMBER_PATH` · `LAYOUT_EMBER_PATH` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `EmberPath:object_events:001` | 12,10 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAGMARIZER; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_EMBER_MAGMARIZER. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `EmberPath:object_events:002` | 34,4 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (34,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `EmberPath:object_events:003` | 36,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_QUICK_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_EMBER_QUICK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `EmberPath:object_events:004` | 13,36 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (13,36); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `EmberPath:object_events:005` | 14,32 | [EventScript_StrengthBoulder](../../baseline/source/data/scripts/field_move_scripts.inc#L226) — EventScript_StrengthBoulder at (14,32); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `EmberPath:object_events:006` | 21,14 | [EmberPath_EventScript_Moltres](../../baseline/source/data/maps/EmberPath/scripts.inc#L18) — EmberPath_EventScript_Moltres at (21,14); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `EmberPath:bg_events:001` | 9,38 | Hidden ITEM_MAGMARIZER at (9,38); persistent flag FLAG_EC_HIDDEN_ITEM_EMBER_PATH_MAGMARIZER. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `EmberPath:warp_events:001` | 33,38 | Warp from (33,38, elevation 0) to MAP_JAGGED_PASS warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EmberPath:warp_events:002` | 3,17 | Warp from (3,17, elevation 0) to MAP_ASHEN_WOODS warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `EmberPath:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [EmberPath_OnTransition](../../baseline/source/data/maps/EmberPath/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls EmberPath_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
