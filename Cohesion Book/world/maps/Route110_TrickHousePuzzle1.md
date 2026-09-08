# Route110_TrickHousePuzzle1

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/map.json) · [Scripts](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/scripts.inc)

## Current map contract

`MAP_ROUTE110_TRICK_HOUSE_PUZZLE1` · `LAYOUT_ROUTE110_TRICK_HOUSE_PUZZLE1` · `WEATHER_NONE` · `MUS_TRICK_HOUSE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route110_TrickHousePuzzle1:object_events:001` | 14,20 | [Route110_TrickHousePuzzle1_EventScript_Sally](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/scripts.inc#L24) — Route110_TrickHousePuzzle1_EventScript_Sally at (14,20); I'll hack and slash my way to victory /  with the CUT we just learned! / Why are you so serious? | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle1:object_events:002` | 14,8 | [Route110_TrickHousePuzzle1_EventScript_Eddie](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/scripts.inc#L29) — Route110_TrickHousePuzzle1_EventScript_Eddie at (14,8); I wandered into this weird house /  by accident… / And now I've lost… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle1:object_events:003` | 2,15 | [Route110_TrickHousePuzzle1_EventScript_Robin](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/scripts.inc#L34) — Route110_TrickHousePuzzle1_EventScript_Robin at (2,15); Just who is the TRICK MASTER? / I lost while I was lost in thought! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route110_TrickHousePuzzle1:object_events:004` | 11,16 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (11,16); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:005` | 13,18 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (13,18); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:006` | 14,14 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (14,14); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:007` | 11,8 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (11,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:008` | 8,10 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (8,10); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:009` | 11,12 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (11,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:010` | 2,4 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (2,4); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:011` | 13,6 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (13,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:012` | 0,6 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (0,6); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:013` | 9,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ORANGE_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_TRICK_HOUSE_PUZZLE_1_ORANGE_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route110_TrickHousePuzzle1:object_events:014` | 4,8 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (4,8); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:object_events:015` | 2,12 | [EventScript_CutTree](../../baseline/source/data/scripts/field_move_scripts.inc#L2) — EventScript_CutTree at (2,12); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route110_TrickHousePuzzle1:bg_events:001` | 3,16 | [Route110_TrickHousePuzzle1_EventScript_Scroll](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/scripts.inc#L13) — Route110_TrickHousePuzzle1_EventScript_Scroll at (3,16); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route110_TrickHousePuzzle1:warp_events:001` | 0,21 | Warp from (0,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle1:warp_events:002` | 1,21 | Warp from (1,21, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_ENTRANCE warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle1:warp_events:003` | 13,1 | Warp from (13,1, elevation 3) to MAP_ROUTE110_TRICK_HOUSE_END warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route110_TrickHousePuzzle1:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [Route110_TrickHousePuzzle1_OnLoad](../../baseline/source/data/maps/Route110_TrickHousePuzzle1/scripts.inc#L5) — MAP_SCRIPT_ON_LOAD calls Route110_TrickHousePuzzle1_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
